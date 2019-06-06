#pragma once
#include "..\Cool3D.h"
#include "SceneNode.h"

namespace Cool3D
{
	class NodeAnimationCtrl;
	class ResSkeleton;
	/**	\class AvatarNode
		\brief 使用骨骼动画,支持换装的Entity
	*/
	class Cool3D_API AvatarNode :	public SceneNode
	{
	public:
		AvatarNode(void);
		virtual ~AvatarNode(void);

		//--序列化接口
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);
		//--SceneNode
		virtual bool IsInFrustum(const Frustum *pFrumstum) const;
		/**	只可以是Skeleton Resource */
		virtual void LoadRes(const TCHAR* szName);
		virtual UINT GetNumRenderNode() const;
		virtual RenderNode *GetRenderNode(UINT i) const;
		virtual const Matrix4& GetWorldMat() const;
		virtual void SetWorldMat(const Matrix4 *pMat);
		virtual void GetBox(AABBox& out) const;
		virtual void Update(float deltaTime,SceneGraph *pSG);
		virtual void ForceUpdate(float deltaTime,SceneGraph *pSG);
		virtual IMaterial*	GetMtl(int mtlID);
		virtual TResult RayCollide(const Ray& ray);

		//--
		/**	外部可以控制AniCtrl的播放(所以不使用const)*/
		NodeAnimationCtrl* GetSkeletonAniCtrl() 	{	return m_pAniCtrl;}
		ResSkeleton* GetResource() const {	return m_pRes;}

		/** 所有皮肤子结点是否已创建 */
		bool IsCreated();

		void Hide(bool bHide){ m_bHide=bHide; }
		bool IsHide()		 { return m_bHide;}

		/** \透明效果
		*/
		void SetTransparent(float alpha);

		/** \颜色设置
		*/
		void SetMulColor(const float r, const float g, const float b);

		/** Editor sport */
		bool ReloadMsgTable();

	private:
		void UpdateBoundingBox();

	private:
		ResSkeleton			*m_pRes;
		NodeAnimationCtrl	*m_pAniCtrl;
		Matrix4				m_toWorldMat;

		AABBox				m_box;//in world space

		//--动画计算优化
		bool			m_bHide;//是否隐藏

		DECL_RTTI_DYNC(AvatarNode);
	};
}//namespace Cool3D