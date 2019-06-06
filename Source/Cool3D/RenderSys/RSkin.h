#pragma once
#include "rendernode.h"
#include "RenderSubMesh.h"
#include "..\ResSys\ResSkin.h"

namespace Cool3D
{
	class MSkin;
	class NodeAnimationCtrl;
	/**	\class RSkin 
		\brief 蒙皮模型渲染器
	*/
	class RSkin :public RenderNode
	{
	public:
		RSkin(void);
		virtual ~RSkin(void);

		//--RenderNode
		virtual void Draw(RenderSys *pSys,SceneNode *pSGNode,int nLod=0,RenderItem* pItem=NULL,SceneGraph* pSG=NULL);
		virtual EMtlTrait GetMtlTrait(SceneNode *pSGNode) const;
		virtual DWORD GetMtlTrait1(SceneNode *pSGNode) const;
		virtual DWORD GetMtlTrait2(SceneNode *pSGNode) const;
		virtual const AABBox& GetAABBox() const;

		//--
		void Create(const LodSubMesh* pSubMesh,IVertBuffer* pStaticVB,EVertType vertType,int subMeshID);
		int  GetSubMeshID() { return m_subMeshID; }
	private:
		void SetShader(MSkin* pMtl,SceneNode* pSGNode,const RenderItem* pItem,SceneGraph* pSG);
		void SetBumpDirectVShader(IVertShader* pVS,SceneGraph* pSG,SceneNode* pSGNode);
		void SetBumpDirectPShader(IPixelShader* pPS,MSkin* pMtl,SceneGraph* pSG);
		void SetBumpPoint1VShader(IVertShader* pVS,const RenderItem* pItem);
		void SetBumpPoint1PShader(IPixelShader* pPS,MSkin* pMtl,const RenderItem* pItem);
		void SetSkinVShader(IVertShader* pVS,SceneNode* pSGNode);
		void SetDirectVShader(IVertShader* pVS,MSkin* pMtl,SceneNode* pSGNode,SceneGraph* pSG);
		void SetPoint1VShader(IVertShader* pVS,MSkin* pMtl,const RenderItem* pItem);

	private:
		int				m_subMeshID;
		RenderSubMesh	m_subMesh;
		IVertBuffer*	m_pStaticVB;//用于shader加速的蒙皮计算
		EVertType		m_vertType;

		DECL_RTTI_DYNC(RSkin);
	};
}//namespace Cool3D