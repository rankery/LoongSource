#include "StdAfx.h"
#include ".\effectmotion.h"
#include "..\Math\Transform.h"
#include "EffectPathCtrl.h"
#include "SGeffectNode.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(EffectMotion,EffectPathCtrl);

	class EffectMotion::Member
	{
	public:
		tagMotionProp	m_prop;
		vector<tagState>	m_keyStates;	//每个关键帧的状态
		
		int					m_curState;		//当前的关键帧状态index
		float				m_stateTime;	//当前状态保持了多少时间
		bool				m_bActive;//现在是否已经激活

		Vector3	curPos;	//没有经过公转的位置
		Vector3	curRev;	//当前公转
		Vector3 curPathPt;	//经过公转后的位置
		Vector3 curVel;
		Vector3	curRot;

		Member():curPos(0,0,0),curRev(0,0,0),curPathPt(0,0,0),curVel(0,0,0),curRot(0,0,0)
		{
			m_curState=0;
			m_stateTime=0;
			m_bActive=true;
		}
		~Member()
		{}
	};

	EffectMotion::EffectMotion(void)
	{
		m_p=new Member;
	}

	EffectMotion::~EffectMotion(void)
	{
		delete m_p;
	}

	void EffectMotion::Serialize(FArchive& ar)
	{
		ar.ValueWrite(m_p->m_prop);
		SerialValVector(ar,m_p->m_keyStates);

		EffectBase::Serialize(ar);
	}

	void EffectMotion::Deserialize(FArchive& ar)
	{
		tagMotionProp prop;
		ar.ValueRead(prop);
		vector<tagState> keyStates;
		DeserialValVector(ar,keyStates);

		//build
		BeginBuild(prop);
		for(size_t i=0;i<keyStates.size();i++)
			AddKeyState(keyStates[i]);
		EndBuild();

		EffectBase::Deserialize(ar);
	}

	void EffectMotion::Update(SceneGraph* pSG,SGEffectNode *pSGNode,CameraBase *pCamera,float deltaTime,float runTime)
	{
		if(GetNumStates() <=0)
			return;
		//--是否启动
		if(!m_p->m_bActive)
		{
			if(runTime > m_p->m_prop.m_startTime)
				m_p->m_bActive=true;
		}
		if(!m_p->m_bActive)
			return;

		m_p->m_stateTime+=deltaTime;
		//--插值出当前的状态
		m_p->m_curState=AdvanceState(m_p->m_curState,m_p->m_keyStates.size(),
			m_p->m_keyStates[m_p->m_curState].lastTime,m_p->m_stateTime,
			m_p->m_prop.m_loop);
		ASSERT(m_p->m_curState>=0
			&& m_p->m_curState<(int)m_p->m_keyStates.size());
		tagState curState=m_p->m_keyStates[m_p->m_curState];
		
		if(m_p->m_keyStates.size()>1)//至少有两帧,才进行插值
		{
			int nextState=m_p->m_curState+1;
			bool bLerp=true;
			if(nextState>=(int)m_p->m_keyStates.size())
			{
				if(!m_p->m_prop.m_loop)//已经播放到最后一个状态,而且不是循环播放,则不需插值
					bLerp=false;
				nextState=0;
			}

			if(bLerp)
			{
				tagState& ts=m_p->m_keyStates[nextState];
				float r=m_p->m_stateTime/curState.lastTime;

				D3DXVec3Lerp(&curState.vel,&curState.vel,&ts.vel,r);
				D3DXVec3Lerp(&curState.revVel,&curState.revVel,&ts.revVel,r);
			}
		}//endof if

		m_p->curPos+=curState.vel*deltaTime;
		m_p->curRev+=curState.revVel*deltaTime;

		m_p->curVel=curState.vel;//记录当前速度

		Vector3 lastPathPt=m_p->curPathPt;

		//--计算最终位置
		Transform trans;
		trans.Rotate(m_p->curRev.y,m_p->curRev.x,m_p->curRev.z);
		m_p->curPathPt=m_p->curPos;
		trans.TransVec3(m_p->curPathPt);

		if(m_p->m_prop.bCalcRot && deltaTime>FLOAT_EPSILON)
		{
			Vector3 vel=m_p->curPathPt-lastPathPt;
			vel/=deltaTime;
			m_p->curRot.x=CalcPitch(vel);
			m_p->curRot.y=CalcYaw(vel);
		}
	}

	void EffectMotion::ResetPlayState()
	{
		m_p->m_curState=0;
		m_p->m_stateTime=0;

		m_p->curPos=m_p->m_prop.initPos;	
		m_p->curVel=m_p->curRev=Vector3(0,0,0);	
		m_p->curPathPt=m_p->curPos;

		m_p->m_bActive=m_p->m_prop.m_bActive;
	}

	const TCHAR* EffectMotion::GetName() const
	{
		return m_p->m_prop.m_szName;
	}

	EffectMotion::tagMotionProp EffectMotion::GetProp() const
	{
		return m_p->m_prop;
	}

	int EffectMotion::GetNumStates() const
	{
		return m_p->m_keyStates.size();
	}

	EffectMotion::tagState EffectMotion::GetState(int i) const
	{
		ASSERT(i>=0 && i<GetNumStates());
		return m_p->m_keyStates[i];
	}

	void EffectMotion::BeginBuild(const tagMotionProp& prop)
	{
		m_p->m_keyStates.clear();
		m_p->m_curState=0;
		
		m_p->m_prop=prop;
	}

	void EffectMotion::AddKeyState(const tagState& state)
	{
		m_p->m_keyStates.push_back(state);
	}

	void EffectMotion::EndBuild()
	{
		ResetPlayState();
	}

	const Vector3& EffectMotion::GetCurPos() const
	{
		return m_p->curPathPt;
	}

	const Vector3& EffectMotion::GetCurVel() const
	{
		return m_p->curVel;
	}

	const Vector3& EffectMotion::GetCurRot() const
	{
		return m_p->curRot;
	}

	void EffectMotion::Rename(const TCHAR* szNewName)
	{
		_tcscpy_s(m_p->m_prop.m_szName,sizeof(m_p->m_prop.m_szName)/sizeof(TCHAR),szNewName);
	}

	bool EffectMotion::GetBox(AABBox& out) const
	{
		Vector3 vRot( 0.0f, 0.0f, 0.0f );
		Vector3 vPos = m_p->m_prop.initPos;
		for(int i=0;i<(int)m_p->m_keyStates.size();i++)
		{
			const tagState& state = m_p->m_keyStates[i];
		
			vPos += state.vel * state.lastTime;
			vRot += state.revVel * state.lastTime;
			Transform trans;
			trans.Rotate( vRot.y, vRot.x, vRot.z );
			Vector3 vCurPos = vPos;
			trans.TransVec3( vCurPos );
			out.Merge( vCurPos );
			if( state.revVel.x != 0.0f || state.revVel.y != 0.0f || state.revVel.z != 0.0f )
			{
				Vector3 vRotVel;
				D3DXVec3Normalize( &vRotVel, &state.revVel );
				Vector3 vPos1;
				D3DXVec3Cross( &vPos1, &vCurPos, &vRotVel );
				out.Merge( vPos1 );
				out.Merge( -vCurPos );
				out.Merge( -vPos1 );
			}
		}
		return true;
	}

	int EffectMotion::GetLOD() const
	{
		return m_p->m_prop.m_nLOD;
	}
}//namespace Cool3D