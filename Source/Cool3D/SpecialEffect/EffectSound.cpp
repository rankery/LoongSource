#include "StdAfx.h"
#include ".\effectsound.h"
#include "EffectPathCtrl.h"
#include "SGeffectNode.h"
#include "..\Kernel.h"
#include "..\Audio\AudioSystem.h"

namespace Cool3D
{
	bool EffectSound::OP_bEnable = true;

	IMP_RTTI_DYNC(EffectSound,EffectBase);

	class EffectSound::Member
	{
	public:
		tagSoundProp		m_prop;
		vector<tagState>	m_soundArray;	//音效数组
		float				m_lastUpdateTime;//上次Update时特效运行了多久,发送这个时间段内的消息
		float				m_localRunTime;//为了实现时间循环

		struct tagSoundData
		{
			int nID;							// 音效ID
			bool bPlayed;						// 音效是否播放过了
			tagSoundData() : nID( 0 ), bPlayed( false ) {}
		};
		vector<tagSoundData>	m_soundData;
	public:
		Member()
		{
			m_lastUpdateTime=0;
			m_localRunTime = 0;
		}
		~Member()	
		{
			FreeSound();
		}

		void FreeSound()
		{
			int numChannel = (int)m_soundData.size();
			for(int i=0;i<numChannel;i++)
			{
					int id = m_soundData[i].nID;
					AudioSystem::Inst()->Destroy3DEventSound(id);
			}
			m_soundData.clear();
		}
	};

	EffectSound::EffectSound(void)
	{
		m_p=new Member;
	}

	EffectSound::~EffectSound(void)
	{
		delete m_p;
	}

	void EffectSound::Serialize(FArchive& ar)
	{
		ar.ValueWrite(m_p->m_prop);
		SerialValVector(ar,m_p->m_soundArray);

		EffectBase::Serialize(ar);
	}

	void EffectSound::Deserialize(FArchive& ar)
	{
		tagSoundProp prop;
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

	RenderNode* EffectSound::GetRenderNode(int i)
	{
		return NULL;
	}

	void EffectSound::Update(SceneGraph* pSG,SGEffectNode *pSGNode,CameraBase *pCamera,float deltaTime,float runTime)
	{
		EffectPathCtrl* pPath=pSGNode->GetEffectPath(m_p->m_prop.szPathName);

		m_p->m_localRunTime+=deltaTime;
		if(m_p->m_prop.timeLoop > 0
			&& m_p->m_localRunTime >= m_p->m_prop.timeLoop)
		{
			m_p->m_localRunTime -= m_p->m_prop.timeLoop;
			m_p->m_lastUpdateTime = m_p->m_localRunTime;
		}

		float localRunTime = m_p->m_localRunTime;

		if(OP_bEnable)
		{
			for(size_t i=0;i<m_p->m_soundArray.size();i++)
			{
				const tagState& sound=m_p->m_soundArray[i];
				if(sound.soundTime>=m_p->m_lastUpdateTime
					&& sound.soundTime<localRunTime)
				{
					AudioSystem::Inst()->Active3DEventSound(m_p->m_soundData[i].nID,sound.volume);
					m_p->m_soundData[i].bPlayed = true;
				}

				//--更新已经在播放的声音的状态
				if(m_p->m_prop.b3DSound)
				{
					Vector3 pos;
					if(pPath!=NULL)
					{
						pos = pPath->GetCurPos();
					}
					else
					{
						const Matrix4& matNode = pSGNode->GetWorldMat();
						pos = Vector3( matNode._41, matNode._42, matNode._43 );
					}
					AudioSystem::Inst()->Set3DEventSoundAtt(m_p->m_soundData[i].nID,
						pos,Vector3( 0.0f, 0.0f, 0.0f ) );
				}
			}
		}//endof if
		m_p->m_lastUpdateTime=localRunTime;
	}

	void EffectSound::ResetPlayState()
	{
		m_p->m_lastUpdateTime=0;
		m_p->m_localRunTime = 0;

		int num = m_p->m_soundData.size();
		for(int i=0;i<num;i++)
		{
			int id = m_p->m_soundData[i].nID;
			AudioSystem::Inst()->Stop3DEventSound(id);
			m_p->m_soundData[i].bPlayed=false;
		}
	}

	const TCHAR* EffectSound::GetName() const
	{
		return m_p->m_prop.m_szName;
	}

	EffectSound::tagSoundProp EffectSound::GetProp() const
	{
		return m_p->m_prop;
	}

	int EffectSound::GetNumStates() const
	{
		return m_p->m_soundArray.size();
	}

	EffectSound::tagState EffectSound::GetState(int i) const
	{
		ASSERT(i>=0 && i<GetNumStates());
		return m_p->m_soundArray[i];
	}

	void EffectSound::BeginBuild(const tagSoundProp& prop)
	{
		m_p->m_soundArray.clear();
		m_p->m_prop=prop;
	}

	void EffectSound::AddKeyState(const tagState& state)
	{
		m_p->m_soundArray.push_back(state);
	}

	void EffectSound::EndBuild()
	{
		m_p->FreeSound();

		size_t size=m_p->m_soundArray.size();
		m_p->m_soundData.resize(size);

		for(size_t i=0;i<size;i++)
		{
			tagState& state=m_p->m_soundArray[i];
			//if(m_p->m_prop.b3DSound)
			// FOR DEBUG??以前的特效没有设
			if(state.maxDist<=0.0f && state.minDist<=0.0f)
			{
				state.minDist=100.0f;
				state.maxDist=5000.0f;
			}
			m_p->m_soundData[i].nID = AudioSystem::Inst()->Create3DEventSound(
				state.szSoundPath,state.bLoop,state.minDist,state.maxDist,state.volume);
			m_p->m_soundData[i].bPlayed = false;
		}
	}

	void EffectSound::Rename(const TCHAR* szNewName)
	{
		_tcscpy_s(m_p->m_prop.m_szName,sizeof(m_p->m_prop.m_szName)/sizeof(TCHAR),szNewName);
	}

	bool EffectSound::IsClosed()
	{
		for(size_t i=0;i<m_p->m_soundArray.size();i++)
		{
			if( !AudioSystem::Inst()->SoundIsStoped( m_p->m_soundData[i].nID ) )
				return false;
			if( !m_p->m_soundData[i].bPlayed )
				return false;
		}
		return true;
	}

	void EffectSound::Close(float sgNodeRunTime)
	{
		for(size_t i=0;i<m_p->m_soundArray.size();i++)
		{
			int id = m_p->m_soundData[i].nID;
			AudioSystem::Inst()->Stop3DEventSound(id);
			m_p->m_soundData[i].bPlayed=true;
		}
	}

	bool EffectSound::GetBox(AABBox& out) const
	{
		float fMaxDist = 0.0f;
		for(size_t i=0;i<m_p->m_soundArray.size();i++)
		{
			const tagState& state = m_p->m_soundArray[i];
			if( fMaxDist < state.maxDist )
				fMaxDist = state.maxDist;
		}
		out.min = Vector3( -fMaxDist, -fMaxDist, -fMaxDist );
		out.max = Vector3(  fMaxDist,  fMaxDist,  fMaxDist );
		return true;
	}

	int EffectSound::GetLOD() const
	{
		return m_p->m_prop.m_nLOD;
	}
}//namespace Cool3D