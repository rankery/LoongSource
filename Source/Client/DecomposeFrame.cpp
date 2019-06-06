#include "stdafx.h"
#include "DecomposeFrame.h"
#include "DragBtn.h"
#include "ProduceData.h"
#include "Item.h"
#include "PocketFrame.h"
#include "Container.h"
#include "ItemMgr.h"
#include "ItemProtoData.h"
#include "ToolTipCreator.h"
#include "ToolTipFrame.h"
#include "CurrencyMgr.h"
#include "..\WorldDefine\ItemDefine.h"
#include "SkillMgr.h"
#include "RoleMgr.h"
#include "AudioSys.h"
#include "SkillEvent.h"
#include "ItemFrame.h"
#include "CombatSysUtil.h"

#define MIsRWeapon(pProtoType)		(pProtoType->eType == EIT_Weapon)
#define MIsClothArmor(pProtoType)	(pProtoType->eType == EIT_ClothArmor)
#define MIsDecoration(pProtoType)	(pProtoType->eType == EIT_Decoration)
#define MIsArmor(pProtoType)		(pProtoType->eType == EIT_Armor)
const TCHAR *szDecomposePic = _T("data\\ui\\main\\l_icon_back.bmp");

DeComposeFrame::DeComposeFrame( void ):m_Trunk(this)
{
	m_pWndMain		= NULL;
	m_pWndCaption	= NULL;
	m_pWndBack		= NULL;
	m_pBtnExit		= NULL;
	m_pBtnCancel	= NULL;
	m_pBtnOk		= NULL;
	m_pDeStuffList	= NULL;
	m_pStcTitle		= NULL;
	m_pDeUnit		= NULL;
	m_pDeSucc		= NULL;
	m_pStcPotVal	= NULL;
	m_pStcGold		= NULL;
	m_pStcSilver	= NULL;
	for( int i=0; i<PointUpSubTypeQuan; i++ )
	{
		m_pPBtnType[i] = NULL;
	}
	m_eCurState = EDeCompose_NULL;
}

DeComposeFrame::~DeComposeFrame( void )
{

}

BOOL DeComposeFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	//--注册游戏事件
	m_pMgr->RegisterEventHandle(_T("ItemEvent_Move2Compose"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&DeComposeFrame::OnEventProduceAddItem));
	m_pMgr->RegisterEventHandle(_T("tagUpdateLPSkillEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&DeComposeFrame::OnUpdateLPSkillEvent));


	//--注册网络消息
	m_pCmdMgr->Register("NS_Decomposition", (NETMSGPROC)m_Trunk.sfp2(&DeComposeFrame::NetRecvNS_Decomposition), _T("NS_Decomposition"));

	return TRUE;
}

BOOL DeComposeFrame::Destroy()
{
	GameFrame::Destroy();
	if( P_VALID(m_pWndMain) )
		m_pGUI->AddToDestroyList(m_pWndMain);

	//--注册游戏事件
	m_pMgr->UnRegisterEventHandler(_T("ItemEvent_Move2Compose"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&DeComposeFrame::OnEventProduceAddItem));
	m_pMgr->UnRegisterEventHandler(_T("tagUpdateLPSkillEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&DeComposeFrame::OnUpdateLPSkillEvent));


	//--注销网络消息
	m_pCmdMgr->UnRegister("NS_Decomposition", (NETMSGPROC)m_Trunk.sfp2(&DeComposeFrame::NetRecvNS_Decomposition));

	return TRUE;
}

BOOL DeComposeFrame::ReloadUI()
{
	XmlElement element;
	tstring strPath = g_strLocalPath + _T("\\ui\\decomposition.xml");
	m_pGUI->LoadXml(&element, "VFS_System", strPath.c_str());
	m_pGUI->ChangeXml(&element, "decomposition\\decompositionbackpic\\decompositionback\\backpic\\itemback\\item", "ClassName", "ComposeStatic");
	m_pGUI->ChangeXml(&element, "decomposition\\decompositionbackpic\\decompositionback\\imback\\im", "ClassName", "ComposeStatic");	
	m_pWndMain = m_pGUI->CreateWnd(_T("\\desktop"), &element);

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWndMain->GetFullName().c_str(), m_Trunk.sfp1(&DeComposeFrame::EventHandler));

	m_pWndCaption	= m_pWndMain->GetChild( _T("decompositionbackpic") );
	m_pWndBack		= m_pWndCaption->GetChild( _T("decompositionback") );
	m_pBtnExit		= (GUIButton*)m_pWndCaption->GetChild( _T("new") );
	m_pBtnOk		= (GUIButton*)m_pWndCaption->GetChild( _T("startbutt") );
	m_pBtnOk->SetEnable(false);
	m_pBtnCancel	= (GUIButton*)m_pWndCaption->GetChild( _T("breakbutt") );
	m_pBtnCancel->SetEnable(false);
	m_pDeStuffList	= (GUIListBox*)m_pWndBack->GetChild( _T("listback\\itemlist") );
	m_pDeStuffList->SetColNum(2, 60);
	m_pStcTitle		= (GUIStatic*)m_pWndBack->GetChild( _T("backpic\\word1") );
	m_pDeUnit		= (ComposeStatic*)m_pWndBack->GetChild( _T("backpic\\itemback\\item") );
	m_pDeUnit->SetPic(szDecomposePic);
	m_pDeSucc		= (ComposeStatic*)m_pWndBack->GetChild( _T("imback\\im") );
	m_pDeSucc->SetPic(szDecomposePic);
	m_pStcPotVal	= (GUIStatic*)m_pWndBack->GetChild( _T("num1") );
	m_pStcGold		= (GUIStatic*)m_pWndBack->GetChild( _T("num2") );
	m_pStcSilver	= (GUIStatic*)m_pWndBack->GetChild( _T("num3") );
	m_pPBtnType[0]	= (GUIPushButton*)m_pWndCaption->GetChild( _T("decompositionfra\\page1") );
	m_pPBtnType[1]	= (GUIPushButton*)m_pWndCaption->GetChild( _T("decompositionfra\\page2") );
	m_pProTime		= (GUIProgress*)m_pWndBack->GetChild( _T("backpic\\timepic\\time") );

	m_pWndMain->SetInvisible(TRUE);

	return TRUE;
}

VOID DeComposeFrame::Update()
{
	GameFrame::Update();
	if( m_eComposeType == EStartCompose_NPC )
	{
		m_escapeTime += Kernel::Inst()->GetDeltaTimeDW();
		if( m_escapeTime > 1000 )//每隔1秒检测距离目标NPC的距离
		{
			m_escapeTime = 0;

			if (RoleMgr::Inst()->IsOutValidDist( m_uComposeID.npcID ) )
			{
				ClearUIandStuff();
				m_pMgr->AddToDestroyList(this);
				return;
			}
		}
	}

	switch(m_eCurState)
	{
	case EDeCompose_SendMsg:
		{
			float deltaTime =  Cool3D::Kernel::Inst()->GetDeltaTime();
			float fcur = m_pProTime->GetCurrentValue();
			float fMax = m_pProTime->GetMaxValue();
			if(fcur < fMax)
			{
				fcur += deltaTime;
				m_pProTime->SetValue(fcur, true);
				m_pProTime->SetRefresh();
			}
			//发送合成消息
			if(fcur >= fMax)
			{				
				SendDecomposeMsg();
			}
		}
		break;
	case EDeCompose_RecvMsg:
		{
			if(Kernel::Inst()->GetAccumTimeDW()- m_dwRecvMsgTime >= 1000)
			{
				ClearUIandStuff();
			}// end if
		}
		break;
	default:
		break;
	}

}

VOID DeComposeFrame::OnEvent( tagGameEvent* pEvent )
{

}

DWORD DeComposeFrame::EventHandler( tagGUIEvent* pEvent )
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;

	switch( pEvent->eEvent )
	{
	case EGUIE_Click:
		{
			if( pWnd == m_pBtnExit )				//退出
			{
				ClearUIandStuff();
				m_pFrameMgr->AddToDestroyList(this);
				SetStrategy(EIST_ItemPocket);
			}
			else if( pWnd == m_pBtnOk )
			{
				JudgeSendDecompose();
			}
			else if( pWnd==m_pBtnCancel && m_eCurState==EDeCompose_SendMsg )				//取消
			{
				ResetStartUIState();
			}
		}
		break;
	case EGUIE_PushButtonDown:
		{
			//如果是模拟发的Down事件,则不处理
			if(pEvent->dwParam1!=1)
				return false;

			if(m_eCurState!=EDeCompose_NULL)
				return FALSE;

			for(int i=0; i<PointUpSubTypeQuan; i++)
			{
				if( pWnd == m_pPBtnType[i] )
				{
					m_eComType = (EComposeType)(m_eProType*10+i);
					ClearUIandStuff();
				}
			}
		}
		break;
	case EOnEventSendDropGUI_Item:
		{
			if(m_eCurState!=EDeCompose_NULL)
				return FALSE;

			return HandleDragStuff(pEvent);
		}
		break;
	case EGUIE_MoveIntoCtrl:
		{
			if( pWnd == m_pDeUnit )  
			{
				tagMainStuff mainStuff;
				m_Container.GetMainStuff(mainStuff);
				Equipment* pEquip = (Equipment*)ItemMgr::Inst()->GetPocketItemByID(mainStuff.n64ID);
				if( P_VALID(pEquip) )
				{
					ShowTip(pWnd, ToolTipCreator::Inst()->GetItemTips(pEquip).c_str());
				}
			}
			else if( pWnd == m_pDeSucc )
			{
				tagIMStuff  im;
				m_Container.GetIMStuff(im);
				Item* pItem = (Item*)ItemMgr::Inst()->GetPocketItemByID(im.n64ID);
				if( P_VALID(pItem) )
				{
					ShowTip(pWnd, ToolTipCreator::Inst()->GetItemTips(pItem).c_str());
				}
			}
		}
		break;
	case EGUIE_MoveOutOfCtrl:
		{
			ShowTip(pWnd, NULL);
		}
		break;
	}

	return TRUE;
}

void DeComposeFrame::SetProduceMode( EProduceType eType )
{
	m_eProType = eType;
	m_eComType = (EComposeType)(eType*10);
	m_pPBtnType[0]->SetState(EGUIBS_PushDown, false);
	if( eType == ECOMT_EquipdDecompose  )
	{
		m_pStcTitle->SetText(g_StrTable[_T("Compose_EquipdDecomposeHint")]);
	}
	else if( eType == ECOMT_PointUp ) 
	{
		m_pStcTitle->SetText(g_StrTable[_T("Compose_PointUpHint")]);
	}
}

void DeComposeFrame::SetOpenUIMode( EStartCompose eType, ComposeID uID )
{
	m_eComposeType = eType;
	m_uComposeID = uID;
}

DWORD DeComposeFrame::OnEventProduceAddItem( tagItemPosChangeExEvent* pGameEvent )
{	
	if( pGameEvent->eTypeSrc != EICT_Bag )
		return 0;

	//正在分解中,错误提示
	if( m_eCurState != EDeCompose_NULL )
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Processing")]);
		return 0;
	}

	DWORD dwTypeID = pGameEvent->dwTypeIDSrc;
	if( m_eProType==EPCT_PointUp )
	{
		//判断是否为装备
		if(MIsEquipment(dwTypeID))
		{
			INT16 nPos = INT16(pGameEvent->n16IndexSrc);
			Equipment* pEquip = (Equipment*)ItemMgr::Inst()->GetPocketItem(nPos);
			if(!P_VALID(pEquip))
				return 0;


			if( pEquip->IsNotIdetified() )
			{
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Equip_NoIdetified")]);
				return 0;
			}

			if( m_eComType == ECOMT_PointUp ) 
			{
				if(  pEquip->GetItemLevel() < 30 )
				{
					CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Decompose_Error3")]);
					return 0;
				}
				else if( pEquip->GetItemQuality() < 1 )
				{
					CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Decompose_Error7")]);
					return 0;
				}
			}


			ClearUIandStuff();

			JudgeDecompositonFormula(dwTypeID, pEquip->GetItemId(), pEquip->GetEquipPos());
			m_pWndMain->SetRefresh(TRUE);
		}
		else   
		{
			//在背包中的位置
			INT16 nPos = INT16( pGameEvent->n16IndexSrc);
			Item* pItem = ItemMgr::Inst()->GetPocketItem(nPos);
			if(!P_VALID(pItem))
				return 0;

			const tagItemProto* pItemProto = pItem->GetProto();
			//特殊物品
			if( pItemProto->eSpecFunc==EISF_ComposeAdvance)
			{
				if( m_dwFormulaID==GT_INVALID || m_dwFormulaID==0 )
				{
					CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Decompose_Error4")]);
					return 0;
				}

				if(!JudgeSpecItem((ESpecFuncComposeAdvance)pItemProto->nSpecFuncVal1))
					return 0;

				tstring strPic = ItemMgr::Inst()->GetItemIconPath(dwTypeID);
				m_pDeSucc->SetPic(strPic);

				tagIMStuff imStuff;
				imStuff.n64ID = pItem->GetItemId();

				m_Container.Add(imStuff);		
			}
			// 保底符
			else if(pItemProto->eSpecFunc==EISF_ProtectSign)
			{
				if( m_dwFormulaID==GT_INVALID || m_dwFormulaID==0 )
				{
					CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Decompose_Error4")]);
					return 0;
				}

				tstring strPic = ItemMgr::Inst()->GetItemIconPath(dwTypeID);
				m_pDeSucc->SetPic(strPic);

				tagIMStuff imStuff;
				imStuff.n64ID = pItem->GetItemId();

				m_Container.Add(imStuff);		
			}
			else 
			{
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Decompose_Error1")]);
			}

			m_pWndMain->SetRefresh(TRUE);
		}	
	}
	return 0;
}

DWORD DeComposeFrame::HandleDragStuff( tagGUIEvent* pEvent )
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;

	if( m_eProType==EPCT_PointUp )
	{
		if( pWnd == m_pDeUnit )
		{
			//判断是否为装备
			DWORD dwTypeID = pEvent->dwParam3;
			if(!MIsEquipment(dwTypeID))
			{
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_NotEquip")]);
				return FALSE;
			}

			INT16 nPos = INT16(pEvent->dwParam2);
			Equipment* pEquip = (Equipment*)ItemMgr::Inst()->GetPocketItem(nPos);
			if(!P_VALID(pEquip))
				return FALSE;

			//未鉴定不能点化分解
			if( pEquip->IsNotIdetified() )
			{
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Equip_NoIdetified")]);
				return FALSE;
			}

			if( m_eComType == ECOMT_PointUp ) 
			{
				if(  pEquip->GetItemLevel() < 30 )
				{
					CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Decompose_Error3")]);
					return FALSE;
				}
				else if( pEquip->GetItemQuality() < 1 )
				{
					CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Decompose_Error7")]);
					return FALSE;
				}
			}			

			ClearUIandStuff();
			
			JudgeDecompositonFormula(dwTypeID, pEquip->GetItemId(), pEquip->GetEquipPos());
			m_pWndMain->SetRefresh(TRUE);
		}
		else if( pWnd == m_pDeSucc )
		{
			if( m_dwFormulaID==GT_INVALID || m_dwFormulaID==0 )
			{
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Decompose_Error4")]);
				return FALSE;
			}
			DWORD dwTypeID = pEvent->dwParam3;
			if(MIsEquipment(dwTypeID))
			{
				IMSG(_T("is not item, return!\n"));
				return FALSE;
			}

			INT16 nPos = INT16(pEvent->dwParam2);
			Item* pItem = ItemMgr::Inst()->GetPocketItem(nPos);
			if(!P_VALID(pItem))
				return FALSE;

			const tagItemProto* pItemProto = pItem->GetProto();
			//特殊物品
			if( pItemProto->eSpecFunc==EISF_ComposeAdvance)
			{
				if( m_dwFormulaID==GT_INVALID || m_dwFormulaID==0 )
				{
					CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Decompose_Error4")]);
					return FALSE;
				}

				if(!JudgeSpecItem((ESpecFuncComposeAdvance)pItemProto->nSpecFuncVal1))
					return FALSE;

				tstring strPic = ItemMgr::Inst()->GetItemIconPath(dwTypeID);
				m_pDeSucc->SetPic(strPic);

				tagIMStuff imStuff;
				imStuff.n64ID = pItem->GetItemId();

				m_Container.Add(imStuff);		
				m_pWndMain->SetRefresh(TRUE);
			}
			// 保底符
			else if(pItemProto->eSpecFunc==EISF_ProtectSign)
			{
				if( m_dwFormulaID==GT_INVALID || m_dwFormulaID==0 )
				{
					CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Decompose_Error4")]);
					return FALSE;
				}

				tstring strPic = ItemMgr::Inst()->GetItemIconPath(dwTypeID);
				m_pDeSucc->SetPic(strPic);

				tagIMStuff imStuff;
				imStuff.n64ID = pItem->GetItemId();

				m_Container.Add(imStuff);	
				m_pWndMain->SetRefresh(TRUE);
			}
			else 
			{
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Decompose_Error1")]);
			}
		}
	}
	return TRUE;
}

BOOL DeComposeFrame::JudgeSpecItem( ESpecFuncComposeAdvance eSpec )
{
	if(eSpec==ESFCA_AllProduce || eSpec==EISFC_All)
		return TRUE;

	if((INT)m_eProType == (INT)eSpec)
		return TRUE;

	return FALSE;
}

DWORD DeComposeFrame::NetRecvNS_Decomposition( tagNS_Decomposition* pMsg, LPVOID pPrama )
{
	switch(pMsg->dwErrorCode)
	{
	case E_Compose_Consolidate_Success:
		{	
			m_eCurState = EDeCompose_RecvMsg;
			m_dwRecvMsgTime = Kernel::Inst()->GetAccumTimeDW();
			CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Consolidate_Success")]);
			TObjRef<AudioSys>()->Play2DSound(_T("make_success"), SOUND_NORMAL);
		}
		break;
	case E_Compose_Consolidate_Perfect:
		{
			m_eCurState = EDeCompose_RecvMsg;
			m_dwRecvMsgTime = Kernel::Inst()->GetAccumTimeDW();
			CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Consolidate_Perfect")]);
			TObjRef<AudioSys>()->Play2DSound(_T("make_perfect"), SOUND_NORMAL);
		}
		break;
	case E_Compose_Consolidate_Lose:
		{
			m_eCurState = EDeCompose_RecvMsg;
			m_dwRecvMsgTime = Kernel::Inst()->GetAccumTimeDW();
			CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Consolidate_Lose")]);
			TObjRef<AudioSys>()->Play2DSound(_T("make_failure"), SOUND_NORMAL);
		}
		break;
	case E_Compose_NPC_Not_Exist:
		ResetStartUIState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_NPC_Not_Exist")]);
		break;
	case E_Compose_Formula_Not_Exist:
		ResetStartUIState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Formula_Not_Exist")]);
		break;
	case E_Compose_Type_Not_Exist:
		ResetStartUIState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Type_Not_Exist")]);
		break;
	case E_Compose_Stuff_Not_Enough:
		ResetStartUIState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Stuff_Not_Enough")]);
		break;
	case E_Compose_FormulaNotMatch:
		ResetStartUIState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_FormulaNotMatch")]);
		break;
	case E_Compose_Skill_Not_Exist:
		ResetStartUIState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Skill_Not_Exist")]);
		break;
	case E_Compose_NPCCanNotCompose:
		ResetStartUIState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_NPCCanNotCompose")]);
		break;
	case E_Compose_NotEnough_Money:
		ResetStartUIState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_NotEnough_Money")]);
		break;
	case E_Compose_Vitality_Inadequate:
		ResetStartUIState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Vitality_Inadequate")]);
		break;
	case E_Compose_Quality_Not_Match:
		ResetStartUIState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Quality_Not_Match")]);
		break;
	case E_Compose_Not_Fashion:
		ResetStartUIState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Not_Fashion")]);
		break;
	case E_Compose_Equip_Lock:
		ResetStartUIState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Equip_Lock")]);
		break;
	case E_Compose_Equip_Time_Limit:
		ResetStartUIState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Equip_Time_Limit")]);
		break;
	case E_Compose_Equip_Not_identify:
		ResetStartUIState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Equip_Not_identify")]);
		break;
	case E_Compose_Equip_Level_Inadequate:
		ResetStartUIState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Equip_Level_Inadequate")]);
		break;
	case E_Compose_Equip_Quality_Inadequate:
		ResetStartUIState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Equip_Quality_Inadequate")]);
		break;
	case E_Compose_Equip_Type_Inadequate:
		ResetStartUIState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Equip_Type_Inadequate")]);
		break;
	case E_Compose_Bag_Full:
		ResetStartUIState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("PickGroundItemErr1")]);
	default:
		break;
	}	
	m_pWndMain->SetRefresh(TRUE);
	return 0;
}

void DeComposeFrame::SendDecomposeMsg()
{
	TCHAR szMsg[1024] = {0};
	tagNC_Decomposition *pMsg = (tagNC_Decomposition*)szMsg;
	pMsg->dwID		= m_pUtil->Crc32("NC_Decomposition");
	pMsg->dwFormulaID = m_dwFormulaID;

	switch(m_eComposeType)
	{
	case EStartCompose_NPC:
		pMsg->dwNPCID = m_uComposeID.npcID;
		pMsg->n64ItemID = GT_INVALID;
		pMsg->dwSkillID = GT_INVALID;
		break;
	case EStartCompose_Skill:
		pMsg->dwNPCID = GT_INVALID;
		pMsg->n64ItemID = GT_INVALID;
		pMsg->dwSkillID = m_uComposeID.skillID;
		break;
	case EStartCompose_Item:
		pMsg->dwNPCID = GT_INVALID;
		pMsg->n64ItemID =m_uComposeID.itemID;
		pMsg->dwSkillID = GT_INVALID;
		break;
	}

	tagIMStuff imStuff;
	m_Container.GetIMStuff(imStuff);
	pMsg->n64IMID = imStuff.n64ID;

	tagMainStuff mainStuff;
	m_Container.GetMainStuff(mainStuff);
	pMsg->n64Item = mainStuff.n64ID;

	pMsg->dwSize = sizeof(tagNC_Decomposition);

	m_pSession->Send(pMsg);
	m_eCurState = EDeCompose_Waiting;
}

void DeComposeFrame::ResetStartUIState()
{
	m_eCurState = EDeCompose_NULL;
	m_pBtnOk->SetEnable(true);
	m_pBtnCancel->SetEnable(false);
	m_pProTime->SetValue(0, true);
	PushButtonState(true);
	m_pWndMain->SetRefresh(TRUE);
}

BOOL DeComposeFrame::EscCancel()
{
	if(m_pWndMain->IsInvisible())
		return FALSE;

	m_pFrameMgr->AddToDestroyList(this);
	SetStrategy(EIST_ItemPocket);
	return TRUE;
}

void DeComposeFrame::ClearUIandStuff()
{
	m_eCurState = EDeCompose_NULL;
	m_pDeStuffList->Clear();
	m_pDeUnit->SetPic(szDecomposePic);
	m_pDeSucc->SetPic(szDecomposePic);
	m_pStcPotVal->SetText(_T(""));
	m_pStcGold->SetText(_T(""));
	m_pStcSilver->SetText(_T(""));
	m_dwFormulaID = GT_INVALID;
	m_pBtnOk->SetEnable(false);	
	m_pBtnCancel->SetEnable(false);
	m_pProTime->SetValue(0, true);
	PushButtonState(true);
	m_pWndMain->SetRefresh(TRUE);
	m_Container.ClearAllStuff();
	if( m_eComType == ECOMT_EquipdDecompose  )
	{
		m_pStcTitle->SetText(g_StrTable[_T("Compose_EquipdDecomposeHint")]);
	}
	else if( m_eComType == ECOMT_PointUp ) 
	{
		m_pStcTitle->SetText(g_StrTable[_T("Compose_PointUpHint")]);
	}
}

void DeComposeFrame::JudgeDecompositonFormula(DWORD dwItemID, INT64 n64ID, EEquipPos ePos)
{
	/*tagMainStuff tagMainStuff;
	m_Container.GetMainStuff(tagMainStuff);*/

	DWORD dwSkillLvl = MTransSkillLevel(m_uComposeID.skillID);

	//
	Equipment* pEquip = (Equipment*)ItemMgr::Inst()->GetPocketItemByID(n64ID);
	if(!P_VALID(pEquip))
		return ;

	if( MIsRWeapon(pEquip->GetProto()) )
	{
		map<DWORD, tagDeComposeProto> decomposeMap;
		decomposeMap = ProduceData::Inst()->GetDeComposeMap();
		for(map<DWORD, tagDeComposeProto>::iterator it=decomposeMap.begin();
			it!=decomposeMap.end(); it++)
		{
			tagDeComposeProto decomposeProto = it->second;
			if(decomposeProto.eComType == m_eComType)
			{
				switch(m_eComposeType)
				{
				case EStartCompose_NPC:
					{
						if( (decomposeProto.eFormFrom==EFormulaFrom_NPC
							|| decomposeProto.eFormFrom==EFormulaFrom_SkillorNPC
							|| decomposeProto.eFormFrom==EFormulaFrom_NPCorItem)
							&& decomposeProto.eTypeEx==pEquip->GetItemTypeEx()
							&& decomposeProto.byQuality==pEquip->GetItemQuality()
							&& decomposeProto.byLevel == pEquip->GetItemLevel()/10 )
						{
							m_dwFormulaID = decomposeProto.dwID;
						}
					}
					break;
				case EStartCompose_Skill:
					{
						if( (decomposeProto.eFormFrom==EFormulaFrom_Skill
							|| decomposeProto.eFormFrom==EFormulaFrom_SkillorNPC
							|| decomposeProto.eFormFrom==EFormulaFrom_SkillorItem)
							&& decomposeProto.eTypeEx==pEquip->GetItemTypeEx()
							&& decomposeProto.byQuality==pEquip->GetItemQuality()
							&& decomposeProto.byLevel == pEquip->GetItemLevel()/10
							&& decomposeProto.dwFormLvl <= dwSkillLvl )
						{
							m_dwFormulaID = decomposeProto.dwID;
						}
					}
					break;
				case EStartCompose_Item:
					{
						if( (decomposeProto.eFormFrom==EFormulaFrom_Item
							|| decomposeProto.eFormFrom==EFormulaFrom_SkillorItem
							|| decomposeProto.eFormFrom==EFormulaFrom_NPCorItem)
							&& decomposeProto.eTypeEx==pEquip->GetItemTypeEx()
							&& decomposeProto.byQuality==pEquip->GetItemQuality()
							&& decomposeProto.byLevel == pEquip->GetItemLevel()/10 )
						{
							m_dwFormulaID = decomposeProto.dwID;
						}
					}
					break;
				}
			}
		}
	}
	else if( MIsClothArmor(pEquip->GetProto()) )
	{
		map<DWORD, tagDeComposeProto> decomposeMap;
		decomposeMap = ProduceData::Inst()->GetDeComposeMap();
		for(map<DWORD, tagDeComposeProto>::iterator it=decomposeMap.begin();
			it!=decomposeMap.end(); it++)
		{
			tagDeComposeProto decomposeProto = it->second;
			if(decomposeProto.eComType == m_eComType)
			{
				switch(m_eComposeType)
				{
				case EStartCompose_NPC:
					{
						if( (decomposeProto.eFormFrom==EFormulaFrom_NPC
							|| decomposeProto.eFormFrom==EFormulaFrom_SkillorNPC
							|| decomposeProto.eFormFrom==EFormulaFrom_NPCorItem)
							&& decomposeProto.byQuality==pEquip->GetItemQuality()
							&& decomposeProto.ePos==pEquip->GetEquipPos()
							&& decomposeProto.byLevel == pEquip->GetItemLevel()/10
							&& decomposeProto.eType == EIT_ClothArmor )
						{
							m_dwFormulaID = decomposeProto.dwID;
						}
					}
					break;
				case EStartCompose_Skill:
					{
						if( (decomposeProto.eFormFrom==EFormulaFrom_Skill
							|| decomposeProto.eFormFrom==EFormulaFrom_SkillorNPC
							|| decomposeProto.eFormFrom==EFormulaFrom_SkillorItem)
							&& decomposeProto.byQuality==pEquip->GetItemQuality()
							&& decomposeProto.ePos==pEquip->GetEquipPos()
							&& decomposeProto.byLevel == pEquip->GetItemLevel()/10
							&& decomposeProto.eType == EIT_ClothArmor
							&& decomposeProto.dwFormLvl <= dwSkillLvl )
						{
							m_dwFormulaID = decomposeProto.dwID;
						}
					}
					break;
				case EStartCompose_Item:
					{
						if( (decomposeProto.eFormFrom==EFormulaFrom_Item
							|| decomposeProto.eFormFrom==EFormulaFrom_SkillorItem
							|| decomposeProto.eFormFrom==EFormulaFrom_NPCorItem)
							&& decomposeProto.byQuality==pEquip->GetItemQuality()
							&& decomposeProto.ePos==pEquip->GetEquipPos()
							&& decomposeProto.byLevel == pEquip->GetItemLevel()/10
							&& decomposeProto.eType == EIT_ClothArmor )
						{
							m_dwFormulaID = decomposeProto.dwID;
						}
					}
					break;
				}
			}
		}
	}
	else if( MIsDecoration(pEquip->GetProto()) )
	{
		map<DWORD, tagDeComposeProto> decomposeMap;
		decomposeMap = ProduceData::Inst()->GetDeComposeMap();
		for(map<DWORD, tagDeComposeProto>::iterator it=decomposeMap.begin();
			it!=decomposeMap.end(); it++)
		{
			tagDeComposeProto decomposeProto = it->second;
			if(decomposeProto.eComType == m_eComType)
			{
				switch(m_eComposeType)
				{
				case EStartCompose_NPC:
					{
						if( (decomposeProto.eFormFrom==EFormulaFrom_NPC
							|| decomposeProto.eFormFrom==EFormulaFrom_SkillorNPC
							|| decomposeProto.eFormFrom==EFormulaFrom_NPCorItem)
							&& decomposeProto.byQuality==pEquip->GetItemQuality()
							&& decomposeProto.ePos==pEquip->GetEquipPos()
							&& decomposeProto.byLevel == pEquip->GetItemLevel()/10
							&& decomposeProto.eType == EIT_Decoration )
						{
							m_dwFormulaID = decomposeProto.dwID;
						}
					}
					break;
				case EStartCompose_Skill:
					{
						if( (decomposeProto.eFormFrom==EFormulaFrom_Skill
							|| decomposeProto.eFormFrom==EFormulaFrom_SkillorNPC
							|| decomposeProto.eFormFrom==EFormulaFrom_SkillorItem)
							&& decomposeProto.byQuality==pEquip->GetItemQuality()
							&& decomposeProto.ePos==pEquip->GetEquipPos()
							&& decomposeProto.byLevel == pEquip->GetItemLevel()/10
							&& decomposeProto.eType == EIT_Decoration
							&& decomposeProto.dwFormLvl <= dwSkillLvl )
						{
							m_dwFormulaID = decomposeProto.dwID;
						}
					}
					break;
				case EStartCompose_Item:
					{
						if( (decomposeProto.eFormFrom==EFormulaFrom_Item
							|| decomposeProto.eFormFrom==EFormulaFrom_SkillorItem
							|| decomposeProto.eFormFrom==EFormulaFrom_NPCorItem)
							&& decomposeProto.byQuality==pEquip->GetItemQuality()
							&& decomposeProto.ePos==pEquip->GetEquipPos()
							&& decomposeProto.byLevel == pEquip->GetItemLevel()/10
							&& decomposeProto.eType == EIT_Decoration )
						{
							m_dwFormulaID = decomposeProto.dwID;
						}
					}
					break;
				}
			}
		}
	}
	else if( MIsArmor(pEquip->GetProto()) )
	{
		map<DWORD, tagDeComposeProto> decomposeMap;
		decomposeMap = ProduceData::Inst()->GetDeComposeMap();
		for(map<DWORD, tagDeComposeProto>::iterator it=decomposeMap.begin();
			it!=decomposeMap.end(); it++)
		{
			tagDeComposeProto decomposeProto = it->second;
			if(decomposeProto.eComType == m_eComType)
			{
				switch(m_eComposeType)
				{
				case EStartCompose_NPC:
					{
						if( (decomposeProto.eFormFrom==EFormulaFrom_NPC
							|| decomposeProto.eFormFrom==EFormulaFrom_SkillorNPC
							|| decomposeProto.eFormFrom==EFormulaFrom_NPCorItem)
							&& decomposeProto.byQuality==pEquip->GetItemQuality()
							&& decomposeProto.ePos==pEquip->GetEquipPos()
							&& decomposeProto.byLevel == pEquip->GetItemLevel()/10
							&& decomposeProto.eType == EIT_Armor )
						{
							m_dwFormulaID = decomposeProto.dwID;
						}
					}
					break;
				case EStartCompose_Skill:
					{
						if( (decomposeProto.eFormFrom==EFormulaFrom_Skill
							|| decomposeProto.eFormFrom==EFormulaFrom_SkillorNPC
							|| decomposeProto.eFormFrom==EFormulaFrom_SkillorItem)
							&& decomposeProto.byQuality==pEquip->GetItemQuality()
							&& decomposeProto.ePos==pEquip->GetEquipPos()
							&& decomposeProto.byLevel == pEquip->GetItemLevel()/10
							&& decomposeProto.eType == EIT_Armor
							&& decomposeProto.dwFormLvl <= dwSkillLvl )
						{
							m_dwFormulaID = decomposeProto.dwID;
						}
					}
					break;
				case EStartCompose_Item:
					{
						if( (decomposeProto.eFormFrom==EFormulaFrom_Item
							|| decomposeProto.eFormFrom==EFormulaFrom_SkillorItem
							|| decomposeProto.eFormFrom==EFormulaFrom_NPCorItem)
							&& decomposeProto.byQuality==pEquip->GetItemQuality()
							&& decomposeProto.ePos==pEquip->GetEquipPos()
							&& decomposeProto.byLevel == pEquip->GetItemLevel()/10
							&& decomposeProto.eType == EIT_Armor )
						{
							m_dwFormulaID = decomposeProto.dwID;
						}
					}
					break;
				}
			}
		}
	}

	if( m_dwFormulaID!=GT_INVALID && m_dwFormulaID!=0 )
	{
		tstring strPic = ItemMgr::Inst()->GetItemIconPath(dwItemID);
		m_pDeUnit->SetPic(strPic);

		tagMainStuff stuff;
		stuff.n64ID = n64ID;
		stuff.euqipPos = ePos;
		m_Container.Add(stuff);

		const tagDeComposeProto* pDeComposeProto = NULL;
		pDeComposeProto = ProduceData::Inst()->FinDeComposeProto(m_dwFormulaID);
		if( !P_VALID(pDeComposeProto) )
			return;

		TCHAR szText[64];
		//--金钱消耗
		DWORD dwMoney = pDeComposeProto->dwMoneyConsume;
		DWORD dwGolden = dwMoney/10000;
		DWORD dwSiller = dwMoney - dwGolden;
		_stprintf( szText, _T("%u"), dwGolden );
		m_pStcGold->SetText(szText);
		_stprintf( szText, _T("%u"), dwSiller );
		m_pStcSilver->SetText(szText);

		//--活力值消耗
		_stprintf( szText, _T("%u"),  pDeComposeProto->dwVitalityConsume);
		m_pStcPotVal->SetText(szText);


		//--提示
		m_pDeStuffList->Clear();
		TCHAR szBuff[X_SHORT_NAME] = {0};
		for(int i=0; i<MAX_PRODUCE_STUFF_QUANTITY; i++)
		{
			DWORD dwTypeID = pDeComposeProto->OutputStuff[i].dwStuffTypeID;
			if(dwTypeID!=GT_INVALID)
			{
				const tagItemDisplayInfo* pDisplayInfo = ItemProtoData::Inst()->FindItemDisplay(dwTypeID);
				if( P_VALID(pDisplayInfo) )
				{
					_sntprintf( szBuff, X_SHORT_NAME, _T("%d%%"), pDeComposeProto->OutputStuff[i].nRate/100 );
					m_pDeStuffList->SetText(i, 0, szBuff);
					m_pDeStuffList->SetText(i, 1, pDisplayInfo->szName);
				}
			}
		}
		m_pBtnOk->SetEnable(true);
	}
	else
	{
		if( m_eComType == ECOMT_PointUp ) 
		{
			switch(m_eComposeType)
			{
			case EStartCompose_NPC:
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Decompose_Error6")]);
				break;
			case EStartCompose_Skill:
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Decompose_Error5")]);
				break;
			case EStartCompose_Item:
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Decompose_Error6")]);
				break;
			}
		}
		else if( m_eComType == ECOMT_EquipdDecompose )
		{
			switch(m_eComposeType)
			{
			case EStartCompose_NPC:
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Decompose_Error2")]);
				break;
			case EStartCompose_Skill:
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Decompose_Error5")]);
				break;
			case EStartCompose_Item:
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Decompose_Error2")]);
				break;
			}
		}
	}
}

void DeComposeFrame::JudgeSendDecompose()
{
	const tagDeComposeProto* pDeComposeProto = NULL;
	pDeComposeProto = ProduceData::Inst()->FinDeComposeProto(m_dwFormulaID);	
	if( !P_VALID(pDeComposeProto) )
		return;

	//判断钱
	INT64 n64Silver = CurrencyMgr::Inst()->GetBagSilver();
	if( n64Silver <  pDeComposeProto->dwMoneyConsume)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ConsolidateMoneyError")]);
		return;
	}

	m_pProTime->SetSpeed(100);
	m_pProTime->SetValue(0, true);
	m_pProTime->SetMaxValue((FLOAT)pDeComposeProto->dwProduceTime/1000);
	m_pProTime->SetRefresh();
	m_eCurState = EDeCompose_SendMsg;
	m_pBtnOk->SetEnable(false);
	m_pBtnCancel->SetEnable(true);
	PushButtonState(false);
	m_pWndMain->SetRefresh(TRUE);

	// 读条音效
	TObjRef<AudioSys>()->Play2DSound(_T("make_item"), SOUND_NORMAL);
}

void DeComposeFrame::PushButtonState( bool bEnable )
{
	for(int i=0; i<PointUpSubTypeQuan; i++)
	{
		m_pPBtnType[i]->SetEnable(bEnable);
	}
}

DWORD DeComposeFrame::OnUpdateLPSkillEvent( tagUpdateLPSkillEvent* pEvent )
{
	if(pEvent->eType == ESUT_Passive)
		return 0;

	if( m_eComposeType==EStartCompose_Skill
		&& m_eProType!= EPCT_NULL 
		&& m_eProType==EPCT_PointUp)
	{
		DWORD dwOrcID = MTransSkillID(m_uComposeID.skillID);

		if( dwOrcID == pEvent->dwID )
		{
			const tagSkillData* pSkill = SkillMgr::Inst()->GetSkillData(dwOrcID);
			if( P_VALID(pSkill) )
			{
				INT nLevel = pSkill->nLevel;
				DWORD dwNewSkillID = MTransSkillTypeID(pEvent->dwID, nLevel);
				if( dwNewSkillID != m_uComposeID.skillID)
				{
					//设置配方
					m_uComposeID.skillID = dwNewSkillID;
				}
			}
		}
	}
	return 0;
}