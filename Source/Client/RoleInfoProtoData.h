#pragma once
#include "basedata.h"

struct tagProvinceProto
{
	DWORD			dwID;
	tstring			strName;
};

struct tagAreaProto 
{
	DWORD							dwID;
	tstring							strName;
	map<DWORD, tagProvinceProto>	mapProvince;
};

struct tagConstellationProto
{
	DWORD			dwID;
	tstring			strName;
};

struct tagChinesZodiacProto
{
	DWORD			dwID;
	tstring			strName;
};


class RoleInfoProtoData :
	public BaseData
{
public:
	RoleInfoProtoData(void);
	~RoleInfoProtoData(void);

	static RoleInfoProtoData *inst();

	virtual VOID LoadFromFile();

	const map<DWORD, tagConstellationProto> *GetConstellation()
	{ return &m_mapConstellation; }
	const map<DWORD, tagChinesZodiacProto> *GetChineseZodiac()
	{ return &m_mapChineseZodiac; }
	const map<DWORD, tagAreaProto> *GetArea() { return &m_mapArea; }

private:
	map<DWORD, tagAreaProto>			m_mapArea;				//ÇøÓò
	map<DWORD, tagConstellationProto>	m_mapConstellation;		//ÐÇ×ù
	map<DWORD, tagChinesZodiacProto>	m_mapChineseZodiac;		//ÉúÐ¤

};
