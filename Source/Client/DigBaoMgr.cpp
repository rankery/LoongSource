#include "stdafx.h"
#include "DigBaoMgr.h"
#include "CombatSysUtil.h"
#include "EffectMgr.h"
#include "..\WorldDefine\msg_digbao.h"
#include "RoleMgr.h"
#include "CombatSysUtil.h"
#include "PlayerNameTab.h"
#include "ChatFrame.h"
#include "MapLogicData.h"
#include "ItemProtoData.h"
#include "CreatureData.h"
#include "..\WorldDefine\specfunc_define.h"
#include "ItemMgr.h"
#include "Item.h"

DigBaoMgr::DigBaoMgr( void ):m_Trunk(this)
{
	
}

DigBaoMgr::~DigBaoMgr( void )
{

}

DigBaoMgr g_DigBaoMgr;
DigBaoMgr* DigBaoMgr::Inst()
{
	return &g_DigBaoMgr;
}

void DigBaoMgr::Init()
{
	m_pUtil =  TObjRef<Util>();
	m_pCmdMgr = TObjRef<NetCmdMgr>();
	m_pSession = TObjRef<NetSession>();
	m_pFrameMgr = TObjRef<GameFrameMgr>();

	//--注册游戏事件
	m_pFrameMgr->RegisterEventHandle(_T("tagDigBaoEvent"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&DigBaoMgr::OnDigBaoEvent));
	m_pFrameMgr->RegisterEventHandle(_T("tagDungeonEnterEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&DigBaoMgr::OnDungeonEnterEvent));
	m_pFrameMgr->RegisterEventHandle(_T("tagGodsOfPracticeEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&DigBaoMgr::OnGodsOfPracticeEvent));
	m_pFrameMgr->RegisterEventHandle(_T("ItemEvent_UnlockChest"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&DigBaoMgr::OnTheRoleOfItemEvent));

	//--注册网络消息
	m_pCmdMgr->Register("NS_DiaBao",			(NETMSGPROC)m_Trunk.sfp2(&DigBaoMgr::OnNS_DiaBao),				_T("NS_DiaBao"));
	m_pCmdMgr->Register("NS_NormalTreasureMap",	(NETMSGPROC)m_Trunk.sfp2(&DigBaoMgr::OnNS_NormalTreasureMap),	_T("NS_NormalTreasureMap"));
	m_pCmdMgr->Register("NS_OldTreasureMap",	(NETMSGPROC)m_Trunk.sfp2(&DigBaoMgr::OnNS_OldTreasureMap),		_T("NS_OldTreasureMap"));
	m_pCmdMgr->Register("NS_RareTreasureMap",	(NETMSGPROC)m_Trunk.sfp2(&DigBaoMgr::OnNS_RareTreasureMap),		_T("NS_RareTreasureMap"));

	m_pCmdMgr->Register("NS_ChatBillike",		(NETMSGPROC)m_Trunk.sfp2(&DigBaoMgr::OnNS_ChatBillike),			_T("NS_ChatBillike"));
	m_pCmdMgr->Register("NS_ChatBadOldSeal",	(NETMSGPROC)m_Trunk.sfp2(&DigBaoMgr::OnNS_ChatBadOldSeal),		_T("NS_ChatBadOldSeal"));
	m_pCmdMgr->Register("NS_Plutus",			(NETMSGPROC)m_Trunk.sfp2(&DigBaoMgr::OnNS_Plutus),				_T("NS_Plutus"));
	m_pCmdMgr->Register("NS_AwakenGods",		(NETMSGPROC)m_Trunk.sfp2(&DigBaoMgr::OnNS_AwakenGods),			_T("NS_AwakenGods"));
	m_pCmdMgr->Register("NS_GodsOfPractice",	(NETMSGPROC)m_Trunk.sfp2(&DigBaoMgr::OnNS_GodsOfPractice),		_T("NS_GodsOfPractice"));
	m_pCmdMgr->Register("NS_MonsterAttackCity",	(NETMSGPROC)m_Trunk.sfp2(&DigBaoMgr::OnNS_MonsterAttackCity),	_T("NS_MonsterAttackCity"));
	m_pCmdMgr->Register("NS_RoleGetChestItem",	(NETMSGPROC)m_Trunk.sfp2(&DigBaoMgr::OnNS_RoleGetChestItem),	_T("NS_RoleGetChestItem"));
}

void DigBaoMgr::Destroy()
{
	//--注销游戏事件
	m_pFrameMgr->UnRegisterEventHandler(_T("tagDigBaoEvent"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&DigBaoMgr::OnDigBaoEvent));
	m_pFrameMgr->UnRegisterEventHandler(_T("tagDungeonEnterEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&DigBaoMgr::OnDungeonEnterEvent));
	m_pFrameMgr->UnRegisterEventHandler(_T("tagGodsOfPracticeEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&DigBaoMgr::OnGodsOfPracticeEvent));
	m_pFrameMgr->UnRegisterEventHandler(_T("ItemEvent_UnlockChest"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&DigBaoMgr::OnTheRoleOfItemEvent));

	//--注销网络消息
	m_pCmdMgr->UnRegister("NS_DiaBao",				(NETMSGPROC)m_Trunk.sfp2(&DigBaoMgr::OnNS_DiaBao));
	m_pCmdMgr->UnRegister("NS_NormalTreasureMap",	(NETMSGPROC)m_Trunk.sfp2(&DigBaoMgr::OnNS_NormalTreasureMap));
	m_pCmdMgr->UnRegister("NS_OldTreasureMap",		(NETMSGPROC)m_Trunk.sfp2(&DigBaoMgr::OnNS_OldTreasureMap));
	m_pCmdMgr->UnRegister("NS_RareTreasureMap",		(NETMSGPROC)m_Trunk.sfp2(&DigBaoMgr::OnNS_RareTreasureMap));

	m_pCmdMgr->UnRegister("NS_ChatBillike",			(NETMSGPROC)m_Trunk.sfp2(&DigBaoMgr::OnNS_ChatBillike));
	m_pCmdMgr->UnRegister("NS_ChatBadOldSeal",		(NETMSGPROC)m_Trunk.sfp2(&DigBaoMgr::OnNS_ChatBadOldSeal));
	m_pCmdMgr->UnRegister("NS_Plutus",				(NETMSGPROC)m_Trunk.sfp2(&DigBaoMgr::OnNS_Plutus));
	m_pCmdMgr->UnRegister("NS_AwakenGods",			(NETMSGPROC)m_Trunk.sfp2(&DigBaoMgr::OnNS_AwakenGods));
	m_pCmdMgr->UnRegister("NS_GodsOfPractice",		(NETMSGPROC)m_Trunk.sfp2(&DigBaoMgr::OnNS_GodsOfPractice));
	m_pCmdMgr->UnRegister("NS_MonsterAttackCity",	(NETMSGPROC)m_Trunk.sfp2(&DigBaoMgr::OnNS_MonsterAttackCity));
	m_pCmdMgr->UnRegister("NS_RoleGetChestItem",	(NETMSGPROC)m_Trunk.sfp2(&DigBaoMgr::OnNS_RoleGetChestItem));

	//--
	if( P_VALID(GetObj("DungeonEnterMsgBox")) )
		KillObj("DungeonEnterMsgBox");
	if( P_VALID(GetObj("GodsOfPracticeMsgBox")) )
		KillObj("GodsOfPracticeMsgBox");
}

void DigBaoMgr::Update()
{
	if( m_vecDigBaoCast.size()==0 )
		return;
	vector<tagDigBaoCast>::iterator iter = m_vecDigBaoCast.begin();
	if( iter!=m_vecDigBaoCast.end() )
	{
		OnDigBaoCast(*iter);
		iter = m_vecDigBaoCast.erase(iter);
	}
}

DWORD DigBaoMgr::OnDigBaoEvent( tagDigBaoEvent* pGameEvent )
{
	tagNC_DiaBao msg;
	msg.dwPlayerID = RoleMgr::Inst()->GetLocalPlayerID();
	msg.dwItemID = pGameEvent->dwTargetID;
	msg.dwItemTypdID = pGameEvent->dwTargetTypeID;
	m_pSession->Send(&msg);
	return 0;
}

DWORD DigBaoMgr::OnNS_DiaBao( tagNS_DiaBao* pMsg, DWORD dwParam )
{
	switch(pMsg->dwErrorCode)
	{
	case E_Success:
		{
			EffectMgr::Inst()->PlayEffect( pMsg->v3Pos, 0.0f, _T("txqt08"), NULL, EffectMgr::EEST_XYZ );
		}
		break;
	default:
		CombatSysUtil::Inst()->ShowScreenCenterMsgEx(_T("服务器异常."));
		break;
	}
	return 0;
}
// 普通
DWORD DigBaoMgr::OnNS_NormalTreasureMap( tagNS_NormalTreasureMap* pMsg, DWORD dwParam )
{
	switch(pMsg->eType)
	{
	case E_Success:
		break;
	case ENTM_Nothing:				//一无所获
		CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("DigBao0")]);
		break;
	case ENTM_GainMoney:			//金钱奖励
		CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("DigBao1")]);
		break;
	case ENTM_ConfrontTrap:			//遭遇陷阱
		CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("DigBao2")]);
		break;
	case ENTM_Billiken:				//福神下凡 广播
		
		break;
	case ENTM_GainOlMap:			//获得古老藏宝图
		CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("DigBao1")]);
		break;
	case ENTM_GainKey:				//获得钥匙
		CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("DigBao1")]);
		break;
	case ENTM_GainOldChest:			//获得旧箱子
		CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("DigBao1")]);
		break;
	case ENTM_DriftBottle:			//漂流瓶
		CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("DigBao1")]);
		break;
	case ENTM_GainLoongSquamaChest://获得龙鳞宝箱
		CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("DigBao1")]);
		break;
	case ENTM_BadOldSeal:			//挖坏上古封印 广播
		
		break;
	case ENTM_Plutus:				//财神下凡 广播
		
		break;
	default:
		CombatSysUtil::Inst()->ShowScreenCenterMsgEx(_T("服务器异常."));
		break;
	}
	return 0;
}

// 上古
DWORD DigBaoMgr::OnNS_OldTreasureMap( tagNS_OldTreasureMap* pMsg, DWORD dwParam )
{
	switch(pMsg->eType)
	{
	case E_Success:
		break;
	case EOTM_Billiken:					//福神下凡 广播
	
		break;
	case EOTM_GainLoongSquamaChest:		//获得龙鳞宝箱
		CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("DigBao1")]);
		break;
	case EOTM_GainChest:				//获得箱子
		CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("DigBao1")]);
		break;
	case EOTM_GainLoongChest:			//获得玄龙宝箱
		CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("DigBao1")]);
		break;
	case EOTM_GainFourTreasures:		//获得文房四宝
		CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("DigBao1")]);
		break;
	case EOTM_GainKey:					//获得钥匙
		CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("DigBao3")]);
		break;
	case EOTM_GainRareTreaMap:			//获得绝世藏宝图
		CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("DigBao1")]);
		break;
	case EOTM_Plutus:					//挖到财神爷 广播
		
		break;
	case EOTM_DungeonEnter:				//发现密境入口 进入奇遇FB
		{
			if( P_VALID(GetObj("DungeonEnterMsgBox")) )
				KillObj("DungeonEnterMsgBox");
			CreateObj("DungeonEnterMsgBox", "MsgBox");
			
			TObjRef<MsgBox>("DungeonEnterMsgBox")->Init(_T(""), g_StrTable[_T("DigBao4")], 
				_T("tagDungeonEnterEvent"), (MsgBoxFlag)(MBF_OK|MBF_Cancel), TRUE);
		}
		break;
	case EOTM_BadLoongPulse:			//掘断龙脉	广播
		
		break;
	case EOTM_BadOldSeal:				//挖坏上古封印 广播
		
		break;
	case EOTM_AwakenGods:				//唤醒上古之灵 广播
		
		break;
	default:
		CombatSysUtil::Inst()->ShowScreenCenterMsgEx(_T("服务器异常."));
		break;
	}
	return 0;
}

// 绝世
DWORD DigBaoMgr::OnNS_RareTreasureMap( tagNS_RareTreasureMap* pMsg, DWORD dwParam )
{
	switch(pMsg->eType)
	{
	case E_Success:
		break;
	case ERTM_GainMoney:				//金钱奖励
		
		break;
	case ERTM_GainRareStuff:			//获得稀有材料若干
		
		break;
	case ERTM_GainDelicacyChest:		//获得精致的箱子
		CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("DigBao1")]);
		break;
	case ERTM_GainLoongChest:			//获得玄龙宝箱
		CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("DigBao1")]);
		break;
	case ERTM_GainLoongSquamaChest:		//获得龙鳞宝箱
		CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("DigBao1")]);
		break;
	case ERTM_GodsOfPractice:			//神之历练 提示历练
		{
			if( P_VALID(GetObj("GodsOfPracticeMsgBox")) )
				KillObj("GodsOfPracticeMsgBox");
			CreateObj("GodsOfPracticeMsgBox", "MsgBox");

			TObjRef<MsgBox>("GodsOfPracticeMsgBox")->Init(_T(""), g_StrTable[_T("DigBao5")], 
				_T("tagGodsOfPracticeEvent"), (MsgBoxFlag)(MBF_OK|MBF_Cancel), TRUE);
		}
		break;
	case ERTM_MonsterAttackCity:		//开放怪物攻城活动 广播
		
		break;
	case ERTM_DungeonEnter:				//发现密境入口 进入奇遇FB
		{
			if( P_VALID(GetObj("DungeonEnterMsgBox")) )
				KillObj("DungeonEnterMsgBox");
			CreateObj("DungeonEnterMsgBox", "MsgBox");

			TObjRef<MsgBox>("DungeonEnterMsgBox")->Init(_T(""), g_StrTable[_T("DigBao4")], 
				_T("tagDungeonEnterEvent"), (MsgBoxFlag)(MBF_OK|MBF_Cancel), TRUE);
		}		
		break;
	case ERTM_AwakenGods:				//唤醒上古之灵
		
		break;
	default:
		break;
	}
	return 0;
}

// 进入奇遇FB
DWORD DigBaoMgr::OnDungeonEnterEvent( tagMsgBoxEvent* pGameEvent )
{
	if( pGameEvent->eResult == MBF_OK )
	{
		tagNC_EnterAdventureFB msg;
		m_pSession->Send(&msg);
	}
	return 0;
}

// 历练
DWORD DigBaoMgr::OnGodsOfPracticeEvent( tagMsgBoxEvent* pGameEvent )
{
	if( pGameEvent->eResult == MBF_OK )
	{
		tagNC_Experience msg;
		m_pSession->Send(&msg);
	}
	return 0;
}

// 开宝箱
DWORD DigBaoMgr::OnTheRoleOfItemEvent(tagOnTheRoleOfItemEvent* pGameEvent)
{
	// 不是钥匙不行
	if( pGameEvent->eSpecFuncSrc!=EISF_ChestKey )
		return 0; 

	//不是背包里操作也不行
	if( pGameEvent->eTypeSrc!=EICT_Bag
		|| pGameEvent->eTypeDst!=EICT_Bag)
		return 0;

	Item* pChest = ItemMgr::Inst()->GetPocketItem(pGameEvent->n16IndexDst);
	if( P_VALID(pChest) && pChest->GetItemSpecFunc()!=EISF_Chest )
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("DigBao13")]);
		return 0;
	}
	Item* pKey = ItemMgr::Inst()->GetPocketItem(pGameEvent->n16IndexSrc);
	if( !P_VALID(pKey) )
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("DigBao14")]);
		return 0;
	}
	const tagItemProto* pItemCehst = ItemProtoData::Inst()->FindItemProto(pGameEvent->dwTypeIDDst);
	if( P_VALID(pItemCehst) && pItemCehst->nSpecFuncVal2!=pGameEvent->dwTypeIDSrc )
	{ 
		TCHAR szBuff[X_LONG_NAME] = {0};
		_sntprintf( szBuff, X_LONG_NAME, g_StrTable[_T("DigBao21")], pKey->GetItemName(), pChest->GetItemName() );
		CombatSysUtil::Inst()->ShowScreenCenterMsg(szBuff);
		return 0;
	}
	else if( !P_VALID(pItemCehst) )
	{
		return 0;
	}

	tagNC_TreasureChest msg;
	msg.n64ChestID	= pChest->GetItemId();
	msg.n64KeyID	= pKey->GetItemId();
	m_pSession->Send(&msg);

	return 0;
}

//福神下凡广播 服务器广播：“XX挖宝幸运的遇到福神下凡”
DWORD DigBaoMgr::OnNS_ChatBillike( tagNS_ChatBillike* pMsg, DWORD dwParam )
{
	ChatFrame* pFrame = (ChatFrame*)m_pFrameMgr->GetFrame(_T("Chat"));
	if( P_VALID(pFrame) )
	{
		tstring strPlayerName = PlayerNameTab::Inst()->FindNameByID(pMsg->dwRoleID);
		if( strPlayerName.empty() )
		{
			tagDigBaoCast cast;
			cast.eType = EDBCT_ChatBillike;
			cast.dwRoleID = pMsg->dwRoleID;
			m_vecDigBaoCast.push_back(cast);
			return 0;
		}
		TCHAR szBuff[X_LONG_NAME] = {0};
		_stprintf( szBuff, g_StrTable[_T("DigBao6")], strPlayerName.c_str() );
		pFrame->PushScrollText(szBuff);
	}
	return 0;
}

//挖坏上古封印广播 服务器广播：“XX挖坏了上古封印，上古的妖怪在XXX祸害人间，请各路英雄速去斩妖附魔！”
DWORD DigBaoMgr::OnNS_ChatBadOldSeal( tagNS_ChatBadOldSeal* pMsg, DWORD dwParam )
{
	ChatFrame* pFrame = (ChatFrame*)m_pFrameMgr->GetFrame(_T("Chat"));
	if( P_VALID(pFrame) )
	{
		tstring strPlayerName = PlayerNameTab::Inst()->FindNameByID(pMsg->dwRoleID);
		if( strPlayerName.empty() )
		{
			tagDigBaoCast cast;
			cast.eType = EDBCT_ChatBadOldSeal;
			cast.dwRoleID = pMsg->dwRoleID;
			cast.dwMapCrc = pMsg->dwMapCrc;
			m_vecDigBaoCast.push_back(cast);
			return 0;
		}
		tstring strMap = MapLogicData::Inst()->FindMapShowName(pMsg->dwMapCrc);
		TCHAR szBuff[X_LONG_NAME] = {0};
		_stprintf( szBuff, g_StrTable[_T("DigBao7")], strPlayerName.c_str(), strMap.c_str() );
		pFrame->PushScrollText(szBuff);
	}
	return 0;
}

//财神下凡广播 服务器广播：“XX挖宝幸运的遇到财神下凡”
DWORD DigBaoMgr::OnNS_Plutus( tagNS_Plutus* pMsg, DWORD dwParam )
{
	ChatFrame* pFrame = (ChatFrame*)m_pFrameMgr->GetFrame(_T("Chat"));
	if( P_VALID(pFrame) )
	{
		tstring strPlayerName = PlayerNameTab::Inst()->FindNameByID(pMsg->dwRoleID);
		if( strPlayerName.empty() )
		{
			tagDigBaoCast cast;
			cast.eType = EDBCT_Plutus;
			cast.dwRoleID = pMsg->dwRoleID;
			m_vecDigBaoCast.push_back(cast);
			return 0; 
		}
		TCHAR szBuff[X_LONG_NAME] = {0};
		_stprintf( szBuff, g_StrTable[_T("DigBao8")], strPlayerName.c_str() );
		pFrame->PushScrollText(szBuff);
	}
	return 0;
}

//唤醒上古之灵 服务器广播“XXX挖宝惊动了上古神灵，他们来到人间考验人类的能力”
DWORD DigBaoMgr::OnNS_AwakenGods( tagNS_AwakenGods* pMsg, DWORD dwParam )
{
	ChatFrame* pFrame = (ChatFrame*)m_pFrameMgr->GetFrame(_T("Chat"));
	if( P_VALID(pFrame) )
	{
		tstring strPlayerName = PlayerNameTab::Inst()->FindNameByID(pMsg->dwRoleID);
		if( strPlayerName.empty() )
		{
			tagDigBaoCast cast;
			cast.eType = EDBCT_AwakenGods;
			cast.dwRoleID = pMsg->dwRoleID;
			m_vecDigBaoCast.push_back(cast);
			return 0;
		}
		TCHAR szBuff[X_LONG_NAME] = {0};
		_stprintf( szBuff, g_StrTable[_T("DigBao9")], strPlayerName.c_str() );
		pFrame->PushScrollText(szBuff);
	}
	return 0;
}

//神之历练 若将BOSS击杀，则全服公告：“XXX（挖宝任务的玩家）挑战###（BOSS名称）成功！获得了XXX”
DWORD DigBaoMgr::OnNS_GodsOfPractice( tagNS_GodsOfPractice* pMsg, DWORD dwParam )
{
	ChatFrame* pFrame = (ChatFrame*)m_pFrameMgr->GetFrame(_T("Chat"));
	if( P_VALID(pFrame) )
	{
		tstring strPlayerName = PlayerNameTab::Inst()->FindNameByID(pMsg->dwRoleID);
		if( strPlayerName.empty() )
		{
			tagDigBaoCast cast;
			cast.eType = EDBCT_GodsOfPractice;
			cast.dwRoleID = pMsg->dwRoleID;
			cast.dwNpcTypeID = pMsg->dwNpcTypeID;
			cast.dwItemTypeID = pMsg->dwItemTypeID;
			m_vecDigBaoCast.push_back(cast);
			return 0;
		}
		const tagCreatureProto* pCreature = CreatureData::Inst()->FindNpcAtt(pMsg->dwNpcTypeID);
		if( !P_VALID(pCreature) )
			return 0;
		tstring strNPC = pCreature->szName;

		const tagItemDisplayInfo* pItem = ItemProtoData::Inst()->FindItemDisplay(pMsg->dwItemTypeID);
		if( !P_VALID(pItem) ) 
			return 0;
		tstring strItem = pItem->szName;

		TCHAR szBuff[X_LONG_NAME] = {0};
		_stprintf( szBuff, g_StrTable[_T("DigBao10")], strPlayerName.c_str(), strNPC.c_str(), strItem.c_str() );
		pFrame->PushScrollText(szBuff);
	}
	return 0;
}

//开放怪物攻城活动 系统公告提示：“###惹闹万物，大量的怪物决定5分钟后对##城进行报复，请大家协助防御”
DWORD DigBaoMgr::OnNS_MonsterAttackCity( tagNS_MonsterAttackCity* pMsg, DWORD dwParam )
{
	ChatFrame* pFrame = (ChatFrame*)m_pFrameMgr->GetFrame(_T("Chat"));
	if( P_VALID(pFrame) )
	{
		tstring strPlayerName = PlayerNameTab::Inst()->FindNameByID(pMsg->dwRoleID);
		if( strPlayerName.empty() )
		{
			tagDigBaoCast cast;
			cast.eType = EDBCT_MonsterAttackCity;
			cast.dwRoleID = pMsg->dwRoleID;
			cast.dwMapCrc = pMsg->dwMapCrc;
			m_vecDigBaoCast.push_back(cast);
			return 0;
		}
		
		tstring strMap = MapLogicData::Inst()->FindMapShowName(pMsg->dwMapCrc);
		TCHAR szBuff[X_LONG_NAME] = {0};
		_stprintf( szBuff, g_StrTable[_T("DigBao11")], strPlayerName.c_str(), strMap.c_str() );
		pFrame->PushScrollText(szBuff);
	}
	return 0;
}

// 广播宝箱开出的物品
DWORD DigBaoMgr::OnNS_RoleGetChestItem(tagNS_RoleGetChestItem* pMsg, DWORD dwParam)
{
	ChatFrame* pFrame = (ChatFrame*)m_pFrameMgr->GetFrame(_T("Chat"));
	if( P_VALID(pFrame) )
	{
		tstring strPlayerName = PlayerNameTab::Inst()->FindNameByID(pMsg->dwRoleID);
		if( strPlayerName.empty() )
		{
			tagDigBaoCast cast;
			cast.eType = EDBCT_RoleGetChestItem;
			cast.dwRoleID = pMsg->dwRoleID;
			cast.dwMapCrc = pMsg->dwItemTypeID;
			m_vecDigBaoCast.push_back(cast);
			return 0;
		}
		const tagItemDisplayInfo* pItem = ItemProtoData::Inst()->FindItemDisplay(pMsg->dwItemTypeID);
		TCHAR szBuff[X_LONG_NAME] = {0};
		_stprintf( szBuff, g_StrTable[_T("DigBao12")], strPlayerName.c_str(), pItem->szName );
		//pFrame->PushScrollText(szBuff);
		pFrame->PushInfo(szBuff, ESCC_Affiche);

	}
	return 0;
}

void DigBaoMgr::OnDigBaoCast( tagDigBaoCast cast )
{
	ChatFrame* pFrame = (ChatFrame*)m_pFrameMgr->GetFrame(_T("Chat"));
	if( !P_VALID(pFrame) )
		return;
	switch(cast.eType)
	{
	case EDBCT_ChatBillike:
		{
			tstring strPlayerName = PlayerNameTab::Inst()->FindNameByID(cast.dwRoleID);
			if( strPlayerName.empty() )
			{
				m_vecDigBaoCast.push_back( cast );
				return;
			}
			TCHAR szBuff[X_LONG_NAME] = {0};
			_stprintf( szBuff, g_StrTable[_T("DigBao6")], strPlayerName.c_str() );
			pFrame->PushScrollText(szBuff);
		}
		break;
	case EDBCT_ChatBadOldSeal:
		{
			tstring strPlayerName = PlayerNameTab::Inst()->FindNameByID(cast.dwRoleID);
			if( strPlayerName.empty() )
			{
				m_vecDigBaoCast.push_back( cast );
				return;
			}
			tstring strMap = MapLogicData::Inst()->FindMapShowName(cast.dwMapCrc);
			TCHAR szBuff[X_LONG_NAME] = {0};
			_stprintf( szBuff, g_StrTable[_T("DigBao7")], strPlayerName.c_str(), strMap.c_str() );
			pFrame->PushScrollText(szBuff);
		}
		break;
	case EDBCT_Plutus:
		{
			tstring strPlayerName = PlayerNameTab::Inst()->FindNameByID(cast.dwRoleID);
			if( strPlayerName.empty() )
			{
				m_vecDigBaoCast.push_back(cast);
				return;
			}
			TCHAR szBuff[X_LONG_NAME] = {0};
			_stprintf( szBuff, g_StrTable[_T("DigBao8")], strPlayerName.c_str() );
			pFrame->PushScrollText(szBuff);
		}
		break;
	case EDBCT_AwakenGods:
		{
			tstring strPlayerName = PlayerNameTab::Inst()->FindNameByID(cast.dwRoleID);
			if( strPlayerName.empty() )
			{
				m_vecDigBaoCast.push_back(cast);
				return;
			}
			TCHAR szBuff[X_LONG_NAME] = {0};
			_stprintf( szBuff, g_StrTable[_T("DigBao9")], strPlayerName.c_str() );
			pFrame->PushScrollText(szBuff);
		}
		break;
	case EDBCT_GodsOfPractice:
		{
			tstring strPlayerName = PlayerNameTab::Inst()->FindNameByID(cast.dwRoleID);
			if( strPlayerName.empty() )
			{
				m_vecDigBaoCast.push_back(cast);
				return;
			}
			const tagCreatureProto* pCreature = CreatureData::Inst()->FindNpcAtt(cast.dwNpcTypeID);
			if( !P_VALID(pCreature) )
				return;
			tstring strNPC = pCreature->szName;

			const tagItemDisplayInfo* pItem = ItemProtoData::Inst()->FindItemDisplay(cast.dwItemTypeID);
			if( !P_VALID(pItem) ) 
				return;
			tstring strItem = pItem->szName;

			TCHAR szBuff[X_LONG_NAME] = {0};
			_stprintf( szBuff, g_StrTable[_T("DigBao10")], strPlayerName.c_str(), strNPC.c_str(), strItem.c_str() );
			pFrame->PushScrollText(szBuff);
		}
		break;
	case EDBCT_MonsterAttackCity:
		{
			tstring strPlayerName = PlayerNameTab::Inst()->FindNameByID(cast.dwRoleID);
			if( strPlayerName.empty() )
			{
				m_vecDigBaoCast.push_back(cast);
				return;
			}

			tstring strMap = MapLogicData::Inst()->FindMapShowName(cast.dwMapCrc);
			TCHAR szBuff[X_LONG_NAME] = {0};
			_stprintf( szBuff, g_StrTable[_T("DigBao11")], strPlayerName.c_str(), strMap.c_str() );
			pFrame->PushScrollText(szBuff);
		}
		break;
	case EDBCT_RoleGetChestItem:
		{
			tstring strPlayerName = PlayerNameTab::Inst()->FindNameByID(cast.dwRoleID);
			if( strPlayerName.empty() )
			{
				m_vecDigBaoCast.push_back(cast);
				return;
			}
			const tagItemDisplayInfo* pItem = ItemProtoData::Inst()->FindItemDisplay(cast.dwItemTypeID);
			TCHAR szBuff[X_LONG_NAME] = {0};
			_stprintf( szBuff, g_StrTable[_T("DigBao12")], strPlayerName.c_str(), pItem->szName );
			//pFrame->PushScrollText(szBuff);
			pFrame->PushInfo(szBuff, ESCC_Affiche);
		}
		break;
	}
}