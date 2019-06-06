#pragma once
#include "basedata.h"

enum EnumTitleColor
{
	ETC_WHITE		=	1,
	ETC_YELLOW		=	2,
	ETC_GREEN		=	3,
	ETC_BLUE		=	4,
	ETC_ORANGE		=	5,

	ETC_END			=	6
};

enum EnumTitleType
{
	ETT_RenWu		=	1,
	ETT_ShiZu		=	2,
	ETT_ChuanQi		=	3,
	ETT_TeShu		=	4,
	ETT_HuoDong		=	5,

	ETT_FIN			=	6
};

const DWORD DWTC_WHITE = 0xFFFFFFFF;
const DWORD DWTC_YELLOW = 0xFFFFFF00;
const DWORD DWTC_GREEN = 0xFF00FF00;
const DWORD DWTC_BLUE = 0xFF6060FF;
const DWORD DWTC_ORANGE = 0xFFFF8000;
const DWORD DWTC_DEFAULT = 0xFFFFF2A4;

struct tagRoleTitleProto
{
	enum {
		NAMEMAX = 16,
		TYPEMAX = 8
	};
	DWORD	 		m_dwTitleID;
	TCHAR 			m_szTitleName[NAMEMAX + 1];
	DWORD			m_dwTypeID;
	TCHAR 			m_szTypeName[TYPEMAX + 1];
	tstring			m_strDescription;
	EnumTitleColor 	m_eColor;
	DWORD			m_dwColor;
	DWORD 			m_dwBuffID;

	tagRoleTitleProto(){ZeroMemory(this, sizeof(*this));}
};

struct tagRoleTitleTypeInfo
{
	tstring			strName;
	int				nTitlesCount;	//所拥有所有titles数量
};

class RoleTitleProtoData : public BaseData
{
public:
	RoleTitleProtoData(void);
	~RoleTitleProtoData(void);

	static RoleTitleProtoData* Inst();

	virtual VOID LoadFromFile();
	
	const map<DWORD, tagRoleTitleProto>* GetTitleMap() {return &m_mapTitleProto;}
	const map<DWORD, tagRoleTitleTypeInfo>* GetTypeMap() {return &m_mapTypename;}
	const tagRoleTitleProto* GetTitlebyID(DWORD titleID) {return FindInMap(m_mapTitleProto, titleID);}
	const tstring GetTitleNamebyID(DWORD titleID)
	{
		if (titleID == GT_INVALID)
			return _T("");

		const tagRoleTitleProto* ptitle = GetTitlebyID(titleID);
		if (P_VALID(ptitle))
		{
			tstring str = ptitle->m_szTitleName;
			return str;
		}

		return _T("");
	}
	const DWORD GetTitleColorbyID(DWORD titleID)
	{
		if (titleID == GT_INVALID)
			return DWTC_DEFAULT;

		const tagRoleTitleProto* ptitle = GetTitlebyID(titleID);
		if (P_VALID(ptitle))
		{
			return ptitle->m_dwColor;
		}

		return DWTC_DEFAULT;
	}
	const tstring GetTypeNamebyID(DWORD typeID)
	{
		tagRoleTitleTypeInfo* prtti = FindInMap(m_mapTypename, typeID);
		if (P_VALID(prtti))
			return prtti->strName;
		else
			return _T("");
	}

private:
	map<DWORD, tagRoleTitleTypeInfo> m_mapTypename;
	map<DWORD, tagRoleTitleProto> m_mapTitleProto;

	//跟据不同的颜色枚举返回dwcolor
	DWORD GetColor(EnumTitleColor eColor)
	{
		switch (eColor)
		{
		case ETC_WHITE:
			return DWTC_WHITE;
			break;
		case ETC_YELLOW:
			return DWTC_YELLOW;
			break;
		case ETC_GREEN:
			return DWTC_GREEN;
			break;
		case ETC_BLUE:
			return DWTC_BLUE;
			break;
		case ETC_ORANGE:
			return DWTC_ORANGE;
			break;
		}

		return DWTC_DEFAULT;
	}
};
