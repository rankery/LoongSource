#pragma once
#include "scenenode.h"

namespace Cool3D
{
	class CameraBase;

	/**	\class SGSunNode 
		\brief 太阳
		\see 
	*/
	class Cool3D_API SGSunNode : public SceneNode
	{
	public:
		SGSunNode(void);
		virtual ~SGSunNode(void);

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
		    \szTexBasePath 贴图路径，贴图文件名内部指定为（sun, flare:sun_1,sun2...）
			 szExt 文件扩展名
		*/
		bool Create( const TCHAR* szTexBasePath );
		/** \获取太阳位置等信息
		*/
		Vector3 GetPos() const;
		POINT   GetScreenPos() const;
		float   GetNumPixels() const;
		/** \是否在视锥内
		*/
		bool IsInFrustum() const;
		/** \打开/关闭光晕
		*/
		void EnableFlare( const bool bEnable ) { m_bEnableFlare = bEnable; }
		bool IsEnableFlare() const { return m_bEnableFlare; }
	private:
		class Member;
		Member *m_p;
		bool m_bEnableFlare;    // 是否打开光晕

		DECL_RTTI(SGSunNode);
	};
}//namespace Cool3D