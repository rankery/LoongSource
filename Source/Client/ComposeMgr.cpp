#include "stdafx.h"
#include "ComposeMgr.h"
#include "QuestEvent.h"
#include "EquipConsolidateFrame.h"
#include "ItemProduceFrame.h"
#include "DecomposeFrame.h"
#include "CreatureData.h"
#include "..\WorldDefine\compose_define.h"
#include "ComColidateEvent.h"
#include "ItemEvent.h"
#include "..\WorldDefine\specfunc_define.h"
#include "Item.h"
#include "ItemMgr.h"
#include "ConsolidateData.h"
#include "CombatSysUtil.h"
#include "ComColidateEvent.h"
#include "SkillMgr.h"
#include "NPCTalkFrame.h"
#include "SkillProtoData.h"
#include "CombatEvent.h"
#include "AudioSys.h"
#include "SkillProtoData.h"
#include "ItemFrame.h"
#include "Container.h"
#include "InsuranceFrame.h"
#include "RoleMgr.h"
#include "LocalPlayer.h"
#include "GemRemovalFrame.h"

ComposeMgr::ComposeMgr( void ):m_Trunk(this)
{
	m_pEquipCon				= NULL;
	m_pItemPro				= NULL;
	m_pDeCompose			= NULL;
	m_dwSkillID				= GT_INVALID;
	m_dwNPCID				= GT_INVALID;
}

ComposeMgr::~ComposeMgr( void )
{
}

ComposeMgr g_ComposeMgr;
ComposeMgr* ComposeMgr::Inst()
{
	return &g_ComposeMgr;
}

BOOL ComposeMgr::Init()
{
	//--注册游戏事件
	TObjRef<GameFrameMgr>()->RegisterEventHandle( _T("tagStartNPCTalkEvent"), 
		(FRAMEEVENTPROC)m_Trunk.sfp1(&ComposeMgr::OnStartNPCTalkEvent));
	TObjRef<GameFrameMgr>()->RegisterEventHandle( _T("ItemEvent_ConsolidateItem"), 
		(FRAMEEVENTPROC)m_Trunk.sfp1(&ComposeMgr::OnConslidateItemEvent));
	TObjRef<GameFrameMgr>()->RegisterEventHandle( _T("LearnProduceSkillEvent"), 
		(FRAMEEVENTPROC)m_Trunk.sfp1(&ComposeMgr::OnLearnProduceSkillEvent));
	TObjRef<GameFrameMgr>()->RegisterEventHandle( _T("LeaveProSkillEvent"), 
		(FRAMEEVENTPROC)m_Trunk.sfp1(&ComposeMgr::OnLeaveProSkillEvent));
	TObjRef<GameFrameMgr>()->RegisterEventHandle( _T("tagUseSkillEvent"), 
		(FRAMEEVENTPROC)m_Trunk.sfp1(&ComposeMgr::OnUseSkillEvent));
	TObjRef<GameFrameMgr>()->RegisterEventHandle( _T("ItemEvent_ConsolidateItem"), 
		(FRAMEEVENTPROC)m_Trunk.sfp1(&ComposeMgr::OnChiselItemEvent));
	TObjRef<GameFrameMgr>()->RegisterEventHandle( _T("Close_Compose"), 
		(FRAMEEVENTPROC)m_Trunk.sfp1(&ComposeMgr::OnCloseComposeFrameEvent));
	TObjRef<GameFrameMgr>()->RegisterEventHandle( _T("Close_Insurance"), 
		(FRAMEEVENTPROC)m_Trunk.sfp1(&ComposeMgr::OnCloseInsuranceFrameEvent));

	//--注册网络消息
	TObjRef<NetCmdMgr>()->Register("NS_Inlay",	(NETMSGPROC)m_Trunk.sfp2(&ComposeMgr::OnNS_Inlay), _T("NS_Inlay"));
	TObjRef<NetCmdMgr>()->Register("NS_Brand",	(NETMSGPROC)m_Trunk.sfp2(&ComposeMgr::OnNS_Brand), _T("NS_Brand"));
	TObjRef<NetCmdMgr>()->Register("NS_AddPot",	(NETMSGPROC)m_Trunk.sfp2(&ComposeMgr::OnNS_AddPot), _T("NS_AddPot"));
	TObjRef<NetCmdMgr>()->Register("NS_LoongSoul",	(NETMSGPROC)m_Trunk.sfp2(&ComposeMgr::OnNS_LoongSoul), _T("NS_LoongSoul"));
	TObjRef<NetCmdMgr>()->Register("NS_Chisel",	(NETMSGPROC)m_Trunk.sfp2(&ComposeMgr::OnNS_Chisel), _T("NS_Chisel"));
	TObjRef<NetCmdMgr>()->Register("NS_LearnSkill",	(NETMSGPROC)m_Trunk.sfp2(&ComposeMgr::OnNS_LearnSkill), _T("NS_LearnSkill"));
	TObjRef<NetCmdMgr>()->Register("NS_ForgetSkill",	(NETMSGPROC)m_Trunk.sfp2(&ComposeMgr::OnNS_ForgetSkill), _T("NS_ForgetSkill"));


	return TRUE;
}

VOID ComposeMgr::Destroy()
{	
	//--注消游戏事件
	TObjRef<GameFrameMgr>()->UnRegisterEventHandler( _T("tagStartNPCTalkEvent"), 
		(FRAMEEVENTPROC)m_Trunk.sfp1(&ComposeMgr::OnStartNPCTalkEvent));
	TObjRef<GameFrameMgr>()->UnRegisterEventHandler( _T("ItemEvent_ConsolidateItem"), 
		(FRAMEEVENTPROC)m_Trunk.sfp1(&ComposeMgr::OnConslidateItemEvent));
	TObjRef<GameFrameMgr>()->UnRegisterEventHandler( _T("LearnProduceSkillEvent"), 
		(FRAMEEVENTPROC)m_Trunk.sfp1(&ComposeMgr::OnLearnProduceSkillEvent));
	TObjRef<GameFrameMgr>()->UnRegisterEventHandler( _T("LeaveProSkillEvent"), 
		(FRAMEEVENTPROC)m_Trunk.sfp1(&ComposeMgr::OnLeaveProSkillEvent));
	TObjRef<GameFrameMgr>()->UnRegisterEventHandler( _T("tagUseSkillEvent"), 
		(FRAMEEVENTPROC)m_Trunk.sfp1(&ComposeMgr::OnUseSkillEvent));
	TObjRef<GameFrameMgr>()->UnRegisterEventHandler( _T("ItemEvent_ConsolidateItem"), 
		(FRAMEEVENTPROC)m_Trunk.sfp1(&ComposeMgr::OnChiselItemEvent));
	TObjRef<GameFrameMgr>()->UnRegisterEventHandler( _T("Close_Compose"), 
		(FRAMEEVENTPROC)m_Trunk.sfp1(&ComposeMgr::OnCloseComposeFrameEvent));
	TObjRef<GameFrameMgr>()->UnRegisterEventHandler( _T("Close_Insurance"), 
		(FRAMEEVENTPROC)m_Trunk.sfp1(&ComposeMgr::OnCloseInsuranceFrameEvent));

	//--注消网络消息
	TObjRef<NetCmdMgr>()->UnRegister("NS_Inlay",	(NETMSGPROC)m_Trunk.sfp2(&ComposeMgr::OnNS_Inlay));
	TObjRef<NetCmdMgr>()->UnRegister("NS_Brand",	(NETMSGPROC)m_Trunk.sfp2(&ComposeMgr::OnNS_Brand));
	TObjRef<NetCmdMgr>()->UnRegister("NS_AddPot",	(NETMSGPROC)m_Trunk.sfp2(&ComposeMgr::OnNS_AddPot));
	TObjRef<NetCmdMgr>()->UnRegister("NS_LoongSoul",	(NETMSGPROC)m_Trunk.sfp2(&ComposeMgr::OnNS_LoongSoul));
	TObjRef<NetCmdMgr>()->UnRegister("NS_Chisel",	(NETMSGPROC)m_Trunk.sfp2(&ComposeMgr::OnNS_Chisel));
	TObjRef<NetCmdMgr>()->UnRegister("NS_LearnSkill",	(NETMSGPROC)m_Trunk.sfp2(&ComposeMgr::OnNS_LearnSkill));
	TObjRef<NetCmdMgr>()->UnRegister("NS_ForgetSkill",	(NETMSGPROC)m_Trunk.sfp2(&ComposeMgr::OnNS_ForgetSkill));

	if( P_VALID(TObjRef<GameFrameMgr>()->GetFrame(_T("EquipConsolidate"))) )
		TObjRef<GameFrameMgr>()->DestroyFrame(m_pEquipCon);

	if( P_VALID(TObjRef<GameFrameMgr>()->GetFrame(_T("ItemProduce"))) )
		TObjRef<GameFrameMgr>()->DestroyFrame(m_pItemPro);

	if( P_VALID(TObjRef<GameFrameMgr>()->GetFrame(_T("DeComposeFrame"))) )
		TObjRef<GameFrameMgr>()->DestroyFrame(m_pItemPro);

	if( P_VALID(GetObj("LeaveProSkillMsgBox")) )
		KillObj("LeaveProSkillMsgBox");
	if( P_VALID(GetObj("LearnProduceSkillMsgBox")) )
		KillObj("LearnProduceSkillMsgBox");
	if( P_VALID(GetObj("LeaveProduceSkillMsgBoxEx")) )
		KillObj("LeaveProduceSkillMsgBoxEx");
	if( P_VALID(GetObj("LearnProduceSkillMsgBoxEx")) )
		KillObj("LearnProduceSkillMsgBoxEx");

	m_pEquipCon = NULL;
	m_pItemPro = NULL;
}

DWORD ComposeMgr::OnStartNPCTalkEvent(tagStartNPCTalkEvent* pEvt)
{
	const tagCreatureProto* pCreatureProto = CreatureData::Inst()->FindNpcAtt(pEvt->typeID);
	//铭纹NPC
	if(pCreatureProto->eFunctionType==EFNPCT_Posy)
	{
		if(!CreateEquipConsolidateFrame())
		{
			IMSG(_T("equipconsolidate frame create error!\n"));
			return 0;
		}
		if(!P_VALID(m_pEquipCon))
		{
			IMSG(_T("EquipConsolidate frame hasn't done, Posy don't process!\n"));
			return 0;
		}
		m_pEquipCon->SetConsolidateMode(ECliDateT_Posy);
		m_pEquipCon->ShowWnd(TRUE);
		ComposeID id;
		id.npcID = pEvt->globalID;
		m_pEquipCon->SetOpenUIMode(EStartCompose_NPC, id);
		SetStrategy(EIST_ItemSmithCompose);
	}
	//镌刻NPC
	else if(pCreatureProto->eFunctionType==EFNPCT_Engrave)
	{
		if(!CreateEquipConsolidateFrame())
		{
			IMSG(_T("itemproduce frame create error!\n"));
			return 0;
		}
		if(!P_VALID(m_pEquipCon))
		{
			IMSG(_T("EquipConsolidate frame hasn't done, Engrave don't process!\n"));
			return 0;
		}
		m_pEquipCon->SetConsolidateMode(ECliDateT_Engrave);
		m_pEquipCon->ShowWnd(TRUE);
		ComposeID id;
		id.npcID = pEvt->globalID;
		m_pEquipCon->SetOpenUIMode(EStartCompose_NPC, id);
		SetStrategy(EIST_ItemSmithCompose);
	}
	//淬火NPC
	else if(pCreatureProto->eFunctionType==EFNPCT_Quench)
	{
		if(!CreateEquipConsolidateFrame())
		{
			IMSG(_T("itemproduce frame create error!\n"));
			return 0;
		}
		if(!P_VALID(m_pEquipCon))
		{
			IMSG(_T("EquipConsolidate frame hasn't done, Engrave don't process!\n"));
			return 0;
		}
		m_pEquipCon->SetConsolidateMode(ECliDateT_Quench);
		m_pEquipCon->ShowWnd(TRUE);
		ComposeID id;
		id.npcID = pEvt->globalID;
		m_pEquipCon->SetOpenUIMode(EStartCompose_NPC, id);
		SetStrategy(EIST_ItemSmithCompose);
	}
	//宝石摘除NPC
	else if( pCreatureProto->eFunctionType==EFNPCT_GemRemoval )
	{
		GemRemovalFrame* pGemFrame = (GemRemovalFrame*)TObjRef<GameFrameMgr>()->GetFrame( _T("GemRemoval") );
		if( !P_VALID(pGemFrame) )
			pGemFrame = (GemRemovalFrame*)TObjRef<GameFrameMgr>()->CreateFrame(_T("World"), _T("GemRemoval"), _T("GemRemovalFrame"), 0 );

		if( P_VALID(pGemFrame) )
		{
			pGemFrame->SetNPCID( pEvt->globalID );
			pGemFrame->ShowWnd( TRUE );
		}
	}
	//合成NPC
	else if( pCreatureProto->eFunctionType==EFNPCT_Compose )
	{
		EProduceType eType = (EProduceType)pCreatureProto->uFunctionID.nProduceType;
		if( eType == EPCT_PointUp )
		{
			if(!CreateDeComposeFrame())
			{
				IMSG(_T("decompose frame create error!\n"));
				return 0;
			}
			if(!P_VALID(m_pDeCompose))
			{
				IMSG(_T("decompose frame hasn't done, don't process!\n"));
				return 0;
			}
			m_pDeCompose->SetProduceMode(EPCT_PointUp);
			ComposeID id;
			id.npcID = pEvt->globalID;
			m_pDeCompose->SetOpenUIMode(EStartCompose_NPC, id);
			m_pDeCompose->ShowWnd(TRUE);
			SetStrategy(EIST_ItemSmithCompose);
		}
		else
		{
			if(!CreateItemProduceFrame())
			{
				IMSG(_T("itemproduce frame create error!\n"));
				return 0;
			}
			if(!P_VALID(m_pItemPro))
			{
				IMSG(_T("ItemProduce frame hasn't done, don't process!\n"));
				return 0;
			}		
			ComposeID id;
			id.npcID = pEvt->globalID;
			m_pItemPro->SetOpenUIMode(EStartCompose_NPC, id);
			m_pItemPro->SetProduceMode(eType);		
			m_pItemPro->ShowWnd(TRUE);
			SetStrategy(EIST_ItemSmithCompose);
		}
	}

	return 0;
}

DWORD ComposeMgr::OnConslidateItemEvent(tagOnTheRoleOfItemEvent* pEvt)
{
	//必须是镶嵌、烙印、龙附的一种
	if(pEvt->eSpecFuncSrc!=EISF_HoleGem
		&& pEvt->eSpecFuncSrc!=EISF_Brand
		&& pEvt->eSpecFuncSrc!=EISF_LongHunShi )
		return 0; 

	//不是背包里操作也不行
	if( pEvt->eTypeSrc!=EICT_Bag
		|| pEvt->eTypeDst!=EICT_Bag)
		return 0;

	//判断是否有该物品和装备
	Item* pItem = ItemMgr::Inst()->GetPocketItem(pEvt->n16IndexSrc);
	if(!P_VALID(pItem))
		return 0;
	Equipment* pEquip = (Equipment*)ItemMgr::Inst()->GetPocketItem(pEvt->n16IndexDst);
	if(!P_VALID(pEquip))
		return 0;
	
	DWORD dwTypeID = pItem->GetItemTypeID();

	const tagConsolidateItem* pConItem = ConsolidateData::Inst()->FindConsolidateItemProto(dwTypeID);
	if(!P_VALID(pConItem))
		return 0;

	// 不能对自己操作
	if( pEvt->dwTypeIDSrc == pEvt->dwTypeIDDst )
	{
		TCHAR szBuff[X_LONG_NAME];
		_stprintf(szBuff, _T("ConsolidateItem%d"), (int)pEvt->eSpecFuncSrc);
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[szBuff]);
		return 0;
	}

	bool bCanCliDate = false;
	for(int i=0; i<MAX_CONSOLIDATE_POS_QUANTITY; i++)
	{
		if(pEquip->GetEquipPos()==pConItem->ConsolidatePos[i].ePos 
			&& pConItem->ConsolidatePos[i].byConsolidate==1)
		{
			bCanCliDate = true;
			break;
		}
	}

	//能强化
	if(bCanCliDate)
	{
		//镶嵌
		if(EISF_HoleGem == pEvt->eSpecFuncSrc)
		{
			tagNC_Inlay inlay;
			inlay.n64SrcItemID = pItem->GetItemId();
			inlay.n64DstItemID = pEquip->GetItemId();
			TObjRef<NetSession>()->Send(&inlay);
		}
		//烙印
		else if(EISF_Brand == pEvt->eSpecFuncSrc)
		{
			
			InsuranceFrame* pFrame = (InsuranceFrame*)TObjRef<GameFrameMgr>()->GetFrame(_T("Insurance"));
			if( P_VALID(pFrame) )
			{
				pFrame->ShowFrame(TRUE, EISF_Brand, pItem->GetItemId(), pEquip->GetItemId());
				SetStrategy(EIST_ItemInsurance);
			}
			
			/*tagNC_Brand brand;
			brand.n64SrcItemID = pItem->GetItemId();
			brand.n64DstItemID = pEquip->GetItemId();
			brand.n64IMID = GT_INVALID;
			TObjRef<NetSession>()->Send(&brand);*/

		}
		//龙附
		else if(EISF_LongHunShi == pEvt->eSpecFuncSrc)
		{
			tagNC_LoongSoul loongsoul;
			loongsoul.n64SrcItemID = pItem->GetItemId();
			loongsoul.n64DstItemID = pEquip->GetItemId();
			TObjRef<NetSession>()->Send(&loongsoul);
		}
	}
	else 
	{
		TCHAR szBuff[X_LONG_NAME];
		_stprintf(szBuff, _T("ConsolidateItem%d"), (int)pEvt->eSpecFuncSrc);
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[szBuff]);
	}

	return 0;
}

DWORD ComposeMgr::OnChiselItemEvent(tagOnTheRoleOfItemEvent* pEvt)
{
	// 不是凿具
	if(pEvt->eSpecFuncSrc!=EISF_Chisel)
		return 0;

	//不是背包里操作也不行
	if( pEvt->eTypeSrc!=EICT_Bag
		|| pEvt->eTypeDst!=EICT_Bag)
		return 0;

	//判断是否有该物品和装备
	Item* pItem = ItemMgr::Inst()->GetPocketItem(pEvt->n16IndexSrc);
	if(!P_VALID(pItem))
		return 0;
	Equipment* pEquip = (Equipment*)ItemMgr::Inst()->GetPocketItem(pEvt->n16IndexDst);
	if(!P_VALID(pEquip))
		return 0;
	if( !MIsEquipment(pEquip->GetItemTypeID()) )
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ConsolidateItem_NoHaveChisel1")]);
		return 0;
	}

	if(pEquip->GetEquipPos() == EEP_Face)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ConsolidateItem_Face")]);
		return 0;
	}

	if(!pEquip->IsCanChisel())
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ConsolidateItem_NoHaveChisel")]);
		return 0;
	}

	tagNC_Chisel chisel;
	chisel.n64SrcItemID = pEquip->GetItemId();
	chisel.n64StuffID = pItem->GetItemId();
	TObjRef<NetSession>()->Send(&chisel);

	return 0;
}

BOOL ComposeMgr::CreateEquipConsolidateFrame()
{
	if( P_VALID(TObjRef<GameFrameMgr>()->GetFrame(_T("EquipConsolidate"))) )
		return TRUE;

	// 创建前，删除其他合成界面，实现界面间的互斥
	OnCloseComposeFrameEvent(NULL);

	if( !TObjRef<GameFrameMgr>()->CreateFrame(_T("World"), _T("EquipConsolidate"), _T("EquipConsolidateFrame"), 0) )
		return FALSE;
	
	m_pEquipCon = (EquipConsolidateFrame*)TObjRef<GameFrameMgr>()->GetFrame(_T("EquipConsolidate"));
	if(!P_VALID(m_pEquipCon))
	{
		IMSG(_T("EquipConsolidate frame don't done"));
		return FALSE;
	}
	return TRUE;
}

BOOL ComposeMgr::CreateItemProduceFrame()
{
	if( P_VALID(TObjRef<GameFrameMgr>()->GetFrame(_T("ItemProduce"))) )
		return TRUE;

	// 创建前，删除其他合成界面，实现界面间的互斥
	OnCloseComposeFrameEvent(NULL);

	if( !TObjRef<GameFrameMgr>()->CreateFrame(_T("World"), _T("ItemProduce"), _T("ItemProduceFrame"), 0) )
		return FALSE;
	
	m_pItemPro = (ItemProduceFrame*)TObjRef<GameFrameMgr>()->GetFrame(_T("ItemProduce"));
	if(!P_VALID(m_pItemPro))
	{
		IMSG(_T("ItemProduce frame don't done"));
		return FALSE;
	}
	return TRUE;
}

BOOL ComposeMgr::CreateDeComposeFrame()
{
	if( P_VALID(TObjRef<GameFrameMgr>()->GetFrame(_T("DeCompose"))) )
		return TRUE;
	
	// 创建前，删除其他合成界面，实现界面间的互斥
	OnCloseComposeFrameEvent(NULL);

	if( !TObjRef<GameFrameMgr>()->CreateFrame(_T("World"), _T("DeCompose"), _T("DeComposeFrame"), 0) )
		return FALSE;

	m_pDeCompose = (DeComposeFrame*)TObjRef<GameFrameMgr>()->GetFrame(_T("DeCompose"));
	if(!P_VALID(m_pDeCompose))
	{
		IMSG(_T("DeCompose frame don't done"));
		return FALSE;
	}
	return TRUE;
}

DWORD ComposeMgr::OnNS_Inlay( tagNS_Inlay* pMsg, DWORD dwParam )
{
	switch(pMsg->dwErrorCode)
	{	
	case E_Compose_Consolidate_Success:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Consolidate_Success")]);
		TObjRef<AudioSys>()->Play2DSound(_T("make_success"), SOUND_NORMAL);
		break;
	case E_Compose_Consolidate_Perfect:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Consolidate_Perfect")]);

		// 完美镶嵌
		TObjRef<AudioSys>()->Play2DSound(_T("make_perfect"), SOUND_NORMAL);
		break;
	case E_Compose_Consolidate_Lose:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Consolidate_Lose")]);
		TObjRef<AudioSys>()->Play2DSound(_T("make_failure"), SOUND_NORMAL);
		break;
	case E_Consolidate_Gem_Not_Exit:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_Gem_Not_Exit")]);
		break;
	case E_Consolidate_Gem_Full:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_Gem_Full")]);
		break;
	case E_Consolidate_EquipCanNotInlay:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_EquipCanNotInlay")]);
		break;
	case E_Consolidate_ValConsume_Inadequate:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_ValConsume_Inadequate")]);
		break;
	case E_Consolidate_Gem_Not_Hole:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_Gem_Not_Hole")]);
		break;
	}

	return 0;
}

DWORD ComposeMgr::OnNS_Brand( tagNS_Brand* pMsg, DWORD dwParam )
{
	switch(pMsg->dwErrorCode)
	{	
	case E_Compose_Consolidate_Success:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Consolidate_Success")]);
		TObjRef<AudioSys>()->Play2DSound(_T("make_success"), SOUND_NORMAL);
		break;
	case E_Compose_Consolidate_Perfect:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Consolidate_Perfect")]);

		// 完美烙印
		TObjRef<AudioSys>()->Play2DSound(_T("make_perfect"), SOUND_NORMAL);
		break;
	case E_Compose_Consolidate_Lose:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Consolidate_Lose")]);
		TObjRef<AudioSys>()->Play2DSound(_T("make_failure"), SOUND_NORMAL);
		break;
	case E_Consolidate_Brand_Not_Exit:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_Brand_Not_Exit")]);
		break;
	case E_Consolidate_Brand_TopLevel:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_Brand_TopLevel")]);
		break;
	case E_Consolidate_EquipCanNotBrand:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_EquipCanNotBrand")]);
		break;
	case E_Consolidate_ValConsume_Inadequate:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_ValConsume_Inadequate")]);
		break;
	}

	return 0;
}


DWORD ComposeMgr::OnNS_AddPot( tagNS_AddPot* pMsg, DWORD dwParam )
{
	switch(pMsg->dwErrorCode)
	{	
	case E_Compose_Consolidate_Success:
		{
			TCHAR szTips[X_LONG_NAME] = {0};
			_sntprintf( szTips, X_LONG_NAME, g_StrTable[_T("AddPot_Success")], pMsg->dwPotValue );
			CombatSysUtil::Inst()->ShowScreenCenterMsg(szTips);
			TObjRef<AudioSys>()->Play2DSound(_T("make_success"), SOUND_NORMAL);
		}
		break;
	case E_Compose_Consolidate_Perfect:
		{
			TCHAR szTips[X_LONG_NAME] = {0};
			_sntprintf( szTips, X_LONG_NAME, g_StrTable[_T("AddPot_Perfect")], pMsg->dwPotValue );
			CombatSysUtil::Inst()->ShowScreenCenterMsg(szTips);
			TObjRef<AudioSys>()->Play2DSound(_T("make_perfect"), SOUND_NORMAL);
		}
		break;
	case E_Compose_Consolidate_Lose:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("AddPot_Fail")]);
		TObjRef<AudioSys>()->Play2DSound(_T("make_failure"), SOUND_NORMAL);
		break;
	case E_Raise_PotVal_Not_Needing:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("EquipPotIsMax")]);
		break;
	case E_Raise_PotVal_Times_Zero:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("EquipNoAddPotTime")]);
		break;
	}
	return 0;
}

DWORD ComposeMgr::OnNS_LoongSoul( tagNS_LoongSoul* pMsg, DWORD dwParam )
{
	switch(pMsg->dwErrorCode)
	{	
	case E_Compose_Consolidate_Success:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Consolidate_Success")]);
		TObjRef<AudioSys>()->Play2DSound(_T("make_success"), SOUND_NORMAL);
		break;
	case E_Compose_Consolidate_Perfect:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Consolidate_Perfect")]);
		// 完美龙魂
		TObjRef<AudioSys>()->Play2DSound(_T("make_perfect"), SOUND_NORMAL);
		break;
	case E_Compose_Consolidate_Lose:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Consolidate_Lose")]);
		TObjRef<AudioSys>()->Play2DSound(_T("make_failure"), SOUND_NORMAL);
		break;
	case E_Consolidate_LoongSoul_Not_Exit:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_LoongSoul_Not_Exit")]);
		break;
	case E_Consolidate_LoongSoul_TopLevel:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_LoongSoul_TopLevel")]);
		break;
	case E_Consolidate_EquipCanNotLoongSoul:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_EquipCanNotLoongSoul")]);
		break;
	case E_Consolidate_ValConsume_Inadequate:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_ValConsume_Inadequate")]);
		break;
	}

	return 0;
}

DWORD ComposeMgr::OnNS_Chisel(tagNS_Chisel* pMsg, DWORD dwParam)
{
	switch(pMsg->dwErrorCode)
	{	
	case E_Compose_Consolidate_Success:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Consolidate_Success")]);
		TObjRef<AudioSys>()->Play2DSound(_T("make_success"), SOUND_NORMAL);
		break;
	case E_Compose_Consolidate_Perfect:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Consolidate_Perfect")]);
		// 完美开凿
		TObjRef<AudioSys>()->Play2DSound(_T("make_perfect"), SOUND_NORMAL);
		break;
	case E_Compose_Consolidate_Lose:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Consolidate_Lose")]);
		TObjRef<AudioSys>()->Play2DSound(_T("make_failure"), SOUND_NORMAL);
		break;
	case E_Consolidate_Equip_Not_Exist:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Chisel_Equip_Not_Exist")]);
		break;
	case E_Consolidate_Equip_CanNot_Chisel:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_Equip_CanNot_Chisel")]);
		break;
	case E_Consolidate_Chisel_Not_Exit:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_Chisel_Not_Exit")]);
		break;
	case E_Consolidate_Equip_Hole_Full:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_Equip_Hole_Full")]);
		break;
	case E_Consolidate_Chisel_Not_Match:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_Chisel_Not_Match")]);
		break;
	case E_Consolidate_NotIdentified:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_NotIdentified")]);
		break;
	}

	return 0;
}

void ComposeMgr::LearnProduceSkill( DWORD dwSkillID, DWORD dwNpcID )
{
	TList<DWORD> skillList;
	SkillMgr::Inst()->GetSkillList(skillList);
	TList<DWORD>::TListIterator iter;
	INT nLearnProSkill = 0;
	DWORD nSkillID = 0;
	BOOL bLearned = false;
	skillList.ResetIterator();
	while(skillList.PeekNext(nSkillID))
	{
		if(MTransProduceSkill(nSkillID)==9000)
			nLearnProSkill++;
		if( MTransSkillID(nSkillID) == dwSkillID )
			bLearned = true;
	}

	if( nLearnProSkill>=MAX_ROLE_PRODUCE_NUM )
	{
		NPCTalkFrame *pFrame = (NPCTalkFrame*)TObjRef<GameFrameMgr>()->GetFrame(_T("NPCTalk"));
		if( !P_VALID(pFrame) )
			return ;

		pFrame->SetContent(g_StrTable[_T("LearnSkill_2Full")]);
		return ;
	}

	if( bLearned )
	{
		NPCTalkFrame *pFrame = (NPCTalkFrame*)TObjRef<GameFrameMgr>()->GetFrame(_T("NPCTalk"));
		if( !P_VALID(pFrame) )
			return ;

		pFrame->SetContent(g_StrTable[_T("LearnSkill_Existed")]);
		return ;
	}

	//INT64 n64Silver = CurrencyMgr::Inst()->GetBagSilver();
	//if( n64Silver <  3000)
	//{
	//	NPCTalkFrame *pFrame = (NPCTalkFrame*)TObjRef<GameFrameMgr>()->GetFrame(_T("NPCTalk"));
	//	if( !P_VALID(pFrame) )
	//		return ;
	//	
	//	pFrame->SetContent(g_StrTable[_T("LearnSkill_Money")]);
	//	return;
	//}


	DWORD dwSkill = MTransSkillTypeID(dwSkillID, 1);
	const tagSkillProtoClient* pSkill = SkillProtoData::Inst()->FindSkillProto( dwSkill );
	if( !P_VALID(pSkill) )
		return;

	if( P_VALID(GetObj("LearnProduceSkillMsgBox")) )
		KillObj("LearnProduceSkillMsgBox");
	CreateObj("LearnProduceSkillMsgBox", "MsgBox");

	TCHAR szBuff[64];
	_stprintf(szBuff, g_StrTable[_T("LearnSkill_IsLearn")], pSkill->szName);

	TObjRef<MsgBox>("LearnProduceSkillMsgBox")->Init(_T(""), szBuff, 
		_T("LearnProduceSkillEvent"), (MsgBoxFlag)(MBF_OK|MBF_Cancel), TRUE);

	m_dwSkillID = dwSkillID;
	m_dwNPCID = dwNpcID;
}

void ComposeMgr::LeaveProduceSkill( DWORD dwSkillID, DWORD dwNpcID )
{
	TList<DWORD> skillList;
	SkillMgr::Inst()->GetSkillList(skillList);
	TList<DWORD>::TListIterator iter;
	BOOL bLearned = false;
	DWORD nSkillID = 0;
	skillList.ResetIterator();
	while(skillList.PeekNext(nSkillID))
	{
		if( nSkillID == dwSkillID )
			bLearned = true;
	}

	if( !bLearned )
	{
		NPCTalkFrame *pFrame = (NPCTalkFrame*)TObjRef<GameFrameMgr>()->GetFrame(_T("NPCTalk"));
		if( !P_VALID(pFrame) )
			return ;

		pFrame->SetContent(g_StrTable[_T("ForgetSkill_NotExist")]);
		return ;
	}

	const tagSkillData* pSkill = SkillMgr::Inst()->GetSkillData( dwSkillID );
	if( !P_VALID(pSkill) )
		return;

	if( P_VALID(GetObj("LeaveProSkillMsgBox")) )
		KillObj("LeaveProSkillMsgBox");
	CreateObj("LeaveProSkillMsgBox", "MsgBox");

	TCHAR szBuff[64];
	_stprintf(szBuff, g_StrTable[_T("ForgetSkill_IsAffirm")], pSkill->pProto->szName);

	TObjRef<MsgBox>("LeaveProSkillMsgBox")->Init(_T(""), szBuff, 
		_T("LeaveProSkillEvent"), (MsgBoxFlag)(MBF_OK|MBF_Cancel), TRUE);
	m_dwSkillID = dwSkillID;
	m_dwNPCID = dwNpcID;
}

DWORD ComposeMgr::OnNS_LearnSkill( tagNS_LearnSkill* pMsg, DWORD dwParam )
{
	/*NPCTalkFrame *pFrame = (NPCTalkFrame*)TObjRef<GameFrameMgr>()->GetFrame(_T("NPCTalk"));
	if( !P_VALID(pFrame) )
		return 0;*/

	DWORD skillID = pMsg->dwSkillID;

	BOOL isProSkill = IsProduceSKill(MTransSkillTypeID(skillID, 1));
	if( !isProSkill )
		return 0;
	
	const tagSkillData* pSkill = SkillMgr::Inst()->GetSkillData( skillID );
	if( !P_VALID(pSkill) )
		return 0;

	switch(pMsg->dwErrorCode)
	{
	case E_Success:
		{
			TCHAR szBuff[64] = {0};
			_stprintf(szBuff, g_StrTable[_T("LearnSkill_Success")], pSkill->pProto->szName);
			if( P_VALID(GetObj("LearnProduceSkillMsgBoxEx")) )
				KillObj("LearnProduceSkillMsgBoxEx");
			CreateObj("LearnProduceSkillMsgBoxEx", "MsgBoxEx");
			TObjRef<MsgBoxEx>("LearnProduceSkillMsgBoxEx")->Init(_T(""), szBuff, 
				_T("LearnProduceSkillMsgBoxExEvent"), (MsgBoxFlag)(MBF_OK), TRUE);
			//pFrame->SetContent(szBuff);
		}
		break;
	case E_LearnSkill_Existed:
		//pFrame->SetContent(g_StrTable[_T("LearnSkill_Existed")]);
		{
			if( P_VALID(GetObj("LearnProduceSkillMsgBoxEx")) )
				KillObj("LearnProduceSkillMsgBoxEx");
			CreateObj("LearnProduceSkillMsgBoxEx", "MsgBoxEx");
			TObjRef<MsgBoxEx>("LearnProduceSkillMsgBoxEx")->Init(_T(""), g_StrTable[_T("LearnSkill_Existed")], 
				_T("LearnProduceSkillMsgBoxExEvent"), (MsgBoxFlag)(MBF_OK), TRUE);
		}
		break;
	case E_LearnSkill_ProtoNotFound:
		//pFrame->SetContent(g_StrTable[_T("LearnSkill_ProtoNotFound")]);
		{
			if( P_VALID(GetObj("LearnProduceSkillMsgBoxEx")) )
				KillObj("LearnProduceSkillMsgBoxEx");
			CreateObj("LearnProduceSkillMsgBoxEx", "MsgBoxEx");
			TObjRef<MsgBoxEx>("LearnProduceSkillMsgBoxEx")->Init(_T(""), g_StrTable[_T("LearnSkill_ProtoNotFound")], 
				_T("LearnProduceSkillMsgBoxExEvent"), (MsgBoxFlag)(MBF_OK), TRUE);
		}
		break;
	case E_LearnSkill_NPCNotEixst:
		//pFrame->SetContent(g_StrTable[_T("LearnSkill_NPCNotEixst")]);
		{
			if( P_VALID(GetObj("LearnProduceSkillMsgBoxEx")) )
				KillObj("LearnProduceSkillMsgBoxEx");
			CreateObj("LearnProduceSkillMsgBoxEx", "MsgBoxEx");
			TObjRef<MsgBoxEx>("LearnProduceSkillMsgBoxEx")->Init(_T(""), g_StrTable[_T("LearnSkill_NPCNotEixst")], 
				_T("LearnProduceSkillMsgBoxExEvent"), (MsgBoxFlag)(MBF_OK), TRUE);
		}
		break;
	case E_LearnSkill_NPCTooFar:
		//pFrame->SetContent(g_StrTable[_T("LearnSkill_NPCTooFar")]);
		{
			if( P_VALID(GetObj("LearnProduceSkillMsgBoxEx")) )
				KillObj("LearnProduceSkillMsgBoxEx");
			CreateObj("LearnProduceSkillMsgBoxEx", "MsgBoxEx");
			TObjRef<MsgBoxEx>("LearnProduceSkillMsgBoxEx")->Init(_T(""), g_StrTable[_T("LearnSkill_NPCTooFar")], 
				_T("LearnProduceSkillMsgBoxExEvent"), (MsgBoxFlag)(MBF_OK), TRUE);
		}
		break;
	case E_LearnSkill_NPCCanNotTeach:
		//pFrame->SetContent(g_StrTable[_T("LearnSkill_NPCCanNotTeach")]);
		{
			if( P_VALID(GetObj("LearnProduceSkillMsgBoxEx")) )
				KillObj("LearnProduceSkillMsgBoxEx");
			CreateObj("LearnProduceSkillMsgBoxEx", "MsgBoxEx");
			TObjRef<MsgBoxEx>("LearnProduceSkillMsgBoxEx")->Init(_T(""), g_StrTable[_T("LearnSkill_NPCCanNotTeach")], 
				_T("LearnProduceSkillMsgBoxExEvent"), (MsgBoxFlag)(MBF_OK), TRUE);
		}
		break;
	}
	m_dwSkillID = GT_INVALID;
	m_dwNPCID = GT_INVALID;
	return 0;
}

DWORD ComposeMgr::OnNS_ForgetSkill( tagNS_ForgetSkill* pMsg, DWORD dwParam )
{

	/*NPCTalkFrame *pFrame = (NPCTalkFrame*)TObjRef<GameFrameMgr>()->GetFrame(_T("NPCTalk"));
	if( !P_VALID(pFrame) )
		return 0;*/
	DWORD skillID = pMsg->dwSkillID;
	
	BOOL isProSkill = IsProduceSKill(MTransSkillTypeID(skillID, 1));
	if( !isProSkill )
		return 0;

	const tagSkillProtoClient* pSkill = SkillProtoData::Inst()->FindSkillProto(MTransSkillTypeID(skillID, 1));
	if( !P_VALID(pSkill) )
		return 0;

	switch(pMsg->dwErrorCode)
	{
	case E_Success:
		{
			TCHAR szBuff[64] = {0};
			_stprintf(szBuff, g_StrTable[_T("ForgetSkill_Success")],pSkill->szName );
			//pFrame->SetContent(szBuff);
			if( P_VALID(GetObj("LeaveProduceSkillMsgBoxEx")) )
				KillObj("LeaveProduceSkillMsgBoxEx");
			CreateObj("LeaveProduceSkillMsgBoxEx", "MsgBoxEx");
			TObjRef<MsgBoxEx>("LeaveProduceSkillMsgBoxEx")->Init(_T(""), szBuff, 
				_T("LeaveProduceSkillMsgBoxExEvent"), (MsgBoxFlag)(MBF_OK), TRUE);
		}
		break;
	case E_ForgetSkill_NotExist:
		//pFrame->SetContent(g_StrTable[_T("ForgetSkill_NotExist")]);
		{
			if( P_VALID(GetObj("LeaveProduceSkillMsgBoxEx")) )
				KillObj("LeaveProduceSkillMsgBoxEx");
			CreateObj("LeaveProduceSkillMsgBoxEx", "MsgBoxEx");
			TObjRef<MsgBoxEx>("LeaveProduceSkillMsgBoxEx")->Init(_T(""), g_StrTable[_T("ForgetSkill_NotExist")], 
				_T("LeaveProduceSkillMsgBoxExEvent"), (MsgBoxFlag)(MBF_OK), TRUE);
		}
		break;
	case E_ForgetSkill_CanNotBeForgotten:
		//pFrame->SetContent(g_StrTable[_T("ForgetSkill_CanNotBeForgotten")]);
		{
			if( P_VALID(GetObj("LeaveProduceSkillMsgBoxEx")) )
				KillObj("LeaveProduceSkillMsgBoxEx");
			CreateObj("LeaveProduceSkillMsgBoxEx", "MsgBoxEx");
			TObjRef<MsgBoxEx>("LeaveProduceSkillMsgBoxEx")->Init(_T(""), g_StrTable[_T("ForgetSkill_CanNotBeForgotten")], 
				_T("LeaveProduceSkillMsgBoxExEvent"), (MsgBoxFlag)(MBF_OK), TRUE);
		}
		break;
	case E_ForgetSkill_NPCNotValid:
		//pFrame->SetContent(g_StrTable[_T("ForgetSkill_NPCNotValid")]);
		{
			if( P_VALID(GetObj("LeaveProduceSkillMsgBoxEx")) )
				KillObj("LeaveProduceSkillMsgBoxEx");
			CreateObj("LeaveProduceSkillMsgBoxEx", "MsgBoxEx");
			TObjRef<MsgBoxEx>("LeaveProduceSkillMsgBoxEx")->Init(_T(""), g_StrTable[_T("ForgetSkill_NPCNotValid")], 
				_T("LeaveProduceSkillMsgBoxExEvent"), (MsgBoxFlag)(MBF_OK), TRUE);
		}
		break;
	}

	return 0;
}

DWORD ComposeMgr::OnLearnProduceSkillEvent(tagMsgBoxEvent* pEvent)
{
	if(pEvent->eResult == MBF_OK)
	{
		tagNC_LearnSkill msg;
		msg.dwNPCID = m_dwNPCID;
		msg.dwSkillID = m_dwSkillID;
		msg.n64ItemID = GT_INVALID;
		TObjRef<NetSession>()->Send(&msg);
	}

	return 0;
}

DWORD ComposeMgr::OnLeaveProSkillEvent( tagMsgBoxEvent* pEvent )
{
	if(pEvent->eResult == MBF_OK)
	{
		tagNC_ForgetSkill msg;
		msg.dwSkillID = m_dwSkillID;
		msg.dwNPCID = m_dwNPCID;
		TObjRef<NetSession>()->Send(&msg);
	}

	return 0;
}

DWORD ComposeMgr::OnUseSkillEvent( tagUseSkillEvent* pEvent )
{
	EProduceType eType = GetProduceType(pEvent->dwSkillID);
	if( eType!=EPCT_NULL )
	{
		if( eType == EPCT_PointUp )
		{
			if(!CreateDeComposeFrame())
			{
				IMSG(_T("decompose frame create error!\n"));
				return 0;
			}
			if(!P_VALID(m_pDeCompose))
			{
				IMSG(_T("decompose frame hasn't done, don't process!\n"));
				return 0;
			}
			m_pDeCompose->SetProduceMode(EPCT_PointUp);
			ComposeID id;
			id.skillID = pEvent->dwSkillID;
			m_pDeCompose->SetOpenUIMode(EStartCompose_Skill, id);
			m_pDeCompose->ShowWnd(TRUE);
			SetStrategy(EIST_ItemSmithCompose);
		}
		else
		{
			if(!CreateItemProduceFrame())
			{
				IMSG(_T("itemproduce frame create error!\n"));
				return 0;
			}
			if(!P_VALID(m_pItemPro))
			{
				IMSG(_T("ItemProduce frame hasn't done, don't process!\n"));
				return 0;
			}
			m_pItemPro->SetProduceMode(eType);		
			ComposeID id;
			id.skillID = pEvent->dwSkillID;
			m_pItemPro->SetOpenUIMode(EStartCompose_Skill, id);
			m_pItemPro->ShowWnd(TRUE);
			SetStrategy(EIST_ItemSmithCompose);
		}
	}
	return 0;
}

EProduceType ComposeMgr::GetProduceType( DWORD dwSkillID )
{
	const tagSkillProtoClient* pSkill = SkillProtoData::Inst()->FindSkillProto(dwSkillID);
	if( !P_VALID(pSkill) )
		return EPCT_NULL;

	if( pSkill->nType2 != ESSTE_Produce )
		return EPCT_NULL;

	LocalPlayer* pLP = RoleMgr::Inst()->GetLocalPlayer();
	if( !P_VALID(pLP) )
		return EPCT_NULL;

	if( pLP->IsDead() && (pSkill->dwSelfStateLimit&0x1)==0  )
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_UseSkill_Error0")]);
		return EPCT_NULL;
	}

	switch(pSkill->nType3)
	{
	case ESSTE2_Smilt:
		return EPCT_Smilt;
		break;
	case ESSTE2_Artisan:
		return EPCT_Artisan;
		break;
	case ESSTE2_DanTraining:
		return EPCT_DanTraining;
		break;
	case ESSTE2_Smith:
		return EPCT_Smith;
		break;
	case ESSTE2_Casting:
		return EPCT_Casting;
		break;
	case ESSTE2_Dressmaker:
		return EPCT_Dressmaker;
		break;
	case ESSTE2_Aechnics:
		return EPCT_Aechnics;
		break;
	case ESSTE2_PointUp:
		return EPCT_PointUp;
		break;

	}
	return EPCT_NULL;
}

DWORD ComposeMgr::GetColorByQuality( BYTE byQuality )
{
	switch(byQuality)
	{
	case 0:
		return 0xFFFFF7E0;
		break;
	case 1:
		return 0xFFFFBC00;
		break;
	case 2:
		return 0xFF05FF00;
		break;
	case 3:
		return 0xFF00A2FF;
		break;
	case 4:
		return 0xFFFF7800;
		break;
	case 5:
		return 0xFF9932CD;
	default:
		return 0xFF8D8D8D;
		break;
	}
}

DWORD ComposeMgr::OnCloseComposeFrameEvent( tagGameEvent* pEvent )
{
	GameFrame* pFrame = TObjRef<GameFrameMgr>()->GetFrame(_T("ItemProduce"));
	if(P_VALID(pFrame))
	{
		TObjRef<GameFrameMgr>()->AddToDestroyList(pFrame);
	}
	pFrame = TObjRef<GameFrameMgr>()->GetFrame(_T("EquipConsolidate"));
	if(P_VALID(pFrame))
	{
		TObjRef<GameFrameMgr>()->AddToDestroyList(pFrame);
	}
	pFrame = TObjRef<GameFrameMgr>()->GetFrame(_T("DeCompose"));
	if(P_VALID(pFrame))
	{
		TObjRef<GameFrameMgr>()->AddToDestroyList(pFrame);
	}
	return 0;
}

DWORD ComposeMgr::OnCloseInsuranceFrameEvent( tagGameEvent* pEvent )
{
	InsuranceFrame* pFrame = (InsuranceFrame*)TObjRef<GameFrameMgr>()->GetFrame(_T("Insurance"));
	if( P_VALID(pFrame) )
	{
		pFrame->ShowFrame(FALSE);
	}
	return 0;
}

BOOL ComposeMgr::IsProduceSKill( DWORD dwSkillID )
{
	const tagSkillProtoClient* pSkill = SkillProtoData::Inst()->FindSkillProto(dwSkillID);
	if( pSkill->nType2 == ESSTE_Produce )
	{
		return TRUE;
	}
	return FALSE;
}