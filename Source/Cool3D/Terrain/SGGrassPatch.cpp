#include "StdAfx.h"
#include ".\sggrasspatch.h"
#include "..\RenderSys\StaticMeshIndexed.h"
#include "..\RenderSys\DynamicMeshIndexed.h"
#include "..\RenderMtl\MBitmap.h"
#include "..\RenderMtl\MMAlphaMask.h"
#include "..\RenderMtl\MGeneral.h"
#include "..\RenderMtl\MMUsingMtlColor.h"
#include "..\RenderMtl\MMTransparent.h"
#include ".\Heightmap.h"
#include "..\Math\Frustum.h"
#include "..\Math\Transform.h"
#include "..\SceneGraph\SceneGraph.h"
#include ".\TerrainPatch.h"
#include "..\Cool3DOption.h"
#include "..\RenderMtl\MConstColor.h"
#include "..\EditorSupport\EditorDecoFile.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(SGGrassPatch,SGTrnDecoPatch);

	class SGGrassPatch::Member
	{
	public:
		AABBox				m_box;
		vector<StaticMeshIndexed*>	m_staticMeshes;
		vector<DynamicMeshIndexed*> m_dynaMeshes;
		MGeneral			*m_mtl;
		
		int m_lod;
		tagTDecoLayer		layerDesc;
	public:
		Member()
		{
			m_mtl=NULL;
			m_lod=0;
		}
		~Member()
		{
			FreeAll();
		}

		void FreeMesh()
		{
			size_t i;
			for(i=0;i<m_staticMeshes.size();i++)
				SAFE_DELETE(m_staticMeshes[i]);
			m_staticMeshes.clear();

			for(i=0;i<m_dynaMeshes.size();i++)
				SAFE_DELETE(m_dynaMeshes[i]);
			m_dynaMeshes.clear();
		}

		void FreeAll()
		{
			FreeMesh();
			SAFE_DELETE(m_mtl);
		}

		bool BuildMesh(vector<Vert_PNT>& vertArray,vector<WORD>& indexArray,bool bDynaMesh,int mtlID,AABBox& box)
		{
			bool bBuild=false;

			if(vertArray.size()>0
				&& indexArray.size()>0)
			{
				bBuild=true;

				VertStream *pVertStream=new VertStream();
				pVertStream->Create(EVType_PNT,vertArray.size());
				memcpy(pVertStream->GetRawBuffer(),&(*vertArray.begin()),pVertStream->GetBufferSize());

				pVertStream->BuildAABBox(box);

				if(bDynaMesh)
				{
					DynamicMeshIndexed* pDynaMesh=new DynamicMeshIndexed;
					pDynaMesh->CreateDirect(EDraw_TriangleList,
						0,mtlID,vertArray.size(),sizeof(WORD)*indexArray.size(),
						&(*indexArray.begin()));
					pDynaMesh->AttachVertStream(pVertStream);
					m_dynaMeshes.push_back(pDynaMesh);
				}
				else
				{
					StaticMeshIndexed* pStaticMesh=new StaticMeshIndexed;
					pStaticMesh->CreateDirect(0,mtlID,pVertStream,sizeof(WORD)*indexArray.size(),
						&(*indexArray.begin()));
					m_staticMeshes.push_back(pStaticMesh);

					SAFE_DELETE(pVertStream);
				}
			}//endof if(have any face)

			return bBuild;
		}
	};

	SGGrassPatch::SGGrassPatch(void)
	{
		m_p=new Member();
	}

	SGGrassPatch::~SGGrassPatch(void)
	{
		delete m_p;
	}

	void SGGrassPatch::Serialize(FArchive& ar)
	{
		//do nothing
	}

	void SGGrassPatch::Deserialize(FArchive& ar)
	{}

	bool SGGrassPatch::IsInFrustum(const Frustum *pFrumstum) const
	{
		return pFrumstum->BoxInFrustum(m_p->m_box);
	}

	void SGGrassPatch::LoadRes(const TCHAR* szName)
	{}

	UINT SGGrassPatch::GetNumRenderNode() const
	{
		if(m_p->m_staticMeshes.size()>0)
			return m_p->m_staticMeshes.size();
		else
			return m_p->m_dynaMeshes.size();
	}

	RenderNode *SGGrassPatch::GetRenderNode(UINT i) const
	{
		if(m_p->m_staticMeshes.size()>0)
			return m_p->m_staticMeshes[i];
		else
			return m_p->m_dynaMeshes[i];
	}

	const Matrix4& SGGrassPatch::GetWorldMat() const
	{
		return MathConst::IdentityMat;
	}

	void SGGrassPatch::SetWorldMat(const Matrix4 *pMat)
	{
		ASSERT(0 && _T("can't set world matrix for grass patch"));
	}

	void SGGrassPatch::GetBox(AABBox& out) const
	{
		out=m_p->m_box;
	}

	IMaterial*	SGGrassPatch::GetMtl(int mtlID)
	{
		return m_p->m_mtl;
	}

	void SGGrassPatch::ForceUpdate(float deltaTime,SceneGraph *pSG)
	{
	}

	void SGGrassPatch::Update(float deltaTime,SceneGraph *pSG)
	{
		//--更新Mesh
		if(Cool3DOption::Inst()->GetOption(Cool3DOption::EO_GrassAni)==Cool3DOption::EGA_Open)//CPU动画
		{
			for(size_t j=0;j<m_p->m_dynaMeshes.size();j++)
			{
				DynamicMeshIndexed* pDynaMesh=m_p->m_dynaMeshes[j];

				VertStream *pVertStream=pDynaMesh->GetVertStream();
				Vert_PT *pVB=(Vert_PT *)pVertStream->GetRawBuffer();
				int numVert=pVertStream->GetNumVert()-1;
				
				float wave=m_p->layerDesc.wave;
				float speed=m_p->layerDesc.speed;
				if(fabs(wave)>FLOAT_EPSILON)
				{
					float r;
					for(int i=0;i<numVert;i+=2)
					{
						r=(Kernel::Inst()->GetAccumTime()+i)*speed;
						float s=sin(r);
						float c=cos(r);
						pVB[i].pos.x=pVB[i+1].pos.x+wave*s*c;
						pVB[i].pos.z=pVB[i+1].pos.z+wave*(s+c);
					}//endof for(i)
				}//endof if			
			}//endof for(j)
		}

		//--更新材质
		Color4f diffuse=pSG->GetSunLight().diffuse;
		diffuse=diffuse+Color4f(1,0.1f,0.1f,0.1f);//加上环境光
		diffuse.A=1.0f;
		if(diffuse.R>0.9f)diffuse.R=0.9f;
		if(diffuse.G>0.9f)diffuse.G=0.9f;
		if(diffuse.B>0.9f)diffuse.B=0.9f;
		if(m_p->m_mtl!=NULL
			&& m_p->m_mtl->m_pColor!=NULL)
		{
			m_p->m_mtl->m_pColor->m_color.diffuse=diffuse;
			m_p->m_mtl->UpdateModifiers(deltaTime);
		}
	}

	void SGGrassPatch::Build(Heightmap *pDensityMap,Heightmap *pTrnHMap,EditorDecoFile& decoFile,
		const tagTDecoLayer& layerDesc,RECT *pSubRect,bool bEditorMode,bool bShadowEffect)
	{
		ASSERT(pDensityMap!=NULL);

		//--释放现有数据,允许重复build
		m_p->FreeAll();

		if(layerDesc.szFileName[0]==0)
			return;

		//--复制数据
		m_p->layerDesc=layerDesc;
		RECT rc;
		if(pSubRect==NULL)
		{
			rc.left=rc.top=0;
			rc.right=pDensityMap->Width();
			rc.bottom=pDensityMap->Height();
		}
		else
			rc=*pSubRect;

		//--创建Mesh
		BuildMesh(pDensityMap,pTrnHMap,decoFile,layerDesc,&rc,bEditorMode,bShadowEffect);
		
		//--创建材质
		if(layerDesc.szFileName[0]!=0)
		{
			//普通材质
			{
				MGeneral *pMtl=new MGeneral;

				MConstColor* pColor=new MConstColor;
				pColor->m_color.diffuse=layerDesc.diffuse;
				pMtl->m_pColor=pColor;

				MBitmap* pDiffuse=new MBitmap;
				pDiffuse->SetTexture(0,layerDesc.szFileName);
				pDiffuse->SetMipFilter(ETFilter_LINEAR);
				pDiffuse->m_bTwoSide=true;
				pDiffuse->m_bEnableLight=false;
				pDiffuse->m_addressU=pDiffuse->m_addressV=ETAddress_Clamp;
				pMtl->m_pDiffuse=pDiffuse;

				MMAlphaMask *pAlphaMask=new MMAlphaMask;
				pAlphaMask->m_alphaTestFunc=ECMP_Greater;
				pAlphaMask->m_alphaRef=layerDesc.alphaRef;
				pMtl->AttachModifier(pAlphaMask);

				MMUsingMtlColor* pMMColor=new MMUsingMtlColor;//关闭灯光，使用材质中的diffuse颜色
				pMMColor->m_diffuse=layerDesc.diffuse;
				pMtl->AttachModifier(pMMColor);

				m_p->m_mtl=pMtl;
			}
		}
	}

	int SGGrassPatch::GetDecoType() const
	{
		return m_p->layerDesc.type;
	}

	void SGGrassPatch::CreateVertsAndIndexes(Heightmap *pDensityMap,const Heightmap *pTrnHMap,
		EditorDecoFile& decoFile,const tagTDecoLayer& layerDesc,RECT *pSubRect,
		vector<Vert_PNT>& vertArray,vector<WORD>& indexArray)
	{
		RECT rc=*pSubRect;

		//创建mesh
		float tileW=pTrnHMap->GetXScale();
		float tileH=pTrnHMap->GetZScale();
		for(int y=rc.top;y<rc.bottom;y++)
		{
			for(int x=rc.left;x<rc.right;x++)
			{
				int num=pDensityMap->GetValue(x,y);
				pDensityMap->SetValue(x,y,0);
				Vector3 tilePos;
				tilePos=pTrnHMap->Tile2World(x,y);
				Vector2 leftTop=Vector2(tilePos.x,tilePos.z);
				Vector2 rightBottom=Vector2(tilePos.x+tileW,tilePos.z+tileH);
				decoFile.AddGrass(leftTop,rightBottom,num,tilePos.y-15);
			}//end of for(x)
		}//end of for(y)

		Vector3 tilePos;
		tilePos=pTrnHMap->Tile2World(rc.left,rc.top);
		Vector2 leftTop=Vector2(tilePos.x,tilePos.z);
		Vector2 rightBottom=Vector2(tilePos.x+tileW*(rc.right-rc.left),tilePos.z+tileH*(rc.bottom-rc.top));
		vector<tagEditorDecoInfo> out;
		decoFile.GetGrass(leftTop,rightBottom,out);

		for(UINT i=0;i<out.size();i++)
		{
			tagEditorDecoInfo& decoInfo=out[i];

			Vector3 rotate=decoInfo.rotateRandom;
			rotate.x*=layerDesc.rotateRand.x;
			rotate.y*=layerDesc.rotateRand.y;
			rotate.z*=layerDesc.rotateRand.z;

			Vector3 pos;
			pos.x=out[i].pos.x;
			pos.z=out[i].pos.y;
			pos.y=out[i].pos.z;
			/*
			0-----------2-
			|			|
			|			|
			|			|
			|			|
			|			|
			1-----|-----3-
			pos
			*/
			Vector2 size=layerDesc.size;
			size.x+=decoInfo.sizeRandom.x*layerDesc.size.x*layerDesc.sizeRandRate;
			size.y+=decoInfo.sizeRandom.y*layerDesc.size.y*layerDesc.sizeRandRate;
			Vert_PNT vert[4];
			vert[0].pos=Vector3(-size.x*0.5f,size.y,0);
			vert[0].normal=Vector3(0.0f,1.0f,0.0f);
			vert[0].uv=Vector2(0,0);

			vert[1].pos=Vector3(-size.x*0.5f,0,0);
			vert[1].normal=Vector3(0.0f,1.0f,0.0f);
			vert[1].uv=Vector2(0,1);

			vert[3].pos=Vector3(size.x*0.5f,0,0);
			vert[3].normal=Vector3(0.0f,1.0f,0.0f);
			vert[3].uv=Vector2(1,1);

			vert[2].pos=Vector3(size.x*0.5f,size.y,0);
			vert[2].normal=Vector3(0.0f,1.0f,0.0f);
			vert[2].uv=Vector2(1,0);

			Transform posTrans;
			posTrans.Translate(pos);
			posTrans.Rotate(rotate.y,rotate.x,rotate.z);
			int j;
			int baseIndex=vertArray.size();

			for(j=0;j<4;j++)
			{
				posTrans.TransVec3(vert[j].pos);
				vertArray.push_back(vert[j]);
			}

			WORD quadIB[6]={0,2,3,3,1,0};
			for(j=0;j<6;j++)
				indexArray.push_back(quadIB[j]+baseIndex);

		}//end of for(i)
	}

	void SGGrassPatch::DelVertsAndIndexes( Heightmap *pDensityMap,const Heightmap *pTrnHMap, 
		EditorDecoFile& decoFile,const tagTDecoLayer& layerDesc,RECT *pSubRect, 
		vector<Vert_PNT>& vertArray,vector<WORD>& indexArray )
	{
		RECT rc=*pSubRect;

		//创建mesh
		float tileW=pTrnHMap->GetXScale();
		float tileH=pTrnHMap->GetZScale();
		for(int y=rc.top;y<rc.bottom;y++)
		{
			for(int x=rc.left;x<rc.right;x++)
			{
				int num=pDensityMap->GetValue(x,y);
				pDensityMap->SetValue(x,y,0);
				Vector3 tilePos;
				tilePos=pTrnHMap->Tile2World(x,y);
				Vector2 leftTop=Vector2(tilePos.x,tilePos.z);
				Vector2 rightBottom=Vector2(tilePos.x+tileW,tilePos.z+tileH);
				decoFile.DeleteGrass(leftTop,rightBottom,num);
			}//end of for(x)
		}//end of for(y)

		Vector3 tilePos;
		tilePos=pTrnHMap->Tile2World(rc.left,rc.top);
		Vector2 leftTop=Vector2(tilePos.x,tilePos.z);
		Vector2 rightBottom=Vector2(tilePos.x+tileW*(rc.right-rc.left),tilePos.z+tileH*(rc.bottom-rc.top));
		vector<tagEditorDecoInfo> out;
		decoFile.GetGrass(leftTop,rightBottom,out);

		for(UINT i=0;i<out.size();i++)
		{
			tagEditorDecoInfo& decoInfo=out[i];

			Vector3 rotate=decoInfo.rotateRandom;
			rotate.x*=layerDesc.rotateRand.x;
			rotate.y*=layerDesc.rotateRand.y;
			rotate.z*=layerDesc.rotateRand.z;

			Vector3 pos;
			pos.x=out[i].pos.x;
			pos.z=out[i].pos.y;
			pos.y=out[i].pos.z;
			/*
			0-----------2-
			|			|
			|			|
			|			|
			|			|
			|			|
			1-----|-----3-
			pos
			*/
			Vector2 size=layerDesc.size;
			size.x+=decoInfo.sizeRandom.x*layerDesc.size.x*layerDesc.sizeRandRate;
			size.y+=decoInfo.sizeRandom.y*layerDesc.size.y*layerDesc.sizeRandRate;
			Vert_PNT vert[4];
			vert[0].pos=Vector3(-size.x*0.5f,size.y,0);
			vert[0].normal=Vector3(0.0f,1.0f,0.0f);
			vert[0].uv=Vector2(0,0);

			vert[1].pos=Vector3(-size.x*0.5f,0,0);
			vert[1].normal=Vector3(0.0f,1.0f,0.0f);
			vert[1].uv=Vector2(0,1);

			vert[3].pos=Vector3(size.x*0.5f,0,0);
			vert[3].normal=Vector3(0.0f,1.0f,0.0f);
			vert[3].uv=Vector2(1,1);

			vert[2].pos=Vector3(size.x*0.5f,size.y,0);
			vert[2].normal=Vector3(0.0f,1.0f,0.0f);
			vert[2].uv=Vector2(1,0);

			Transform posTrans;
			posTrans.Translate(pos);
			posTrans.Rotate(rotate.y,rotate.x,rotate.z);
			int j;
			int baseIndex=vertArray.size();

			for(j=0;j<4;j++)
			{
				posTrans.TransVec3(vert[j].pos);
				vertArray.push_back(vert[j]);
			}

			WORD quadIB[6]={0,2,3,3,1,0};
			for(j=0;j<6;j++)
				indexArray.push_back(quadIB[j]+baseIndex);

		}
	}

	void SGGrassPatch::GetVertsAndIndexes( const Heightmap *pTrnHMap,EditorDecoFile& decoFile, 
		const tagTDecoLayer& layerDesc,RECT *pSubRect, vector<Vert_PNT>& vertArray,vector<WORD>& indexArray )
	{
		RECT rc=*pSubRect;

		//--创建mesh
		float tileW=pTrnHMap->GetXScale();
		float tileH=pTrnHMap->GetZScale();
		for(int y=rc.top;y<rc.bottom;y++)
		{
			for(int x=rc.left;x<rc.right;x++)
			{
				Vector3 tilePos;
				tilePos=pTrnHMap->Tile2World(x,y);
				Vector2 leftTop=Vector2(tilePos.x,tilePos.z);
				Vector2 rightBottom=Vector2(tilePos.x+tileW,tilePos.z+tileH);
				decoFile.ChangeGrassHeight(leftTop,rightBottom,tilePos.y-15);
			}
		}

		Vector3 tilePos;
		tilePos=pTrnHMap->Tile2World(rc.left,rc.top);
		Vector2 leftTop=Vector2(tilePos.x,tilePos.z);
		Vector2 rightBottom=Vector2(tilePos.x+tileW*(rc.right-rc.left),tilePos.z+tileH*(rc.bottom-rc.top));
		vector<tagEditorDecoInfo> out;
		decoFile.GetGrass(leftTop,rightBottom,out);

		for(UINT i=0;i<out.size();i++)
		{
			tagEditorDecoInfo& decoInfo=out[i];

			Vector3 rotate=decoInfo.rotateRandom;
			rotate.x*=layerDesc.rotateRand.x;
			rotate.y*=layerDesc.rotateRand.y;
			rotate.z*=layerDesc.rotateRand.z;

			Vector3 pos;
			pos.x=out[i].pos.x;
			pos.z=out[i].pos.y;
			pos.y=out[i].pos.z;
			/*
				0-----------2
				|			|
				|			|
				|			|
				|			|
				|			|
				1-----------3
			pos
			*/
			Vector2 size=layerDesc.size;
			size.x+=decoInfo.sizeRandom.x*layerDesc.size.x*layerDesc.sizeRandRate;
			size.y+=decoInfo.sizeRandom.y*layerDesc.size.y*layerDesc.sizeRandRate;
			Vert_PNT vert[4];
			vert[0].pos=Vector3(-size.x*0.5f,size.y,0);
			vert[0].normal=Vector3(0.0f,1.0f,0.0f);
			vert[0].uv=Vector2(0,0);

			vert[1].pos=Vector3(-size.x*0.5f,0,0);
			vert[1].normal=Vector3(0.0f,1.0f,0.0f);
			vert[1].uv=Vector2(0,1);

			vert[3].pos=Vector3(size.x*0.5f,0,0);
			vert[3].normal=Vector3(0.0f,1.0f,0.0f);
			vert[3].uv=Vector2(1,1);

			vert[2].pos=Vector3(size.x*0.5f,size.y,0);
			vert[2].normal=Vector3(0.0f,1.0f,0.0f);
			vert[2].uv=Vector2(1,0);

			Transform posTrans;
			posTrans.Translate(pos);
			posTrans.Rotate(rotate.y,rotate.x,rotate.z);
			int j;
			int baseIndex=vertArray.size();

			for(j=0;j<4;j++)
			{
				posTrans.TransVec3(vert[j].pos);
				vertArray.push_back(vert[j]);
			}

			WORD quadIB[6]={0,2,3,3,1,0};
			for(j=0;j<6;j++)
				indexArray.push_back(quadIB[j]+baseIndex);
		}
	}

	void SGGrassPatch::BuildMesh(Heightmap *pDensityMap,Heightmap *pTrnHMap,
		EditorDecoFile& decoFile,const tagTDecoLayer& layerDesc,
		RECT *pSubRect,bool bEditorMode,bool bShadowEffect)
	{
		m_p->FreeMesh();

		//--创建verts 和 indexes
		vector<Vert_PNT>	vertArray;
		vector<WORD>		indexArray;
		CreateVertsAndIndexes(pDensityMap,pTrnHMap,decoFile,layerDesc,pSubRect,vertArray,indexArray);

		bool bDynaMesh=false;
		if(bEditorMode
			|| Cool3DOption::Inst()->GetOption(Cool3DOption::EO_GrassAni)==Cool3DOption::EGA_Open)
			bDynaMesh=true;

		m_p->BuildMesh(vertArray,indexArray,bDynaMesh,0,m_p->m_box);
	}

	void SGGrassPatch::DelMesh( Heightmap *pDensityMap,Heightmap *pTrnHMap,EditorDecoFile& decoFile,
		const tagTDecoLayer& layerDesc, RECT *pSubRect,bool bEditorMode,bool bShadowEffect/*=false*/ )
	{
		m_p->FreeMesh();

		//删除verts 和 indexes
		vector<Vert_PNT>	vertArray;
		vector<WORD>		indexArray;
		DelVertsAndIndexes(pDensityMap,pTrnHMap,decoFile,layerDesc,pSubRect,vertArray,indexArray);

		bool bDynaMesh=false;
		if(bEditorMode
			|| Cool3DOption::Inst()->GetOption(Cool3DOption::EO_GrassAni)==Cool3DOption::EGA_Open)
			bDynaMesh=true;

		m_p->BuildMesh(vertArray,indexArray,bDynaMesh,0,m_p->m_box);
	}

	void SGGrassPatch::GetMesh( Heightmap *pTrnHMap,EditorDecoFile& decoFile,const tagTDecoLayer& layerDesc, 
		RECT *pSubRect,bool bEditorMode,bool bShadowEffect/*=false*/ )
	{
		m_p->FreeMesh();

		//获得verts 和 indexes
		vector<Vert_PNT>	vertArray;
		vector<WORD>		indexArray;
		GetVertsAndIndexes(pTrnHMap,decoFile,layerDesc,pSubRect,vertArray,indexArray);

		bool bDynaMesh=false;
		if(bEditorMode
			|| Cool3DOption::Inst()->GetOption(Cool3DOption::EO_GrassAni)==Cool3DOption::EGA_Open)
			bDynaMesh=true;

		m_p->BuildMesh(vertArray,indexArray,bDynaMesh,0,m_p->m_box);
	}

	void SGGrassPatch::FadeEffect(bool bFadeIn)
	{
		//todo
	}

	bool SGGrassPatch::GetVertStream(const Heightmap *pTrnHMap,EditorDecoFile& decoFile,
		const RECT *pSubRect,const tagTDecoLayer& layerDesc,
		VertStream * pVertStream, vector<WORD>& indexArray)
	{
		if(layerDesc.szFileName[0]==0)
			return false;

		RECT rc;
		if(pSubRect==NULL)
		{
			rc.left=rc.top=0;
			rc.right=pTrnHMap->Width();
			rc.bottom=pTrnHMap->Height();
		}
		else
			rc=*pSubRect;

		//--获得verts和indexes
		vector<Vert_PNT>	vertArray;
		GetVertsAndIndexes(pTrnHMap,decoFile,layerDesc,&rc,vertArray,indexArray);
		if(vertArray.size()==0)//随机后草的浓度可能变为0
			return false;

		pVertStream->Create(EVType_PTD,vertArray.size());
		Vert_PNT* pSrc=(Vert_PNT*)&vertArray[0];
		Vert_PTD* pDst=(Vert_PTD*)pVertStream->GetRawBuffer();
		for(size_t i=0;i<vertArray.size();i++)
		{
			pDst->pos=pSrc->pos;
			pDst->uv=pSrc->uv;
			pDst->diffuse=0xffffffff;
			pSrc++;
			pDst++;
		}

		return true;
	}
}//namespace Cool3D