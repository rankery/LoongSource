#pragma once

namespace Cool3D
{
	class AnimationCtrl;
	/**	\class AnimationState
		\brief 表示动画的状态
	*/
	class AnimationState
	{
	public:
		AnimationState(DWORD name,float timePos,float fadeinTime,float length,bool enabled=true);
		AnimationState(const AnimationState& rhs);
		virtual ~AnimationState(void);

		DWORD GetAnimationName(void) const				{ return m_name; }
		float GetTimePosition(void) const				{ return m_timePos; }
		float GetLength(void) const						{ return m_length; }
		void  SetLength(float len)						{ m_length=len; }
		void  SetTimePosition(float timePos,float fadeinTime,bool bRestart=true);

		void  AddTime(float offset);

		//--在非循环状态下动画是否结束
		bool  IsEnded(void) const						{ return (fabsf(m_timePos-m_length)<1.0e-6f && !m_loop); }

		bool  GetEnabled(void) const					{ return m_enabled; }
		void  SetEnabled(bool enabled)					{ m_enabled=enabled; }

		void  SetLoop(bool loop)						{ m_loop=loop; }
		bool  GetLoop(void) const						{ return m_loop; }

		void  SetAniCtrl(const AnimationCtrl* pCtrl)	{ m_pCtrl=pCtrl; }
		bool  IsFadeinTime(float timePos) const;
		float GetFadeinTime() const						{ return (m_fadeinEnd-m_fadeinBegin); }
		float GetEscapeFadeinTime() const				{ return (m_timePos-m_fadeinBegin); }

		bool  PeekFrameMsg(DWORD& msgCode);
		void  ClearFrameMsg();

	private:
		void  SetTimePosition(float timePos);

	private:
		DWORD				m_name;
		float				m_timePos;
		float				m_length;
		bool				m_enabled;
		bool				m_loop;
		const AnimationCtrl	*m_pCtrl;

		typedef std::list<DWORD> FrameMsgList;
		FrameMsgList		m_frameMsgs;

		float				m_fadeinBegin;
		float				m_fadeinEnd;
		bool				m_bRestart;
	};
}