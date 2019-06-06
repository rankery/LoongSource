//-----------------------------------------------------------------------------
// File: Util.h
// Desc: Game System Util 2.0
// Auth: Lyp
// Date: 2003/11/13
//
// Copyright (c) 1985-2007 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once


#define UTIL_PROBABILITY_MOD	8192	// 概率取模数
//-----------------------------------------------------------------------------
// 工具类
//-----------------------------------------------------------------------------
class Util
{
public:
	DWORD	GetFreeID();
	
	// 随机数
	VOID	Randomize(DWORD dwSeed);
	DWORD	GetRandHolder() { return m_nRandHolder; }
	INT		Rand();

	// 给出概率百分数，看是否命中
	BOOL	Probability(INT nProbCent);
	// 正负浮动数值
	FLOAT	Fluctuate(FLOAT fNum, FLOAT fIncPercent, FLOAT fDecPercent);
	
	// Win98操作系统下IntersectionRect有BUG, 这里我们重写一个
	BOOL	RectIntersection(RECT& rcOut, RECT rc1, RECT rc2);

	// 计算32位CRC,算法快速
	DWORD	Crc32(LPCSTR lpData);	// 专用来计算字符串
	DWORD	Crc32(LPCTSTR pData);	// 专用来计算字符串
	DWORD	Crc32(LPBYTE pData, INT nByteCount);

	// unicode与unicode8转换
	// 使用时注意使用的是公共buff,避免多次调用后内容覆盖
	LPCSTR	UnicodeToUnicode8(LPCTSTR szSrc);
	LPCSTR	UnicodeToAnsi(LPCTSTR szSrc);
	LPCTSTR	Unicode8ToUnicode(LPCSTR szSrc);
	LPCTSTR	AnsiToUnicode(LPCSTR szSrc);

	// 去掉tstring首尾的空格
	VOID	CutSpaceOfStringHeadAndTail(tstring& string);
	// 将长串字符转为token;
	VOID	StringToToken(std::vector<tstring>& token, LPCWSTR szString, WCHAR chSeparator=_T(' '));

	// 检测注册表中指定键值是否存在
	BOOL CheckReg(HKEY key, LPCTSTR subKey);
	// 将指定键值写入注册表中指定路径,如果指定路径没有,则创建并写入
	BOOL WriteReg(LPCTSTR KEY_ROOT/*注册表中的路径*/, LPCTSTR KEY_NAME, LPCTSTR sz);
	// 读取注册表中指定路径的键值
	BOOL ReadReg(LPCTSTR KEY_ROOT, LPCTSTR KEY_NAME, DWORD& key_value);
	BOOL ReadReg(LPCTSTR KEY_ROOT, LPCTSTR KEY_NAME, LPTSTR outstr);

	// 得到磁盘的剩余空间
	INT64 GetDriveFreeSpace(LPCTSTR szDriver);

	Util();
	~Util();

private:

	CRITICAL_SECTION	m_Lock;

	DWORD	m_dwIDHolder;		// 记录ID使用状态
	INT		m_nRandHolder;
	INT		m_nRandCallCount;
	CHAR*	m_pCharPool;
	TCHAR*	m_pTCharPool;
};



