#pragma once
#include ".\BaseData.h"

struct tagMallTitleData
{
	INT			nIndex;
	tstring		strTitleName;
};

class MallData : public BaseData
{
public:
	MallData(void);
	~MallData(void);

	static MallData* Inst();

	/** 载入商城类型文件
	*/
	virtual void LoadFromFile();

	tagMallTitleData* FindMallTitleData(DWORD dwID)
	{
		return FindInMap(m_mapMallData, dwID);
	}

	DWORD GetMallTitleSize() { return m_dwSize; }

private:
	map<DWORD, tagMallTitleData>		m_mapMallData;
	DWORD								m_dwSize;
};