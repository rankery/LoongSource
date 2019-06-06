#include "stdafx.h"
#include "AnimationCtrl.h"
#include "AnimationTrack.h"
#include "AnimationState.h"
#include "AnimationMixer.h"
#include "..\ResSys\ResSkeleton.h"
#include "..\ResSys\ResKeyFrame.h"
#include "..\Kernel.h"
#include "..\Device\DrawX.h"

namespace Cool3D
{
	//-----------------------class AnimationCtrl-------------------------------
	IMP_RTTI(AnimationCtrl,RTTIObj);

	AnimationCtrl::AnimationCtrl( void )
		: m_timeFactor(1.0f)
	{
		m_lastUpdateTime=Kernel::Inst()->GetAccumTime();
	}

	AnimationCtrl::~AnimationCtrl( void )
	{
		AnimationStateList::iterator i;
		for(i=m_stateList.begin();i!=m_stateList.end();++i)
			SAFE_DELETE(i->second);

		m_stateList.clear();
	}

	void AnimationCtrl::SetEnable( const TCHAR *szName,bool enabled )
	{
		DWORD name=_MakeFourCC(szName);

		AnimationStateList::iterator i=m_stateList.find(name);
		if( i!=m_stateList.end() )
		{
			AnimationState *pState=i->second;
			pState->SetEnabled(enabled);
		}
	}

	float AnimationCtrl::GetTrackPosition( const TCHAR *szName ) const
	{
		DWORD name=_MakeFourCC(szName);

		AnimationStateList::const_iterator i=m_stateList.find(name);
		if( i!=m_stateList.end() )
		{
			AnimationState *pState=i->second;
			return pState->GetTimePosition();
		}
		else
			return -1.0f;
	}

	bool AnimationCtrl::IsTrackEnd( const TCHAR *szName ) const
	{
		DWORD name=_MakeFourCC(szName);

		// 查看当前动作列表
		AnimationStateList::const_iterator statei=m_stateList.find(name);
		if( statei!=m_stateList.end() )
		{
			AnimationState *pState=statei->second;
			if( pState->GetEnabled() && !pState->IsEnded() )
				return false;
		}

		// 查看缓存动作列表
		CacheCmdList::const_iterator cachei;
		for( cachei = m_cacheCmd.begin(); cachei != m_cacheCmd.end(); ++cachei )
		{
			const tagCmd& cmd = *cachei;
			if( cmd.name == name )
				return false;
		}
		
		return true;
	}

	bool AnimationCtrl::IsFadeinTime( const TCHAR *szName ) const
	{
		DWORD name=_MakeFourCC(szName);

		AnimationStateList::const_iterator statei=m_stateList.find(name);
		if( statei!=m_stateList.end() )
		{
			AnimationState *pState=statei->second;
			if( pState->IsFadeinTime(pState->GetTimePosition()) )
				return true;
		}

		return false;
	}
	//-----------------------class AnimationCtrl-------------------------------

	//-----------------------class NodeAnimationCtrl---------------------------
	IMP_RTTI(NodeAnimationCtrl,AnimationCtrl);

	NodeAnimationCtrl::NodeAnimationCtrl( void )
		: m_pRes(0)
		, m_resIsReady(false)
		, m_upperYaw(0.0f)
		, m_upperPitch(0.0f)
		, m_upperRoll(0.0f)
		, m_headYaw(0.0f)
		, m_headPitch(0.0f)
		, m_headRoll(0.0f)
	{
		m_pMixer=new NodeAnimationMixer(this);
	}

	NodeAnimationCtrl::~NodeAnimationCtrl( void )
	{
		NodeTrackList::iterator i;
		for(i=m_nodeTrackList.begin();i!=m_nodeTrackList.end();++i)
			SAFE_DELETE(i->second);

		m_nodeTrackList.clear();
		m_boneTypeList.clear();

		m_pRes=NULL;

		SAFE_DELETE(m_pMixer);
	}

	void NodeAnimationCtrl::BindAnimationRes( ResBase *pRes )
	{
		m_pRes=static_cast<ResSkeleton*>(pRes);

		UpdateRes();
	}

	bool NodeAnimationCtrl::OpenTrack( const TCHAR *szName,bool loop,float timePos,float fadeinTime,BodyPartType partType,bool isBlending,float weight )
	{
		DWORD name=_MakeFourCC(szName);

		tagCmd cmd;
		cmd.name=name;
		cmd.loop=loop;
		cmd.timePos=timePos;
		cmd.fadeinTime=fadeinTime;
		cmd.partType=partType;
		cmd.isBlending=isBlending;
		cmd.weight=weight;

		// 删除缓存列表中相同类型的动作
		if( !cmd.isBlending )//非混合动作
		{
			switch( cmd.partType )
			{
			case BPT_All:
				{
					CacheCmdList::iterator cachei;
					for( cachei = m_cacheCmd.begin(); cachei != m_cacheCmd.end(); )
					{
						tagCmd& cache = *cachei;
						if( !cache.isBlending )
							cachei = m_cacheCmd.erase(cachei);
						else
							++cachei;
					}
				}
				break;

			case BPT_Upper:
				{
					CacheCmdList::iterator cachei;
					for( cachei = m_cacheCmd.begin(); cachei != m_cacheCmd.end(); )
					{
						tagCmd& cache = *cachei;
						if( !cache.isBlending )
						{
							if( cache.partType == BPT_Upper )
							{
								cachei = m_cacheCmd.erase(cachei);
							}
							else if( cache.partType == BPT_All )
							{
								cache.partType = BPT_Lower;
								++cachei;
							}
							else
								++cachei;
						}
						else
							++cachei;
					}
				}
				break;

			case BPT_Lower:
				{
					CacheCmdList::iterator cachei;
					for( cachei = m_cacheCmd.begin(); cachei != m_cacheCmd.end(); )
					{
						tagCmd& cache = *cachei;
						if( !cache.isBlending )
						{
							if( cache.partType == BPT_Lower )
							{
								cachei = m_cacheCmd.erase(cachei);
							}
							else if( cache.partType == BPT_All )
							{
								cache.partType = BPT_Upper;
								++cachei;
							}
							else
								++cachei;
						}
						else
							++cachei;
					}
				}
				break;
			}
		}

		if( TrackIsReady(cmd.name) )
			ExecuteCmd(cmd);
		else
			m_cacheCmd.push_back(cmd);

		return true;
	}

	bool NodeAnimationCtrl::TrackIsReady( DWORD dwName )
	{
		return m_pRes->IsTrackLoaded(dwName);
	}

	void NodeAnimationCtrl::ExecuteCmd( const tagCmd& cmd )
	{
		NodeAnimationTrack *pCurrenTrack=NULL;
		AnimationState *pCurrentState=NULL;
		float timePos=cmd.timePos;
		NodeTrackList::const_iterator tracki=m_nodeTrackList.find(cmd.name);
		if( tracki==m_nodeTrackList.end() )	//创建新的Track
		{
			//--Track
			pCurrenTrack=new NodeAnimationTrack(this,cmd.name,cmd.partType,cmd.isBlending,cmd.weight);
			m_nodeTrackList[cmd.name]=pCurrenTrack;
			pCurrenTrack->CreateKeyFrames(m_pRes);

			//--State
			if( timePos<0.0f )
			{
				timePos=m_pRes->GetTrackTimeFromRes(cmd.name);
			}
			pCurrentState=new AnimationState(cmd.name,timePos,cmd.fadeinTime,m_pRes->GetTrackTimeFromRes(cmd.name));
			pCurrentState->SetLoop(cmd.loop);
			pCurrentState->SetAniCtrl(this);
			m_stateList[cmd.name]=pCurrentState;
		}
		else
		{
			pCurrenTrack=tracki->second;
			pCurrenTrack->SetPartType(cmd.partType);
			pCurrenTrack->SetWeight(cmd.weight);
			pCurrenTrack->SetBlending(cmd.isBlending);
			pCurrenTrack->SetLowerPreKeyFrame(NULL);
			pCurrenTrack->SetUpperPreKeyFrame(NULL);

			//--重置State
			pCurrentState=m_stateList[cmd.name];
			pCurrentState->ClearFrameMsg();
			if( timePos<0.0f )
			{
				timePos=pCurrentState->GetLength();
			}
			pCurrentState->SetTimePosition(timePos,cmd.fadeinTime);
			pCurrentState->SetLoop(cmd.loop);
		}

		ActiveTrackList::iterator activei = m_activeTracks.find( cmd.name );
		if( activei != m_activeTracks.end() )//从活动动作中删除相同动作
		{
			m_activeTracks.erase( activei );
		}

		if( !cmd.isBlending )	//非动作混合
		{
			switch( cmd.partType )
			{
			case BPT_All:
				{
					for( activei=m_activeTracks.begin();activei!=m_activeTracks.end(); )
					{
						AnimationState *pActiveState=m_stateList[*activei];
						NodeAnimationTrack *pActiveTrack=m_nodeTrackList[*activei];

						//--混合的动作并且没有播放完毕的不用删除
						if( pActiveTrack->IsBlending()
							&& !pActiveState->IsEnded() )
						{
							++activei;
							continue;
						}

						if( pActiveTrack->GetPartType()==BPT_Upper )
						{
							pCurrenTrack->SetUpperPreKeyFrame(pActiveTrack->GetCurKeyFrame());
						}
						else if( pActiveTrack->GetPartType()==BPT_Lower )
						{
							pCurrenTrack->SetLowerPreKeyFrame(pActiveTrack->GetCurKeyFrame());
						}
						else if( pActiveTrack->GetPartType()==BPT_All )
						{
							pCurrenTrack->SetLowerPreKeyFrame(pActiveTrack->GetCurKeyFrame());
							pCurrenTrack->SetUpperPreKeyFrame(pActiveTrack->GetCurKeyFrame());
						}

						pActiveState->SetEnabled(false);
						activei=m_activeTracks.erase(activei);
					}
				}
				break;
			case BPT_Upper:
				{
					if( m_activeTracks.empty() )
					{
						pCurrenTrack->SetPartType(BPT_All);
						break;
					}

					for( activei=m_activeTracks.begin();activei!=m_activeTracks.end(); )
					{
						AnimationState *pActiveState=m_stateList[*activei];
						NodeAnimationTrack *pActiveTrack=m_nodeTrackList[*activei];

						//--混合的动作并且没有播放完毕的不用删除
						if( pActiveTrack->IsBlending()
							&& !pActiveState->IsEnded() )
						{
							++activei;
							continue;
						}

						if( pActiveTrack->GetPartType()==BPT_Upper )
						{
							pCurrenTrack->SetUpperPreKeyFrame(pActiveTrack->GetCurKeyFrame());
							pActiveState->SetEnabled(false);
							activei=m_activeTracks.erase(activei);
						}
						else if( pActiveTrack->GetPartType()==BPT_All )
						{
							pCurrenTrack->SetUpperPreKeyFrame(pActiveTrack->GetCurKeyFrame());
							pActiveTrack->SetPartType(BPT_Lower);//全身动画改为下身动画

							++activei;
						}
						else
						{
							++activei;
						}
					}
				}
				break;
			case BPT_Lower:
				{
					if( m_activeTracks.empty() )
					{
						pCurrenTrack->SetPartType(BPT_All);
						break;
					}

					for( activei=m_activeTracks.begin();activei!=m_activeTracks.end(); )
					{
						AnimationState *pActiveState=m_stateList[*activei];
						NodeAnimationTrack *pActiveTrack=m_nodeTrackList[*activei];

						//--混合的动作并且没有播放完毕的不用删除
						if( pActiveTrack->IsBlending()
							&& !pActiveState->IsEnded() )
						{
							++activei;
							continue;
						}

						if( pActiveTrack->GetPartType()==BPT_Lower )
						{
							pCurrenTrack->SetLowerPreKeyFrame(pActiveTrack->GetCurKeyFrame());
							pActiveState->SetEnabled(false);
							activei=m_activeTracks.erase(activei);
						}
						else if( pActiveTrack->GetPartType()==BPT_All )
						{
							pCurrenTrack->SetLowerPreKeyFrame(pActiveTrack->GetCurKeyFrame());
							pActiveTrack->SetPartType(BPT_Upper);//全身动画改为上身动画

							++activei;
						}
						else
						{
							++activei;
						}
					}
				}
				break;
			}
		}

		pCurrentState->SetEnabled(true);
		m_activeTracks.insert(cmd.name);
	}

	void NodeAnimationCtrl::CloseTrack( const TCHAR *szName )
	{
		DWORD name=_MakeFourCC(szName);

		AnimationStateList::iterator statei=m_stateList.find(name);
		if( statei!=m_stateList.end() )
		{
			SAFE_DELETE(statei->second);

			m_stateList.erase(statei);
		}

		NodeTrackList::iterator tracki=m_nodeTrackList.find(name);
		if( tracki!=m_nodeTrackList.end() )
		{
			SAFE_DELETE(tracki->second);

			m_nodeTrackList.erase(tracki);
		}

		ActiveTrackList::iterator activei = m_activeTracks.find(name);
		if( activei != m_activeTracks.end() )
		{
			m_activeTracks.erase( activei );
		}

		CacheCmdList::iterator cachei;
		for( cachei=m_cacheCmd.begin(); cachei!=m_cacheCmd.end(); )
		{
			const tagCmd& cmd=*cachei;
			if( cmd.name==name )
				cachei=m_cacheCmd.erase(cachei);
			else
				++cachei;
		}
	}

	void NodeAnimationCtrl::ClearTrack( void )
	{
		AnimationStateList::iterator statei;
		for( statei=m_stateList.begin();statei!=m_stateList.end();++statei )
		{
			SAFE_DELETE(statei->second);
		}
		m_stateList.clear();

		NodeTrackList::iterator tracki;
		for( tracki=m_nodeTrackList.begin();tracki!=m_nodeTrackList.end();++tracki )
		{
			SAFE_DELETE(tracki->second);
		}
		m_nodeTrackList.clear();

		m_activeTracks.clear();

		m_cacheCmd.clear();
	}

	const AniMsgTable* NodeAnimationCtrl::GetAniMsgTable( void ) const
	{
		if( m_pRes!=NULL
			&& m_pRes->IsCreated() )
			return m_pRes->GetMsgTable();
		else
			return NULL;
	}

	void NodeAnimationCtrl::UpdateRes( void )
	{
		if( m_pRes==NULL
			|| !m_pRes->IsCreated() )
			return;

		if( !m_resIsReady )
		{
			m_resIsReady=true;

			//SkeletonKeyFrame *pSF=m_pRes->GetFrame(0);
			const SkeletonStruct& sk=m_pRes->GetStruct();
			UINT numBones=m_pRes->GetNumBone();
			m_boneTypeList.resize(numBones);
			for(UINT i=0;i<m_pRes->GetNumBone();i++)
			{
				m_boneTypeList[i]=0;

				DWORD dwInfoOfBone=m_pRes->GetPartInfoOfBone(sk.m_boneNames[i]);
				m_boneTypeList[i]|=dwInfoOfBone;
			}

			if( m_nodeTrackList.size()==0
				&& m_cacheCmd.size()==0 )
			{
				//--加载一个默认的Track
				vector<tstring> trackNames;
				m_pRes->GetTrackName(trackNames);
				if( trackNames.size()>0 )
				{
					OpenTrack(trackNames[0].c_str(),true);
				}
			}
		}

		//--检测Track资源的加载情况
		CacheCmdList::const_iterator iter;
		for( iter=m_cacheCmd.begin(); iter!=m_cacheCmd.end(); ++iter )
		{
			const tagCmd& cmd=*iter;

			m_pRes->PrepareTrack(cmd.name);
			m_pRes->UpdateTrackRes();
		}

		//--清除资源已经被释放的Track
		{
			NodeTrackList::iterator tracki;
			AnimationStateList::iterator statei;
			for( tracki=m_nodeTrackList.begin(),statei=m_stateList.begin(); 
				tracki!=m_nodeTrackList.end(); )
			{
				NodeAnimationTrack *pTrack=tracki->second;
				if( m_pRes->IsTrackRelease(pTrack->GetName()) )
				{
					ActiveTrackList::iterator activei = m_activeTracks.find( tracki->first );
					if( activei != m_activeTracks.end() )
					{
						m_activeTracks.erase(activei);
					}

					SAFE_DELETE(statei->second);
					statei=m_stateList.erase(statei);

					SAFE_DELETE(tracki->second);
					tracki=m_nodeTrackList.erase(tracki);
				}
				else
				{
					++tracki;
					++statei;
				}
			}
		}

		//--更新Track的LastOpenTime，每秒执行一次
		if( Kernel::Inst()->GetAccumTime()-m_lastUpdateTime>=1.0f )
		{
			m_lastUpdateTime=Kernel::Inst()->GetAccumTime();

			ActiveTrackList::iterator i;
			for(i=m_activeTracks.begin();i!=m_activeTracks.end();++i)
			{
				m_pRes->UpdateTrackLastOpenTime(*i);
			}
		}
	}

	void NodeAnimationCtrl::Update( void )
	{
		if( !m_resIsReady )
			return;

		m_pMixer->Reset();

		ActiveTrackList::iterator activei;
		for( activei=m_activeTracks.begin(); activei!=m_activeTracks.end(); ++activei )
		{
			AnimationState *pState = m_stateList[*activei];
			NodeAnimationTrack *pTrack = m_nodeTrackList[*activei];

			float timePos=pState->GetTimePosition();
			float blendt=1.0f;
			if( pState->IsFadeinTime(timePos) )
			{
				float t=pState->GetEscapeFadeinTime()/pState->GetFadeinTime();
				pTrack->Apply(pTrack->GetTimeIndex(timePos),pState->GetLoop(),true,t);

				if( pTrack->IsBlending() )
					blendt=t;
			}
			else
			{
				pTrack->SetLowerPreKeyFrame(NULL);
				pTrack->SetUpperPreKeyFrame(NULL);
				pTrack->Apply(pTrack->GetTimeIndex(timePos),pState->GetLoop());

				if( pTrack->IsBlending() && !pState->GetLoop() )
				{
					blendt=(pState->GetLength()-timePos)/0.2f;
					if( blendt>1.0f )
						blendt=1.0f;
				}
			}

			//--把Track加入Mixer中
			m_pMixer->AddTrack(pTrack,blendt);
		}

		//--进行动作混合
		m_pMixer->Update();
	}

	void NodeAnimationCtrl::ApplyTime( float offset )
	{
		UpdateRes();

		if( !m_resIsReady )
			return;

		//执行缓存的命令
		CacheCmdList::iterator cachei;
		for( cachei=m_cacheCmd.begin(); cachei!=m_cacheCmd.end(); )
		{
			const tagCmd& cmd=*cachei;
			if( TrackIsReady(cmd.name) )
			{
				ExecuteCmd(cmd);
				cachei=m_cacheCmd.erase(cachei);
			}
			else
				break;
		}

		ActiveTrackList::iterator activei;
		for( activei=m_activeTracks.begin(); activei!=m_activeTracks.end(); )
		{
			AnimationState *pState = m_stateList[*activei];
			NodeAnimationTrack *pTrack = m_nodeTrackList[*activei];

			if( pState == NULL )
			{
				TCHAR szError[100];
				_stprintf_s( szError, 99, _T("Animation Error: track %s has released!"), _FourCC2Str(*activei).c_str() );
				::MessageBox(NULL, szError, _T("error"), MB_OK);
				++activei;
				continue;
			}
			if( pTrack->IsBlending() && pState->IsEnded() )
			{
				activei = m_activeTracks.erase( activei );
				pState->SetEnabled(false);
			}
			else
			{
				++activei;
				pState->AddTime(offset*m_timeFactor);
			}
		}
	}

	bool NodeAnimationCtrl::CanGetBoneSpace( void ) const
	{
		return m_pMixer->CanGetBoneSpace();
	}

	void NodeAnimationCtrl::GetAllBonePos( vector<Vector3>& bonesPos ) const
	{
		m_pMixer->GetAllBonePos( bonesPos );
	}

	void NodeAnimationCtrl::GetAllBoneMat( vector<Matrix4>& bonesMat ) const
	{
		m_pMixer->GetAllBoneMat( bonesMat );
	}

	const Matrix4* NodeAnimationCtrl::GetBoneSpace( UINT boneID ) const
	{
		static Transform retTran;

		const Matrix4* pMat=m_pMixer->GetBoneSpace(boneID);
		if( pMat!=NULL )	//从Mixer中获得每个骨骼的世界矩阵
			retTran.LoadMatrix(pMat);
		else				//动作没有加载完时，先使用已加载完的动作，避免返回单位矩阵
		{
			NodeTrackList::const_iterator tracki;
			if( !m_nodeTrackList.empty() )
			{
				tracki=m_nodeTrackList.begin();
				NodeAnimationTrack *pTrack=tracki->second;
				const Transform& tran=pTrack->GetBoneSpace(boneID);
				retTran.LoadMatrix(tran.GetMatrix());
			}
		}

		return retTran.GetMatrix();
	}

	const Matrix4* NodeAnimationCtrl::GetBoneSpace( const TCHAR* szBoneName ) const
	{
		static Transform retTran;
		int boneID = m_pRes->GetBoneID(szBoneName);
		if( boneID == -1 )
			return retTran.GetMatrix();

		const Matrix4* pMat=m_pMixer->GetBoneSpace(boneID);
		if( pMat!=NULL )	//从Mixer中获得每个骨骼的世界矩阵
			retTran.LoadMatrix(pMat);
		else				//动作没有加载完时，先使用已加载完的动作，避免返回单位矩阵
		{
			NodeTrackList::const_iterator tracki;
			if( !m_nodeTrackList.empty() )
			{
				tracki=m_nodeTrackList.begin();
				NodeAnimationTrack *pTrack=tracki->second;
				const Transform& tran=pTrack->GetBoneSpace(boneID);
				retTran.LoadMatrix(tran.GetMatrix());
			}
		}

		return retTran.GetMatrix();
	}

	bool NodeAnimationCtrl::BoneIsMirrored( UINT boneID ) const
	{
		return m_pMixer->BoneIsMirrored(boneID);
	}

	DWORD NodeAnimationCtrl::PeekFrameMsg( void )
	{
		ActiveTrackList::iterator iter;
		for( iter=m_activeTracks.begin() ; iter!=m_activeTracks.end(); iter++ )
		{
			// map[]如果没有取到值，会进行插入操作，不建议使用，最好还是使用find
			AnimationStateList::iterator statei = m_stateList.find(*iter);
			if( statei != m_stateList.end() && statei->second != NULL )
			{
				AnimationState *pState = statei->second;
				DWORD msgCode;
				if( pState->PeekFrameMsg(msgCode) )
					return msgCode;
			}
		}

		return 0;
	}

	void NodeAnimationCtrl::GetBoneChildren( UINT boneID,vector<UINT>& children ) const
	{
		NodeTrackList::const_iterator tracki;
		AnimationStateList::const_iterator statei;
		for( tracki=m_nodeTrackList.begin(),statei=m_stateList.begin()
			;tracki!=m_nodeTrackList.end();++tracki,++statei )
		{
			if( !statei->second->GetEnabled())
				continue;

			NodeAnimationTrack *pTrack=tracki->second;
			switch( pTrack->GetPartType() )
			{
			case BPT_All:
				pTrack->GetBoneChildren(boneID,children);
				break;
			case BPT_Upper:
				if( m_boneTypeList[boneID]&SKELETONUPPERPART )
					pTrack->GetBoneChildren(boneID,children);
				break;
			case BPT_Lower:
				if( m_boneTypeList[boneID]&SKELETONLOWERPART )
					pTrack->GetBoneChildren(boneID,children);
				break;
			case BPT_Head:
				if( m_boneTypeList[boneID]&SKELETONHEADPART )
					pTrack->GetBoneChildren(boneID,children);
				break;
			}
		}
	}

	void NodeAnimationCtrl::GetTrackSet( vector<tstring>& trackNames )
	{
		if( m_pRes!=NULL
			&& m_pRes->IsCreated() )
		{
			trackNames.clear();
			m_pRes->GetTrackName(trackNames);
		}
	}

	void NodeAnimationCtrl::DrawSkeleton( void ) const
	{
		if( !m_resIsReady )
			return;

		const Vector3 size(1.0f,1.0f,1.0f);
		AABBox box;
		Vector3 pos;
		Vector3 p(0.0f,0.0f,0.0f);

		UINT numBones=m_pRes->GetNumBone();
		AABBox allBox;
		allBox.max=Vector3(-FLOAT_MAX,-FLOAT_MAX,-FLOAT_MAX);
		allBox.min=Vector3(FLOAT_MAX,FLOAT_MAX,FLOAT_MAX);

		//--绘制骨骼
		for(UINT i=0;i<numBones;i++)
		{
			const Matrix4* pMat=GetBoneSpace(i);
			pos=p*(*pMat);
			box.max=pos+size;
			box.min=pos-size;
			DrawX::DrawAABBox(box);

			allBox.Merge(box);
		}
		DrawX::DrawAABBox(allBox);

		//--连接骨骼
		for(UINT i=0;i<numBones;i++)
		{
			const Matrix4* pSMat=GetBoneSpace(i);
			vector<UINT> children;
			Vector3 s=p*(*pSMat);
			GetBoneChildren(i,children);
			for( UINT chi=0;chi<children.size();chi++ )
			{
				const Matrix4* pEMat=GetBoneSpace(children[chi]);
				Vector3 e=p*(*pEMat);
				DrawX::Draw3DLine(s,e,0xFFFF0000);
			}
		}
	}

	void NodeAnimationCtrl::RotateUpper( float yaw,float pitch,float roll )
	{
		m_upperYaw=yaw;
		m_upperPitch=pitch;
		m_upperRoll=roll;
	}

	void NodeAnimationCtrl::RotateHead( float yaw,float pitch,float roll )
	{
		m_headYaw=yaw;
		m_headPitch=pitch;
		m_headRoll=roll;
	}

	void NodeAnimationCtrl::SyncTrack( const TCHAR *szName )
	{
		DWORD name=_MakeFourCC(szName);

		AnimationStateList::const_iterator statei;
		NodeTrackList::const_iterator tracki;
		statei=m_stateList.find(name);
		if( statei==m_stateList.end() )
			return;

		m_activeTracks.clear();

		tracki=m_nodeTrackList.find(name);
		NodeAnimationTrack *pActiveTrack=tracki->second;
		AnimationState *pActiveState=statei->second;

		pActiveTrack->SetLowerPreKeyFrame(NULL);
		pActiveTrack->SetUpperPreKeyFrame(NULL);
		pActiveTrack->SetPartType(BPT_All);
		m_activeTracks.insert(name);
		pActiveState->ClearFrameMsg();
		if( !pActiveState->GetEnabled() )
			pActiveState->SetEnabled(true);

		bool bFadein = false;
		for(statei=m_stateList.begin(),tracki=m_nodeTrackList.begin()
			;statei!=m_stateList.end();++statei,++tracki)
		{
			NodeAnimationTrack *pTrack=tracki->second;
			AnimationState *pState=statei->second;

			if( pState->GetEnabled()
				&& pState->GetAnimationName()!=name )
			{
				if( pTrack->GetPartType()==BPT_Lower )
				{
					pActiveTrack->SetLowerPreKeyFrame(pTrack->GetCurKeyFrame());
				}
				else if( pTrack->GetPartType()==BPT_Upper )
				{
					pActiveTrack->SetUpperPreKeyFrame(pTrack->GetCurKeyFrame());
				}
				else if( pTrack->GetPartType()==BPT_All )
				{
					pActiveTrack->SetLowerPreKeyFrame(pTrack->GetCurKeyFrame());
					pActiveTrack->SetUpperPreKeyFrame(pTrack->GetCurKeyFrame());
				}
				pState->SetEnabled(false);
				bFadein = true;
			}
		}

		if( bFadein )
			pActiveState->SetTimePosition( pActiveState->GetTimePosition(), 0.2f, false );
	}

	bool NodeAnimationCtrl::IsBoneType( UINT boneID,BodyPartType partType ) const
	{
		switch( partType )
		{
		case BPT_All:
			{
				if( boneID<m_boneTypeList.size() )
					return true;
			}
			break;
		case BPT_Upper:
			{
				if( m_boneTypeList[boneID]&SKELETONUPPERPART )
					return true;
			}
			break;
		case BPT_Lower:
			{
				if( m_boneTypeList[boneID]&SKELETONLOWERPART )
					return true;
			}
			break;
		case BPT_Head:
			{
				if( m_boneTypeList[boneID]&SKELETONHEADPART )
					return true;
			}
			break;
		case BPT_UpperOrigin:
			{
				if( m_boneTypeList[boneID]&SKELETONUPPERORIGIN )
					return true;
			}
			break;
		case BPT_HeadOrigin:
			{
				if( m_boneTypeList[boneID]&SKELETONHEADORIGIN )
					return true;
			}
			break;
		}

		return false;
	}
	//-----------------------class NodeAnimationCtrl---------------------------

	//-----------------------class VertexAnimationCtrl-------------------------
	IMP_RTTI(VertexAnimationCtrl,AnimationCtrl);
	VertexAnimationCtrl::VertexAnimationCtrl( void )
		: m_pRes(0)
		, m_resIsReady(false)
		, m_activeTrack(0)
		, m_hasActiveTrack(false)
	{

	}

	VertexAnimationCtrl::~VertexAnimationCtrl( void )
	{
		VertexTrackList::iterator i;
		for(i=m_vertexTrackList.begin();i!=m_vertexTrackList.end();++i)
			SAFE_DELETE(i->second);

		m_vertexTrackList.clear();

		m_pRes=NULL;
	}

	void VertexAnimationCtrl::BindAnimationRes( ResBase *pRes )
	{
		m_pRes=static_cast<ResKeyFrame*>(pRes);

		UpdateRes();
	}

	bool VertexAnimationCtrl::OpenTrack( const TCHAR *szName,bool loop,float timePos,float fadeinTime,BodyPartType partType,bool isBlending,float weight )
	{
		DWORD name=_MakeFourCC(szName);

		tagCmd cmd;
		cmd.name=name;
		cmd.loop=loop;
		cmd.timePos=timePos;
		cmd.fadeinTime=fadeinTime;
		cmd.partType=partType;
		cmd.isBlending=isBlending;
		cmd.weight=weight;

		// 删除缓存列表中相同类型的动作
		CacheCmdList::iterator cachei;
		for( cachei = m_cacheCmd.begin(); cachei != m_cacheCmd.end(); )
		{
			tagCmd& cache = *cachei;
			if( !cache.isBlending )
				cachei = m_cacheCmd.erase(cachei);
			else
				++cachei;
		}

		if( TrackIsReady(cmd.name) )
			ExecuteCmd(cmd);
		else
			m_cacheCmd.push_back(cmd);

		return true;
	}

	void VertexAnimationCtrl::ExecuteCmd( const tagCmd& cmd )
	{
		VertexAnimationTrack *pCurTrack=NULL;
		AnimationState *pCurState=NULL;
		AnimationStateList::const_iterator statei=m_stateList.find(cmd.name);
		if( statei==m_stateList.end() )//创建新的Track
		{
			//--Track
			pCurTrack=new VertexAnimationTrack(cmd.name,cmd.partType,cmd.isBlending,cmd.weight);
			m_vertexTrackList[cmd.name]=pCurTrack;
			pCurTrack->CreateKeyFrames(m_pRes);
			pCurTrack->SetTagNodes(&m_tagNodes);

			//--State
			pCurState=new AnimationState(cmd.name,cmd.timePos,cmd.fadeinTime,m_pRes->GetTrackTime(cmd.name));
			pCurState->SetAniCtrl(this);
			pCurState->SetLoop(cmd.loop);
			m_stateList[cmd.name]=pCurState;
		}
		else
		{
			pCurTrack=m_vertexTrackList[cmd.name];
			pCurTrack->SetWeight(cmd.weight);

			//--重置State
			pCurState=statei->second;
			pCurState->ClearFrameMsg();
			pCurState->SetTimePosition(cmd.timePos,cmd.fadeinTime);
			pCurState->SetEnabled(true);
			pCurState->SetLoop(cmd.loop);
		}

		if( m_hasActiveTrack )
		{
			const VertexAnimationTrack *pActiveTrack=m_vertexTrackList[m_activeTrack];
			pCurTrack->SetPreKeyFrame(pActiveTrack->GetCurKeyFrame());

			AnimationState *pActiveState=m_stateList[m_activeTrack];
			if( cmd.name!=m_activeTrack )
				pActiveState->SetEnabled(false);
		}
		m_hasActiveTrack=true;
		m_activeTrack=cmd.name;
	}

	void VertexAnimationCtrl::CloseTrack( const TCHAR *szName )
	{
		DWORD name=_MakeFourCC(szName);

		AnimationStateList::iterator statei=m_stateList.find(name);
		if( statei!=m_stateList.end() )
		{
			SAFE_DELETE(statei->second);

			m_stateList.erase(statei);
		}

		VertexTrackList::iterator tracki=m_vertexTrackList.find(name);
		if( tracki!=m_vertexTrackList.end() )
		{
			SAFE_DELETE(tracki->second);

			m_vertexTrackList.erase(tracki);
		}

		CacheCmdList::iterator cachei;
		for( cachei=m_cacheCmd.begin(); cachei!=m_cacheCmd.end(); )
		{
			const tagCmd& cmd=*cachei;
			if( cmd.name==name )
				cachei=m_cacheCmd.erase(cachei);
			else
				++cachei;
		}
	}

	void VertexAnimationCtrl::ClearTrack( void )
	{
		AnimationStateList::iterator statei;
		for( statei=m_stateList.begin();statei!=m_stateList.end();++statei )
		{
			SAFE_DELETE(statei->second);
		}
		m_stateList.clear();

		VertexTrackList::iterator tracki;
		for( tracki=m_vertexTrackList.begin();tracki!=m_vertexTrackList.end();++tracki )
		{
			SAFE_DELETE(tracki->second);
		}
		m_vertexTrackList.clear();

		m_cacheCmd.clear();
	}

	const AniMsgTable* VertexAnimationCtrl::GetAniMsgTable( void ) const
	{
		if( m_pRes!=NULL
			&& m_pRes->IsCreated() )
		{
			return m_pRes->GetMsgTable();
		}
		else
			return NULL;
	}

	void VertexAnimationCtrl::UpdateRes( void )
	{
		if( m_pRes==NULL
			|| !m_pRes->IsCreated() )
			return;

		if( !m_resIsReady )
		{
			//--分配Tag nodes内存
			const vector<TagNodeFrame>& tagNodeFrameArray=m_pRes->GetTagFrameArray();
			const TagNodeFrame *pTagFrm=&tagNodeFrameArray[0];
			m_tagNodes.resize(pTagFrm->nodes.size());
			int numTagNodes=(int)pTagFrm->nodes.size();
			for(int i=0;i<numTagNodes;i++)
			{
				m_tagNodes[i].name=pTagFrm->nodes[i].name;
			}	
		}

		m_resIsReady=true;

		if( m_vertexTrackList.size()==0
			&& m_cacheCmd.size()==0 )
		{
			//--加载一个默认的Track
			vector<tstring> trackNames;
			m_pRes->GetTrackName(trackNames);
			if( trackNames.size()>0 )
			{
				OpenTrack(trackNames[0].c_str(),true);
			}
		}		
	}

	void VertexAnimationCtrl::Update( void )
	{
		if( !m_resIsReady )
			return;

		VertexTrackList::iterator tracki;
		AnimationStateList::iterator statei;
		for( tracki=m_vertexTrackList.begin(),statei=m_stateList.begin()
			;tracki!=m_vertexTrackList.end();++tracki,++statei )
		{
			AnimationState *pState=statei->second;
			if( !pState->GetEnabled() )
				continue;

			float timePos=pState->GetTimePosition();
			VertexAnimationTrack *pTrack=tracki->second;
			if( pState->IsFadeinTime(timePos) )
			{
				float t=pState->GetEscapeFadeinTime()/pState->GetFadeinTime();
				pTrack->Apply(pTrack->GetTimeIndex(timePos),pState->GetLoop(),true,t);
			}
			else
				pTrack->Apply(pTrack->GetTimeIndex(timePos),pState->GetLoop());
		}
	}

	void VertexAnimationCtrl::ApplyTime( float offset )
	{
		UpdateRes();

		if( !m_resIsReady )
			return;

		//执行缓存的命令
		CacheCmdList::iterator cachei;
		for( cachei=m_cacheCmd.begin(); cachei!=m_cacheCmd.end(); )
		{
			const tagCmd& cmd=*cachei;
			ExecuteCmd(cmd);
			cachei=m_cacheCmd.erase(cachei);
		}

		VertexTrackList::iterator tracki;
		AnimationStateList::iterator statei;
		for( tracki=m_vertexTrackList.begin(),statei=m_stateList.begin()
			;tracki!=m_vertexTrackList.end();++tracki,++statei )
		{
			AnimationState *pState=statei->second;
			if( !pState->GetEnabled() )
				continue;

			pState->AddTime(offset*m_timeFactor);
		}
	}

	VertStream* VertexAnimationCtrl::GetFrameVertStream( void ) const
	{
		AnimationStateList::const_iterator statei;
		VertexTrackList::const_iterator tracki;
		for( statei=m_stateList.begin(),tracki=m_vertexTrackList.begin()
			;tracki!=m_vertexTrackList.end();++statei,++tracki )
		{
			if( !statei->second->GetEnabled())
				continue;

			VertexAnimationTrack *pTrack=tracki->second;
			const VertexKeyFrame *kf=pTrack->GetCurKeyFrame();

			return kf->m_pStream;
		}

		//--动作没有加载完时，先使用已加载完的动作，避免返回空值
		if( !m_vertexTrackList.empty() )
		{
			tracki=m_vertexTrackList.begin();
			VertexAnimationTrack *pTrack=tracki->second;
			const VertexKeyFrame *kf=pTrack->GetCurKeyFrame();
			return kf->m_pStream;
		}

		return NULL;
	}

	void VertexAnimationCtrl::GetTrackSet( vector<tstring>& trackNames )
	{
		if( m_pRes!=NULL
			&& m_pRes->IsCreated())
		{
			trackNames.clear();
			m_pRes->GetTrackName(trackNames);
		}
	}

	DWORD VertexAnimationCtrl::PeekFrameMsg( void )
	{
		if( m_hasActiveTrack )
		{
			AnimationState *pState=m_stateList[m_activeTrack];
			DWORD msgCode;
			if( pState->PeekFrameMsg(msgCode) )
				return msgCode;
		}

		return 0;
	}

	bool VertexAnimationCtrl::TrackIsReady( DWORD dwTrackName )
	{
		return m_resIsReady;
	}

	bool VertexAnimationCtrl::GetTagSpace( const TCHAR* szTagName,Matrix4& out )
	{
		if( szTagName==NULL || _tcslen(szTagName)<=4 )
			return false;

		// tag_XXXX
		tstring strName = szTagName + 4;
		const DWORD dwNameFourCC = _MakeFourCC( strName );
		for( size_t i = 0; i < m_tagNodes.size(); i++ )
		{
			tagTagNodeSpace& tagSpace = m_tagNodes[i];
			if( dwNameFourCC == tagSpace.name )
			{
				out = *(tagSpace.trans.GetMatrix());
				return true;
			}
		}

		return false;
	}
	//-----------------------class VertexAnimationCtrl-------------------------
}//namespace Cool3D