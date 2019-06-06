#pragma once
#include ".\BaseData.h"
#include "..\WorldDefine\filter.h"
class FilterData
	: public BaseData
{
public:
	FilterData(void);
	~FilterData(void);

	static FilterData* Inst();

	/** 载入过滤词文件
	*/
	virtual void LoadFromFile();

	/** 名字是否全法
	*/
	DWORD IsValidName(LPCTSTR name, INT nMax=7, INT nMin=1);

	/** 聊天过滤
	*/
	DWORD WordsFilteror(tstring& strText, INT nMax=200);

private:
	vector<tstring>					m_vecNameFilter;
	vector<tstring>					m_vecChatFilter;
};