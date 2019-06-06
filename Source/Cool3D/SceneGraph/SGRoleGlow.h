#pragma once
#include "..\..\Cool3D\scenegraph\scenenode.h"


namespace Cool3D
{
	/**	\class SGRoleGlow
		\brief 角色光晕效果
		\remarks 可以作为SGAvatarNode的child使用
	*/
	class Cool3D_API SGRoleGlow : public SceneNode
	{
	public:
		enum EStyle//光晕风格
		{
			ES_Round,//球形Billboard
			ES_Bucket,//桶形Billboard
		};

		struct tagParam//参数
		{
			tstring szHeadBone;//起始骨骼名称
			tstring szTailBone;//结束骨骼名称
			int uDim;//贴图横向帧数
			int vDim;//贴图纵向帧数
			float fps;//贴图动画速度
			float width;//片宽度(半径)
			float heightExtend;//片高度延长
		};

	public:
		SGRoleGlow(void);
		virtual ~SGRoleGlow(void);

		//--SceneNode
		virtual bool IsInFrustum(const Frustum *pFrumstum) const;
		virtual void LoadRes(const TCHAR *szName);
		virtual UINT GetNumRenderNode() const;
		virtual RenderNode *GetRenderNode(UINT i) const;
		virtual const Matrix4& GetWorldMat() const;
		virtual void SetWorldMat(const Matrix4 *pMat);
		virtual void GetBox(AABBox& out) const;
		virtual IMaterial*	GetMtl(int mtlID);
		virtual void Update(float deltaTime,SceneGraph *pSG);
		virtual void ForceUpdate(float deltaTime,SceneGraph *pSG);

		//--
		void Init(EStyle style,tagParam& param);
		void Enable(bool bEnable) {m_bEnable=bEnable;}
		void SetColor(Color4ub& color);
		void SetScale(float scale);
		void SetBlendOne(bool bBlendOne);

	private:
		bool m_bEnable;		

		class Member;
		Member *m_p;

		DECL_RTTI_DYNC(SGRoleGlow);
	};

}//namespace Cool3D