#pragma once
#include "rendernode.h"
#include "RenderSubMesh.h"
namespace Cool3D
{
	class VertStream;
	/**	\class DynamicMeshIndexed
		\brief 使用动态VB的indexed mesh
	*/
	class DynamicMeshIndexed :	public RenderNode
	{
	public:
		DynamicMeshIndexed(void);
		virtual ~DynamicMeshIndexed(void);

		//--RenderNode
		virtual bool SupportLod();
		virtual void Draw(RenderSys *pSys,SceneNode *pSGNode,int nLod=0,RenderItem* pItem=NULL,SceneGraph* pSG=NULL);
		virtual EMtlTrait GetMtlTrait(SceneNode *pSGNode) const;
		virtual DWORD GetMtlTrait1(SceneNode *pSGNode) const;
		virtual DWORD GetMtlTrait2(SceneNode *pSGNode) const;
		virtual const AABBox& GetAABBox() const;
		virtual int GetMtlID() const{ return m_subMesh.GetMtlID();}

		//--
		void Create(const LodSubMesh* pSubMesh);
		void CreateDirect(EDrawType drawType,WORD baseIndex,int mtlID,WORD numVert,UINT indexBufSize/*in bytes*/,WORD *pIndices);
		void AttachVertStream(VertStream *pVStream);
		VertStream* GetVertStream()	{	return m_pVertStream;}
		void SetMtlID(int ID)	{	m_subMesh.SetMtlID(ID);}
	private:
		RenderSubMesh	m_subMesh;
		VertStream		*m_pVertStream;

		DECL_RTTI_DYNC(DynamicMeshIndexed);
	};
}//namespace Cool3D