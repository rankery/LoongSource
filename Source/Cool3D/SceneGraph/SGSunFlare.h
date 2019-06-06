#pragma once
#include "scenenode.h"

namespace Cool3D
{
	/**	\class SGSunFlare 
		\brief 太阳光晕
		\remarks 
		\see 
	*/
	class SGSunNode;
	class Cool3D_API SGSunFlare : public SceneNode
	{
	public:
		SGSunFlare(void);
		virtual ~SGSunFlare(void);

		//--SceneNode
		virtual bool IsInFrustum(const Frustum *pFrumstum) const;
		virtual void LoadRes(const TCHAR* szName);
		virtual UINT GetNumRenderNode() const;
		virtual RenderNode *GetRenderNode(UINT i) const;
		virtual const Matrix4& GetWorldMat() const;
		virtual void SetWorldMat(const Matrix4 *pMat);
		virtual void GetBox(AABBox& out) const;
		virtual IMaterial*	GetMtl(int mtlID);
		virtual void Update(float deltaTime,SceneGraph *pSG);
		virtual void ForceUpdate(float deltaTime,SceneGraph *pSG);

		/** \创建
		*/
		bool Create(const TCHAR* szTexBaseName, const TCHAR* szExt);
		/** \遮挡查询接口，由渲染系统调用
		*/
		bool BeginVizQuery();
		bool EndVizQuery();
		float GetVisble();
		/** \硬件是否支持
		*/
		static bool IsHWSupport();
	private:

		class Member;
		Member *m_p;

		DECL_RTTI(SGSunFlare);
	};
}//namespace Cool3D