#include "stdafx.h"
#include "AnimationCtrl.h"
#include "AnimationState.h"
#include "..\ResSys\AniMsgTable.h"

namespace Cool3D
{
	AnimationState::AnimationState( DWORD name,float timePos,float fadeinTime,float length,bool enabled )
		: m_name(name)
		, m_timePos(timePos)
		, m_length(length)
		, m_enabled(enabled)
		, m_loop(false)
		, m_pCtrl(0)
		, m_bRestart(true)
	{
		DWORD msgCode=_MakeFourCC(_T("BGN"));
		m_frameMsgs.push_back(msgCode);

		m_fadeinBegin=m_timePos;
		m_fadeinEnd=m_fadeinBegin+fadeinTime;
	}

	AnimationState::AnimationState( const AnimationState& rhs )
		: m_name(rhs.m_name)
		, m_timePos(rhs.m_timePos)
		, m_length(rhs.m_length)
		, m_enabled(rhs.m_enabled)
		, m_loop(rhs.m_loop)
		, m_pCtrl(rhs.m_pCtrl)
		, m_fadeinBegin(rhs.m_fadeinBegin)
		, m_fadeinEnd(rhs.m_fadeinEnd)
	{
		m_frameMsgs.clear();
		FrameMsgList::const_iterator iter;
		for( iter=rhs.m_frameMsgs.begin(); iter!=rhs.m_frameMsgs.end(); ++iter )
		{
			m_frameMsgs.push_back(*iter);
		}
	}

	AnimationState::~AnimationState( void )
	{
		m_pCtrl=NULL;
		m_frameMsgs.clear();
	}

	void AnimationState::SetTimePosition( float timePos )
	{
		if( fabs(timePos-m_timePos)>1.0e-6f )
		{
			float lastTimePos=m_timePos;
			m_timePos=timePos;
			if( IsFadeinTime(m_timePos) )//处在过渡时间段
				return;
			if( m_fadeinEnd!=0.0f
				&& m_timePos>=m_fadeinEnd )
			{
				if( m_bRestart )
				{
					m_timePos-=m_fadeinEnd-m_fadeinBegin;
					lastTimePos=0.0f;
				}
				m_fadeinBegin=m_fadeinEnd=0.0f;
			}

			if( m_loop )
			{
				//--循环播放
				m_timePos=fmod(m_timePos,m_length);
				if( m_timePos<0 )
					m_timePos+=m_length;     
			}
			else
			{
				//--只播放一次
				if( m_timePos>m_length )
					m_timePos=m_length;
			}

			//--计算动作的消息
			if( lastTimePos>m_timePos )
			{
				DWORD msgCode=_MakeFourCC(_T("BGN"));//动作又从头开始
				m_frameMsgs.push_back(msgCode);
				lastTimePos=0.0f;
			}
			const AniMsgTable *pTable=m_pCtrl->GetAniMsgTable();
			if( pTable!=NULL )
				pTable->CheckMsgForTime(m_name,lastTimePos,m_timePos,m_frameMsgs);
		}
	}

	void AnimationState::SetTimePosition( float timePos,float fadeinTime,bool bRestart )
	{
		m_timePos=timePos;
		m_fadeinBegin=m_timePos;
		m_fadeinEnd=m_fadeinBegin+fadeinTime;
		m_bRestart = bRestart;
	}

	void AnimationState::AddTime( float offset )
	{
		SetTimePosition(m_timePos+offset);
	}

	bool AnimationState::IsFadeinTime( float timePos ) const
	{
		if( timePos>=m_fadeinBegin && timePos<m_fadeinEnd )
			return true;
		else
			return false;
	}

	bool AnimationState::PeekFrameMsg( DWORD& msgCode )
	{
		if( !m_frameMsgs.empty() )
		{
			FrameMsgList::iterator first=m_frameMsgs.begin();
			msgCode=*first;
			m_frameMsgs.erase(first);

			return true;
		}

		return false;
	}

	void AnimationState::ClearFrameMsg()
	{
		m_frameMsgs.clear();
	}
}//namespace Cool3D