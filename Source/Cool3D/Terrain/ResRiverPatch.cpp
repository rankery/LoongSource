#include "stdafx.h"
#include ".\ResRiverPatch.h"
#include ".\RRiverPatch.h"
#include "..\EditorSupport\TRiverEditor.h"
#include "..\ResSys\VertStream.h"
#include ".\SGRiverPatch.h"
#include ".\Heightmap.h"
#include ".\RRiverPatch.h"
#include "..\Util\Filename.h"
#include "..\Device\IDevice.h"
#include "..\Device\IVertBuffer.h"
#include "..\Kernel.h"
#include "..\RenderMtl\MBitmap.h"
#include "..\RenderMtl\MMFrameBlend.h"
#include "..\RenderMtl\MBitmapFrames.h"
#include "..\RenderMtl\MWater.h"
#include "..\RenderMtl\MWaterReflect.h"
#include "..\RenderMtl\MWaterRefract.h"

namespace Cool3D
{
	IMP_RTTI(ResRiverPatch,ResRenderable);

	class ResRiverPatch::Member
	{
	public:
		VertStream			*m_pVS;
		IVertBuffer			*m_pVB;
		RRiverPatch			*m_pRenderNode;
		AABBox				m_box;
		int					m_patchX;
		int					m_patchZ;

		//--MtlDesc
		tstring				m_szWaterTex;
		tstring				m_szBumpMap;
		int					m_waterFrm;
		float				m_waterFps;
		bool				m_bBlendTex;
		Color4f				m_blendColor;
		float				m_bumpScale;
		Color4f				m_diffuse;
		Color4f				m_ambient;
		Color4f				m_specular;
		float				m_power;
		float				m_reflectPower;
		Vector2             m_bumpUVScroll;
		float               m_reflectSpecularScale;
		tstring				m_szAlphaMap;

	public:
		Member()
			: m_pVB(0),m_pRenderNode(0)
		{
			m_pVS=new VertStream;
		}

		~Member()
		{
			SAFE_DELETE(m_pVB);
			SAFE_DELETE(m_pVS);
			SAFE_DELETE(m_pRenderNode);
		}

		void LoadFromFile(IFS *pFS,const TCHAR* szName)
		{
			DWORD hFile=pFS->Open(szName);
			THROW_NULLEX(hFile,_T("River patch load faied"),szName);

			//--read header
			tagHeader header;
			pFS->Read(&header,sizeof(tagHeader),hFile);
			m_box=header.box;
			m_patchX=header.patchX;
			m_patchZ=header.patchZ;

			//--MtlDesc
			m_szWaterTex=header.szWaterTex;
			m_szBumpMap=header.szBumpMap;
			m_waterFrm=header.waterFrm;
			m_waterFps=header.waterFps;
			m_blendColor=header.blendColor;
			m_bumpScale=header.bumpScale;
			m_diffuse=header.diffuse;
			m_ambient=header.ambient;
			m_specular=header.specular;
			m_power=header.power;
			m_reflectPower=header.reflectPower;
			m_bumpUVScroll=header.bumpUVScroll;
			m_reflectSpecularScale=header.reflectSpecularScale;

			//--new AlphaMap path
			Filename szPatchName=szName;
			Filename szAlphaMap=szPatchName.NoExtension();
			m_szAlphaMap=szAlphaMap+_T(".tga");

			//--load VS
			m_pVS->Create(EVType_PT,header.numVerts);
			pFS->Seek(hFile,header.vertsOffset,FILE_BEGIN);
			pFS->Read(m_pVS->GetRawBuffer(),m_pVS->GetBufferSize(),hFile);

			//--create VB
			m_pVB=Device()->NewVB();
			m_pVB->Create(m_pVS->GetBufferSize(),EB_StaticWriteOnly,m_pVS->GetStride());
			m_pVB->CopyFromVertStream(m_pVS);

			pFS->Close(hFile);
		}

		void CreateRenderNode()
		{
			m_pRenderNode=new RRiverPatch();
		}
	};

	ResRiverPatch::ResRiverPatch(const TCHAR *szName)
		:ResRenderable(szName)
	{
		m_p=new Member;
	}

	ResRiverPatch::~ResRiverPatch()
	{
		SAFE_DELETE(m_p);
	}

	UINT ResRiverPatch::CreateContent(DWORD param)
	{
		m_p->LoadFromFile(GetSafeFS(),GetName());
		m_p->CreateRenderNode();

		return ResRenderable::CreateContent(param);
	}

	UINT ResRiverPatch::GetNumRenderNode() const
	{
		if( !IsCreated() )
			return 0;
		else
			return 1;
	}

	RenderNode * ResRiverPatch::GetRenderNode( UINT i ) const
	{
		if( !IsCreated() )
			return NULL;

		ASSERT(i<1);
		return m_p->m_pRenderNode;
	}

	const AABBox * ResRiverPatch::GetAABBox() const
	{
		return &m_p->m_box;
	}

	int ResRiverPatch::GetNumMtl() const
	{
		if( !IsCreated() )
			return 0;
		else
			return 1;
	}

	void ResRiverPatch::FillVertStream( VertStream *pVS,Heightmap *pHMap,const RECT *pRect,
		float uTileSize,float vTileSize )
	{
		ASSERT(pVS!=NULL);
		ASSERT(uTileSize>1.0f && vTileSize>1.0f);

		RECT rect=*pRect;

		int stepX=(rect.right-rect.left)/(TRiverEditor::EPatchSideVerts-1);
		int stepZ=(rect.bottom-rect.top)/(TRiverEditor::EPatchSideVerts-1);

		Vert_PT *pVert=(Vert_PT*)pVS->GetRawBuffer();
		int sideVerts=TRiverEditor::EPatchSideVerts;

		for(int z=0;z<sideVerts;z++)
		{
			for(int x=0;x<sideVerts;x++)
			{
				int hx,hz;
				if(x==sideVerts-1)
					hx=rect.right-1;
				else
					hx=rect.left+x*stepX;	
				if(z==sideVerts-1)
					hz=rect.bottom-1;
				else
					hz=rect.top+z*stepZ;

				pHMap->GetVertPos(hx,hz,pVert->pos);

				pVert->uv.x=float(hx)/uTileSize;
				pVert->uv.y=float(hz)/vTileSize;

				//--
				pVert++;
			}
		}
	}

	bool ResRiverPatch::EditorSave( TRiverEditor *pEditor,UINT riverID,
		const TCHAR *szVertsPath,const TCHAR *szAlphaPath,const RECT *pRect )
	{
		ASSERT(pEditor!=NULL && pRect!=NULL);

		FILE *fp=_tfopen(szVertsPath,_T("wb"));
		if(fp==NULL)
			return false;

		Heightmap *pHMap=pEditor->GetRiverHeightMapByID(riverID);
		ASSERT(pHMap!=NULL);

		tagRiverMtlDesc *pMtlDesc=pEditor->GetRiverMtlDescByID(riverID);
		ASSERT(pMtlDesc!=NULL);

		tagHeader header;
		memset(&header,0,sizeof(tagHeader));
		_tcscpy((TCHAR*)header.magic,_T("RPATCH"));

		//--MtlDesc
		TCHAR szDir[256];
		TCHAR szFileName[256];
		_tsplitpath(pMtlDesc->szWaterTex,NULL,szDir,szFileName,NULL);
		if(_tcslen(szFileName)>2)
			szFileName[_tcslen(szFileName)-2]=0;
		_stprintf(header.szWaterTex,_T("%s%s"),szDir,szFileName);

		_tcscpy(header.szBumpMap, pMtlDesc->szBumpMap);

		header.waterFrm=pMtlDesc->waterFrm;
		header.waterFps=pMtlDesc->waterFps;
		header.blendColor=pMtlDesc->blendColor;
		header.bumpScale=pMtlDesc->bumpScale;
		header.diffuse=pMtlDesc->diffuse;
		header.ambient=pMtlDesc->ambient;
		header.specular=pMtlDesc->specular;
		header.power=pMtlDesc->power;
		header.reflectPower=pMtlDesc->reflectPower;
		header.bumpUVScroll=pMtlDesc->bumpUVScroll;
		header.reflectSpecularScale=pMtlDesc->reflectSpecularScale;

		//--AABBox
		pHMap->BuildAABBox(&header.box,pRect);

		//地块坐标
		header.patchX=pRect->left/TRiverEditor::EPatchSize;
		header.patchZ=pRect->top/TRiverEditor::EPatchSize;

		fwrite(&header,sizeof(header),1,fp);

		//--保存顶点
		VertStream vertStream;
		vertStream.Create(EVType_PT,TRiverEditor::EPatchVerts);
		FillVertStream(&vertStream,pHMap,pRect,pMtlDesc->uTileSize,pMtlDesc->vTileSize);
		header.numVerts=vertStream.GetNumVert();
		header.vertsOffset=ftell(fp);
		fwrite(vertStream.GetRawBuffer(),vertStream.GetBufferSize(),1,fp);

		//--回写Header
		fseek(fp,0,SEEK_SET);
		fwrite(&header,sizeof(header),1,fp);

		fclose(fp);

		return true;
	}

	IVertBuffer * ResRiverPatch::GetPosTexVB()
	{
		return m_p->m_pVB;
	}

	IMaterial * ResRiverPatch::CreateWaterMtl()
	{
		if(m_p->m_szWaterTex[0]!=0)
		{
			if(m_p->m_waterFrm!=1)	//处理贴图动画
			{
				MWater* pWaterMtl=new MWater;
				pWaterMtl->SetFrames(m_p->m_szWaterTex.c_str(),m_p->m_waterFrm,m_p->m_waterFps);
				pWaterMtl->SetAlphaMap(m_p->m_szAlphaMap.c_str());

				pWaterMtl->m_color.m_color.diffuse=m_p->m_diffuse;
				pWaterMtl->m_color.m_color.ambient=m_p->m_ambient;
				pWaterMtl->m_color.m_color.specular=m_p->m_specular;
				pWaterMtl->m_color.m_color.power=m_p->m_power;

				return pWaterMtl;
			}
			else
			{
				MBitmap* pWaterMtl=new MBitmap;
				pWaterMtl->SetTexture(0,m_p->m_szWaterTex.c_str(),0);

				return pWaterMtl;
			}
		}
		return NULL;
	}

	MWaterReflect * ResRiverPatch::CreateReflectMtl()
	{
		if(m_p->m_szWaterTex[0]!=0)
		{
			MWaterReflect *pReflectMtl=new MWaterReflect;

			//bump map
			MBitmap *pBumpMtl=new MBitmap;
			pBumpMtl->SetTexture( 0, m_p->m_szBumpMap.c_str() );
			pBumpMtl->m_addressU=pBumpMtl->m_addressV=pBumpMtl->m_addressW=ETAddress_Wrap;
			pReflectMtl->m_pBumpMap=pBumpMtl;

			//bump uv scroll
			pReflectMtl->m_bumpUVScroll=m_p->m_bumpUVScroll;

			//bump scale
			pReflectMtl->m_bumpScale=m_p->m_bumpScale;

			//blend color
			pReflectMtl->m_blendColor=m_p->m_blendColor;

			pReflectMtl->m_color.m_color.diffuse=m_p->m_diffuse;
			pReflectMtl->m_color.m_color.ambient=m_p->m_ambient;
			pReflectMtl->m_color.m_color.specular=m_p->m_specular;
			pReflectMtl->m_color.m_color.power=m_p->m_reflectPower;
			pReflectMtl->m_specularScale=m_p->m_reflectSpecularScale;

			pReflectMtl->SetAlphaMap(m_p->m_szAlphaMap.c_str());

			return pReflectMtl;
		}

		return NULL;
	}

	MWaterRefract * ResRiverPatch::CreateRefractMtl()
	{
		if(m_p->m_szWaterTex[0]!=0)
		{
			MWaterRefract *pRefractMtl=new MWaterRefract;

			//bump map
			MBitmap *pBumpMtl=new MBitmap;
			pBumpMtl->SetTexture( 0, m_p->m_szBumpMap.c_str() );
			pBumpMtl->m_addressU=pBumpMtl->m_addressV=pBumpMtl->m_addressW=ETAddress_Wrap;
			pRefractMtl->m_pBumpMap=pBumpMtl;

			//bump uv scroll
			pRefractMtl->m_bumpUVScroll=m_p->m_bumpUVScroll;

			//bump scale
			pRefractMtl->m_bumpScale=m_p->m_bumpScale;

			//blend color
			pRefractMtl->m_blendColor=m_p->m_blendColor;

			pRefractMtl->m_color.m_color.diffuse=m_p->m_diffuse;
			pRefractMtl->m_color.m_color.ambient=m_p->m_ambient;
			pRefractMtl->m_color.m_color.specular=m_p->m_specular;
			pRefractMtl->m_color.m_color.power=m_p->m_reflectPower;
			pRefractMtl->m_specularScale=m_p->m_reflectSpecularScale;

			pRefractMtl->SetAlphaMap(m_p->m_szAlphaMap.c_str());

			return pRefractMtl;
		}

		return NULL;
	}

	int ResRiverPatch::GetPatchX()
	{
		return m_p->m_patchX;
	}

	int ResRiverPatch::GetPatchZ()
	{
		return m_p->m_patchZ;
	}
}//namespace Cool3D