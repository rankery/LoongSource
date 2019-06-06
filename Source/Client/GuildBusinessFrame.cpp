#include "stdafx.h"
#include "IconStaticEx.h"
#include "GuildBusinessFrame.h"
#include "GuildDefine.h"
#include "..\WorldDefine\guild_define.h"
#include "GuildMgr.h"
#include "CombatSysUtil.h"
#include "ToolTipCreator.h"
#include "ToolTipFrame.h"
#include "RoleMgr.h"
#include "QuestMgr.h"

GuildBusinessFrame::GuildBusinessFrame()
	: m_Trunk(this)
	, m_pWnd(NULL)
{
	for (int i = 0; i < COMMERCE_NUMBER; ++i)
	{
		m_pCommerceBack[i] = NULL;
		m_pCommerceItem[i] = NULL;
		m_pStcCommerceName[i] = NULL;
		m_pStcCommercePrice[i] = NULL;
	}
	for (int i = 0; i < GOODS_NUMBER; ++i)
	{
		m_pGoodsBack[i] = NULL;
		m_pGoodsItem[i] = NULL;
		m_pStcGoodsName[i] = NULL;
		m_pStcGoodsPrice[i] = NULL;
	}
	m_pStcSilver = NULL;
	m_pStcPurpose = NULL;
	m_pBtnClose = NULL;
	m_pBtnBuy = NULL;
	m_pBtnSell = NULL;
	m_pStcCOCDesc = NULL;
	m_bShow = false;
	m_dwCurrentChooseGoodID = GT_INVALID;
	m_dwCurrentChooseCommerceID = GT_INVALID;
	m_dwTime = 0;
}

GuildBusinessFrame::~GuildBusinessFrame()
{
	if (P_VALID(m_pWnd))
	{
		m_pGUI->AddToDestroyList(m_pWnd);
		m_pWnd = NULL;
	}

}

BOOL GuildBusinessFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	if (!GameFrame::Init(pMgr, pFather, dwParam))
		return FALSE;
	
	m_pGameFrameMgr->RegisterEventHandle(_T("Refresh_COC"), (FRAMEEVENTPROC)m_Trunk.sfp1(&GuildBusinessFrame::OnEventRefreshCOC));
	m_pGameFrameMgr->RegisterEventHandle(_T("Refresh_Commodity"), (FRAMEEVENTPROC)m_Trunk.sfp1(&GuildBusinessFrame::OnEventRefreshCommodity));
	m_pGameFrameMgr->RegisterEventHandle(_T("EventBuyMsgInputBox"), (FRAMEEVENTPROC)m_Trunk.sfp1(&GuildBusinessFrame::OnEventBuyMsgInputBox));
	m_pGameFrameMgr->RegisterEventHandle(_T("EventSellMsgInputBox"), (FRAMEEVENTPROC)m_Trunk.sfp1(&GuildBusinessFrame::OnEventSellMsgInputBox));
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName(), (VUIEVENTHANDLE)m_Trunk.sfp1(&GuildBusinessFrame::EventHandler));
	return TRUE;
}

BOOL GuildBusinessFrame::Destroy()
{
	m_pGameFrameMgr->UnRegisterEventHandler(_T("Refresh_COC"), (FRAMEEVENTPROC)m_Trunk.sfp1(&GuildBusinessFrame::OnEventRefreshCOC));
	m_pGameFrameMgr->UnRegisterEventHandler(_T("Refresh_Commodity"), (FRAMEEVENTPROC)m_Trunk.sfp1(&GuildBusinessFrame::OnEventRefreshCommodity));
	m_pGameFrameMgr->UnRegisterEventHandler(_T("EventBuyMsgInputBox"), (FRAMEEVENTPROC)m_Trunk.sfp1(&GuildBusinessFrame::OnEventBuyMsgInputBox));
	m_pGameFrameMgr->UnRegisterEventHandler(_T("EventSellMsgInputBox"), (FRAMEEVENTPROC)m_Trunk.sfp1(&GuildBusinessFrame::OnEventSellMsgInputBox));
	m_pGUI->UnRegisterEventHandler(m_pWnd->GetFullName(), (VUIEVENTHANDLE)m_Trunk.sfp1(&GuildBusinessFrame::EventHandler));
	return GameFrame::Destroy();
}

BOOL GuildBusinessFrame::ReloadUI()
{
	tstring strPath = vEngine::g_strLocalPath + _T("\\ui\\runforbusiness.xml");
	XmlElement ele;
	m_pGUI->LoadXml(&ele, "VFS_System", strPath.c_str());

	for (int i = 0; i < COMMERCE_NUMBER; ++i)
	{
		std::string strItemBackPath = "rfb_win\\rfb_pic\\commerceback\\pic%d\\imgback%d";
		char buffer[512];
		_snprintf(buffer, 512, strItemBackPath.c_str(), i, i);
		m_pGUI->ChangeXml(&ele, buffer, "ClassName", "IconStaticEx");
		std::string strItemPath = "rfb_win\\rfb_pic\\commerceback\\pic%d\\imgback%d\\img%d";
		_snprintf(buffer, 512, strItemPath.c_str(), "ClassName", "IconStaticEx");
		m_pGUI->ChangeXml(&ele, buffer, "ClassName", "IconStaticEx");
	}

	for (int i = 0; i < GOODS_NUMBER; ++i)
	{
		std::string strItemBackPath = "rfb_win\\rfb_pic\\goodsback\\pic%d\\imgback%d";
		char buffer[512];
		_snprintf(buffer, 512, strItemBackPath.c_str(), i, i);
		m_pGUI->ChangeXml(&ele, buffer, "ClassName", "IconStaticEx");
		std::string strItemPath = "rfb_win\\rfb_pic\\goodsback\\pic%d\\imgback%d\\img%d";
		_snprintf(buffer, 512, strItemPath.c_str(), "ClassName", "IconStaticEx");
		m_pGUI->ChangeXml(&ele, buffer, "ClassName", "IconStaticEx");
	}

	m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &ele);
	
	m_pBtnClose = (GUIButton*)m_pWnd->GetChild(_T("rfb_pic\\close"));
	m_pBtnBuy = (GUIButton*)m_pWnd->GetChild(_T("rfb_pic\\buybutt"));
	m_pBtnSell = (GUIButton*)m_pWnd->GetChild(_T("rfb_pic\\sellbutt"));

	m_pCommerceBack[9] = (IconStaticEx*)m_pWnd->GetChild(_T("rfb_pic\\commerceback\\pic0\\imgback0"));
	m_pCommerceItem[9] = (IconStaticEx*)m_pWnd->GetChild(_T("rfb_pic\\commerceback\\pic0\\imgback0\\img0"));
	m_pStcCommerceName[9] = (GUIStatic*)m_pWnd->GetChild(_T("rfb_pic\\commerceback\\pic0\\name0"));
	m_pStcCommercePrice[9] = (GUIStatic*)m_pWnd->GetChild(_T("rfb_pic\\commerceback\\pic0\\num0"));
	for (int i = 1; i < COMMERCE_NUMBER; ++i)
	{
		tstring strItemBackPath = _T("rfb_pic\\commerceback\\pic%d\\imgback%d");
		TCHAR buffer[512];
		_sntprintf(buffer, 512, strItemBackPath.c_str(), i, i);
		m_pCommerceBack[i - 1] = (IconStaticEx*)m_pWnd->GetChild(buffer);
		tstring strItemPath = _T("rfb_pic\\commerceback\\pic%d\\imgback%d\\img%d");
		_sntprintf(buffer, 512, strItemPath.c_str(), i, i, i);
		m_pCommerceItem[i - 1] = (IconStaticEx*)m_pWnd->GetChild(buffer);
		tstring strNamePath = _T("rfb_pic\\commerceback\\pic%d\\name%d");
		_sntprintf(buffer, 512, strNamePath.c_str(), i, i);
		m_pStcCommerceName[i - 1] = (GUIStatic*)m_pWnd->GetChild(buffer);
		tstring strPricePath = _T("rfb_pic\\commerceback\\pic%d\\num%d");
		_sntprintf(buffer, 512, strPricePath.c_str(), i, i);
		m_pStcCommercePrice[i - 1] = (GUIStatic*)m_pWnd->GetChild(buffer);
	}

	m_pGoodsBack[9] = (IconStaticEx*)m_pWnd->GetChild(_T("rfb_pic\\goodsback\\pic0\\imgback0"));
	m_pGoodsItem[9] = (IconStaticEx*)m_pWnd->GetChild(_T("rfb_pic\\goodsback\\pic0\\imgback0\\img0"));
	m_pStcGoodsName[9] = (GUIStatic*)m_pWnd->GetChild(_T("rfb_pic\\goodsback\\pic0\\name0"));
	m_pStcGoodsPrice[9] = (GUIStatic*)m_pWnd->GetChild(_T("rfb_pic\\goodsback\\pic0\\num0"));
	for (int i = 1; i < GOODS_NUMBER; ++i)
	{
		tstring strItemBackPath = _T("rfb_pic\\goodsback\\pic%d\\imgback%d");
		TCHAR buffer[512];
		_sntprintf(buffer, 512, strItemBackPath.c_str(), i, i);
		m_pGoodsBack[i - 1] = (IconStaticEx*)m_pWnd->GetChild(buffer);
		tstring strItemPath = _T("rfb_pic\\goodsback\\pic%d\\imgback%d\\img%d");
		_sntprintf(buffer, 512, strItemPath.c_str(), i, i, i);
		m_pGoodsItem[i - 1] = (IconStaticEx*)m_pWnd->GetChild(buffer);
		tstring strNamePath = _T("rfb_pic\\goodsback\\pic%d\\name%d");
		_sntprintf(buffer, 512, strNamePath.c_str(), i, i);
		m_pStcGoodsName[i - 1] = (GUIStatic*)m_pWnd->GetChild(buffer);
		tstring strPricePath = _T("rfb_pic\\goodsback\\pic%d\\num%d");
		_sntprintf(buffer, 512, strPricePath.c_str(), i, i);
		m_pStcGoodsPrice[i - 1] = (GUIStatic*)m_pWnd->GetChild(buffer);
	}
	m_pStcCOCDesc = (GUIStatic*)m_pWnd->GetChild(_T("rfb_pic\\commerceback\\word"));
	m_pStcSilver = (GUIStatic*)m_pWnd->GetChild(_T("rfb_pic\\goodsback\\moneynum"));
	m_pStcPurpose = (GUIStatic*)m_pWnd->GetChild(_T("rfb_pic\\goodsback\\linenum"));
	m_pBtnBuy->SetEnable(false);
	m_pBtnSell->SetEnable(false);

	// 设为不可见
	Show(false);
	return TRUE;
}

BOOL GuildBusinessFrame::EscCancel()
{
	if (P_VALID(m_pWnd) && !m_pWnd->IsInvisible())
	{
		m_pGameFrameMgr->SendEvent(&tagGameEvent(_T("Close_COC"), this));
		return TRUE;
	}
	return GameFrame::EscCancel();
}

bool GuildBusinessFrame::IsShow() const
{
	return m_bShow;
}

void GuildBusinessFrame::Show(bool bShow)
{
	m_bShow = bShow;
	if (bShow)
		m_pWnd->SetInvisible(FALSE);
	else
		m_pWnd->SetInvisible(TRUE);
}


DWORD GuildBusinessFrame::EventHandler(tagGUIEvent *pEvent)
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	switch (pEvent->eEvent)
	{
	case EGUIE_DragStart:
		{
			for (int i = 0; i < GOODS_NUMBER; ++i)
			{
				if (pWnd == m_pGoodsItem[i])
				{
					OnCommodityBagClick(i);
					break;
				}
			}
			for (int i = 0; i < COMMERCE_NUMBER; ++i)
			{
				if (pWnd == m_pCommerceItem[i])
				{
					OnCOCBagClick(i);
					break;
				}
			}
		} break;
	case EGUIE_MoveIntoCtrl:
		{
			for (int i = 0; i < GOODS_NUMBER; ++i)
			{
				if (pWnd == m_pGoodsItem[i])
				{
					const std::map<DWORD, tagCommerceGoodInfo>& GoodInfo = GuildMgr::Inst()->GetCurrentPlayerGoods();
					std::map<DWORD, tagCommerceGoodInfo>::const_iterator end = GoodInfo.end();
					std::map<DWORD, tagCommerceGoodInfo>::const_iterator iter = GoodInfo.begin();

					int n = 0;
					for (; n < i; ++n, ++iter)
					{
						if (iter == end)
						{
							return 0;
						}
					}
					if (iter == end)
					{
						return 0;
					}
					DWORD dwID = iter->second.dwGoodID;
					ShowTip(pWnd, ToolTipCreator::Inst()->GetCommerceGoodsTips(dwID).c_str());
					break;
				}
			}
			for (int i = 0; i < COMMERCE_NUMBER; ++i)
			{
				if (pWnd == m_pCommerceItem[i])
				{
					const std::map<DWORD, tagCommerceGoodInfo>& GoodInfo = GuildMgr::Inst()->GetCurrentCOCGoods();
					std::map<DWORD, tagCommerceGoodInfo>::const_iterator end = GoodInfo.end();
					std::map<DWORD, tagCommerceGoodInfo>::const_iterator iter = GoodInfo.begin();
					int n = 0;
					for (; n < i; ++n, ++iter)
					{
						if (iter == end)
						{
							return 0;
						}
					}
					if (iter == end)
					{
						return 0;
					}
					DWORD dwID = iter->second.dwGoodID;
					ShowTip(pWnd, ToolTipCreator::Inst()->GetCommerceGoodsTips(dwID).c_str());
					break;
				}
			}
		}
		break;
	case EGUIE_MoveOutOfCtrl:
		{
			ShowTip(pWnd, NULL);
		}
		break;
	case EGUIE_Click:
		{
			if (pWnd == m_pBtnClose)
			{
				m_pGameFrameMgr->SendEvent(&tagGameEvent(_T("Close_COC"), this));
				break;
			}
			else if (pWnd == m_pBtnBuy)
			{
				// 如果该特产商会所属城市不是被玩家的帮派所拥有
				BYTE byCity = GuildMgr::Inst()->GetCurrentCOCHoldCity();
				if (GuildMgr::Inst()->IsBeOwned(byCity) == false)
				{
					CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("GuildCommerceError_Code13")]);
					return 0;
				}

				const std::map<DWORD, tagCommerceGoodInfo>& GoodInfo = GuildMgr::Inst()->GetCurrentCOCGoods();
				std::map<DWORD, tagCommerceGoodInfo>::const_iterator end = GoodInfo.end();
				std::map<DWORD, tagCommerceGoodInfo>::const_iterator iter = GoodInfo.find(m_dwCurrentChooseCommerceID);
				if (iter == end)
					return 0;

				// 计算最大可购买的商品数量
				int nSilver = GuildMgr::Inst()->GetCurrentOwnCommerceSilver();
				int nMaxValue = nSilver / iter->second.nCost;


				if (P_VALID(GetObj("BuyCommerceInput")))
					KillObj("BuyCommerceInput");
				CreateObj("BuyCommerceInput", "MsgInputBox");
				TObjRef<MsgInputBox>("BuyCommerceInput")->Init(_T(""), g_StrTable[_T("GuildBuyNum")], _T("EventBuyMsgInputBox"), nMaxValue, MBIT_Number);
				return 0;
			}
			else if (pWnd == m_pBtnSell)
			{
				// 如果是特产商会，不能出售货物
				BYTE byCity = GuildMgr::Inst()->GetCurrentCOCHoldCity();
				if (byCity != 0)
				{
					CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("GuildCommerceError_Code14")]);
					return 0;
				}
				const std::map<DWORD, tagCommerceGoodInfo>& GoodInfo = GuildMgr::Inst()->GetCurrentPlayerGoods();
				std::map<DWORD, tagCommerceGoodInfo>::const_iterator end = GoodInfo.end();
				std::map<DWORD, tagCommerceGoodInfo>::const_iterator iter = GoodInfo.find(m_dwCurrentChooseGoodID);
				if (iter == end)
					return 0;
				int nMaxValue = (int)iter->second.byGoodNum;
				if (P_VALID(GetObj("SellCommerceInput")))
					KillObj("SellCommerceInput");
				CreateObj("SellCommerceInput", "MsgInputBox");
				TObjRef<MsgInputBox>("SellCommerceInput")->Init(_T(""), g_StrTable[_T("GuildSellNum")], _T("EventSellMsgInputBox"), nMaxValue, MBIT_Number);
				return 0;
			}
		}
	}
	return 0;
}

DWORD GuildBusinessFrame::OnEventRefreshCOC(tagGameEvent *pEvent)
{
	SetDefaultCOCBagUI();
	const std::map<DWORD, tagCommerceGoodInfo>& GoodInfo = GuildMgr::Inst()->GetCurrentCOCGoods();
	std::map<DWORD, tagCommerceGoodInfo>::const_iterator end = GoodInfo.end();
	int n = 0;
	for (std::map<DWORD, tagCommerceGoodInfo>::const_iterator iter = GoodInfo.begin(); iter != end; ++iter)
	{
		DWORD dwID = iter->second.dwGoodID;
		const tagGoodInfo *pInfo = GuildMgr::Inst()->FindGoodInfo(dwID);
		if (P_VALID(pInfo))
		{
			tstring strIconPath = _T("data\\ui\\Icon\\%s.tga");
			TCHAR buffer[X_LONG_NAME] = {0};
			_sntprintf(buffer, X_LONG_NAME, strIconPath.c_str(), pInfo->strIconPath.c_str());
			m_pCommerceItem[n]->SetPic(buffer);
			_sntprintf(buffer, X_LONG_NAME, _T("%d"), (int)iter->second.byGoodNum);
			m_pCommerceItem[n]->DrawText(buffer);
			m_pStcCommerceName[n]->SetText(pInfo->strName.c_str());
			_sntprintf(buffer, X_LONG_NAME, g_StrTable[_T("CommerceSilverText")], iter->second.nCost);
			m_pStcCommercePrice[n]->SetText(buffer);
		}
		n++;
	}
	DWORD dwCOCID = GuildMgr::Inst()->GetCurrentCOCID();
	const tagCOCInfo *pInfo = GuildMgr::Inst()->FindCOCInfo(dwCOCID);
	if (P_VALID(pInfo))
		m_pStcCOCDesc->SetText(pInfo->strDesc.c_str());
	return 0;
}

DWORD GuildBusinessFrame::OnEventRefreshCommodity(tagGameEvent *pEvent)
{
	SetDefaultCommodityBagUI();
	const std::map<DWORD, tagCommerceGoodInfo>& GoodInfo = GuildMgr::Inst()->GetCurrentPlayerGoods();
	std::map<DWORD, tagCommerceGoodInfo>::const_iterator end = GoodInfo.end();
	int n = 0;
	for (std::map<DWORD, tagCommerceGoodInfo>::const_iterator iter = GoodInfo.begin(); iter != end; ++iter)
	{
		DWORD dwID = iter->second.dwGoodID;
		const tagGoodInfo *pInfo = GuildMgr::Inst()->FindGoodInfo(dwID);
		if (P_VALID(pInfo))
		{
			tstring strIconPath = _T("data\\ui\\Icon\\%s.tga");
			TCHAR buffer[X_LONG_NAME] = {0};
			_sntprintf(buffer, X_LONG_NAME, strIconPath.c_str(), pInfo->strIconPath.c_str());
			m_pGoodsItem[n]->SetPic(buffer);
			_sntprintf(buffer, X_LONG_NAME, _T("%d"), (int)iter->second.byGoodNum);
			m_pGoodsItem[n]->DrawText(buffer);
			m_pStcGoodsName[n]->SetText(pInfo->strName.c_str());
			_sntprintf(buffer, X_LONG_NAME, g_StrTable[_T("CommerceSilverText")], iter->second.nCost);
			m_pStcGoodsPrice[n]->SetText(buffer);
		}
		n++;
	}
	INT32 nCommerceSilver = GuildMgr::Inst()->GetCurrentOwnCommerceSilver();
	INT nLevel = GuildMgr::Inst()->GetRunForBusinessLevel();
	// 获取最大商银和指标商银
	const tagCommerceInfo *pCInfo = GuildMgr::Inst()->FindCommerceInfo(nLevel);
	if (!P_VALID(pCInfo))
		return 0;
	TCHAR buffer[256];
	_sntprintf(buffer, 256, _T("%d/%d"), nCommerceSilver, pCInfo->nMaxTael);
	m_pStcSilver->SetText(buffer);
	_sntprintf(buffer, 256, _T("%d"), pCInfo->nPurposeTael);
	m_pStcPurpose->SetText(buffer);
	return 0;
}

VOID GuildBusinessFrame::SetDefaultCOCBagUI()
{
	for (int n = 0; n < COMMERCE_NUMBER; ++n)
	{
		m_pCommerceItem[n]->SetPic(strEmpty);
		m_pCommerceItem[n]->DrawText(_T(""));
		m_pStcCommerceName[n]->SetText(_T(""));
		m_pStcCommercePrice[n]->SetText(_T(""));
	}
}

VOID GuildBusinessFrame::SetDefaultCommodityBagUI()
{
	for (int n = 0; n < GOODS_NUMBER; ++n)
	{
		m_pGoodsItem[n]->SetPic(strEmpty);
		m_pGoodsItem[n]->DrawText(_T(""));
		m_pStcGoodsName[n]->SetText(_T(""));
		m_pStcGoodsPrice[n]->SetText(_T(""));
	}
}

VOID GuildBusinessFrame::SetDefaultCOCBagBackUI()
{
	for (int n = 0; n < COMMERCE_NUMBER; ++n)
	{
		m_pCommerceBack[n]->SetPic(strBackEmpty);
	}
}

VOID GuildBusinessFrame::SetDefaultCommodityBagBackUI()
{
	for (int n = 0; n < GOODS_NUMBER; ++n)
	{
		m_pGoodsBack[n]->SetPic(strBackEmpty);
	}
}


VOID GuildBusinessFrame::OnCOCBagClick(int n)
{
	SetDefaultCOCBagBackUI();
	SetDefaultCommodityBagBackUI();
	const std::map<DWORD, tagCommerceGoodInfo>& GoodInfo = GuildMgr::Inst()->GetCurrentCOCGoods();
	std::map<DWORD, tagCommerceGoodInfo>::const_iterator end = GoodInfo.end();
	std::map<DWORD, tagCommerceGoodInfo>::const_iterator iter = GoodInfo.begin();
	int i = 0;
	for (; i < n; ++i,++iter)
	{
		if (iter == end)
		{
			m_pBtnBuy->SetEnable(false);
			m_pBtnSell->SetEnable(false);
			return;
		}

	}
	if (iter == end)
	{
		m_pBtnBuy->SetEnable(false);
		m_pBtnSell->SetEnable(false);
		return;
	}
	m_dwCurrentChooseCommerceID = iter->second.dwGoodID;
	m_pCommerceBack[n]->SetPic(strBackChoose);
	m_pBtnBuy->SetEnable(true);
	m_pBtnSell->SetEnable(false);
}

VOID GuildBusinessFrame::OnCommodityBagClick(int n)
{
	SetDefaultCOCBagBackUI();
	SetDefaultCommodityBagBackUI();
	

	const std::map<DWORD, tagCommerceGoodInfo>& GoodInfo = GuildMgr::Inst()->GetCurrentPlayerGoods();
	std::map<DWORD, tagCommerceGoodInfo>::const_iterator end = GoodInfo.end();
	std::map<DWORD, tagCommerceGoodInfo>::const_iterator iter = GoodInfo.begin();

	int i = 0;
	for (; i < n; ++i, ++iter)
	{
		if (iter == end)
		{
			m_pBtnBuy->SetEnable(false);
			m_pBtnSell->SetEnable(false);
			return;
		}
	}
	if (iter == end)
	{
		m_pBtnBuy->SetEnable(false);
		m_pBtnSell->SetEnable(false);
		return;
	}
	m_dwCurrentChooseGoodID = iter->second.dwGoodID;
	m_pGoodsBack[n]->SetPic(strBackChoose);
	m_pBtnBuy->SetEnable(false);
	m_pBtnSell->SetEnable(true);
}

DWORD GuildBusinessFrame::OnEventBuyMsgInputBox(tagMsgInputBoxEvent *pEvent)
{
	if(MBF_OK == pEvent->eResult && MBIT_Number == pEvent->eInputType)
	{
		INT32 nSilver = GuildMgr::Inst()->GetCurrentOwnCommerceSilver();
		const std::map<DWORD, tagCommerceGoodInfo>& GoodInfo = GuildMgr::Inst()->GetCurrentCOCGoods();
		std::map<DWORD, tagCommerceGoodInfo>::const_iterator end = GoodInfo.end();
		std::map<DWORD, tagCommerceGoodInfo>::const_iterator iter = GoodInfo.find(m_dwCurrentChooseCommerceID);
		if (iter == end)
			return 0;
		if (pEvent->nInputNum <= 0)
			return 0;

		INT32 nMoney = iter->second.nCost * pEvent->nInputNum;
		if (nMoney > nSilver)
		{
			CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("GuildError_Code104")]);
			return 0;
		}
		GuildMgr::Inst()->SendBuyCOCGoods(GuildMgr::Inst()->GetCurrentTalkCOCNPCID(), m_dwCurrentChooseCommerceID, (BYTE)pEvent->nInputNum);
	}
	return 0;
}

DWORD GuildBusinessFrame::OnEventSellMsgInputBox(tagMsgInputBoxEvent *pEvent)
{
	if(MBF_OK == pEvent->eResult && MBIT_Number == pEvent->eInputType)
	{
		if (pEvent->nInputNum <= 0)
			return 0;
		GuildMgr::Inst()->SendSellCOCGoods(GuildMgr::Inst()->GetCurrentTalkCOCNPCID(), m_dwCurrentChooseGoodID, (BYTE)pEvent->nInputNum);
	}
	return 0;
}

VOID GuildBusinessFrame::Update()
{
	m_dwTime += Kernel::Inst()->GetDeltaTimeDW();
	if( m_dwTime > 1000 )//每隔1秒检测距离目标NPC的距离
	{
		m_dwTime = 0;

		if (RoleMgr::Inst()->IsOutValidDist( QuestMgr::Inst()->GetTalkNPCGlobalID() ) )
		{
			m_pGameFrameMgr->SendEvent(&tagGameEvent(_T("Close_COC"), this));
		}
	}

	GameFrame::Update();

}