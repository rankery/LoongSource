#pragma once
#include "rendernode.h"
#include "..\Math\GeomDefine.h"
#include "RenderSubMesh.h"


namespace Cool3D
{
	class LodSubMesh;
	struct tagMtlDesc;
	
	/**	\class StaticMeshIndexed 
		\brief 静态物体,不需要任何动画计算
	*/
	class StaticMeshIndexed :	public RenderNode
	{
	public:
		StaticMeshIndexed(void);
		virtual ~StaticMeshIndexed(void);

		void CreateDirect(WORD baseIndex,int mtlID,VertStream *pVertStream,UINT indexBufSize/*in bytes*/,WORD *pIndices);
		void Create(SharedVB pVB,EVertType vt,const LodSubMesh* pSubMesh);
		virtual void Draw(RenderSys *pSys,SceneNode *pSGNode,int nLod=0,RenderItem* pItem=NULL,SceneGraph* pSG=NULL);
		virtual EMtlTrait GetMtlTrait(SceneNode *pSGNode) const;
		virtual DWORD GetMtlTrait1(SceneNode *pSGNode) const;
		virtual DWORD GetMtlTrait2(SceneNode *pSGNode) const;
		virtual const AABBox& GetAABBox() const;
		virtual int GetMtlID() const{ return m_subMesh.GetMtlID();}

		EVertType GetVertType()		{	return m_vertType; }
		int GetMtlID()				{   return m_subMesh.GetMtlID();}

	protected:
		SharedVB		m_pVB;
		EVertType		m_vertType;
		RenderSubMesh	m_subMesh;
		
		DECL_RTTI_DYNC(StaticMeshIndexed);
	};
}//namespace Cool3D