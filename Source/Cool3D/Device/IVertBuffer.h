#pragma once
#include "..\Util\SharedPtr.h"
#include "..\ResSys\VertStream.h"

namespace Cool3D
{
	enum EBufferType
	{
		EB_Static,
		EB_Dynamic,
		EB_StaticWriteOnly,
		EB_DynamicWriteOnly,
	};

	/**	\class IVertBuffer
		\brief Vertex Buffer∂‘œÛ
	*/
	class IVertBuffer
	{
	public:
		IVertBuffer(void);
		virtual ~IVertBuffer(void);

		virtual void Create(UINT nSizeInBytes, EBufferType type, UINT nStride) =0;
		virtual void Destroy() =0;
		virtual UINT GetStride() =0;
		virtual UINT GetNumVert() =0;
		virtual bool IsDynamic() =0;
		virtual UINT GetSize() =0;

		virtual void CopyFromVertStream(const VertStream *pStream,UINT nOffsetBytes=0,bool bNoOverwrite=false) =0;
		virtual void CopyFromMem(void* pBuf, UINT nSizeInBytes, UINT nNumVert, UINT nStride,UINT nOffsetBytes=0,bool bNoOverwrite=false) =0;
	};
}//namespace Cool3D