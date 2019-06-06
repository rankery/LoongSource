#include "StdAfx.h"
#include ".\dx9vb.h"
#include "..\..\Util\DebugMisc.h"
#include "..\..\Util\Exception.h"
#include "DX9Device.h"
#include "..\..\Util\FMemory.h"

namespace Cool3D
{
	namespace Detail
	{
		DX9VB::DX9VB(void)
		{
			m_pVB=NULL;
			m_nStride=0;
			m_nNumVert=0;
			m_nSize=0;
			m_bufType=EB_Static;

			m_memShadow = NULL;
			m_shadowSize = 0;

			m_bValid = false;
		}

		DX9VB::~DX9VB(void)
		{
			MyDX9Dev()->OnVBFree(this);
			Destroy();
		}

		UINT DX9VB::GetStride()
		{
			return m_nStride;
		}
		UINT DX9VB::GetNumVert()
		{
			return m_nNumVert;
		}
		UINT DX9VB::GetSize()
		{
			return m_nSize;
		}
		bool DX9VB::IsDynamic()
		{
			return m_bufType==EB_Dynamic
				||m_bufType==EB_DynamicWriteOnly;
		}

		DWORD BufferType2D3DUSAGE(EBufferType type)
		{
			switch(type)
			{
			case EB_Static:
				return 0;
			case EB_Dynamic:
				return D3DUSAGE_DYNAMIC;
			case EB_StaticWriteOnly:
				return D3DUSAGE_WRITEONLY;
			case EB_DynamicWriteOnly:
				return D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY;
			}
			return 0;
		}

		D3DPOOL SelectPool(EBufferType type)
		{
			/*if(type==D3DUSAGE_DYNAMIC
				|| type==EB_DynamicWriteOnly)
				return D3DPOOL_MANAGED;//dynamic vb不能创建在DefaultPool,见DX9文档
			else
			*/
				return D3DPOOL_DEFAULT;
		}

		void DX9VB::Create(UINT nSizeInBytes, EBufferType type, UINT nStride)
		{
			m_nStride=nStride;

			D3DPOOL pool=SelectPool(type);
			
			
			DWORD FVF=0;//不使用FVF
			IDirect3DDevice9 *pDev=DX9Dev();
			HRESULT hr=pDev->CreateVertexBuffer(nSizeInBytes,BufferType2D3DUSAGE(type),
				FVF,pool,&m_pVB,NULL);
			THROW_FAILED(hr,NULL);

			m_nSize=nSizeInBytes;
			m_bufType=type;

			if(type == EB_Static
				|| type == EB_StaticWriteOnly)//dynamic buffer不需要shaodw
			{
				if(m_memShadow == NULL)
				{
					m_memShadow = (BYTE *)malloc(nSizeInBytes);
					ASSERT(m_memShadow != NULL);
					memset(m_memShadow,0,nSizeInBytes);
					m_shadowSize = nSizeInBytes;
				}
			}

#ifdef RENDER_STATUS
			if(m_bufType==EB_Static
				||m_bufType==EB_StaticWriteOnly)
				Device()->GetRenderStatus()->staticVBSize+=m_nSize;
			else
				Device()->GetRenderStatus()->dynamicVBSize+=m_nSize;
#endif

			m_bValid = true;
		}
		
		void DX9VB::Destroy()
		{
#ifdef RENDER_STATUS
			if(m_bufType==EB_Static
				||m_bufType==EB_StaticWriteOnly)
				Device()->GetRenderStatus()->staticVBSize-=m_nSize;
			else
				Device()->GetRenderStatus()->dynamicVBSize-=m_nSize;
#endif

			SAFE_RELEASE(m_pVB);
			m_nSize=0;

			SAFE_FREE(m_memShadow);


			m_bValid = false;
		}

		void DX9VB::_CopyFromMem(const void *pBuf, UINT nSizeInBytes,UINT nOffsetBytes,bool bNoOverwrite)
		{
			CopyToDeviceBuffer(pBuf,nSizeInBytes,nOffsetBytes,bNoOverwrite);

			if(m_memShadow != NULL)
			{
				FMemcpy(m_memShadow+nOffsetBytes,pBuf,nSizeInBytes);
			}
		}

		void DX9VB::CopyFromVertStream(const VertStream *pStream,UINT nOffsetBytes,bool bNoOverwrite)
		{
			m_nNumVert=pStream->GetNumVert();
			m_nStride=pStream->GetStride();
			ASSERT(m_nSize >= pStream->GetBufferSize()+nOffsetBytes);
			_CopyFromMem(pStream->GetRawBuffer(),pStream->GetBufferSize(),nOffsetBytes,bNoOverwrite);
			m_nStride=pStream->GetStride();
		}

		void DX9VB::CopyFromMem(void* pBuf, UINT nSizeInBytes, UINT nNumVert, UINT nStride,UINT nOffsetBytes,bool bNoOverwrite)
		{
			ASSERT( pBuf != NULL );
			ASSERT( nSizeInBytes >= nStride * nNumVert );

			m_nNumVert=nNumVert;
			m_nStride=nStride;
			ASSERT(m_nSize >= nOffsetBytes+nSizeInBytes);
			_CopyFromMem(pBuf,nSizeInBytes,nOffsetBytes,bNoOverwrite);
		}
		
		void DX9VB::PreDeviceReset()
		{
#ifdef RENDER_STATUS
			if(m_bufType==EB_Static
				||m_bufType==EB_StaticWriteOnly)
				Device()->GetRenderStatus()->staticVBSize-=m_nSize;
			else
				Device()->GetRenderStatus()->dynamicVBSize-=m_nSize;
#endif

			SAFE_RELEASE(m_pVB);
		}

		void DX9VB::PostDeviceReset()
		{
			if(m_bValid)
			{
				Create(m_nSize,m_bufType,m_nStride);

				if(m_memShadow != NULL)
					CopyToDeviceBuffer(m_memShadow,m_shadowSize,0,false);
			}
		}

		void DX9VB::CopyToDeviceBuffer(const void *pBuf, UINT nSizeInBytes,UINT nOffsetBytes,bool bNoOverwrite)
		{
			ASSERT(pBuf!=NULL);
			ASSERT(nOffsetBytes+nSizeInBytes <= m_nSize);
			void *pLock=NULL;
			DWORD flags=0;

			if(bNoOverwrite)
				flags|=D3DLOCK_NOOVERWRITE;
			else if(m_bufType==EB_Dynamic || m_bufType==EB_DynamicWriteOnly)
				flags|=D3DLOCK_DISCARD;//Can specify D3DLOCK_DISCARD or D3DLOCK_NOOVERWRITE for only Vertex Buffers created with D3DUSAGE_DYNAMIC

			HRESULT hr=m_pVB->Lock(nOffsetBytes,nSizeInBytes,&pLock,flags);
			if(FAILED(hr))
			{
				TRACE(_T("Vertex Buffer lock failed.\n"));
				return;
			}
			FMemcpy(pLock,pBuf,nSizeInBytes);
			hr=m_pVB->Unlock();
		}
	}
}//endof namespace