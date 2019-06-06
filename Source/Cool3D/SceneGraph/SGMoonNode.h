#pragma once
#include "scenenode.h"
#include ".\sgsunnode.h"
#include"SGSkyDome.h"

namespace F3D
{
	/**	\class SGMoonNode 
		\brief 月亮效果
		\remarsk 必须事先初始化太阳结点
		\see 
	*/
	class F3D_API SGMoonNode : public SceneNode
	{
	public:
		SGMoonNode(void);
		virtual ~SGMoonNode(void);

		//--这些SceneNode在这里都没用啦
		virtual bool IsInFrustum(const Frustum *pFrumstum) const;
		virtual void LoadRes(const TCHAR* szName);
		virtual const Matrix4& GetWorldMat() const;
		virtual void SetWorldMat(const Matrix4 *pMat);
		virtual void GetBox(AABBox& out) const;

		virtual UINT GetNumRenderNode() const;
		virtual RenderNode *GetRenderNode(UINT i) const;
		virtual IMaterial*	GetMtl(int mtlID);
		virtual void Update(float deltaTime,SceneGraph *pSG);
		virtual void ForceUpdate(float deltaTime,SceneGraph *pSG);

		//功能
		void Create(Vector3 InitMoonPos, Vector3 InitMoonRotateAxis,float fSize,SGSunNode* pSunNode,SGSkyDome* pSkyDome);
		//--设置日期信息
		void SetDateInfo(int nDate);
		//更新月亮的位置
		void UpdateMoonPos(float deltaTime,Vector3 CamPos);
		//得到月光照射方向
		Vector3 GetLightDir();


	private:
		class Member;
		Member *m_p;

		DECL_RTTI(SGMoonNode);
	};
}//namespace F3D