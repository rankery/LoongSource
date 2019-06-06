#include "StdAfx.h"
#include ".\EffectTarget.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(EffectTarget,EffectPathCtrl);

	class EffectTarget::Member
	{
	public:
		tagTargetProp	m_prop;
		bool			m_bActive;//现在是否已经激活

		Vector3	curPos;
		Vector3 curVel;
		Vector3	curRot;
		bool bArrive;

		Member():curPos(0,0,0),curVel(0,0,0),curRot(0,0,0)
		{
			m_bActive=true;
			bArrive=false;
		}
		~Member()
		{}
		void CalcRunTime()
		{
			if( 3 == m_prop.upLineType )
			{
				// 抛物线，把prop.speed作为水平方向的速度
				m_prop.time = D3DXVec2Length( &( Vector2( m_prop.endPt.x, m_prop.endPt.z ) - Vector2( m_prop.startPt.x, m_prop.startPt.z ) ) ) / m_prop.speed;
			}
			else
			{
				//对于所有其他曲线,暂时都使用直行公式去估算时间
				m_prop.time=Vec3Dist(m_prop.startPt,m_prop.endPt)/m_prop.speed;
			}
		}
	};

	EffectTarget::EffectTarget(void)
	{
		m_p=new Member;
	}

	EffectTarget::~EffectTarget(void)
	{
		delete m_p;
	}

	void EffectTarget::Serialize(FArchive& ar)
	{
		ar.ValueWrite(m_p->m_prop);
		EffectBase::Serialize(ar);
	}

	void EffectTarget::Deserialize(FArchive& ar)
	{
		ar.ValueRead(m_p->m_prop);
		EffectBase::Deserialize(ar);

		//--
		ResetPlayState();
	}

	void EffectTarget::Update(SceneGraph* pSG,SGEffectNode *pSGNode,CameraBase *pCamera,float deltaTime,float runTime)
	{
		//--是否启动
		if(!m_p->m_bActive)
		{
			if(runTime > m_p->m_prop.m_startTime)
				m_p->m_bActive=true;
		}
		if(!m_p->m_bActive)
			return;

		float myRunTime=runTime-m_p->m_prop.m_startTime;
		tagTargetProp&	prop=m_p->m_prop;//for short
		if(myRunTime>=prop.time)
		{
			//已经到达终点
			m_p->curPos=prop.endPt;
			m_p->curVel=Vector3(0,0,0);
			m_p->bArrive=true;
		}
		else
		{
			Vector3 lastPos=m_p->curPos;
			float r=myRunTime/prop.time;
			//计算当前位置
			switch(prop.speedType)
			{
			case 1:
				{
					r=(1-cos(r*FLOAT_PI))*0.5f;
					D3DXVec3Lerp(&m_p->curPos,&prop.startPt,&prop.endPt,r);
				}
				break;
			case 2:
				{
					r=(1-sinf(r*FLOAT_PI))*0.5f;
					D3DXVec3Lerp(&m_p->curPos,&prop.startPt,&prop.endPt,r);
				}
				break;
			default://默认0,直线
				D3DXVec3Lerp(&m_p->curPos,&prop.startPt,&prop.endPt,r);
			}

			//计算高度变化
			float dist=Vec3Dist(m_p->curPos,prop.startPt)/Vec3Dist(prop.startPt,prop.endPt);
			switch(prop.upLineType)
			{
			case 1:
				m_p->curPos.y+=cosf(dist*FLOAT_PI)*prop.upMax;
				break;
			case 2:
				m_p->curPos.y+=sinf(dist*FLOAT_PI)*prop.upMax;
				break;
			case 3:
				{
					float s = myRunTime / prop.time;
					Vector2 vXZCur;
					D3DXVec2Lerp( &vXZCur, &Vector2( prop.startPt.x, prop.startPt.z ), &Vector2( prop.endPt.x, prop.endPt.z ), s );
					m_p->curPos.x = vXZCur.x;
					m_p->curPos.z = vXZCur.y;
					float fV0 = ( prop.endPt.y - prop.startPt.y ) / prop.time + 4.9f * 50.0f * prop.time;
					m_p->curPos.y = prop.startPt.y + fV0 * myRunTime - 4.9f * 50.0f * myRunTime * myRunTime;
				}
				break;
			default:
				break;
			}

			//计算摆动曲线
			switch(prop.sideLineType)
			{
			case 1:
			case 2:
				{
					Vector3 forward=prop.endPt-prop.startPt;
					D3DXVec3Normalize(&forward,&forward);
					Vector3 side;
					D3DXVec3Cross(&side,&MathConst::AxisY,&forward);
					if(prop.sideLineType==1)
						m_p->curPos+=side*(cosf(dist*FLOAT_PI)*prop.sideMax);
					else
						m_p->curPos+=side*(sinf(dist*FLOAT_PI)*prop.sideMax);
				}
				break;
			default:
				break;
			}

			//计算速度及朝向
			if(deltaTime>FLOAT_EPSILON)
			{
				m_p->curVel=m_p->curPos-lastPos;
				m_p->curVel/=deltaTime;
				m_p->curRot.x=CalcPitch(m_p->curVel);
				m_p->curRot.y=CalcYaw(m_p->curVel);
			}
		}//endof else
	}

	void EffectTarget::ResetPlayState()
	{
		m_p->curPos=m_p->m_prop.startPt;
		m_p->curVel=Vector3(0,0,0);

		m_p->m_bActive=m_p->m_prop.m_bActive;
		m_p->bArrive=false;
	}

	const TCHAR* EffectTarget::GetName() const
	{
		return m_p->m_prop.m_szName;
	}

	const Vector3& EffectTarget::GetCurPos() const
	{
		return m_p->curPos;
	}

	void EffectTarget::Rename(const TCHAR* szNewName)
	{
		_tcscpy_s(m_p->m_prop.m_szName,sizeof(m_p->m_prop.m_szName)/sizeof(TCHAR),szNewName);
	}

	bool EffectTarget::GetBox(AABBox& out) const
	{
		out.Merge( m_p->m_prop.startPt );
		out.Merge( m_p->m_prop.endPt );
		return true;
	}

	const Vector3& EffectTarget::GetCurVel() const
	{
		return m_p->curVel;
	}

	const Vector3& EffectTarget::GetCurRot() const
	{
		return m_p->curRot;
	}

	void EffectTarget::SetProperty(const tagTargetProp& prop)
	{
		m_p->m_prop=prop;

		if(m_p->m_prop.speed>0)
			m_p->CalcRunTime();
	}

	EffectTarget::tagTargetProp& EffectTarget::GetProperty()
	{
		return m_p->m_prop;
	}

	void EffectTarget::ResetTarget(const Vector3& startPt,const Vector3& targetPt)
	{
		m_p->m_prop.endPt=targetPt;
		m_p->m_prop.startPt=startPt;
		Vector3 dir = targetPt - startPt;
		m_p->curRot.x=CalcPitch(dir);
		m_p->curRot.y=CalcYaw(dir);
		m_p->curPos=startPt;

		if(m_p->m_prop.speed>0)
			m_p->CalcRunTime();
	}

	void EffectTarget::ChangeTargetPt(const Vector3& targetPt)
	{
		m_p->m_prop.endPt=targetPt;

		if(m_p->m_prop.speed>0)
			m_p->CalcRunTime();
	}

	bool EffectTarget::IsArrive()
	{
		return m_p->bArrive;
	}

	int EffectTarget::GetLOD() const
	{
		return m_p->m_prop.m_nLOD;
	}
}//namespace Cool3D