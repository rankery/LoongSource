//-------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: dword_flag.h
// author: aslan
// actor:
// data: 2009-08-08
// last:
// brief: 无符号整型位标示数组
//-------------------------------------------------------------------------------
#pragma once

template<INT nSize>
struct tagDWORDFlagArray
{
public:
	tagDWORDFlagArray() : m_dwFlags(0) {}
	explicit tagDWORDFlagArray(DWORD dwFlags) : m_dwFlags(dwFlags) {}

	VOID	Init(DWORD dwFlags) { m_dwFlags = dwFlags; }

	VOID	SetFlag(INT nIndex);
	VOID	UnsetFlag(INT nIndex);
	BOOL	IsSet(INT nIndex);
	DWORD	GetFlags();

private:
	DWORD	m_dwFlags;		// 无符号整型标志位数组
};

//---------------------------------------------------------------------------------
// 设置标志位
//---------------------------------------------------------------------------------
template<INT nSize>
inline VOID tagDWORDFlagArray<nSize>::SetFlag(INT nIndex)
{
	if( nIndex < 0 || nIndex >= nSize ) return;

	m_dwFlags |= (1 << nIndex);
}

//---------------------------------------------------------------------------------
// 取消标志位
//---------------------------------------------------------------------------------
template<INT nSize>
inline VOID tagDWORDFlagArray<nSize>::UnsetFlag(INT nIndex)
{
	if( nIndex < 0 || nIndex >= nSize ) return;

	m_dwFlags &= ~(1 << nIndex);
}

//---------------------------------------------------------------------------------
// 某个标志位是否已经设置
//---------------------------------------------------------------------------------
template<INT nSize>
inline BOOL tagDWORDFlagArray<nSize>::IsSet(INT nIndex)
{
	if( nIndex < 0 || nIndex >= nSize ) return FALSE;

	return m_dwFlags & (1 << nIndex);
}

//---------------------------------------------------------------------------------
// 得到标志位数组
//---------------------------------------------------------------------------------
template<INT nSize>
inline DWORD tagDWORDFlagArray<nSize>::GetFlags()
{
	return m_dwFlags;
}