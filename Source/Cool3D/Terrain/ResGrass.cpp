#include "StdAfx.h"
#include ".\ResGrass.h"
#include "..\Util\Exception.h"
#include "..\VFS\FileUtil.h"
#include "..\ResSys\VertStream.h"
#include "..\EditorSupport\TerrainEditor.h"
#include ".\Heightmap.h"
#include "..\Cool3DOption.h"
#include "..\RenderSys\StaticMeshIndexed.h"
#include "..\RenderSys\DynamicMeshIndexed.h"
#include "..\RenderMtl\MGeneral.h"
#include "..\RenderMtl\MMAlphaMask.h"
#include "..\RenderMtl\MMUsingMtlColor.h"
#include "..\RenderMtl\MConstColor.h"
#include "..\EditorSupport\EditorDecoFile.h"
#include "..\Image\Image.h"

namespace Cool3D
{
	IMP_RTTI(ResGrass,ResRenderable);


	//--class ResGrass::Member----------------------------------------------
	class ResGrass::Member
	{
	public:
		vector<StaticMeshIndexed*>		m_staticMeshes;
		vector<WORD>					m_indexes;
		vector<VertStream*>				m_vertStreams;
		AABBox							m_box;
		tagGrassHeader					m_header;

		Member()
		{}

		~Member()
		{
			UINT i;
			for (i=0;i<m_staticMeshes.size();i++)
			{
				StaticMeshIndexed* pSM=m_staticMeshes[i];
				SAFE_DELETE(pSM);
			}
			m_staticMeshes.clear();

		}

		void BuildRenderNode()
		{
			for(UINT i=0;i<m_vertStreams.size();i++)
			{
				VertStream* pVertStream=m_vertStreams[i];

				StaticMeshIndexed* pStaticMesh=new StaticMeshIndexed;
				pStaticMesh->CreateDirect(
					0,
					i,
					pVertStream,
					sizeof(WORD)*m_indexes.size(),
					&(*m_indexes.begin())
					);
				m_staticMeshes.push_back(pStaticMesh);

				SAFE_DELETE(pVertStream);
			}
			m_vertStreams.clear();
		}
	};

	//--class ResGrass------------------------------------------------------
	ResGrass::ResGrass(const TCHAR* szName):ResRenderable(szName)
	{
		m_p=new Member;
	}

	ResGrass::~ResGrass(void)
	{
		Destroy();
	}

	void ResGrass::Destroy()
	{
		SAFE_DELETE(m_p);
	}

	UINT ResGrass::CreateContent(DWORD param)
	{
		IFS *pFS=GetSafeFS();
		const TCHAR* szName=GetName();
		DWORD hFile=pFS->Open(szName);
		THROW_NULLEX(hFile,_T("Grass patch load faied"),szName);

		//--read header
		pFS->Read(&m_p->m_header,sizeof(m_p->m_header),hFile);

		//--load vertstream
		pFS->Seek(hFile,m_p->m_header.vertOffset,FILE_BEGIN);
		VertStream* pVertStream=new VertStream;
		pVertStream->CreateFromFile(pFS,hFile,m_p->m_header.vertType,m_p->m_header.numVerts);
		m_p->m_vertStreams.push_back(pVertStream);

		pVertStream->BuildAABBox(m_p->m_box);

		//--load index buffer
		FReadValVector(pFS,hFile,m_p->m_indexes);

		//--close file
		pFS->Close(hFile);

		m_p->BuildRenderNode();

		return ResRenderable::CreateContent(param);
	}

	UINT ResGrass::GetNumRenderNode() const
	{
		return m_p->m_staticMeshes.size();
	}

	RenderNode *ResGrass::GetRenderNode(UINT i) const
	{
		ASSERT(i<m_p->m_staticMeshes.size());
		return m_p->m_staticMeshes[i];
	}
	
	const AABBox *ResGrass::GetAABBox() const
	{ 
		return &m_p->m_box; 
	}

	int ResGrass::GetNumMtl() const
	{
		if(IsCreated())
			return 1;
		else
			return 0;
	}

	void ResGrass::CreateMtlArray(vector<IMaterial*>& mtls,const TCHAR* szLightMap,float mtlDiffuseFactor) const
	{
		if (IsCreated())
		{
			mtls.clear();
			MGeneral *pMtl=new MGeneral;

			MBitmap* pDiffuse=new MBitmap;
			pDiffuse->SetTexture(0,m_p->m_header.szTexPath);
			pDiffuse->SetMipFilter(ETFilter_LINEAR);
			pDiffuse->m_bTwoSide=true;
			pDiffuse->m_addressU=pDiffuse->m_addressV=ETAddress_Clamp;
			pDiffuse->m_bEnableLight=false;//关闭光照
			pMtl->m_pDiffuse=pDiffuse;

			MMAlphaMask *pAlphaMask=new MMAlphaMask;
			pAlphaMask->m_alphaTestFunc=ECMP_Greater;
			pAlphaMask->m_alphaRef=m_p->m_header.alphaRef;
			pMtl->AttachModifier(pAlphaMask);

			mtls.push_back(pMtl);
		}
	}

	struct tagGrassIndexes
	{
		vector<WORD> indexArray;
	};
	bool ResGrass::EditorSave(const TCHAR* szPath,const RECT& rect,TerrainEditor *pEditor)
	{
		/*struct tagGrassIndexes
		{
			vector<WORD> indexArray;
		};*/

		//--先选出在一个Patch上真的存在的layer
		vector<tagTDecoLayer>		decoLayers;
		vector<const VertStream*>	decoVertStreams;
		vector<tagGrassIndexes>		grassIndexes;
		vector<tagMtlDesc>			grassMtlDescs;
		int xStep=(rect.right-rect.left)/4;
		int zStep=(rect.bottom-rect.top)/4;
		const Heightmap *pHMap=pEditor->GetHeightmap();
		float tileW=pHMap->GetXScale();
		float tileH=pHMap->GetZScale();

		for(UINT i=0;i<TerrainEditor::MAX_DECO_LAYER;i++)
		{
			tagTDecoLayer *pLayer=pEditor->GetDecoLayer(i);
			if(pLayer==NULL
				|| pLayer->szName[0]==0)
				continue;

			EditorDecoFile *pDeco=pEditor->GetDecoFile(pLayer->szName);

			//--将每个地块的草分为16份
			for(int x=0;x<4;x++)
			{
				for(int z=0;z<4;z++)
				{
					RECT subRC;
					subRC.left=rect.left+x*xStep;
					subRC.top=rect.top+z*zStep;
					subRC.right=subRC.left+xStep;
					subRC.bottom=subRC.top+zStep;

					Vector3 tilePos;
					tilePos=pHMap->Tile2World(subRC.left,subRC.top);
					Vector2 leftTop=Vector2(tilePos.x,tilePos.z);
					Vector2 rightBottom=Vector2(tilePos.x+tileW*(subRC.right-subRC.left),tilePos.z+tileH*(subRC.bottom-subRC.top));
					if(pDeco->HaveDeco(leftTop,rightBottom))
					{
						VertStream *pDecoVS=new VertStream;
						tagGrassIndexes grassIndex;
						if (pEditor->GetDecoVBAndIB(pLayer->szName,subRC,pDecoVS,grassIndex.indexArray))
						{
							decoLayers.push_back(*pLayer);
							decoVertStreams.push_back(pDecoVS);
							grassIndexes.push_back(grassIndex);
						}
					}
				}
			}
		}

		TCHAR szDecoFile[255];
		for(UINT i=0;i<decoLayers.size();i++)
		{
			_stprintf(szDecoFile,_T("%s_%02d.deco"),szPath,i);
			FILE *fp=_tfopen(szDecoFile,_T("wb"));
			if (fp==NULL)
				return false;

			//--header
			tagGrassHeader header;
			memset(&header,0,sizeof(tagGrassHeader));
			_tcscpy((TCHAR*)header.magic,_T("GRASS"));
			header.alphaRef=decoLayers[i].alphaRef;
			_tcsncpy(header.szTexPath,decoLayers[i].szFileName,sizeof(header.szTexPath)/sizeof(TCHAR));
			fwrite(&header,sizeof(header),1,fp);

			//--vert stream
			header.vertOffset=ftell(fp);
			header.vertType=EVType_PTD;
			header.numVerts=decoVertStreams[i]->GetNumVert();
			
			fwrite(decoVertStreams[i]->GetRawBuffer(),decoVertStreams[i]->GetBufferSize(),1,fp);

			//--index buffer
			FWriteValVector(fp,grassIndexes[i].indexArray);

			//--回写header
			fseek(fp,0,SEEK_SET);
			fwrite(&header,sizeof(header),1,fp);

			fclose(fp);
		}

		return true;
	}

	bool ResGrass::ApplyLightMap( Image* pLightMap,TCHAR* szDecoFile,RECT& rcTPatch,TerrainEditor *pEditor )
	{
		IFS *pFS=Kernel::Inst()->DefaultFS();
		DWORD hFile=pFS->Open(szDecoFile);
		if(hFile==NULL)
			return false;

		//--read header
		tagGrassHeader header;
		pFS->Read(&header,sizeof(header),hFile);

		//--load vertstream
		VertStream vertStream;
		pFS->Seek(hFile,header.vertOffset,FILE_BEGIN);
		vertStream.CreateFromFile(pFS,hFile,header.vertType,header.numVerts);
		pFS->Close(hFile);

		//--apply lightmap
		const Heightmap* pHMap=pEditor->GetHeightmap();

		ASSERT(vertStream.GetType()==EVType_PTD);
		Vert_PTD* pVert=(Vert_PTD*)vertStream.GetRawBuffer();

		ASSERT(pLightMap->GetBpp()==32);
		tagBGRA* pPixels=(tagBGRA*)pLightMap->GetRawBuffer();

		for(UINT i=0;i<vertStream.GetNumVert();++i)
		{
			POINT tile=pHMap->World2Tile(pVert->pos);
			int x=int(float(tile.x-rcTPatch.left)/float(rcTPatch.right-rcTPatch.left)*(float)pLightMap->Width());
			int z=int(float(tile.y-rcTPatch.top)/float(rcTPatch.bottom-rcTPatch.top)*(float)pLightMap->Height());
			if(x<0)	x=0;
			if(z<0)	z=0;
			if(x>=(int)pLightMap->Width())	x=pLightMap->Width()-1;
			if(z>=(int)pLightMap->Height())	z=pLightMap->Height()-1;

			tagBGRA* pPixel=&pPixels[x+z*pLightMap->Width()];
			pVert->diffuse=Color4ub(255,pPixel->R,pPixel->G,pPixel->B);

			++pVert;
		}

		//--save vertstream
		FILE* fp=_tfopen(szDecoFile,_T("r+b"));
		if(fp==NULL)
			return false;

		fseek(fp,header.vertOffset,SEEK_SET);
		fwrite(vertStream.GetRawBuffer(),vertStream.GetBufferSize(),1,fp);
		fclose(fp);

		return true;
	}
}//namespace Cool3D