#pragma once
#include "scenenode.h"
#include "..\Device\Light.h"

namespace Cool3D
{
	/**	\class SGLightNode 
		\brief 点光源,射光灯作为场景节点处理
		\par 静态和动态光源都用这个类处理
		\par 灯光Node可能作为其他SceneNode的child,随父节点移动
	*/
	class Cool3D_API SGLightNode :	public SceneNode
	{
	public:
		SGLightNode(void);
		virtual ~SGLightNode(void);

		//--序列化接口
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);
        
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

		void SetLight(const tagLight *pLgt);
		void SetDynamic(bool bDynamic);
		bool IsDynamic();
		tagLight* GetLight();
		float GetNoBlockRange() const;
		void SetNoBlockRange(float range);
		void SetLightModulus(float modulus);
		float GetLightModulus() const;
		
		//! 计算对于某个物体(包裹盒)的影响系数,系数越大影响越大,小于等于0则没影响
		float LightCost(const AABBox& box);
	private:
		class Member;
		Member *m_p;

		DECL_RTTI_DYNC(SGLightNode);
	};
}//namespace Cool3D