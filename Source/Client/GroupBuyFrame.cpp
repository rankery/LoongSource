#include "stdafx.h"
#include "GroupBuyFrame.h"
#include "MallMgr.h"
#include "IconStatic.h"
#include "MallFrame.h"
#include "ItemProtoData.h"
#include "Item.h"
#include "ItemButton.h"
#include "ItemMgr.h"
#include "ServerTime.h"
#include "PlayerNameTab.h"
#include "ToolTipCreator.h"
#include "ToolTipFrame.h"
#include "LocalPlayer.h"
#include "RoleMgr.h"
#include "ChatFrame.h"


#define Max_Update_GroupInfo_Time 1000 * 10

const TCHAR *szPromoteHave1	= _T("data\\ui\\mall\\L_cu.bmp");
const TCHAR *szPromoteNot1	= _T("data\\ui\\mall\\L_cu_daizhong.bmp");
const TCHAR *szGroupHave1	= _T("data\\ui\\mall\\L_tuan.bmp");
const TCHAR *szGroupNot1	= _T("data\\ui\\mall\\L_tuan_daizhong.bmp");
const TCHAR *szPresentHave1	= _T("data\\ui\\mall\\L_zeng.bmp");
const TCHAR *szPresentNot1	= _T("data\\ui\\mall\\L_zeng_daizhong.bmp");
const TCHAR *szNumHave1		= _T("data\\ui\\mall\\L_xian.bmp");
const TCHAR *szNumNot1		= _T("data\\ui\\mall\\L_xian_daizhong.bmp");
const TCHAR *szExchangeHove1	= _T("data\\ui\\mall\\L_dui.bmp");
const TCHAR *szExchangeNot1	= _T("data\\ui\\mall\\L_dui_daizhong.bmp");

const TCHAR *szSelectItem1	= _T("data\\ui\\Common\\L_icon-l.bmp");


GroupBuyFrame::GroupBuyFrame( void ):m_Trunk(this)
{
	m_pWndGBuy			= NULL;
	m_pWndGInfo			= NULL;
	m_pBtnLauncher		= NULL;
	m_pBtnCancel		= NULL;
	m_pStcItem			= NULL;
	m_pStcHoldTime		= NULL;
	m_nInfoCurPage		= GT_INVALID;
	m_nInfoMaxPage		= GT_INVALID;
	m_pBtnPageUp		= NULL;
	m_pBtnPageDown		= NULL;
	m_dwUpdateTime		= GT_INVALID;
	m_eScope			= EGPS_NULL;
	m_pLBoxResponse		= NULL;
	m_pBtnResponse		= NULL;
	m_pBtnGroupBuyExp	= NULL;
	m_pStcPage			= NULL;
	for( int i=0; i<Max_GroupBuy_Size; ++i )
	{
		m_pStcGBBack[i]			= NULL;
		m_pStcResponse[i]		= NULL;
		m_pStcDiscount[i]		= NULL;
		m_pStcPrice[i]			= NULL;
		m_pPBtnScale[i]			= NULL;
		m_pStcScaleBack[i]		= NULL;
	}
	for( int i=0; i<Max_GBInfo_PerPage; ++i )
	{
		m_pStcInfoBack[i]		= NULL;
		m_pStcInfoSele[i]		= NULL;
		m_pBtnBGItemInfo[i]		= NULL;
		m_pStcItemName[i]		= NULL;
		m_pStcItemPrice[i]		= NULL;
		m_pStcResBack[i]		= NULL;
		m_pStcLauncherName[i]	= NULL;
		m_pStcResCount[i]		= NULL;
		m_pStcResTime[i]		= NULL;
		for( int j=0; j<Max_Item_Pro; ++j )
		{
			m_pStcGBItemState[i][j]		= NULL;
		}
	}
}

GroupBuyFrame::~GroupBuyFrame( void )
{

}

BOOL GroupBuyFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam );

	//--注册网络消息
	m_pCmdMgr->Register("NS_LaunchGroupPurchase",	(NETMSGPROC)m_Trunk.sfp2(&GroupBuyFrame::OnNS_LaunchGroupPurchase),		_T("NS_LaunchGroupPurchase"));
	m_pCmdMgr->Register("NS_Getparticipators",		(NETMSGPROC)m_Trunk.sfp2(&GroupBuyFrame::OnNS_Getparticipators),		_T("NS_Getparticipators"));
	m_pCmdMgr->Register("NS_RespondGroupPurchase",	(NETMSGPROC)m_Trunk.sfp2(&GroupBuyFrame::OnNS_RespondGroupPurchase),	_T("NS_RespondGroupPurchase"));
	m_pCmdMgr->Register("NS_RespondBroadCast",		(NETMSGPROC)m_Trunk.sfp2(&GroupBuyFrame::OnNS_RespondBroadCast),		_T("NS_RespondBroadCast"));

	return bRet;
}

BOOL GroupBuyFrame::Destroy()
{
	//--注销网络消息
	m_pCmdMgr->UnRegister("NS_LaunchGroupPurchase",		(NETMSGPROC)m_Trunk.sfp2(&GroupBuyFrame::OnNS_LaunchGroupPurchase));
	m_pCmdMgr->UnRegister("NS_Getparticipators",		(NETMSGPROC)m_Trunk.sfp2(&GroupBuyFrame::OnNS_Getparticipators));
	m_pCmdMgr->UnRegister("NS_RespondGroupPurchase",	(NETMSGPROC)m_Trunk.sfp2(&GroupBuyFrame::OnNS_RespondGroupPurchase));
	m_pCmdMgr->UnRegister("NS_RespondBroadCast",		(NETMSGPROC)m_Trunk.sfp2(&GroupBuyFrame::OnNS_RespondBroadCast));

	if( P_VALID(GetObj("NotSelectMallItem_MsgBox")) )
		KillObj("NotSelectMallItem_MsgBox");
	if( P_VALID(GetObj("FreeMallItemNoLauncher_MsgBox")) )
		KillObj("FreeMallItemNoLauncher_MsgBox");
	if( P_VALID(GetObj("MallItemNoLauncher_MsgBox")) )
		KillObj("MallItemNoLauncher_MsgBox");
	if( P_VALID(GetObj("LauncherSucess_MsgBox")) )
		KillObj("LauncherSucess_MsgBox");
	if( P_VALID(GetObj("LauncherError_MsgBox")) )
		KillObj("LauncherError_MsgBox");
	if( P_VALID(GetObj("PackMallItemNoLauncher_MsgBox")) )
		KillObj("PackMallItemNoLauncher_MsgBox");
	if( P_VALID(GetObj("ResponseSelf_MsgBox")) )
		KillObj("ResponseSelf_MsgBox");
	if( P_VALID(GetObj("ResponseNumberFull_MsgBox")) )
		KillObj("ResponseNumberFull_MsgBox");


	if( P_VALID(m_pWndGBuy) )
		m_pGUI->AddToDestroyList(m_pWndGBuy);

	return GameFrame::Destroy();
}

BOOL GroupBuyFrame::EscCancel()
{
	if( m_pWndGBuy->IsInvisible() )
		return FALSE;

	m_pWndGBuy->SetInvisible(TRUE);
	return TRUE;
}

BOOL GroupBuyFrame::ReloadUI()
{
	XmlElement ele;
	tstring strPath = g_strLocalPath + _T("\\ui\\groupbuy.xml");
	m_pGUI->LoadXml(&ele, "VFS_System", strPath.c_str());
	m_pGUI->ChangeXml(&ele, "groupbuy_win\\groupbuy_backpic\\gb_back\\itemback\\item", "ClassName", "MallButton");
	//调整位置
	GUIWnd* pDesktop = m_pGUI->GetDesktop();
	stringstream streamX, streamY;
	streamX << (pDesktop->GetSize().x/2-225);
	streamY << (pDesktop->GetSize().y/2);
	m_pGUI->ChangeXml(&ele, "groupbuy_win", "LogicPos_x", streamX.str().c_str());
	m_pGUI->ChangeXml(&ele, "groupbuy_win", "LogicPos_y", streamY.str().c_str());
	m_pWndGBuy = m_pGUI->CreateWnd(_T("\\desktop"), &ele);
	m_pWndGBuy->SetInvisible(TRUE);


	//--加载发起团购控件
	m_pBtnLauncher		= (GUIButton*)m_pWndGBuy->GetChild(_T("groupbuy_backpic\\new"));
	m_pBtnCancel		= (GUIButton*)m_pWndGBuy->GetChild(_T("groupbuy_backpic\\new0"));
	m_pStcItem			= (MallButton*)m_pWndGBuy->GetChild(_T("groupbuy_backpic\\gb_back\\itemback\\item"));
	m_pStcHoldTime		= (GUIStatic*)m_pWndGBuy->GetChild(_T("groupbuy_backpic\\gb_back\\time"));
	TCHAR szBuff[X_LONG_NAME] = {0};
	for( int i=0; i<Max_GroupBuy_Size; i++ )
	{
		_sntprintf( szBuff, X_LONG_NAME, _T("groupbuy_backpic\\gb_back\\groupbuy_pic%d"), i+1 );
		m_pStcGBBack[i] = (GUIStatic*)m_pWndGBuy->GetChild(szBuff);

		_sntprintf( szBuff, X_LONG_NAME, _T("groupbuy_backpic\\gb_back\\groupbuy_pic%d\\person_num%d"), i+1, i+1 );
		m_pStcResponse[i] = (GUIStatic*)m_pWndGBuy->GetChild(szBuff);
		
		_sntprintf( szBuff, X_LONG_NAME, _T("groupbuy_backpic\\gb_back\\groupbuy_pic%d\\num%d"), i+1, i+1 );
		m_pStcDiscount[i] = (GUIStatic*)m_pWndGBuy->GetChild(szBuff);

		_sntprintf( szBuff, X_LONG_NAME, _T("groupbuy_backpic\\gb_back\\groupbuy_pic%d\\price%d"), i+1, i+1 );
		m_pStcPrice[i] = (GUIStatic*)m_pWndGBuy->GetChild(szBuff);

		_sntprintf( szBuff, X_LONG_NAME, _T("groupbuy_backpic\\gb_back\\groupbuy_pic%d"), i+1 );
		m_pStcScaleBack[i] = (GUIPushButton*)m_pWndGBuy->GetChild(szBuff);

		_sntprintf( szBuff, X_LONG_NAME, _T("groupbuy_backpic\\gb_back\\groupbuy_pic%d\\choose%d"), i+1, i+1 );
		m_pPBtnScale[i] = (GUIPushButton*)m_pWndGBuy->GetChild(szBuff);
	}

	//--加载响应团购
	GUIWnd* pWnd	= m_pGUI->GetWnd(((MallFrame*)m_pFather)->GetGUIFullName().c_str());

	m_pWndGInfo		= pWnd->GetChild(_T("mallbackpic\\groupbuy_page"));
	m_pBtnPageUp	= (GUIButton*)m_pWndGInfo->GetChild(_T("backbutt"));
	m_pBtnPageDown	= (GUIButton*)m_pWndGInfo->GetChild(_T("nextbutt"));
	m_pStcPage		= (GUIStatic*)m_pWndGInfo->GetChild(_T("page"));
	m_pBtnResponse	= (GUIButton*)m_pWndGInfo->GetChild(_T("button"));
	m_pBtnGroupBuyExp	= (GUIStatic*)m_pWndGInfo->GetChild(_T("num1"));
	m_pLBoxResponse	= (GUIListBox*)m_pWndGInfo->GetChild(_T("memberlist"));
	m_pLBoxResponse->SetColNum(1, 130);

	for( int i=0; i<Max_GBInfo_PerPage; ++i )
	{
		_sntprintf( szBuff, X_LONG_NAME, _T("itemlist\\itemback%d"), i+1 );
		m_pStcInfoBack[i] = (GUIStatic*)m_pWndGInfo->GetChild(szBuff);
		m_pStcInfoBack[i]->SetInvisible(TRUE);
		
		_sntprintf( szBuff, X_LONG_NAME, _T("itemlist\\itemback%d\\itempic"), i+1 );
		m_pStcInfoSele[i] = (IconStatic*)m_pWndGInfo->GetChild(szBuff);

		_sntprintf( szBuff, X_LONG_NAME, _T("itemlist\\itemback%d\\itempic\\item"), i+1 );
		m_pBtnBGItemInfo[i] = (MallButton*)m_pWndGInfo->GetChild(szBuff);

		_sntprintf( szBuff, X_LONG_NAME, _T("itemlist\\itemback%d\\name"), i+1 );
		m_pStcItemName[i] = (GUIStatic*)m_pWndGInfo->GetChild(szBuff);

		_sntprintf( szBuff, X_LONG_NAME, _T("itemlist\\itemback%d\\price"), i+1 );
		m_pStcItemPrice[i] = (GUIStatic*)m_pWndGInfo->GetChild(szBuff);

		_sntprintf( szBuff, X_LONG_NAME, _T("itemlist\\gbmessage%d"), i+1 );
		m_pStcResBack[i] = (GUIStatic*)m_pWndGInfo->GetChild(szBuff);
		m_pStcResBack[i]->SetInvisible(TRUE);

		_sntprintf( szBuff, X_LONG_NAME, _T("itemlist\\gbmessage%d\\name"), i+1 );
		m_pStcLauncherName[i] = (GUIStatic*)m_pWndGInfo->GetChild(szBuff);

		_sntprintf( szBuff, X_LONG_NAME, _T("itemlist\\gbmessage%d\\num"), i+1 );
		m_pStcResCount[i] = (GUIStatic*)m_pWndGInfo->GetChild(szBuff);

		_sntprintf( szBuff, X_LONG_NAME, _T("itemlist\\gbmessage%d\\time"), i+1 );
		m_pStcResTime[i] = (GUIStatic*)m_pWndGInfo->GetChild(szBuff);

		for( int j=0; j<Max_Item_Pro; ++j )
		{
			_sntprintf( szBuff, X_LONG_NAME, _T("itemlist\\itemback%d\\pic%d"), i+1, j+1 );
			m_pStcGBItemState[i][j] = (IconStatic*)m_pWndGInfo->GetChild(szBuff);
		}
	}


	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWndGBuy->GetFullName().c_str(), m_Trunk.sfp1(&GroupBuyFrame::EventHandler2GroupBuy));
	m_pGUI->RegisterEventHandler(m_pWndGInfo->GetFullName().c_str(), m_Trunk.sfp1(&GroupBuyFrame::EventHandler2GroupInfo));

	return TRUE;
}

VOID GroupBuyFrame::Update()
{

}

DWORD GroupBuyFrame::EventHandler2GroupBuy( tagGUIEvent* pEvent )
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	switch (pEvent->eEvent)
	{
	case EGUIE_Click:
		{
			if( pWnd == m_pBtnLauncher )
			{
				MallFrame* pFrame = MallMgr::Inst()->GetMallFrame();
				if( P_VALID(pFrame) )
				{
					MallFrame::EMallItemOperateState eOperate = pFrame->GetOpereateState();
					switch(eOperate)
					{
					case MallFrame::EMIOS_NormalItem:
					case MallFrame::EMIOS_Promote:
						{							
							tagMallItemProto* pItem = MallMgr::Inst()->GetMallItemByID(m_dwGLauncherItemID);
							if( P_VALID(pItem) )
							{
								tagNC_LaunchGroupPurchase msg;
								msg.dwTypeID = m_dwGLauncherItemID;
								msg.nIndexInServer = pItem->nIndexInServer;
								DWORD dwTime = CalcTimeDiff(tagDWORDTime(DWORD(pItem->dwTimeSaleEnd)), ServerTime::Inst()->CalCurrentServerDwordTime());
								if( dwTime>0 && pItem->dwTimeSaleEnd!=GT_INVALID
									&& pItem->nSalePrice!=GT_INVALID && pItem->nPrice!=pItem->nSalePrice )
									msg.nPrice = pItem->nSalePrice;
								else
									msg.nPrice = pItem->nPrice;
								msg.byScope = (BYTE)m_eScope;
								m_pSession->Send(&msg);
							}	
							else
							{
								if( P_VALID(GetObj("LauncherError_MsgBox")) )
									KillObj("LauncherError_MsgBox");
								CreateObj("LauncherError_MsgBox", "MsgBoxEx");

								TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade30")], 
									_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
							}
						}
						break;			
					default:
						{
							if( P_VALID(GetObj("LauncherError_MsgBox")) )
								KillObj("LauncherError_MsgBox");
							CreateObj("LauncherError_MsgBox", "MsgBoxEx");

							TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade31")], 
								_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
						}
						break;
					}
					
				}
			}
			else if( pWnd == m_pBtnCancel )
			{
				m_pWndGBuy->SetInvisible(TRUE);
			}
		}
		break;
	case EGUIE_PushButtonDown:
		{
			if( pEvent->dwParam1 != 1 )
				return 0;

			// 每个pushbutton都不是互斥的,所以要单独判断
			if( pWnd == m_pPBtnScale[EGPS_SMALL] )
			{
				m_eScope = EGPS_SMALL;
				m_pPBtnScale[EGPS_MEDIUM]->SetState(EGUIBS_PopUp, FALSE);
				m_pPBtnScale[EGPS_BIG]->SetState(EGUIBS_PopUp, FALSE);
			}
			else if( pWnd == m_pPBtnScale[EGPS_MEDIUM]  )
			{
				m_eScope = EGPS_MEDIUM;
				m_pPBtnScale[EGPS_SMALL]->SetState(EGUIBS_PopUp, FALSE);
				m_pPBtnScale[EGPS_BIG]->SetState(EGUIBS_PopUp, FALSE);
			}
			else if( pWnd == m_pPBtnScale[EGPS_BIG]  )
			{
				m_eScope = EGPS_BIG;
				m_pPBtnScale[EGPS_SMALL]->SetState(EGUIBS_PopUp, FALSE);
				m_pPBtnScale[EGPS_MEDIUM]->SetState(EGUIBS_PopUp, FALSE);
			}
		}
		break;
	case EGUIE_MoveIntoCtrl:
		{
			if( pWnd == m_pStcItem )
			{
				DWORD dwID =  m_pStcItem->GetBtnData().GetData1();
				const tagMallItemProto* pItemProto = MallMgr::Inst()->GetMallItemByID(dwID);
				if( P_VALID(pItemProto) )
				{
					DWORD dwTypeID = pItemProto->dwTypeID;
					ShowTip(m_pStcItem, ToolTipCreator::Inst()->GetItemTips(dwTypeID).c_str());
					break;;
				}
			}
		}
		break;
	case EGUIE_MoveOutOfCtrl:
		{
			ShowTip(NULL, _T(""));
		}
		break;
	default:
		break;
	}

	return 1;
}

DWORD GroupBuyFrame::EventHandler2GroupInfo(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	switch (pEvent->eEvent)
	{
	case EGUIE_Click:
		{
			if( pWnd == m_pBtnPageDown )
			{
				this->PageDown();
			}
			else if( pWnd == m_pBtnPageUp )
			{
				this->PageUp();
			}
			else if( pWnd == m_pBtnResponse )
			{
				if( !JudgeResponseGroupBuy() )
				{
					return 0;
				}

				tagNC_RespondGroupPurchase msg;
				msg.dwTypeID = m_seleDate.GetData1();
				msg.dwRoleID = m_seleDate.GetData2();
				msg.dwGuildID = (DWORD)m_seleDate.GetData3();

				INT64 n64Tmp = msg.dwRoleID;
				INT64 n64Key = (n64Tmp << 32) | msg.dwTypeID;
				tagGroupPurchase* pGP = MallMgr::Inst()->GetGroupBuyInfoByID(n64Key);
				if( P_VALID(pGP) )
				{
					msg.nPrice = pGP->nPrice;
				}
				m_pSession->Send(&msg);
			}
		}
		break;
	case EGUIE_Drag:
		{
			//左键单击,选中团购商品
			if (_tcsnccmp(pWnd->GetName().c_str(), _T("item"), 4) == 0)
			{
				this->RefreshGroupBuyItemUI();
				for( int i=0; i<Max_GBInfo_PerPage; ++i )
				{
					if( pWnd == m_pBtnBGItemInfo[i] )
					{
						m_seleDate = m_pBtnBGItemInfo[i]->GetBtnData();
						m_pStcInfoSele[i]->SetPic(szSelectItem1);
						tagNC_GetParticipators msg;
						msg.dwTypeID = m_seleDate.GetData1();
						msg.dwRoleID = m_seleDate.GetData2();
						msg.dwGuildID = (DWORD)m_seleDate.GetData3();
						m_pSession->Send(&msg);
						break;
					}
				}
			}
		}
		break;
	case EGUIE_MoveIntoCtrl:
		{
			if (_tcsnccmp(pWnd->GetName().c_str(), _T("item"), 4) == 0)
			{
				for( int i=0; i<Max_GBInfo_PerPage; ++i )
				{
					if( pWnd == m_pBtnBGItemInfo[i] )
					{
						DWORD dwID =  m_pBtnBGItemInfo[i]->GetBtnData().GetData1();
						const tagMallItemProto* pItemProto = MallMgr::Inst()->GetMallItemByID(dwID);
						if( P_VALID(pItemProto) )
						{
							DWORD dwTypeID = pItemProto->dwTypeID;
							ShowTip(m_pBtnBGItemInfo[i], ToolTipCreator::Inst()->GetItemTips(dwTypeID).c_str());
							break;;
						}
					}
				}
			}
			else if (_tcsnccmp(pWnd->GetName().c_str(), _T("pic"), 3) == 0)
			{
				for( int i=0; i<Max_GBInfo_PerPage; ++i )
				{
					for( int j=0; j<Max_Item_Pro; ++j )
					{
						if( pWnd == m_pStcGBItemState[i][j] )
						{
							ShowMallItemTips(i, j);
							break;
						}
					}
				}
			}
		}
		break;
	case EGUIE_MoveOutOfCtrl:
		{
			ShowTip(NULL, _T(""));
		}
		break;
	default:
		break;
	}
	return 1;
}

void GroupBuyFrame::ShowWnd( EGroupBuy eType/*=EGroupBuy_NULL*/ )
{
	switch(eType)
	{
	case EGroupBuy_NULL:
		{
			m_pWndGBuy->SetInvisible(TRUE);
			m_pWndGInfo->SetInvisible(TRUE);
		}
		break;
	case EGroupBuy_Purchase:
		{
			this->HandlerGroupLauncher();
			m_eScope = EGPS_NULL;
			m_pWndGBuy->SetInvisible(FALSE);
			m_pWndGBuy->FlipToTop();
		}
		break;
	case EGroupBuy_Info:
		{
			// 1分钟向服务器发次Update
			if( timeGetTime()-m_dwUpdateTime >= Max_Update_GroupInfo_Time )
			{
				m_dwUpdateTime = timeGetTime();
				//向服务器查询当前团购的信息
				MallMgr::Inst()->SendGetGroupBuyInfo();
			}
			else
			{
				RefreshGroupBuyInfo();
			}
		}
		break;
	}
}

void GroupBuyFrame::RefreshGroupBuyInfo()
{
	//每次Update时,重新刷新团购信息总数
	INT nSize = MallMgr::Inst()->GetGroupBuyInfoSize();
	if( nSize%Max_GBInfo_PerPage == 0 )
	{
		m_nInfoMaxPage = nSize / Max_GBInfo_PerPage;
	}
	else
	{
		m_nInfoMaxPage = nSize / Max_GBInfo_PerPage + 1;
	}
	ShowCurPageGroupBuyInfo(0);
	m_pWndGInfo->SetInvisible(FALSE);
	m_pWndGInfo->SetRefresh(TRUE);
}

void GroupBuyFrame::ShowCurPageGroupBuyInfo( INT nPage )
{
	HideTotalGroupBuyInfoCtrl();
	m_nInfoCurPage = nPage;
	map<INT64, tagGroupPurchase> mapGroupPur = MallMgr::Inst()->GetGroupBuyInfo();
	map<INT64, tagGroupPurchase>::iterator it = mapGroupPur.begin();
	int i=0;
	for( ; i<m_nInfoCurPage*Max_GBInfo_PerPage; ++i )
	{
		++it;
		if( i > (int)mapGroupPur.size() )
			break;
	}
	i=0;
	TCHAR szBuff[X_LONG_NAME] = {0};
	while( i<Max_GBInfo_PerPage && it!=mapGroupPur.end() )
	{
		tagGroupPurchase* pGP = &(it->second);
		if( P_VALID(pGP) )
		{
			const tagMallItemProto* pItem = MallMgr::Inst()->GetMallItemByID(pGP->dwMallID);
			const tagItemProto* pItemProto = ItemProtoData::Inst()->FindItemProto(pItem->dwTypeID);
			const tagItemDisplayInfo *pInfo = ItemProtoData::Inst()->FindItemDisplay(pItem->dwTypeID);
			if( P_VALID(pItem) && P_VALID(pItemProto) && P_VALID(pInfo) )  
			{
				m_pStcInfoBack[i]->SetInvisible(FALSE);
				m_pStcResBack[i]->SetInvisible(FALSE);
				m_pBtnBGItemInfo[i]->RefreshItem(pItemProto->dwTypeID, pItem->byGroupPurchaseAmount, pItemProto->byQuality);
				//记录 商城商品表id 发起人id 帮派id 
				m_pBtnBGItemInfo[i]->SetBtnData(ESBT_Item, pGP->dwMallID, pGP->dwRoleID, pGP->dwGuildID);
				m_pStcItemName[i]->SetText(pInfo->szName);			
				_sntprintf(szBuff, X_LONG_NAME, g_StrTable[_T("MallTrade62")], pItem->nPrice*pItem->byGroupPurchaseAmount, pGP->nPrice);
				m_pStcItemPrice[i]->SetText(szBuff);

				//发起人
				tstring strRoleName = PlayerNameTab::Inst()->FindNameByID(pGP->dwRoleID);
				m_pStcLauncherName[i]->SetText(strRoleName.c_str());

				//响应人数
				_sntprintf( szBuff, X_LONG_NAME, _T("%d/%d"), pGP->nParticipatorNum, pGP->nRequireNum );
				m_pStcResCount[i]->SetText(szBuff);

				//剩余时间
				_sntprintf( szBuff, X_LONG_NAME, _T("%u 小时"), pGP->dwRemainTime/3600+1 );
				m_pStcResTime[i]->SetText(szBuff);

				//热
				if( pItem->bHot )
					m_pBtnBGItemInfo[i]->SetHotMallItem(true);
				else
					m_pBtnBGItemInfo[i]->SetHotMallItem(false);
				//新
				if( pItem->bNew )
					m_pBtnBGItemInfo[i]->SetNewMallItem(true);
				else
					m_pBtnBGItemInfo[i]->SetNewMallItem(false);

				//促
				if( JudgePromotePro(pItem) )
					m_pStcGBItemState[i][0]->SetPic(szPromoteHave1);
				else
					m_pStcGBItemState[i][0]->SetPic(szPromoteNot1);
				//团
				if( JudgeGroupBuyPro(pItem) )
					m_pStcGBItemState[i][1]->SetPic(szGroupHave1);
				else
					m_pStcGBItemState[i][1]->SetPic(szGroupNot1);
				//赠
				if( JudgePresentPro(pItem) )
					m_pStcGBItemState[i][2]->SetPic(szPresentHave1);
				else
					m_pStcGBItemState[i][2]->SetPic(szPresentNot1);
				//限
				if( JudgeNumPro(pItem) )
					m_pStcGBItemState[i][3]->SetPic(szNumHave1);
				else 
					m_pStcGBItemState[i][3]->SetPic(szNumNot1);
				//兑
				if( JudgeExchangePro(pItem) )
					m_pStcGBItemState[i][4]->SetPic(szExchangeHove1);
				else 
					m_pStcGBItemState[i][4]->SetPic(szExchangeNot1);
			}

		}
		++i;
		++it;
	}

	TCHAR szPage[X_SHORT_NAME] = {0};
	_sntprintf( szPage, X_SHORT_NAME, _T("%d / %d"), m_nInfoCurPage+1, m_nInfoMaxPage );
	m_pStcPage->SetText( szPage );
	_sntprintf( szPage, X_SHORT_NAME, _T("%d"), MallMgr::Inst()->GetGroupPurExp() );
	m_pBtnGroupBuyExp->SetText(szPage);
}

void GroupBuyFrame::HideTotalGroupBuyCtrl()
{
	m_pStcItem->SetInvisible(TRUE);
	m_pStcHoldTime->SetInvisible(TRUE);
	m_pStcItem->SetBtnData(ESBT_Null, GT_INVALID, GT_INVALID, GT_INVALID);
	for( int i=0; i<Max_GroupBuy_Size; ++i )
	{
		m_pStcGBBack[i]->SetInvisible(TRUE);
		m_pPBtnScale[i]->SetState(EGUIBS_PopUp);
	}
}

void GroupBuyFrame::HideTotalGroupBuyInfoCtrl()
{
	m_seleDate.Set(ESBT_Null, GT_INVALID, GT_INVALID, GT_INVALID);
	tstring strPic = _T("data\\ui\\common\\l_icon.bmp");
	for( int i=0; i<Max_GBInfo_PerPage; ++i )
	{
		m_pStcInfoBack[i]->SetInvisible(TRUE);
		m_pStcResBack[i]->SetInvisible(TRUE);
		m_pStcInfoSele[i]->SetPic(strPic.c_str());
	}
}

void GroupBuyFrame::HandlerGroupLauncher()
{
	HideTotalGroupBuyCtrl();
	MallFrame* pFrame = MallMgr::Inst()->GetMallFrame();
	if( P_VALID(pFrame) )
	{
		m_dwGLauncherItemID = pFrame->GetMallItemHopeTypeID();
		MallFrame::EMallItemOperateState eOperate = pFrame->GetOpereateState();
		switch( eOperate )
		{
		case MallFrame::EMIOS_NULL:
			{
				if( P_VALID(GetObj("NotSelectMallItem_MsgBox")) )
					KillObj("NotSelectMallItem_MsgBox");
				CreateObj("NotSelectMallItem_MsgBox", "MsgBoxEx");

				TObjRef<MsgBoxEx>("NotSelectMallItem_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade30")], 
					_T("NotSelectMallItem_Event"), MsgBoxFlag(MBF_OK), TRUE);

				m_pWndGBuy->SetInvisible(TRUE);
			}
			break;
		case MallFrame::EMIOS_Free:
			{

				if( P_VALID(GetObj("FreeMallItemNoLauncher_MsgBox")) )
					KillObj("FreeMallItemNoLauncher_MsgBox");
				CreateObj("FreeMallItemNoLauncher_MsgBox", "MsgBoxEx");

				TObjRef<MsgBoxEx>("FreeMallItemNoLauncher_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade31")], 
					_T("FreeMallItemNoLauncher_Event"), MsgBoxFlag(MBF_OK), TRUE);

				m_pWndGBuy->SetInvisible(TRUE);
			}
			break;
		case MallFrame::EMIOS_NormalItem:
		case MallFrame::EMIOS_Promote:
			{
				tagMallItemProto* pItem = MallMgr::Inst()->GetMallItemByID(m_dwGLauncherItemID);
				if( P_VALID(pItem) ) 
				{
					//验证是否能发起团购
					if( pItem->bySmallGroupDiscount==(BYTE)GT_INVALID
						&& pItem->byMediumGroupDiscount==(BYTE)GT_INVALID
						&& pItem->byBigGroupDiscount==(BYTE)GT_INVALID )
					{
						m_pWndGBuy->SetInvisible(TRUE);

						if( P_VALID(GetObj("MallItemNoLauncher_MsgBox")) )
							KillObj("MallItemNoLauncher_MsgBox");
						CreateObj("MallItemNoLauncher_MsgBox", "MsgBoxEx");

						TObjRef<MsgBoxEx>("MallItemNoLauncher_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade31")], 
							_T("MallItemNoLauncher_Event"), MsgBoxFlag(MBF_OK), TRUE);
					}

					//商品图标
					const tagItemProto* pItemProto = ItemProtoData::Inst()->FindItemProto(pItem->dwTypeID);
					if( P_VALID(pItemProto) )
					{
						m_pStcItem->RefreshItem(pItem->dwTypeID, pItem->byGroupPurchaseAmount, pItemProto->byQuality);
						m_pStcItem->SetBtnData( ESBT_Item, m_dwGLauncherItemID, pItem->dwTypeID, 0 );
						m_pStcItem->SetInvisible(FALSE);
					}

					// 团购持续时间
					TCHAR szTime[X_SHORT_NAME] = {0};
					_sntprintf( szTime, X_SHORT_NAME, g_StrTable[_T("MallTrade32")], pItem->dwPersistTime );
					m_pStcHoldTime->SetText(szTime);
					m_pStcHoldTime->SetInvisible(FALSE);

					//验证团购
					if( pItem->bySmallGroupDiscount!=(BYTE)GT_INVALID )
					{
						//响应人数
						TCHAR szBuff[X_SHORT_NAME] = {0};
						_sntprintf( szBuff, X_SHORT_NAME, _T("%d"), pItem->bySmallGroupHeadcount );
						m_pStcResponse[EGPS_SMALL]->SetText(szBuff);

						//折扣率
						_sntprintf( szBuff, X_SHORT_NAME, _T("%d"), pItem->bySmallGroupDiscount );
						m_pStcDiscount[EGPS_SMALL]->SetText(szBuff);

						//价格
						DWORD dwTime = CalcTimeDiff(tagDWORDTime(DWORD(pItem->dwTimeSaleEnd)), ServerTime::Inst()->CalCurrentServerDwordTime());
						if( dwTime>0 && pItem->dwTimeSaleEnd!=GT_INVALID
							&& pItem->nSalePrice!=GT_INVALID && pItem->nPrice!=pItem->nSalePrice )
						{
							float count = ((float)pItem->bySmallGroupDiscount)/100.0f;
							float exp = 0.2f*MallMgr::Inst()->GetGroupPurExp()/100000;
							DWORD dwPrice = (DWORD)((float)pItem->nSalePrice*(float)pItem->byGroupPurchaseAmount*(count-exp)+0.5f);
							_sntprintf(szBuff, X_SHORT_NAME, _T("%u"), dwPrice);
						}
						else
						{
							float count = ((float)pItem->bySmallGroupDiscount)/100.0f;
							float exp = 0.2f*MallMgr::Inst()->GetGroupPurExp()/100000;
							DWORD dwPrice = (DWORD)((float)pItem->nPrice*(float)pItem->byGroupPurchaseAmount*(count-exp)+0.5f);
							_sntprintf(szBuff, X_SHORT_NAME, _T("%u"), dwPrice);
						}
						m_pStcPrice[EGPS_SMALL]->SetText(szBuff);

						m_pStcScaleBack[EGPS_SMALL]->SetInvisible(FALSE);
					}
					else
						m_pStcScaleBack[EGPS_SMALL]->SetInvisible(TRUE);

					if( pItem->byMediumGroupDiscount!=(BYTE)GT_INVALID )
					{
						//响应人数
						TCHAR szBuff[X_SHORT_NAME] = {0};
						_sntprintf( szBuff, X_SHORT_NAME, _T("%d"), pItem->byMediumGroupHeadcount );
						m_pStcResponse[EGPS_MEDIUM]->SetText(szBuff);

						//折扣率
						_sntprintf( szBuff, X_SHORT_NAME, _T("%d"), pItem->byMediumGroupDiscount );
						m_pStcDiscount[EGPS_MEDIUM]->SetText(szBuff);

						//价格
						DWORD dwTime = CalcTimeDiff(tagDWORDTime(DWORD(pItem->dwTimeSaleEnd)), ServerTime::Inst()->CalCurrentServerDwordTime());
						if( dwTime>0 && pItem->dwTimeSaleEnd!=GT_INVALID
							&& pItem->nSalePrice!=GT_INVALID && pItem->nPrice!=pItem->nSalePrice )
						{
							float count = ((float)pItem->byMediumGroupDiscount)/100.0f;
							float exp = 0.2f*MallMgr::Inst()->GetGroupPurExp()/100000;
							DWORD dwPrice = (DWORD)((float)pItem->nSalePrice*(float)pItem->byGroupPurchaseAmount*(count-exp)+0.5f);
								_sntprintf(szBuff, X_SHORT_NAME, _T("%u"), dwPrice);
						}
						else
						{
							float count = ((float)pItem->byMediumGroupDiscount)/100.0f;
							float exp = 0.2f*MallMgr::Inst()->GetGroupPurExp()/100000;
							DWORD dwPrice = (DWORD)((float)pItem->nPrice*(float)pItem->byGroupPurchaseAmount*(count-exp)+0.5f);
								_sntprintf(szBuff, X_SHORT_NAME, _T("%u"), dwPrice);
						}
						m_pStcPrice[EGPS_MEDIUM]->SetText(szBuff);

						m_pStcScaleBack[EGPS_MEDIUM]->SetInvisible(FALSE);
					}
					else
						m_pStcScaleBack[EGPS_MEDIUM]->SetInvisible(TRUE);

					if( pItem->byBigGroupDiscount!=(BYTE)GT_INVALID )
					{
						//响应人数
						TCHAR szBuff[X_SHORT_NAME] = {0};
						_sntprintf( szBuff, X_SHORT_NAME, _T("%d"), pItem->byBigGroupHeadcount );
						m_pStcResponse[EGPS_BIG]->SetText(szBuff);

						//折扣率
						_sntprintf( szBuff, X_SHORT_NAME, _T("%d"), pItem->byBigGroupDiscount );
						m_pStcDiscount[EGPS_BIG]->SetText(szBuff);

						//价格
						DWORD dwTime = CalcTimeDiff(tagDWORDTime(DWORD(pItem->dwTimeSaleEnd)), ServerTime::Inst()->CalCurrentServerDwordTime());
						if( dwTime>0 && pItem->dwTimeSaleEnd!=GT_INVALID
							&& pItem->nSalePrice!=GT_INVALID && pItem->nPrice!=pItem->nSalePrice )
						{
							float count = ((float)pItem->byBigGroupDiscount)/100.0f;
							float exp = 0.2f*MallMgr::Inst()->GetGroupPurExp()/100000;
							DWORD dwPrice = (DWORD)((float)pItem->nSalePrice*(float)pItem->byGroupPurchaseAmount*(count-exp)+0.5f);
								_sntprintf(szBuff, X_SHORT_NAME, _T("%u"), dwPrice);
						}
						else
						{
							float count = ((float)pItem->byBigGroupDiscount)/100.0f;
							float exp = 0.2f*MallMgr::Inst()->GetGroupPurExp()/100000;
							DWORD dwPrice = (DWORD)((float)pItem->nPrice*(float)pItem->byGroupPurchaseAmount*(count-exp)+0.5f);
								_sntprintf(szBuff, X_SHORT_NAME, _T("%u"), dwPrice);
						}
						m_pStcPrice[EGPS_BIG]->SetText(szBuff);

						m_pStcScaleBack[EGPS_BIG]->SetInvisible(FALSE);
					}
					else 
						m_pStcScaleBack[EGPS_BIG]->SetInvisible(TRUE);
				}
				else
				{
					m_pWndGBuy->SetInvisible(TRUE);
				}
			}
			break;
		case MallFrame::EMIOS_PackItem:
			{
				if( P_VALID(GetObj("PackMallItemNoLauncher_MsgBox")) )
					KillObj("PackMallItemNoLauncher_MsgBox");
				CreateObj("PackMallItemNoLauncher_MsgBox", "MsgBoxEx");

				TObjRef<MsgBoxEx>("PackMallItemNoLauncher_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade31")], 
					_T("PackMallItemNoLauncher_Event"), MsgBoxFlag(MBF_OK), TRUE);

				m_pWndGBuy->SetInvisible(TRUE);
			}
			break;
		}		
	}
}

DWORD GroupBuyFrame::OnNS_LaunchGroupPurchase( tagNS_LaunchGroupPurchase* pMsg, DWORD dwParam )
{
	switch( pMsg->dwErrorCode )
	{
	case E_Success:
		{
			if( P_VALID(GetObj("LauncherSucess_MsgBox")) )
				KillObj("LauncherSucess_MsgBox");
			CreateObj("LauncherSucess_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherSucess_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade33")], 
				_T("LauncherSucess_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_GroupPurchase_ItemNotAllowable:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade31")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_GroupPurchase_NotInGuild:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade34")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_GroupPurchase_AlreadyInitiate:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade35")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_BagYuanBao_NotEnough:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade36")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_GroupPurchase_ServerBusy:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade45")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Mall_Close:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade46")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Con_PswNotPass:    //行囊没解锁
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade47")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	}
	m_pWndGBuy->SetInvisible(TRUE);
	return 0;
}

DWORD GroupBuyFrame::OnNS_Getparticipators(tagNS_Getparticipators* pMsg, DWORD dwParam)
{
	switch( pMsg->dwErrorCode )
	{
	case E_Success:
		{
			m_listRespons.Clear();
			m_pLBoxResponse->Clear();
			INT nParticipatorNum = pMsg->nParticipatorNum;
			for( int i=0; i<nParticipatorNum; ++i )
			{
				m_listRespons.PushBack(pMsg->dwParticipators[i]);
			}
			INT64 n64Key = pMsg->nGroupPurchaseKey;
			INT64 n64Tmp = m_seleDate.GetData2();
			INT64 n64Key1 = (n64Tmp << 32) | m_seleDate.GetData1();
			if( n64Key1 == n64Key )
			{
				ListResponse();
				MallMgr::Inst()->RefreshGroupBuyInfoByID(n64Key, m_listRespons.Size());
			}
		}
		break;
	case E_GroupPurchase_NoInfo:
		{
			INT64 n64Key = pMsg->nGroupPurchaseKey;
			INT64 n64Tmp = m_seleDate.GetData2();
			INT64 n64Key1 = (n64Tmp << 32) | m_seleDate.GetData1();
			if( n64Key1 == n64Key )
			{
				MallMgr::Inst()->DeleGroupBuyInfoByID(n64Key);
				RefreshGroupBuyInfo();
			}
		}
		break;
	}
	return 0;
}

DWORD GroupBuyFrame::OnNS_RespondGroupPurchase(tagNS_RespondGroupPurchase* pMsg, DWORD dwParam)
{
	switch( pMsg->dwErrorCode )
	{
	case E_Success:
		{
			if( P_VALID(GetObj("LauncherSucess_MsgBox")) )
				KillObj("LauncherSucess_MsgBox");
			CreateObj("LauncherSucess_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherSucess_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade37")],  
				_T("LauncherSucess_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_GroupPurchase_NotMember:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade38")],  
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_GroupPurchase_IsInitiate:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade39")],  
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_GroupPurchase_AlreadyInitiate:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade40")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_GroupPurchase_AlreadyEnd:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade41")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);

			//此时要把本地保存的删除
			INT64 n64Key = pMsg->nGroupPurchaseKey;
			if( n64Key!=GT_INVALID )
			{
				MallMgr::Inst()->DeleGroupBuyInfoByID(n64Key);
				RefreshGroupBuyInfo();
			}
		}
		break;
	case E_Trade_BagYB_NotEnough:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade42")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_GroupPurchase_NoInfo:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade43")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Mall_ID_Error:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade44")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_BagYuanBao_NotEnough:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("CurrencyError_NotEnoughYuanbao")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_GroupPurchase_ServerBusy:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade45")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Mall_Close:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade46")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Con_PswNotPass:    //行囊没解锁
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade47")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	}
	return 0;
}

DWORD GroupBuyFrame::OnNS_RespondBroadCast(tagNS_RespondBroadCast* pMsg, DWORD dwParam)
{
	switch( pMsg->eType )
	{
	case ERespondBroadCast_Launch:
		{
			ChatFrame* pFrame = (ChatFrame*)m_pFrameMgr->GetFrame(_T("Chat"));
			if( P_VALID(pFrame) )
			{
				tstring strRoleName = PlayerNameTab::Inst()->FindNameByID(pMsg->dwRoleID);
				const tagItemDisplayInfo *pInfo = ItemProtoData::Inst()->FindItemDisplay(pMsg->dwTypeID);
				tstring strItemName;
				if( P_VALID(pInfo) )
				{
					strItemName = pInfo->szName;
				}
				if( !strRoleName.empty() && !strItemName.empty() )
				{
					TCHAR szBuff[X_LONG_NAME] = {0};
					_sntprintf( szBuff, X_LONG_NAME, g_StrTable[_T("MallTrade48")], strRoleName.c_str(), strItemName.c_str	() );
					pFrame->PushInfo( szBuff, ESCC_Guild );
				}
			}
		}
		break;
	case ERespondBroadCast_Success:
		{
			ChatFrame* pFrame = (ChatFrame*)m_pFrameMgr->GetFrame(_T("Chat"));
			if( P_VALID(pFrame) )
			{
				const tagItemDisplayInfo *pInfo = ItemProtoData::Inst()->FindItemDisplay(pMsg->dwTypeID);
				tstring strItemName;
				if( P_VALID(pInfo) )
				{
					strItemName = pInfo->szName;
				}
				if( !strItemName.empty() )
				{
					TCHAR szBuff[X_LONG_NAME] = {0};
					_sntprintf( szBuff, X_LONG_NAME, g_StrTable[_T("MallTrade49")], strItemName.c_str() );
					pFrame->PushInfo( szBuff, ESCC_Guild );
				}
			}
		}
		break;
	case ERespondBroadCast_Lose:
		{
			ChatFrame* pFrame = (ChatFrame*)m_pFrameMgr->GetFrame(_T("Chat"));
			if( P_VALID(pFrame) )
			{
				const tagItemDisplayInfo *pInfo = ItemProtoData::Inst()->FindItemDisplay(pMsg->dwTypeID);
				tstring strItemName;
				if( P_VALID(pInfo) )
				{
					strItemName = pInfo->szName;
				}
				if( !strItemName.empty() )
				{
					TCHAR szBuff[X_LONG_NAME] = {0};
					_sntprintf( szBuff, X_LONG_NAME, g_StrTable[_T("MallTrade50")], strItemName.c_str() );
					pFrame->PushInfo( szBuff, ESCC_Guild );
				}
			}
		}
		break;
	}
	return 0;
}

bool GroupBuyFrame::JudgePromotePro( const tagMallItemProtoBase* pItem )
{
	if( pItem->dwTimeSaleEnd != GT_INVALID )
	{
		DWORD dwTimeSaleEnd = pItem->dwTimeSaleEnd;

		tagDWORDTime TimeSaleEnd = dwTimeSaleEnd;
		tagDWORDTime TimeCur = ServerTime::Inst()->CalCurrentServerDwordTime();
		DWORD dwTimeDiff = CalcTimeDiff(TimeSaleEnd, TimeCur);
		if( dwTimeDiff <= 0 )
			return false;
		else
			return true;
	}

	return false;
}

bool GroupBuyFrame::JudgeGroupBuyPro( const tagMallItemProtoBase* pItem )
{
	if( pItem->bySmallGroupDiscount != (BYTE)GT_INVALID )
	{
		return true;
	}
	else if( pItem->byMediumGroupDiscount != (BYTE)GT_INVALID )
	{
		return true;
	}
	else if( pItem->byBigGroupDiscount != (BYTE)GT_INVALID )
	{
		return true;
	}

	return false;
}

bool GroupBuyFrame::JudgePresentPro( const tagMallItemProtoBase* pItem )
{
	if( pItem->dwPresentID != GT_INVALID )  
	{
		const tagItemDisplayInfo* pInfo = ItemProtoData::Inst()->FindItemDisplay(pItem->dwPresentID);
		if( P_VALID(pInfo) )
			return true;
		else
			return false;
	}
	return false;
}

bool GroupBuyFrame::JudgeNumPro( const tagMallItemProtoBase* pItem )
{
	if( pItem->byNum != (BYTE)GT_INVALID )
		return true;

	return false;
}

bool GroupBuyFrame::JudgeExchangePro( const tagMallItemProtoBase* pItem )
{
	if( pItem->byExNum!=0 && pItem->byExNum!=(BYTE)GT_INVALID )
		return true;

	return false;
}

void GroupBuyFrame::PageDown()
{
	++m_nInfoCurPage;
	if( m_nInfoCurPage >= m_nInfoMaxPage ) 
	{
		--m_nInfoCurPage;
	}
	ShowCurPageGroupBuyInfo(m_nInfoCurPage);
}

void GroupBuyFrame::PageUp()
{
	--m_nInfoCurPage;
	if( m_nInfoCurPage < 0 ) 
	{
		++m_nInfoCurPage;
	}
	ShowCurPageGroupBuyInfo(m_nInfoCurPage);
}

void GroupBuyFrame::RefreshGroupBuyItemUI()
{
	tstring strPic = _T("data\\ui\\common\\l_icon.bmp");
	for( int i=0; i<Max_GBInfo_PerPage; ++i )
	{
		m_pStcInfoSele[i]->SetPic(strPic.c_str());
	}
}

void GroupBuyFrame::ShowMallItemTips( int nPos, int nProPos )
{
	DWORD dwID = m_pBtnBGItemInfo[nPos]->GetBtnData().GetData1();
	const tagMallItemProto* pItem = MallMgr::Inst()->GetMallItemByID(dwID);
	if( P_VALID(pItem) )
	{
		switch(nProPos)
		{
		case 0:	//促
			{
				ShowPromoteProTips(pItem, nPos, nProPos);
			}
			break;
		case 1:	//团
			{
				ShowGroupBuyProTips(pItem, nPos, nProPos);
			}
			break;
		case 2:	//赠
			{
				ShowPresentProTips(pItem, nPos, nProPos);
			}
			break;
		case 3:	//限
			{
				ShowNumProTips(pItem, nPos, nProPos);
			}
			break;
		case 4:	//兑
			{
				ShowExchangeProTips(pItem, nPos, nProPos);
			}
			break;
		}
	}
}

void GroupBuyFrame::ShowPromoteProTips( const tagMallItemProtoBase* pItem, int nGoodsPos, int nStatePos )
{
	TCHAR szBuff[X_LONG_NAME];
	if( pItem->dwTimeSaleEnd != GT_INVALID )
	{
		DWORD dwTimeSaleEnd = pItem->dwTimeSaleEnd;
		tagDWORDTime TimeSaleEnd = dwTimeSaleEnd;
		tagDWORDTime TimeCur = ServerTime::Inst()->CalCurrentServerDwordTime();
		DWORD dwTimeDiff = CalcTimeDiff(TimeSaleEnd, TimeCur);
		if(dwTimeDiff <= 0)
		{
			_stprintf( szBuff, g_StrTable[_T("MallTrade12")] );
		}
		else 
		{
			DWORD dwDay = MallMgr::Inst()->GetDay(dwTimeDiff);
			DWORD dwHour = MallMgr::Inst()->GetHour(dwTimeDiff, dwDay);
			DWORD dwMin = MallMgr::Inst()->GetMin(dwTimeDiff, dwDay, dwHour);
			_stprintf( szBuff, g_StrTable[_T("MallTrade13")], 
				dwDay, dwHour, dwMin, pItem->nSalePrice );
		}
		tstring strTips = g_StrTable[_T("TipsDefaultFont")];
		strTips += szBuff;
		ShowTip( m_pStcGBItemState[nGoodsPos][nStatePos], strTips.c_str() );
	}
}

void GroupBuyFrame::ShowGroupBuyProTips( const tagMallItemProtoBase* pItem, int nGoodsPos, int nStatePos )
{
	TCHAR szBuff[X_LONG_NAME];
	tstring strTips;
	strTips = g_StrTable[_T("TipsDefaultFont")];
	if( pItem->bySmallGroupDiscount != (BYTE)GT_INVALID )
	{
		_stprintf(szBuff, g_StrTable[_T("MallTrade14")], pItem->bySmallGroupDiscount);	
		strTips = strTips + szBuff + _T("\\n");
	}
	if( pItem->byMediumGroupDiscount != (BYTE)GT_INVALID )
	{
		_stprintf(szBuff, g_StrTable[_T("MallTrade15")], pItem->byMediumGroupDiscount);
		strTips = strTips + szBuff + _T("\\n");
	}
	if( pItem->byBigGroupDiscount != (BYTE)GT_INVALID )
	{
		_stprintf(szBuff, g_StrTable[_T("MallTrade16")], pItem->byBigGroupDiscount);
		strTips = strTips + szBuff + _T("\\n");
	}
	ShowTip( m_pStcGBItemState[nGoodsPos][nStatePos], strTips.c_str() );
}

void GroupBuyFrame::ShowPresentProTips( const tagMallItemProtoBase* pItem, int nGoodsPos, int nStatePos )
{
	TCHAR szBuff[X_LONG_NAME];
	if( pItem->dwPresentID != GT_INVALID )  
	{
		const tagItemDisplayInfo* pInfo = ItemProtoData::Inst()->FindItemDisplay(pItem->dwPresentID);
		if( P_VALID(pInfo) )
		{
			_stprintf( szBuff, _T("%s x %d"), pInfo->szName, pItem->byPresentNum );
			tstring strTips = g_StrTable[_T("TipsDefaultFont")];
			strTips += szBuff;
			ShowTip( m_pStcGBItemState[nGoodsPos][nStatePos], strTips.c_str() );
		}
	}
}

void GroupBuyFrame::ShowNumProTips( const tagMallItemProtoBase* pItem, int nGoodsPos, int nStatePos )
{
	TCHAR szBuff[X_LONG_NAME];
	tstring strTips;
	if( pItem->byNum != (BYTE)GT_INVALID )
	{
		_stprintf( szBuff, g_StrTable[_T("MallTrade17")], pItem->byNum );
		strTips = g_StrTable[_T("TipsDefaultFont")];
		strTips += szBuff;
	}
	else 
	{
		_stprintf( szBuff, g_StrTable[_T("MallTrade18")] );
		strTips = g_StrTable[_T("TipsDefaultFont")];
		strTips += szBuff;
	}
	ShowTip( m_pStcGBItemState[nGoodsPos][nStatePos], strTips.c_str() );
}

void GroupBuyFrame::ShowExchangeProTips( const tagMallItemProtoBase* pItem, int nGoodsPos, int nStatePos )
{
	TCHAR szBuff[X_LONG_NAME];
	if( pItem->byExNum!=0 && pItem->byExNum!=(BYTE)GT_INVALID )
	{

		_stprintf( szBuff, g_StrTable[_T("MallTrade19")], pItem->byExNum );
		tstring strTips = g_StrTable[_T("TipsDefaultFont")];
		strTips += szBuff;
		ShowTip( m_pStcGBItemState[nGoodsPos][nStatePos], strTips.c_str() );
	}
}

bool GroupBuyFrame::JudgeResponseGroupBuy()
{
	//判断是否响应自己发起的团购
	DWORD dwRoleID = m_seleDate.GetData2();
	
	if( dwRoleID == RoleMgr::Inst()->GetLocalPlayerID() )
	{
		if( P_VALID(GetObj("ResponseSelf_MsgBox")) )
			KillObj("ResponseSelf_MsgBox");
		CreateObj("ResponseSelf_MsgBox", "MsgBoxEx");

		TObjRef<MsgBoxEx>("ResponseSelf_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade39")], 
			_T("ResponseSelf_Event"), MsgBoxFlag(MBF_OK), TRUE);
		return false;
	}
	else 
	{
		INT64 n64Tmp = dwRoleID; 
		INT64 n64Key = ( n64Tmp << 32 ) | m_seleDate.GetData1();
		const tagGroupPurchase* pGP = MallMgr::Inst()->GetGroupBuyInfoByID( n64Key );
		if( P_VALID(pGP) && pGP->nParticipatorNum==pGP->nRequireNum)
		{
			if( P_VALID(GetObj("ResponseNumberFull_MsgBox")) )
				KillObj("ResponseNumberFull_MsgBox");
			CreateObj("ResponseNumberFull_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("ResponseNumberFull_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade41")], 
				_T("ResponseNumberFull_Event"), MsgBoxFlag(MBF_OK), TRUE);
			return false;	
		}
	}

	return true;
}

void GroupBuyFrame::ListResponse()
{
	if( m_listRespons.Empty() )
		return;
	tstring strRoleName;
	m_listRespons.ResetIterator();
	DWORD dwResID = GT_INVALID;
	int i=0;
	while(m_listRespons.PeekNext(dwResID))
	{
		if( IS_PLAYER( dwResID ) )
		{
			strRoleName = PlayerNameTab::Inst()->FindNameByID(dwResID);
			if( !strRoleName.empty() )
			{
				m_pLBoxResponse->SetText( i, 0, strRoleName.c_str() );
				++i;
			}
		}
	}
}