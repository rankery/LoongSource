#include "StdAfx.h"
#include "NavCollider_Swim.h"
#include "NavColliderPrivateDef.h"
#include "NavCollider.h"
#include "NavMovePath.h"
#include "NavSceneNode.h"


namespace WorldBase
{
	//--class NavCollider_Swim::Collider------------------------------------------------------------
	class NavCollider_Swim::Collider
	{
	public:
		struct tagCDResult						//碰撞结果
		{
			ENavResult		result;				//碰撞返回值
			NavMovePath		path;				//游泳路径
		};
		
	public:
		NavCollider_Swim*	m_p;
		tagCDResult			m_cdResult;
		float				m_xzSwimdDist;
		Vector3				m_stepStart;

	public:
		Collider(NavCollider_Swim* pParent)
			:m_p(pParent)
		{}
		~Collider()
		{}

		const tagCDResult& GetResult()
		{
			return m_cdResult;
		}

		void Init()
		{
			m_xzSwimdDist=0.0f;
			m_stepStart=m_p->m_input.startPos;
		}

		bool Collide( NavCollider* pCD,float startTime,float deltaTime,bool bWaterWalk )
		{
			const Vector3& roleSize		= m_p->m_input.roleSize;
			const Vector3 halfSize		= m_p->m_input.roleSize*0.5f;
			const Vector3& dir			= m_p->m_input.swimDir;
			const Vector3& end			= m_p->m_input.endPos;
			const float speed			= m_p->m_input.swimSpeed;
			const float xzDist			= m_p->m_input.xzDist;
			const int maxCDTimes		= m_p->m_input.maxCDTimes;
			const bool bPassCheck		= m_p->m_input.bPassCheck;
			const DWORD dwCDBoxTopFlags	= m_p->m_input.dwCDBoxTopFlags;
			const DWORD dwCDBoxFlags	= m_p->m_input.dwCDBoxFlags;
	
			m_cdResult.path.Clear();
			m_cdResult.path.SetStartPos(m_stepStart);

			float swimdTime=0.0f;
			bool bArrive=false;
			for(int i=0;;++i)
			{
				/////////////////////////////////////////////////////////////////////////////////////
				// 在xz平面游泳一个步长
				Vector3 stepEnd=m_stepStart+dir*g_collideStep;

				/////////////////////////////////////////////////////////////////////////////////////
				// 计算游泳（或水上行走）时脚底的高度
				if( !bWaterWalk )
					stepEnd.y=pCD->CalcSwimFootHeight(stepEnd,roleSize.y,stepEnd.y);
				else
					stepEnd.y=pCD->GetWaterHeightEx(stepEnd,stepEnd.y);

				/////////////////////////////////////////////////////////////////////////////////////
				// 判断是否到达终点
				m_xzSwimdDist+=g_collideStep;
				if(m_xzSwimdDist>=xzDist)
				{
					stepEnd.x=end.x;
					stepEnd.z=end.z;
					bArrive=true;
				}

				/////////////////////////////////////////////////////////////////////////////////////
				// 计算在走完当前步需要的时间
				float stepTime=Vec3Dist(m_stepStart,stepEnd)/speed;

				/////////////////////////////////////////////////////////////////////////////////////
				// 检查可行走区域
				if(!pCD->IfCanGo(stepEnd.x,stepEnd.z))
				{
					m_cdResult.result=ENR_Blocking;
					return true;
				}

				/////////////////////////////////////////////////////////////////////////////////////
				// 生成角色AABB
				AABBox box;
				box.min.x=stepEnd.x-halfSize.x;
				box.max.x=stepEnd.x+halfSize.x;
				box.min.z=stepEnd.z-halfSize.z;
				box.max.z=stepEnd.z+halfSize.z;

				/////////////////////////////////////////////////////////////////////////////////////
				// 快速判断角色所在区域是否有模型
				bool bHaveModel=pCD->HaveModel(box.min.x,box.min.z,box.max.x,box.max.z);

				/////////////////////////////////////////////////////////////////////////////////////
				// 取得地面高度
				float groundH=pCD->SafeGetGroundHeightSlerp(stepEnd.x,stepEnd.z);

				if(bPassCheck)
				{
					/////////////////////////////////////////////////////////////////////////////////
					// 检查是否碰到障碍物
					box.min.y=stepEnd.y;
					box.max.y=box.min.y+roleSize.y;
					if(pCD->AABBoxCollideBoxAndTrn(box,0.0f,groundH,dwCDBoxFlags))
					{
						/////////////////////////////////////////////////////////////////////////////
						// 检查是否碰到地面或盒子顶部
						box.max.y=stepEnd.y+g_moveFall;
						box.min.y=stepEnd.y;
						bool bOnGround=false;
						if(pCD->AABBoxCollideBoxTopAndTrn(box,0.0f,groundH,stepEnd.y,&bOnGround,dwCDBoxTopFlags))
						{
							/////////////////////////////////////////////////////////////////////////
							// 判断落差
							if(stepEnd.y-m_stepStart.y>g_moveFall)
							{
								m_cdResult.result=ENR_Blocking;
								return true;
							}

							/////////////////////////////////////////////////////////////////////////
							// 检查落脚点是否可通过
							box.min.y=stepEnd.y+10.0f;
							box.max.y=box.min.y+roleSize.y-10.0f;
							if(!pCD->AABBoxCollideBoxAndTrn(box,0.0f,groundH,dwCDBoxFlags))
							{
								m_cdResult.path.AddPoint(stepEnd);
								m_cdResult.result=ENR_SwimLanded;
								return true;
							}
						}

						m_cdResult.result=ENR_Blocking;
						return true;
					}
				}

				if(bArrive)
				{
					m_cdResult.path.AddPoint(stepEnd);
					m_cdResult.result=ENR_Arrive;
					return true;
				}

				/////////////////////////////////////////////////////////////////////////////////////
				// 判断时间是否到了？
				if(swimdTime+stepTime>=deltaTime
					||i>=maxCDTimes)
				{
					m_cdResult.path.AddPoint(stepEnd);
					m_cdResult.result=ENR_ToBeContinued;
					
					m_stepStart=stepEnd;
					return false;
				}

				////////////////////////////////////////////////////////////////////////////////////
				// 放入一个路径点
				m_cdResult.path.AddPoint(stepEnd);

				swimdTime+=stepTime;
				m_stepStart=stepEnd;
			}

			//不可能执行到这里！
			ASSERT(0);
			return false;
		}	
	};


	//--class NavCollider_Swim::SwimCtrl------------------------------------------------------------
	class NavCollider_Swim::SwimCtrl
	{
	public:
		NavCollider_Swim*		m_p;

		float					m_totalTime;		//总游泳时间
		float					m_swimdTime;		//已游泳时间
		float					m_endTime;			//游泳结束时间
		Vector3					m_endPos;			//游泳结束位置
		bool					m_bFinished;		//游泳完成标志

	public:
		SwimCtrl(NavCollider_Swim* pParent)
			:m_p(pParent)
		{}
		~SwimCtrl()
		{}

		bool SwimStart(const Collider::tagCDResult& cdResult,float startTime,float deltaTime,float& swimTime,float& endTime,Vector3& out)
		{
			//--计算总游泳时间
			m_totalTime=cdResult.path.GetDist()/m_p->m_input.swimSpeed;
	
			//--初始化已游泳时间
			m_swimdTime=0.0f;

			//--清空游泳完成标志
			m_bFinished=false;

			//--调用游泳Update
			return SwimUpdate(cdResult,startTime,deltaTime,swimTime,endTime,out);
		}

		bool SwimUpdate(const Collider::tagCDResult& cdResult,float startTime,float deltaTime,float& swimTime,float& endTime,Vector3& out)
		{
			if(m_bFinished)
			{
				swimTime=0.0f;
				endTime=m_endTime;
				out=m_endPos;
				return true;
			}

			float leaveTime=m_totalTime-m_swimdTime;
			if(deltaTime>=leaveTime)
			{
				swimTime=leaveTime;

				endTime=startTime+swimTime;

				out=cdResult.path.Back();

				m_bFinished=true;
				m_endTime=endTime;
				m_endPos=out;
				return true;
			}
			else
			{
				m_swimdTime+=deltaTime;

				swimTime=deltaTime;

				float factor;
				if(m_totalTime>0)
					factor=m_swimdTime/m_totalTime;
				else
					factor=0.0f;

				cdResult.path.SlerpPos(factor,out);
				return false;
			}
		}
	};


	//--class NavCollider_Swim----------------------------------------------------------------------
	NavCollider_Swim::NavCollider_Swim(void):m_state(EState_None)
	{
		m_pCollider=new Collider(this);
		m_pSwimCtrl=new SwimCtrl(this);
	}

	NavCollider_Swim::~NavCollider_Swim(void)
	{
		SAFE_DELETE(m_pCollider);
		SAFE_DELETE(m_pSwimCtrl);
	}

	void NavCollider_Swim::Init( const Vector3& startPos,const Vector3& endPos,const Vector3& roleSize,float swimSpeed,bool bPassCheck,int maxCDTimes )
	{
		//--检查输入参数
		ASSERT(swimSpeed>0.0f);
		ASSERT(maxCDTimes>0);

		//--保存输入参数
		m_input.startPos		= startPos;
		m_input.endPos			= endPos;
		m_input.roleSize		= roleSize;
		m_input.swimSpeed		= swimSpeed;
		m_input.bPassCheck		= bPassCheck;
		m_input.maxCDTimes		= maxCDTimes;

		m_input.dwCDBoxTopFlags=0;
		m_input.dwCDBoxTopFlags|=NavSceneNode::EFlag_NotCarrier;

		m_input.dwCDBoxFlags=0;
		m_input.dwCDBoxFlags|=NavSceneNode::EFlag_NotCarrier;

		//--计算xz平面方向
		m_input.swimDir			= endPos-startPos;
		m_input.swimDir.y		= 0.0f;

		//--计算xz平面距离
		m_input.xzDist			= Vec3Len(&m_input.swimDir);

		//--归一化xz平面方向
		if(m_input.xzDist>0.0f)
			m_input.swimDir/=m_input.xzDist;
		else
			m_input.swimDir=MathConst::Zero3;

		//--初始化m_output
		ZeroMemory(&m_output,sizeof(m_output));

		//--
		m_pCollider->Init();

		//--设置状态
		m_state					= EState_Initialized;
	}

	ENavResult NavCollider_Swim::Update( NavCollider* pCollider,float startTime,float deltaTime,Vector3& outPos,float& endTime,bool bWaterWalk/*=false*/ )
	{
		const Collider::tagCDResult& cdResult=m_pCollider->GetResult();

		switch(m_state)
		{
		case EState_Initialized:
			{
				//--检查xz距离
				if(m_input.xzDist==0.0f)
				{
					m_state=EState_Finished;
					m_output.result=ENR_Arrive;
					m_output.endTime=startTime;
					m_output.outPos=m_input.startPos;

					//--需要给返回值赋值
					outPos=m_output.outPos;
					endTime=m_output.endTime;

					return ENR_Arrive;
				}

				//--碰撞
				bool bCDOk=m_pCollider->Collide(pCollider,startTime,deltaTime,bWaterWalk);
				if(bCDOk)
				{
					m_output.result=cdResult.result;
				}

				//--开始游泳
				float swimTime=0.0f;
				bool bSwimOk=m_pSwimCtrl->SwimStart(cdResult,startTime,deltaTime,swimTime,m_output.endTime,m_output.outPos);
			
				//--设置状态
				if(bCDOk&&bSwimOk)
				{
					m_state=EState_Finished;
					outPos=m_output.outPos;
					endTime=m_output.endTime;
					return m_output.result;
				}
				else if(bCDOk&&!bSwimOk)
				{
					m_state=EState_WaitForFinish;
					outPos=m_output.outPos;
					return ENR_ToBeContinued;
				}
				else
				{
					m_state=EState_Swimming;
					outPos=m_output.outPos;
					return ENR_ToBeContinued;
				}
			}
			break;
		case EState_Swimming:
			{
				//--游泳更新
				float swimTime=0.0f;
				bool bSwimOk=m_pSwimCtrl->SwimUpdate(cdResult,startTime,deltaTime,swimTime,m_output.endTime,m_output.outPos);
				if(!bSwimOk)
				{
					outPos=m_output.outPos;
					return ENR_ToBeContinued;
				}

				startTime+=swimTime;
				deltaTime-=swimTime;

				//--碰撞
				bool bCDOk=m_pCollider->Collide(pCollider,startTime,deltaTime,bWaterWalk);
				if(bCDOk)
				{
					m_output.result=cdResult.result;
				}

				//--开始游泳
				bSwimOk=m_pSwimCtrl->SwimStart(cdResult,startTime,deltaTime,swimTime,m_output.endTime,m_output.outPos);

				//--设置状态
				if(bCDOk&&bSwimOk)
				{
					m_state=EState_Finished;
					outPos=m_output.outPos;
					endTime=m_output.endTime;
					return m_output.result;
				}
				else if(bCDOk&&!bSwimOk)
				{
					m_state=EState_WaitForFinish;
					outPos=m_output.outPos;
					return ENR_ToBeContinued;
				}
				else
				{
					m_state=EState_Swimming;
					outPos=m_output.outPos;
					return ENR_ToBeContinued;
				}
			}
			break;
		case EState_WaitForFinish:
			{
				//--游泳更新
				float swimTime=0.0f;
				bool bSwimOk=m_pSwimCtrl->SwimUpdate(cdResult,startTime,deltaTime,swimTime,m_output.endTime,m_output.outPos);
				if(!bSwimOk)
				{
					outPos=m_output.outPos;
					return ENR_ToBeContinued;
				}
				
				//--设置状态
				m_state=EState_Finished;
				outPos=m_output.outPos;
				endTime=m_output.endTime;
				return m_output.result;
			}
			break;
		case EState_Finished:
			{
				outPos=m_output.outPos;
				endTime=m_output.endTime;
				return m_output.result;
			}
			break;
		}

		ASSERT(0);
		return ENR_Failed;
	}

	ENavResult NavCollider_Swim::IfCanGo( NavCollider* pCollider,const Vector3& startPos,const Vector3& endPos,const Vector3& roleSize,Vector3* pOutPos,int maxCDTimes/*=100*/ )
	{
		const float speed=500.0f;
		const float timeOut=1000000.0f;

		m_input.startPos		= startPos;
		m_input.endPos			= endPos;
		m_input.roleSize		= roleSize;
		m_input.swimSpeed		= speed;
		m_input.bPassCheck		= true;
		m_input.maxCDTimes		= maxCDTimes;

		m_input.dwCDBoxTopFlags=0;
		m_input.dwCDBoxTopFlags|=NavSceneNode::EFlag_NotCarrier;

		m_input.dwCDBoxFlags=0;
		m_input.dwCDBoxFlags|=NavSceneNode::EFlag_NotCarrier;

		//--计算xz平面方向
		m_input.swimDir			= endPos-startPos;
		m_input.swimDir.y		= 0.0f;

		//--计算xz平面距离
		m_input.xzDist			= Vec3Len(&m_input.swimDir);

		//--归一化xz平面方向
		if(m_input.xzDist>0.0f)
			m_input.swimDir/=m_input.xzDist;
		else
			m_input.swimDir=MathConst::Zero3;

		//--初始化m_output
		ZeroMemory(&m_output,sizeof(m_output));

		//--碰撞检测s
		const Collider::tagCDResult& cdResult=m_pCollider->GetResult();

		m_pCollider->Init();
		if(!m_pCollider->Collide(pCollider,0.0f,timeOut,false))
		{
			if(pOutPos)*pOutPos=cdResult.path.Back();
			return ENR_ToBeContinued;
		}

		if(pOutPos)*pOutPos=cdResult.path.Back();
		return cdResult.result;
	}
}//namespace WorldBase