#pragma once
#include "NavColliderPublicDef.h"

namespace WorldBase
{
	class NavCollider;
	/** \class NavCollider_Move
		\brief 移动碰撞检测器
	*/
	class WORLD_BASE_API NavCollider_Move
	{
		enum EState						//状态
		{
			EState_None,				//未进行任何移动操作
			EState_Initialized,			//已初始化
			EState_Moving,				//移动中
			EState_WaitForFinish,		//等待移动到最后一步
			EState_Finished,			//移动完成
		};

		struct tagInput					//输入参数
		{
			Vector3		startPos;		//移动起点
			Vector3		endPos;			//移动终点
			Vector3		moveDir;		//xz平面移动方向
			Vector3		roleSize;		//角色尺寸
			float		moveSpeed;		//移动速度
			float		xzDist;			//xz平面距离
			int			maxCDTimes;		//最大碰撞次数
			bool		bPassCheck;		//是否进行通过性检测
			DWORD		dwCDBoxTopFlags;//碰撞盒子顶部标志位
			DWORD		dwCDBoxFlags;	//碰撞盒子标志位
		};

		struct tagOutput				//输出参数和返回值
		{
			ENavResult	result;			//返回值
			Vector3		outPos;			//角色位置
			float		endTime;		//结束时间
			DWORD		dwCarrierObjID;	//载具ObjID
		};

	public:
		NavCollider_Move(void);
		~NavCollider_Move(void);

		/** 初始化
		*/
		void Init(const Vector3& startPos,const Vector3& endPos,const Vector3& roleSize,float moveSpeed,bool bPassCheck=true,bool bIgnoreCarrier=false,int maxCDTimes=100);
		/** 移动
		*/
		ENavResult Update(NavCollider* pCollider,float startTime,float deltaTime,Vector3& outPos,float& endTime,DWORD& dwCarrierObjID,bool bOnWaterCheck=false);
		/** 移动测试，忽略载具
		*/
		ENavResult IfCanGo(NavCollider* pCollider,const Vector3& startPos,const Vector3& endPos,const Vector3& roleSize,Vector3* pOutPos,int maxCDTimes=100,bool bIgnoreCarrier=true,bool bMustSupportPathFind=false);

	private:
		tagInput	m_input;			//输入参数
		tagOutput	m_output;			//输出参数和返回值

		EState		m_state;			//状态

		class Collider;
		class MoveCtrl;

		friend class Collider;
		friend class MoveCtrl;

		Collider* m_pCollider;
		MoveCtrl* m_pMoveCtrl;
	};
}//namespace WorldBase