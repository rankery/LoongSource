#include "StdAfx.h"
#include "ShopProtoData.h"


ShopProtoData::ShopProtoData(void)
{
}

ShopProtoData::~ShopProtoData(void)
{
}

ShopProtoData m_shopproto;
ShopProtoData* ShopProtoData::Inst()
{
	return &m_shopproto;
}

VOID ShopProtoData::LoadFromFile()
{
	LPCTSTR szShopProto = _T("data\\system\\attdata\\shop_proto.xml");
	tstring strPath = g_strLocalPath + _T("\\attdata\\shop_name.xml");
	LPCTSTR szShopName = strPath.c_str(); 
	LPCTSTR szShopRare = _T("data\\system\\attdata\\shop_proto_rare.xml");
	
	list<tstring> ShopProtoFieldList;
	list<tstring> ShopNameFieldList;
	list<tstring> ShopRareFieldList;
	list<tstring>::iterator iter;

	//---------------------------------------------------------------------------------
	//∂¡»°shop_proto
	//---------------------------------------------------------------------------------
	TObjRef<VarContainer> VarShopProto = CreateObj("ShopProto", "VarContainer");
	if(!VarShopProto->Load("VFS_System", szShopProto, "id", &ShopProtoFieldList))
		IMSG(_T("Load file Shop_proto.xml failed\r\n"));

	for(iter = ShopProtoFieldList.begin(); iter != ShopProtoFieldList.end(); ++iter)
	{
		tagShopProtoClient shop;
		shop.dwID			=					VarShopProto->GetDword( _T("id"),				(*iter).c_str(), GT_INVALID );
		shop.bEquip			=	(BOOL)			VarShopProto->GetDword( _T("IsEquip"),			(*iter).c_str(), FALSE );
		shop.bClanTreasury	=	(BOOL)			VarShopProto->GetDword( _T("IsClanTreasury"),	(*iter).c_str(), FALSE );
		shop.eCostType		=	(ECurCostType)	VarShopProto->GetDword( _T("CostType"),			(*iter).c_str(), ECCT_Null );
		shop.dwItemTypeID	=					VarShopProto->GetDword( _T("CostItemID"),		(*iter).c_str(), GT_INVALID	);
		
		for (INT i=0; i<MAX_SHOP_SHELF_NUM; ++i)
		{
			TCHAR szTmp[X_SHORT_NAME] = {0};
			_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("ItemNum%d"), i+1);
			shop.n16Num[i] = VarShopProto->GetInt( szTmp, (*iter).c_str(), 0 );
		}
		for (INT j=0; j<MAX_SHOP_COMMON_ITEM; ++j)
		{
			TCHAR szTemp[X_SHORT_NAME] = {0};
			_sntprintf(szTemp, sizeof(szTemp)/sizeof(TCHAR), _T("ItemID%d"),		j+1);
			shop.Item[j].dwTypeID	=		VarShopProto->GetDword( szTemp, (*iter).c_str(), GT_INVALID );
			_sntprintf(szTemp, sizeof(szTemp)/sizeof(TCHAR), _T("Money%d"),			j+1);
			shop.Item[j].nSilver	=		VarShopProto->GetInt(	szTemp, (*iter).c_str(), 0 );
			_sntprintf(szTemp, sizeof(szTemp)/sizeof(TCHAR), _T("CostVal%d"),		j+1);
			shop.Item[j].nCostNum	=		VarShopProto->GetInt(	szTemp, (*iter).c_str(), 0 );
			_sntprintf(szTemp, sizeof(szTemp)/sizeof(TCHAR), _T("MinRepLevel%d"),	j+1);
			shop.Item[j].byRepLevel = (BYTE)VarShopProto->GetInt(	szTemp, (*iter).c_str(), GT_INVALID );
			_sntprintf(szTemp, sizeof(szTemp)/sizeof(TCHAR), _T("Shelf%d"),			j+1);
			shop.Item[j].byShelf	= (BYTE)VarShopProto->GetInt(	szTemp, (*iter).c_str(), 1 );
			
		}

		//more

		m_mapShopProto.insert(make_pair(shop.dwID, shop));
	}

	KillObj("ShopProto");



	//---------------------------------------------------------------------------------
	//∂¡»°Shop_name
	//---------------------------------------------------------------------------------
	TObjRef<VarContainer> VarShopDisplay = CreateObj("ShopName", "VarContainer");
	if(!VarShopDisplay->Load("VFS_System", szShopName, "id", &ShopNameFieldList))
		IMSG(_T("Load file shop_name.xml failed\r\n"));

	for(iter = ShopNameFieldList.begin(); iter != ShopNameFieldList.end(); ++iter)
	{
		DWORD dwID = VarShopDisplay->GetDword(_T("id"), (*iter).c_str(), GT_INVALID);
		tagShopProtoClient* pShop = FindInMap(m_mapShopProto, dwID);
		if(P_VALID(pShop))
		{
			_tcsncpy( pShop->szName,		VarShopDisplay->GetString( _T("name"),				(*iter).c_str(), _T("")	), sizeof(pShop->szName)/sizeof(TCHAR));
			_tcsncpy( pShop->szShelf[0],	VarShopDisplay->GetString( _T("shelf_1"),			(*iter).c_str(), _T("")	), sizeof(pShop->szShelf[0])/sizeof(TCHAR));
			_tcsncpy( pShop->szShelf[1],	VarShopDisplay->GetString( _T("shelf_2"),			(*iter).c_str(), _T("")	), sizeof(pShop->szShelf[1])/sizeof(TCHAR));
			_tcsncpy( pShop->szShelf[2],	VarShopDisplay->GetString( _T("shelf_3"),			(*iter).c_str(), _T("")	), sizeof(pShop->szShelf[2])/sizeof(TCHAR));
			_tcsncpy( pShop->szShelf[3],	VarShopDisplay->GetString( _T("shelf_4"),			(*iter).c_str(), _T("")	), sizeof(pShop->szShelf[3])/sizeof(TCHAR));

			//more
		}
	}

	KillObj("ShopName");

	//---------------------------------------------------------------------------------
	//∂¡»°Shop_proto_rare
	//---------------------------------------------------------------------------------
	TObjRef<VarContainer> VarShopRare = CreateObj("ShopRare", "VarContainer");
	if(!VarShopRare->Load("VFS_System", szShopRare, "id2", &ShopRareFieldList))
		IMSG(_T("Load file shop_proto_rare.xml failed\r\n"));

	for(iter = ShopRareFieldList.begin(); iter != ShopRareFieldList.end(); ++iter)
	{
		DWORD dwID = VarShopRare->GetDword(_T("id2"), (*iter).c_str(), GT_INVALID);
		tagShopProtoClient* pShop = FindInMap(m_mapShopProto, dwID);
		if(P_VALID(pShop))
		{
			for (INT j=0; j<MAX_SHOP_RARE_ITEM; ++j)
			{
				TCHAR szTemp[X_SHORT_NAME] = {0};
				_sntprintf(szTemp, sizeof(szTemp)/sizeof(TCHAR), _T("RareID%d"),		j+101);
				pShop->RareItem[j].dwTypeID	=		VarShopRare->GetDword( szTemp, (*iter).c_str(), GT_INVALID );
				_sntprintf(szTemp, sizeof(szTemp)/sizeof(TCHAR), _T("Money%d"),			j+101);
				pShop->RareItem[j].nSilver	=		VarShopRare->GetInt(	szTemp, (*iter).c_str(), 0 );
				_sntprintf(szTemp, sizeof(szTemp)/sizeof(TCHAR), _T("CostVal%d"),		j+101);
				pShop->RareItem[j].nCostNum	=		VarShopRare->GetInt(	szTemp, (*iter).c_str(), 0 );
				_sntprintf(szTemp, sizeof(szTemp)/sizeof(TCHAR), _T("MinRepLevel%d"),	j+101);
				pShop->RareItem[j].byRepLevel = (BYTE)VarShopRare->GetInt(	szTemp, (*iter).c_str(), GT_INVALID );
				_sntprintf(szTemp, sizeof(szTemp)/sizeof(TCHAR), _T("Shelf%d"),			j+101);
				pShop->RareItem[j].byShelf	= (BYTE)VarShopRare->GetInt(	szTemp, (*iter).c_str(), 1 );
				_sntprintf(szTemp, sizeof(szTemp)/sizeof(TCHAR), _T("Qlty%d"),			j+101);
				pShop->RareItem[j].byQuality	= (BYTE)VarShopRare->GetInt(	szTemp, (*iter).c_str(), 0 );
			}

			//more
		}
	}

	KillObj("ShopRare");
}

