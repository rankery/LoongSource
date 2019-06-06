#include "StdAfx.h"
#include "NavCollider_Move.h"
#include "NavColliderPrivateDef.h"
#include "NavCollider.h"
#include "NavMovePath.h"
#include "NavSceneNode.h"


namespace WorldBase
{
	//--class NavCollider_Move::Collider------------------------------------------------------------
	class NavCollider_Move::Collider
	{
	public:
		struct tagCDResult						//碰撞结果
		{
			ENavResult		result;				//碰撞返回值
			NavMovePath		path;				//移动路径

			struct tagCarry
			{
				DWORD		dwCarrierObjID;
				D3DVECTOR	carryPos;

				void Fill(DWORD _dwCarrierObjID,const Vector3& _carryPos)
				{
					dwCarrierObjID		= _dwCarrierObjID;
					carryPos			= _carryPos;
				}
			};

			union 
			{
				tagCarry	carry;
			};
		};
		
	public:
		NavCollider_Move*	m_p;
		tagCDResult			m_cdResult;
		float				m_xzMovedDist;
		Vector3				m_stepStart;

	public:
		Collider(NavCollider_Move* pParent)
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
			m_xzMovedDist=0.0f;
			m_stepStart=m_p->m_input.startPos;
		}

		bool Collide( NavCollider* pCD,float startTime,float deltaTime,bool bOnWaterCheck )
		{
			const Vector3& roleSize		= m_p->m_input.roleSize;
			const Vector3 halfSize		= m_p->m_input.roleSize*0.5f;
			const Vector3& dir			= m_p->m_input.moveDir;
			const Vector3& end			= m_p->m_input.endPos;
			const float speed			= m_p->m_input.moveSpeed;
			const float xzDist			= m_p->m_input.xzDist;
			const int maxCDTimes		= m_p->m_input.maxCDTimes;
			const bool bPassCheck		= m_p->m_input.bPassCheck;
			const DWORD dwCDBoxTopFlags	= m_p->m_input.dwCDBoxTopFlags;
			const DWORD dwCDBoxFlags	= m_p->m_input.dwCDBoxFlags;
	
			m_cdResult.path.Clear();
			m_cdResult.path.SetStartPos(m_stepStart);

			float movedTime=0.0f;
			bool bArrive=false;
			for(int i=0;;++i)
			{
				/////////////////////////////////////////////////////////////////////////////////////
				// 在xz平面移动一个步长
				Vector3 xzStepEnd=m_stepStart+dir*g_collideStep;

				/////////////////////////////////////////////////////////////////////////////////////
				// 判断是否到达终点
				m_xzMovedDist+=g_collideStep;
				if(m_xzMovedDist>=xzDist)
				{
					xzStepEnd.x=end.x;
					xzStepEnd.z=end.z;
					bArrive=true;
				}

				/////////////////////////////////////////////////////////////////////////////////////
				// 计算在xz平面走完当前步需要的时间
				float xzStepTime;
				if(bArrive)
					xzStepTime=Vec3Dist(m_stepStart,xzStepEnd)/speed;
				else
					xzStepTime=g_collideStep/speed;

				/////////////////////////////////////////////////////////////////////////////////////
				// 检查可行走区域
				if(!pCD->IfCanGo(xzStepEnd.x,xzStepEnd.z))
				{
					m_cdResult.result=ENR_Blocking;
					return true;
				}

				/////////////////////////////////////////////////////////////////////////////////////
				// 生成角色AABB
				AABBox box;
				box.min.x=xzStepEnd.x-halfSize.x;
				box.max.x=xzStepEnd.x+halfSize.x;
				box.min.z=xzStepEnd.z-halfSize.z;
				box.max.z=xzStepEnd.z+halfSize.z;

				/////////////////////////////////////////////////////////////////////////////////////
				// 快速判断角色所在区域是否有模型
				bool bHaveModel=pCD->HaveModel(box.min.x,box.min.z,box.max.x,box.max.z);

				/////////////////////////////////////////////////////////////////////////////////////
				// 取得地面高度
				float groundH=pCD->SafeGetGroundHeightSlerp(xzStepEnd.x,xzStepEnd.z);

				/////////////////////////////////////////////////////////////////////////////////////
				// 找出落脚点
				Vector3 stepEnd=xzStepEnd;
				bool bOnGround=false;
				DWORD dwCarrierObjID=-1;
				if(bHaveModel)
				{
					box.max.y=xzStepEnd.y+g_moveFall;
					box.min.y=-g_floatMax;
					if(!pCD->AABBoxCollideBoxTopAndTrn(box,startTime+movedTime+xzStepTime,groundH,stepEnd.y,&bOnGround,dwCDBoxTopFlags,&dwCarrierObjID))
					{
						m_cdResult.result=ENR_Blocking;
						return true;
					}
				}
				else
				{
					bOnGround=true;
					stepEnd.y=groundH;
				}

				/////////////////////////////////////////////////////////////////////////////////////
				// 计算走完当前步需要的时间
				float stepTime=Vec3Dist(m_stepStart,stepEnd)/speed;

				if(bPassCheck)
				{
					////////////////////////////////////////////////////////////////////////////////
					// 判断落差
					if(fabs(m_stepStart.y-stepEnd.y)>g_moveFall)
					{
						if(stepEnd.y>m_stepStart.y)
						{
							m_cdResult.result=ENR_Blocking;
							return true;
						}
						else
						{
							////////////////////////////////////////////////////////////////////////
							// 检查是否被挡住
							box.max.y=xzStepEnd.y+roleSize.y;
							box.min.y=xzStepEnd.y;
							if(pCD->AABBoxCollideBox(box,startTime+movedTime+stepTime,dwCDBoxFlags))
							{
								m_cdResult.result=ENR_Blocking;
								return true;
							}
							else
							{
								m_cdResult.path.AddPoint(xzStepEnd);
								m_cdResult.result=ENR_WillDrop;
								return true;
							}
						}
					}

					////////////////////////////////////////////////////////////////////////////////
					// 检查落脚点是否能通过
					if(bHaveModel)
					{
						box.min.y=stepEnd.y+10.0f;
						box.max.y=box.min.y+roleSize.y-10.0f;
						if(pCD->AABBoxCollideBoxAndTrn(box,startTime+movedTime+stepTime,groundH,dwCDBoxFlags))
						{
							m_cdResult.result=ENR_Blocking;
							return true;
						}
					}

					////////////////////////////////////////////////////////////////////////////////
					// 判断是否入水
					if(bOnWaterCheck && pCD->IfWillOnWater(stepEnd))
					{
						m_cdResult.path.AddPoint(stepEnd);
						m_cdResult.result=ENR_WillOnWater;
						return true;
					}

					////////////////////////////////////////////////////////////////////////////////
					// 判断是否会游泳
					if(pCD->IfWillSwim(stepEnd,roleSize.y,stepEnd.y))
					{
						m_cdResult.path.AddPoint(stepEnd);
						m_cdResult.result=ENR_WillSwim;
						return true;
					}

					////////////////////////////////////////////////////////////////////////////////
					// 判断滑落
					if(bOnGround
						&&pCD->IfWillSlide(m_stepStart.x,m_stepStart.z))
					{
						m_cdResult.path.AddPoint(stepEnd);
						m_cdResult.result=ENR_WillSlide;
						return true;
					}
				}

				/////////////////////////////////////////////////////////////////////////////////////
				// 判断是否移动到载具上面？
				if(dwCarrierObjID!=-1)
				{
					m_cdResult.path.AddPoint(xzStepEnd);

					m_cdResult.result=ENR_WillCarry;
					m_cdResult.carry.Fill(dwCarrierObjID,stepEnd);
					return true;
				}

				if(bArrive)
				{
					m_cdResult.path.AddPoint(stepEnd);
					m_cdResult.result=ENR_Arrive;
					return true;
				}

				/////////////////////////////////////////////////////////////////////////////////////
				// 判断时间是否到了？
				if(movedTime+stepTime>=deltaTime
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

				movedTime+=stepTime;
				m_stepStart=stepEnd;
			}

			//不可能执行到这里！
			ASSERT(0);
			return false;
		}	
	};


	//--class NavCollider_Move::MoveCtrl------------------------------------------------------------
	class NavCollider_Move::MoveCtrl
	{
	public:
		NavCollider_Move*		m_p;

		float					m_totalTime;		//总移动时间
		float					m_movedTime;		//已移动时间
		float					m_endTime;			//移动结束时间
		Vector3					m_endPos;			//移动结束位置
		bool					m_bFinished;		//移动完成标志

	public:
		MoveCtrl(NavCollider_Move* pParent)
			:m_p(pParent)
		{}
		~MoveCtrl()
		{}

		bool MoveStart(const Collider::tagCDResult& cdResult,float startTime,float deltaTime,float& moveTime,float& endTime,Vector3& out)
		{
			//--计算总移动时间
			m_totalTime=cdResult.path.GetDist()/m_p->m_input.moveSpeed;
	
			//--初始化已移动时间
			m_movedTime=0.0f;

			//--清空移动完成标志
			m_bFinished=false;

			//--调用移动Update
			return MoveUpdate(cdResult,startTime,deltaTime,moveTime,endTime,out);
		}

		bool MoveUpdate(const Collider::tagCDResult& cdResult,float startTime,float deltaTime,float& moveTime,float& endTime,Vector3& out)
		{
			if(m_bFinished)
			{
				moveTime=0.0f;
				endTime=m_endTime;
				out=m_endPos;
				return true;
			}

			float leaveTime=m_totalTime-m_movedTime;
			if(deltaTime>=leaveTime)
			{
				moveTime=leaveTime;

				endTime=startTime+moveTime;

				out=cdResult.path.Back();
				if(cdResult.result==ENR_WillCarry)
					out=cdResult.carry.carryPos;

				m_bFinished=true;
				m_endTime=endTime;
				m_endPos=out;
				return true;
			}
			else
			{
				m_movedTime+=deltaTime;

				moveTime=deltaTime;

				float factor;
				if(m_totalTime>0)
					factor=m_movedTime/m_totalTime;
				else
					factor=0.0f;
				cdResult.path.SlerpPos(factor,out);
				return false;
			}
		}
	};


	//--class NavCollider_Move----------------------------------------------------------------------
	NavCollider_Move::NavCollider_Move(void):m_state(EState_None)
	{
		m_pCollider=new Collider(this);
		m_pMoveCtrl=new MoveCtrl(this);
	}

	NavCollider_Move::~NavCollider_Move(void)
	{
		SAFE_DELETE(m_pCollider);
		SAFE_DELETE(m_pMoveCtrl);
	}

	void NavCollider_Move::Init( const Vector3& startPos,const Vector3& endPos,const Vector3& roleSize,float moveSpeed,bool bPassCheck,bool bIgnoreCarrier,int maxCDTimes )
	{
		//--检查输入参数
		ASSERT(moveSpeed>0.0f);
		ASSERT(maxCDTimes>0);

		//--保存输入参数
		m_input.startPos		= startPos;
		m_input.endPos			= endPos;
		m_input.roleSize		= roleSize;
		m_input.moveSpeed		= moveSpeed;
		m_input.bPassCheck		= bPassCheck;
		m_input.maxCDTimes		= maxCDTimes;

		m_input.dwCDBoxTopFlags=0;
		if(bIgnoreCarrier)
			m_input.dwCDBoxTopFlags|=NavSceneNode::EFlag_NotCarrier;//忽略载具

		m_input.dwCDBoxFlags=0;
		if(bIgnoreCarrier)
			m_input.dwCDBoxFlags|=NavSceneNode::EFlag_NotCarrier;//忽略载具

		//--计算xz平面方向
		m_input.moveDir			= endPos-startPos;
		m_input.moveDir.y		= 0.0f;

		//--计算xz平面距离
		m_input.xzDist			= Vec3Len(&m_input.moveDir);

		//--归一化xz平面方向
		if(m_input.xzDist>0.0f)
			m_input.moveDir/=m_input.xzDist;
		else
			m_input.moveDir=MathConst::Zero3;

		//--初始化m_output
		ZeroMemory(&m_output,sizeof(m_output));
		m_output.dwCarrierObjID	= -1;

		//--设置状态
		m_state					= EState_Initialized;
	}

	ENavResult NavCollider_Move::Update( NavCollider* pCollider,float startTime,float deltaTime,Vector3& outPos,float& endTime,DWORD& dwCarrierObjID,bool bOnWaterCheck/*=false*/ )
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
					m_output.outPos=m_input.startPos;
					m_output.endTime=startTime;

					//--需要给返回值赋值
					outPos=m_output.outPos;
					endTime=m_output.endTime;
					dwCarrierObjID=m_output.dwCarrierObjID;

					return ENR_Arrive;
				}

				//--初始化碰撞检测器
				m_pCollider->Init();

				//--碰撞
				bool bCDOk=m_pCollider->Collide(pCollider,startTime,deltaTime,bOnWaterCheck);
				if(bCDOk)
				{
					m_output.result=cdResult.result;
					if(cdResult.result==ENR_WillCarry)
						m_output.dwCarrierObjID=cdResult.carry.dwCarrierObjID;
				}

				//--开始移动
				float moveTime=0.0f;
				bool bMoveOk=m_pMoveCtrl->MoveStart(cdResult,startTime,deltaTime,moveTime,m_output.endTime,m_output.outPos);
			
				//--设置状态
				if(bCDOk&&bMoveOk)
				{
					m_state=EState_Finished;
					outPos=m_output.outPos;
					endTime=m_output.endTime;
					dwCarrierObjID=m_output.dwCarrierObjID;
					return m_output.result;
				}
				else if(bCDOk&&!bMoveOk)
				{
					m_state=EState_WaitForFinish;
					outPos=m_output.outPos;
					return ENR_ToBeContinued;
				}
				else
				{
					m_state=EState_Moving;
					outPos=m_output.outPos;
					return ENR_ToBeContinued;
				}
			}
			break;
		case EState_Moving:
			{
				//--移动更新
				float moveTime=0.0f;
				bool bMoveOk=m_pMoveCtrl->MoveUpdate(cdResult,startTime,deltaTime,moveTime,m_output.endTime,m_output.outPos);
				if(!bMoveOk)
				{
					outPos=m_output.outPos;
					return ENR_ToBeContinued;
				}

				startTime+=moveTime;
				deltaTime-=moveTime;

				//--碰撞
				bool bCDOk=m_pCollider->Collide(pCollider,startTime,deltaTime,bOnWaterCheck);
				if(bCDOk)
				{
					m_output.result=cdResult.result;
					if(cdResult.result==ENR_WillCarry)
						m_output.dwCarrierObjID=cdResult.carry.dwCarrierObjID;
				}

				//--开始移动
				bMoveOk=m_pMoveCtrl->MoveStart(cdResult,startTime,deltaTime,moveTime,m_output.endTime,m_output.outPos);

				//--设置状态
				if(bCDOk&&bMoveOk)
				{
					m_state=EState_Finished;
					outPos=m_output.outPos;
					endTime=m_output.endTime;
					dwCarrierObjID=m_output.dwCarrierObjID;
					return m_output.result;
				}
				else if(bCDOk&&!bMoveOk)
				{
					m_state=EState_WaitForFinish;
					outPos=m_output.outPos;
					return ENR_ToBeContinued;
				}
				else
				{
					m_state=EState_Moving;
					outPos=m_output.outPos;
					return ENR_ToBeContinued;
				}
			}
			break;
		case EState_WaitForFinish:
			{
				//--移动更新
				float moveTime=0.0f;
				bool bMoveOk=m_pMoveCtrl->MoveUpdate(cdResult,startTime,deltaTime,moveTime,m_output.endTime,m_output.outPos);
				if(!bMoveOk)
				{
					outPos=m_output.outPos;
					return ENR_ToBeContinued;
				}
				
				//--设置状态
				m_state=EState_Finished;
				outPos=m_output.outPos;
				endTime=m_output.endTime;
				dwCarrierObjID=m_output.dwCarrierObjID;
				return m_output.result;
			}
			break;
		case EState_Finished:
			{
				outPos=m_output.outPos;
				endTime=m_output.endTime;
				dwCarrierObjID=m_output.dwCarrierObjID;
				return m_output.result;
			}
			break;
		}

		ASSERT(0);
		return ENR_Failed;
	}

	ENavResult NavCollider_Move::IfCanGo( NavCollider* pCollider,const Vector3& startPos,const Vector3& endPos,const Vector3& roleSize,Vector3* pOutPos,int maxCDTimes/*=100*/,bool bIgnoreCarrier,bool bMustSupportPathFind )
	{
		const float speed=500.0f;
		const float timeOut=1000000.0f;

		//--
		m_input.startPos		= startPos;
		m_input.endPos			= endPos;
		m_input.roleSize		= roleSize;
		m_input.moveSpeed		= speed;
		m_input.bPassCheck		= true;
		m_input.maxCDTimes		= maxCDTimes;

		m_input.dwCDBoxTopFlags=0;
		if(bIgnoreCarrier)
			m_input.dwCDBoxTopFlags|=NavSceneNode::EFlag_NotCarrier;//忽略载具
		if(bMustSupportPathFind)
			m_input.dwCDBoxTopFlags|=NavSceneNode::EFlag_SupportPlayerPathFind;//支持自动寻路

		m_input.dwCDBoxFlags=0;
		if(bIgnoreCarrier)
			m_input.dwCDBoxFlags|=NavSceneNode::EFlag_NotCarrier;//忽略载具
		
		//--计算xz平面方向
		m_input.moveDir			= endPos-startPos;
		m_input.moveDir.y		= 0.0f;

		//--计算xz平面距离
		m_input.xzDist			= Vec3Len(&m_input.moveDir);

		//--归一化xz平面方向
		if(m_input.xzDist>0.0f)
			m_input.moveDir/=m_input.xzDist;
		else
			m_input.moveDir=MathConst::Zero3;

		//--碰撞检测
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