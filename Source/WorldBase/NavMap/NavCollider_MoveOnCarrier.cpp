#include "StdAfx.h"
#include "NavCollider_MoveOnCarrier.h"
#include "NavColliderPrivateDef.h"
#include "NavCarrierNode.h"
#include "NavCollider.h"
#include "NavMovePath.h"


namespace WorldBase
{
	//--class NavCollider_MoveOnCarrier::Collider--------------------------------------------------
	class NavCollider_MoveOnCarrier::Collider
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

			struct tagLanded
			{
				D3DVECTOR	landPos;

				void Fill(const Vector3& _landPos)
				{
					landPos				= _landPos;
				}
			};

			union 
			{
				tagCarry	carry;
				tagLanded	landed;
			};
		};
	public:
		NavCollider_MoveOnCarrier*	m_p;
		tagCDResult					m_cdResult;
		Vector3						m_stepStart;

	public:
		Collider(NavCollider_MoveOnCarrier* pParent)
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
			m_stepStart=m_p->m_input.localStartPos;
		}

		bool Collide( NavCollider* pCD,NavCarrierNode* pCarrier,float startTime,float deltaTime )
		{
			const Vector3& roleSize		= m_p->m_input.roleSize;
			const Vector3 halfSize		= m_p->m_input.roleSize*0.5f;
			const Vector3& dir			= m_p->m_input.localMoveDir;
			const float speed			= m_p->m_input.moveSpeed;
			const int maxCDTimes		= m_p->m_input.maxCDTimes;

			m_cdResult.path.Clear();
			m_cdResult.path.SetStartPos(m_stepStart);

			float movedTime=0.0f;
			for(int i=0;;++i)
			{
				/////////////////////////////////////////////////////////////////////////////////////
				// 在xz平面移动一个步长
				Vector3 xzStepEnd=m_stepStart+dir*g_collideStep;

				/////////////////////////////////////////////////////////////////////////////////////
				// 计算在xz平面走完当前步需要的时间
				float xzStepTime=g_collideStep/speed;

				/////////////////////////////////////////////////////////////////////////////////////
				// 生成角色AABB
				AABBox box;
				box.min.x=xzStepEnd.x-halfSize.x;
				box.max.x=xzStepEnd.x+halfSize.x;
				box.min.z=xzStepEnd.z-halfSize.z;
				box.max.z=xzStepEnd.z+halfSize.z;

				/////////////////////////////////////////////////////////////////////////////////////
				// 与载具碰撞，取得落脚点作为步长终点
				box.max.y=xzStepEnd.y+g_moveFall;
				box.min.y=-g_floatMax;
				float intersectH=box.min.y;
				if(!pCarrier->AABBoxCollideBoxTopHighestInLocal(box,intersectH))
				{
					pCarrier->EnableCollide(false);
					CollideWorldOutOfCarrier(pCD,pCarrier,xzStepEnd,startTime+movedTime+xzStepTime);
					pCarrier->EnableCollide(true);

					return true;
				}

				Vector3 stepEnd=xzStepEnd;
				stepEnd.y=intersectH;

				/////////////////////////////////////////////////////////////////////////////////////
				// 计算走完当前步需要的时间
				float stepTime=Vec3Dist(m_stepStart,stepEnd)/speed;

				/////////////////////////////////////////////////////////////////////////////////////
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
						m_cdResult.path.AddPoint(xzStepEnd);
						m_cdResult.result=ENR_WillDrop;
						return true;
					}
				}

				/////////////////////////////////////////////////////////////////////////////////////
				// 检查落脚点是否可通过
				box.max.y=stepEnd.y+roleSize.y-10.0f;
				box.min.y=stepEnd.y+10.0f;
				if(pCarrier->AABBoxCollideBoxInLocal(box))
				{
					m_cdResult.result=ENR_Blocking;
					return true;
				}

				/////////////////////////////////////////////////////////////////////////////////////
				// 在载具上与场景碰撞
				{
					pCarrier->EnableCollide(false);
					bool bResult=CollideWorldOnCarrier(pCD,pCarrier,stepEnd,startTime+movedTime+stepTime);
					pCarrier->EnableCollide(true);

					if(bResult)
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

		void CollideWorldOutOfCarrier( NavCollider* pCD,NavCarrierNode* pCarrier,const Vector3& stepEnd,float curTime )
		{
			const Vector3 halfSize	= m_p->m_input.roleSize*0.5f;
			const Vector3& roleSize	= m_p->m_input.roleSize;

			Vector3 wpt;
			pCarrier->PosToWorld(stepEnd,curTime,wpt);

			//////////////////////////////////////////////////////////////////////////////////////////
			// 生成角色AABB
			AABBox box;
			box.min.x=wpt.x-halfSize.x;
			box.max.x=wpt.x+halfSize.x;
			box.min.z=wpt.z-halfSize.z;
			box.max.z=wpt.z+halfSize.z;

			//////////////////////////////////////////////////////////////////////////////////////////
			// 碰撞得到落脚点
			box.max.y=wpt.y+g_moveFall;
			box.min.y=-g_floatMax;

			float intersectH=0.0f;
			DWORD dwCarrierObjID=-1;
			if(!pCD->AABBoxCollideBoxTopAndTrn(box,curTime,intersectH,NULL,0,&dwCarrierObjID))
			{
				m_cdResult.result=ENR_Blocking;
				return;
			}

			//////////////////////////////////////////////////////////////////////////////////////////
			// 判断落差
			if(fabs(wpt.y-intersectH)>g_moveFall)
			{
				if(intersectH>wpt.y)
				{
					m_cdResult.result=ENR_Blocking;
					return;
				}
				else
				{
					/////////////////////////////////////////////////////////////////////////////////
					// 检查是否被挡住
					box.max.y=wpt.y+roleSize.y;
					box.min.y=wpt.y;
					if(pCD->AABBoxCollideBox(box,curTime))
					{
						m_cdResult.result=ENR_Blocking;
						return;
					}
					else
					{
						m_cdResult.path.AddPoint(stepEnd);
						m_cdResult.result=ENR_WillDrop;
						return;
					}
				}
			}

			//////////////////////////////////////////////////////////////////////////////////////////
			// 检查落脚点是否可通过
			box.max.y=intersectH+roleSize.y-10.0f;
			box.min.y=intersectH+10.0f;
			if(pCD->AABBoxCollideBox(box,curTime))
			{
				m_cdResult.result=ENR_Blocking;
				return;
			}

			//////////////////////////////////////////////////////////////////////////////////////////
			// 着陆到其它载具或静态表面
			wpt.y=intersectH;
			if(dwCarrierObjID==-1)
			{
				m_cdResult.path.AddPoint(stepEnd);
				m_cdResult.result=ENR_CarryLanded;
				m_cdResult.landed.Fill(wpt);
				return;
			}
			else
			{
				m_cdResult.path.AddPoint(stepEnd);
				m_cdResult.result=ENR_WillCarry;
				m_cdResult.carry.Fill(dwCarrierObjID,wpt);
				return;
			}
		}

		bool CollideWorldOnCarrier( NavCollider* pCD,NavCarrierNode* pCarrier,const Vector3& stepEnd,float curTime )
		{
			const Vector3 halfSize	= m_p->m_input.roleSize*0.5f;
			const Vector3& roleSize	= m_p->m_input.roleSize;

			Vector3 wpt;
			pCarrier->PosToWorld(stepEnd,curTime,wpt);

			//////////////////////////////////////////////////////////////////////////////////////////
			// 生成角色AABB
			AABBox box;
			box.min.x=wpt.x-halfSize.x;
			box.max.x=wpt.x+halfSize.x;
			box.min.z=wpt.z-halfSize.z;
			box.max.z=wpt.z+halfSize.z;

			//////////////////////////////////////////////////////////////////////////////////////////
			// 碰撞得到落脚点
			box.max.y=wpt.y+g_moveFall;
			box.min.y=wpt.y;

			float intersectH=0.0f;
			DWORD dwCarrierObjID=-1;
			if(!pCD->AABBoxCollideBoxTop(box,curTime,intersectH,0,&dwCarrierObjID))
			{
				m_cdResult.path.AddPoint(stepEnd);
				m_cdResult.result=ENR_ToBeContinued;
				return false;
			}

			//////////////////////////////////////////////////////////////////////////////////////////
			// 检查落脚点是否可通过
			box.max.y=intersectH+roleSize.y-10.0f;
			box.min.y=intersectH+10.0f;
			if(pCD->AABBoxCollideBox(box,curTime))
			{
				m_cdResult.result=ENR_Blocking;
				return true;
			}

			//////////////////////////////////////////////////////////////////////////////////////////
			// 着陆到其它载具或静态表面
			wpt.y=intersectH;
			if(dwCarrierObjID==-1)
			{
				m_cdResult.path.AddPoint(stepEnd);
				m_cdResult.result=ENR_CarryLanded;
				m_cdResult.landed.Fill(wpt);
				return true;
			}
			else
			{
				m_cdResult.path.AddPoint(stepEnd);
				m_cdResult.result=ENR_WillCarry;
				m_cdResult.carry.Fill(dwCarrierObjID,wpt);
				return true;
			}
		}
	};
	

	//--class NavCollider_MoveOnCarrier::MoveCtrl--------------------------------------------------
	class NavCollider_MoveOnCarrier::MoveCtrl
	{
	public:
		NavCollider_MoveOnCarrier*	m_p;

		float						m_totalTime;		//总移动时间
		float						m_movedTime;		//已移动时间
		float						m_endTime;			//移动结束时间
		Vector3						m_endPos;			//移动结束位置
		bool						m_bFinished;		//移动完成标志

	public:
		MoveCtrl(NavCollider_MoveOnCarrier* pParent)
			:m_p(pParent)
		{}
		~MoveCtrl()
		{}

		bool MoveStart(const Collider::tagCDResult& cdResult,NavCarrierNode* pCarrier,float startTime,float deltaTime,float& moveTime,float& endTime,Vector3& out)
		{
			//--计算总移动时间
			m_totalTime=cdResult.path.GetDist()/m_p->m_input.moveSpeed;

			//--初始化已移动时间
			m_movedTime=0.0f;

			//--清空移动完成标志
			m_bFinished=false;

			//--调用移动Update
			return MoveUpdate(cdResult,pCarrier,startTime,deltaTime,moveTime,endTime,out);
		}

		bool MoveUpdate(const Collider::tagCDResult& cdResult,NavCarrierNode* pCarrier,float startTime,float deltaTime,float& moveTime,float& endTime,Vector3& out)
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

				if(cdResult.result==ENR_WillCarry)
					out=cdResult.carry.carryPos;
				else if(cdResult.result==ENR_CarryLanded)
					out=cdResult.landed.landPos;
				else
					pCarrier->PosToWorld(cdResult.path.Back(),endTime,out);

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
				Vector3 local;
				cdResult.path.SlerpPos(factor,local);

				pCarrier->PosToWorld(local,startTime+deltaTime,out);

				return false;
			}
		}
	};



	//--class NavCollider_MoveOnCarrier------------------------------------------------------------
	NavCollider_MoveOnCarrier::NavCollider_MoveOnCarrier(void):m_state(EState_None)
	{
		m_pCollider=new Collider(this);
		m_pMoveCtrl=new MoveCtrl(this);
	}

	NavCollider_MoveOnCarrier::~NavCollider_MoveOnCarrier(void)
	{
		SAFE_DELETE(m_pCollider);
		SAFE_DELETE(m_pMoveCtrl);
	}

	void NavCollider_MoveOnCarrier::Init( DWORD dwCarrierObjID,const Vector3& localStartPos,const Vector3& localMoveDir,const Vector3& roleSize,float moveSpeed,int maxCDTimes )
	{
		//--检查输入参数
		ASSERT(moveSpeed>0.0f);
		ASSERT(maxCDTimes>0);

		//--保存输入参数
		m_input.dwCarrierObjID	= dwCarrierObjID;
		m_input.localStartPos	= localStartPos;
		m_input.localMoveDir	= localMoveDir;
		m_input.roleSize		= roleSize;
		m_input.moveSpeed		= moveSpeed;
		m_input.maxCDTimes		= maxCDTimes;

		//--移动方向转换成xz平面方向
		m_input.localMoveDir.y	= 0;
		Vec3Normalize(m_input.localMoveDir);

		//--初始化m_output
		ZeroMemory(&m_output,sizeof(m_output));
		m_output.dwCarrierObjID=-1;

		//--设置状态
		m_state					= EState_Initialized;
	}

	ENavResult NavCollider_MoveOnCarrier::Update( NavCollider* pCollider,float startTime,float deltaTime,Vector3& outPos,float& endTime,DWORD& dwCarrierObjID )
	{
		const Collider::tagCDResult& cdResult=m_pCollider->GetResult();

		//--查找载具
		NavCarrierNode* pCarrier=pCollider->FindCarrier(m_input.dwCarrierObjID);
		ASSERT(pCarrier!=NULL&&pCarrier->IsRunning());

		switch(m_state)
		{
		case EState_Initialized:
			{
				//--初始化碰撞检测器
				m_pCollider->Init();

				//--碰撞
				bool bCDOk=m_pCollider->Collide(pCollider,pCarrier,startTime,deltaTime);
				if(bCDOk)
				{
					m_output.result=cdResult.result;
					if(cdResult.result==ENR_WillCarry)
						m_output.dwCarrierObjID=cdResult.carry.dwCarrierObjID;
				}

				//--开始移动
				float moveTime=0.0f;
				bool bMoveOk=m_pMoveCtrl->MoveStart(cdResult,pCarrier,startTime,deltaTime,moveTime,m_output.endTime,m_output.outPos);

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
				bool bMoveOk=m_pMoveCtrl->MoveUpdate(cdResult,pCarrier,startTime,deltaTime,moveTime,m_output.endTime,m_output.outPos);
				if(!bMoveOk)
				{
					outPos=m_output.outPos;
					return ENR_ToBeContinued;
				}

				startTime+=moveTime;
				deltaTime-=moveTime;

				//--碰撞
				bool bCDOk=m_pCollider->Collide(pCollider,pCarrier,startTime,deltaTime);
				if(bCDOk)
				{
					m_output.result=cdResult.result;
					if(cdResult.result==ENR_WillCarry)
						m_output.dwCarrierObjID=cdResult.carry.dwCarrierObjID;
				}

				//--开始移动
				bMoveOk=m_pMoveCtrl->MoveStart(cdResult,pCarrier,startTime,deltaTime,moveTime,m_output.endTime,m_output.outPos);

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
				bool bMoveOk=m_pMoveCtrl->MoveUpdate(cdResult,pCarrier,startTime,deltaTime,moveTime,m_output.endTime,m_output.outPos);
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
}//namespace WorldBase