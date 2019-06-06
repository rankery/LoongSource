#include "stdafx.h"
#include ".\EquipConsolidateFrame.h"
#include "DragBtn.h"
#include "Item.h"
#include "PocketFrame.h"
#include "Container.h"
#include "ItemMgr.h"
#include "ConsolidateData.h"
#include "ItemProtoData.h"
#include "ToolTipCreator.h"
#include "ToolTipFrame.h"
#include "ComColidateEvent.h"
#include "CurrencyMgr.h"
#include "RoleMgr.h"
#include "AudioSys.h"
#include "ItemFrame.h"
#include "ComposeMgr.h"
#include "CombatSysUtil.h"
#include "LocalPlayer.h"
#include "..\WorldDefine\currency_define.h"


EquipConsolidateFrame::EquipConsolidateFrame():m_Trunk(this)
{
	m_pWndMain		= NULL;
	m_pWndCaption	= NULL;
	m_pWndName		= NULL;
	m_pBtnExit		= NULL;
	m_pBtnOk		= NULL;
	m_pBtnCancel	= NULL;
	m_pStcNotice	= NULL;
	m_pTreeList		= NULL;
	m_pStcListTitle	= NULL;
	m_pStcSuccTitle	= NULL;
	m_pStcSuccCtrl		= NULL;
	m_pStcSuccOdds	= NULL;
	m_pStcModi		= NULL;
	m_pStcModiOdds	= NULL;
	m_pStcNeedShow	= NULL;
	m_pStcGolden	= NULL;
	m_pStcSiller	= NULL;
	m_pProTime		= NULL;
	m_pEditComNum	= NULL;
	m_pStcSuccMaster= NULL;
	m_pStcPotValHint= NULL;
	m_pStcPotVal	= NULL;
	m_pStcSuccHint	= NULL;
	m_pStcLoseHint	= NULL;
	m_pStcPerfectHint= NULL;
	memset(m_pStcCtrl, 0x0, sizeof(m_pStcCtrl));
	memset(m_pPBtnFormular, 0x0, sizeof(m_pPBtnFormular));
	memset(m_pStcStuffHint, 0x0, sizeof(m_pStcStuffHint));
	memset(m_pStcStuffQuan, 0x0, sizeof(m_pStcStuffQuan));
	m_dwRecvMsgTime	= 0;
	m_dwFormulaID	= GT_INVALID;
	m_eConType		= ECliDateT_NULL;
	m_eCurState		= EEConsolidate_NULL;
	m_dwNPCID		= GT_INVALID;
	m_escapeTime	= GT_INVALID;
	m_dwErrorCode	= 0;
	m_nLoseTry		= 0;
	m_pStcShowAddedValue = NULL;
}

EquipConsolidateFrame::~EquipConsolidateFrame()
{
	
}

BOOL EquipConsolidateFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	//--注册游戏事件
	m_pMgr->RegisterEventHandle(_T("ItemEvent_Move2Compose"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&EquipConsolidateFrame::OnEventConsolidateAddItem));
	
	//--注册网络消息
	m_pCmdMgr->Register("NS_ConsolidatePosy", (NETMSGPROC)m_Trunk.sfp2(&EquipConsolidateFrame::NetRecvConsolidatePosy), 
						_T("NS_ConsolidatePosy"));
	m_pCmdMgr->Register("NS_ConsolidateEngrave", (NETMSGPROC)m_Trunk.sfp2(&EquipConsolidateFrame::NetRecvConsolidateEngrave), 
		_T("NS_ConsolidateEngrave"));
	m_pCmdMgr->Register("NS_ConsolidateQuench", (NETMSGPROC)m_Trunk.sfp2(&EquipConsolidateFrame::NetRecvConsolidateQuench), 
		_T("NS_ConsolidateQuench"));

	return TRUE;
}

BOOL EquipConsolidateFrame::Destroy()
{
	HandleItem(true);
	GameFrame::Destroy();
	if( P_VALID(m_pWndMain) )
		m_pGUI->AddToDestroyList(m_pWndMain);

	//--注销游戏事件
	m_pMgr->UnRegisterEventHandler(_T("ItemEvent_Move2Compose"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&EquipConsolidateFrame::OnEventConsolidateAddItem));

	//--注销网络消息
	m_pCmdMgr->UnRegister("NS_ConsolidatePosy", (NETMSGPROC)m_Trunk.sfp2(&EquipConsolidateFrame::NetRecvConsolidatePosy));
	m_pCmdMgr->UnRegister("NS_ConsolidateEngrave", (NETMSGPROC)m_Trunk.sfp2(&EquipConsolidateFrame::NetRecvConsolidateEngrave));
	m_pCmdMgr->UnRegister("NS_ConsolidateQuench", (NETMSGPROC)m_Trunk.sfp2(&EquipConsolidateFrame::NetRecvConsolidateQuench));

	return TRUE;
}

BOOL EquipConsolidateFrame::EscCancel()
{
	if( m_eCurState==EEConsolidate_SendMsg )
	{
		HandleItem(true);
		m_eCurState = EEConsolidate_NULL;
		m_pBtnOk->SetEnable(true);
		m_pBtnCancel->SetEnable(false);
		m_pBtnOk->SetRefresh();
		m_pProTime->SetValue(0, true);		
		return TRUE;
	}
	else
	{
		ClearStuff();
		DeleteFormula();
		m_pFrameMgr->AddToDestroyList(this);
		SetStrategy(EIST_ItemPocket);
		return TRUE;
	}
	return FALSE;
}

VOID EquipConsolidateFrame::OnEvent( tagGameEvent* pEvent )
{

}

BOOL EquipConsolidateFrame::ReloadUI()
{
	XmlElement element;
	tstring strPath = g_strLocalPath + _T("\\ui\\compose.xml");
	m_pGUI->LoadXml(&element, "VFS_System", strPath.c_str());
	for(int i=0; i<CTRL_QUANTITY; i++)
	{
		char temp[256] = {0};
		sprintf(temp, "composewin\\cb_pic\\composepic\\item%d", i);
		m_pGUI->ChangeXml(&element, temp, "ClassName", "ComposeStatic");
	}
	m_pGUI->ChangeXml(&element, "composewin\\cb_pic\\composepic\\im", "ClassName", "ComposeStatic");
	m_pGUI->ChangeXml(&element, "composewin\\cb_pic\\directionpic\\direction", "ClassName", "AppliedTree");
	m_pWndMain = m_pGUI->CreateWnd(_T("\\desktop"), &element);

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWndMain->GetFullName().c_str(), m_Trunk.sfp1(&EquipConsolidateFrame::EventHandler));

	m_pWndCaption	= m_pWndMain->GetChild( _T("cb_pic") );
	m_pWndName		= (GUIStatic*)m_pWndCaption->GetChild( _T("win_name") );
	m_pBtnExit		= (GUIButton*)m_pWndCaption->GetChild( _T("closebutt") );

	m_pStcClidateBack	= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic") );
	m_pBtnOk		= (GUIButton*)m_pWndCaption->GetChild( _T("composepic\\butt1") );
	m_pBtnCancel	= (GUIButton*)m_pWndCaption->GetChild( _T("composepic\\butt2") );
	m_pBtnCancel->SetEnable(false);
	m_pStcNotice	= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\notic") );

	m_pStcTreeBack	= (GUIPatch*)m_pWndCaption->GetChild( _T("directionpic") );
	m_pTreeList		= (AppliedTree*)m_pWndCaption->GetChild( _T("directionpic\\direction") );
	m_pStcListTitle = (GUIStatic*)m_pWndCaption->GetChild( _T("directionpic\\title") );

	m_pStcSuccTitle	= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\title") );
	m_pStcSuccCtrl	= (ComposeStatic*)m_pWndCaption->GetChild( _T("composepic\\im") );
	m_pStcSuccOdds	= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\odds1") );
	m_pStcSuccOdds->SetText(_T(""));
	m_pStcModi		= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\word2") );
	m_pStcModi->SetInvisible(TRUE);
	m_pStcModiOdds	= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\odds2") );
	m_pStcModiOdds->SetText(_T(""));

	m_pStcNeedShow	= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\show") );

	m_pEditComNum	= (GUIEditBox*)m_pWndCaption->GetChild( _T("composepic\\putinumpic\\putinum") );
	m_pEditComNum->SetText(_T("1"));
	m_pStcConNum	= (GUIEditBox*)m_pWndCaption->GetChild( _T("composepic\\word12") );
	m_pStcConNum->SetText(g_StrTable[_T("Consolidate_LoseNum")]);
	m_pStcMasterHint=(GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\word4") );
	m_pStcMasterHint->SetInvisible(TRUE);
	m_pStcSuccMaster= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\num1") );
	m_pStcSuccMaster->SetText(_T(""));
	m_pStcPotValHint= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\word5") );
	m_pStcPotValHint->SetText(g_StrTable[_T("ConsolidatePotValue")]);
	m_pStcPotVal	= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\num2") );
	m_pStcPotVal->SetText(_T(""));;

	m_pStcGolden	= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\money1") );
	m_pStcGolden->SetText(_T(""));
	m_pStcSiller	= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\money2") );
	m_pStcSiller->SetText(_T(""));

	m_pProTime		= (GUIProgress*)m_pWndCaption->GetChild( _T("composepic\\timepic\\time") );
	m_pStcMainStuffHint	= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\hide0") );

	m_pStcSuccHint	= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\pic4") );
	m_pStcSuccHint->SetInvisible(TRUE);
	m_pStcLoseHint	= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\pic3") );
	m_pStcLoseHint->SetInvisible(TRUE);
	m_pStcPerfectHint = (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\pic5") );
	m_pStcPerfectHint->SetInvisible(TRUE);

	m_pStcShowAddedValue = (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\word6") );
	m_pStcShowAddedValue->SetInvisible(TRUE);

	for(int i=0; i<CTRL_QUANTITY; i++)
	{
		tstringstream szName;
		szName << _T("composepic\\item") << i;
		m_pStcCtrl[i] = (ComposeStatic*)m_pWndCaption->GetChild(szName.str().c_str());	
		m_pStcCtrl[i]->SetText(_T(""));
		if(i>0)
			m_pStcCtrl[i]->SetInvisible(TRUE);
	}

	for(int i=0; i<FORMULA_QUANTITY; i++)
	{
		tstringstream szName;
		szName << _T("com_fra\\optbutt") << i;
		m_pPBtnFormular[i] = (GUIPushButton*)m_pWndCaption->GetChild(szName.str().c_str());
		m_pPBtnFormular[i]->SetInvisible(TRUE);
	}

	for(int i=0; i<STUFF_QUANTITY; i++)
	{
		tstringstream szName;
		szName << _T("composepic\\itemnum") << i+1;
		m_pStcStuffQuan[i] = (GUIStatic*)m_pWndCaption->GetChild(szName.str().c_str());
		m_pStcStuffQuan[i]->SetInvisible(TRUE);
		szName.str(_T(""));
		szName << _T("composepic\\hide") << i+1;
		m_pStcStuffHint[i] = (GUIStatic*)m_pWndCaption->GetChild(szName.str().c_str());
		m_pStcStuffHint[i]->SetInvisible(TRUE);
	}

	m_pWndMain->SetInvisible(TRUE);

	return TRUE;
}

VOID EquipConsolidateFrame::Update()
{

	GameFrame::Update();

	m_escapeTime += Kernel::Inst()->GetDeltaTimeDW();
	if( m_escapeTime > 1000 )//每隔1秒检测距离目标NPC的距离
	{
		m_escapeTime = 0;

		if (RoleMgr::Inst()->IsOutValidDist( m_dwNPCID ) )
		{
			HandleItem(true);
			ClearStuff();
			DeleteFormula();
			m_pMgr->AddToDestroyList(this);
			SetStrategy(EIST_ItemPocket);
			return;
		}
	}

	switch(m_eCurState)
	{
	case EEConsolidate_SendMsg:
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
			//发送强化消息
			if(fcur >= fMax)
			{
				SendConsolidateMsg();
			}
		}
		break;
	case EEConsolidate_RecvMsg:
		{
			if(Kernel::Inst()->GetAccumTimeDW()-m_dwRecvMsgTime >= 1000)
			{
				m_pStcSuccHint->SetInvisible(TRUE);
				m_pStcLoseHint->SetInvisible(TRUE);
				m_pStcPerfectHint->SetInvisible(TRUE);
				switch( m_eConType )
				{
				case ECliDateT_Posy:
				case ECliDateT_Engrave:
				case ECliDateT_Quench:
					if( m_dwErrorCode==E_Compose_Consolidate_Lose )
					{
						if( m_nLoseTry!=0 )
						{
							--m_nLoseTry;
							TCHAR szBuff[X_SHORT_NAME] = {0};
							_itot(m_nLoseTry, szBuff, 10);
							m_pEditComNum->SetText(szBuff);
							if(!JudgeFormulaForCliDate(m_eConType, m_dwFormulaID))
							{
								OperatedClear();
							}
						}
						else
						{
							OperatedClear();
						}
					}
					else
					{
						OperatedClear();
					}
					break;
				}
				
			}
		}
		break;
	}
}

void EquipConsolidateFrame::SendConsolidateMsg()
{
	m_pProTime->SetInvisible(TRUE);
	TCHAR szMsg[1024] = {0};
	switch(m_eConType)
	{
	case ECliDateT_Posy:
		{
			tagNC_ConsolidatePosy* pMsg = (tagNC_ConsolidatePosy*)szMsg;
			pMsg->dwID		= m_pUtil->Crc32("NC_ConsolidatePosy");
			pMsg->dwNPCID	= m_dwNPCID;
			pMsg->dwFormulaID = m_dwFormulaID;

			//--从容器中读取主料和辅料数据
			tagMainStuff mainStuff;
			m_Container.GetMainStuff(mainStuff);
			pMsg->n64ItemID	= mainStuff.n64ID;

			tagIMStuff imStuff;
			m_Container.GetIMStuff(imStuff);
			pMsg->n64IMID = imStuff.n64ID;

			vector<tagAssistStuff> assMap;
			m_Container.GetAssistStuffMap(assMap);
			int nStuffQuan = 0;
			for(vector<tagAssistStuff>::iterator it=assMap.begin(); 
				it!=assMap.end(); ++it)
			{
				if((it->stuffType!=EST_Null || it->dwTypeID!=0)
					&& it->nBagQuan>=it->nQuantity)
				{
					pMsg->n64StuffID[nStuffQuan]  = it->n64ID;
					nStuffQuan++;
				}
			}

			pMsg->dwSize = sizeof(tagNC_ConsolidatePosy) + sizeof(INT64)*(nStuffQuan-1);
			m_pSession->Send(pMsg);
			m_eCurState = EEConsolidate_Waiting;

		}
		break;
	case ECliDateT_Engrave:
		{
			tagNC_ConsolidateEngrave* pMsg = (tagNC_ConsolidateEngrave*)szMsg;
			pMsg->dwID		= m_pUtil->Crc32("NC_ConsolidateEngrave");
			pMsg->dwNPCID	= m_dwNPCID;
			pMsg->dwFormulaID = m_dwFormulaID;

			//--从容器中读取主料和辅料数据
			tagMainStuff mainStuff;
			m_Container.GetMainStuff(mainStuff);
			pMsg->n64ItemID	= mainStuff.n64ID;

			tagIMStuff imStuff;
			m_Container.GetIMStuff(imStuff);
			pMsg->n64IMID = imStuff.n64ID;

			vector<tagAssistStuff> assMap;
			m_Container.GetAssistStuffMap(assMap);
			int nStuffQuan = 0;
			for(vector<tagAssistStuff>::iterator it=assMap.begin(); 
				it!=assMap.end(); ++it)
			{
				if((it->stuffType!=EST_Null || it->dwTypeID!=0)
					&& it->nBagQuan>=it->nQuantity)
				{
					pMsg->n64StuffID[nStuffQuan]  = it->n64ID;
					nStuffQuan++;
				}
			}

			pMsg->dwSize = sizeof(tagNC_ConsolidateEngrave) + sizeof(INT64)*(nStuffQuan-1);
			m_pSession->Send(pMsg);
			m_eCurState = EEConsolidate_Waiting;
		}
		break;
	case ECliDateT_Quench:
		{
			tagNC_ConsolidateQuench* pMsg = (tagNC_ConsolidateQuench*)szMsg;
			pMsg->dwID		= m_pUtil->Crc32("NC_ConsolidateQuench");
			pMsg->dwNPCID	= m_dwNPCID;
			pMsg->dwFormulaID = m_dwFormulaID;

			//--从容器中读取主料和辅料数据
			tagMainStuff mainStuff;
			m_Container.GetMainStuff(mainStuff);
			pMsg->n64ItemID	= mainStuff.n64ID;

			tagIMStuff imStuff;
			m_Container.GetIMStuff(imStuff);
			pMsg->n64IMID = imStuff.n64ID;

			vector<tagAssistStuff> assMap;
			m_Container.GetAssistStuffMap(assMap);
			int nStuffQuan = 0;
			for(vector<tagAssistStuff>::iterator it=assMap.begin(); 
				it!=assMap.end(); ++it)
			{
				if((it->stuffType!=EST_Null || it->dwTypeID!=0)
					&& it->nBagQuan>=it->nQuantity)
				{
					pMsg->n64StuffID[nStuffQuan]  = it->n64ID;
					nStuffQuan++;
				}
			}

			pMsg->dwSize = sizeof(tagNC_ConsolidateQuench) + sizeof(INT64)*(nStuffQuan-1);
			m_pSession->Send(pMsg);
			m_eCurState = EEConsolidate_Waiting;
		}
		break;
	}
	
}

DWORD EquipConsolidateFrame::NetRecvConsolidatePosy(tagNS_ConsolidatePosy* pMsg, DWORD pPrama)
{
	if( pMsg->dwID != m_pUtil->Crc32("NS_ConsolidatePosy"))
		return 0;
	switch(pMsg->dwErrorCode)
	{
	case E_Compose_Consolidate_Success:
		{	
			m_pStcSuccHint->SetInvisible(FALSE);
			m_dwRecvMsgTime = Kernel::Inst()->GetAccumTimeDW();
			m_dwErrorCode = 0;

			TObjRef<AudioSys>()->Play2DSound(_T("make_success"), SOUND_NORMAL);
		}
		break;
	case E_Compose_Consolidate_Perfect:
		{
			m_pStcPerfectHint->SetInvisible(FALSE);
			m_dwRecvMsgTime = Kernel::Inst()->GetAccumTimeDW();
			m_dwErrorCode = 0;

			// 完美强化
			TObjRef<AudioSys>()->Play2DSound(_T("make_perfect"), SOUND_NORMAL);
		}
		break;
	case E_Compose_Consolidate_Lose:
		{
			m_pStcLoseHint->SetInvisible(FALSE);
			m_dwRecvMsgTime = Kernel::Inst()->GetAccumTimeDW();

			TObjRef<AudioSys>()->Play2DSound(_T("make_failure"), SOUND_NORMAL);
			m_dwErrorCode = E_Compose_Consolidate_Lose;
			if( !RefreshItemNum2UI() )
			{
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ConsolidateStuffError")]);
				m_dwErrorCode = 0;
			}
		}
		break;
	case E_Consolidate_Equip_Not_Exist:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_Equip_Not_Exist")]);
		break;
	case E_Consolidate_Type_Not_Exist:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_Type_Not_Exist")]);
		break;
	case E_Consolidate_NPCCanNotPosy:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_NPCCanNotPosy")]);
		break;
	case E_Consolidate_NotEquipment:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_NotEquipment")]);
		break;
	case E_Consolidate_BeyondPosyTime:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_BeyondPosyTime")]);
		break;
	case E_Consolidate_FormulaNotMatch:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_FormulaNotMatch")]);
		break; 
	case E_Consolidate_NotEnough_Money:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_NotEnough_Money")]);
		break;
	case E_Consolidate_NotEnough_Stuff:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_NotEnough_Stuff")]);
		break;
	case E_Consolidate_EquipCanNotPosy:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_EquipCanNotPosy")]);
		break;
	case E_Consolidate_ValConsume_Inadequate:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_ValConsume_Inadequate")]);
		break;
	default:
		break;
	}
	m_eCurState = EEConsolidate_RecvMsg;
	m_pWndMain->SetRefresh(TRUE);
	m_pTreeList->SetCurAppliedTreeState();
	return 0;
}

DWORD EquipConsolidateFrame::NetRecvConsolidateEngrave(tagNS_ConsolidateEngrave* pMsg, DWORD pPrama)
{
	switch(pMsg->dwErrorCode)
	{
	case E_Compose_Consolidate_Success:
		{	
			m_pStcSuccHint->SetInvisible(FALSE);
			m_dwRecvMsgTime = Kernel::Inst()->GetAccumTimeDW();
			m_dwErrorCode = 0;

			TObjRef<AudioSys>()->Play2DSound(_T("make_success"), SOUND_NORMAL);
		}
		break;
	case E_Compose_Consolidate_Perfect:
		{
			m_pStcPerfectHint->SetInvisible(FALSE);
			m_dwRecvMsgTime = Kernel::Inst()->GetAccumTimeDW();
			m_dwErrorCode = 0;

			// 完美强化
			TObjRef<AudioSys>()->Play2DSound(_T("make_perfect"), SOUND_NORMAL);
		}
		break;
	case E_Compose_Consolidate_Lose:
		{
			m_pStcLoseHint->SetInvisible(FALSE);
			m_dwRecvMsgTime = Kernel::Inst()->GetAccumTimeDW();

			TObjRef<AudioSys>()->Play2DSound(_T("make_failure"), SOUND_NORMAL);
			m_dwErrorCode = E_Compose_Consolidate_Lose;
			if( !RefreshItemNum2UI() )
			{
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ConsolidateStuffError")]);
				m_dwErrorCode = 0;
			}
		}
		break;
	case E_Consolidate_Equip_Not_Exist:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_Equip_Not_Exist")]);
		break;
	case E_Consolidate_Type_Not_Exist:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_Type_Not_Exist")]);
		break;
	case E_Consolidate_NPCCanNotPosy:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_NPCCanNotPosy")]);
		break;
	case E_Consolidate_NotEquipment:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_NotEquipment")]);
		break;
	case E_Consolidate_BeyondEngraveTime:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_BeyondPosyTime")]);
		break;
	case E_Consolidate_FormulaNotMatch:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_FormulaNotMatch")]);
		break; 
	case E_Consolidate_NotEnough_Money:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_NotEnough_Money")]);
		break;
	case E_Consolidate_NotEnough_Stuff:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_NotEnough_Stuff")]);
		break;
	case E_Consolidate_EquipCanNotPosy:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_EquipCanNotPosy")]);
		break;
	case E_Consolidate_ValConsume_Inadequate:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_ValConsume_Inadequate")]);
		break;
	default:
		break;
	}	
	m_eCurState = EEConsolidate_RecvMsg;
	m_pWndMain->SetRefresh(TRUE);
	m_pTreeList->SetCurAppliedTreeState();
	return 0;
}

DWORD EquipConsolidateFrame::NetRecvConsolidateQuench(tagNS_ConsolidateQuench* pMsg, DWORD pPrama)
{
	switch(pMsg->dwErrorCode)
	{
	case E_Compose_Consolidate_Success:
		{	
			m_pStcSuccHint->SetInvisible(FALSE);
			m_dwRecvMsgTime = Kernel::Inst()->GetAccumTimeDW();
			m_dwErrorCode = 0;

			TObjRef<AudioSys>()->Play2DSound(_T("make_success"), SOUND_NORMAL);
		}
		break;
	case E_Compose_Consolidate_Perfect:
		{
			m_pStcPerfectHint->SetInvisible(FALSE);
			m_dwRecvMsgTime = Kernel::Inst()->GetAccumTimeDW();
			m_dwErrorCode = 0;

			// 完美强化
			TObjRef<AudioSys>()->Play2DSound(_T("make_perfect"), SOUND_NORMAL);
		}
		break;
	case E_Compose_Consolidate_Lose:
		{
			m_pStcLoseHint->SetInvisible(FALSE);
			m_dwRecvMsgTime = Kernel::Inst()->GetAccumTimeDW();

			TObjRef<AudioSys>()->Play2DSound(_T("make_failure"), SOUND_NORMAL);
			m_dwErrorCode = E_Compose_Consolidate_Lose;
			if( !RefreshItemNum2UI() )
			{
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ConsolidateStuffError")]);
				m_dwErrorCode = 0;
			}
		}
		break;
	case E_Consolidate_Equip_Not_Exist:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_Equip_Not_Exist")]);
		break;
	case E_Consolidate_Type_Not_Exist:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_Type_Not_Exist")]);
		break;
	case E_Consolidate_NPCCanNotPosy:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_NPCCanNotPosy")]);
		break;
	case E_Consolidate_NotEquipment:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_NotEquipment")]);
		break;
	case E_Consolidate_BeyondPosyTime:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_BeyondPosyTime")]);
		break;
	case E_Consolidate_FormulaNotMatch:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_FormulaNotMatch")]);
		break; 
	case E_Consolidate_NotEnough_Money:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_NotEnough_Money")]);
		break;
	case E_Consolidate_NotEnough_Stuff:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_NotEnough_Stuff")]);
		break;
	case E_Consolidate_EquipCanNotPosy:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_EquipCanNotPosy")]);
		break;
	case E_Consolidate_ValConsume_Inadequate:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_ValConsume_Inadequate")]);
		break;
	default:
		break;
	}
	m_eCurState = EEConsolidate_RecvMsg;
	m_pWndMain->SetRefresh(TRUE);
	m_pTreeList->SetCurAppliedTreeState();

	return 0;
}

DWORD EquipConsolidateFrame::EventHandler(tagGUIEvent* pEvent)
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
				HandleItem(true);
				ClearStuff();
				DeleteFormula();
				m_pFrameMgr->AddToDestroyList(this);
				SetStrategy(EIST_ItemPocket);
			}
			if( pWnd == m_pBtnOk )					//开始
			{
				JudgeFormulaForCliDate(m_eConType, m_dwFormulaID);
				m_pStcShowAddedValue->SetInvisible(true);
			}
			
			if( pWnd==m_pBtnCancel && m_eCurState==EEConsolidate_SendMsg )				//取消
			{
				HandleItem(true);
				m_eCurState = EEConsolidate_NULL;
				m_pBtnOk->SetEnable(true);
				m_pBtnCancel->SetEnable(false);
				m_pBtnOk->SetRefresh();
				m_pProTime->SetValue(0, true);
				m_pTreeList->SetCurAppliedTreeState(AppliedTree::EAppliedTree_Operate);
			}

		}
		break;
	case EGUIE_ItemClick:
		{
			if( pWnd == m_pTreeList )
			{
				if(m_eCurState != EEConsolidate_NULL)
					return FALSE;

				TreeFormularMap::iterator iter = m_tree2formula.find(pEvent->dwParam1);
				if(iter != m_tree2formula.end())
				{
					m_dwFormulaID = iter->second;
					SetStuffOnFormula(m_eConType, m_dwFormulaID);
				}
				m_pStcShowAddedValue->SetInvisible(true);
			}
		}
		break;
	case EGUIE_Drag:
		{
			if (_tcsnccmp(pWnd->GetName().c_str(), _T("item"), 4) == 0)
			{
				for(int i=0; i<CTRL_QUANTITY; i++)
				{
					if( pWnd == m_pStcCtrl[i] )
					{
						DragBtn* pDrag = (DragBtn*)m_pGUI->GetWnd(_T("\\desktop\\DragButton"));
						if( P_VALID(pDrag) )
						{
							pDrag->OnEvent(pEvent);
							break;
						}
					}
				}
			}
			else if(_tcsnccmp(pWnd->GetName().c_str(), _T("im"), 2) == 0)
			{
				if( pWnd == m_pStcSuccCtrl )
				{
					DragBtn* pDrag = (DragBtn*)m_pGUI->GetWnd(_T("\\desktop\\DragButton"));
					if( P_VALID(pDrag) )
					{
						pDrag->OnEvent(pEvent);
						break;
					}
				}
			}
		}
		break;
	case EOnEventSendDropGUI_Item:
		{
			if(m_eCurState != EEConsolidate_NULL)
				return FALSE;

			return HandleDragStuff(pEvent);
		} 
		break;
	case EGUIE_MoveIntoCtrl:
		{
			if( pWnd == m_pStcCtrl[0] )
			{
				tagMainStuff mainStuff;
				m_Container.GetMainStuff(mainStuff);
				Equipment* pEquip = (Equipment*)ItemMgr::Inst()->GetPocketItemByID(mainStuff.n64ID);
				if( P_VALID(pEquip) )
				{
					ShowTip(pWnd, ToolTipCreator::Inst()->GetItemTips(pEquip).c_str());
				}

			}
			else if( pWnd == m_pStcSuccCtrl )
			{
				tagIMStuff  im;
				m_Container.GetIMStuff(im);
				Item* pItem = (Item*)ItemMgr::Inst()->GetPocketItemByID(im.n64ID);
				if( P_VALID(pItem) )
				{
					ShowTip(pWnd, ToolTipCreator::Inst()->GetItemTips(pItem).c_str());
				}
			}
			else
			{
				for(int i=0; i<STUFF_QUANTITY; i++)
				{
					int nPos = i+1;
					if( pWnd == m_pStcCtrl[nPos] )
					{
						tagAssistStuff* pAssStuff = m_Container.GetAssistStuff(i);
						if(!P_VALID(pAssStuff))
							break;
						Item* pItem = ItemMgr::Inst()->GetPocketItemByID(pAssStuff->n64ID);
						if( P_VALID(pItem) )
						{
							ShowTip(pWnd, ToolTipCreator::Inst()->GetItemTips(pItem).c_str());
						}
						break;
					}
				}
			}
		}
		break;
	case EGUIE_MoveOutOfCtrl:
		{
			ShowTip(pWnd, NULL);
		}
		break;
	default:
		break;
	}

	return FALSE;
}

void EquipConsolidateFrame::SetOpenUIMode(EStartCompose eType, ComposeID uID)
{ 
	m_dwNPCID = uID.npcID; 
}

BOOL EquipConsolidateFrame::JudgeSpecItem(ESpecFuncComposeAdvance eSpec)
{
	if(eSpec==EISFC_AllConsolidate || eSpec==EISFC_All)
		return TRUE;

	if( (INT)(m_eConType*10) == (INT)eSpec )
		return TRUE;

	return FALSE;
}

DWORD EquipConsolidateFrame::HandleDragStuff(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;

	// 接收拖动释放
	// 放入特殊物品以提高生成成功率
	if( pWnd == m_pStcSuccCtrl )
	{
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
		if( pItemProto->eSpecFunc==EISF_ComposeAdvance)
		{
			if(!JudgeSpecItem((ESpecFuncComposeAdvance)pItemProto->nSpecFuncVal1))
				return FALSE;
		}
		else if( pItemProto->eSpecFunc!=EISF_ProtectSign )
		{
			return FALSE;
		}
		
		tstring strPic = ItemMgr::Inst()->GetItemIconPath(dwTypeID);
		m_pStcSuccCtrl->SetPic(strPic);

		tagIMStuff imStuff;
		imStuff.n64ID = pItem->GetItemId();

		m_Container.Add(imStuff);		

		m_pWndMain->SetRefresh(TRUE);
	}
	// 主料
	else if( pWnd == m_pStcCtrl[0] )
	{								
		//判断是否为装备
		DWORD dwTypeID = pEvent->dwParam3;
		if(!MIsEquipment(dwTypeID))
		{
			IMSG(_T("is not equip, return!\n"));
			return FALSE;
		}

		INT16 nPos = INT16(pEvent->dwParam2);
		Equipment* pEquip = (Equipment*)ItemMgr::Inst()->GetPocketItem(nPos);
		if(!P_VALID(pEquip))
			return FALSE;

		tstring strPic = ItemMgr::Inst()->GetItemIconPath(dwTypeID);
		m_pStcCtrl[0]->SetPic(strPic);

		//放入主料时清空配方和辅料
		ClearStuffSelectFormula();
		DeleteFormula();
		m_dwFormulaID = GT_INVALID;

		//缓存主料(缺位置)
		tagMainStuff tagMainStuff;
		tagMainStuff.n64ID = pEquip->GetItemId();
		tagMainStuff.euqipPos = pEquip->GetEquipPos();
		m_Container.Add(tagMainStuff);

		//根据装备锻数设置配方
		switch(m_eConType)
		{
		case ECliDateT_Posy:
			{
				//装备当前的铭纹锻数
				BYTE byTimes = pEquip->GetEquipPosyTimes();
				SetFormula(byTimes);
			}
			break;
		case ECliDateT_Engrave:
			{
				//装备当前的镌刻锻数
				BYTE byTimes = pEquip->GetEquipEngraveTimes();
				SetFormula(byTimes);
			}
			break;
		case ECliDateT_Quench:
			{
				SetFormula(0);
			}
			break;
		}	

	}
	//辅料
	else
	{
		int i=0;
		for(; i<STUFF_QUANTITY; i++)
		{
			int nNum = i+1;
			if( pWnd == m_pStcCtrl[nNum] )
			{
				DWORD dwTypeID = pEvent->dwParam3;
				if(MIsEquipment(dwTypeID))
				{
					IMSG(_T("is not item, return!\n"));
					return FALSE;
				}

				//在背包中的位置
				INT16 nPos = INT16( pEvent->dwParam2);
				Item* pItem = ItemMgr::Inst()->GetPocketItem(nPos);
				if(!P_VALID(pItem))
					return FALSE;

				//同堆物品不能放到不同的位置
				vector<tagAssistStuff> assStuff;
				m_Container.GetAssistStuffMap(assStuff);
				for(int j=0; j<(int)assStuff.size(); j++)
				{
					if(i!=j && assStuff[j].n64ID==pItem->GetItemId())
						return FALSE;
				}


				//背包中的数量
				INT16 nItemCount = INT16( (pEvent->dwParam2 & 0xFFFF000) >> 16 );

				tstring strPic = ItemMgr::Inst()->GetItemIconPath(dwTypeID);
				

				//判断配方需求辅料的类型与拖动释放的物品类型是否相同
				tagAssistStuff* pAssStuff = m_Container.GetAssistStuff(i);
				if(pAssStuff->stuffType!=EST_Null && pAssStuff->stuffType!=GT_INVALID)
				{
					//材料类型相同
					if(pAssStuff->stuffType == pItem->GetItemStuffType())
					{
						pAssStuff->nBagQuan = nItemCount;
						pAssStuff->dwTypeID = pItem->GetItemTypeID();
						pAssStuff->n64ID = pItem->GetItemId();
						//设置控件数量
						TCHAR szText[64];
						_stprintf(szText, _T("%d/%d"), pAssStuff->nBagQuan, pAssStuff->nQuantity);
						m_pStcStuffQuan[i]->SetText(szText);

						if(pAssStuff->nBagQuan < pAssStuff->nQuantity)
							m_pStcStuffQuan[i]->SetTextColor(0xFFFF0000);
						else
							m_pStcStuffQuan[i]->SetTextColor(0xFF00FF00);

						m_pStcCtrl[nNum]->SetPic(strPic);
					}
					else 
						m_pStcNeedShow->SetText(g_StrTable[_T("ConsolidateStuffError")]);
				}
				else  if(pAssStuff->dwTypeID!=0 && pAssStuff->dwTypeID!=GT_INVALID)
				{
					//材料类型相同
					if(pAssStuff->dwTypeID == pItem->GetItemTypeID())
					{
						pAssStuff->nBagQuan = nItemCount;
						pAssStuff->stuffType = pItem->GetItemStuffType();
						pAssStuff->n64ID = pItem->GetItemId();
						//设置控件数量
						TCHAR szText[64];
						_stprintf(szText, _T("%d/%d"), pAssStuff->nBagQuan, pAssStuff->nQuantity);
						m_pStcStuffQuan[i]->SetText(szText);

						if(pAssStuff->nBagQuan < pAssStuff->nQuantity)
							m_pStcStuffQuan[i]->SetTextColor(0xFFFF0000);
						else
							m_pStcStuffQuan[i]->SetTextColor(0xFF00FF00);

						m_pStcCtrl[nNum]->SetPic(strPic);
					}
					else 
						m_pStcNeedShow->SetText(g_StrTable[_T("ConsolidateStuffError")]);
				}
				UtilCalculateSuccessRatio();
				m_pWndMain->SetRefresh(TRUE);
			} 
		}
	}  
	return TRUE;
}

void EquipConsolidateFrame::SetConsolidateMode(EConsolidateType eType)
{ 
	if( !P_VALID(m_pWndName) || !P_VALID(m_pStcMainStuffHint))
		return;
	m_eConType = eType;
	switch(m_eConType)
	{
	case ECliDateT_Posy:			//铭文
		{
			m_pWndName->SetText(g_StrTable[_T("ConsolidatePosy")]);
			m_pBtnOk->SetText(g_StrTable[_T("ConsolidatePosy")]);
			m_pStcMainStuffHint->SetText(g_StrTable[_T("ConsolidatePosyHint")]);
			tstringstream notice;
			notice << g_StrTable[_T("Consolidate_notice2")] << _T("\n");
			m_pStcNotice->SetText(notice.str().c_str());
			notice.clear();
		}
		break;
	case ECliDateT_Engrave:			//镌刻
		{
			m_pWndName->SetText(g_StrTable[_T("ConsolidateEngrave")]);
			m_pBtnOk->SetText(g_StrTable[_T("ConsolidateEngrave")]);
			m_pStcMainStuffHint->SetText(g_StrTable[_T("ConsolidateEngraveHint")]);
			tstringstream notice;
			notice << g_StrTable[_T("Consolidate_notice1")] << _T("\n");
			m_pStcNotice->SetText(notice.str().c_str());
			notice.clear();
		}
		break;
	case ECliDateT_Quench:
		{
			m_pWndName->SetText(g_StrTable[_T("ConsolidateQuench")]);
			m_pBtnOk->SetText(g_StrTable[_T("ConsolidateQuench")]);
			m_pStcMainStuffHint->SetText(g_StrTable[_T("ConsolidateQuenchHint")]);
		}
	default:
		IMSG(_T("SetTitle title type error! \n"));
		break;
	}
	ClearStuff();
	DeleteFormula();
}

void EquipConsolidateFrame::ClearStuff()
{

	for(int i=0; i<STUFF_QUANTITY; i++)
	{
		m_pStcStuffHint[i]->SetText(_T(""));
		m_pStcStuffHint[i]->SetInvisible(TRUE);
		m_pStcStuffQuan[i]->SetText(_T(""));
		m_pStcStuffQuan[i]->SetInvisible(TRUE);
		m_pStcCtrl[i+1]->SetInvisible(TRUE);
		m_pStcCtrl[i+1]->SetPic(_T("data\\ui\\main\\l_icon_back.bmp"));	
	}
	m_pStcCtrl[0]->SetPic(_T("data\\ui\\main\\l_icon_back.bmp"));
	m_pStcSuccOdds->SetText(_T(""));
	m_pStcNeedShow->SetText(_T(""));
	m_pStcPotVal->SetText(_T(""));
	m_pProTime->SetValue(0, true);
	m_Container.ClearAllStuff();
	m_eCurState = EEConsolidate_NULL;
	m_pStcSuccCtrl->SetPic(_T("data\\ui\\main\\l_icon_back.bmp"));
}

void EquipConsolidateFrame::ClearStuffSelectFormula()
{
	for(int i=0; i<STUFF_QUANTITY; i++)
	{
		m_pStcStuffHint[i]->SetText(_T(""));
		m_pStcStuffHint[i]->SetInvisible(TRUE);
		m_pStcStuffQuan[i]->SetText(_T(""));		
		m_pStcStuffQuan[i]->SetInvisible(TRUE);
		m_pStcCtrl[i+1]->SetInvisible(TRUE);
		m_pStcCtrl[i+1]->SetPic(_T("data\\ui\\main\\l_icon_back.bmp"));
	}
	m_pStcSuccOdds->SetText(_T(""));
	m_pStcNeedShow->SetText(_T(""));
	m_pStcPotVal->SetText(_T(""));
	m_pStcSuccMaster->SetText(_T(""));
	m_pProTime->SetValue(0, true);
	m_Container.ClearAssistStuff();
	m_eCurState = EEConsolidate_NULL;
	m_pStcSuccCtrl->SetPic(_T("data\\ui\\main\\l_icon_back.bmp"));
	m_pStcShowAddedValue->SetInvisible(true);
}

void EquipConsolidateFrame::DeleteFormula()
{
	TreeFormularMap::iterator iter;
	for( iter=m_tree2formula.begin(); iter != m_tree2formula.end();  )
	{
		m_pTreeList->RemoveItem( iter->first );
		iter = m_tree2formula.erase(iter);
	}
}


void EquipConsolidateFrame::SetFormula(BYTE byTimes)
{
	switch(m_eConType)
	{
	case ECliDateT_Posy:
		{
			map<DWORD, tagPosyProto> posyMap;
			posyMap = ConsolidateData::Inst()->GetPosyMap();

			for( map<DWORD, tagPosyProto>::iterator iter=posyMap.begin();
				iter!=posyMap.end(); iter++)
			{
				if(iter->second.byPosyTimes == byTimes+1)
				{
					bool bSet = false;
					for(int i=0; i<MAX_CONSOLIDATE_POS_QUANTITY; i++)
					{
						if(m_Container.GetMainStuffEquipPos()==iter->second.ConsolidatePos[i].ePos 
							&& iter->second.ConsolidatePos[i].byConsolidate==1)
						{
							bSet = true;
							break;
						}
					}
					if( bSet )
						AddFormula2Tree(ECliDateT_Posy, iter->second.dwID);
				}
			}
		}
		break;
	case ECliDateT_Engrave:
		{
			map<DWORD, tagEngraveProto> engraveMap;
			engraveMap = ConsolidateData::Inst()->GetEngraveMap();
			for(map<DWORD, tagEngraveProto>::iterator iter=engraveMap.begin();
				iter!=engraveMap.end(); iter++)
			{
				if(iter->second.byEngraveTimes == byTimes+1)
				{
					bool bSet = false;
					for(int i=0; i<MAX_CONSOLIDATE_POS_QUANTITY; i++)
					{
						if(m_Container.GetMainStuffEquipPos()==iter->second.ConsolidatePos[i].ePos 
							&& iter->second.ConsolidatePos[i].byConsolidate==1)
						{
							bSet = true;
							break;
						}
					}
					if( bSet )
						AddFormula2Tree(ECliDateT_Engrave, iter->second.dwID);
				}
			}
		}
		break;
	case ECliDateT_Quench:
		{
			map<DWORD, tagQuenchProto> quenchMap;
			quenchMap = ConsolidateData::Inst()->GetQuenchMap();
			for(map<DWORD, tagQuenchProto>::iterator iter=quenchMap.begin();
				iter!=quenchMap.end(); iter++)
			{
				AddFormula2Tree(ECliDateT_Quench, iter->second.dwID);
			}
		}
		break;
	default:
		IMSG( _T("SetFormula formula type error! \n") );
		break;
	}
}

void EquipConsolidateFrame::AddFormula2Tree(EConsolidateType eType, DWORD dwFormulaID)
{
	DWORD treeID = GT_INVALID;

	if( m_tree2formula.empty() )	
	{
		m_pTreeList->InsterItem(0, _T("Root"), _T("RootNode"));
	}
	switch(eType)
	{
	case ECliDateT_Posy:
		{
			const tagPosyProto* pPosy = NULL;
			pPosy = ConsolidateData::Inst()->FindPosyProto(dwFormulaID);
			if( !P_VALID(pPosy) )
				return;
			TCHAR szText[64];
			_stprintf(szText, _T("Posy_%d"), dwFormulaID);
			treeID = m_pTreeList->InsterItem(_T("\\Root"), szText, pPosy->szName);
		}
		break;
	case ECliDateT_Engrave:
		{
			const tagEngraveProto* pEngrave = NULL;
			pEngrave = ConsolidateData::Inst()->FindEngraveProto(dwFormulaID);
			if( !P_VALID(pEngrave) )
				return;
			TCHAR szText[64];
			_stprintf(szText, _T("Engrave_%d"), dwFormulaID);
			treeID = m_pTreeList->InsterItem(_T("\\Root"), szText, pEngrave->szName);
		}
		break;
	case ECliDateT_Quench:
		{
			const tagQuenchProto* pQuench = NULL;
			pQuench = ConsolidateData::Inst()->FindQuenchProto(dwFormulaID);
			if( !P_VALID(pQuench) )
				return;
			TCHAR szBuff[64];
			TCHAR szPath[64];
			TCHAR szText[64];
			_stprintf(szBuff, _T("QuenchType_%d"),pQuench->eQuenchType);
			m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff]);
			_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
			_stprintf(szText, _T("Quench_%d"), pQuench->dwID);
			treeID = m_pTreeList->InsterItem(szPath, szText, pQuench->szName);
		}
		break;
	default:
		IMSG(_T("AddFormula2Tree formula type error\n"));
		break;
	}
	m_tree2formula[treeID] = dwFormulaID;
}

void EquipConsolidateFrame::SetStuffOnFormula(EConsolidateType eType,  DWORD dwID)
{
	ClearStuffSelectFormula();
	switch(eType)
	{
	case ECliDateT_Posy:
		{
			const tagPosyProto* pPosy = NULL;
			pPosy = ConsolidateData::Inst()->FindPosyProto(dwID);	
			if( !P_VALID(pPosy) )
				return;

			bool bCanCliDate = false;
			for(int i=0; i<MAX_CONSOLIDATE_POS_QUANTITY; i++)
			{
				if(m_Container.GetMainStuffEquipPos()==pPosy->ConsolidatePos[i].ePos 
					&& pPosy->ConsolidatePos[i].byConsolidate==1)
				{
					bCanCliDate = true;
					break;
				}
			}
			//如果能强化才显示材料
			if(bCanCliDate)
			{
				for(int i=0; i<MAX_CONSOLIDATE_STUFF_QUANTITY; i++)
				{
					//有材料类型
					EStuffType eStuffType = pPosy->ConsolidateStuff[i].eStuffType;
					if(eStuffType!=EST_Null && eStuffType!=GT_INVALID)
					{
						TCHAR szText[64];
						//--材料提示
						_stprintf(szText, _T("%s%d"), _T("StuffType_"), eStuffType);
						m_pStcStuffHint[i]->SetText(g_StrTable[szText]);
						m_pStcStuffHint[i]->SetTextColor(0xFFFFFFFF);
						m_pStcStuffHint[i]->SetInvisible(FALSE);
						//--材料数量
						_stprintf(szText, _T("%d/%d"), 0, pPosy->ConsolidateStuff[i].dwStuffNum);
						m_pStcStuffQuan[i]->SetTextColor(0xFFFF0000);
						m_pStcStuffQuan[i]->SetText(szText);
						m_pStcStuffQuan[i]->SetInvisible(FALSE);
						//--材料控件
						m_pStcCtrl[i+1]->SetInvisible(FALSE);

						tagAssistStuff tagConso;
						tagConso.stuffType = eStuffType;
						tagConso.nQuantity = pPosy->ConsolidateStuff[i].dwStuffNum;
						//缓存强化辅料数据
						m_Container.Add(tagConso);
					}
					//有材料的TypeID
					else if(pPosy->ConsolidateStuff[i].dwStuffID!=0 
						&& pPosy->ConsolidateStuff[i].dwStuffID!=GT_INVALID)
					{
						TCHAR szText[64];
						//--材料提示(从背包中获取该TypeID材料的材料类型)
						tstring strName =  ItemProtoData::Inst()->FindItemDisplay(pPosy->ConsolidateStuff[i].dwStuffID)->szName;
						BYTE byQuality = ItemProtoData::Inst()->FindItemProto(pPosy->ConsolidateStuff[i].dwStuffID)->byQuality;
						DWORD dwColor = ComposeMgr::Inst()->GetColorByQuality(byQuality);
						m_pStcStuffHint[i]->SetTextColor(dwColor);;
						m_pStcStuffHint[i]->SetText(strName.c_str());
						m_pStcStuffHint[i]->SetInvisible(FALSE);
						//--材料数量
						_stprintf(szText, _T("%d/%d"), 0, pPosy->ConsolidateStuff[i].dwStuffNum);
						m_pStcStuffQuan[i]->SetTextColor(0xFFFF0000);
						m_pStcStuffQuan[i]->SetText(szText);
						m_pStcStuffQuan[i]->SetInvisible(FALSE);
						//--材料控件
						m_pStcCtrl[i+1]->SetInvisible(FALSE);

						tagAssistStuff tagConso;
						tagConso.dwTypeID = pPosy->ConsolidateStuff[i].dwStuffID;
						tagConso.nQuantity = pPosy->ConsolidateStuff[i].dwStuffNum;

						//缓存强化辅料数据
						m_Container.Add(tagConso);
					}
				}// end for

				//--成功率
				TCHAR szText[64];
				_stprintf( szText, _T("%d%%"),  pPosy->nSuccessRate/100);
				m_pStcSuccOdds->SetText(szText);

				//--金钱消耗
				if(pPosy->dwMoneyConsume==GT_INVALID ||
					pPosy->dwMoneyConsume <0)
				{
					_stprintf( szText, _T("%u"), 0 );
					m_pStcSiller->SetText(szText);
				}
				else
				{
					DWORD dwMoney = pPosy->dwMoneyConsume;
					DWORD dwGolden = MSilver2DBGold(dwMoney);
					DWORD dwSiller = MSilver2DBSilver(dwMoney);
					_stprintf( szText, _T("%u"), dwGolden );
					m_pStcGolden->SetText(szText);
					_stprintf( szText, _T("%u"), dwSiller );
					m_pStcSiller->SetText(szText);
				}

				//--潜力值消耗
				_stprintf( szText, _T("%u"),  pPosy->dwPotValConsume);
				m_pStcPotVal->SetText(szText);
			}// end if
		
			//--显示提示信息
			tstringstream strHint;
			strHint << g_StrTable[_T("ConsolidatePos")] << _T("\\n");
			for(int i=0; i<MAX_CONSOLIDATE_POS_QUANTITY; i++)
			{
				if( pPosy->ConsolidatePos[i].byConsolidate==1 )
				{
					TCHAR szBuff[64];
					_stprintf( szBuff, _T("%s%d"), _T("ConsolidatePos_"), pPosy->ConsolidatePos[i].ePos);
					strHint << g_StrTable[szBuff] << _T("  ");
				}
			}
			m_pStcNeedShow->SetText(strHint.str().c_str());
		}
		break;
	case ECliDateT_Engrave:
		{
			const tagEngraveProto* pEngrave = NULL;
			pEngrave = ConsolidateData::Inst()->FindEngraveProto(dwID);	
			if( !P_VALID(pEngrave) )
				return;

			bool bCanCliDate = false;
			for(int i=0; i<MAX_CONSOLIDATE_POS_QUANTITY; i++)
			{
				if(m_Container.GetMainStuffEquipPos()==pEngrave->ConsolidatePos[i].ePos 
					&& pEngrave->ConsolidatePos[i].byConsolidate==1)
				{
					bCanCliDate = true;
					break;
				}
			}
			//如果能强化才显示材料
			if(bCanCliDate)
			{
				for(int i=0; i<MAX_CONSOLIDATE_STUFF_QUANTITY; i++)
				{
					//有材料类型
					EStuffType eStuffType = pEngrave->ConsolidateStuff[i].eStuffType;
					if(eStuffType!=EST_Null && eStuffType!=GT_INVALID)
					{
						TCHAR szText[64];
						//--材料提示
						_stprintf(szText, _T("%s%d"), _T("StuffType_"), eStuffType);
						m_pStcStuffHint[i]->SetTextColor(0xFFFFFFFF);
						m_pStcStuffHint[i]->SetText(g_StrTable[szText]);
						m_pStcStuffHint[i]->SetInvisible(FALSE);
						//--材料数量
						_stprintf(szText, _T("%d/%d"), 0, pEngrave->ConsolidateStuff[i].dwStuffNum);
						m_pStcStuffQuan[i]->SetTextColor(0xFFFF0000);
						m_pStcStuffQuan[i]->SetText(szText);
						m_pStcStuffQuan[i]->SetInvisible(FALSE);
						//--材料控件
						m_pStcCtrl[i+1]->SetInvisible(FALSE);

						tagAssistStuff tagConso;
						tagConso.stuffType = eStuffType;
						tagConso.nQuantity = pEngrave->ConsolidateStuff[i].dwStuffNum;

						//缓存强化辅料数据
						m_Container.Add(tagConso);
					}
					//有材料的TypeID
					else if(pEngrave->ConsolidateStuff[i].dwStuffID!=0 
						&& pEngrave->ConsolidateStuff[i].dwStuffID!=GT_INVALID)
					{
						TCHAR szText[64];
						//--材料提示(从背包中获取该TypeID材料的材料类型)
						tstring strName =  ItemProtoData::Inst()->FindItemDisplay(pEngrave->ConsolidateStuff[i].dwStuffID)->szName;
						BYTE byQuality = ItemProtoData::Inst()->FindItemProto(pEngrave->ConsolidateStuff[i].dwStuffID)->byQuality;
						DWORD dwColor = ComposeMgr::Inst()->GetColorByQuality(byQuality);
						m_pStcStuffHint[i]->SetTextColor(dwColor);
						m_pStcStuffHint[i]->SetText(strName.c_str());
						m_pStcStuffHint[i]->SetInvisible(FALSE);
						//--材料数量
						_stprintf(szText, _T("%d/%d"), 0, pEngrave->ConsolidateStuff[i].dwStuffNum);
						m_pStcStuffQuan[i]->SetTextColor(0xFFFF0000);
						m_pStcStuffQuan[i]->SetText(szText);
						m_pStcStuffQuan[i]->SetInvisible(FALSE);
						//--材料控件
						m_pStcCtrl[i+1]->SetInvisible(FALSE);

						tagAssistStuff tagConso;
						tagConso.dwTypeID = pEngrave->ConsolidateStuff[i].dwStuffID;
						tagConso.nQuantity = pEngrave->ConsolidateStuff[i].dwStuffNum;

						//缓存强化辅料数据
						m_Container.Add(tagConso);
					}
				}// end for

				//--成功率
				TCHAR szText[64];
				_stprintf( szText, _T("%d%%"),  pEngrave->nSuccessRate/100);
				m_pStcSuccOdds->SetText(szText);

				//--金钱消耗
				if(pEngrave->dwMoneyConsume==GT_INVALID ||
					pEngrave->dwMoneyConsume <0)
				{
					_stprintf( szText, _T("%u"), 0 );
					m_pStcSiller->SetText(szText);
				}
				else 
				{
					DWORD dwMoney = pEngrave->dwMoneyConsume;
					DWORD dwGolden = MSilver2DBGold(dwMoney);
					DWORD dwSiller = MSilver2DBSilver(dwMoney);
					_stprintf( szText, _T("%u"), dwGolden );
					m_pStcGolden->SetText(szText);
					_stprintf( szText, _T("%u"), dwSiller );
					m_pStcSiller->SetText(szText);
				}

				//--潜力值消耗
				_stprintf( szText, _T("%u"),  pEngrave->dwPotValConsume);
				m_pStcPotVal->SetText(szText);
			}// end if

			//--显示提示信息
			tstringstream strHint;
			strHint << g_StrTable[_T("ConsolidatePos")] << _T("\\n");
			for(int i=0; i<MAX_CONSOLIDATE_POS_QUANTITY; i++)
			{
				if( pEngrave->ConsolidatePos[i].byConsolidate==1 )
				{
					TCHAR szBuff[64];
					_stprintf( szBuff, _T("%s%d"), _T("ConsolidatePos_"), pEngrave->ConsolidatePos[i].ePos);
					strHint << g_StrTable[szBuff] << _T("  ");
				}
			}
			m_pStcNeedShow->SetText(strHint.str().c_str());
		}
		break;
	case ECliDateT_Quench:
		{
			const tagQuenchProto* pQuench = NULL;
			pQuench = ConsolidateData::Inst()->FindQuenchProto(dwID);	
			if( !P_VALID(pQuench) )
				return;

			tagMainStuff value;
			m_Container.GetMainStuff(value);
			Equipment* pEquip = (Equipment*)ItemMgr::Inst()->GetPocketItemByID(value.n64ID);
			if(!P_VALID(pEquip))
				return;

			//判断五行属性是否满足配方要求
			bool bCanCliDate = false;
			if( pEquip->GetEquipWuxing(pQuench->srcQuenchAtt.eWuXing) >= pQuench->srcQuenchAtt.nWuXingValue)
				bCanCliDate = true;

			if(bCanCliDate)
			{
				for(int i=0; i<MAX_CONSOLIDATE_STUFF_QUANTITY; i++)
				{
					//有材料类型
					EStuffType eStuffType = pQuench->ConsolidateStuff[i].eStuffType;
					if(eStuffType!=EST_Null && eStuffType!=GT_INVALID)
					{
						TCHAR szText[64];
						//--材料提示
						_stprintf(szText, _T("%s%d"), _T("StuffType_"), eStuffType);
						m_pStcStuffHint[i]->SetTextColor(0xFFFFFFFF);
						m_pStcStuffHint[i]->SetText(g_StrTable[szText]);
						m_pStcStuffHint[i]->SetInvisible(FALSE);
						//--材料数量
						_stprintf(szText, _T("%d/%d"), 0, pQuench->ConsolidateStuff[i].dwStuffNum);
						m_pStcStuffQuan[i]->SetTextColor(0xFFFF0000);
						m_pStcStuffQuan[i]->SetText(szText);
						m_pStcStuffQuan[i]->SetInvisible(FALSE);
						//--材料控件
						m_pStcCtrl[i+1]->SetInvisible(FALSE);

						tagAssistStuff tagConso;
						tagConso.stuffType = eStuffType;
						tagConso.nQuantity = pQuench->ConsolidateStuff[i].dwStuffNum;

						//缓存强化辅料数据
						m_Container.Add(tagConso);
					}
					//有材料的TypeID
					else if(pQuench->ConsolidateStuff[i].dwStuffID!=0 
						&& pQuench->ConsolidateStuff[i].dwStuffID!=GT_INVALID)
					{
						TCHAR szText[64];
						//--材料提示(从背包中获取该TypeID材料的材料类型)
						tstring strName =  ItemProtoData::Inst()->FindItemDisplay(pQuench->ConsolidateStuff[i].dwStuffID)->szName;
						BYTE byQuality = ItemProtoData::Inst()->FindItemProto(pQuench->ConsolidateStuff[i].dwStuffID)->byQuality;
						DWORD dwColor = ComposeMgr::Inst()->GetColorByQuality(byQuality);
						m_pStcStuffHint[i]->SetTextColor(dwColor);
						m_pStcStuffHint[i]->SetText(strName.c_str());
						m_pStcStuffHint[i]->SetInvisible(FALSE);
						//--材料数量
						_stprintf(szText, _T("%d/%d"), 0, pQuench->ConsolidateStuff[i].dwStuffNum);
						m_pStcStuffQuan[i]->SetTextColor(0xFFFF0000);
						m_pStcStuffQuan[i]->SetText(szText);
						m_pStcStuffQuan[i]->SetInvisible(FALSE);
						//--材料控件
						m_pStcCtrl[i+1]->SetInvisible(FALSE);

						tagAssistStuff tagConso;
						tagConso.dwTypeID = pQuench->ConsolidateStuff[i].dwStuffID;
						tagConso.nQuantity = pQuench->ConsolidateStuff[i].dwStuffNum;

						//缓存强化辅料数据
						m_Container.Add(tagConso);
					}
				}// end for

				//--成功率
				TCHAR szText[64];
				_stprintf( szText, _T("%d%%"),  pQuench->nSuccessRate/100);
				m_pStcSuccOdds->SetText(szText);

				//--金钱消耗
				if(pQuench->dwMoneyConsume==GT_INVALID ||
					pQuench->dwMoneyConsume <0)
				{
					_stprintf( szText, _T("%u"), 0 );
					m_pStcSiller->SetText(szText);
				}
				else 
				{
					DWORD dwMoney = pQuench->dwMoneyConsume;
					DWORD dwGolden = MSilver2DBGold(dwMoney);
					DWORD dwSiller = MSilver2DBSilver(dwMoney);
					_stprintf( szText, _T("%u"), dwGolden );
					m_pStcGolden->SetText(szText);
					_stprintf( szText, _T("%u"), dwSiller );
					m_pStcSiller->SetText(szText);
				}

				//--潜力值消耗
				_stprintf( szText, _T("%u"),  pQuench->dwPotValConsume);
				m_pStcPotVal->SetText(szText);
			} // end if
			
			//--显示提示信息
			TCHAR srcBuff[32];
			TCHAR dstBuff[32];
			TCHAR szBuff[128];
			_stprintf(srcBuff, _T("WuXing_%d"), pQuench->srcQuenchAtt.eWuXing);
			_stprintf(dstBuff, _T("WuXing_%d"), pQuench->dstQuenchAtt.eWuXing);
			_stprintf(szBuff, g_StrTable[_T("QuenchHint")],pQuench->srcQuenchAtt.nWuXingValue, g_StrTable[srcBuff],
				pQuench->dstQuenchAtt.nWuXingValue, g_StrTable[dstBuff]);
			m_pStcNeedShow->SetText(szBuff);
	
		}
		break;
	default:
		IMSG( _T("SetStuffOnFormula formula type error! \n") );
		break;
	}	
}

bool EquipConsolidateFrame::JudgeFormulaForCliDate( EConsolidateType eType,  DWORD dwID )
{
	switch(eType)
	{
	case ECliDateT_Posy:
		{
			const tagPosyProto* pPosy = NULL;
			pPosy = ConsolidateData::Inst()->FindPosyProto(dwID);	
			//没有配方不能强化
			if( !P_VALID(pPosy) )
				return false;

			//判断钱
			INT64 n64Silver = CurrencyMgr::Inst()->GetBagSilver();
			if( n64Silver <  pPosy->dwMoneyConsume)
			{
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ConsolidateMoneyError")]);
				m_pWndMain->SetRefresh(TRUE);
				return false;

			}

			//判断装备潜力值是否满足配方需求(待定)
			tagMainStuff mainStuff;
			m_Container.GetMainStuff(mainStuff);
			Equipment* pEquip = (Equipment*)ItemMgr::Inst()->GetPocketItemByID(mainStuff.n64ID);
			if(!P_VALID(pEquip))
				return false;
			if(pEquip->GetEquipPotVal() < (INT32)pPosy->dwPotValConsume)
			{
				//潜力值不满足
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ConsolidatePotError")]);
				m_pWndMain->SetRefresh(TRUE);
				return false;

			}

			//判断辅料是否满足配方需求
			vector<tagAssistStuff> assStuff;
			m_Container.GetAssistStuffMap(assStuff);
			bool bSendMsg = true;
			if( (int)assStuff.size()==0 )
				bSendMsg = false;
			for(int i=0; i<(int)assStuff.size(); i++)
			{
				if(assStuff[i].nQuantity > assStuff[i].nBagQuan )
				{
					bSendMsg = false;
					break;
				}
			}

			if(bSendMsg)
			{
				HandleItem(false);
				m_pProTime->SetInvisible(FALSE);
				m_pProTime->SetSpeed(100);
				m_pProTime->SetValue(0, true);
				m_pProTime->SetMaxValue((FLOAT)pPosy->dwProduceTime/1000);
				m_pProTime->SetRefresh();
				m_eCurState = EEConsolidate_SendMsg;
				m_pBtnOk->SetEnable(false);
				m_pBtnCancel->SetEnable(true);
				m_pTreeList->SetCurAppliedTreeState(AppliedTree::EAppliedTree_NoOperate);
				m_pWndMain->SetRefresh(TRUE);
				m_nLoseTry = _ttoi(m_pEditComNum->GetText());
				// 强化读条
				TObjRef<AudioSys>()->Play2DSound(_T("make_item"), SOUND_NORMAL);
				return true;
			}
			else
			{
				//辅料不满足配方需求
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ConsolidateStuffError")]);
				m_pWndMain->SetRefresh(TRUE);
				return false;
			}
		}
		break;
	case ECliDateT_Engrave:
		{
			const tagEngraveProto* pEngrave = NULL;
			pEngrave = ConsolidateData::Inst()->FindEngraveProto(dwID);	
			//没有配方不能强化
			if( !P_VALID(pEngrave) )
				return false;

			//判断钱
			INT64 n64Silver = CurrencyMgr::Inst()->GetBagSilver();
			if( n64Silver <  pEngrave->dwMoneyConsume)
			{
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ConsolidateMoneyError")]);
				m_pWndMain->SetRefresh(TRUE);
				return false;
			}

			//判断装备潜力值是否满足配方需求(待定)
			tagMainStuff mainStuff;
			m_Container.GetMainStuff(mainStuff);
			Equipment* pEquip = (Equipment*)ItemMgr::Inst()->GetPocketItemByID(mainStuff.n64ID);
			if(!P_VALID(pEquip))
				return false;
			if(pEquip->GetEquipPotVal() < (INT32)pEngrave->dwPotValConsume)
			{
				//潜力值不满足
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ConsolidatePotError")]);
				m_pWndMain->SetRefresh(TRUE);
				return false;
			}

			//判断辅料是否满配方需求
			vector<tagAssistStuff> assStuff;
			m_Container.GetAssistStuffMap(assStuff);
			bool bSendMsg = true;
			if( (int)assStuff.size()==0 )
				bSendMsg = false;
			for(int i=0; i<(int)assStuff.size(); i++)
			{
				if(assStuff[i].nQuantity > assStuff[i].nBagQuan )
				{
					bSendMsg = false;
					break;
				}
			}

			if(bSendMsg)
			{
				HandleItem(false);
				m_pProTime->SetInvisible(FALSE);
				m_pProTime->SetSpeed(100);
				m_pProTime->SetValue(0, true);
				m_pProTime->SetMaxValue((FLOAT)pEngrave->dwProduceTime/1000);
				m_pProTime->SetRefresh();
				m_eCurState = EEConsolidate_SendMsg;
				m_pBtnOk->SetEnable(false);
				m_pBtnCancel->SetEnable(true);
				m_pTreeList->SetCurAppliedTreeState(AppliedTree::EAppliedTree_NoOperate);
				m_pWndMain->SetRefresh(TRUE);
				m_nLoseTry = _ttoi(m_pEditComNum->GetText());
				// 强化读条
				TObjRef<AudioSys>()->Play2DSound(_T("make_item"), SOUND_NORMAL);
				return true;
			}
			else
			{
				//辅料不满足配方需求
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ConsolidateStuffError")]);
				m_pWndMain->SetRefresh(TRUE);
				return false;
			}
		}
		break;
	case ECliDateT_Quench:
		{
			const tagQuenchProto* pQuench = NULL;
			pQuench = ConsolidateData::Inst()->FindQuenchProto(dwID);	
			//没有配方不能强化
			if( !P_VALID(pQuench) )
				return false;

			//判断钱
			INT64 n64Silver = CurrencyMgr::Inst()->GetBagSilver();
			if( n64Silver <  pQuench->dwMoneyConsume)
			{
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ConsolidateMoneyError")]);
				m_pWndMain->SetRefresh(TRUE);
				return false;
			}

			//判断装备潜力值是否满足配方需求(待定)
			tagMainStuff mainStuff;
			m_Container.GetMainStuff(mainStuff);
			Equipment* pEquip = (Equipment*)ItemMgr::Inst()->GetPocketItemByID(mainStuff.n64ID);
			if(!P_VALID(pEquip))
				return false;

			if(pEquip->GetEquipPotVal() < (INT32)pQuench->dwPotValConsume)
			{
				//潜力值不满足
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ConsolidatePotError")]);
				m_pWndMain->SetRefresh(TRUE);
				return false;
			}

			//判断辅料是否满配方需求
			vector<tagAssistStuff> assStuff;
			m_Container.GetAssistStuffMap(assStuff);
			bool bSendMsg = true;
			if( (int)assStuff.size()==0 )
				bSendMsg = false;
			for(int i=0; i<(int)assStuff.size(); i++)
			{
				if(assStuff[i].nQuantity > assStuff[i].nBagQuan )
				{
					bSendMsg = false;
					break;
				}
			}

			if(bSendMsg)
			{
				HandleItem(false);
				m_pProTime->SetInvisible(FALSE);
				m_pProTime->SetSpeed(100);
				m_pProTime->SetValue(0, true);
				m_pProTime->SetMaxValue((FLOAT)pQuench->dwProduceTime/1000);
				m_pProTime->SetRefresh();
				m_eCurState = EEConsolidate_SendMsg;
				m_pBtnOk->SetEnable(false);
				m_pBtnCancel->SetEnable(true);
				m_pTreeList->SetCurAppliedTreeState(AppliedTree::EAppliedTree_NoOperate);
				m_pWndMain->SetRefresh(TRUE);
				// 强化读条
				TObjRef<AudioSys>()->Play2DSound(_T("make_item"), SOUND_NORMAL);
				return true;
			}
			else
			{
				//辅料不满足配方需求
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ConsolidateStuffError")]);
				m_pWndMain->SetRefresh(TRUE);
				return false;
			}
		}
		break;
	default:
		IMSG( _T("JudgeFormulaForCliDate formula type error! \n") );
		break;
	}
	return false;
}

void EquipConsolidateFrame::ResetStuffFormular()
{
	ClearStuffSelectFormula();
	DeleteFormula();
	switch(m_eConType)
	{
	case ECliDateT_Posy:
		{
			tagMainStuff mainStuff;
			m_Container.GetMainStuff(mainStuff);
			INT64 n64ID = mainStuff.n64ID;
			//获取装备铭纹锻数
			Equipment* pEquip = (Equipment*)ItemMgr::Inst()->GetPocketItemByID(mainStuff.n64ID);
			if(!P_VALID(pEquip))
				return;

			BYTE byTimes = pEquip->GetEquipPosyTimes();
			SetFormula(byTimes);
		}
		break;
	case ECliDateT_Engrave:
		{
			tagMainStuff mainStuff;
			m_Container.GetMainStuff(mainStuff);
			INT64 n64ID = mainStuff.n64ID;
			//获取装备镌刻锻数
			//获取装备铭纹锻数
			Equipment* pEquip = (Equipment*)ItemMgr::Inst()->GetPocketItemByID(mainStuff.n64ID);
			if(!P_VALID(pEquip))
				return;

			BYTE byTimes = pEquip->GetEquipEngraveTimes();
			SetFormula(byTimes);
		}
		break;
	case ECliDateT_Quench:
		{
			tagMainStuff mainStuff;
			m_Container.GetMainStuff(mainStuff);
			INT64 n64ID = mainStuff.n64ID;

			Equipment* pEquip = (Equipment*)ItemMgr::Inst()->GetPocketItemByID(mainStuff.n64ID);
			if(!P_VALID(pEquip))
				return;

			SetFormula(0);
		}
		break;
	default:
		IMSG(_T("ResetStuffFormular formula type error!\n"));
		break;
	}
}

DWORD EquipConsolidateFrame::OnEventConsolidateAddItem( tagItemPosChangeExEvent* pGameEvent )
{
	if( pGameEvent->eTypeSrc != EICT_Bag )
		return 0;

	if( m_eCurState!=EEConsolidate_NULL )
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_Equip_Processing")]);
		m_pWndMain->SetRefresh(TRUE);
		return 0;
	}

	//判断是否为装备
	DWORD dwTypeID = pGameEvent->dwTypeIDSrc;
	if(MIsEquipment(dwTypeID))
	{
		INT16 nPos = INT16(pGameEvent->n16IndexSrc);
		Equipment* pEquip = (Equipment*)ItemMgr::Inst()->GetPocketItem(nPos);
		if(!P_VALID(pEquip))
			return 0;

		if( pEquip->IsNotIdetified() )
		{
			CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_Equip_NoIdetified")]);
			m_pWndMain->SetRefresh(TRUE);
			return 0;
		}

		tstring strPic = ItemMgr::Inst()->GetItemIconPath(dwTypeID);
		m_pStcCtrl[0]->SetPic(strPic);

		//放入主料时清空配方和辅料
		ClearStuffSelectFormula();
		DeleteFormula();
		m_dwFormulaID = GT_INVALID;

		//缓存主料(缺位置)
		tagMainStuff tagMainStuff;
		tagMainStuff.n64ID = pEquip->GetItemId();
		tagMainStuff.euqipPos = pEquip->GetEquipPos();
		m_Container.Add(tagMainStuff);

		//根据装备锻数设置配方
		switch(m_eConType)
		{
		case ECliDateT_Posy:
			{
				//装备当前的铭纹锻数
				BYTE byTimes = pEquip->GetEquipPosyTimes();
				SetFormula(byTimes);
			}
			break;
		case ECliDateT_Engrave:
			{
				//装备当前的镌刻锻数
				BYTE byTimes = pEquip->GetEquipEngraveTimes();
				SetFormula(byTimes);
			}
			break;
		case ECliDateT_Quench:
			{
				SetFormula(0);
			}
			break;
		}	
	}
	else 
	{
		if( m_dwFormulaID == GT_INVALID )
		{
			CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_Equip_First")]);
			m_pWndMain->SetRefresh(TRUE);
			return 0;
		}

		//在背包中的位置
		INT16 nPos = INT16( pGameEvent->n16IndexSrc);
		Item* pItem = ItemMgr::Inst()->GetPocketItem(nPos);
		if(!P_VALID(pItem))
			return 0;

		const tagItemProto* pItemProto = pItem->GetProto();
		//特殊物品
		if( pItemProto->eSpecFunc==EISF_ComposeAdvance)
		{
			if(!JudgeSpecItem((ESpecFuncComposeAdvance)pItemProto->nSpecFuncVal1))
				return 0;

			tstring strPic = ItemMgr::Inst()->GetItemIconPath(dwTypeID);
			m_pStcSuccCtrl->SetPic(strPic);

			tagIMStuff imStuff;
			imStuff.n64ID = pItem->GetItemId();

			m_Container.Add(imStuff);		

			m_pWndMain->SetRefresh(TRUE);
		}
		// 保底符
		else if(pItemProto->eSpecFunc==EISF_ProtectSign)
		{
			tstring strPic = ItemMgr::Inst()->GetItemIconPath(dwTypeID);
			m_pStcSuccCtrl->SetPic(strPic);

			tagIMStuff imStuff;
			imStuff.n64ID = pItem->GetItemId();

			m_Container.Add(imStuff);		

			m_pWndMain->SetRefresh(TRUE);
		}
		//普通材料
		else
		{
			int i=0;
			for(; i<m_Container.GetAssStuffSize(); i++)
			{
				int nNum = i+1;
				//背包中的数量
				INT16 nItemCount = pItem->GetItemQuantity();
				tstring strPic = ItemMgr::Inst()->GetItemIconPath(dwTypeID);


				//判断配方需求辅料的类型与拖动释放的物品类型是否相同
				tagAssistStuff* pAssStuff = m_Container.GetAssistStuff(i);
				if( !P_VALID(pAssStuff) )
				{
					CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Consolidate_Equip_First")]);
					m_pWndMain->SetRefresh(TRUE);
					return 0;
				}
				if(pAssStuff->stuffType!=EST_Null && pAssStuff->stuffType!=GT_INVALID)
				{
					//材料类型相同
					if(pAssStuff->stuffType == pItem->GetItemStuffType())
					{
						pAssStuff->nBagQuan = nItemCount;
						pAssStuff->dwTypeID = pItem->GetItemTypeID();
						pAssStuff->n64ID = pItem->GetItemId();
						//设置控件数量
						TCHAR szText[64];
						_stprintf(szText, _T("%d/%d"), pAssStuff->nBagQuan, pAssStuff->nQuantity);
						m_pStcStuffQuan[i]->SetText(szText);

						if(pAssStuff->nBagQuan < pAssStuff->nQuantity)
							m_pStcStuffQuan[i]->SetTextColor(0xFFFF0000);
						else
							m_pStcStuffQuan[i]->SetTextColor(0xFF00FF00);

						m_pStcCtrl[nNum]->SetPic(strPic);
						m_pWndMain->SetRefresh(TRUE);
						UtilCalculateSuccessRatio();
						return 0;
					}
				}
				else  if(pAssStuff->dwTypeID!=0 && pAssStuff->dwTypeID!=GT_INVALID)
				{
					//材料类型相同
					if(pAssStuff->dwTypeID == pItem->GetItemTypeID())
					{
						pAssStuff->nBagQuan = nItemCount;
						pAssStuff->stuffType = pItem->GetItemStuffType();
						pAssStuff->n64ID = pItem->GetItemId();
						//设置控件数量
						TCHAR szText[64];
						_stprintf(szText, _T("%d/%d"), pAssStuff->nBagQuan, pAssStuff->nQuantity);
						m_pStcStuffQuan[i]->SetText(szText);

						if(pAssStuff->nBagQuan < pAssStuff->nQuantity)
							m_pStcStuffQuan[i]->SetTextColor(0xFFFF0000);
						else
							m_pStcStuffQuan[i]->SetTextColor(0xFF00FF00);

						m_pStcCtrl[nNum]->SetPic(strPic);
						m_pWndMain->SetRefresh(TRUE);
						UtilCalculateSuccessRatio();
						return 0;
					}
				}	
			}	
			CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ConsolidateStuffError")]);
			m_pWndMain->SetRefresh(TRUE);
		}
	}
	return 0;
}

void EquipConsolidateFrame::OperatedClear()
{
	HandleItem(true);
	m_eCurState = EEConsolidate_NULL;
	m_pBtnOk->SetEnable(true);
	m_pBtnCancel->SetEnable(false);
	ResetStuffFormular();
	m_dwFormulaID = GT_INVALID;
	m_pWndMain->SetRefresh(TRUE);
	m_pEditComNum->SetText(_T("1"));
	m_dwErrorCode = 0; 
}

bool EquipConsolidateFrame::RefreshItemNum2UI()
{
	TCHAR szText[X_SHORT_NAME] = {0};
	vector<tagAssistStuff> stuff;
	m_Container.GetAssistStuffMap(stuff);
	int size = (int)stuff.size();
	bool bRet = true;
	for( int i=0; i<size; ++i )
	{
		Item* pItem = ItemMgr::Inst()->GetPocketItemByID(stuff[i].n64ID);
		if( P_VALID(pItem) )
		{
			stuff[i].nBagQuan = pItem->GetItemQuantity();
		}
		else
		{
			stuff[i].nBagQuan = 0;
		}
		if( stuff[i].nBagQuan < stuff[i].nQuantity)
		{
			_stprintf(szText, _T("%d/%d"), stuff[i].nBagQuan, stuff[i].nQuantity);
			m_pStcStuffQuan[i]->SetTextColor(0xFFFF0000);
			m_pStcStuffQuan[i]->SetText(szText);
			m_pStcStuffQuan[i]->SetInvisible(FALSE);
			bRet = false;
		}
		else
		{
			_stprintf(szText, _T("%d/%d"), stuff[i].nBagQuan, stuff[i].nQuantity);
			m_pStcStuffQuan[i]->SetText(szText);
			m_pStcStuffQuan[i]->SetInvisible(FALSE);
		}
	}
	m_pWndMain->SetRefresh(TRUE);

	return bRet;
}

void EquipConsolidateFrame::UtilCalculateSuccessRatio()
{
	switch( m_eConType )
	{
	case ECliDateT_Posy:
	
		{
			// 最终成功率=基础成功率+（-10%×白品材料数量/总材料数量+0%×黄品材料数量/总材料数量+5%×绿品材料数量
			// /总材料数量+10%×蓝品材料数量/总材料数量+20%×橙品材料数量/总材料数量）×[1+（装备等级-75）/150]
			// +（角色福缘/1000）+IM道具加成                                      最终成功率的取值为0%-100%

			const tagPosyProto* pPosy = NULL;
			pPosy = ConsolidateData::Inst()->FindPosyProto(m_dwFormulaID);	
			if( !P_VALID(pPosy) )
				return;

			float fStuffEffect = SetSuccessRatioAndGetStuffEffect( pPosy->nSuccessRate );
			ShowNewPosyAtt( pPosy, fStuffEffect );
		}
		break;
	case ECliDateT_Engrave:
		{

			const tagEngraveProto* pEngrave = NULL;
			pEngrave = ConsolidateData::Inst()->FindEngraveProto(m_dwFormulaID);	
			if( !P_VALID(pEngrave) )
				return;

			float fStuffEffect = SetSuccessRatioAndGetStuffEffect( pEngrave->nSuccessRate );
			ShowNewEngraveAtt( pEngrave, fStuffEffect );
		}
		break;
	case ECliDateT_Quench:
		{
			const tagQuenchProto* pQuench = NULL;
			pQuench = ConsolidateData::Inst()->FindQuenchProto(m_dwFormulaID);	
			if( !P_VALID(pQuench) )
				return;
			
			SetSuccessRatioAndGetStuffEffect( pQuench->nSuccessRate );
		}
		break;
	}
}

float EquipConsolidateFrame::SetSuccessRatioAndGetStuffEffect( DWORD dwSrcSuccess )
{
	int nStuffNum = 0;
	vector<tagAssistStuff> stuff;
	m_Container.GetAssistStuffMap(stuff);
	int size = (int)stuff.size();
	int nStuffCount = 0;
	for( int i=0; i<size; ++i )
	{
		if( (stuff[i].nBagQuan!=0 && stuff[i].nBagQuan!=GT_INVALID)
			&& stuff[i].nBagQuan >= stuff[i].nQuantity )
		{
			++nStuffNum;
			if( EST_Null != stuff[i].stuffType )
				nStuffCount += stuff[i].nQuantity;
		}
	}

	//所有材料都放了
	if( nStuffNum == size )
	{
		tagMainStuff main;
		m_Container.GetMainStuff(main);
		Equipment* pEquip = (Equipment*)ItemMgr::Inst()->GetPocketItemByID(main.n64ID);
		if(!P_VALID(pEquip))
			return GT_INVALID;

		LocalPlayer *pPlayer = RoleMgr::Inst()->GetLocalPlayer();
		if( !P_VALID(pPlayer) )
			return GT_INVALID;

		INT nWhiteItem = 0;
		INT nYellowItem = 0;
		INT nGreenitem = 0;
		INT nBlueItem = 0;
		INT nOrangeItem = 0;
		INT nPurpleItem = 0;
		for( int i=0; i<size; ++i )
		{
			if( EST_Null != stuff[i].stuffType )
			{
				BYTE byQuality = ItemProtoData::Inst()->FindItemProto(stuff[i].dwTypeID)->byQuality;
				switch( byQuality )
				{
				case 0:
					nWhiteItem += stuff[i].nQuantity;
					break;
				case 1:
					nYellowItem += stuff[i].nQuantity;
					break;
				case 2:
					nGreenitem += stuff[i].nQuantity;
					break;
				case 3:
					nBlueItem += stuff[i].nQuantity;
					break;
				case 4:
					nOrangeItem += stuff[i].nQuantity;
					break;
				case 5:
					nPurpleItem += stuff[i].nQuantity;
					break;
				default:
					nWhiteItem += stuff[i].nQuantity;
					break;
				}
			}
		}

		INT nEquipLvl = pEquip->GetItemLevel();

		INT nFortune = pPlayer->GetAttribute(ERA_Fortune);

		FLOAT fSuccRatio = dwSrcSuccess + (( -0.1f*nWhiteItem/nStuffCount + 0.05f*nGreenitem/nStuffCount + 
			0.1f*nBlueItem/nStuffCount + 0.2f*nOrangeItem/nStuffCount + 
			0.5f*nPurpleItem/nStuffCount)*(1+(nEquipLvl-75)/150.0f) + (nFortune/1000.0f)) * 10000;
		INT nSuccRatio = (INT)fSuccRatio/100;
		if( nSuccRatio>100 )
			nSuccRatio = 100;
		else if( nSuccRatio<0 )
			nSuccRatio = 0;

		TCHAR szText[64];
		_stprintf( szText, _T("%d%%"),  nSuccRatio);
		m_pStcSuccOdds->SetText(szText);

		FLOAT fStuffEffect = -0.3f*nWhiteItem/nStuffCount + 
			0.5f*nGreenitem/nStuffCount + 1.0f*nBlueItem/nStuffCount + 
			2.0f*nOrangeItem/nStuffCount + 3.0f*nPurpleItem/nStuffCount;
		return fStuffEffect;
	}
	return GT_INVALID;
}

void EquipConsolidateFrame::ShowNewEngraveAtt( const tagEngraveProto *pEngrave, float fStuffEffect )
{
	int nStuffNum = 0;
	vector<tagAssistStuff> stuff;
	m_Container.GetAssistStuffMap(stuff);
	int size = (int)stuff.size();
	int nStuffCount = 0;
	for( int i=0; i<size; ++i )
	{
		if( (stuff[i].nBagQuan!=0 && stuff[i].nBagQuan!=GT_INVALID)
			&& stuff[i].nBagQuan >= stuff[i].nQuantity )
		{
			++nStuffNum;
			nStuffCount += stuff[i].nQuantity;
		}
	}

	//所有材料都放了
	if( nStuffNum == size )
	{
		tagMainStuff main;
		m_Container.GetMainStuff(main);
		Equipment* pEquip = (Equipment*)ItemMgr::Inst()->GetPocketItemByID(main.n64ID);
		if(!P_VALID(pEquip))
			return ;

		INT nEquipLvl = pEquip->GetItemLevel();

		LocalPlayer *pPlayer = RoleMgr::Inst()->GetLocalPlayer();
		if( !P_VALID(pPlayer) )
			return;

		//if( fStuffEffect >= 0.5f )
		//	fStuffEffect = 0.5f;
		//else if( fStuffEffect <= 0.0f )
		//	fStuffEffect = 0.0f;

		// 加值=（装备等级×fcoefficientA + fcoefficientB）/ fcoefficientC ×（1+材料品质加成）
		INT nAddValue = INT(( nEquipLvl* pEngrave->fcoefficientA + pEngrave->fcoefficientB ) / pEngrave->fcoefficientC * ( 1.0f + fStuffEffect ));

		TCHAR szShowInfo[X_SHORT_NAME] = {0};
		_sntprintf( szShowInfo, X_SHORT_NAME, _T("Engrave_%d"), pEngrave->eEngraveAtt );
		TCHAR szShow[X_SHORT_NAME] = {0};
		_sntprintf( szShow, X_SHORT_NAME, g_StrTable[szShowInfo], nAddValue );
		m_pStcShowAddedValue->SetInvisible(false);
		m_pStcShowAddedValue->SetText(szShow);
	}
}

void EquipConsolidateFrame::ShowNewPosyAtt( const tagPosyProto *pPosy, float fStuffEffect )
{
	int nStuffNum = 0;
	vector<tagAssistStuff> stuff;
	m_Container.GetAssistStuffMap(stuff);
	int size = (int)stuff.size();
	int nStuffCount = 0;
	for( int i=0; i<size; ++i )
	{
		if( (stuff[i].nBagQuan!=0 && stuff[i].nBagQuan!=GT_INVALID)
			&& stuff[i].nBagQuan >= stuff[i].nQuantity )
		{
			++nStuffNum;
			nStuffCount += stuff[i].nQuantity;
		}
	}

	//所有材料都放了
	if( nStuffNum == size )
	{
		tagMainStuff main;
		m_Container.GetMainStuff(main);
		Equipment* pEquip = (Equipment*)ItemMgr::Inst()->GetPocketItemByID(main.n64ID);
		if(!P_VALID(pEquip))
			return ;

		INT nEquipLvl = pEquip->GetItemLevel();

		LocalPlayer *pPlayer = RoleMgr::Inst()->GetLocalPlayer();
		if( !P_VALID(pPlayer) )
			return;

		//if( fStuffEffect >= 1.0f )
		//	fStuffEffect = 1.0f;
		//else if( fStuffEffect <= 0.0f )
		//	fStuffEffect = 0.0f;

		// 加值=（装备等级×fcoefficientA + fcoefficientB）/ fcoefficientC ×（1+材料品质加成）
		INT nAddValue = INT(( nEquipLvl* pPosy->fcoefficientA + pPosy->fcoefficientB ) / pPosy->fcoefficientC * ( 1.0f + fStuffEffect ));

		TCHAR szShowInfo[X_SHORT_NAME] = {0};
		_sntprintf( szShowInfo, X_SHORT_NAME, _T("Posy_%d"), pPosy->ePosyAtt );
		TCHAR szShow[X_SHORT_NAME] = {0};
		_sntprintf( szShow, X_SHORT_NAME, g_StrTable[szShowInfo], nAddValue );
		m_pStcShowAddedValue->SetInvisible(false);
		m_pStcShowAddedValue->SetText(szShow);
	}
}

void EquipConsolidateFrame::HandleItem( bool bOperable )
{
	vector<tagAssistStuff> assStuff;
	m_Container.GetAssistStuffMap(assStuff);
	for( size_t i = 0; i < assStuff.size(); i++ )
	{
		Item* pItem = (Item*)ItemMgr::Inst()->GetPocketItemByID(assStuff[i].n64ID);
		if( P_VALID(pItem) )
			pItem->SetOperable(bOperable);
	}

	tagIMStuff im;
	m_Container.GetIMStuff(im);
	Item* pItem = (Item*)ItemMgr::Inst()->GetPocketItemByID(im.n64ID);
	if( P_VALID(pItem) )
		pItem->SetOperable(bOperable);

	tagMainStuff mainStuff;
	m_Container.GetMainStuff(mainStuff);
	pItem = (Item*)ItemMgr::Inst()->GetPocketItemByID(mainStuff.n64ID);
	if( P_VALID(pItem) )
		pItem->SetOperable(bOperable);
}