#pragma once
#include "..\Math\GeomDefine.h"
#include "..\Kernel.h"
#include "..\Device\IDevice.h"
#include "..\Device\IIndexBuffer.h"
#define VFS_READ
#include "..\VFS\IFS.h"
#include "..\Public\SubMesh.h"
#include "..\Util\Exception.h"
#include "RenderSys.h"
#include "RenderNode.h"

namespace Cool3D
{
	class IMaterial;
	/**	\class RenderSubMesh
		\biref 保存Index buffers
	*/
	class RenderSubMesh
	{
	public:
		RenderSubMesh(void);
		virtual ~RenderSubMesh(void);

		void CreateDirect(EDrawType drawType,WORD baseIndex,int mtlID,WORD numVert,UINT indexBufSize/*in bytes*/,WORD *pIndices)
		{
			m_baseIndex	=	baseIndex;
			m_mtlID		=	mtlID;
			m_numVert	=	numVert;
			IIndexBuffer *pIB= Device()->NewIB();
			pIB->CreateWord(indexBufSize,EB_StaticWriteOnly);
			pIB->CopyFromMem(pIndices,indexBufSize);
			m_lodFaces.push_back(pIB);

			m_drawType=drawType;
		}

		void CreateFromSubMesh(const SubMesh* pSubMesh)
		{
			m_baseIndex	=	pSubMesh->baseIndex;
			m_mtlID		=	pSubMesh->mtlID;
			m_numVert	=	pSubMesh->numVert;
			m_box.max	=	pSubMesh->max;
			m_box.min	=	pSubMesh->min;

			ASSERT(m_lodFaces.size()==0);//没有创建过
			IIndexBuffer *pIB=AllocIndexBuffer(pSubMesh->baseFaces);
			THROW_NULL(pIB,_T("Render sub mesh index buffer create failed"));
			m_lodFaces.push_back(pIB);

			m_drawType=EDraw_TriangleList;
		}

		void CreateFromLodSubMesh(const LodSubMesh* pSubMesh)
		{
			CreateFromSubMesh(pSubMesh);
			for(int i=0;i<pSubMesh->numLod;i++)
			{
				IIndexBuffer *pIB=AllocIndexBuffer(pSubMesh->lodFaces[i]);
				THROW_NULL(pIB,_T("Render sub mesh LOD index buffer create failed"));
				m_lodFaces.push_back(pIB);
			}
		}

		void Draw(int lod,RenderSys *pSys,RenderNode *pNode,IMaterial *pMtl)
		{
			if(m_lodFaces.size()==0)
				return;
			if(lod<0)
				lod=0;
			if(lod>=(int)m_lodFaces.size())
				lod=m_lodFaces.size()-1;
			if(m_lodFaces.size()==1)
				lod=0;
			IIndexBuffer *pIB=m_lodFaces[lod];
			int numFaces=0;
			if(m_drawType==EDraw_TriangleList)
				numFaces=(pIB->GetSize()/sizeof(WORD))/3;
			else if(m_drawType==EDraw_TriangleStrip)
				numFaces=(pIB->GetSize()/sizeof(WORD))-2;
			else
				ASSERT(0 && "todo");

			IDevice *pDevice=Device();
			pDevice->SetIB(pIB);

			int numPass=pNode->BeginMtl(pSys,pMtl);
			for(int i=0;i<numPass;i++)
			{
				pNode->BeginMtlPass(i);
				pDevice->DrawIndexed(m_drawType,m_baseIndex,m_numVert,numFaces);
				pNode->EndMtlPass();
			}
			pNode->EndMtl();
		}
		const AABBox& GetAABBox() const
		{
			return m_box;
		}
		int GetMtlID() const
		{
			return m_mtlID;
		}
		void SetMtlID(int ID)	{	m_mtlID=ID;}
	private:
		IIndexBuffer* AllocIndexBuffer(const FaceBuffer& faces)
		{
			IIndexBuffer *pIB = Device()->NewIB();
			int ibsize=faces.size()*3*sizeof(WORD);
			pIB->CreateWord(ibsize,EB_StaticWriteOnly);
			pIB->CopyFromMem(&(*faces.begin()),ibsize);
			return pIB;
		}
	private:
		WORD					m_baseIndex;
		int						m_mtlID;
		WORD					m_numVert;
		AABBox					m_box;
		vector<IIndexBuffer*>	m_lodFaces;//每个Indexbuffer针对一个lod,至少一个
		EDrawType				m_drawType;
	};
}//namespace Cool3D