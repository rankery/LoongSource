#include "stdafx.h"
#include ".\ResRiver.h"
#include "..\Device\IDevice.h"
#include "..\Device\IVertBuffer.h"
#include "..\ResSys\VertStream.h"
#include "..\EditorSupport\TRiverEditor.h"
#include "..\Kernel.h"

namespace Cool3D
{
	IMP_RTTI(ResRiver,ResRenderable);

	class ResRiver::Member
	{
	public:
		VertStream			*m_pVS;
		IVertBuffer			*m_pVB;

		UINT				m_riverID;
		bool				m_bEnableEffect;
		float				m_height;

	public:
		Member()
		{
			m_pVS=new VertStream;
			m_pVB=NULL;
		}

		~Member()
		{
			SAFE_DELETE(m_pVB);
			SAFE_DELETE(m_pVS);
		}

		void LoadFromFile(IFS *pFS,const TCHAR *szName)
		{
			DWORD hFile=pFS->Open(szName);
			THROW_NULLEX(hFile,_T("River load faied"),szName);

			//--read header
			tagHeader header;
			pFS->Read(&header,sizeof(tagHeader),hFile);

			m_riverID=header.riverID;
			m_bEnableEffect=header.bEnableEffect;
			m_height=header.height;

			//--load VS
			m_pVS->Create(EVType_NT,header.numVerts);
			pFS->Seek(hFile,header.vertsOffset,FILE_BEGIN);
			pFS->Read(m_pVS->GetRawBuffer(),m_pVS->GetBufferSize(),hFile);

			//--create VB
			m_pVB=Device()->NewVB();
			m_pVB->Create(m_pVS->GetBufferSize(),EB_StaticWriteOnly,m_pVS->GetStride());
			m_pVB->CopyFromVertStream(m_pVS);

			pFS->Close(hFile);
		}
	};

	ResRiver::ResRiver( const TCHAR *szName )
		: ResRenderable(szName)
	{
		m_p=new Member;
	}

	ResRiver::~ResRiver()
	{
		SAFE_DELETE(m_p);
	}

	UINT ResRiver::CreateContent( DWORD param )
	{
		m_p->LoadFromFile(GetSafeFS(),GetName());

		return ResRenderable::CreateContent(param);
	}

	UINT ResRiver::GetNumRenderNode() const
	{
		return 0;
	}

	RenderNode * ResRiver::GetRenderNode( UINT i ) const
	{
		return NULL;
	}

	const AABBox * ResRiver::GetAABBox() const
	{
		return &MathConst::SmallBox;
	}

	int ResRiver::GetNumMtl() const
	{
		return 0;
	}

	void ResRiver::FillVertStream( VertStream *pVS,Heightmap *pHMap )
	{
		ASSERT(pVS!=NULL);

		RECT rect;
		rect.left=rect.top=0;
		rect.right=rect.bottom=TRiverEditor::EPatchAlphaMapSize+1;

		int stepX=(rect.right-rect.left)/(TRiverEditor::EPatchSideVerts-1);
		int stepZ=(rect.bottom-rect.top)/(TRiverEditor::EPatchSideVerts-1);

		Vert_NT *pVert=(Vert_NT*)pVS->GetRawBuffer();
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

				pVert->normal=Vector3(0.0f,1.0f,0.0f);

				pVert->uv.x=float(hx-rect.left)/TRiverEditor::EPatchAlphaMapSize;
				pVert->uv.y=float(hz-rect.top)/TRiverEditor::EPatchAlphaMapSize;

				//--
				pVert++;
			}
		}
	}

	bool ResRiver::EditorSave( TRiverEditor *pEditor,UINT riverID,const TCHAR *szBasePath )
	{
		ASSERT(pEditor!=NULL);

		TCHAR szFilePath[256];
		_stprintf(szFilePath,_T("%sRiver_%02d.river"),szBasePath,riverID);

		FILE *fp=_tfopen(szFilePath,_T("wb"));
		if(fp==NULL)
			return false;

		Heightmap *pHMap=pEditor->GetRiverHeightMapByID(riverID);
		ASSERT(pHMap!=NULL);

		tagRiverMtlDesc *pMtlDesc=pEditor->GetRiverMtlDescByID(riverID);
		ASSERT(pMtlDesc!=NULL);

		tagHeader header;
		memset(&header,0,sizeof(tagHeader));
		_tcscpy((TCHAR*)header.magic,_T("RIVER"));

		header.riverID=riverID;
		header.bEnableEffect=pMtlDesc->bEnableEffect;
		header.height=pEditor->GetRiverHeightByID(riverID)*1.0f;

		fwrite(&header,sizeof(header),1,fp);

		//--±£´æ¶¥µã
		VertStream vertStream;
		vertStream.Create(EVType_NT,TRiverEditor::EPatchVerts);
		FillVertStream(&vertStream,pHMap);
		header.numVerts=vertStream.GetNumVert();
		header.vertsOffset=ftell(fp);
		fwrite(vertStream.GetRawBuffer(),vertStream.GetBufferSize(),1,fp);

		//--»ØÐ´Header
		fseek(fp,0,SEEK_SET);
		fwrite(&header,sizeof(header),1,fp);

		fclose(fp);

		return true;
	}

	IVertBuffer * ResRiver::GetNormalAlphaVB()
	{
		return m_p->m_pVB;
	}

	bool ResRiver::IsEnableEffect()
	{
		return m_p->m_bEnableEffect;
	}

	float ResRiver::GetHeight()
	{
		return m_p->m_height;
	}

	UINT ResRiver::GetRiverID()
	{
		return m_p->m_riverID;
	}
}//namespace Cool3D