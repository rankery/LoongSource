#pragma once
#include "NavColliderPublicDef.h"

namespace WorldBase
{
	class NavCollider;
	class NavCarrierNode;
	/** \class NavCollider_Jump
		\brief 跳跃碰撞检测器
	*/
	class WORLD_BASE_API NavCollider_Jump
	{
		enum EState						//状态
		{
			EState_None,				//未进行任何移动操作
			EState_Initialized,			//已初始化
			EState_Jumpping,			//跳跃中
			EState_WaitForFinish,		//等待跳跃完成最后一步
			EState_Finished,			//跳跃完成
		};

		struct tagInput					//输入参数
		{
			Vector3		startPos;		//移动起点
			Vector3		xzDir;			//xz平面跳跃方向
			Vector3		roleSize;		//角色尺寸
			float		xzSpeed;		//xz平移速度
			float		ySpeed;			//y初速度
			float		g;				//重力加速度
			float		cdTimeOut;		//碰撞超时
			DWORD		cdBoxTopFlags;	//碰撞标志位
			DWORD		cdBoxFlag;		//碰撞标志位
		};

		struct tagOutput				//输出参数和返回值
		{
			ENavResult	result;			//返回值
			Vector3		outPos;			//角色位置
			float		endTime;		//结束时间
			float		cdCarrierTime;	//碰撞到载具的时间
			DWORD		dwCarrierObjID;	//载具ObjID
		};

	public:
		NavCollider_Jump(void);
		~NavCollider_Jump(void);

		/** 初始化
		*/
		void Init(const Vector3& startPos,const Vector3& xzDir,const Vector3& roleSize,float xzSpeed,float ySpeed,float g=1200.0f,float cdTimeOut=60.0f,bool bTrnOnly=false,bool bIgnoreCarrier=false);
		/** 跳跃
		*/
		ENavResult Update(NavCollider* pCollider,float startTime,float deltaTime,BitMap* pSpecCangoMap,Vector3& outPos,float& endTime,DWORD& dwCarrierObjID,float& cdCarrierTime,bool bOnWaterCheck=false);

	private:
		tagInput	m_input;			//输入参数
		tagOutput	m_output;			//输出参数和返回值

		EState		m_state;			//状态

		class Collider;
		class JumpCtrl;

		friend class Collider;
		friend class JumpCtrl;

		Collider* m_pCollider;
		JumpCtrl* m_pJumpCtrl;
	};
}//namespace WorldBase