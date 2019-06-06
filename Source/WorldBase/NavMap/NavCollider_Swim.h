#pragma once
#include "NavColliderPublicDef.h"

namespace WorldBase
{
	class NavCollider;
	/** \class NavCollider_Swim
		\brief 游泳碰撞检测器
	*/
	class WORLD_BASE_API NavCollider_Swim
	{
		enum EState						//状态
		{
			EState_None,				//未进行任何游泳操作
			EState_Initialized,			//已初始化
			EState_Swimming,			//游泳中
			EState_WaitForFinish,		//等待游泳到最后一步
			EState_Finished,			//游泳完成
		};

		struct tagInput					//输入参数
		{
			Vector3		startPos;		//游泳起点
			Vector3		endPos;			//游泳终点
			Vector3		swimDir;		//xz平面游泳方向
			Vector3		roleSize;		//角色尺寸
			float		swimSpeed;		//游泳速度
			float		xzDist;			//xz平面距离
			int			maxCDTimes;		//最大碰撞次数
			bool		bPassCheck;		//是否进行通过性检测
			DWORD		dwCDBoxTopFlags;//碰撞标志位
			DWORD		dwCDBoxFlags;	//碰撞标志位
		};

		struct tagOutput				//输出参数和返回值
		{
			ENavResult	result;			//返回值
			Vector3		outPos;			//角色位置
			float		endTime;		//结束时间
		};

	public:
		NavCollider_Swim(void);
		~NavCollider_Swim(void);

		/** \初始化
		*/
		void Init(const Vector3& startPos,const Vector3& endPos,const Vector3& roleSize,float swimSpeed,bool bPassCheck=true,int maxCDTimes=100);
		/** \游泳
		*/
		ENavResult Update(NavCollider* pCollider,float startTime,float deltaTime,Vector3& outPos,float& endTime,bool bWaterWalk=false);
		/** 移动测试，忽略载具
		*/
		ENavResult IfCanGo(NavCollider* pCollider,const Vector3& startPos,const Vector3& endPos,const Vector3& roleSize,Vector3* pOutPos,int maxCDTimes=100);

	private:
		tagInput	m_input;			//输入参数
		tagOutput	m_output;			//输出参数和返回值

		EState		m_state;			//状态

		class Collider;
		class SwimCtrl;

		friend class Collider;
		friend class SwimCtrl;

		Collider* m_pCollider;
		SwimCtrl* m_pSwimCtrl;
	};
}//namespace WorldBase