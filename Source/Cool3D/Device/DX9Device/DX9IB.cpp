#include "StdAfx.h"
#include ".\dx9ib.h"
#include "..\..\Util\DebugMisc.h"
#include "..\..\Util\Exception.h"
#include "..\..\Util\FMemory.h"
#include "DX9Device.h"

namespace Cool3D
{
	namespace Detail
	{
		DX9IB::DX9IB(void)
		{
			m_pIB=NULL;
			m_nSize = 0;
			
			m_memShadow = NULL;
			m_memShadowSize = 0;

			m_bValid = false;
		}

		DX9IB::~DX9IB(void)
		{
			Destroy();
		}

		extern D3DPOOL SelectPool(EBufferType type);
		extern DWORD BufferType2D3DUSAGE(EBufferType type);
		
		void DX9IB::CreateWord(UINT nSizeInBytes,EBufferType type)
		{
			IDirect3DDevice9 *pDev=DX9Dev();
			HRESULT hr=pDev->CreateIndexBuffer(nSizeInBytes,
				BufferType2D3DUSAGE(type),
				D3DFMT_INDEX16,
				SelectPool(type),
				&m_pIB,
				NULL);
			THROW_FAILED(hr,NULL);
			
			if(m_memShadow == NULL)//可能是Device Reset
			{
				m_memShadow = (WORD*)malloc(nSizeInBytes);
				ASSERT(m_memShadow!=NULL);
				memset(m_memShadow,0,nSizeInBytes);
			}

			m_nSize=nSizeInBytes;
			m_bufferType = type;

#ifdef RENDER_STATUS
			if(m_bufferType==EB_Static
				||m_bufferType==EB_StaticWriteOnly)
				Device()->GetRenderStatus()->staticIBSize+=m_nSize;
			else
				Device()->GetRenderStatus()->dynamicIBSize+=m_nSize;
#endif

			m_bValid = true;
		}

		void DX9IB::Destroy()
		{
			MyDX9Dev()->OnIBFree(this);
			SAFE_RELEASE(m_pIB);

			SAFE_FREE(m_memShadow);


#ifdef RENDER_STATUS
			if(m_bufferType==EB_Static
				||m_bufferType==EB_StaticWriteOnly)
				Device()->GetRenderStatus()->staticIBSize-=m_nSize;
			else
				Device()->GetRenderStatus()->dynamicIBSize-=m_nSize;
#endif

			m_bValid = false;
		}

		void DX9IB::CopyFromMem(const void *pBuf,UINT nSizeInBytes,bool bNoOverwrite)
		{
			CopyToDeviceBuffer(pBuf,nSizeInBytes,bNoOverwrite);

			//--拷贝到mem shadow
			if(m_memShadow!=NULL)
			{
				m_memShadowSize = nSizeInBytes;
				FMemcpy(m_memShadow,pBuf,nSizeInBytes);
			}
		}

		void DX9IB::PreDeviceReset()
		{
			SAFE_RELEASE(m_pIB);

#ifdef RENDER_STATUS
			if(m_bufferType==EB_Static
				||m_bufferType==EB_StaticWriteOnly)
				Device()->GetRenderStatus()->staticIBSize-=m_nSize;
			else
				Device()->GetRenderStatus()->dynamicIBSize-=m_nSize;
#endif

		}

		void DX9IB::PostDeviceReset()
		{
			if(m_bValid)
			{
				CreateWord(m_nSize,m_bufferType);

				if(m_memShadow!=NULL)
					CopyToDeviceBuffer(m_memShadow,m_memShadowSize,false);
			}
			
		}

		void DX9IB::CopyToDeviceBuffer(const void *pBuf,UINT nSizeInBytes,bool bNoOverwrite)
		{
			ASSERT(nSizeInBytes <= m_nSize);

			DWORD flags=0;
			if(bNoOverwrite)
				flags|=D3DLOCK_NOOVERWRITE;
			else if(m_bufferType==EB_Dynamic || m_bufferType==EB_DynamicWriteOnly)
				flags|=D3DLOCK_DISCARD;

			void *pLock=NULL;
			HRESULT hr=m_pIB->Lock(0,nSizeInBytes,&pLock,flags);
			if(FAILED(hr))
			{
				TRACE(_T("Index Buffer lock failed."));
				return;
			}
			FMemcpy(pLock,pBuf,nSizeInBytes);
			hr=m_pIB->Unlock();
		}

		bool DX9IB::IsDynamic()
		{
			return m_bufferType==EB_Dynamic
				||m_bufferType==EB_DynamicWriteOnly;
		}
	}
}//endof namespace