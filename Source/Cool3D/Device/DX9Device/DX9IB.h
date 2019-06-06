#pragma once
#include "..\iindexbuffer.h"
#include <d3d9.h>

namespace Cool3D
{
	namespace Detail
	{
		class DX9IB : public IIndexBuffer
		{
		public:
			DX9IB(void);
			virtual ~DX9IB(void);

			IDirect3DIndexBuffer9* GetDX9() const
			{
				return m_pIB;
			}

			virtual void CreateWord(UINT nSizeInBytes,EBufferType type);
			virtual void Destroy();

			virtual void CopyFromMem(const void *pBuf,UINT nSizeInBytes,bool bNoOverwrite=false);
			virtual UINT GetSize() const {	return m_nSize;}

			void PreDeviceReset();
			void PostDeviceReset();

			virtual bool IsDynamic();
			EBufferType GetType(){ return m_bufferType;}

		private:
			void CopyToDeviceBuffer(const void *pBuf,UINT nSizeInBytes,bool bNoOverwrite);

			IDirect3DIndexBuffer9	*m_pIB;
			UINT					m_nSize;
			
			WORD*					m_memShadow;
			int						m_memShadowSize;

			EBufferType				m_bufferType;

			bool m_bValid;
		};
	}
}//endof namespace