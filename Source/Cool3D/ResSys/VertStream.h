#pragma once
#include "..\Cool3D.h"
#include "..\Util\Noncopyable.h"
#include "..\Public\VertexDefine.h"
#include "..\Util\SharedPtr.h"
#include "..\Math\GeomDefine.h"
#include "..\Util\FArchive.h"

	class IFS;
namespace Cool3D
{
	/**	\class VertStream
		\brief vertex stream
	*/
	class Cool3D_API VertStream : private Noncopyable
	{
	public:
		VertStream(void);
		virtual ~VertStream(void);

		/**	创建buffer,并从文件中读取所有顶点
		*/
		void CreateFromFile(IFS *pFS,DWORD hFile,EVertType vertType,UINT numVert);
		void Create(UINT stride,UINT numVert);
		void Create(EVertType vertType,UINT numVert);
		/**	新创建一个对象,它的内容和自己是完全一致的
			\param 是否拷贝Buffer中的顶点内容
		*/
		VertStream *Clone(bool bMemCpy);
		/**	得到单个顶点的大小
		*/
		UINT GetStride() const
		{	return m_stride; }
		EVertType GetType() const
		{	return m_type;	}
		UINT GetNumVert() const
		{	return m_numVert; }
		UINT GetBufferSize() const
		{	return m_numVert*m_stride; }
		void* GetRawBuffer() const
		{	return m_pBuf;	}

		//--
		bool GetPos(UINT i,Vector3& out) const;
		bool GetNormal(UINT i,Vector3& out) const;
		bool GetTangent(UINT i,Vector3& out) const;
		void BuildAABBox(AABBox& box);

		void Serialize(FArchive& ar);
		void Deserialize(FArchive& ar);

	protected:
		void Destroy();
	protected:
		EVertType	m_type;
		UINT		m_numVert;
		BYTE		*m_pBuf;
		UINT		m_stride;
	};

	//typedef SharedPtr<VertStream,SharePtrDelete<VertStream> >	SharedVertStream;
}//namespace Cool3D