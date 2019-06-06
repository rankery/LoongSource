#pragma once
#include "NavColliderPublicDef.h"

namespace WorldBase
{
	class NavCollider;
	/** \class NavCollider_Slide
		\brief 滑动碰撞检测器
	*/
	class WORLD_BASE_API NavCollider_Slide
	{
		enum EState						//状态
		{
			EState_None,				//未进行任何滑动操作
			EState_Initialized,			//已初始化
			EState_Sliding,				//滑动中
			EState_WaitForFinish,		//等待滑动到最后一步
			EState_Finished,			//滑动完成
		};

		struct tagInput					//输入参数
		{
			Vector3		startPos;		//滑动起点
			Vector3		roleSize;		//角色尺寸
			int			maxCDTimes;		//最大碰撞次数
		};

		struct tagOutput				//输出参数和返回值
		{
			ENavResult	result;			//返回值
			Vector3		outPos;			//角色位置
			float		endTime;		//结束时间
		};

	public:
		NavCollider_Slide(void);
		~NavCollider_Slide(void);

		/** \初始化
		*/
		void Init(const Vector3& startPos,const Vector3& roleSize,int maxCDTimes=100);
		/** \滑动
		*/
		ENavResult Update(NavCollider* pCollider,float startTime,float deltaTime,Vector3& outPos,float& endTime,bool bOnWaterCheck=false);

	private:
		tagInput	m_input;			//输入参数
		tagOutput	m_output;			//输出参数和返回值

		EState		m_state;			//当前状态

		class Collider;
		class SlideCtrl;

		friend class Collider;
		friend class SlideCtrl;

		Collider*	m_pCollider;
		SlideCtrl*	m_pSlideCtrl;
	};
}//namespace WorldBase