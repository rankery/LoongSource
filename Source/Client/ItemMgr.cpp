#include "StdAfx.h"
#include "RoleMgr.h"
#include "ItemMgr.h"
#include "Container.h"
#include "Item.h"
#include "ItemProtoData.h"
#include "ItemEvent.h"
#include "ItemFrame.h"
#include "StorageFrame.h"
#include "..\WorldDefine\msg_func_npc.h"
#include "QuestMgr.h"
#include "CurrencyMgr.h"
#include "CombatSysUtil.h"
#include "LocalPlayer.h"

ItemMgr::ItemMgr(void):m_Trunk(this)
{
	m_pPocket		= NULL;
	m_pEquipment	= NULL;
	m_pStorge		= NULL;
	m_pThrone		= NULL;
	m_pQuest		= NULL;
    m_pGuildStore   = NULL;

	m_bNotEnoughYuanbao = false;
	m_bNotEnoughSilver	= false;
	m_bUseSilverExtend	= false;
	m_bNotInitBaiBaoRecord	= true;
	m_eEncryptState			= EPES_Null;
}

ItemMgr::~ItemMgr(void)
{
}

ItemMgr g_ItemMgr;
ItemMgr* ItemMgr::Inst()
{
	return &g_ItemMgr;
}

VOID ItemMgr::Update()
{

}

VOID ItemMgr::Init()
{
	m_pCmdMgr = TObjRef<NetCmdMgr>();
	m_pSession = TObjRef<NetSession>();
	m_pFrameMgr = TObjRef<GameFrameMgr>();
	m_pUtil	= TObjRef<Util>();

	m_pPocket = new ItemContainer(EICT_Bag,SPACE_ONE_BAG,SPACE_ONE_BAG);
	m_pEquipment = new ItemContainer(EICT_Equip,EEP_End,EEP_End);
	m_pStorge = new ItemContainer(EICT_RoleWare,SPACE_ONE_WARE,SPACE_ONE_WARE);
	m_pThrone = new ItemContainer(EICT_Baibao,SPACE_BAIBAO_BAG,SPACE_BAIBAO_BAG);
	m_pQuest = new ItemContainer(EICT_Quest,SPACE_QUEST_BAG,SPACE_QUEST_BAG);
    m_pGuildStore = new ItemContainer(EICT_GuildWare, SPACE_ONE_GUILDWARE, MAX_GUILDWARE_NUM);

	m_mapSuitNum.Clear();
	m_mapLonghunOn.Clear();
	m_mapCurStartTime.Clear();
	// 百宝袋记录初始化
	m_bNotInitBaiBaoRecord = true;
	m_listBaiBaoRecord.Clear();
	LoadBeGoldStoneItemTable();
	
	m_pCmdMgr->Register("NS_GetRoleInitState_Item",			(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetInitLocalPlayerItem),		_T("NS_GetRoleInitState_Item"));
	m_pCmdMgr->Register("NS_GetRoleInitState_ItemCDTime",	(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetInitLocalPlayerItemCD),	_T("NS_GetRoleInitState_ItemCDTime"));
	m_pCmdMgr->Register("NS_GetRoleInitState_Suit",			(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetInitLocalPlayerSuit),		_T("NS_GetRoleInitState_Suit"));
	m_pCmdMgr->Register("NS_GetRoleInitState_Longhun",		(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetInitLocalPlayerLonghun),	_T("NS_GetRoleInitState_Longhun"));
		
	m_pCmdMgr->Register("NS_Equip",					(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetEquipment),			_T("NS_Equip"));
	m_pCmdMgr->Register("NS_Unequip",				(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetUnEquipment),			_T("NS_Unequip"));
	m_pCmdMgr->Register("NS_SwapWeapon",			(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetSwapWeapon),			_T("NS_SwapWeapon"));
	
	m_pCmdMgr->Register("NS_ItemPosChange",			(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetItemPosChange),		_T("NS_ItemPosChange"));
	m_pCmdMgr->Register("NS_ItemPosChangeEx",		(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetItemPosChangeEx),		_T("NS_ItemPosChangeEx"));
	m_pCmdMgr->Register("NS_NewItemAdd",			(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetNewItemAdd),			_T("NS_NewItemAdd"));
	m_pCmdMgr->Register("NS_NewEquipAdd",			(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetNewEquipAdd),			_T("NS_NewEquipAdd"));
	m_pCmdMgr->Register("NS_ItemAdd",				(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetItemAdd),				_T("NS_ItemAdd"));
	m_pCmdMgr->Register("NS_ItemRemove",			(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetItemRemove),			_T("NS_ItemRemove"));
	m_pCmdMgr->Register("NS_ItemBind",				(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetItemBind),			_T("NS_ItemBind"));
	
	m_pCmdMgr->Register("NS_EquipChange",			(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetEquipChange),			_T("NS_EquipChange"));
	m_pCmdMgr->Register("NS_ItemCDUpdate",			(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetItemCDUpdate),		_T("NS_ItemCDUpdate"));

	m_pCmdMgr->Register("NS_BagExtend",				(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetBagExtend),			_T("NS_BagExtend"));
	m_pCmdMgr->Register("NS_WareExtend",			(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetWareExtend),			_T("NS_WareExtend"));
	
	m_pCmdMgr->Register("NS_SuitNum",				(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetSuitNumChange),		_T("NS_SuitNum"));
	m_pCmdMgr->Register("NS_LongHunOn",				(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetEquipLonghunOn),		_T("NS_LongHunOn"));
	m_pCmdMgr->Register("NS_LongHunOff",			(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetEquipLonghunOff),		_T("NS_LongHunOff"));
	
	m_pCmdMgr->Register("NS_IdentifyEquip",			(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetIdentifyEquip),		_T("NS_IdentifyEquip"));
	m_pCmdMgr->Register("NS_Abrase",				(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetAbraseWeapon),		_T("NS_Abrase"));
	m_pCmdMgr->Register("NS_RWNewessChange",		(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetWeaponNewnessChange), _T("NS_RWNewessChange"));
    m_pCmdMgr->Register("NS_DyeFashion",		    (NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetDyeFashion),          _T("NS_DyeFashion"));
	
	m_pCmdMgr->Register("NS_BagPsd",				(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetBagPassword),			_T("NS_BagPsd"));
	
	m_pCmdMgr->Register("NS_ItemReorder",			(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetContainerReorder),	_T("NS_ItemReorder"));
	m_pCmdMgr->Register("NS_ItemReorderEx",			(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetContainerReorderEx),  _T("NS_ItemReorderEx"));

	m_pCmdMgr->Register("NS_InitBaiBaoRecord",		(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetInitBaiBaoRecord),	_T("NS_InitBaiBaoRecord"));
	m_pCmdMgr->Register("NS_SingleBaiBaoRecord",	(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetSingleBaiBaoRecord),  _T("NS_SingleBaiBaoRecord"));
	m_pCmdMgr->Register("NS_WareOpen",				(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetWareOpen),			_T("NS_WareOpen") );
	m_pCmdMgr->Register("NS_BeGoldStone",			(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetBeGoldStone),		_T("NS_BeGoldStone"));

	m_pFrameMgr->RegisterEventHandle(_T("Bag_Extend"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemMgr::OnEventBagExtend));
	m_pFrameMgr->RegisterEventHandle(_T("MsgBox_BagExtend"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemMgr::OnEventBagExtendCheck));
	m_pFrameMgr->RegisterEventHandle(_T("Ware_Extend"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemMgr::OnEventWareExtend));
	m_pFrameMgr->RegisterEventHandle(_T("MsgBox_WareExtend"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemMgr::OnEventWareExtendCheck));
}

VOID ItemMgr::Destroy()
{
	if(NULL != m_pPocket)
	{
		m_pPocket->Destroy();
		SAFE_DEL(m_pPocket);
	}
	if(NULL != m_pThrone)
	{
		m_pThrone->Destroy();
		SAFE_DEL(m_pThrone);
	}
	if(NULL != m_pEquipment)
	{
		m_pEquipment->Destroy();
		SAFE_DEL(m_pEquipment);
	}
	if(NULL != m_pStorge)
	{
		m_pStorge->Destroy();
		SAFE_DEL(m_pStorge);
	}
	if(NULL != m_pQuest)
	{
		m_pQuest->Destroy();
		SAFE_DEL(m_pQuest);
	}
    if (NULL != m_pGuildStore)
    {
        m_pGuildStore->Destroy();
        SAFE_DEL(m_pGuildStore);
    }

	if(P_VALID(GetObj("GoldItemInfo")))
		KillObj("GoldItemInfo");

	m_pCmdMgr->UnRegister("NS_GetRoleInitState_Item",		(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetInitLocalPlayerItem));
	m_pCmdMgr->UnRegister("NS_GetRoleInitState_ItemCDTime",	(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetInitLocalPlayerItemCD));
	m_pCmdMgr->UnRegister("NS_GetRoleInitState_Suit",		(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetInitLocalPlayerSuit));
	m_pCmdMgr->UnRegister("NS_GetRoleInitState_Longhun",	(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetInitLocalPlayerLonghun));
	
	m_pCmdMgr->UnRegister("NS_Equip",					(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetEquipment));
	m_pCmdMgr->UnRegister("NS_Unequip",					(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetUnEquipment));
	m_pCmdMgr->UnRegister("NS_SwapWeapon",				(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetSwapWeapon));

	m_pCmdMgr->UnRegister("NS_ItemPosChange",			(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetItemPosChange));
	m_pCmdMgr->UnRegister("NS_ItemPosChangeEx",			(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetItemPosChangeEx));
	m_pCmdMgr->UnRegister("NS_NewItemAdd",				(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetNewItemAdd));
	m_pCmdMgr->UnRegister("NS_NewEquipAdd",				(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetNewEquipAdd));
	m_pCmdMgr->UnRegister("NS_ItemAdd",					(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetItemAdd));
	m_pCmdMgr->UnRegister("NS_ItemRemove",				(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetItemRemove));
	m_pCmdMgr->UnRegister("NS_ItemBind",				(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetItemBind));
	m_pCmdMgr->UnRegister("NS_EquipChange",				(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetEquipChange));
	m_pCmdMgr->UnRegister("NS_ItemCDUpdate",			(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetItemCDUpdate));

	m_pCmdMgr->UnRegister("NS_BagExtend",				(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetBagExtend));
	m_pCmdMgr->UnRegister("NS_WareExtend",				(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetWareExtend));

	m_pCmdMgr->UnRegister("NS_SuitNum",					(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetSuitNumChange));
	m_pCmdMgr->UnRegister("NS_LongHunOn",				(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetEquipLonghunOn));
	m_pCmdMgr->UnRegister("NS_LongHunOff",				(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetEquipLonghunOff));

	m_pCmdMgr->UnRegister("NS_IdentifyEquip",			(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetIdentifyEquip));
	m_pCmdMgr->UnRegister("NS_Abrase",					(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetAbraseWeapon));
	m_pCmdMgr->UnRegister("NS_RWNewessChange",			(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetWeaponNewnessChange));
	m_pCmdMgr->UnRegister("NS_DyeFashion",				(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetDyeFashion));
	
	m_pCmdMgr->UnRegister("NS_BagPsd",					(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetBagPassword));
	
	m_pCmdMgr->UnRegister("NS_ItemReorder",				(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetContainerReorder));
	m_pCmdMgr->UnRegister("NS_ItemReorderEx",			(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetContainerReorderEx));

	m_pCmdMgr->UnRegister("NS_InitBaiBaoRecord",		(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetInitBaiBaoRecord));
	m_pCmdMgr->UnRegister("NS_SingleBaiBaoRecord",		(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetSingleBaiBaoRecord));
	m_pCmdMgr->UnRegister("NS_WareOpen",				(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetWareOpen) );
	m_pCmdMgr->UnRegister("NS_BeGoldStone",			(NETMSGPROC)m_Trunk.sfp2(&ItemMgr::NetBeGoldStone) );

	m_pFrameMgr->UnRegisterEventHandler(_T("Bag_Extend"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemMgr::OnEventBagExtend));
	m_pFrameMgr->UnRegisterEventHandler(_T("MsgBox_BagExtend"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemMgr::OnEventBagExtendCheck));
	m_pFrameMgr->UnRegisterEventHandler(_T("Ware_Extend"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemMgr::OnEventWareExtend));
	m_pFrameMgr->UnRegisterEventHandler(_T("MsgBox_WareExtend"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemMgr::OnEventWareExtendCheck));
}

tstring ItemMgr::GetItemIconPath(DWORD dwTypeID)
{
	TCHAR szPath[X_LONG_NAME] = {0};
	const tagItemDisplayInfo* pDisplay = ItemProtoData::Inst()->FindItemDisplay(dwTypeID);
	if(P_VALID(pDisplay))
		_sntprintf(szPath, sizeof(szPath), _T("data\\ui\\Icon\\%s.tga"), pDisplay->szMaleIcon);
	
	return szPath;
}


DWORD ItemMgr::NetInitLocalPlayerItem(tagNS_GetRoleInitState_Item* pNetCmd,DWORD)
{
	m_pPocket->SetMaxSize(pNetCmd->n16SzBag);
	m_pStorge->SetMaxSize(pNetCmd->n16SzRoleWare);
	//接收前先清空数据
	m_pPocket->Destroy();
	m_pEquipment->Destroy();
	m_pStorge->Destroy();
	m_pThrone->Destroy();
	m_pQuest->Destroy();

	for(INT i=0,pos=0; i<pNetCmd->nNum; ++i)
	{
		tagItem item;
		memcpy(&item, pNetCmd->byData+pos, sizeof(tagItem));
		if(MIsEquipment(item.dwTypeID))
		{
			tagEquip equip;
			memcpy(&equip, pNetCmd->byData+pos, sizeof(tagEquip));
			pos += sizeof(tagEquip);
			Equipment* pEquip = new Equipment(equip);
			AddNewItem(pEquip);
		}
		else
		{
			pos += sizeof(tagItem);
			Item* pItem = new Item(item);
			AddNewItem(pItem);
		}
	}
	
	// 物品更新后发送武器切换，方便快捷栏同步
	tagGameEvent e(_T("Weapon_Swap"), NULL);
	m_pFrameMgr->SendEvent(&e);


	tagNC_GetRoleInitState m;
	m.eType = ERIT_FrindAndEnemy;
	m_pSession->Send(&m);

	return 0;
}


DWORD ItemMgr::NetInitLocalPlayerItemCD( tagNS_GetRoleInitState_ItemCDTime* pNetCmd, DWORD )
{
	m_mapCurStartTime.Clear();

	for(INT i=0,pos=0; i<pNetCmd->nNum; ++i)
	{
		tagCDTime time;
		memcpy(&time, pNetCmd->byData+pos, sizeof(tagCDTime));
		const tagItemProto* pProto = ItemProtoData::Inst()->FindItemProto(time.dwTypeID);
		if(P_VALID(pProto))
		{
			// 计算当前冷却时间和该物品冷却时间的差值，计算物品的开始计时间
			INT nIncMilliSecond = (INT)(time.dwTime - pProto->dwCooldownTime);
			DWORD dwStartTime = Kernel::Inst()->GetAccumTimeDW() + nIncMilliSecond;
			m_mapCurStartTime.Add(time.dwTypeID, dwStartTime);
		}
		
		pos += sizeof(tagCDTime);
	}

	return 0;
}



VOID ItemMgr::AddNewItem( Item* pItem )
{
	BOOL bRet = FALSE;// 添加物品的结果

	ItemContainer* pItemCon = GetContainer(pItem->GetConType());
	
	if(P_VALID(pItemCon))
	{
		bRet = pItemCon->Add(pItem);
	}
	
	if(bRet)// 添加成功发送游戏事件
	{
		tagUpdateLPItemEvent e(_T("ItemEvent_UpdateItem"),NULL);
		e.eType = pItem->GetConType();
		e.n16Index = pItem->GetPos();
		e.dwTypeID = pItem->GetItemTypeID();
		m_pFrameMgr->SendEvent(&e);
	}
	else // 没有成功则删除物品
	{
		SAFE_DEL(pItem);

		// 发送给服务器
	}
}

DWORD ItemMgr::NetEquipment( tagNS_Equip* pNetCmd, DWORD )
{
	if(0 != pNetCmd->dwErrorCode)
	{
		ShowItemErrorMsg(pNetCmd->dwErrorCode);
	}

	return 0;
}

DWORD ItemMgr::NetUnEquipment( tagNS_Unequip* pNetCmd, DWORD )
{
	if(0 != pNetCmd->dwErrorCode)
	{
		ShowItemErrorMsg(pNetCmd->dwErrorCode);
	}
	return 0;
}

DWORD ItemMgr::NetSwapWeapon( tagNS_SwapWeapon* pNetCmd, DWORD )
{
	if(0 != pNetCmd->dwErrorCode)
	{
		ShowItemErrorMsg(pNetCmd->dwErrorCode);
	}
	return 0;
}

BOOL ItemMgr::NetItemPosChange( tagNS_ItemPosChange* pNetCmd, DWORD )
{
	
	BOOL bRet = TRUE;

	//查找所在容器，没有则返回
	ItemContainer* pItemCon = GetContainer(pNetCmd->eConType);
	if(!P_VALID(pItemCon))
		return FALSE;
	
	//创建几个临时变量
	Item* pItem1 = NULL;
	Item* pItem2 = NULL;
	Item* pTempItem = NULL;
	DWORD dwTypeID1 = GT_INVALID;
	DWORD dwTypeID2 = GT_INVALID;
	//物品一没有找到
	pItem1 = pItemCon->GetItem(pNetCmd->n64Serial1);
	if(!P_VALID(pItem1))
		return FALSE;

	dwTypeID1 = pItem1->GetItemTypeID();
	
	if(GT_INVALID == pNetCmd->n64Serial2)
	{
		dwTypeID2 = dwTypeID1;

		pTempItem = pItemCon->Remove(pItem1->GetPos());
		if(pTempItem != pItem1)
			return FALSE;

		if(0 == pNetCmd->n16Num1)//如果数量为零直接删除
		{
			SAFE_DEL(pItem1);
		}
		else
		{
			pItem1->SetPos(pNetCmd->n16PosDst1);
			pItem1->SetItemQuantity(pNetCmd->n16Num1);
			bRet = bRet && pItemCon->Add(pItem1);
		}
	}
	else
	{
		if(pNetCmd->bCreateItem)
		{
			pTempItem = pItemCon->Remove(pItem1->GetPos());
			if(pTempItem != pItem1)
				return FALSE;
			
			if(0 == pNetCmd->n16Num1)//如果数量为零直接删除
			{
				SAFE_DEL(pItem1);
			}
			else
			{
				pItem1->SetPos(pNetCmd->n16PosDst1);
				bRet = bRet && pItemCon->Add(pItem1);
				pItem1->SetItemQuantity(pNetCmd->n16Num1);
			}
				//根据物品一创造物品二
			if(MIsEquipment(pItem1->GetItemTypeID()))
			{
				pItem2 = new Equipment(*((Equipment*)pItem1));
			}
			else
			{
				pItem2 = new Item(*pItem1);
			}

			dwTypeID2 = pItem2->GetItemTypeID();

			pItem2->SetItemId(pNetCmd->n64Serial2);
			pItem2->SetPos(pNetCmd->n16PosDst2);	
			bRet = bRet && pItemCon->Add(pItem2);
			pItem2->SetItemQuantity(pNetCmd->n16Num2);
		}
		else
		{
			pItem2 = pItemCon->GetItem(pNetCmd->n64Serial2);
			if(!P_VALID(pItem2))
				return FALSE;

			dwTypeID2 = pItem2->GetItemTypeID();
			
			Item* pTempItem = pItemCon->Remove(pItem1->GetPos());
			if(pTempItem != pItem1)
				return FALSE;
				pTempItem = pItemCon->Remove(pItem2->GetPos());
			if(pTempItem != pItem2)
				return FALSE;

			if(0 == pNetCmd->n16Num1)//如果数量为零直接删除
			{
				SAFE_DEL(pItem1);
			}
			else
			{
				pItem1->SetPos(pNetCmd->n16PosDst1);
				pItem1->SetItemQuantity(pNetCmd->n16Num1);
				bRet = bRet && pItemCon->Add(pItem1);
			}

			pItem2->SetPos(pNetCmd->n16PosDst2);
			pItem2->SetItemQuantity(pNetCmd->n16Num2);
			bRet = bRet && pItemCon->Add(pItem2);
		}
	}
	
	if(bRet)
	{
		tagUpdateLPItemEvent event(_T("ItemEvent_UpdateItem"),NULL);
		event.eType = pNetCmd->eConType;
		event.n16Index = pNetCmd->n16PosDst1;
		event.dwTypeID = dwTypeID1;
		m_pFrameMgr->SendEvent(&event);

		event.n16Index = pNetCmd->n16PosDst2;
		event.dwTypeID = dwTypeID2;
		m_pFrameMgr->SendEvent(&event);

		if( EICT_Equip == pNetCmd->eConType 
			&& (EEP_RightHand == pNetCmd->n16PosDst1 || EEP_LeftHand == pNetCmd->n16PosDst1
			|| EEP_RightHand == pNetCmd->n16PosDst2 || EEP_LeftHand == pNetCmd->n16PosDst2) )
		{
			tagGameEvent e(_T("Weapon_Swap"), NULL);
			m_pFrameMgr->SendEvent(&e);
		}
	}
	return bRet;
}

BOOL ItemMgr::NetItemPosChangeEx( tagNS_ItemPosChangeEx* pNetCmd, DWORD )
{
	
	BOOL bRet = TRUE;
	
	ItemContainer* pItemConSrc1 = GetContainer(pNetCmd->eConTypeSrc1);
	ItemContainer* pItemConSrc2 = GetContainer(pNetCmd->eConTypeSrc2);
	ItemContainer* pItemConDst1 = GetContainer(pNetCmd->eConTypeDst1);
	ItemContainer* pItemConDst2 = GetContainer(pNetCmd->eConTypeDst2);
	if( !P_VALID(pItemConSrc1) || !P_VALID(pItemConSrc2)
		|| !P_VALID(pItemConDst1) || !P_VALID(pItemConDst2) )
		return FALSE;
	
	Item* pItem1 = NULL;
	Item* pItem2 = NULL;
	Item* pTempItem = NULL;
	DWORD dwTypeID1 = GT_INVALID;
	DWORD dwTypeID2 = GT_INVALID;
	INT16 n16Index1 = 0;
	INT16 n16Index2 = 0;



	if(GT_INVALID != pNetCmd->n64Serial1)
	{
		pItem1 = pItemConSrc1->GetItem(pNetCmd->n64Serial1);
		if(!P_VALID(pItem1))
			return FALSE;

		pTempItem = pItemConSrc1->Remove(pItem1->GetPos());
		if(pTempItem != pItem1)
			return FALSE;

		dwTypeID1 = pItem1->GetItemTypeID();
		n16Index2 = pItem1->GetPos();
	}

	if(GT_INVALID != pNetCmd->n64Serial2)
	{
		pItem2 = pItemConSrc2->GetItem(pNetCmd->n64Serial2);
		if(!P_VALID(pItem2))
			return FALSE;

		pTempItem = pItemConSrc2->Remove(pItem2->GetPos());
		if(pTempItem != pItem2)
			return FALSE;

		dwTypeID2 = pItem2->GetItemTypeID();
		n16Index1 = pItem2->GetPos();
	}

	if(0 == pNetCmd->n16Num1)//如果数量为零直接删除
	{
		SAFE_DEL(pItem1);
	}
	
	if(P_VALID(pItem1))
	{
		pItem1->SetConType(pNetCmd->eConTypeDst1);
		pItem1->SetPos(pNetCmd->n16PosDst1);
		pItem1->SetItemQuantity(pNetCmd->n16Num1);
		bRet = bRet && pItemConDst1->Add(pItem1);
	}

	if(P_VALID(pItem2))
	{
		pItem2->SetConType(pNetCmd->eConTypeDst2);
		pItem2->SetPos(pNetCmd->n16PosDst2);
		pItem2->SetItemQuantity(pNetCmd->n16Num2);
		bRet = bRet && pItemConDst2->Add(pItem2);
	}

	if(bRet)
	{
		n16Index1 = pNetCmd->n16PosDst1 == GT_INVALID ? n16Index1 : pNetCmd->n16PosDst1;
		n16Index2 = pNetCmd->n16PosDst2 == GT_INVALID ? n16Index2 : pNetCmd->n16PosDst2;

		tagUpdateLPItemEvent event(_T("ItemEvent_UpdateItem"),NULL);
		event.eType = pNetCmd->eConTypeDst1;
		event.n16Index = n16Index1;
		event.dwTypeID = dwTypeID1;
		m_pFrameMgr->SendEvent(&event);

		event.eType	= pNetCmd->eConTypeDst2; 	
		event.n16Index = n16Index2;
		event.dwTypeID = dwTypeID2;
		m_pFrameMgr->SendEvent(&event);

		if(EICT_Bag == pNetCmd->eConTypeDst1)
		{
			event.eType	= EICT_Bag;
			event.n16Index = n16Index1;
			event.dwTypeID = dwTypeID2;
			m_pFrameMgr->SendEvent(&event);
		}
		else if(EICT_Bag == pNetCmd->eConTypeDst2)
		{
			event.eType = EICT_Bag;
			event.n16Index = n16Index2;
			event.dwTypeID = dwTypeID1;
			m_pFrameMgr->SendEvent(&event);
		}

		if( (EICT_Equip == pNetCmd->eConTypeSrc1 || EICT_Equip == pNetCmd->eConTypeSrc2) 
			&& (EEP_RightHand == n16Index1 || EEP_LeftHand == n16Index1
			|| EEP_RightHand == n16Index2 || EEP_LeftHand == n16Index2) )
		{
			tagGameEvent e(_T("Weapon_Swap"), NULL);
			m_pFrameMgr->SendEvent(&e);
		}

	}

	return bRet;
}

DWORD ItemMgr::NetNewItemAdd( tagNS_NewItemAdd* pNetCmd, DWORD )
{
	
	Item* pItem = new Item(pNetCmd->Item);
	if(P_VALID(pItem))
	{
		AddNewItem(pItem);
		
		//发送游戏事件
		ASSERT(P_VALID(pItem));
		tagCombatMsgForItemChange event(_T("CombatMsg_ItemAdd"), NULL);
		event.dwTypeID = pNetCmd->Item.dwTypeID;
		event.n16Num = pNetCmd->Item.n16Num;
		m_pFrameMgr->SendEvent(&event);

	}
	return 0;
}

DWORD ItemMgr::NetNewEquipAdd( tagNS_NewEquipAdd* pNetCmd, DWORD )
{
	
	Equipment* pEquip = new Equipment(pNetCmd->Equip);
	if(P_VALID(pEquip))
	{
		AddNewItem(pEquip);

		//发送游戏事件
		ASSERT(P_VALID(pEquip));
		tagCombatMsgForItemChange event(_T("CombatMsg_ItemAdd"), NULL);
		event.dwTypeID = pEquip->GetItemTypeID();
		event.n16Num = pEquip->GetItemQuantity();
		m_pFrameMgr->SendEvent(&event);
	
	}
	return 0;
}

DWORD ItemMgr::NetItemAdd( tagNS_ItemAdd* pNetCmd, DWORD )
{
	ItemContainer* pItemCon = GetContainer(pNetCmd->eConType);
	if(P_VALID(pItemCon))
	{
		Item* pItem = pItemCon->GetValue(pNetCmd->n16Index);
		if(P_VALID(pItem))
		{
			DWORD dwTypeID = pItem->GetItemTypeID();
			INT nQuantity = pNetCmd->n16Num - pItem->GetItemQuantity();

			pItem->SetItemQuantity(pNetCmd->n16Num);

			//发送游戏事件
			tagCombatMsgForItemChange event(_T("CombatMsg_ItemAdd"), NULL);
			event.dwTypeID = dwTypeID;
			event.n16Num = nQuantity;
			m_pFrameMgr->SendEvent(&event);

			tagUpdateLPItemEvent e(_T("ItemEvent_UpdateItem"),NULL);
			e.eType = pNetCmd->eConType;
			e.n16Index = pNetCmd->n16Index;
			e.dwTypeID = dwTypeID;
			m_pFrameMgr->SendEvent(&e);
		}
		
	}

	return 0;
}

DWORD ItemMgr::NetItemRemove( tagNS_ItemRemove* pNetCmd, DWORD )
{
	ItemContainer* pItemCon = GetContainer(pNetCmd->eConType);
	if(P_VALID(pItemCon))
	{
		Item* pItem = pItemCon->GetValue(pNetCmd->n16Index);
		if(P_VALID(pItem) && pNetCmd->n64Serial == pItem->GetItemId())
		{
			DWORD dwTypeID = pItem->GetItemTypeID();
			INT nQuantity = pItem->GetItemQuantity() - pNetCmd->n16Num;
			
			if(pNetCmd->n16Num > 0)
			{
				//如果是可多次使用的物品，n16Num是可使用的次数
				if(pItem->GetProto()->nMaxUseTimes > 1)
				{
					pItem->SetItemUseTimes(pNetCmd->n16Num);
					nQuantity = 0;
				}
				else
				{
					pItem->SetItemQuantity(pNetCmd->n16Num);
				}
			}
			else
			{
				Item* pItem1 = pItemCon->Remove(pItem->GetPos());
				if(pItem == pItem1)
				{
					SAFE_DEL(pItem);
				}
			}

			//发送游戏事件
			if(nQuantity > 0)
			{
				tagCombatMsgForItemChange event(_T("CombatMsg_ItemRemove"), NULL);
				event.dwTypeID = dwTypeID;
				event.n16Num = nQuantity;
				m_pFrameMgr->SendEvent(&event);
			}

			tagUpdateLPItemEvent e(_T("ItemEvent_UpdateItem"),NULL);
			e.eType = pNetCmd->eConType;
			e.n16Index = pNetCmd->n16Index;
			e.dwTypeID = dwTypeID;
			m_pFrameMgr->SendEvent(&e);
			
		}
	}

	return 0;
}

DWORD ItemMgr::NetItemBind( tagNS_ItemBind* pNetCmd, DWORD )
{
	ItemContainer* pItemCon = GetContainer(pNetCmd->eConType);
	if(P_VALID(pItemCon))
	{
		Item* pItem = pItemCon->GetItem(pNetCmd->n64Serial);
		if (P_VALID(pItem))
		{
			pItem->SetItemBind();
			pItem->SetTradeable(FALSE);
		}
	}

	return 0;
}

DWORD ItemMgr::NetEquipChange( tagNS_EquipChange* pNetCmd, DWORD )
{
	//--如果物品位置有效且为装备
	Equipment* pEquip = (Equipment*)ItemMgr::Inst()->GetPocketItemByID(pNetCmd->n64Serial);
	//--如果有该装备且装备ID与服务器传过来的装备ID相符
	if(P_VALID(pEquip) && MIsEquipment(pEquip->GetItemTypeID()))
	{
		//--更新装备数据
		pEquip->UpdateEquipment(pNetCmd->equipSpec);
		tagUpdateLPItemEvent eve(_T("ItemEvent_EquipChange"),NULL);
		eve.eType = pEquip->GetConType();
		eve.n16Index = pEquip->GetPos();
		m_pFrameMgr->SendEvent(&eve);
	}
	return 0;
}


DWORD ItemMgr::NetItemCDUpdate( tagNS_ItemCDUpdate* pNetCmd, DWORD )
{
	const tagItemProto* pProto = ItemProtoData::Inst()->FindItemProto(pNetCmd->dwTypeID);
	if(P_VALID(pProto))
	{
		// 计算当前冷却时间和该物品冷却时间的差值，计算物品的开始计时间
		INT nIncMilliSecond = (INT)(pNetCmd->dwCDTime - pProto->dwCooldownTime);
		DWORD dwStartTime = Kernel::Inst()->GetAccumTimeDW() + nIncMilliSecond;
		if(m_mapCurStartTime.IsExist(pNetCmd->dwTypeID))
			m_mapCurStartTime.ChangeValue(pNetCmd->dwTypeID, dwStartTime);
		else 
			m_mapCurStartTime.Add(pNetCmd->dwTypeID, dwStartTime);
		
		tagGameEvent eve(_T("tagUpdateItemCDEvent"), NULL);
		m_pFrameMgr->SendEvent(&eve);
	}
	return 0;
}

DWORD ItemMgr::NetAbraseWeapon( tagNS_Abrase* pNetCmd, DWORD )
{
	if(E_Success == pNetCmd->dwErrorCode)
	{
		Item* pItem = m_pPocket->GetItem(pNetCmd->n64WeaponSerial);
		if (P_VALID(pItem))
		{
			pItem->SetItemUseTimes(pNetCmd->nAttackTimes);
		}

		//发送游戏事件
		tagUpdateLPItemEvent eve(_T("ItemEvent_EquipChange"),NULL);
		eve.eType = pItem->GetConType();
		eve.n16Index = pItem->GetPos();
		m_pFrameMgr->SendEvent(&eve);
		
	}
	else
	{

	}
	return 0;
}


DWORD ItemMgr::NetWareOpen( tagNS_WareOpen* pNetCmd, DWORD )
{
	if( E_Success == pNetCmd->dwErrorCode )
	{
        // 打开仓库界面
        ItemFrame* pItemFrame = (ItemFrame*)m_pFrameMgr->GetFrame(_T("Item"));
        if (!P_VALID(pItemFrame))
            return 0;

        StorageFrame* pFrame = (StorageFrame*)m_pFrameMgr->GetFrame(_T("Storage"));
        if (!P_VALID(pFrame))
        {
            pFrame = (StorageFrame*)m_pFrameMgr->CreateFrame(_T("Item"), _T("Storage"), _T("StorageFrame"), 0);
        }

        if(P_VALID(pFrame))
        {
            pFrame->SetWalkWareEnable(true);
            pItemFrame->SetCurStrategy(EIST_ItemStorage);
        }
	}
	return 0;
}


ItemContainer* ItemMgr::GetContainer( EItemConType eType )
{
	switch(eType)
	{
	case EICT_Bag://行囊
		return m_pPocket;
	case EICT_Baibao://百宝袋
		return m_pThrone;
	case EICT_Equip://装备
		return m_pEquipment;
	case EICT_Quest://任务
		return m_pQuest;
	case EICT_RoleWare://仓库
		return m_pStorge;
    case EICT_GuildWare:// 帮派仓库
        return m_pGuildStore;
	default://如果不是以上容器则直接删除
		return NULL;
	}
	return NULL;
}

Item* ItemMgr::GetPocketItem( INT16 n16Pos )
{
	return m_pPocket->GetValue(n16Pos);
}

Item* ItemMgr::GetPocketItemByID( INT64 n64ID )
{
	return m_pPocket->GetItem(n64ID);
}

Equipment* ItemMgr::GetCurEquip( EEquipPos ePos )
{
	return (Equipment*)m_pEquipment->GetValue((INT16)ePos);
}

Equipment* ItemMgr::GetCurEquipByID( INT64 n64ID )
{
	return (Equipment*)m_pEquipment->GetItem(n64ID);
}


BOOL ItemMgr::IsNotEquip( EEquipPos ePos )
{
	Item* pItem = m_pEquipment->GetValue((INT16)ePos);
	return !P_VALID(pItem);
}

Item* ItemMgr::GetConItem( EItemConType ePos, INT16 n16Pos )
{
	ItemContainer* pCon = GetContainer(ePos);
	if(!P_VALID(pCon))
		return NULL;

	return pCon->GetValue(n16Pos);
}

Item* ItemMgr::GetConItemByID( EItemConType ePos, INT64 n64ID )
{
	ItemContainer* pCon = GetContainer(ePos);
	if(!P_VALID(pCon))
		return NULL;

	return pCon->GetItem(n64ID);
}

INT64 ItemMgr::GetSpecfuncItemID( EItemSpecFunc eType )
{
	INT64 n64Ret = GT_INVALID;
	if(P_VALID(m_pPocket))
	{
		n64Ret = m_pPocket->GetSpecFuncItemID(eType);
	}
	return n64Ret;
}

EItemTypeEx ItemMgr::GetHandWeaponType(EEquipPos ePos/*=EEP_RightHan*/)
{
	if( !P_VALID(m_pEquipment) )
		return EITE_Null;
	Item* pItem = m_pEquipment->GetValue((INT16)ePos);
	if(P_VALID(pItem) && MIsEquipment(pItem->GetItemTypeID()))
	{
		Equipment* pEquip = (Equipment*)pItem;
		return pEquip->GetItemTypeEx();
	}
	return EITE_Null;
}

VOID ItemMgr::GetItemCurCDTime( DWORD dwTypeID, FLOAT& fCDCount, FLOAT& fRestoreRatio )
{
	DWORD dwStartTime = m_mapCurStartTime.Peek(dwTypeID);
	const tagItemProto* pProto = ItemProtoData::Inst()->FindItemProto(dwTypeID);
	if(GT_VALID(dwStartTime) && P_VALID(pProto))
	{
		FLOAT fCDTime = (FLOAT)pProto->dwCooldownTime;
		//离上一次该类物品使用的时间
		INT nPassMilliSecord = Kernel::Inst()->GetAccumTimeDW() - dwStartTime;
		//倒计时时间
		fCDCount = fCDTime - nPassMilliSecord;	
		//倒计时旋转律
		fRestoreRatio = (fCDTime - fCDCount) / fCDTime + 0.000001f;
	}
}

INT16 ItemMgr::GetConBlank( EItemConType ePos, INT nIndex /*= 0*/ )
{
	ItemContainer* pCon = GetContainer(ePos);
	if(!P_VALID(pCon))
		return GT_INVALID;

	return pCon->GetBlank(nIndex);
}

DWORD ItemMgr::OnEventBagExtend( tagItemContainerExtend* pGameEvent )
{
	ASSERT(P_VALID(m_pPocket));
	INT nMaxSize = m_pPocket->MaxSize();
	if (nMaxSize >= SPACE_ALL_BAG)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ItemMsgError_CantExtend")]);
		return 0;
	}

	// 先重置状态位
	m_bNotEnoughYuanbao = false;
	m_bNotEnoughSilver	= false;
	
	// 计算需要消耗金钱还是元宝
	TCHAR szTemp[X_LONG_NAME] = {0};
	INT64 nExtendSilver = 0;
	INT nExtendYuanbao = 0;
	INT nExtendTimes = MCalBagExTimes(nMaxSize);
	if (pGameEvent->bUseSilver)
	{
		nExtendSilver = MCalBagExSilver(nExtendTimes);
		// 先判断金钱是否足够，等确认以后再提示
		if(nExtendSilver > CurrencyMgr::Inst()->GetBagSilver())
			m_bNotEnoughSilver = true;

		_sntprintf(szTemp, sizeof(szTemp)/sizeof(TCHAR), g_StrTable[_T("BagExtend_Check")], 
			CurrencyMgr::Inst()->GetSilverDisplay(nExtendSilver).c_str());
	}
	else
	{
		nExtendYuanbao = MCalBagExYuanBao(nExtendTimes);
		
		// 先判断元宝是否足够，等确认以后再提示
		if(nExtendYuanbao > CurrencyMgr::Inst()->GetBagYuanbao())
			m_bNotEnoughYuanbao = true;

		_sntprintf(szTemp, sizeof(szTemp)/sizeof(TCHAR), g_StrTable[_T("BagExtend_Check")], 
			CurrencyMgr::Inst()->GetYuanbaoDisplay(nExtendYuanbao).c_str());
	}
	// 记录消耗的是什么
	m_bUseSilverExtend = pGameEvent->bUseSilver;
	if (GetObj("BagExtendBox"))
		KillObj("BagExtendBox");

	CreateObj("BagExtendBox", "MsgBox");
	TObjRef<MsgBox>("BagExtendBox")->Init(_T(""), szTemp, _T("MsgBox_BagExtend"));

	return 0;
}

DWORD ItemMgr::OnEventWareExtend( tagItemContainerExtend* pGameEvent )
{
	ASSERT(P_VALID(m_pStorge));
	INT nMaxSize = m_pStorge->MaxSize();
	if (nMaxSize >= SPACE_ALL_WARE)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ItemMsgError_CantExtend")]);
		return 0;
	}
	
	// 先重置状态位
	m_bNotEnoughYuanbao = false;
	m_bNotEnoughSilver	= false;

	// 计算需要消耗金钱还是元宝
	TCHAR szTemp[X_LONG_NAME] = {0};
	INT64 nExtendSilver = 0;
	INT nExtendYuanbao = 0;
	INT nExtendTimes = MCalWareExTimes(nMaxSize);
	if (pGameEvent->bUseSilver)
	{
		nExtendSilver = MCalWareExSilver(nExtendTimes);

		// 先判断金钱是否足够，等确认以后再提示
		if(nExtendSilver > CurrencyMgr::Inst()->GetBagSilver())
			m_bNotEnoughSilver = true;

		_sntprintf(szTemp, sizeof(szTemp)/sizeof(TCHAR), g_StrTable[_T("WareExtend_Check")], 
			CurrencyMgr::Inst()->GetSilverDisplay(nExtendSilver).c_str());
	}
	else
	{
		nExtendYuanbao = MCalWareExYuanBao(nExtendTimes);

		// 先判断元宝是否足够，等确认以后再提示
		if(nExtendYuanbao > CurrencyMgr::Inst()->GetBagYuanbao())
			m_bNotEnoughYuanbao = true;

		_sntprintf(szTemp, sizeof(szTemp)/sizeof(TCHAR), g_StrTable[_T("WareExtend_Check")], 
			CurrencyMgr::Inst()->GetYuanbaoDisplay(nExtendYuanbao).c_str());
	}
	// 记录消耗的是什么
	m_bUseSilverExtend = pGameEvent->bUseSilver;
	if (GetObj("WareExtendBox"))
		KillObj("WareExtendBox");

	CreateObj("WareExtendBox", "MsgBox");
	TObjRef<MsgBox>("WareExtendBox")->Init(_T(""), szTemp, _T("MsgBox_WareExtend"));

	return 0;
}

DWORD ItemMgr::OnEventBagExtendCheck( tagMsgBoxEvent* pGameEvent )
{
	if (MBF_OK == pGameEvent->eResult)
	{
		//if(m_bNotEnoughSilver)
		//{
		//	CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("CurrencyError_NotEnoughSilver")]);
		//	return 0;
		//}
		//else if(m_bNotEnoughYuanbao)
		//{
		//	CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("CurrencyError_NotEnoughYuanbao")]);
		//	return 0;
		//}

        Item* pItem = GetFirstSpecFuncItemFromPocket(EISF_BagExtend);
        if (!P_VALID(pItem))
        {
            CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("BagExtend_Error")]);
            return 0;
        }

		tagNC_BagExtend m;
		m.dwNPCID = QuestMgr::Inst()->GetTalkNPCGlobalID();
		m.n64Serial = pItem->GetItemId();
		m_pSession->Send(&m);
	}
	return 0;
}

DWORD ItemMgr::OnEventWareExtendCheck( tagMsgBoxEvent* pGameEvent )
{
	if (MBF_OK == pGameEvent->eResult)
	{
		//if(m_bNotEnoughSilver)
		//{
		//	CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("CurrencyError_NotEnoughSilver")]);
		//	return 0;
		//}
		//else if(m_bNotEnoughYuanbao)
		//{
		//	CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("CurrencyError_NotEnoughYuanbao")]);
		//	return 0;
		//}

        Item* pItem = GetFirstSpecFuncItemFromPocket(EISF_RoleWareExtend);
        if (!P_VALID(pItem))
        {
            CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("WareExtend_Error")]);
            return 0;
        }

		tagNC_WareExtend m;
		m.dwNPCID = QuestMgr::Inst()->GetTalkNPCGlobalID();
		m.n64Serial = pItem->GetItemId();
		m_pSession->Send(&m);
	}
	return 0;
}

DWORD ItemMgr::NetBagExtend( tagNS_BagExtend* pNetCmd, DWORD )
{
	if (E_Success == pNetCmd->dwErrorCode)
	{
		ASSERT(P_VALID(m_pPocket));
		m_pPocket->SetMaxSize(pNetCmd->n16BagNum);

		tagGameEvent event(_T("ItemEvent_BagExtend"),NULL);
		m_pFrameMgr->SendEvent(&event);
	}
	return 0;
}

DWORD ItemMgr::NetWareExtend( tagNS_WareExtend* pNetCmd, DWORD )
{
	if (E_Success == pNetCmd->dwErrorCode)
	{
		ASSERT(P_VALID(m_pStorge));
		m_pStorge->SetMaxSize(pNetCmd->n16WareNum);

		tagGameEvent event(_T("ItemEvent_WareExtend"),NULL);
		m_pFrameMgr->SendEvent(&event);
	}
	return 0;
}

DWORD ItemMgr::NetSuitNumChange( tagNS_SuitNum* pNetCmd, DWORD )
{
	if (m_mapSuitNum.IsExist(pNetCmd->dwSuitID))
	{
		m_mapSuitNum.ChangeValue(pNetCmd->dwSuitID, pNetCmd->n8Num);
	}
	else
	{
		m_mapSuitNum.Add(pNetCmd->dwSuitID, pNetCmd->n8Num);
	}
	return 0;
}

VOID ItemMgr::ShowItemErrorMsg( DWORD dwErrorCode )
{
	if( (dwErrorCode >= E_Item_LevelLimit && dwErrorCode <= E_Item_AttA_Limit)
		|| (dwErrorCode >= E_BagPsd_Exist && dwErrorCode <= E_BagPsd_Error)
		|| (dwErrorCode >= E_Equip_OnFailed && dwErrorCode <= E_Equip_VocationLimit) )
	{
		TCHAR szTmp[X_SHORT_NAME] = {0};
		_sntprintf(szTmp,sizeof(szTmp)/sizeof(TCHAR), _T("ItemMsgError%d"), dwErrorCode);
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[szTmp]);
	}
}

DWORD ItemMgr::NetBagPassword( tagNS_BagPsd* pNetCmd, DWORD )
{
	if (E_Success == pNetCmd->dwErrorCode)
	{
		// 设置当前密码状态
		RoleMgr::Inst()->GetLocalPlayer()->SetRoleStateEx(pNetCmd->dwRoleStateEx);
		
		switch (m_eEncryptState)
		{
		case EPES_EnterPassword:
			{
				m_pFrameMgr->SendEvent(&tagGameEvent(_T("Open_Package"), NULL));
			}
			break;
		case EPES_SetupPassword:
			{
				CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("PocketEncrypt_PromptSetup")]);
			}
			break;
		case EPES_ModifyPassword:
			{
				CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("PocketEncrypt_PromptModify")]);
			}
			break;
		case EPES_CancelPassword:
			{
				CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("PocketEncrypt_PromptCancel")]);
			}
			break;
		}
		// 处理完了就置为默认值
		m_eEncryptState = EPES_Null;
	}
	else if (E_BagPsd_OK == pNetCmd->dwErrorCode)
	{
		if(P_VALID(GetObj("ModPsdSecond")))
			KillObj("ModPsdSecond");

		CreateObj("ModPsdSecond", "MsgInputBox");
		TObjRef<MsgInputBox>("ModPsdSecond")->Init(
			g_StrTable[_T("PocketEncrypt_CaptionModify")],g_StrTable[_T("PocketEncrypt_TextNewPassword")],
			_T("MsgBox_ModPsdSecond"),GT_INVALID, MBIT_Password);
	}
	else
	{
		ShowItemErrorMsg(pNetCmd->dwErrorCode);
		if(!RoleMgr::Inst()->GetLocalPlayer()->GetRoleStateEx(ERSE_BagPsdPass))
		{
			m_pFrameMgr->SendEvent(&tagGameEvent(_T("Close_Package_UI"), NULL));
		}
	}
	
	return 0;
}

DWORD ItemMgr::NetWeaponNewnessChange( tagNS_RWNewessChange* pNetCmd, DWORD )
{
	Item* pItem = m_pEquipment->GetValue(EEP_RightHand);
	if(P_VALID(pItem))
	{
		pItem->SetItemUseTimes(pNetCmd->nAttackTimes);
	}

	//发送游戏事件
	tagUpdateLPItemEvent eve(_T("ItemEvent_EquipChange"),NULL);
	eve.eType = pItem->GetConType();
	eve.n16Index = pItem->GetPos();
	m_pFrameMgr->SendEvent(&eve);
	return 0;
}

DWORD ItemMgr::NetContainerReorder( tagNS_ItemReorder* pNetCmd, DWORD )
{
	ItemContainer* pItemCon = GetContainer(pNetCmd->eConType);
	if(P_VALID(pItemCon))
	{
		if(E_Success == pNetCmd->dwErrorCode)
		{
			TMap<INT16, Item*> TempMap;
			INT pos = 0;
			for(INT i=0; i<pNetCmd->n16ItemNum; ++i)
			{
				INT16 n16OldPos;
				memcpy(&n16OldPos, pNetCmd->n16OldIndex+pos, sizeof(INT16));
				Item* pItem = pItemCon->Remove(n16OldPos);
				if(P_VALID(pItem))
				{
					pItem->SetPos(pos);
					TempMap.Add(n16OldPos, pItem);				
				}
				pos++;
			}

			Item* pTemp = NULL;
			INT16 n16Pos = 0;
			TempMap.ResetIterator();
			while(TempMap.PeekNext(n16Pos,pTemp))
			{
				pItemCon->Add(pTemp);
				tagUpdateLPItemEvent event(_T("ItemEvent_UpdateItem"), NULL);
				event.eType = pNetCmd->eConType;
				event.n16Index = n16Pos;
				event.dwTypeID = pTemp->GetItemTypeID();
				m_pFrameMgr->SendEvent(&event);

				event.n16Index = pTemp->GetPos();
				m_pFrameMgr->SendEvent(&event);
			}
			CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("PocketSettle_SettleFinish")]);
		}
		else
		{
			CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("PocketSettle_SettleFailed")]);
		}
		pItemCon->Unlock(true);
	}
	return 0;
}

DWORD ItemMgr::NetContainerReorderEx( tagNS_ItemReorderEx* pNetCmd, DWORD )
{
	ItemContainer* pItemCon = GetContainer(pNetCmd->eConType);
	if(P_VALID(pItemCon))
	{
		if(E_Success == pNetCmd->dwErrorCode)
		{
			TMap<INT16, Item*> TempMap;
			INT pos = 0;
			for(INT i=0; i<pNetCmd->n16ItemNum; ++i)
			{
				tagItemOrder m;
				memcpy(&m, pNetCmd->byData+pos, sizeof(tagItemOrder));
				Item* pItem = pItemCon->Remove(m.n16OldIndex);
				if(P_VALID(pItem))
				{
					pItem->SetPos(m.n16NewIndex);
					TempMap.Add(m.n16OldIndex, pItem);
				}
				
				pos += sizeof(tagItemOrder);
			}
			
			Item* pTemp = NULL;
			INT16 n16Pos = 0;
			TempMap.ResetIterator();
			while(TempMap.PeekNext(n16Pos,pTemp))
			{
				pItemCon->Add(pTemp);
				tagUpdateLPItemEvent event(_T("ItemEvent_UpdateItem"), NULL);
				event.eType = pNetCmd->eConType;
				event.n16Index = n16Pos;
				event.dwTypeID = pTemp->GetItemTypeID();
				m_pFrameMgr->SendEvent(&event);

				event.n16Index = pTemp->GetPos();
				m_pFrameMgr->SendEvent(&event);
			}
			CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("PocketSettle_SettleFinish")]);
		}
		else
		{
			CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("PocketSettle_SettleFailed")]);
		}
		pItemCon->Unlock(true);
	}
	return 0;
}

DWORD ItemMgr::NetIdentifyEquip( tagNS_IdentifyEquip* pNetCmd, DWORD )
{
	if(pNetCmd->dwErrorCode != E_Success)
	{
		if(pNetCmd->dwErrorCode == E_Item_LevelLimit)
		{
			CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ItemMsgError_IdentifyLevelLimit")]);
		}
		else
		{
			ShowItemErrorMsg(pNetCmd->dwErrorCode);
		}
	}
	return 0;
}

DWORD ItemMgr::NetInitLocalPlayerSuit( tagNS_GetRoleInitState_Suit* pNetCmd, DWORD )
{
	m_mapSuitNum.Clear();

	for(INT i=0,pos=0; i<pNetCmd->nSuitNum; ++i)
	{
		tagSuitInit item;
		memcpy(&item, pNetCmd->byData+pos, sizeof(tagSuitInit));
		m_mapSuitNum.Add(item.dwSuitID, (INT8)item.nEquipNum);
		
		pos += sizeof(tagSuitInit);
	}
	return 0;
}

DWORD ItemMgr::NetInitLocalPlayerLonghun( tagNS_GetRoleInitState_Longhun* pNetCmd, DWORD )
{
	m_mapLonghunOn.Clear();

	for(INT i=0; i<pNetCmd->n8Num; ++i)
	{
		m_mapLonghunOn.Add(pNetCmd->n16EquipPos[i], true);
	}
	return 0;
}

DWORD ItemMgr::NetEquipLonghunOn( tagNS_LongHunOn* pNetCmd, DWORD )
{
	INT16 n16Pos = m_mapLonghunOn.Peek(pNetCmd->n16EquipPos);
	if(GT_VALID(n16Pos))
	{
		m_mapLonghunOn.ChangeValue(pNetCmd->n16EquipPos, true);
	}
	else
	{
		m_mapLonghunOn.Add(pNetCmd->n16EquipPos, true);
	}
	return 0;
}

DWORD ItemMgr::NetEquipLonghunOff( tagNS_LongHunOff* pNetCmd, DWORD )
{
	m_mapLonghunOn.Erase(pNetCmd->n16EquipPos);

	return 0;
}

bool ItemMgr::GetLonghun( EEquipPos ePos, INT64 id )
{
	Item* pItem = m_pEquipment->GetValue((INT16)ePos);
	
	if(!P_VALID(pItem) || pItem->GetItemId() != id)
		return false;

	return m_mapLonghunOn.Peek((INT16)ePos) == GT_INVALID ? false : true;
}

DWORD ItemMgr::NetDyeFashion( tagNS_DyeFashion* pNetCmd, DWORD )
{

    switch (pNetCmd->dwErrCode)
    {
    case E_Success:
        CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("DyeFashionErr5")]);
    	break;
    case E_Dye_OtherItem:
        CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("DyeFashionErr1")]);
        break;
    case E_Dye_Exist:
        CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("DyeFashionErr2")]);
        break;
    case E_Dye_LvLimit:
        CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("DyeFashionErr3")]);
        break;
    default:
        CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("DyeFashionErr4")]);
    }

    return 0;
}

DWORD ItemMgr::NetInitBaiBaoRecord( tagNS_InitBaiBaoRecord* pNetCmd, DWORD )
{
	// 清空列表
	m_listBaiBaoRecord.Clear();

	for(INT i=0, pos=0; i<pNetCmd->n16Num; ++i)
	{
		tagBaiBaoRecord* pRecord = (tagBaiBaoRecord*)(pNetCmd->byData+pos);
		AddNewBaiBaoRecord(pRecord);

		pos += pRecord->n16Size;
	}

	// 发送游戏事件
	m_pFrameMgr->SendEvent(&tagGameEvent(_T("ItemEvent_BaiBaoRecord"), NULL));
	
	return 0;
}

DWORD ItemMgr::NetSingleBaiBaoRecord( tagNS_SingleBaiBaoRecord* pNetCmd, DWORD )
{
	AddNewBaiBaoRecord(&pNetCmd->sRecord);

	// 发送游戏事件
	m_pFrameMgr->SendEvent(&tagGameEvent(_T("ItemEvent_BaiBaoRecord"), NULL));

	return 0;
}

VOID ItemMgr::AddNewBaiBaoRecord( tagBaiBaoRecord* pRecord )
{
	if(!P_VALID(pRecord))
		return;
	
	// 赋值
	tagBaiBaoRecordC sRecord;
	sRecord.dwRoleID	= pRecord->dwRoleID;
	sRecord.eType		= (EBaiBaoRecordType)pRecord->n16Type;
	sRecord.dwTypeID	= pRecord->dwTypeID;
	sRecord.sTime		= (tagDWORDTime)pRecord->dwTime;
	sRecord.strWords	= pRecord->szWords;
	// 存入列表
	m_listBaiBaoRecord.PushBack(sRecord);
}

VOID ItemMgr::GetBaiBaoList( TList<tagBaiBaoRecordC> &listRecord )
{
	// 如果没有初始化，则发送请求
	if(m_bNotInitBaiBaoRecord)
	{
		tagNC_InitBaiBaoRecord cmd;
		m_pSession->Send(&cmd);
		
		// 不再请求第二次
		m_bNotInitBaiBaoRecord = false;
	}

	listRecord = m_listBaiBaoRecord;
}

void ItemMgr::ClearGuildWare()
{
    ASSERT(P_VALID(m_pGuildStore));
    TMap<INT64, Item*> mapContainer;
    m_pGuildStore->Clear(mapContainer);

    INT64 n64ItemId = 0;
    Item* pTemp = NULL;

    mapContainer.ResetIterator();
    while (mapContainer.PeekNext(n64ItemId, pTemp))
    {
        if (P_VALID(pTemp))
        {
            pTemp->SetOperable(true);
            SAFE_DEL(pTemp);
        }
    }
}

void ItemMgr::OnGetGuildWare( tagNS_GetGuildWare* pMsg )
{
    ASSERT(P_VALID(m_pGuildStore));

    m_pGuildStore->SetMaxSize(pMsg->nSzGuildWare);
    
    size_t offset = 0;
    for (int i = 0; i < pMsg->nItemNum; ++i)
    {
        tagGuildWareUpdate* pData = (tagGuildWareUpdate*)(pMsg->byData + offset);
        
        switch (pData->eType)
        {
        case EGWU_Change:
            {
                tagItem* pItemData = (tagItem*)pData->byData;
                
                // 先移除原位置
                 Item* pItemRemove = m_pGuildStore->Remove(pItemData->n16Index);
                 if (P_VALID(pItemRemove))
                    SAFE_DEL(pItemRemove);
                 Item* pItemRepetition = m_pGuildStore->GetItem(pItemData->n64Serial);
                 if (P_VALID(pItemRepetition))
                 {
                    pItemRemove = m_pGuildStore->Remove(pItemRepetition->GetPos());
                    if (P_VALID(pItemRemove))
                        SAFE_DEL(pItemRemove);
                 }

                 // 插入新的
                if (MIsEquipment(pItemData->dwTypeID))
                {
                    tagEquip* pEquipData = (tagEquip*)pData->byData;
                    Equipment* pEquip = new Equipment(*pEquipData);
                    m_pGuildStore->Add(pEquip);
                }
                else
                {
                    tagItem* pItemData = (tagItem*)pData->byData;
                    Item* pItem = new Item(*pItemData);
                    m_pGuildStore->Add(pItem);
                }
            }
        	break;
        case EGWU_Delete:
            {
                Item* pItemRemove = m_pGuildStore->Remove(pData->nIndex);
                if (P_VALID(pItemRemove))
                    SAFE_DEL(pItemRemove);
            }
            break;
        }

        offset += pData->Size();
        
    }

    // 发送游戏事件
    tagGameEvent e(_T("InitGuildWareEvent"), NULL);
    m_pFrameMgr->SendEvent(&e);
}

Item* ItemMgr::GetFirstSpecFuncItemFromPocket( EItemSpecFunc eItemSpecFunc )
{
	return m_pPocket->GetSpecFuncItem( EICT_Bag, eItemSpecFunc );
}	

void ItemMgr::LoadBeGoldStoneItemTable()
{
	if(P_VALID(GetObj("GoldItemInfo")))
		KillObj("GoldItemInfo");
	m_pVarContainer = CreateObj("GoldItemInfo", "VarContainer");
	const TCHAR* szItemTable = _T("data\\system\\attdata\\BeGoldStone_Info.xml");
	list<tstring> listItemTable;
	m_pVarContainer->Load("VFS_System", szItemTable, "id", &listItemTable);
	list<tstring>::iterator iter = listItemTable.begin();

	for( ; iter!=listItemTable.end(); iter++ )
	{
		tagBeGoldStoneItemTable GoldItemInfo;
		GoldItemInfo.dwSrcItemID = m_pVarContainer->GetDword( _T("SrcItemID"), (*iter).c_str(), GT_INVALID );
		GoldItemInfo.dwDestItemID = m_pVarContainer->GetDword( _T("DestItemID"), (*iter).c_str(), GT_INVALID );
		GoldItemInfo.dwGoldStoneType = m_pVarContainer->GetDword( _T("GoldStoneType"), (*iter).c_str(), GT_INVALID );
		m_vecBeGoldStoneItemTable.push_back( GoldItemInfo );
	}
}

bool ItemMgr::IsInGoldStoneTable( DWORD dwItemId, DWORD dwGoldStoneType )
{
	for( size_t i = 0; i < m_vecBeGoldStoneItemTable.size(); i++ )
	{
		if( dwItemId == m_vecBeGoldStoneItemTable[i].dwSrcItemID &&
			dwGoldStoneType == m_vecBeGoldStoneItemTable[i].dwGoldStoneType )
		{
			return true;
		}
	}
	return false;
}

Item* ItemMgr::GetFirstSpecFuncItemFromPocket( EItemSpecFunc eItemSpecFunc, DWORD dwItemTypeID, BOOL bEnableType )
{
	if( !bEnableType )
	{
		return m_pPocket->GetSpecFuncItem( EICT_Bag, eItemSpecFunc );
	}
	else
	{
		return m_pPocket->GetSpecFuncItem( EICT_Bag, eItemSpecFunc, dwItemTypeID );
	}
}


DWORD ItemMgr::NetBeGoldStone( tagNS_BeGoldStone *pNetCmd, DWORD )
{

	switch (pNetCmd->dwErrCode)
	{
	case E_Success:
		CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("GoldStoneSuccess")]);
		break;
	case E_GoldStone_SrcItemNotExist:		
	case E_GoldStone_NoConfigFile:
	case E_GoldStone_CreateDestItemFail:
	case E_GoldStone_MissingMatch:
	case E_GoldStone_OtherError:
		CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("GoldStoneFail")]);
		break;
	}

	return 0;
}