#pragma once
#include "NavColliderPublicDef.h"

namespace WorldBase
{
	class NavCollider;
	class NavCarrierNode;
	/** \class NavCollider_MoveOnCarrier
		\brief 载具上移动碰撞检测器
	*/
	class WORLD_BASE_API NavCollider_MoveOnCarrier
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
			DWORD		dwCarrierObjID;	//载具ObjID
			Vector3		localStartPos;	//移动起点
			Vector3		localMoveDir;	//xz平面移动方向
			Vector3		roleSize;		//角色尺寸
			float		moveSpeed;		//移动速度
			int			maxCDTimes;		//最大碰撞次数
		};

		struct tagOutput				//输出参数和返回值
		{
			ENavResult	result;			//返回值
			Vector3		outPos;			//角色位置
			float		endTime;		//结束时间
			DWORD		dwCarrierObjID;	//载具ObjID
		};
	
	public:
		NavCollider_MoveOnCarrier(void);
		~NavCollider_MoveOnCarrier(void);

		/** \初始化
		*/
		void Init(DWORD dwCarrierObjID,const Vector3& localStartPos,const Vector3& localMoveDir,const Vector3& roleSize,float moveSpeed,int maxCDTimes=100);
		/** \移动
		*/
		ENavResult Update(NavCollider* pCollider,float startTime,float deltaTime,Vector3& outPos,float& endTime,DWORD& dwCarrierObjID);

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