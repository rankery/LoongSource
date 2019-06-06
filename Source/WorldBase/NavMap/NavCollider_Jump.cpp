#include "StdAfx.h"
#include "NavCollider_Jump.h"
#include "NavColliderPrivateDef.h"
#include "NavCollider.h"
#include "NavCarrierNode.h"


namespace WorldBase
{

	/** 计算到达抛物线指定高度的时间
	*/
	inline bool __CalcJumpCollideTime(float ySpeed,float startH,float intersectH,float minTime,float g,float& time)
	{
		float a=0.5f*g;
		float b=-ySpeed;
		float c=intersectH-startH;
		float d=b*b-4.0f*a*c;
		if(d>0)
		{
			time=(-b+sqrtf(d))/(2.0f*a);
			if(time>=minTime)
			{
				return true;
			}
		}
		return false;
	}

	/** 根据时间计算抛物线位置
	*/
	inline void __CalcJumpPos( const Vector3& start,const Vector3& xzDir,float xzSpeed,float ySpeed,float time,float g,Vector3& out )
	{
		out=start+xzDir*(xzSpeed*time);
		out.y=start.y+(ySpeed*time-0.5f*g*time*time);//v0t-1/2gt^2
	}


	//--class NavCollider_Jump::Collider------------------------------------------------------------
	class NavCollider_Jump::Collider
	{
	public:
		struct tagCDResult						//碰撞结果
		{
			ENavResult	result;					//碰撞返回值

			struct tagLanded
			{
				float		landTime;			//着陆时间（相对起跳的时间）

				void Fill(float _landTime)
				{
					landTime			= _landTime;
				}
			};

			struct tagSwim
			{
				float		swimTime;			//游泳时间（相对起跳的时间）

				void Fill(float _swimTime)
				{
					swimTime			= _swimTime;
				}
			};

			struct tagOnWater
			{
				float		onWaterTime;		//入水时间（相对起跳的时间）

				void Fill(float _onWaterTime)
				{
					onWaterTime			= _onWaterTime;
				}
			};

			struct tagSlide
			{
				float		slideTime;			//滑动时间（相对起跳的时间）

				void Fill(float _slideTime)
				{
					slideTime			= _slideTime;
				}
			};

			struct tagToBeContinue
			{
				float		lastCDTime;			//最后一次碰撞时间（相对起跳的时间）

				void Fill(float _lastCDTime)
				{
					lastCDTime			= _lastCDTime;
				}
			};

			struct tagCarry
			{
				float		cdTime;				//碰撞时间（相对起跳的时间）
				D3DVECTOR	localLandPos;		//局部空间着陆位置
				DWORD		dwCarrierObjID;		//载具ObjID

				void Fill(float _cdTime,const Vector3& _localLandPos,DWORD _dwCarrierObjID)
				{
					cdTime				= _cdTime;
					localLandPos		= _localLandPos;
					dwCarrierObjID		= _dwCarrierObjID;
				}
			};

			struct tagVDrop
			{
				float		vdropTime;			//垂直掉落时间（相对起跳的时间）
				float		cdCarrierTime;		//碰撞到载具的时间（绝对时间）
				DWORD		dwCarrierObjID;		//碰撞到的载具ObjID，-1表示未碰撞到载具

				void Fill(float _vdropTime,const float _cdCarrierTime,DWORD _dwCarrierObjID)
				{
					vdropTime			= _vdropTime;
					cdCarrierTime		= _cdCarrierTime;
					dwCarrierObjID		= _dwCarrierObjID;
				}			
			};

			union 
			{
				tagLanded		landed;
				tagSwim			swim;
				tagOnWater		onWater;
				tagSlide		slide;
				tagToBeContinue	toBeContinue;
				tagCarry		carry;
				tagVDrop		vdrop;
			};
		};

	public:
		NavCollider_Jump*	m_p;
		Vector3				m_lastPos;			//上次碰撞位置
		float				m_lastTime;			//上次碰撞时间（相对起跳的时间）
		float				m_cdInterval;		//碰撞时间间隔


	public:
		Collider(NavCollider_Jump* pParent)
			:m_p(pParent)
		{}
		~Collider()
		{}

		void Init()
		{
			//--
			m_lastTime=0.0f;

			//--
			m_lastPos=m_p->m_input.startPos;

			//--
			if(m_p->m_input.xzSpeed>0.0f
				||m_p->m_input.ySpeed>0.0f)
			{
				m_cdInterval=g_collideStep/max(m_p->m_input.xzSpeed,m_p->m_input.ySpeed);
				if(m_cdInterval>0.1f)
					m_cdInterval=0.1f;
			}
			else
				m_cdInterval=0.1f;
		}

		bool Collide( NavCollider* pCD,float startTime,float deltaTime,BitMap* pSpecCangoMap,bool bOnWaterCheck,tagCDResult& out )
		{
			const Vector3& startPos	= m_p->m_input.startPos;
			const float xzSpeed		= m_p->m_input.xzSpeed;
			const float ySpeed		= m_p->m_input.ySpeed;
			const Vector3& dir		= m_p->m_input.xzDir;
			const float g			= m_p->m_input.g;
			const float timeOut		= m_p->m_input.cdTimeOut;

			for(float t=m_cdInterval;;t+=m_cdInterval)
			{
				//--
				float curTime=m_lastTime+m_cdInterval;

				//--
				Vector3 curPos=startPos+dir*(xzSpeed*curTime);
				curPos.y=startPos.y+(ySpeed*curTime-0.5f*g*curTime*curTime);//v0*t-0.5*g*t^2;

				//--检查可行走区域
				if(!pCD->IfCanGo(curPos.x,curPos.z,pSpecCangoMap))
				{
					out.result=ENR_WillVDrop;
					out.vdrop.Fill(m_lastTime,0.0f,-1);
					return true;
				}

				//--碰撞
				if(ySpeed>g*curTime)//上升
				{
					if(RiseCollide(pCD,curPos,curTime,m_lastTime,startTime+t,out))
						return true;
				}
				else//下降
				{
					if(DropCollide(pCD,curPos,curTime,m_lastTime,startTime+t,bOnWaterCheck,out))
						return true;
				}

				//--调整cdInterval
				float jumpSpeed=Vec3Dist(m_lastPos,curPos)/m_cdInterval;
				m_cdInterval=g_collideStep/jumpSpeed;
				if(m_cdInterval>0.1f)
					m_cdInterval=0.1f;

				//--判断时间是否到了？
				if(t>=deltaTime
					||t>=timeOut)
				{
					out.result=ENR_ToBeContinued;
					out.toBeContinue.Fill(curTime);

					//--
					m_lastPos=curPos;
					m_lastTime=curTime;
					return false;
				}

				//--
				m_lastPos=curPos;
				m_lastTime=curTime;
			}

			//不可能执行到这里！
			ASSERT(0);
			return false;
		}	

		bool RiseCollide( NavCollider* pCD,Vector3 curPos,float curTime,float lastTime,float absTime,tagCDResult& out )
		{
			const Vector3 halfSize		= m_p->m_input.roleSize*0.5f;
			const Vector3& roleSize		= m_p->m_input.roleSize;
			const DWORD cdBoxFlags		= m_p->m_input.cdBoxFlag;

			//--生成角色AABB
			AABBox box;
			box.min.x=curPos.x-halfSize.x;
			box.max.x=curPos.x+halfSize.x;
			box.min.z=curPos.z-halfSize.z;
			box.max.z=curPos.z+halfSize.z;
			box.min.y=curPos.y;
			box.max.y=curPos.y+roleSize.y;

			//--测试是否有碰撞
			DWORD dwCarrierObjID=-1;
			if(pCD->AABBoxCollideBoxAndTrn(box,absTime,cdBoxFlags,&dwCarrierObjID))
			{
				out.result=ENR_WillVDrop;
				out.vdrop.Fill(m_lastTime,absTime,dwCarrierObjID);
				return true;
			}

			return false;
		}

		bool DropCollide( NavCollider* pCD,Vector3 curPos,float curTime,float lastTime,float absTime,bool bOnWaterCheck,tagCDResult& out )
		{
			const Vector3 halfSize		= m_p->m_input.roleSize*0.5f;
			const Vector3& roleSize		= m_p->m_input.roleSize;
			const Vector3& startPos		= m_p->m_input.startPos;
			const Vector3& dir			= m_p->m_input.xzDir;
			const float xzSpeed			= m_p->m_input.xzSpeed;
			const float ySpeed			= m_p->m_input.ySpeed;
			const float g				= m_p->m_input.g;
			const DWORD cdBoxTopFlags	= m_p->m_input.cdBoxTopFlags;
			const DWORD cdBoxFlags		= m_p->m_input.cdBoxFlag;

			////////////////////////////////////////////////////////////////////////////////////
			// 生成角色AABB
			AABBox box;
			box.min.x=curPos.x-halfSize.x;
			box.max.x=curPos.x+halfSize.x;
			box.min.z=curPos.z-halfSize.z;
			box.max.z=curPos.z+halfSize.z;
			box.min.y=curPos.y;
			box.max.y=curPos.y+roleSize.y;

			////////////////////////////////////////////////////////////////////////////////////
			// 测试是否有碰撞
			DWORD cdCarrierObjID=-1;
			box.min.y=curPos.y;
			box.max.y=curPos.y+roleSize.y;
			if(pCD->AABBoxCollideBoxAndTrn(box,absTime,cdBoxTopFlags,&cdCarrierObjID))
			{
				////////////////////////////////////////////////////////////////////////////////////
				// 判断是否碰撞到着陆点
				float intersectH=0;
				bool bOnGround=false;
				DWORD landCarrierObjID=-1;
				if(!pCD->AABBoxCollideBoxTopAndTrn(box,absTime,intersectH,&bOnGround,cdBoxFlags,&landCarrierObjID))
				{
					out.result=ENR_WillVDrop;
					out.vdrop.Fill(lastTime,absTime,cdCarrierObjID);
					return true;
				}

				////////////////////////////////////////////////////////////////////////////////////
				// 计算到达抛物线上碰撞高度的时间
				float intersectTime=0.0f;
				if(!__CalcJumpCollideTime(ySpeed,startPos.y,intersectH,lastTime,g,intersectTime))
				{
					out.result=ENR_WillVDrop;
					out.vdrop.Fill(lastTime,absTime,cdCarrierObjID);
					return true;
				}

				////////////////////////////////////////////////////////////////////////////////////
				// 计算到达抛物物线上碰撞高度的坐标
 				__CalcJumpPos(startPos,dir,xzSpeed,ySpeed,intersectTime,g,curPos);

				////////////////////////////////////////////////////////////////////////////////////
				// 判断着陆点是否可以通过
				box.min.y=curPos.y+10.0f;
				box.max.y=box.min.y+roleSize.y-10.0f;
				if(pCD->AABBoxCollideBoxAndTrn(box,absTime,cdBoxTopFlags,&cdCarrierObjID))
				{
					out.result=ENR_WillVDrop;
					out.vdrop.Fill(lastTime,absTime,cdCarrierObjID);
					return true;
				}

				////////////////////////////////////////////////////////////////////////////////////
				// 判断着陆类型
				if(landCarrierObjID!=-1)//载具
				{
					NavCarrierNode* pCarrier=pCD->FindCarrier(landCarrierObjID);
					ASSERT(pCarrier!=NULL&&pCarrier->IsRunning());

					Vector3 landPos;
					pCarrier->PosToLocal(curPos,absTime,landPos);

					out.result=ENR_WillCarry;
					out.carry.Fill(curTime,landPos,landCarrierObjID);
					return true;
				}
				else//静态地物
				{
					////////////////////////////////////////////////////////////////////////////////////
					// 判断是否会入水
					float waterH=0;
					if(bOnWaterCheck && pCD->IfWillOnWater(curPos,waterH))
					{
						curPos.y=waterH;
						float onWaterTime=0;
						__CalcJumpCollideTime(ySpeed,startPos.y,curPos.y,lastTime,g,onWaterTime);
						out.result=ENR_WillOnWater;
						out.onWater.Fill(onWaterTime);
						return true;
					}

					////////////////////////////////////////////////////////////////////////////////////
					// 判断是否会游泳
					if(pCD->IfWillSwim(curPos,roleSize.y,curPos.y))
					{
						float swimTime=0;
						__CalcJumpCollideTime(ySpeed,startPos.y,curPos.y,lastTime,g,swimTime);
						out.result=ENR_WillSwim;
						out.swim.Fill(swimTime);
						return true;
					}

					////////////////////////////////////////////////////////////////////////////////////
					// 判断滑落
					if(bOnGround
						&&pCD->IfWillSlide(curPos.x,curPos.z))
					{
						out.result=ENR_WillSlide;
						out.slide.Fill(intersectTime);
						return true;
					}

					////////////////////////////////////////////////////////////////////////////////////
					// 着陆到静态表面
					out.result=ENR_Landed;
					out.landed.Fill(intersectTime);
					return true;
				}
			}
			else
			{
				////////////////////////////////////////////////////////////////////////////////////
				// 判断是否会入水
				float waterH=0;
				if(bOnWaterCheck && pCD->IfWillOnWater(curPos,waterH))
				{
					curPos.y=waterH;
					float onWaterTime=0;
					__CalcJumpCollideTime(ySpeed,startPos.y,curPos.y,lastTime,g,onWaterTime);
					out.result=ENR_WillOnWater;
					out.onWater.Fill(onWaterTime);
					return true;
				}

				////////////////////////////////////////////////////////////////////////////////////
				// 判断是否会游泳
				if(pCD->IfWillSwim(curPos,roleSize.y,curPos.y))
				{
					float swimTime=0;
					__CalcJumpCollideTime(ySpeed,startPos.y,curPos.y,lastTime,g,swimTime);
					out.result=ENR_WillSwim;
					out.swim.Fill(swimTime);
					return true;
				}

				return false;
			}
		}
	};

	//--class NavCollider_Jump::JumpCtrl------------------------------------------------------------
	class NavCollider_Jump::JumpCtrl
	{
	public:
		NavCollider_Jump*		m_p;
		Collider::tagCDResult	m_cdResult;			//碰撞结果
		float					m_startJumpTime;	//起跳时间
		float					m_totalTime;		//总跳跃时间
		float					m_jumpedTime;		//已跳跃时间
		bool					m_bFinished;		//已跳跃完成
		float					m_endTime;
		Vector3					m_endPos;


	public:
		JumpCtrl(NavCollider_Jump* pParent)
			:m_p(pParent)
		{}
		~JumpCtrl()
		{}

		void Init(float startJumpTime)
		{
			//--
			m_startJumpTime=startJumpTime;

			//--
			m_jumpedTime=0.0f;
		}

	
		bool JumpStart(NavCollider* pCD,const Collider::tagCDResult& cdResult,float startTime,float deltaTime,float& jumpTime,float& endTime,Vector3& out)
		{
			//--保存碰撞结果
			m_cdResult=cdResult;

			//--计算总跳跃时间
			m_totalTime=0.0f;
			switch(cdResult.result)
			{
			case ENR_ToBeContinued:
				{
					m_totalTime=cdResult.toBeContinue.lastCDTime;
				}
				break;
			case ENR_Landed:
				{
					m_totalTime=cdResult.landed.landTime;
				}
				break;
			case ENR_WillCarry:
				{
					m_totalTime=cdResult.carry.cdTime;
				}
				break;
			case ENR_WillSlide:
				{
					m_totalTime=cdResult.slide.slideTime;
				}
				break;
			case ENR_WillSwim:
				{
					m_totalTime=cdResult.swim.swimTime;
				}
			case ENR_WillOnWater:
				{
					m_totalTime=cdResult.onWater.onWaterTime;
				}
				break;
			case ENR_WillVDrop:
				{
					m_totalTime=cdResult.vdrop.vdropTime;
				}
				break;
			default:
				{
					ASSERT(0);
				}
			}

			//--清空完成标志
			m_bFinished=false;

			//--调用跳跃Update
			return JumpUpdate(pCD,startTime,deltaTime,jumpTime,endTime,out);
		}

		bool IsLandedToCarrier( NavCarrierNode* pCarrier,const Vector3& localLandPos,float curTime,float jumpTime,float minTime,float& landTime )
		{
			//计算当前载具着陆位置
			Vector3 worldLandPos;
			pCarrier->PosToWorld(localLandPos,curTime,worldLandPos);

			//计算当前角色脚底高度
			float roleH=m_p->m_input.startPos.y+(m_p->m_input.ySpeed*jumpTime-0.5f*m_p->m_input.g*jumpTime*jumpTime);//v0t-1/2gt^2

			//判断角色脚底高度是否低于着陆高度
			if(roleH<=worldLandPos.y)
			{
				return __CalcJumpCollideTime(m_p->m_input.ySpeed,m_p->m_input.startPos.y,worldLandPos.y,minTime,m_p->m_input.g,landTime);
			}

			return false;
		}

		bool JumpUpdate(NavCollider* pCD,float startTime,float deltaTime,float& jumpTime,float& endTime,Vector3& out)
		{
			if(m_bFinished)
			{
				jumpTime=0.0f;
				endTime=m_endTime;
				out=m_endPos;
				return true;
			}

			switch(m_cdResult.result)
			{
			case ENR_ToBeContinued:
			case ENR_Landed:
			case ENR_WillSlide:
			case ENR_WillSwim:
			case ENR_WillVDrop:
			case ENR_WillOnWater:
				{
					float leaveTime=m_totalTime-m_jumpedTime;
					if(deltaTime>=leaveTime)//finish
					{
						//output jumptime
						jumpTime=leaveTime;

						//add jumped time
						m_jumpedTime+=jumpTime;

						//output endtime
						endTime=m_startJumpTime+m_totalTime;

						//output pos
						__CalcJumpPos(m_p->m_input.startPos,
							m_p->m_input.xzDir,
							m_p->m_input.xzSpeed,
							m_p->m_input.ySpeed,
							m_totalTime,
							m_p->m_input.g,
							out);

						//save pos and endtime
						m_bFinished=true;
						m_endPos=out;
						m_endTime=endTime;

						return true;
					}
					else//jumpping
					{
						//output jumptime
						jumpTime=deltaTime;

						//add jumped time
						m_jumpedTime+=jumpTime;

						//output pos
						__CalcJumpPos(m_p->m_input.startPos,
							m_p->m_input.xzDir,
							m_p->m_input.xzSpeed,
							m_p->m_input.ySpeed,
							m_jumpedTime,
							m_p->m_input.g,
							out);

						return false;
					}
				}
				break;
			case ENR_WillCarry:
				{
					//--查找载具
					NavCarrierNode* pCarrier=pCD->FindCarrier(m_cdResult.carry.dwCarrierObjID);
					ASSERT(pCarrier!=NULL&&pCarrier->IsRunning());

					//--判断是否已着陆到载具
					float landTime=0.0f;
					if(IsLandedToCarrier(pCarrier,m_cdResult.carry.localLandPos,startTime+deltaTime,m_jumpedTime+deltaTime,m_jumpedTime,landTime))
					{//finish

						//output jumptime
						jumpTime=landTime-m_jumpedTime;

						//output endtime
						endTime=m_startJumpTime+landTime;

						//output pos
						__CalcJumpPos(m_p->m_input.startPos,m_p->m_input.xzDir,m_p->m_input.xzSpeed,m_p->m_input.ySpeed,landTime,m_p->m_input.g,out);

						//save pos and endtime
						m_bFinished=true;
						m_endPos=out;
						m_endTime=endTime;

						return true;
					}

					//--判断是否超过跳跃总时间
					float leaveTime=m_totalTime-m_jumpedTime;
					if(deltaTime>=leaveTime)//finish
					{
						//output jumptime
						jumpTime=leaveTime;

						//output endtime
						endTime=m_startJumpTime+m_totalTime;

						//output pos
						__CalcJumpPos(m_p->m_input.startPos,m_p->m_input.xzDir,m_p->m_input.xzSpeed,m_p->m_input.ySpeed,m_totalTime,m_p->m_input.g,out);

						//save pos and endtime
						m_bFinished=true;
						m_endPos=out;
						m_endTime=endTime;

						return true;
					}
					else//jumpping
					{
						//output jumptime
						jumpTime=deltaTime;

						//output pos
						__CalcJumpPos(m_p->m_input.startPos,
							m_p->m_input.xzDir,
							m_p->m_input.xzSpeed,
							m_p->m_input.ySpeed,
							m_jumpedTime,
							m_p->m_input.g,
							out);

						//add jumped time
						m_jumpedTime+=deltaTime;
						return false;
					}
				}
				break;
			default:
				{
					ASSERT(0);
				}
			}

			return true;
		}

	};


	//--class NavCollider_Jump----------------------------------------------------------------------
	NavCollider_Jump::NavCollider_Jump(void):m_state(EState_None)
	{
		m_pCollider=new Collider(this);
		m_pJumpCtrl=new JumpCtrl(this);
	}

	NavCollider_Jump::~NavCollider_Jump(void)
	{
		SAFE_DELETE(m_pCollider);
		SAFE_DELETE(m_pJumpCtrl);
	}

	void NavCollider_Jump::Init( const Vector3& startPos,const Vector3& xzDir,const Vector3& roleSize,float xzSpeed,float ySpeed,float g,float cdTimeOut,bool bTrnOnly,bool bIgnoreCarrier )
	{
		//--保存输入参数
		m_input.startPos		= startPos;
		m_input.xzDir			= xzDir;
		m_input.roleSize		= roleSize;
		m_input.xzSpeed			= xzSpeed;
		m_input.ySpeed			= ySpeed;
		m_input.g				= g;
		m_input.cdTimeOut		= cdTimeOut;

		//--生成碰撞标志位 
		m_input.cdBoxTopFlags	= 0;
		if(bTrnOnly)
			m_input.cdBoxTopFlags|=NavSceneNode::EFlag_Terrain;
		if(bIgnoreCarrier)
			m_input.cdBoxTopFlags|=NavSceneNode::EFlag_NotCarrier;

		m_input.cdBoxFlag		= m_input.cdBoxTopFlags;

		//--计算xz平面jump方向
		m_input.xzDir.y=0;
		Vec3Normalize(m_input.xzDir);

		//--初始化m_output
		ZeroMemory(&m_output,sizeof(m_output));
		m_output.dwCarrierObjID=-1;

		//--
		m_pCollider->Init();

		//--设置状态
		m_state					= EState_Initialized;
	}


	ENavResult NavCollider_Jump::Update( NavCollider* pCollider,float startTime,float deltaTime,BitMap* pSpecCangoMap,Vector3& outPos,float& endTime,DWORD& dwCarrierObjID,float& cdCarrierTime,bool bOnWaterCheck/*=false*/ )
	{
		switch(m_state)
		{
		case EState_Initialized:
			{
				//--初始化跳跃控制器
				m_pJumpCtrl->Init(startTime);

				//--碰撞
				Collider::tagCDResult cdResult;
				bool bCDOk=m_pCollider->Collide(pCollider,startTime,deltaTime,pSpecCangoMap,bOnWaterCheck,cdResult);
				if(bCDOk)
				{
					m_output.result=cdResult.result;
					if(cdResult.result==ENR_WillCarry)
					{
						m_output.dwCarrierObjID=cdResult.carry.dwCarrierObjID;
					}
					if(cdResult.result==ENR_WillVDrop)
					{
						if(cdResult.vdrop.vdropTime>0.0f
							||m_input.xzSpeed>0.0f
							||m_input.ySpeed>0.0f)//xzspeed>0或yspeed>0表示非vdrop进入vdrop
						{
							m_output.dwCarrierObjID=cdResult.vdrop.dwCarrierObjID;
							m_output.cdCarrierTime=cdResult.vdrop.cdCarrierTime;
						}
						else
						{
							//vdropTime为0表示无法起跳，如果返回vdrop可能会进入vdrop无限循环
							m_state=EState_Finished;

							if(cdResult.vdrop.dwCarrierObjID!=-1)
							{
								m_output.result=ENR_WillCarry;
								m_output.outPos=m_input.startPos;
								m_output.endTime=startTime;
								m_output.dwCarrierObjID=cdResult.vdrop.dwCarrierObjID;

								outPos=m_output.outPos;
								endTime=m_output.endTime;
								dwCarrierObjID=m_output.dwCarrierObjID;
								return m_output.result; 
							}
							else
							{
								m_output.result=ENR_Landed;
								m_output.outPos=m_input.startPos;
								m_output.endTime=startTime;

								outPos=m_output.outPos;
								endTime=m_output.endTime;
								return m_output.result; 
							}
						}
					}
				}

				//--开始跳跃
				float jumpTime=0.0f;
				bool bJumpOk=m_pJumpCtrl->JumpStart(pCollider,cdResult,startTime,deltaTime,jumpTime,m_output.endTime,m_output.outPos);

				//--设置状态
				if(bCDOk&&bJumpOk)
				{
					m_state=EState_Finished;
					outPos=m_output.outPos;
					endTime=m_output.endTime;
					dwCarrierObjID=m_output.dwCarrierObjID;
					cdCarrierTime=m_output.cdCarrierTime;
					return m_output.result;
				}
				else if(bCDOk&&!bJumpOk)
				{
					m_state=EState_WaitForFinish;
					outPos=m_output.outPos;
					return ENR_ToBeContinued;
				}
				else
				{
					m_state=EState_Jumpping;
					outPos=m_output.outPos;
					return ENR_ToBeContinued;
				}
			}
			break;
		case EState_Jumpping:
			{
				//--跳跃更新
				float jumpTime=0.0f;
				bool bJumpOk=m_pJumpCtrl->JumpUpdate(pCollider,startTime,deltaTime,jumpTime,m_output.endTime,m_output.outPos);
				if(!bJumpOk)
				{
					outPos=m_output.outPos;
					return ENR_ToBeContinued;
				}

				startTime+=jumpTime;
				deltaTime-=jumpTime;

				//--碰撞
				Collider::tagCDResult cdResult;
				bool bCDOk=m_pCollider->Collide(pCollider,startTime,deltaTime,pSpecCangoMap,bOnWaterCheck,cdResult);
				if(bCDOk)
				{
					m_output.result=cdResult.result;
					if(cdResult.result==ENR_WillCarry)
					{
						m_output.dwCarrierObjID=cdResult.carry.dwCarrierObjID;
					}
					if(cdResult.result==ENR_WillVDrop)
					{
						m_output.dwCarrierObjID=cdResult.vdrop.dwCarrierObjID;
						m_output.cdCarrierTime=cdResult.vdrop.cdCarrierTime;
					}
				}

				//--开始跳跃
				bJumpOk=m_pJumpCtrl->JumpStart(pCollider,cdResult,startTime,deltaTime,jumpTime,m_output.endTime,m_output.outPos);

				//--设置状态
				if(bCDOk&&bJumpOk)
				{
					m_state=EState_Finished;
					outPos=m_output.outPos;
					endTime=m_output.endTime;
					dwCarrierObjID=m_output.dwCarrierObjID;
					cdCarrierTime=m_output.cdCarrierTime;
					return m_output.result;
				}
				else if(bCDOk&&!bJumpOk)
				{
					m_state=EState_WaitForFinish;
					outPos=m_output.outPos;
					return ENR_ToBeContinued;
				}
				else
				{
					m_state=EState_Jumpping;
					outPos=m_output.outPos;
					return ENR_ToBeContinued;
				}
			}
			break;
		case EState_WaitForFinish:
			{
				//--跳跃更新
				float jumpTime=0.0f;
				bool bJumpOk=m_pJumpCtrl->JumpUpdate(pCollider,startTime,deltaTime,jumpTime,m_output.endTime,m_output.outPos);
				if(!bJumpOk)
				{
					outPos=m_output.outPos;
					return ENR_ToBeContinued;
				}

				//--设置状态
				m_state=EState_Finished;
				outPos=m_output.outPos;
				endTime=m_output.endTime;
				dwCarrierObjID=m_output.dwCarrierObjID;
				cdCarrierTime=m_output.cdCarrierTime;
				return m_output.result;
			}
			break;
		case EState_Finished:
			{
				outPos=m_output.outPos;
				endTime=m_output.endTime;
				dwCarrierObjID=m_output.dwCarrierObjID;
				cdCarrierTime=m_output.cdCarrierTime;
				return m_output.result;
			}
			break;
		}

		ASSERT(0);
		return ENR_Failed;
	}

}//namespace WorldBase