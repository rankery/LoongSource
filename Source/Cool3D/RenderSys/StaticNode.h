#pragma once
#include "rendernode.h"
#include "..\Math\GeomDefine.h"
#include "RenderSubMesh.h"


namespace F3D
{
	class LodSubMesh;
	struct tagMtlDesc;
	
	/**	\class StaticNode 
		\brief 静态物体,不需要任何动画计算
	*/
	class StaticNode :	public RenderNode
	{
	public:
		StaticNode(void);
		virtual ~StaticNode(void);

		void Create(SharedVB pVB,EVertType vt,const LodSubMesh* pSubMesh);
		virtual void Draw(RenderSys *pSys,SceneNode *pSGNode,int nLod =0);
		virtual ETransparentType GetTransparentType(const SceneNode *pSGNode) const;
		virtual DWORD GetMtlTrait(SceneNode *pSGNode) const;
		virtual const AABBox& GetAABBox() const;

		EVertType GetVertType()		{	return m_vertType; }
		

	protected:
		SharedVB		m_pVB;
		EVertType		m_vertType;
		RenderSubMesh	m_subMesh;
		
		DECL_RTTI_DYNC(StaticNode);
	};
}//namespace F3D