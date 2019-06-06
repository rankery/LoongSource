#include "StdAfx.h"
#include ".\EffectCamera.h"
#include "..\Math\Transform.h"
#include "SGeffectNode.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(EffectCamera,EffectBase);

	class EffectCamera::Member
	{
	public:
		tagCameraProp			m_prop;
		vector<tagState>		m_keyStates;	// 每个关键帧的状态
		int						m_curState;		// 当前的关键帧状态index
		float					m_stateTime;	// 当前状态保持了多少时间
		bool					m_bActive;      // 现在是否已经激活
	public:
		Member()
		{
			m_curState=0;
			m_stateTime=0;
			m_bActive=true;
		}
		~Member()	
		{
			Destroy();
		}

		void Destroy()
		{
		}

	};

	EffectCamera::EffectCamera(void)	  
	{								  
		m_p=new Member;
	}

	EffectCamera::~EffectCamera(void)
	{
		delete m_p;
	}

	void EffectCamera::Serialize(FArchive& ar)
	{
		ar.ValueWrite(m_p->m_prop);
		SerialValVector(ar,m_p->m_keyStates);

		EffectBase::Serialize(ar);
	}

	void EffectCamera::Deserialize(FArchive& ar)
	{
		tagCameraProp prop;
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

	void EffectCamera::Update(SceneGraph* pSG,SGEffectNode *pSGNode,CameraBase *pCamera,float deltaTime,float runTime)
	{
		//--是否启动
		if( !m_p->m_bActive )
		{
			if(runTime > m_p->m_prop.m_startTime)
				m_p->m_bActive=true;
		}
		if( !m_p->m_bActive )
			return;

		m_p->m_stateTime += deltaTime;

		// 插值出当前的状态
		m_p->m_curState=AdvanceState( m_p->m_curState,
			                          m_p->m_keyStates.size(),
									  m_p->m_keyStates[m_p->m_curState].lastTime,
									  m_p->m_stateTime,
									  m_p->m_prop.m_loop );
		ASSERT(m_p->m_curState>=0 && m_p->m_curState<(int)m_p->m_keyStates.size());
		tagState curState=m_p->m_keyStates[m_p->m_curState];
		
		// 根据与摄像机的距离计算衰减
		const Matrix4& matWorld = pSGNode->GetWorldMat();
		Vector3 vNodePos( matWorld._41, matWorld._42, matWorld._43 );
		Vector3 vCamPos = pCamera->GetPos();
		Vector3 vLookAt = pCamera->GetLookat();
		float   t = Vec3Dist( vNodePos, vCamPos ) * m_p->m_prop.attenuationFactor / Vec3Len( &curState.swing );
		t = t <= 1.0f ? t : 1.0f;
		curState.swing *= ( 1.0f - t );

		// 计算镜头移动速度
		Vector3 transVel = curState.swing * 2.0f / curState.lastTime * deltaTime;
		if( m_p->m_stateTime > curState.lastTime * 0.25f && m_p->m_stateTime < curState.lastTime * 0.75f )
			transVel *= -1.0f;

		vCamPos+=transVel;
		vLookAt+=transVel;
		pCamera->SetLookAt( vCamPos, vLookAt, pCamera->GetUp() );
		
		if( m_bClosing )
		{
			EffectBase::UpdateClosingFade(runTime,m_p->m_prop.m_closeTime);
			//m_p->SetClosingFade(m_closingFade);
		}
	}

	void EffectCamera::BeginBuild(const tagCameraProp& prop)
	{
		m_p->m_keyStates.clear();
		m_p->m_curState=0;
		
		m_p->m_prop=prop;
	}

	void EffectCamera::AddKeyState(const tagState& state)
	{
		m_p->m_keyStates.push_back(state);
	}

	void EffectCamera::EndBuild()
	{
		ResetPlayState();

		ASSERT(m_p->m_keyStates.size()>0);
	}

	void EffectCamera::ResetPlayState()
	{
		m_p->m_curState=0;
		m_p->m_stateTime=0;
	
		m_p->m_bActive=m_p->m_prop.m_bActive;
		//if(!m_p->m_bActive)
			//m_p->m_pRMesh->SetDrawLimit(0);

		EffectBase::ResetPlayState();
	}

	const TCHAR* EffectCamera::GetName() const
	{
		return m_p->m_prop.m_szName;
	}

	EffectCamera::tagCameraProp EffectCamera::GetProp() const
	{
		return m_p->m_prop;
	}

	int EffectCamera::GetNumStates() const
	{
		return m_p->m_keyStates.size();
	}

	EffectCamera::tagState EffectCamera::GetState(int i) const
	{
		ASSERT(i>=0 && i<GetNumStates());
		return m_p->m_keyStates[i];
	}

	void EffectCamera::Rename(const TCHAR* szNewName)
	{
		_tcscpy_s(m_p->m_prop.m_szName,sizeof(m_p->m_prop.m_szName)/sizeof(TCHAR),szNewName);
	}

	float EffectCamera::GetPlayTime()
	{
		if(m_p->m_prop.m_loop)
			return -1;

		float time=0;
		for(int i=0;i<(int)m_p->m_keyStates.size();i++)
		{
			time+=m_p->m_keyStates[i].lastTime;
		}
		return time;
	}

	bool EffectCamera::GetBox(AABBox& out) const
	{
		out.min = Vector3( -m_p->m_prop.range, -m_p->m_prop.range, -m_p->m_prop.range );
		out.max = Vector3( m_p->m_prop.range, m_p->m_prop.range, m_p->m_prop.range );
		return true;
	}

	int EffectCamera::GetLOD() const
	{
		return m_p->m_prop.m_nLOD;
	}
}//namespace Cool3D