//-----------------------------------------------------------------------------
// File: FilterMgr.h
// Desc: game tool filter manager 1.0
// Time: 2003-1-8
// Auth: CTCRST
//
// Copyright (c) 2003 Softstar Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#ifndef _GT_FILTER_MANAGER_
#define _GT_FILTER_MANAGER_
#pragma once


enum EFMFilter	// 过滤器种类
{
	FMF_Null,
	FMF_Changeless,
	FMF_MiniLzo1040,
	FMF_ZLib,
	FMF_End
};



struct tagFilterParam	// 过滤器启动参数
{
	EFMFilter	eFilter;
	BOOL		bCompress;	// 正向或者逆向
	LPVOID		lpMemIn;
	LPVOID		lpMemOut;
	DWORD		dwInBufferSize;
	DWORD		dwOutBufferSize;
	DWORD		dwInDataSize;
	DWORD		dwOutDataSize;	// 一般由过滤器填写
	BOOL		bSuccess;		// 一般由过滤器填写
};



//-----------------------------------------------------------------------------
// 简单的过滤器管理
//-----------------------------------------------------------------------------
class CFilterMgr
{
public:

	static BOOL InitFilter(EFMFilter eFilter);	// 初始化过滤器
	
	static INT GetFilterWorkSpace();	// 得到过滤器所需要的工作缓冲
	static INT GetFilterExtraSpace(INT nSrcSize);	

	// 检查各种过滤器算法需要的额外运算空间，不是指Work Space
	static BOOL CheckBufferSize(DWORD dwInSize, DWORD dwOutSize, EFMFilter eFilter);
	static VOID Filter(tagFilterParam* pParam);

	CFilterMgr();
	~CFilterMgr();

private:
	static BOOL m_bInit[FMF_End];
};


#endif	// _GT_FILTER_MANAGER_