//-------------------------------------------------------------------------------------------------
// Copyright (c) 2006 TENGWU Entertainment All rights reserved.
// filename: main.h
// author: 网上下载
// actor:
// data: 2006-08-10
// last:
// brief: clistview控件item排序处理
//-------------------------------------------------------------------------------------------------
#pragma once

//-------------------------------------------------------------------------------------------------
//排序方式
//-------------------------------------------------------------------------------------------------
enum ESortMode
{
	ESM_Start = 0,

	ESM_String,	 //字符串排序
	ESM_Value,	 //数值
	ESM_Percent, //百分数

	ESM_End = 20,
};

//-------------------------------------------------------------------------------------------------
class CSortClass
{
public:
	CSortClass(CListCtrl * pListCtrl, const int _iCol, const INT nSortMode);
	virtual ~CSortClass();

	void Sort(const BOOL bAsc);

	static INT CALLBACK CompareString(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static INT CALLBACK CompareValue(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

private:
	INT			m_nColIndex;    //列的序列号
	CListCtrl	*m_pListCtrl;	//排序控件
	INT			m_nSortMode;	//排序模式

public:
	class CSortItem
	{
	public:
		virtual  ~CSortItem();
		CSortItem(const CString &_txt, BOOL bFile = TRUE);
		CString txt;
		BOOL  bFile;
	};
	class CSortItemInt
	{
	public:
		CSortItemInt(const CString &_txt);
		long lValue ;
	};
};

