#pragma once
#include "basedata.h"

struct tagLongHunProtoClient;
/** \class LonghunAttData
	\brief 龙魂能力属性管理类
*/
class LonghunAttData :
	public BaseData
{
public:
	LonghunAttData(void);
	~LonghunAttData(void);

	static LonghunAttData* Inst();
	
	/** \载入龙魂能力信息
	*/
	virtual void LoadFromFile();

	const tagLongHunProtoClient* FindLonghunData(DWORD dwID);

private:
	map<DWORD, tagLongHunProtoClient>		m_mapLunghun;
};
