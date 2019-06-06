#include "StdAfx.h"
#include ".\vertstream.h"
#include "..\Util\Exception.h"
#include "..\VFS\IFS.h"
#include "..\Util\FMemory.h"

namespace Cool3D
{
	VertStream::VertStream(void)
	{
		m_type=EVType_None;
		m_numVert=0;
		m_pBuf=NULL;
	}

	VertStream::~VertStream(void)
	{
		Destroy();
	}

	void VertStream::CreateFromFile(IFS *pFS,DWORD hFile,EVertType vertType,UINT numVert)
	{
		Destroy();
		//--
		ASSERT(pFS!=NULL);
		ASSERT(hFile!=0);
		ASSERT(numVert > 0);

		m_type=vertType;
		m_numVert=numVert;
		UINT vertSize=GetVertSize(m_type);
		ASSERT(vertSize > 0);
		m_stride=vertSize;
		UINT bufSize=vertSize*numVert;
		m_pBuf=(BYTE*)malloc(bufSize);
		ASSERT(m_pBuf!=NULL);
		pFS->Read(m_pBuf,bufSize,hFile);
	}

	void VertStream::Create(EVertType vertType,UINT numVert)
	{
		Destroy();
		//--
		m_type=vertType;
		m_numVert=numVert;
		UINT vertSize=GetVertSize(m_type);
		ASSERT(vertSize > 0);
		m_stride=vertSize;
		UINT bufSize=vertSize*numVert;
		m_pBuf=(BYTE*)malloc(bufSize);
		ASSERT(m_pBuf!=NULL);
	}

	void VertStream::Create(UINT stride,UINT numVert)
	{
		Destroy();
		//--
		m_type=EVType_None;//!!!
		m_numVert=numVert;
		m_stride=stride;
		UINT bufSize=m_stride*numVert;
		m_pBuf=(BYTE*)malloc(bufSize);
		ASSERT(m_pBuf!=NULL);
	}

	VertStream *VertStream::Clone(bool bBufferCopy)
	{
		ASSERT(m_type != EVType_None);
		ASSERT(m_pBuf != NULL);

		VertStream *pRet=new VertStream;
		pRet->m_type=m_type;
		pRet->m_numVert=m_numVert;
		pRet->m_stride=m_stride;

		pRet->m_pBuf=(BYTE*)malloc(GetBufferSize());
		
		if(bBufferCopy)
			FMemcpy(pRet->m_pBuf,m_pBuf,GetBufferSize());

		return pRet;
	}

	bool VertStream::GetPos(UINT i,Vector3& out) const
	{
		ASSERT(i<m_numVert);
		
		switch(m_type)
		{
		case EVType_PNT:
			{
				Vert_PNT *pV=(Vert_PNT *)m_pBuf;
				pV+=i;
				out=pV->pos;
				break;
			}
		case EVType_PNT2:
			{
				Vert_PNT2 *pV=(Vert_PNT2 *)m_pBuf;
				pV+=i;
				out=pV->pos;
				break;
			}
		case EVType_PT2:
			{
				Vert_PT2 *pV=(Vert_PT2 *)m_pBuf;
				pV+=i;
				out=pV->pos;
				break;
			}
		case EVType_PD:
			{
				Vert_PD *pV=(Vert_PD *)m_pBuf;
				pV+=i;
				out=pV->pos;
				break;
			}
		case EVType_PTD:
			{
				Vert_PTD *pV=(Vert_PTD *)m_pBuf;
				pV+=i;
				out=pV->pos;
				break;
			}
		case EVType_PTp:
			{
				Vert_PTp *pV=(Vert_PTp *)m_pBuf;
				pV+=i;
				out=pV->pos;
				break;
			}
		case EVType_PT:
			{
				Vert_PT *pV=(Vert_PT *)m_pBuf;
				pV+=i;
				out=pV->pos;
				break;
			}
		case EVType_PN:
			{
				Vert_PN *pV=(Vert_PN *)m_pBuf;
				pV+=i;
				out=pV->pos;
				break;
			}
		case EVType_PNGT:
			{
				Vert_PNGT *pV=(Vert_PNGT *)m_pBuf;
				pV+=i;
				out=pV->pos;
				break;
			}
		default:
			return false;
		}

		return true;
	}

	bool VertStream::GetNormal(UINT i,Vector3& out) const
	{
		ASSERT(i<m_numVert);

		switch(m_type)
		{
		case EVType_PNT:
			{
				Vert_PNT *pV=(Vert_PNT *)m_pBuf;
				pV+=i;
				out=pV->normal;
			}
			break;
		case EVType_PNT2:
			{
				Vert_PNT2 *pV=(Vert_PNT2 *)m_pBuf;
				pV+=i;
				out=pV->normal;
			}
			break;
		case EVType_PNGT:
			{
				Vert_PNGT *pV=(Vert_PNGT *)m_pBuf;
				pV+=i;
				out=pV->normal;
			}
			break;
		case EVType_PN:
			{
				Vert_PN *pV=(Vert_PN *)m_pBuf;
				pV+=i;
				out=pV->normal;
			}
			break;
		default:
			return false;
		}

		return true;
	}

	bool VertStream::GetTangent(UINT i,Vector3& out) const
	{
		ASSERT(i<m_numVert);

		switch(m_type)
		{
		case EVType_PNGT:
			{
				Vert_PNGT *pV=(Vert_PNGT *)m_pBuf;
				pV+=i;
				out=pV->tangent;
				break;
			}
		default:
			return false;
		}

		return true;
	}

	void VertStream::BuildAABBox(AABBox& box)
	{
		ASSERT(m_pBuf!=NULL);
		box.max=Vector3(-FLOAT_MAX,-FLOAT_MAX,-FLOAT_MAX);
		box.min=Vector3(FLOAT_MAX,FLOAT_MAX,FLOAT_MAX);

		Vector3 pos;
		Vector3& max=box.max;
		Vector3& min=box.min;
		for(UINT i=0;i<m_numVert;i++)
		{
			GetPos(i,pos);
			if(pos.x > max.x)
				max.x=pos.x;
			if(pos.y > max.y)
				max.y=pos.y;
			if(pos.z > max.z)
				max.z=pos.z;

			if(pos.x < min.x)
				min.x=pos.x;
			if(pos.y < min.y)
				min.y=pos.y;
			if(pos.z < min.z)
				min.z=pos.z;
		}
	}
	
	void VertStream::Destroy()
	{
		SAFE_FREE(m_pBuf);
		m_type=EVType_None;
		m_numVert=0;
	}

	void VertStream::Serialize(FArchive& ar)
	{
		ASSERT(m_pBuf!=NULL);

		int type=m_type;
		ar<<type;
		ar<<m_numVert;
		ar<<m_stride;
		UINT bufSize=m_stride*m_numVert;
		ar.BufferWrite(m_pBuf,bufSize);

	}

	void VertStream::Deserialize(FArchive& ar)
	{
		Destroy();

		int type;
		ar>>type;
		m_type=(EVertType)type;
		ar>>m_numVert;
		ar>>m_stride;
		UINT bufSize=m_stride*m_numVert;
		m_pBuf=(BYTE*)malloc(bufSize);
		ASSERT(m_pBuf!=NULL);
		ar.BufferRead(m_pBuf,bufSize);
	}

}//namespace Cool3D