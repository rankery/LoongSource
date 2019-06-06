#include "StdAfx.h"
#include ".\effectmsg.h"
#include "SGEffectNode.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(EffectMsg,EffectBase);

	class EffectMsg::Member
	{
	public:
		tagProp				m_prop;
		vector<tagState>	m_msgArray;	//消息数组
		float				m_lastUpdateTime;//上次Update时特效运行了多久,发送这个时间段内的消息
	public:
		Member()
		{
			m_lastUpdateTime=0;
		}
		~Member()	{}
	};

	EffectMsg::EffectMsg(void)
	{
		m_p=new Member;
	}

	EffectMsg::~EffectMsg(void)
	{
		delete m_p;
	}

	void EffectMsg::Serialize(FArchive& ar)
	{
		ar.ValueWrite(m_p->m_prop);
		SerialValVector(ar,m_p->m_msgArray);

		EffectBase::Serialize(ar);
	}

	void EffectMsg::Deserialize(FArchive& ar)
	{
		tagProp prop;
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

	RenderNode* EffectMsg::GetRenderNode(int i)
	{
		return NULL;
	}

	void EffectMsg::Update(SceneGraph* pSG,SGEffectNode *pSGNode,CameraBase *pCamera,float deltaTime,float runTime)
	{
		for(vector<tagState>::iterator iter=m_p->m_msgArray.begin();
			iter!=m_p->m_msgArray.end();++iter)
		{
			const tagState& msg=(*iter);
			if(msg.msgTime>=m_p->m_lastUpdateTime
				&& msg.msgTime<runTime)
			{
				pSGNode->PushFrmMsg(msg);
			}
		}
		m_p->m_lastUpdateTime=runTime;
	}

	void EffectMsg::ResetPlayState()
	{
		m_p->m_lastUpdateTime=0;
	}

	const TCHAR* EffectMsg::GetName() const
	{
		return m_p->m_prop.m_szName;
	}

	EffectMsg::tagProp EffectMsg::GetProp() const
	{
		return m_p->m_prop;
	}

	int EffectMsg::GetNumStates() const
	{
		return m_p->m_msgArray.size();
	}

	EffectMsg::tagState EffectMsg::GetState(int i) const
	{
		ASSERT(i>=0 && i<GetNumStates());
		return m_p->m_msgArray[i];
	}

	void EffectMsg::BeginBuild(const tagProp& prop)
	{
		m_p->m_msgArray.clear();
		m_p->m_prop=prop;
	}

	void EffectMsg::AddKeyState(const tagState& state)
	{
		m_p->m_msgArray.push_back(state);
	}

	void EffectMsg::EndBuild()
	{}

	void EffectMsg::Rename(const TCHAR* szNewName)
	{
		_tcscpy_s(m_p->m_prop.m_szName,sizeof(m_p->m_prop.m_szName)/sizeof(TCHAR),szNewName);
	}
}//namespace Cool3D