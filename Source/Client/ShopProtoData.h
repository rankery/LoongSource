#pragma once
#include "basedata.h"
#include "..\WorldDefine\shop_define.h"

class ShopProtoData :
	public BaseData
{
public:
	ShopProtoData(void);
	~ShopProtoData(void);

	static ShopProtoData* Inst();

	virtual VOID LoadFromFile();

	const tagShopProtoClient* FindShopProto(DWORD dwID){return FindInMap(m_mapShopProto, dwID);}
	
private:

	map<DWORD, tagShopProtoClient>	m_mapShopProto;
};
