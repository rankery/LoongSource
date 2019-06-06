#pragma once

#include ".\BaseData.h"


/**服务器活动开启模式
*/
enum EServerActiveMode
{
	ESAM_Week,
	ESAM_MonthDay,
};

#define MAX_WEEK		7
#define Is_FastivalActivity(name)	( ((EServerActiveMode)name) == ESAM_MonthDay)

struct tagSerActiveData
{
	DWORD				dwID;
	EServerActiveMode	eStartMode;				//活动开启模式
	INT					nMonth;					//月
	INT					nDay;					//日
	BYTE				byDate[MAX_WEEK];		//周几
	DWORD				dwColor;
	tstring				strActiveName;
	tstring				strContent;
	tstring				strTime;

	tagSerActiveData()
	{
		memset( this, 0, sizeof(*this) );
	}
};

class SerActiveData
	: public BaseData
{
public:
	SerActiveData(void);
	~SerActiveData(void);

	static SerActiveData* Inst();


	virtual void LoadFromFile();

	tagSerActiveData* FindServerActiveData(DWORD dwID)
	{
		return FindInMap(m_mapSerActiveData, dwID);
	}

	map<DWORD, tagSerActiveData>& GetServerActiveData()
	{
		return m_mapSerActiveData;
	}

	vector<tagSerActiveData>& GetServerActiveVecData()
	{
		return m_vecData;
	}

private:
	map<DWORD, tagSerActiveData>			m_mapSerActiveData;
	vector<tagSerActiveData>				m_vecData;
	
};