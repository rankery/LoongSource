#include "stdafx.h"
#include "ItemFrame.h"
#include "PocketFrame.h"
#include "StorageFrame.h"
#include "ThroneFrame.h"
#include "ItemProduceFrame.h"
#include "ShopShelfFrame.h"
#include "TradeFrame.h"
#include "ItemButton.h"
#include "ButtonEx.h"
#include "ItemEvent.h"
#include "CombatEvent.h"
#include "GuildEvent.h"
#include "QuestEvent.h"
#include "Item.h"
#include "CursorMgr.h"
#include "ItemMgr.h"
#include "NetSession.h"
#include "..\WorldDefine\msg_item.h"
#include "..\WorldDefine\msg_loot.h"
#include "..\WorldDefine\msg_talent.h"
#include "..\WorldDefine\msg_func_npc.h"
#include "..\WorldDefine\msg_exchange.h"
#include "GameClientDefine.h"
#include "QuestMgr.h"
#include "QuestScript.h"
#include "CombatSysUtil.h"
#include "SocialEvent.h"
#include "RoleMgr.h"
#include "LocalPlayer.h"
#include "PetEvent.h"
#include "SkillMgr.h"
#include "PetManager.h"
#include "GuildMgr.h"
#include "NPCTalkFrame.h"
#include "InsuranceFrame.h"
#include "FishMgr.h"
#include "RenameFrame.h"

ItemFrame::ItemFrame():m_Trunk(this), m_n64OperSrcItem(GT_INVALID)
{
	m_pPocket = NULL;
	m_pOperSrcItem = NULL;
    m_n64OperSrcItem = GT_INVALID;
	m_pOperDstItem = NULL;
	m_eCurStrategy = EIST_ItemPocket;
}

ItemFrame::~ItemFrame()
{

}

BOOL ItemFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam );

	m_pMgr->RegisterEventHandle(_T("Open_Package"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemFrame::OnOpenPackage));
	m_pMgr->RegisterEventHandle(_T("Close_Package"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemFrame::OnClosePackage));
	m_pMgr->RegisterEventHandle(_T("Open_Ware"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemFrame::OnOpenWare));
	m_pMgr->RegisterEventHandle(_T("Close_Ware"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemFrame::OnCloseWare));
	m_pMgr->RegisterEventHandle(_T("Close_Ware_Now"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemFrame::OnCloseWareNow));
	m_pMgr->RegisterEventHandle(_T("tagClickItemEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemFrame::OnTryToUseItem));
	m_pMgr->RegisterEventHandle(_T("ItemEvent_TargetItem"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemFrame::OnTheRoleOfItem));
	m_pMgr->RegisterEventHandle(_T("PutDownItem"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemFrame::OnRolePutDownItem));
	m_pMgr->RegisterEventHandle(_T("MsgBox_PutDownCheck"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemFrame::OnPutDownItemCheck));
	m_pMgr->RegisterEventHandle(_T("MsgBox_UseItemCheck"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemFrame::OnUseSpecItemCheck));
	m_pMgr->RegisterEventHandle(_T("MsgBox_AddPotTips"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemFrame::OnAddPotValue));
	
	return bRet;
}

BOOL ItemFrame::Destroy()
{
	BOOL bRet = GameFrame::Destroy();

	m_pMgr->UnRegisterEventHandler(_T("Open_Package"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemFrame::OnOpenPackage));
	m_pMgr->UnRegisterEventHandler(_T("Close_Package"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemFrame::OnClosePackage));
	m_pMgr->UnRegisterEventHandler(_T("Open_Ware"),					(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemFrame::OnOpenWare));
	m_pMgr->UnRegisterEventHandler(_T("Close_Ware"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemFrame::OnCloseWare));
	m_pMgr->UnRegisterEventHandler(_T("Close_Ware_Now"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemFrame::OnCloseWareNow));
	m_pMgr->UnRegisterEventHandler(_T("tagClickItemEvent"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemFrame::OnTryToUseItem));
	m_pMgr->UnRegisterEventHandler(_T("ItemEvent_TargetItem"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemFrame::OnTheRoleOfItem));
	m_pMgr->UnRegisterEventHandler(_T("PutDownItem"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemFrame::OnRolePutDownItem));
	m_pMgr->UnRegisterEventHandler(_T("MsgBox_PutDownCheck"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemFrame::OnPutDownItemCheck));
	m_pMgr->UnRegisterEventHandler(_T("MsgBox_UseItemCheck"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemFrame::OnUseSpecItemCheck));
	m_pMgr->UnRegisterEventHandler(_T("MsgBox_AddPotTips"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemFrame::OnAddPotValue));

	return bRet;
}

BOOL ItemFrame::ReloadUI()
{
	// 背包界面
	m_pPocket = (PocketFrame*)m_pFrameMgr->CreateFrame(m_strName.c_str(), _T("Pocket"), _T("PocketFrame"), 0);
	return TRUE;
}

DWORD ItemFrame::OnTryToUseItem( tagClickItemEvent* pGameEvent )
{
	
	switch (m_eCurStrategy)
	{
	case EIST_ItemPocket:
		{
			if(EICT_Bag == pGameEvent->eType || EICT_Quest == pGameEvent->eType)
			{
				if(MIsEquipment(pGameEvent->dwTypeID))
				{
					tagItemPosChangeExEvent event(_T("ItemEvent_Move2Equip"),this);
					event.eTypeSrc = pGameEvent->eType;
					event.eTypeDst = EICT_Equip;
					event.n16IndexSrc = pGameEvent->n16Index;
					event.dwTypeIDSrc = pGameEvent->dwTypeID;
					m_pMgr->SendEvent(&event);
				}
				else
				{
					// 物品使用
					Item* pItem = ItemMgr::Inst()->GetConItem(pGameEvent->eType, pGameEvent->n16Index);
					if(P_VALID(pItem))
					{
						//--检查玩家状态
						const DWORD RoleStateMask=ERS_Stall;
						if(RoleMgr::Inst()->GetLocalPlayer()->GetRoleState()&RoleStateMask)
							return 0;

						// 优先判断是否是特殊功用物品
						EItemSpecFunc eType = pItem->GetItemSpecFunc();
						if(eType != EISF_Null)
						{
							OnUseSpecItem(pItem);
						}
						// 判断物品是否可以使用
						else if(pItem->IsUseable())
						{						
							OnUseItem(pItem);
						}
					}
				}
			}
		}
		break;
	case EIST_ItemStorage:
		{
			// 不是背包发送的直接返回
			if (pGameEvent->pSender == NULL)
				return 0;

			if(EICT_Bag == pGameEvent->eType)
			{
				tagItemPosChangeExEvent event(_T("ItemEvent_Move2Ware"),this);
				event.eTypeSrc = EICT_Bag;
				event.eTypeDst = EICT_RoleWare;
				event.n16IndexSrc = pGameEvent->n16Index;
				event.dwTypeIDSrc = pGameEvent->dwTypeID;
				m_pMgr->SendEvent(&event);
			}
			else if(EICT_RoleWare == pGameEvent->eType)
			{
				tagItemPosChangeExEvent event(_T("ItemEvent_Move2Bag"),this);
				event.eTypeSrc = EICT_RoleWare;
				event.eTypeDst = EICT_Bag;
				event.n16IndexSrc = pGameEvent->n16Index;
				event.dwTypeIDSrc = pGameEvent->dwTypeID;
				m_pMgr->SendEvent(&event);
			}
		}
		break;
    case EIST_GuildStorage:
        {
            // 判断是否有权限
            if(!GuildMgr::Inst()->isWareOpretable())
            {
                CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("GuildPrivacyUseErr")]);
                return 0;
            }

            // 判断是否绑定
            Item* pItem = ItemMgr::Inst()->GetConItem(EICT_Bag, pGameEvent->n16Index);
            if (P_VALID(pItem))
            {
                if (pItem->IsItemBinded())
                {
                    CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("GuildWareBinding")]);
                    return 0;
                }
            }

            // 不是背包发送的直接返回
            if (pGameEvent->pSender == NULL)
                return 0;

            if (EICT_Bag == pGameEvent->eType)
            {
                tagItemPosChangeExEvent event(_T("ItemEvent_Move2Guild"),this);
                event.eTypeSrc = EICT_Bag;
                event.eTypeDst = EICT_GuildWare;
                event.n16IndexSrc = pGameEvent->n16Index;
                event.dwTypeIDSrc = pGameEvent->dwTypeID;
                m_pMgr->SendEvent(&event);
            }
        }
        break;
	case EIST_ItemShopVend:
		{
			// 不是背包发送的直接返回
			if (pGameEvent->pSender == NULL)
				return 0;

			if(EICT_Bag == pGameEvent->eType)
			{
				tagItemPosChangeExEvent event(_T("ItemEvent_Move2Shop"),this);
				event.eTypeSrc = EICT_Bag;
				event.eTypeDst = EICT_Shop;
				event.n16IndexSrc = pGameEvent->n16Index;
				event.dwTypeIDSrc = pGameEvent->dwTypeID;
				m_pMgr->SendEvent(&event);
			}
		}
		break;
	case EIST_ItemPlayerTrade:
		{
			// 不是背包发送的直接返回
			if (pGameEvent->pSender == NULL)
				return 0;

			if(EICT_Bag == pGameEvent->eType)
			{
				tagItemPosChangeExEvent event(_T("ItemEvent_Move2Trade"),this);
				event.eTypeSrc = EICT_Bag;
				event.eTypeDst = EICT_End;
				event.n16IndexSrc = pGameEvent->n16Index;
				event.dwTypeIDSrc = pGameEvent->dwTypeID;
				m_pMgr->SendEvent(&event);
			}
		}
		break;
	case EIST_ItemStall:
		{
			// 不是背包发送的直接返回
			if (pGameEvent->pSender == NULL)
				return 0;

			if(EICT_Bag == pGameEvent->eType)
			{
				tagItemPosChangeExEvent event(_T("ItemEvent_Move2Stall"),this);
				event.eTypeSrc = EICT_Bag;
				event.eTypeDst = EICT_End;
				event.n16IndexSrc = pGameEvent->n16Index;
				event.dwTypeIDSrc = pGameEvent->dwTypeID;
				m_pMgr->SendEvent(&event);
			}
		}
		break;
	case EIST_ItemSmithCompose:
		{
			// 不是背包发送的直接返回
			if (pGameEvent->pSender == NULL)
				return 0;

			if( EICT_Bag == pGameEvent->eType )
			{
				tagItemPosChangeExEvent event(_T("ItemEvent_Move2Compose"),this);
				event.eTypeSrc = EICT_Bag;
				event.eTypeDst = EICT_End;
				event.n16IndexSrc = pGameEvent->n16Index;
				event.dwTypeIDSrc = pGameEvent->dwTypeID;
				m_pMgr->SendEvent(&event);
			}
		}
		break;
	case EIST_ItemInsurance:
		{
			// 不是背包发送的直接返回
			if (pGameEvent->pSender == NULL)
				return 0;

			if( EICT_Bag == pGameEvent->eType )
			{
				tagItemPosChangeExEvent event(_T("ItemEvent_Move2Insurance"),this);
				event.eTypeSrc = EICT_Bag;
				event.eTypeDst = EICT_End;
				event.n16IndexSrc = pGameEvent->n16Index;
				event.dwTypeIDSrc = pGameEvent->dwTypeID;
				m_pMgr->SendEvent(&event);
			}
		}
		break;
	case EIST_GuildUpgrade:
		{
			// 不是背包发送的直接返回
			if ( NULL != pGameEvent->pSender && EICT_Bag == pGameEvent->eType )
			{
				tagItemPosChangeExEvent event(_T("ItemEvent_Move2GuildUpgrade"),this);
				event.eTypeSrc = EICT_Bag;
				event.eTypeDst = EICT_End;
				event.n16IndexSrc = pGameEvent->n16Index;
				event.dwTypeIDSrc = pGameEvent->dwTypeID;
				m_pMgr->SendEvent(&event);
			}
		}
		break;
	}
	return 0;
}

VOID ItemFrame::OnUseItem(Item* pItem)
{
	tagUseItemEvent e(_T("tagUseItemEvent"), this);
	e.dwItemTypeID = pItem->GetItemTypeID();
	e.n64ItemID = pItem->GetItemId();
	m_pFrameMgr->SendEvent(&e);
}

VOID ItemFrame::OnUseSpecItem(Item* pItem)
{
	switch (pItem->GetItemSpecFunc())
	{
	case EISF_AcceptQuest:
		{
			tagUseQuestEvent e(_T("QuestEvent_UseQuest"), this);
			e.itemID = pItem->GetItemTypeID();
			e.questID = pItem->GetProto()->nSpecFuncVal1;
			m_pFrameMgr->SendEvent(&e);
		}
		break;
	case EISF_IdetifyEquip:
		{
			tagActiveCursorEvent e(_T("Event_CursorActive"), this);
			e.eCursor = ECAN_Identify;
			e.dwParam = pItem->GetPos();
			m_pFrameMgr->SendEvent(&e);
		}
		break;
	case EISF_HoleGem:
		{
			tagActiveCursorEvent e(_T("Event_CursorActive"),this);
			e.eCursor = ECAN_Gem;
			e.dwParam = pItem->GetPos();
			m_pFrameMgr->SendEvent(&e);
		}
		break;
	case EISF_Brand:
		{
			tagActiveCursorEvent e(_T("Event_CursorActive"),this);
			e.eCursor = ECAN_Brand;
			e.dwParam = pItem->GetPos();
			m_pFrameMgr->SendEvent(&e);

		}
		break;
	case EISF_LongHunShi:
		{
			tagActiveCursorEvent e(_T("Event_CursorActive"),this);
			e.eCursor = ECAN_Longfu;
			e.dwParam = pItem->GetPos();
			m_pFrameMgr->SendEvent(&e);
		}
		break;
	case EISF_Chisel:
		{
			tagActiveCursorEvent e(_T("Event_CursorActive"),this);
			e.eCursor = ECAN_Chisel;
			e.dwParam = pItem->GetPos();
			m_pFrameMgr->SendEvent(&e);
		}
		break;
	case EISF_Grind:
		{
			tagActiveCursorEvent e(_T("Event_CursorActive"),this);
			e.eCursor = ECAN_Grind;
			e.dwParam = pItem->GetPos();
			m_pFrameMgr->SendEvent(&e);
		}
		break;
	case EISF_EquipAddPotValue:
		{
			tagActiveCursorEvent e(_T("Event_CursorActive"),this);
			e.eCursor = ECAN_AddPot;
			e.dwParam = pItem->GetPos();
			m_pFrameMgr->SendEvent(&e);
		}
		break;
	case EISF_BeGoldStone:
		{
			tagActiveCursorEvent e(_T("Event_CursorActive"),this);
			e.eCursor = ECAN_BeGoldStone;
			e.dwParam = pItem->GetPos();
			m_pFrameMgr->SendEvent(&e);
		}
		break;
	case EISF_ChestKey:// 宝箱钥匙
		{
			tagActiveCursorEvent e(_T("Event_CursorActive"),this);
			e.eCursor = ECAN_Key;
			e.dwParam = pItem->GetPos();
			m_pFrameMgr->SendEvent(&e);
		}
		break;
	case EISF_RemoveAttPt:
		{
			ShowUseItemCheckDlg(pItem);
		}
		break;
	case EISF_RemoveTalentPt:
		{
			ShowUseItemCheckDlg(pItem);
		}
		break;
	case EISF_FriendGift:
		{
			tagUseGiftEvent e(_T("SocialEvent_UseGift"), this);
			e.giftGlobalID	= pItem->GetItemId();
			e.giftTypeID	= pItem->GetItemTypeID();
			m_pFrameMgr->SendEvent(&e);
		}
		break;
    case EISF_PetEgg:
        {
            ShowUseItemCheckDlg(pItem);
        }
        break;
    case EISF_PetFood:
        {
            tagUsePetFoodEvent e(_T("tagUsePetFoodEvent"), this);
            e.n64ItemID     = pItem->GetItemId();
            m_pFrameMgr->SendEvent(&e);
        }
        break;
    case EISF_PetSkillBook:
        {
            ShowUseItemCheckDlg(pItem);
        }
        break;
    case EISF_PetEquipment:
        {
            tagUsePetEquip e(_T("tagUsePetEquip"), this);
            e.n64ItemID     =   pItem->GetItemId();
            m_pFrameMgr->SendEvent(&e);
        }
        break;
    case EISF_PetLvlupStep:
        {
            PetManager::Inst()->OnUseLvlupStep(pItem);
        }
        break;
    case EISF_PetExpPour:
        {
            PetManager::Inst()->OnUseExpPour(pItem);
        }
        break;
    case EISF_PetEnhance:
        {
            PetManager::Inst()->OnUseEnhance(pItem);
        }
        break;
    case EISF_ColorProbability:
        {
        }
        break;
    case EISF_Dye:
        {
            tagActiveCursorEvent e(_T("Event_CursorActive"),this);
            e.eCursor = ECAN_Dye;
            e.dwParam = pItem->GetPos();
            m_pFrameMgr->SendEvent(&e);
        }
        break;
	case EISF_LearnSkill:// 技能书
		{
			// 判断技能是否可学，可学则走物品使用正常流程，不可使用则显示错误信息
			DWORD dwRet = SkillMgr::Inst()->IsSkillCanLearn((DWORD)pItem->GetProto()->nSpecFuncVal1);
			if(E_Success == dwRet)
			{
				ShowUseItemCheckDlg(pItem);
			}
			else
			{
				SkillMgr::Inst()->ShowNetError(dwRet);
			}
		}
		break;
	case EISF_GuildSkillBook:
		{
			tagUseGuildSkillBookEvent event(_T("GuildEvent_UseGuildSkillBook"), this);
			event.pItem = pItem;
			m_pFrameMgr->SendEvent(&event);
		}
        break;
    case EISF_PetLock:
        {
            PetManager::Inst()->SetPetLock(pItem->GetItemId(), true);
        }
        break;
    case EISF_PetUnLock:
        {
            PetManager::Inst()->SetPetLock(pItem->GetItemId(), false);
        }
        break;
	case EISF_PortableWare:									//随身仓库
		{
			tagNC_WareOpen e;
			e.n64ItemID = pItem->GetItemId();
			m_pSession->Send( &e );
		}
		break;
	case EISF_Fishing:
		{
			FishMgr::Inst()->SetFishFoodType( pItem->GetItemTypeID() );
			OnUseItem( pItem );
		}
		break;
    case EISF_TransportTool:                                //可记录传送符
        {
            NPCTalkFrame* pNpcTalkFrame = (NPCTalkFrame*)m_pFrameMgr->GetFrame(_T("NPCTalk"));
            if (!P_VALID(pNpcTalkFrame))
            {
                pNpcTalkFrame = (NPCTalkFrame*)m_pFrameMgr->CreateFrame( _T("World"), _T("NPCTalk"), _T("NPCTalkFrame"), 0 );
            }

            pNpcTalkFrame->ClearOptions();
            TCHAR szTemp[256] = {0};
            LocalPlayer* pLp = RoleMgr::Inst()->GetLocalPlayer();
            if (P_VALID(pLp))
            {
                _sntprintf(szTemp, sizeof(szTemp) / sizeof(TCHAR), g_StrTable[_T("TransportItemDesc")]
                    , pLp->GetTransportPointName().c_str(), pLp->GetTransportPoint().x, pLp->GetTransportPoint().z);
            }
            pNpcTalkFrame->SetTransportItemEnable(true);
            pNpcTalkFrame->SetContent(szTemp);
            pNpcTalkFrame->AddTalkOption(g_StrTable[_T("TransportSet")], 0, 0, NPCTalkFrame::EOT_SetTransportPoint);
            pNpcTalkFrame->AddTalkOption(g_StrTable[_T("TransportGo")], 1, 0, NPCTalkFrame::EOT_SetTransportPoint);
            pNpcTalkFrame->AddTalkOption(g_StrTable[_T("TransportCanel")], 2, 0, NPCTalkFrame::EOT_SetTransportPoint);
            pNpcTalkFrame->Show();
            pNpcTalkFrame->SetTransItem(pItem);
        }
		break;
	case EISF_RenameItem:
		{
			ShowUseItemCheckDlg( pItem );
		}
        break;
	}
}

DWORD ItemFrame::OnOpenPackage( tagGameEvent* pGameEvent )
{
	if(RoleMgr::Inst()->GetLocalPlayer()->GetRoleStateEx(ERSE_BagPsdPass))
	{
		m_pPocket->ShowWnd(TRUE);
	}
	else
	{
		if(P_VALID(GetObj("OpenBagPsd")))
			KillObj("OpenBagPsd");

		CreateObj("OpenBagPsd", "MsgInputBox");
		TObjRef<MsgInputBox>("OpenBagPsd")->Init(
			_T(""),g_StrTable[_T("PocketEncrypt_TextCurrentPassword")],
			_T("MsgBox_OpenBagPsd"),GT_INVALID, MBIT_Password);

	}
	return 0;
}

DWORD ItemFrame::OnClosePackage( tagGameEvent* pGameEvent )
{
	m_pPocket->ShowWnd(FALSE);
	return 0;
}

DWORD ItemFrame::OnTheRoleOfItem( tagOnTheRoleOfItemEvent* pGameEvent )
{
	switch (pGameEvent->eSpecFuncSrc)
	{
	case EISF_IdetifyEquip:
		{
			//--只有装备才可以被鉴定
			if(MIsEquipment(pGameEvent->dwTypeIDDst))
			{
				//--鉴定卷和所要鉴定的装备都应该在背包中
				if(pGameEvent->eTypeSrc == EICT_Bag &&
					pGameEvent->eTypeDst == EICT_Bag)
				{
					Item* pItemSrc = ItemMgr::Inst()->GetConItem(pGameEvent->eTypeSrc, pGameEvent->n16IndexSrc);
					Item* pItemDst = ItemMgr::Inst()->GetConItem(pGameEvent->eTypeDst, pGameEvent->n16IndexDst);
					//--确定找到两物品
					ASSERT(P_VALID(pItemSrc) && P_VALID(pItemDst));
					//--两个物品的TypeID相符
					ASSERT(pItemSrc->GetItemTypeID() == pGameEvent->dwTypeIDSrc && pItemDst->GetItemTypeID() == pGameEvent->dwTypeIDDst);
					
					if(EISF_IdetifyEquip == pItemSrc->GetItemSpecFunc()
						&& ((Equipment*)pItemDst)->IsNotIdetified())
					{
						ShowUseItemCheckDlg(pItemSrc, pItemDst);
					}
				}
			}
		}
		break;
	case EISF_Grind:
		{
			// 先需要判断是否是装备，因为只有武器才有崭新，武器属于装备
			if(MIsEquipment(pGameEvent->dwTypeIDDst))
			{
				//--磨石和所要打磨的装备都应该在背包中
				if(pGameEvent->eTypeSrc == EICT_Bag &&
					pGameEvent->eTypeDst == EICT_Bag)
				{
					Item* pItemSrc = ItemMgr::Inst()->GetConItem(pGameEvent->eTypeSrc, pGameEvent->n16IndexSrc);
					Item* pItemDst = ItemMgr::Inst()->GetConItem(pGameEvent->eTypeDst, pGameEvent->n16IndexDst);
					//--确定找到两物品
					ASSERT(P_VALID(pItemSrc) && P_VALID(pItemDst));
					//--两个物品的TypeID相符
					ASSERT(pItemSrc->GetItemTypeID() == pGameEvent->dwTypeIDSrc && pItemDst->GetItemTypeID() == pGameEvent->dwTypeIDDst);
					
					if(EISF_Grind == pItemSrc->GetItemSpecFunc()
						&& pItemDst->GetProto()->eType == EIT_Weapon)
					{
						ShowUseItemCheckDlg(pItemSrc, pItemDst);
					}
				}
			}
		}
		break;
    case EISF_Dye:
        {
            // 获得相关物品
            Item* pItemSrc = ItemMgr::Inst()->GetConItem(pGameEvent->eTypeSrc, pGameEvent->n16IndexSrc);
            Equipment* pFashionDst = static_cast<Equipment*>(ItemMgr::Inst()->GetConItem(pGameEvent->eTypeDst, pGameEvent->n16IndexDst));
            ASSERT(P_VALID(pItemSrc) && P_VALID(pFashionDst));

            // 判断是否是时装
            if (pFashionDst->GetProto()->eType != EIT_Fashion)
            {
                CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("DyeFashionErr1")]);
            }
            else
            {
                // 如果此时装已有同样的颜色则不需要染
                if (pItemSrc->GetProto()->nSpecFuncVal1 == pFashionDst->GetEquip()->n8ColorID)
                {
                    CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("DyeFashionErr2")]);
                }
                else
                {
                    // 判断等级
                    if (pItemSrc->GetItemLevel() < pFashionDst->GetItemLevel() / 10)
                    {
                        CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("DyeFashionErr3")]);
                    }
                    else
                    {
                        ShowUseItemCheckDlg(pItemSrc, pFashionDst);
                    }
                }
            }
        }
        break;
	case EISF_EquipAddPotValue:
		{
			// 先需要判断是否是装备,只有装备才可以提升潜力
			if(MIsEquipment(pGameEvent->dwTypeIDDst))
			{
				//--道具和装备是否都在包里
				if(pGameEvent->eTypeSrc == EICT_Bag &&
					pGameEvent->eTypeDst == EICT_Bag)
				{
					Item* pItemSrc = ItemMgr::Inst()->GetConItem(pGameEvent->eTypeSrc, pGameEvent->n16IndexSrc);
					Item* pItemDst = ItemMgr::Inst()->GetConItem(pGameEvent->eTypeDst, pGameEvent->n16IndexDst);
					//--确定找到两物品
					ASSERT(P_VALID(pItemSrc) && P_VALID(pItemDst));
					//--两个物品的TypeID相符
					ASSERT(pItemSrc->GetItemTypeID() == pGameEvent->dwTypeIDSrc && pItemDst->GetItemTypeID() == pGameEvent->dwTypeIDDst);

					Equipment *pEquip = (Equipment*)pItemDst;
					if(P_VALID(pEquip))
					{
						INT32 nCurPot = pEquip->GetEquipPotVal();	// 当前可用潜力值
						INT32 nMaxPot = pEquip->GetEquipPotMax();
						if( nCurPot >= nMaxPot )
						{
							CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("EquipPotIsMax")]);
							return 0;
						}
						// 潜力值可提升次数为0
						if( 0 == pEquip->GetEquipPotAdvanceTime() )
						{
							CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("EquipNoAddPotTime")]);
							return 0;
						}

						INT32 nAddPot = pItemSrc->GetProto()->nSpecFuncVal2;	// 可提升的潜力值
						if( nCurPot + nAddPot > nMaxPot )
						{
							INT32 nCanAddPot = nMaxPot - nCurPot;
							if( nCanAddPot > 0 )
							{
								if(P_VALID(GetObj("AddPotTips")))
									KillObj("AddPotTips");

								TCHAR szTemp[X_LONG_NAME] = {0};
								_sntprintf(szTemp, sizeof(szTemp)/sizeof(TCHAR), g_StrTable[_T("AddPotValue")], nAddPot, nCanAddPot );
								CreateObj("AddPotTips", "MsgBox");
								TObjRef<MsgBox>("AddPotTips")->Init( _T(""), szTemp, _T("MsgBox_AddPotTips"), MsgBoxFlag(MBF_OK | MBF_Cancel), TRUE );
								m_pOperSrcItem = pItemSrc;
								m_n64OperSrcItem = pItemSrc->GetItemId();
								m_pOperDstItem = pItemDst;
							}
						}
						else
						{
							if(EISF_EquipAddPotValue == pItemSrc->GetItemSpecFunc()
								&& pItemDst->GetProto()->eType == EIT_Weapon)
							{
								ShowUseItemCheckDlg(pItemSrc, pItemDst);
							}
						}
					}
				}
			}
			else
			{
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ItemCanNotAddPot")]);
			}
		}
		break;
	case EISF_BeGoldStone:
		{
			//--道具和装备是否都在包里
			if(pGameEvent->eTypeSrc == EICT_Bag &&
				pGameEvent->eTypeDst == EICT_Bag)
			{
				Item* pItemSrc = ItemMgr::Inst()->GetConItem(pGameEvent->eTypeSrc, pGameEvent->n16IndexSrc);
				Item* pItemDst = ItemMgr::Inst()->GetConItem(pGameEvent->eTypeDst, pGameEvent->n16IndexDst);
				//--确定找到两物品
				ASSERT(P_VALID(pItemSrc) && P_VALID(pItemDst));
				//--两个物品的TypeID相符
				ASSERT(pItemSrc->GetItemTypeID() == pGameEvent->dwTypeIDSrc && pItemDst->GetItemTypeID() == pGameEvent->dwTypeIDDst);
				DWORD dwGoldStoneType = pItemSrc->GetProto()->nSpecFuncVal1;
				if( ItemMgr::Inst()->IsInGoldStoneTable( pItemDst->GetItemTypeID(), dwGoldStoneType ) )
				{
					if( EISF_BeGoldStone == pItemSrc->GetItemSpecFunc() )
					{
						ShowUseItemCheckDlg(pItemSrc, pItemDst);
					}
				}
				else
				{
					if( dwGoldStoneType == 1 )
					{
						CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("CanNotUseGoldStone")]);
					}
					else if( dwGoldStoneType == 2 )
					{
						CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("CanNotUseMoreGoldStone")]);
					}
				}
			}
		}
		break;
	default:
		{
			Item* pItemSrc = ItemMgr::Inst()->GetConItem(pGameEvent->eTypeSrc, pGameEvent->n16IndexSrc);
			Item* pItemDst = ItemMgr::Inst()->GetConItem(pGameEvent->eTypeDst, pGameEvent->n16IndexDst);
			//--确定找到两物品
			ASSERT(P_VALID(pItemSrc) && P_VALID(pItemDst));
			ShowUseItemCheckDlg(pItemSrc, pItemDst);
		}
		break;
	}
	return 0;
}

DWORD ItemFrame::OnRolePutDownItem( tagPutDownItemEvent* pGameEvent )
{
	if(EDT_Item == pGameEvent->eType)
	{
		INT16 n16Pos = (INT16)LOWORD(pGameEvent->dwParam);
		Item* pItem = ItemMgr::Inst()->GetPocketItem(n16Pos);
		if(P_VALID(pItem) && pGameEvent->dwTypeID == pItem->GetItemTypeID())
		{
			//判断是否可丢弃
			if(pItem->IsDroppable())
			{
				TCHAR szTmp[X_LONG_NAME] = {0};
				BOOL bSendOnly = TRUE; //标志位，是否可以直接发送丢弃消息
				
				if(!pItem->IsTradeable())//判断是否可交易，不可交易的物品需要提示是否删除该物品
				{
					_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), g_StrTable[_T("RoleDeleteItem_Check")], pItem->GetDisplay()->szName, pItem->GetItemQuantity());
					bSendOnly = FALSE;
				}
				else if(pItem->GetItemQuality() > 0)//判断物品评级，白品以上都提示
				{
					_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), g_StrTable[_T("RolePushDownItem_Check")], pItem->GetDisplay()->szName, pItem->GetItemQuantity());
					bSendOnly = FALSE;
				}

				if(bSendOnly)
				{
					SendPutDownItem(pItem->GetItemId());
				}
				else//需要弹出对话框
				{
					if(P_VALID(GetObj("UseItemCheck")))
						KillObj("UseItemCheck");

					CreateObj("UseItemCheck", "MsgBox");
					TObjRef<MsgBox>("UseItemCheck")->Init(_T(""), szTmp, _T("MsgBox_PutDownCheck"));
					m_pOperSrcItem = pItem;
                    m_n64OperSrcItem = pItem->GetItemId();

				}
			}
			else
			{
				// 说明不可丢弃的原因
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("CantDrop")]);
			}
		}
	}
	return 0;
}

DWORD ItemFrame::OnPutDownItemCheck( tagMsgBoxEvent* pGameEvent )
{
	if(MBF_OK == pGameEvent->eResult)
	{
		ASSERT(P_VALID(m_pOperSrcItem));
		if(P_VALID(m_pOperSrcItem))
		{
			SendPutDownItem(m_pOperSrcItem->GetItemId());
		}
	}

	m_pOperSrcItem = NULL;
    m_n64OperSrcItem = GT_INVALID;

	return 0;
}


VOID ItemFrame::SendPutDownItem( INT64 n64ItemSerial )
{
	tagNC_RolePutDownItem m;
	m.n64Serial = n64ItemSerial;
	m_pSession->Send(&m);
}


DWORD ItemFrame::OnOpenWare( tagGameEvent* pGameEvent )
{
	// 打开仓库界面
	GameFrame* pFrame = m_pFrameMgr->GetFrame(_T("Storage"));
	if (!P_VALID(pFrame))
	{
		pFrame = m_pFrameMgr->CreateFrame(m_strName.c_str(), _T("Storage"), _T("StorageFrame"), 0);
		if(P_VALID(pFrame))
			this->SetCurStrategy(EIST_ItemStorage);
	}

	return 0;
}

DWORD ItemFrame::OnCloseWare( tagGameEvent* pGameEvent )
{
	// 关闭仓库窗口
	GameFrame* pFrame = m_pFrameMgr->GetFrame(_T("Storage"));
	if(P_VALID(pFrame))
	{
		m_pFrameMgr->AddToDestroyList(pFrame);
		m_eCurStrategy = EIST_ItemPocket;
	}
	return 0;
}

DWORD ItemFrame::OnCloseWareNow( tagGameEvent* pGameEvent )
{
	// 关闭仓库窗口
	GameFrame* pFrame = m_pFrameMgr->GetFrame(_T("Storage"));
	if(P_VALID(pFrame))
	{
		m_pFrameMgr->AddToDestroyList(pFrame);
		m_eCurStrategy = EIST_ItemPocket;
	}
	return 0;
}

VOID ItemFrame::ShowUseItemCheckDlg( Item* pSrcItem, Item* pDstItem /*= NULL*/ )
{
	if(!P_VALID(pSrcItem))
		return;

	TCHAR szTmp[X_LONG_NAME];

	if(P_VALID(GetObj("UseItemCheck")))
		KillObj("UseItemCheck");
	
	if(P_VALID(pDstItem))
	{
		_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), g_StrTable[_T("UseItem_Check2")], pDstItem->GetItemName().c_str(), pSrcItem->GetItemName().c_str());
	}
	else
	{
		_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), g_StrTable[_T("UseItem_Check1")], pSrcItem->GetItemName().c_str());
	}
	CreateObj("UseItemCheck", "MsgBox");
	TObjRef<MsgBox>("UseItemCheck")->Init(_T(""), szTmp, _T("MsgBox_UseItemCheck"));
	m_pOperSrcItem = pSrcItem;
    m_n64OperSrcItem = pSrcItem->GetItemId();
	m_pOperDstItem = pDstItem;
}

DWORD ItemFrame::OnUseSpecItemCheck( tagMsgBoxEvent* pGameEvent )
{
    if (!P_VALID(ItemMgr::Inst()->GetConItemByID(EICT_Bag, m_n64OperSrcItem)))
        return 0;

	if(!P_VALID(m_pOperSrcItem))
		return 0;

	if(MBF_OK == pGameEvent->eResult)
	{
		switch(m_pOperSrcItem->GetItemSpecFunc())
		{
		case EISF_IdetifyEquip:// 鉴定卷
			{
				if(!P_VALID(m_pOperDstItem))
					return 0;

				tagNC_IdentifyEquip cmd;
				cmd.n64SerialReel = m_pOperSrcItem->GetItemId();
				cmd.n64SerialEquip = m_pOperDstItem->GetItemId();
				m_pSession->Send(&cmd);
			}
			break;
		case EISF_Grind:// 磨石
			{
				if(!P_VALID(m_pOperDstItem))
					return 0;

				tagNC_Abrase cmd;
				cmd.n64AbraserSerial = m_pOperSrcItem->GetItemId();
				cmd.n64WeaponSerial	 = m_pOperDstItem->GetItemId();
				m_pSession->Send(&cmd);
			}
			break;
		case EISF_Dye:// 染料
			{
				if(!P_VALID(m_pOperDstItem))
					return 0;

				// 发送染色请求
				tagNC_DyeFashion msg;
				msg.n64DyeSN        =   m_pOperSrcItem->GetItemId();
				msg.n64EquipSerial  =   m_pOperDstItem->GetItemId();
				m_pSession->Send(&msg);
			}
			break;
		case EISF_RemoveAttPt:// 洗属性
			{
				tagNC_RoleClearAttPoint m;
				m.n64ItemID = m_pOperSrcItem->GetItemId();
				m_pSession->Send(&m);
			}
			break;
		case EISF_RemoveTalentPt:// 洗技能
			{
				tagNC_ClearTalent m;
				m.n64ItemID = m_pOperSrcItem->GetItemId();
				m.eType = ETT_Blade;
				m_pSession->Send(&m);
			}
			break;
		case EISF_LearnSkill:// 学技能
			{
				OnUseItem(m_pOperSrcItem);
			}
			break;
		case EISF_PetEgg:// 宠物蛋
			{
				tagUsePetEggEvent e(_T("tagUsePetEggEvent"), this);
				e.n64ItemID      =   m_pOperSrcItem->GetItemId();
				m_pFrameMgr->SendEvent(&e);
			}
			break;
        case EISF_PetSkillBook: // 宠物技能书
            {
                DWORD dwPetID = PetManager::Inst()->GetCurrPetID();
                if (dwPetID != GT_INVALID)
                {
                    PetManager::Inst()->BeginNS_PetLearnSkill(dwPetID, m_pOperSrcItem->GetItemId());
                }
            }
            break;
		case EISF_HoleGem:// 宝石	
		case EISF_Brand:// 烙印
		case EISF_LongHunShi:// 龙附石
		case EISF_Chisel:// 凿具
			{
				if(!P_VALID(m_pOperDstItem))
					return 0;

				tagOnTheRoleOfItemEvent e(_T("ItemEvent_ConsolidateItem"), NULL);
				e.eTypeSrc = m_pOperSrcItem->GetConType();
				e.n16IndexSrc = m_pOperSrcItem->GetPos();
				e.dwTypeIDSrc = m_pOperSrcItem->GetItemTypeID();
				e.eSpecFuncSrc = m_pOperSrcItem->GetItemSpecFunc();
				e.eTypeDst = m_pOperDstItem->GetConType();
				e.n16IndexDst = m_pOperDstItem->GetPos();
				e.dwTypeIDDst = m_pOperDstItem->GetItemTypeID();
				m_pFrameMgr->SendEvent(&e);
			}
			break;
		case EISF_EquipAddPotValue: // 提升装备潜力
			{
				InsuranceFrame* pFrame = (InsuranceFrame*)TObjRef<GameFrameMgr>()->GetFrame(_T("Insurance"));
				if( P_VALID(pFrame) )
				{
					pFrame->ShowFrame(TRUE, EISF_EquipAddPotValue, m_pOperSrcItem->GetItemId(), m_pOperDstItem->GetItemId());
					SetStrategy(EIST_ItemInsurance);
				}
			}
		case EISF_ChestKey:// 宝箱钥匙
			{
				if(!P_VALID(m_pOperDstItem))
					return 0;
				
				tagOnTheRoleOfItemEvent e(_T("ItemEvent_UnlockChest"), NULL);
				e.eTypeSrc = m_pOperSrcItem->GetConType();
				e.n16IndexSrc = m_pOperSrcItem->GetPos();
				e.dwTypeIDSrc = m_pOperSrcItem->GetItemTypeID();
				e.eSpecFuncSrc = m_pOperSrcItem->GetItemSpecFunc();
				e.eTypeDst = m_pOperDstItem->GetConType();
				e.n16IndexDst = m_pOperDstItem->GetPos();
				e.dwTypeIDDst = m_pOperDstItem->GetItemTypeID();
				m_pFrameMgr->SendEvent(&e);
			}
			break;
		case EISF_BeGoldStone:	// 点金石
			{
				if(!P_VALID(m_pOperDstItem))
					return 0;

				tagNC_BeGoldStone cmd;
				cmd.n64SrcItemID = m_pOperSrcItem->GetItemId();
				cmd.n64DstItemID = m_pOperDstItem->GetItemId();
				m_pSession->Send(&cmd);
			}
			break;
		case EISF_RenameItem: // 易名贴
			{
				RenameFrame* pFrame = (RenameFrame*)TObjRef<GameFrameMgr>()->GetFrame(_T("Rename"));
				if( !P_VALID(pFrame) )
					pFrame = (RenameFrame*)TObjRef<GameFrameMgr>()->CreateFrame( _T("World"), _T("Rename"), _T("RenameFrame"), 0 );
				if( P_VALID(pFrame) )
					pFrame->ShowFrame( TRUE, m_pOperSrcItem->GetItemId() );
			}
			break;
		}
	}

	m_pOperSrcItem = NULL;
    m_n64OperSrcItem = GT_INVALID;
	m_pOperDstItem = NULL;
	return 0;
}

DWORD ItemFrame::OnAddPotValue( tagMsgBoxEvent* pGameEvent )
{
	if(MBF_OK == pGameEvent->eResult)
	{
		ShowUseItemCheckDlg( m_pOperSrcItem, m_pOperDstItem );
	}
	else
	{
		m_pOperSrcItem = NULL;
		m_n64OperSrcItem = GT_INVALID;
		m_pOperDstItem = NULL;
	}

	return 0;
}

VOID ItemFrame::SetCurStrategy( EItemStrategy eType )
{
	// 如果不是设置为背包，则需要将上一次打开的界面关闭
	if(EIST_ItemPocket != eType && m_eCurStrategy != eType)
	{
		switch(m_eCurStrategy)
		{
		case EIST_ItemStorage://仓库
			{
				tagGameEvent event(_T("Close_Ware"), this);
				m_pMgr->SendEvent(&event);
			}
			break;
		case EIST_ItemPlayerTrade://玩家交易
			{
				tagNC_ExchangeCancel m;
				m_pSession->Send(&m);
			}
			break;
		case EIST_ItemShopVend:// 商店售卖
			{
				tagGameEvent event(_T("Close_Shop"), this);
				m_pMgr->SendEvent(&event);
			}
			break;
		case EIST_ItemStall:// 摆摊
			{
				tagGameEvent event(_T("StallEvent_CloseWindow"), this);
				m_pMgr->SendEvent(&event);
			}
			break;
		case EIST_ItemSmithCompose:// 合成
			{
				tagGameEvent event(_T("Close_Compose"), this);
				m_pMgr->SendEvent(&event);
			}
			break;
		case EIST_ItemInsurance:// 保底符
			{
				tagGameEvent event(_T("Close_Insurance"), this);
				m_pMgr->SendEvent(&event);
			}
			break;
        case EIST_GuildStorage: // 帮派仓库
            {
                tagGameEvent event(_T("Close_GuildWare"), this);
                m_pMgr->SendEvent(&event);
            }
            break;
		case EIST_GuildUpgrade: // 帮派升级
			{
				tagGameEvent event(_T("Close_GuildUpgrade"), this);
				m_pMgr->SendEvent(&event);
			}
			break;
		default:
			break;
		}
	}
	m_eCurStrategy = eType;
}