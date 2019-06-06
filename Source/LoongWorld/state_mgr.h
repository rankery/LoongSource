//-----------------------------------------------------------------------------
//!\file player_session.h
//!\author
//!
//!\date 2009-03-10
//!
//!\brief 状态位相关操作
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once
//-----------------------------------------------------------------------------
// 类声明
//-----------------------------------------------------------------------------
template<class T, typename E>
class State
{
public:
	State();
	
	VOID	InitState(T tState);

	BOOL	IsInState(E eState) const;
	BOOL	IsInStateAll(T tState) const;
	BOOL	IsInStateAny(T tState) const;

	T		GetState() const;
	VOID	SetState(E eState);
	VOID	UnsetState(E eState);

private:
	T		m_tState;
};


//-----------------------------------------------------------------------------
// 内联实现
//-----------------------------------------------------------------------------
template<class T, typename E>
inline State<T, E>::State()
{
	ZeroMemory(this, sizeof(*this));
}

template<class T, typename E>
inline VOID State<T, E>::InitState(T tState)
{
	m_tState = tState;
}

template<class T, typename E>
inline T State<T, E>::GetState() const
{
	return m_tState;
}

template<class T, typename E>
inline BOOL State<T, E>::IsInState(E eState) const
{
	return (m_tState & (T)eState);
}

template<class T, typename E>
inline BOOL State<T, E>::IsInStateAll(T tState) const
{
	return (m_tState & tState) == tState;
}

template<class T, typename E>
inline BOOL State<T, E>::IsInStateAny(T tState) const
{
	return m_tState & tState;
}

template<class T, typename E>
inline VOID State<T, E>::SetState(E eState)
{
	m_tState |= eState;
}

template<class T, typename E>
inline VOID State<T, E>::UnsetState(E eState)
{
	if(IsInState(eState))
	{
		m_tState ^= eState;
	}
}