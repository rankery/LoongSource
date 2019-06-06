#pragma once
#include "effectpathctrl.h"

namespace Cool3D
{
	/**	\class EffectTarget 
		\brief 指定目标点和运动曲线,来产生运动路径的特效路径控制
	*/
	class Cool3D_API EffectTarget : public EffectPathCtrl
	{
	public:
		struct tagTargetProp : public tagProp
		{
			int			speedType;	//0直线,1cos曲线,2sin曲线
			float		time;		//从起点到终点使用多少时间
			float		speed;		//当time<=0时,使用指定的速度估算time
			Vector3		startPt;
			Vector3		endPt;
			Vector3		reservePt[4];

			int			upLineType;	//高度曲线类型,3抛物线（设为抛物线之后，速度曲线将不再起作用）
			int			sideLineType;//摆动曲线类型
			float		upMax;	//最大高度
			float		sideMax;//最大摆动幅度
			DWORD		dwReserve[12];

			tagTargetProp()
			{
				speedType=0;
				time=-1;
				startPt=Vector3(0,0,0);
				endPt=Vector3(1000,0,0);
				speed=200;

				upLineType=sideLineType=0;
				upMax=sideMax=200;

				memset(reservePt,0,sizeof(reservePt));
				memset(dwReserve,0,sizeof(dwReserve));
			}
		};
	public:
		EffectTarget(void);
		virtual ~EffectTarget(void);

		//--序列化接口
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);
		//--EffectBase
		virtual void Update(SceneGraph* pSG,SGEffectNode *pSGNode,CameraBase *pCamera,float deltaTime,float runTime);
		virtual void ResetPlayState();
		virtual const TCHAR* GetName() const;
		virtual void Rename(const TCHAR* szNewName);
		virtual bool GetBox(AABBox& out) const;
		virtual int GetLOD() const;

		//--EffectPathCtrl
		virtual const Vector3& GetCurPos() const;
		virtual const Vector3& GetCurVel() const;
		virtual const Vector3& GetCurRot() const;

		void SetProperty(const tagTargetProp& prop);
		tagTargetProp& GetProperty();
		void ResetTarget(const Vector3& startPt,const Vector3& targetPt);
		void ChangeTargetPt(const Vector3& targetPt);

		// 是否到达终点
		bool IsArrive();

	private:
		class Member;
		Member *m_p;

		DECL_RTTI_DYNC(EffectTarget);
	};
}//namespace Cool3D