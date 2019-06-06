#pragma once

#include "IVertBuffer.h"//for bufferType define
#include "..\Util\SharedPtr.h"

namespace Cool3D
{
	/**	\class IIndexBuffer
		\brief IndexBuffer对象
	*/
	class IIndexBuffer
	{
	public:
		IIndexBuffer(void);
		virtual ~IIndexBuffer(void);

		virtual void CreateWord(UINT nSizeInBytes,EBufferType type) =0;
		virtual void Destroy() =0;
		virtual bool IsDynamic() =0;

		/**	从一块内存中拷贝内容
		*/
		virtual void CopyFromMem(const void *pBuf,UINT nSizeInBytes,bool bNoOverwrite=false) =0;
		virtual UINT GetSize() const =0;
	};
}//namespace Cool3D