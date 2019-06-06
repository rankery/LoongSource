#pragma once
#include "..\ivertbuffer.h"
#include <d3d9.h>

namespace Cool3D
{
	namespace Detail
	{
		class DX9VB : public IVertBuffer
		{
		public:
			DX9VB(void);
			virtual ~DX9VB(void);

			virtual void Create(UINT nSizeInBytes, EBufferType type, UINT nStride);
			virtual void Destroy();
			virtual UINT GetStride();
			virtual UINT GetNumVert();
			virtual bool IsDynamic();
			virtual UINT GetSize();
			
			virtual void CopyFromVertStream(const VertStream *pStream,UINT nOffsetBytes=0,bool bNoOverwrite=false);
			virtual void CopyFromMem(void* pBuf, UINT nSizeInBytes, UINT nNumVert, UINT nStride,UINT nOffsetBytes=0,bool bNoOverwrite=false);
			
			IDirect3DVertexBuffer9 *GetDX9() const
			{ return m_pVB; }

			void PreDeviceReset();
			void PostDeviceReset();

			EBufferType GetType(){ return m_bufType;}

		private:
			void _CopyFromMem(const void *pBuf, UINT nSizeInBytes,UINT nOffsetBytes,bool bNoOverwrite);
			void CopyToDeviceBuffer(const void *pBuf, UINT nSizeInBytes,UINT nOffsetBytes,bool bNoOverwrite);
		private:
			EBufferType		m_bufType;
			
			BYTE*			m_memShadow;
			int				m_shadowSize;

			UINT	m_nNumVert;
			UINT	m_nStride;
			UINT	m_nSize;
			IDirect3DVertexBuffer9 *m_pVB;

			bool m_bValid;
		};

	}
}//endof namespace