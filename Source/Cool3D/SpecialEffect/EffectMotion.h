#pragma once
#include "effectpathctrl.h"

namespace Cool3D
{
	/**	\class EffectMotion 
		\brief 使用速度,旋转来产生运动路径的特效路径控制
	*/
	class Cool3D_API EffectMotion : public EffectPathCtrl
	{
	public:
		struct tagMotionProp : public tagProp
		{
			Vector3		initPos;	//初始位置
			bool        bCalcRot;   //是否计算朝向
			BYTE		byReserve[3];
			DWORD		dwReserve[7];
			
			tagMotionProp():initPos(0,0,0)
			{
				memset(dwReserve,0,sizeof(dwReserve));
			}
		};

		struct tagState
		{
			float	lastTime;	//这个状态持续的时间,可以用来与下一状态进行插值
			Vector3 vel;		//移动速度
			Vector3 revVel;		//公转速度

			DWORD	dwReserve[8];

			tagState():lastTime(1),vel(0,0,0),revVel(0,0,0)
			{
				memset(dwReserve,0,sizeof(dwReserve));
			}
		};
	public:
		EffectMotion(void);
		virtual ~EffectMotion(void);

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

		//--
		//编辑器数据导出接口
		tagMotionProp GetProp() const;
		int GetNumStates() const;
		tagState GetState(int i) const;
		//创建接口
		void BeginBuild(const tagMotionProp& prop);	//基本属性
		void AddKeyState(const tagState& state);
		void EndBuild();

	private:
		class Member;
		Member *m_p;

		DECL_RTTI_DYNC(EffectMotion);
	};
}//namespace Cool3D