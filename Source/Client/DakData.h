#pragma once
#include "..\WorldDefine\func_npc_define.h"
#include ".\BaseData.h"

struct tagDakSiteEx :
	public tagDakSite
{
	tstring strName;							//地图名
	tstring strDesc;							//描述
};

struct tagDakProtoEx
{
	DWORD			dwID;						// 驿站编号
	tagDakSiteEx	dakSite[MAX_DAK_SITE_NUM];
};

class DakData :
	public BaseData
{
public:
	DakData(void);
	~DakData(void);

	static DakData* Inst();
	/** 载入驿站属性文件
	*/
	virtual void LoadFromFile();
	/**获取驿站基础结构
	*/
	tagDakProtoEx* FindDakData(DWORD dwID)
	{
		return FindInMap(m_pDakProto, dwID);
	}

private:
	map<DWORD, tagDakProtoEx>			m_pDakProto;
};