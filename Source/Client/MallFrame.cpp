#include "stdafx.h"
#include "MallFrame.h"
#include "NetSession.h"
#include "IconStatic.h"
#include "ItemButton.h"
#include "PlayerTryDressWnd.h"
#include "RoleMgr.h"
#include "LocalPlayer.h"
#include "ToolTipCreator.h"
#include "ToolTipFrame.h"
#include "..\WorldDefine\ItemDefine.h"
#include "ItemProtoData.h"
#include "MallMgr.h"
#include "CurrencyMgr.h"
#include "MallData.h"
#include "MallGiftFrame.h"
#include "PlayerNameTab.h"
#include "CombatSysUtil.h"
#include "ItemProtoData.h"
#include "ServerTime.h"
#include "YuanbaoTradeMgr.h"
#include "GroupBuyFrame.h"
#include "shellapi.h"
#include "HttpPicEx.h"
#include "SaveAccount.h"

const TCHAR *szPromoteHave	= _T("data\\ui\\mall\\L_cu.bmp");
const TCHAR *szPromoteNot	= _T("data\\ui\\mall\\L_cu_daizhong.bmp");
const TCHAR *szGroupHave	= _T("data\\ui\\mall\\L_tuan.bmp");
const TCHAR *szGroupNot		= _T("data\\ui\\mall\\L_tuan_daizhong.bmp");
const TCHAR *szPresentHave	= _T("data\\ui\\mall\\L_zeng.bmp");
const TCHAR *szPresentNot	= _T("data\\ui\\mall\\L_zeng_daizhong.bmp");
const TCHAR *szNumHave		= _T("data\\ui\\mall\\L_xian.bmp");
const TCHAR *szNumNot		= _T("data\\ui\\mall\\L_xian_daizhong.bmp");
const TCHAR *szExchangeHove	= _T("data\\ui\\mall\\L_dui.bmp");
const TCHAR *szExchangeNot	= _T("data\\ui\\mall\\L_dui_daizhong.bmp");

const TCHAR *szSelectItem	= _T("data\\ui\\Common\\L_icon-l.bmp");
const TCHAR *szSelectPack	= _T("data\\ui\\mall\\L_kuangjia-d-L.bmp");

#define MAX_PRESENT_ITEM_TIME 2000

MallFrame::MallFrame( void ):m_Trunk(this)
{
	m_pWnd					= NULL;
	m_pWndCaption			= NULL;		
	m_pWndTryDress			= NULL;							
	m_pStcYuanBao			= NULL;	
	m_pExchangeVolume		= NULL;
	m_pPBtnGroupBuy			= NULL;
	m_pBtnClose				= NULL;							
	m_pPBtnYuanBaoTrade		= NULL;						
	m_pBtnYuanBaoAbundance	= NULL;					
	m_pBtnReset				= NULL;							

	m_pStcFetchBack			= NULL;
	m_pStcFetchSel			= NULL;							
	m_pFetchUnitItem				= NULL;							
	m_pBtnFetch				= NULL;							
	m_pBtnFetchTitle		= NULL;		
	m_pStcGoodsBack			= NULL;
	m_pStcGoodsBackEx		= NULL;
	for(int i=0; i<MALL_MAXITEM_PERPAGE; i++)
	{
		m_pUnitItemBack[i]	= NULL;
		m_pUnitItemSel[i]	= NULL;
		m_pUnitItem[i]		= NULL;
		m_pStcItemName[i]	= NULL;
		m_pStcInfo[i]		= NULL;
		for(int j=0; j<MALL_ITEM_STATE; j++)
		{
			m_pStcState[i][j] = NULL;
		}
	}
	m_pBtnPDItem			= NULL;
	m_pBtnPUItem			= NULL;
	m_pStcCPItem			= NULL;
	m_pBtnPDPack			= NULL;
	m_pBtnPUPack			= NULL;
	m_pBtnCPPack			= NULL;
	m_pBtnBuy				= NULL;
	m_pBtnPresent			= NULL;
	m_pBtnGroupBuy			= NULL;
	m_pBtnExchange			= NULL;
	m_pStcProBack			= NULL;
	for(int i=0; i<MALL_MAX_PROMOTE_ITEM; i++)
	{
		m_pProItemBack[i]	= NULL;
		m_pUnitProItemSel[i] = NULL;
		m_pUnitProItem[i]	= NULL;
		m_pProItemName[i]	= NULL;
		m_pProItmeInfo[i]	= NULL;
		for(int j=0; j<MALL_ITEM_STATE; j++)
		{
			m_pProItemState[i][j] = NULL;
		}
	}
	m_pPBtnAll				= NULL;
	m_pPBtnPackage			= NULL;
	for(int i=0; i<MALL_ITEM_SUBTYPE; i++)
	{
		m_pPBtnSubMall[i]	= NULL;
	}
	m_pPBtnMall				= NULL;
	m_dwSelectItemID		= GT_INVALID;
	m_n16BuyNum				= GT_INVALID;
	m_bSelectPromote		= false;
	m_pStcGroupBuyBack		= NULL;
	m_eOperateState			= EMIOS_NULL;
	m_pWndNotice			= NULL;
	m_pHttp_MallNotice		= NULL;
}

MallFrame::~MallFrame( void )
{

}

BOOL MallFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam );

	//--注册游戏事件
	m_pMgr->RegisterEventHandle(_T("BuyMallItemNum_Event"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&MallFrame::OnBuyMallItemInputEvent));
	m_pMgr->RegisterEventHandle(_T("BuyMallItemPromoteNum_Event"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&MallFrame::OnBuyMallPromoteItemInputEvet));
	m_pMgr->RegisterEventHandle(_T("AffirmBuyMallItem_Event"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&MallFrame::OnAffirmBuyMallItemEvent));
	m_pMgr->RegisterEventHandle(_T("tagMsgMallGiftEvent"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&MallFrame::OnMsgMallGiftEvent));
	m_pMgr->RegisterEventHandle(_T("Bag_Yuanbao"),					(FRAMEEVENTPROC)m_Trunk.sfp1(&MallFrame::OnBagYuanbaoChange));
	m_pMgr->RegisterEventHandle(_T("tagExchangeVolumeChange"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&MallFrame::OnExchangeVolumeChange));
	m_pMgr->RegisterEventHandle(_T("ExchangePackItemNum_Event"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&MallFrame::OnExchangePackItem));
	m_pMgr->RegisterEventHandle(_T("ExchangeMallItemNum_Event"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&MallFrame::OnExchangeMallItem));
	
	m_pMgr->RegisterEventHandle(_T("AffirmExchangePackItem_Event"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&MallFrame::OnAffirmExchangePackItem));
	m_pMgr->RegisterEventHandle(_T("AffirmExchangeMallItem_Event"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&MallFrame::OnAffirmExchangeMallItem));


	//--注册网络消息
	m_pCmdMgr->Register("NS_MallBuyItem",			(NETMSGPROC)m_Trunk.sfp2(&MallFrame::OnNS_MallBuyItem),		_T("NS_MallBuyItem"));
	m_pCmdMgr->Register("NS_MallFreeGetItem",		(NETMSGPROC)m_Trunk.sfp2(&MallFrame::OnNS_MallFreeGetItem),	_T("NS_MallFreeGetItem"));
	m_pCmdMgr->Register("NS_MallPresentItem",		(NETMSGPROC)m_Trunk.sfp2(&MallFrame::OnNS_MallPresentItem),	_T("NS_MallPresentItem"));
	m_pCmdMgr->Register("NS_MallBuyPack",			(NETMSGPROC)m_Trunk.sfp2(&MallFrame::OnNS_MallBuyPack),		_T("NS_MallBuyPack"));
	m_pCmdMgr->Register("NS_MallPresentPack",		(NETMSGPROC)m_Trunk.sfp2(&MallFrame::OnNS_MallPresentPack),	_T("NS_MallPresentPack"));
	m_pCmdMgr->Register("NS_MallItemExchange",		(NETMSGPROC)m_Trunk.sfp2(&MallFrame::OnNS_MallItemExchange),_T("NS_MallItemExchange"));
	m_pCmdMgr->Register("NS_MallPackExchange",		(NETMSGPROC)m_Trunk.sfp2(&MallFrame::OnNS_MallPackExchange),_T("NS_MallPackExchange"));
	

	return bRet;
}

BOOL MallFrame::Destroy()
{
	BOOL bRet = GameFrame::Destroy();

	//--注销游戏事件
	m_pMgr->UnRegisterEventHandler(_T("BuyMallItemNum_Event"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&MallFrame::OnBuyMallItemInputEvent));
	m_pMgr->UnRegisterEventHandler(_T("BuyMallItemPromoteNum_Event"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&MallFrame::OnBuyMallPromoteItemInputEvet));
	m_pMgr->UnRegisterEventHandler(_T("AffirmBuyMallItem_Event"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&MallFrame::OnAffirmBuyMallItemEvent));
	m_pMgr->UnRegisterEventHandler(_T("tagMsgMallGiftEvent"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&MallFrame::OnMsgMallGiftEvent));
	m_pMgr->UnRegisterEventHandler(_T("Bag_Yuanbao"),					(FRAMEEVENTPROC)m_Trunk.sfp1(&MallFrame::OnBagYuanbaoChange));
	m_pMgr->UnRegisterEventHandler(_T("tagExchangeVolumeChange"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&MallFrame::OnExchangeVolumeChange));
	m_pMgr->UnRegisterEventHandler(_T("ExchangePackItemNum_Event"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&MallFrame::OnExchangePackItem));
	m_pMgr->UnRegisterEventHandler(_T("ExchangeMallItemNum_Event"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&MallFrame::OnExchangeMallItem));
	m_pMgr->UnRegisterEventHandler(_T("AffirmExchangePackItem_Event"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&MallFrame::OnAffirmExchangePackItem));
	m_pMgr->UnRegisterEventHandler(_T("AffirmExchangeMallItem_Event"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&MallFrame::OnAffirmExchangeMallItem));

	//--注销网络消息
	m_pCmdMgr->UnRegister("NS_MallBuyItem",			(NETMSGPROC)m_Trunk.sfp2(&MallFrame::OnNS_MallBuyItem));
	m_pCmdMgr->UnRegister("NS_MallFreeGetItem",		(NETMSGPROC)m_Trunk.sfp2(&MallFrame::OnNS_MallFreeGetItem));
	m_pCmdMgr->UnRegister("NS_MallPresentItem",		(NETMSGPROC)m_Trunk.sfp2(&MallFrame::OnNS_MallPresentItem));
	m_pCmdMgr->UnRegister("NS_MallBuyPack",			(NETMSGPROC)m_Trunk.sfp2(&MallFrame::OnNS_MallBuyPack));
	m_pCmdMgr->UnRegister("NS_MallPresentPack",		(NETMSGPROC)m_Trunk.sfp2(&MallFrame::OnNS_MallPresentPack));
	m_pCmdMgr->UnRegister("NS_MallItemExchange",	(NETMSGPROC)m_Trunk.sfp2(&MallFrame::OnNS_MallItemExchange));
	m_pCmdMgr->UnRegister("NS_MallPackExchange",	(NETMSGPROC)m_Trunk.sfp2(&MallFrame::OnNS_MallPackExchange));


	//--防止出错
	if( P_VALID(GetObj("BuyMallItemNum_MsgBox")) )
		KillObj("BuyMallItemNum_MsgBox");
	if( P_VALID(GetObj("BuyMallPromoteItemNum_MsgBox")) )
		KillObj("BuyMallPromoteItemNum_MsgBox");
	if( P_VALID(GetObj("AffirmBuyMallItem_MsgBox")) )
		KillObj("AffirmBuyMallItem_MsgBox");
	if( P_VALID(GetObj("NotBuyMallItem_MsgBox")) )
		KillObj("NotBuyMallItem_MsgBox");
	if( P_VALID(GetObj("NotGiftMallItem_MsgBox")) )
		KillObj("NotGiftMallItem_MsgBox");
	if( P_VALID(GetObj("LauncherSucess_MsgBox")) )
		KillObj("LauncherSucess_MsgBox");
	if( P_VALID(GetObj("LauncherError_MsgBox")) )
		KillObj("LauncherError_MsgBox");
	if( P_VALID(GetObj("ExchangeMallItemNum_MsgBox")) )
		KillObj("ExchangeMallItemNum_MsgBox");
	if( P_VALID(GetObj("AffirmExchangePackItem_MsgBox")) )
		KillObj("AffirmExchangePackItem_MsgBox");
	if( P_VALID(GetObj("AffirmExchangeMallItem_MsgBox")) )
		KillObj("AffirmExchangeMallItem_MsgBox");

	if(P_VALID(m_pWnd))
	{
		m_pGUI->UnRegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&MallFrame::EventHandler));
		m_pGUI->AddToDestroyList(m_pWnd);
		m_pWnd = NULL;
	}
	return bRet;
}

BOOL MallFrame::EscCancel()
{
	if( m_pWnd->IsInvisible() )
		return FALSE;

	tagGameEvent event(_T("Close_Mall_UI"), this);
	m_pFrameMgr->SendEvent(&event);
	return TRUE;
}

BOOL MallFrame::ReloadUI()
{
	XmlElement ele;
	tstring strPath = g_strLocalPath + _T("\\ui\\mall.xml");
	m_pGUI->LoadXml(&ele, "VFS_System", strPath.c_str());

	//试穿窗口
	m_pGUI->ChangeXml(&ele, "mallwin\\mallbackpic\\fittingroom", "ClassName", "PlayerTryDressWnd");
	m_pGUI->ChangeXml(&ele, "mallwin\\mallbackpic\\freeback\\itempic", "ClassName", "IconStatic");
	m_pGUI->ChangeXml(&ele, "mallwin\\mallbackpic\\freeback\\itempic\\item", "ClassName", "ItemButton");
	m_pGUI->ChangeXml(&ele, "mallwin\\mallbackpic\\adv_back\\adv_pic", "ClassName", "HttpPicEx");
	//商品区
	for(int i=0; i<MALL_MAXITEM_PERPAGE; i++)
	{
		CHAR szFullName[X_LONG_NAME] = {0};
		_snprintf(szFullName, sizeof(szFullName), "mallwin\\mallbackpic\\mallback\\mallshow\\itemback%d\\itempic", i+1);
		m_pGUI->ChangeXml(&ele, szFullName, "ClassName", "IconStatic");
		_snprintf(szFullName, sizeof(szFullName), "mallwin\\mallbackpic\\mallback\\mallshow\\itemback%d\\itempic\\item", i+1);
		m_pGUI->ChangeXml(&ele, szFullName, "ClassName", "MallButton");
		for(int j=0; j<MALL_ITEM_STATE; j++)
		{
			_snprintf(szFullName, sizeof(szFullName), "mallwin\\mallbackpic\\mallback\\mallshow\\itemback%d\\pic%d", i+1, j+1);
			m_pGUI->ChangeXml(&ele, szFullName, "ClassName", "IconStatic");
		}
	}
	//推荐区
	for(int i=0; i<MALL_MAX_PROMOTE_ITEM; i++)
	{
		CHAR szFullName[X_LONG_NAME] = {0};
		_snprintf(szFullName, sizeof(szFullName), "mallwin\\mallbackpic\\mallshow1\\itemback%d\\itempic", i+1);
		m_pGUI->ChangeXml(&ele, szFullName, "ClassName", "IconStatic");
		_snprintf(szFullName, sizeof(szFullName), "mallwin\\mallbackpic\\mallshow1\\itemback%d\\itempic\\item", i+1);
		m_pGUI->ChangeXml(&ele, szFullName, "ClassName", "MallButton");
		for(int j=0; j<MALL_ITEM_STATE; j++)
		{
			_snprintf(szFullName, sizeof(szFullName), "mallwin\\mallbackpic\\mallshow1\\itemback%d\\pic%d", i+1, j+1);
			m_pGUI->ChangeXml(&ele, szFullName, "ClassName", "IconStatic");
		}
	}
	//礼包
	for(int i=0; i<MALL_MAXPACK_PERPAGE; i++)
	{
		CHAR szFullName[X_LONG_NAME] = {0};
		_snprintf(szFullName, sizeof(szFullName), "mallwin\\mallbackpic\\mallback\\packagebackpic\\package%d", i+1);
		m_pGUI->ChangeXml(&ele, szFullName, "ClassName", "IconStatic");
		_snprintf(szFullName, sizeof(szFullName), "mallwin\\mallbackpic\\mallback\\packagebackpic\\package%d\\packagename", i+1);
		m_pGUI->ChangeXml(&ele, szFullName, "ClassName", "IconStatic");
		_snprintf(szFullName, sizeof(szFullName), "mallwin\\mallbackpic\\mallback\\packagebackpic\\package%d\\packageprice", i+1);
		m_pGUI->ChangeXml(&ele, szFullName, "ClassName", "IconStatic");
		for(int j=0; j<MALL_PACKPRO_NUM; j++)
		{
			_snprintf(szFullName, sizeof(szFullName), "mallwin\\mallbackpic\\mallback\\packagebackpic\\package%d\\item%d", i+1, j+1);
			m_pGUI->ChangeXml(&ele, szFullName, "ClassName", "IconStatic");
			_snprintf(szFullName, sizeof(szFullName), "mallwin\\mallbackpic\\mallback\\packagebackpic\\package%d\\pic%d", i+1, j+1);
			m_pGUI->ChangeXml(&ele, szFullName, "ClassName", "IconStatic");			
			_snprintf(szFullName, sizeof(szFullName), "mallwin\\mallbackpic\\mallback\\packagebackpic\\package%d\\item%d\\itembutt", i+1, j+1);
			m_pGUI->ChangeXml(&ele, szFullName, "ClassName", "MallButton");
		}
	}
	//团购
	for( int i=0; i<Max_GBInfo_PerPage; ++i )
	{
		CHAR szFullName[X_LONG_NAME] = {0};
		_snprintf( szFullName, X_LONG_NAME, "mallwin\\mallbackpic\\groupbuy_page\\itemlist\\itemback%d\\itempic", i+1 );
		m_pGUI->ChangeXml(&ele, szFullName, "ClassName", "IconStatic");
		_snprintf( szFullName, X_LONG_NAME, "mallwin\\mallbackpic\\groupbuy_page\\itemlist\\itemback%d\\itempic\\item", i+1 );
		m_pGUI->ChangeXml(&ele, szFullName, "ClassName", "MallButton");
		for( int j=0; j<Max_Item_Pro; ++j )
		{
			_snprintf( szFullName, X_LONG_NAME, "mallwin\\mallbackpic\\groupbuy_page\\itemlist\\itemback%d\\pic%d", i+1, j+1 );
			m_pGUI->ChangeXml(&ele, szFullName, "ClassName", "IconStatic");
		}
	}

	// 创建
	m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &ele);
	m_pWndCaption = m_pWnd->GetChild(_T("mallbackpic"));
	m_pWndNotice = (GUIStatic*)m_pWndCaption->GetChild(_T("adv_back"));
	m_pWndNotice->SetInvisible(TRUE);
	m_pWndTryDress = (PlayerTryDressWnd*)m_pWndCaption->GetChild(_T("fittingroom"));
	m_pBtnClose	= (GUIButton*)m_pWndCaption->GetChild(_T("closebutt"));
	m_pPBtnYuanBaoTrade = (GUIPushButton*)m_pWndCaption->GetChild(_T("mallfra\\trade"));
	m_pBtnYuanBaoAbundance = (GUIButton*)m_pWndCaption->GetChild(_T("tradebutt0"));
	//m_pBtnYuanBaoAbundance->SetEnable(false);
	m_pStcYuanBao = (GUIStatic*)m_pWndCaption->GetChild(_T("moneynum"));
	m_pExchangeVolume = (GUIStatic*)m_pWndCaption->GetChild(_T("moneynum2"));
	m_pPBtnGroupBuy = (GUIPushButton*)m_pWndCaption->GetChild(_T("mallfra\\groupbuy"));
	m_pBtnReset = (GUIButton*)m_pWndCaption->GetChild(_T("fittingroom\\refresh"));
	m_pPBtnMall = (GUIPushButton*)m_pWndCaption->GetChild(_T("mallfra\\commodities"));
	m_pPBtnNotice = (GUIPushButton*)m_pWndCaption->GetChild(_T("mallfra\\post"));
	m_pHttp_MallNotice = (HttpPicEx*)m_pWndNotice->GetChild(_T("adv_pic"));

	m_pStcFetchBack = (GUIStatic*)m_pWndCaption->GetChild(_T("freeback"));
	m_pStcFetchSel = (IconStatic*)m_pWndCaption->GetChild(_T("freeback\\itempic"));
	m_pFetchUnitItem = (ItemButton*)m_pWndCaption->GetChild(_T("freeback\\itempic\\item"));
	m_pBtnFetch = (GUIButton*)m_pWndCaption->GetChild(_T("freeback\\freeget"));
	m_pBtnFetchTitle = (GUIStatic*)m_pWndCaption->GetChild(_T("freeback\\itemname"));

	m_pPBtnAll = (GUIPushButton*)m_pWndCaption->GetChild(_T("mallback\\fra\\all"));
	m_pPBtnAll->SetText(g_StrTable[_T("MallTrade0")]);
	m_pPBtnPackage = (GUIPushButton*)m_pWndCaption->GetChild(_T("mallback\\fra\\package"));
	m_pPBtnPackage->SetText(g_StrTable[_T("MallTrade1")]);

	m_pStcGoodsBack = (GUIStatic*)m_pWndCaption->GetChild(_T("mallback"));
    m_pStcYuanbaoBack = (GUIStatic*)m_pWndCaption->GetChild(_T("tradeback"));
	m_pStcGroupBuyBack = (GUIStatic*)m_pWndCaption->GetChild(_T("groupbuy_page"));
	m_pStcGroupBuyBack->SetInvisible(TRUE);
	for(int i=0; i<MALL_ITEM_SUBTYPE; i++)
	{
		TCHAR szFullName[X_LONG_NAME] = {0};
		_sntprintf(szFullName, sizeof(szFullName), _T("fra\\page%d"), i+1);
		m_pPBtnSubMall[i] = (GUIPushButton*)m_pStcGoodsBack->GetChild(szFullName);
		tagMallTitleData* pData = MallData::Inst()->FindMallTitleData(i);
		if( !P_VALID(pData) )
			continue;
		if( pData->strTitleName!=_T("") )
		{
			m_pPBtnSubMall[i]->SetText(pData->strTitleName.c_str());
			m_pPBtnSubMall[i]->SetInvisible(FALSE);
		}
		else
		{
			m_pPBtnSubMall[i]->SetInvisible(TRUE);
		}
	}

	m_pStcGoodsBackEx = (GUIStatic*)m_pWndCaption->GetChild(_T("mallback\\mallshow"));
	m_pStcGoodsBackEx->SetInvisible(TRUE);
	m_pBtnPDItem	= (GUIButton*)m_pStcGoodsBackEx->GetChild(_T("pageback0"));
	m_pBtnPUItem = (GUIButton*)m_pStcGoodsBackEx->GetChild(_T("pageback"));
	m_pStcCPItem = (GUIButton*)m_pStcGoodsBackEx->GetChild(_T("pagenum"));
	m_pBtnBuy = (GUIButton*)m_pStcGoodsBack->GetChild(_T("buy"));
	m_pBtnPresent = (GUIButton*)m_pStcGoodsBack->GetChild(_T("gift"));
	m_pBtnGroupBuy = (GUIButton*)m_pStcGoodsBack->GetChild(_T("wholesale"));
	m_pBtnExchange = (GUIButton*)m_pStcGoodsBack->GetChild(_T("exchange"));
	m_pBtnExchange->SetEnable(false);

	for(int i=0; i<MALL_MAXITEM_PERPAGE; i++)
	{
		TCHAR szFullName[X_LONG_NAME] = {0};
		_sntprintf(szFullName, sizeof(szFullName), _T("mallshow\\itemback%d"), i+1);
		m_pUnitItemBack[i] = (GUIStatic*)m_pStcGoodsBack->GetChild(szFullName);
		_sntprintf(szFullName, sizeof(szFullName), _T("mallshow\\itemback%d\\itempic"), i+1);
		m_pUnitItemSel[i] = (IconStatic*)m_pStcGoodsBack->GetChild(szFullName);
		_sntprintf(szFullName, sizeof(szFullName), _T("mallshow\\itemback%d\\itempic\\item"), i+1);
		m_pUnitItem[i] = (MallButton*)m_pStcGoodsBack->GetChild(szFullName);
		_sntprintf(szFullName, sizeof(szFullName), _T("mallshow\\itemback%d\\name"), i+1);
		m_pStcItemName[i] = (GUIStatic*)m_pStcGoodsBack->GetChild(szFullName);
		_sntprintf(szFullName, sizeof(szFullName), _T("mallshow\\itemback%d\\price"), i+1);
		m_pStcInfo[i] = (GUIStatic*)m_pStcGoodsBack->GetChild(szFullName);
		for(int j=0; j<MALL_ITEM_STATE; j++)
		{
			_sntprintf(szFullName, sizeof(szFullName), _T("mallshow\\itemback%d\\pic%d"), i+1, j+1);
			m_pStcState[i][j] = (IconStatic*)m_pStcGoodsBack->GetChild(szFullName);
		}
	}

	m_pStcProBack = (GUIStatic*)m_pWndCaption->GetChild(_T("mallshow1"));
	for(int i=0; i<MALL_MAX_PROMOTE_ITEM; i++)
	{
		TCHAR szFullName[X_LONG_NAME] = {0};
		_sntprintf(szFullName, sizeof(szFullName), _T("itemback%d"), i+1);
		m_pProItemBack[i] = (GUIStatic*)m_pStcProBack->GetChild(szFullName);
		_sntprintf(szFullName, sizeof(szFullName), _T("itemback%d\\itempic"), i+1);
		m_pUnitProItemSel[i] = (IconStatic*)m_pStcProBack->GetChild(szFullName);
		_sntprintf(szFullName, sizeof(szFullName), _T("itemback%d\\itempic\\item"), i+1);
		m_pUnitProItem[i] = (MallButton*)m_pStcProBack->GetChild(szFullName);
		_sntprintf(szFullName, sizeof(szFullName), _T("itemback%d\\name"), i+1);
		m_pProItemName[i] = (GUIStatic*)m_pStcProBack->GetChild(szFullName);
		_sntprintf(szFullName, sizeof(szFullName), _T("itemback%d\\price"), i+1);
		m_pProItmeInfo[i] = (GUIStatic*)m_pStcProBack->GetChild(szFullName);
		for(int j=0; j<MALL_ITEM_STATE; j++)
		{
			_sntprintf(szFullName, sizeof(szFullName), _T("itemback%d\\pic%d"), i+1, j+1);
			m_pProItemState[i][j] = (IconStatic*)m_pStcProBack->GetChild(szFullName);
		}

	}

	m_pStcPackBack = (GUIStatic*)m_pWndCaption->GetChild(_T("mallback\\packagebackpic"));
	m_pStcPackBack->SetInvisible(TRUE);
	m_pBtnPDPack = (GUIButton*)m_pStcPackBack->GetChild(_T("pageback0"));
	m_pBtnPUPack = (GUIButton*)m_pStcPackBack->GetChild(_T("pageback"));
	m_pBtnCPPack = (GUIStatic*)m_pStcPackBack->GetChild(_T("pagenum"));
	for(int i=0; i<MALL_MAXPACK_PERPAGE; i++)
	{
		TCHAR szFullName[X_LONG_NAME] = {0};
		_sntprintf(szFullName, sizeof(szFullName), _T("package%d"), i+1);
		m_pUnitPackBack[i] = (IconStatic*)m_pStcPackBack->GetChild(szFullName);
		_sntprintf(szFullName, sizeof(szFullName), _T("package%d\\packageprice"), i+1);
		m_pStcPackInfo[i] = (IconStatic*)m_pStcPackBack->GetChild(szFullName);
		_sntprintf(szFullName, sizeof(szFullName), _T("package%d\\packagename"), i+1);
		m_pStcPackName[i] =(IconStatic*)m_pStcPackBack->GetChild(szFullName);
		_sntprintf(szFullName, sizeof(szFullName), _T("package%d\\new"), i+1);
		m_pStcPackNew[i] = (GUIStatic*)m_pStcPackBack->GetChild(szFullName);
		_sntprintf(szFullName, sizeof(szFullName), _T("package%d\\hot"), i+1);
		m_pStcPackHot[i] = (GUIStatic*)m_pStcPackBack->GetChild(szFullName);
		for(int j=0; j<MALL_PACKPRO_NUM; j++)
		{
			_sntprintf(szFullName, sizeof(szFullName), _T("package%d\\item%d"), i+1, j+1);
			m_pUnitPackSel[i][j] = (IconStatic*)m_pStcPackBack->GetChild(szFullName);
			_sntprintf(szFullName, sizeof(szFullName), _T("package%d\\item%d\\itembutt"), i+1, j+1);
			m_pUnitPack[i][j] = (MallButton*)m_pStcPackBack->GetChild(szFullName);
			_sntprintf(szFullName, sizeof(szFullName), _T("package%d\\pic%d"), i+1, j+1);
			m_pStcPackState[i][j] = (IconStatic*)m_pStcPackBack->GetChild(szFullName);
		}
	}

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&MallFrame::EventHandler));

	m_pStcGoodsBack->SetInvisible(TRUE);

	m_pWnd->SetInvisible(TRUE);

    // 创建元宝交易frame
    MallMgr::Inst()->CreateYuanBaoFrame(m_strName.c_str());

	// 创建团购frame
	MallMgr::Inst()->CreateGroupBuyFrame(m_strName.c_str());

    // 默认显示商城而非元宝界面
    m_pPBtnMall->SetState(EGUIBS_PushDown);

	SetOperateState();

	return TRUE;
}

void MallFrame::UpdateAvatar()
{
	LocalPlayer* pLp = RoleMgr::Inst()->GetLocalPlayer();
	if(P_VALID(pLp) && P_VALID(m_pWndTryDress))
	{
		m_pWndTryDress->UpdataAvatar( pLp );
	}
}

VOID MallFrame::Render3D()
{
	if(P_VALID(m_pWndTryDress))
		m_pWndTryDress->Render3D();
	GameFrame::Render3D();
}

VOID MallFrame::Update()
{
	if( m_vecGiftEvent.size() == 0 )
		return;
	for(vector<tagMsgMallGiftEvent>::iterator it=m_vecGiftEvent.begin();
		it!=m_vecGiftEvent.end(); )
	{
		if(timeGetTime() - it->dwTime >= MAX_PRESENT_ITEM_TIME)
		{
			tagMsgMallGiftEvent evt  = (*it);
			OnMsgMallGiftEvent(&evt);
			it = m_vecGiftEvent.erase(it);
			break;
		}
		else
			++it;
	}
}

DWORD MallFrame::EventHandler( tagGUIEvent* pEvent )
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	switch (pEvent->eEvent)
	{
	case EGUIE_Click:
		{
			//关闭
			if( pWnd == m_pBtnClose )
			{
				GroupBuyFrame* pFrame = MallMgr::Inst()->GetGroupBuyFrame();
				if( P_VALID(pFrame) )
				{
					pFrame->ShowWnd();
				}
				tagGameEvent event(_T("Close_Mall_UI"), this);
				m_pFrameMgr->SendEvent(&event);
			}
			//下一页
			else if( pWnd==m_pBtnPDItem || pWnd==m_pBtnPDPack )
			{
				PageDown();
				RefreshMallItemFlag();
			}
			//上一页
			else if( pWnd==m_pBtnPUItem || pWnd==m_pBtnPUPack )
			{
				PageUp();
				RefreshMallItemFlag();
			}
			//重置试穿
			else if( pWnd == m_pBtnReset )
			{
				if( P_VALID(m_pWndTryDress) )
				{
					m_pWndTryDress->RotateYaw(0.0f);
					m_pWndTryDress->ReLoadTryEquip();
				}
			}
			//元宝冲值
			else if( pWnd == m_pBtnYuanBaoAbundance )
			{
				YuanBaoAbundance();
			}
			else if( pWnd == m_pBtnGroupBuy  )
			{
				GroupBuyFrame* pFrame = MallMgr::Inst()->GetGroupBuyFrame();
				if( P_VALID(pFrame) )
				{
					pFrame->ShowWnd(GroupBuyFrame::EGroupBuy_Purchase);
				}
			}
			else if( pWnd == m_pBtnExchange )
			{
				ExchangeMallItem();
			}	
			//购买
			else if( pWnd == m_pBtnBuy )
			{
				if( m_bSelectPromote )
					BuyMallItemPromote();
				else
					BuyMallItem();
			}
			//赠送
			else if( pWnd == m_pBtnPresent )
			{
				if( m_dwSelectItemID == GT_INVALID )
					return 0;
				MallMgr::Inst()->CreateMallGiftFrame();
				this->SetPresentMaxNum();
			}
			//领取
			else if( pWnd == m_pBtnFetch )
			{
				FreeGetMallItem();
			}
			//右键单击,购买商品
			else if (_tcsnccmp(pWnd->GetName().c_str(), _T("item"), 4) == 0)
			{
				for(int i=0; i<MALL_MAXITEM_PERPAGE; i++)
				{
					if( pWnd == m_pUnitItem[i] )
					{
						SetHopeBuyItemID(m_pUnitItem[i]->GetBtnData().GetData1());
						m_pUnitItemSel[i]->SetPic(szSelectItem);
						m_bSelectPromote = false;
						BuyMallItem();
						return 0;
					}
				}
				for(int i=0; i<MALL_MAX_PROMOTE_ITEM; i++)
				{
					if( pWnd == m_pUnitProItem[i] )
					{
						SetHopeBuyItemID(m_pUnitProItem[i]->GetBtnData().GetData1());
						m_pUnitProItemSel[i]->SetPic(szSelectItem);
						m_bSelectPromote = true;
						BuyMallItemPromote();
						return 0;
					}
				}
				for( int i=0; i<MALL_MAXPACK_PERPAGE; i++ )
				{
					for(int j=0; j<MALL_PACKPRO_NUM; j++)
					{
						if( pWnd == m_pUnitPack[i][j] )
						{
							SetHopeBuyItemID(m_pUnitPack[i][j]->GetBtnData().GetData1());
							m_pUnitPackBack[i]->SetPic(szSelectPack);
							m_bSelectPromote = false;
							BuyMallItem();
							return 0;
						}
					}
				}
				if( pWnd == m_pFetchUnitItem )
				{
					m_pStcFetchSel->SetPic(szSelectItem);
					SetHopeBuyItemID(m_pFetchUnitItem->GetBtnData().GetData1());
					m_bSelectPromote = false;
					FreeGetMallItem();
					return 0;
				}
			}
			else if(_tcsnccmp(pWnd->GetName().c_str(), _T("package"), 7) == 0)
			{
				RefreshMallItemFlag();
				for( int i=0; i<MALL_MAXPACK_PERPAGE; i++ )
				{
					if( pWnd==m_pUnitPackBack[i] || pWnd==m_pStcPackName[i] || pWnd==m_pStcPackInfo[i] )
					{
						SetHopeBuyItemID(m_pUnitPack[i][0]->GetBtnData().GetData1());
						m_pUnitPackBack[i]->SetPic(szSelectPack);
						m_bSelectPromote = false;
						BuyMallItem();
						return 0;
					}
				}
			}
		}
		break;
	case EGUIE_Drag:
		{
			//左键单击,选中商品
			if(_tcsnccmp(pWnd->GetName().c_str(), _T("item"), 4) == 0)
			{
				RefreshMallItemFlag();
				for(int i=0; i<MALL_MAXITEM_PERPAGE; i++)
				{
					if( pWnd == m_pUnitItem[i] )
					{
						BtnData data = m_pUnitItem[i]->GetBtnData();
						SetHopeBuyItemID(data.GetData1());
						m_pUnitItemSel[i]->SetPic(szSelectItem);
						JudgeTryDress(data.GetData2());
						m_bSelectPromote = false;
						SetOperateState(EMIOS_NormalItem);
						return 0;
					}
				}
				for(int i=0; i<MALL_MAX_PROMOTE_ITEM; i++)
				{
					if( pWnd == m_pUnitProItem[i] )
					{
						BtnData data = m_pUnitProItem[i]->GetBtnData();
						SetHopeBuyItemID(data.GetData1());
						m_pUnitProItemSel[i]->SetPic(szSelectItem);
						JudgeTryDress(data.GetData2());
						m_bSelectPromote = true;
						SetOperateState(EMIOS_Promote);
						return 0;
					}
				}
				for( int i=0; i<MALL_MAXPACK_PERPAGE; i++ )
				{
					if( pWnd==m_pUnitPackBack[i] || pWnd==m_pStcPackName[i])
					{
						SetHopeBuyItemID(m_pUnitPack[i][0]->GetBtnData().GetData1());
						m_pUnitPackBack[i]->SetPic(szSelectPack);
						m_bSelectPromote = false;
						SetOperateState(EMIOS_PackItem);
						return 0;
					}

					for(int j=0; j<MALL_PACKPRO_NUM; j++)
					{
						if( pWnd == m_pUnitPack[i][j] )
						{
							SetHopeBuyItemID(m_pUnitPack[i][j]->GetBtnData().GetData1());
							m_pUnitPackBack[i]->SetPic(szSelectPack);
							m_bSelectPromote = false;
							SetOperateState(EMIOS_PackItem);
							return 0;
						}
					}
				}
				if( pWnd == m_pFetchUnitItem )
				{
					m_pStcFetchSel->SetPic(szSelectItem);
					SetHopeBuyItemID(m_pFetchUnitItem->GetBtnData().GetData1());
					m_bSelectPromote = false;
					SetOperateState(EMIOS_Free);
					return 0;
				}
			}
			else if( _tcsnccmp(pWnd->GetName().c_str(), _T("package"), 7) == 0 )
			{
				RefreshMallItemFlag();
				for( int i=0; i<MALL_MAXPACK_PERPAGE; i++ )
				{
					if( pWnd==m_pUnitPackBack[i] || pWnd==m_pStcPackName[i] || pWnd==m_pStcPackInfo[i] )
					{
						SetHopeBuyItemID(m_pUnitPack[i][0]->GetBtnData().GetData1());
						m_pUnitPackBack[i]->SetPic(szSelectPack);
						m_bSelectPromote = false;
						SetOperateState(EMIOS_PackItem);
						return 0;
					}
				}
			}
		}
		break;
	case EGUIE_PushButtonDown:
		{
			if( pEvent->dwParam1 != 1 )
				return 0;
			SetOperateState();
			//商品区
			if( pWnd == m_pPBtnMall )
			{
				RefreshMallItemFlag();
				m_pStcGoodsBack->SetInvisible(FALSE);
                m_pStcYuanbaoBack->SetInvisible(TRUE);
				m_pWndNotice->SetInvisible(TRUE);
				//团购隐藏
				GroupBuyFrame* pFrame = MallMgr::Inst()->GetGroupBuyFrame();
				if( P_VALID(pFrame) )
				{
					pFrame->ShowWnd();
				}			
			}
			//元宝交换区
			else if( pWnd == m_pPBtnYuanBaoTrade )
			{
				RefreshMallItemFlag();
				m_pStcGoodsBack->SetInvisible(TRUE);
				//元宝交易显示s
                m_pStcYuanbaoBack->SetInvisible(FALSE);
				m_pWndNotice->SetInvisible(TRUE);
                YuanbaoTradeMgr::Inst()->BeginNC_SynYBTradeInfo();
				//团购隐藏
				GroupBuyFrame* pFrame = MallMgr::Inst()->GetGroupBuyFrame();
				if( P_VALID(pFrame) )
				{
					pFrame->ShowWnd();
				}
			}
			//公告区
			else if( pWnd == m_pPBtnNotice )
			{
				//隐藏其他3个区
				m_pStcGoodsBack->SetInvisible(TRUE);
				m_pStcYuanbaoBack->SetInvisible(TRUE);				
				GroupBuyFrame* pFrame = MallMgr::Inst()->GetGroupBuyFrame();
				if( P_VALID(pFrame) )
				{
					pFrame->ShowWnd();
				}
				ShowMallNotice(TRUE);
			}
			//团购区
			else if( pWnd == m_pPBtnGroupBuy )
			{
				RefreshMallItemFlag();
				m_pStcGoodsBack->SetInvisible(TRUE);
				m_pStcYuanbaoBack->SetInvisible(TRUE);
				m_pWndNotice->SetInvisible(TRUE);
				//响应团购
				GroupBuyFrame* pFrame = MallMgr::Inst()->GetGroupBuyFrame();
				if( P_VALID(pFrame) )
				{
					pFrame->ShowWnd(GroupBuyFrame::EGroupBuy_Info);
				}

			}
			else if( pWnd == m_pPBtnAll )
			{
				JudgeShowMallItem(EMIT_All);
				RefreshMallItemFlag();
			}
			else if( pWnd == m_pPBtnPackage )
			{
				JudgeShowMallItem(EMIT_Package);
				RefreshMallItemFlag();
			}
			else 
			{
				if (_tcsnccmp(pWnd->GetName().c_str(), _T("page"), 4) == 0)
				{
					for(int i=0; i<MALL_ITEM_SUBTYPE; i++)
					{
						if( pWnd == m_pPBtnSubMall[i] )
						{
							JudgeShowMallItem(i);
							RefreshMallItemFlag();
						}
					}
				}
			}
		}
		break;
	case EGUIE_MoveIntoCtrl:
		{
			if( _tcsnccmp(pWnd->GetName().c_str(), _T("pic"), 3) == 0 )
			{
				for(int i=0; i<MALL_MAXITEM_PERPAGE; ++i)
				{
					for(int j=0; j<MALL_ITEM_STATE; ++j)
					{
						if( pWnd == m_pStcState[i][j] )
						{
							ShowMallGoodsTips(i, j);
							return 0;
						}
					}
				}
				
				for( int i=0; i<MALL_MAX_PROMOTE_ITEM; ++i )
				{
					for( int j=0; j<MALL_ITEM_STATE; ++j )
					{
						if( pWnd == m_pProItemState[i][j] )
						{
							ShowMallPromoteGoodsTips(i, j);
							return 0;
						}
					}
				}

				for( int i=0; i<MALL_MAXPACK_PERPAGE; i++ )
				{
					for( int j=0; j<MALL_PACKPRO_NUM; j++ )
					{
						if( pWnd == m_pStcPackState[i][j] )
						{
							ShowMallPackTips(i, j);
							return 0;
						}
					}
				}
				
			}

			if( _tcsnccmp(pWnd->GetName().c_str(), _T("item"), 4) == 0 )
			{
				for( int i=0; i<MALL_MAXITEM_PERPAGE; ++i )
				{	
					if( pWnd == m_pUnitItem[i] )  
					{
						DWORD dwID =  m_pUnitItem[i]->GetBtnData().GetData2();
						ShowTip(m_pUnitItem[i], ToolTipCreator::Inst()->GetItemTips(dwID).c_str());
						return 0;
					}
				}

				for( int i=0; i<MALL_MAX_PROMOTE_ITEM; ++i )
				{
					if( pWnd == m_pUnitProItem[i] )
					{
						DWORD dwID =  m_pUnitProItem[i]->GetBtnData().GetData2();
						ShowTip(m_pUnitProItem[i], ToolTipCreator::Inst()->GetItemTips(dwID).c_str());
						return 0;
					}
				}

				for( int i=0; i<MALL_MAXPACK_PERPAGE; i++ )
				{
					for( int j=0; j<MALL_PACKPRO_NUM; j++ )
					{
						if( pWnd == m_pUnitPack[i][j] )
						{
							DWORD dwID =  m_pUnitPack[i][j]->GetBtnData().GetData2();
							ShowTip(m_pUnitPack[i][j], ToolTipCreator::Inst()->GetItemTips(dwID).c_str());
							return 0;
						}
					}
				}

				if( pWnd == m_pFetchUnitItem ) 
				{
					DWORD dwID = m_pFetchUnitItem->GetBtnData().GetData1();
					ShowTip(m_pFetchUnitItem, ToolTipCreator::Inst()->GetItemTips(dwID).c_str());
					return 0;
				}
			}
		}
		break;
	case EGUIE_MoveOutOfCtrl:
		{
			ShowTip(NULL, _T(""));
		}
		break;
	}
	return 1;
}

void MallFrame::RefreshMallItemFlag()
{
	tstring strPic = _T("data\\ui\\common\\l_icon.bmp");
	tstring strPicPack = _T("data\\ui\\mall\\L_kuangjia-d.bmp");
	for(int i=0; i<MALL_MAXITEM_PERPAGE; i++)
	{
		m_pUnitItemSel[i]->SetPic(strPic);
	}
	for(int i=0; i<MALL_MAX_PROMOTE_ITEM; i++)
	{
		m_pUnitProItemSel[i]->SetPic(strPic);
	}
	for(int i=0; i<MALL_MAXPACK_PERPAGE; i++)
	{
		m_pUnitPackBack[i]->SetPic(strPicPack);
	}
	m_pStcFetchSel->SetPic(strPic);
	m_dwSelectItemID = GT_INVALID;
}

void MallFrame::SetYuanBao()
{ 
	if(P_VALID(m_pStcYuanBao))
	{
		TCHAR szBuff[X_SHORT_NAME] = {0};
		_itot(CurrencyMgr::Inst()->GetBagYuanbao(), szBuff, 10);
		m_pStcYuanBao->SetText(szBuff) ;
	}
}

void MallFrame::SetExchangeVolume()
{
	if( P_VALID(m_pExchangeVolume) ) 
	{
		TCHAR szBuff[X_SHORT_NAME] = {0};
		_itot(CurrencyMgr::Inst()->GetExchangeVolume(), szBuff, 10);
		m_pExchangeVolume->SetText(szBuff);
	}
}

void MallFrame::ClassifiedMallItem()
{
	//分类时先clear
	for(int i=0; i<MALL_ITEM_SUBTYPE; i++)
	{
		m_mapMallItem[i].Clear();
	}

	tagMallItemProto* item = NULL;
	TMap<DWORD, tagMallItemProto*> mapitem = MallMgr::Inst()->GetMallProto();
	mapitem.ResetIterator();
	while(mapitem.PeekNext(item))
	{
		if( !P_VALID(item)
			|| (item->n8Kind-1)>MALL_ITEM_SUBTYPE 
			|| item->n8Kind==0
			|| (item->dwID==GT_INVALID || item->dwID==0) )
			continue;
		m_mapMallItem[item->n8Kind-1].Add(item->dwID, item);
	}
	
	//计算每个分类下的商品共个有多少页
	for(int i=0; i<MALL_ITEM_SUBTYPE; i++)
	{
		m_nCurMaxPage[i] = m_mapMallItem[i].Size() / MALL_MAXITEM_PERPAGE;
	}

	//
	this->RefreshPromoteItem();
	this->RefreshFreeItem();
	this->SetYuanBao();
	this->SetExchangeVolume();
	m_pPBtnMall->SetState(EGUIBS_PushDown);
	m_pPBtnAll->SetState(EGUIBS_PushDown);
	LocalPlayer* pLocal = RoleMgr::Inst()->GetLocalPlayer();
	if( P_VALID(pLocal)
		&& !pLocal->IsInGuild() )
	{
		m_pPBtnGroupBuy->SetEnable(false);
		m_pBtnGroupBuy->SetEnable(false);
	}
	m_pWnd->FlipToTop();
}

void MallFrame::ShowCurPageMallItemByType( INT nPage )
{
	HideCtrlMallItem();
	m_nCurPage[m_nCurMenu] = nPage;
	TMap<DWORD, tagMallItemProto*>::TMapIterator it = m_mapMallItem[m_nCurMenu].Begin();
	int i=0; 
	for(; i<m_nCurPage[m_nCurMenu]*MALL_MAXITEM_PERPAGE; ++i)
	{
		++it;
		if( i > m_mapMallItem[m_nCurMenu].Size() )
			break;
	}
	i = 0;
	tagMallItemProto* pProto = NULL;
	DWORD pos = 0;
	TCHAR szBuff[X_LONG_NAME] = {0};
	while(i<MALL_MAXITEM_PERPAGE && m_mapMallItem[m_nCurMenu].PeekNext(it, pos, pProto))
	{
		if( P_VALID(pProto) )
		{
			const tagItemProto* pItem = ItemProtoData::Inst()->FindItemProto(pProto->dwTypeID);
			const tagItemDisplayInfo *pInfo = ItemProtoData::Inst()->FindItemDisplay(pProto->dwTypeID);
			if( P_VALID(pItem) && P_VALID(pInfo) )
			{
				m_pUnitItemBack[i]->SetInvisible(FALSE);
				m_pUnitItem[i]->RefreshItem(pProto->dwTypeID, 1, pItem->byQuality);
				m_pUnitItem[i]->SetBtnData(ESBT_Item, pProto->dwID, pProto->dwTypeID, pProto->nIndexInServer);
				m_pStcItemName[i]->SetText(pInfo->szName);
				DWORD dwTime = CalcTimeDiff(tagDWORDTime(DWORD(pProto->dwTimeSaleEnd)), ServerTime::Inst()->CalCurrentServerDwordTime());
				if( dwTime>0 && pProto->dwTimeSaleEnd!=GT_INVALID
					&& pProto->nSalePrice!=GT_INVALID && pProto->nPrice!=pProto->nSalePrice )
					_stprintf(szBuff, g_StrTable[_T("MallTrade2")], pProto->nPrice, pProto->nSalePrice);
				else 
					_stprintf(szBuff, g_StrTable[_T("MallTrade3")], pProto->nPrice);
				m_pStcInfo[i]->SetText(szBuff);

				//热
				if( pProto->bHot )
					m_pUnitItem[i]->SetHotMallItem(true);
				else
					m_pUnitItem[i]->SetHotMallItem(false);
				//新
				if( pProto->bNew )
					m_pUnitItem[i]->SetNewMallItem(true);
				else
					m_pUnitItem[i]->SetNewMallItem(false);
				//促
				if( JudgePromotePro(pProto) )
					m_pStcState[i][EMIST_Promote]->SetPic(szPromoteHave);
				else
					m_pStcState[i][EMIST_Promote]->SetPic(szPromoteNot);
				//团
				if( JudgeGroupBuyPro(pProto) )
					m_pStcState[i][EMIST_GroupBuy]->SetPic(szGroupHave);
				else
					m_pStcState[i][EMIST_GroupBuy]->SetPic(szGroupNot);
				//赠
				if( JudgePresentPro(pProto) )
					m_pStcState[i][EMIST_Present]->SetPic(szPresentHave);
				else
					m_pStcState[i][EMIST_Present]->SetPic(szPresentNot);
				//限
				if( JudgeNumPro(pProto) )
					m_pStcState[i][EMIST_Num]->SetPic(szNumHave);
				else 
					m_pStcState[i][EMIST_Num]->SetPic(szNumNot);
				//兑
				if( JudgeExchangePro(pProto) )
					m_pStcState[i][EMIST_Exchange]->SetPic(szExchangeHove);
				else 
					m_pStcState[i][EMIST_Exchange]->SetPic(szExchangeNot);
			}
		}
		++i;
	}
	SetCurrentPage();
}

void MallFrame::ShowCurPageMallItemByAll(INT nPage)
{
	HideCtrlMallItem();
	m_nSpecialPage[m_nCurMenu+SPECIAL_TYPE] = nPage;
	TMap<DWORD, tagMallItemProto*> mapAll = MallMgr::Inst()->GetMallProto();
	TMap<DWORD, tagMallItemProto*>::TMapIterator it = mapAll.Begin();
	int i=0; 
	for(; i<m_nSpecialPage[m_nCurMenu+SPECIAL_TYPE]*MALL_MAXITEM_PERPAGE; ++i)
	{
		++it;
		if( i > mapAll.Size() )
			break;
	}
	i = 0;
	tagMallItemProto* pProto=NULL;
	DWORD pos = 0;
	TCHAR szBuff[X_LONG_NAME] = {0};
	while ( i<MALL_MAXITEM_PERPAGE && mapAll.PeekNext(it, pos, pProto) )
	{
		if( P_VALID(pProto) )
		{
			const tagItemProto* pItem = ItemProtoData::Inst()->FindItemProto(pProto->dwTypeID);
			const tagItemDisplayInfo *pInfo = ItemProtoData::Inst()->FindItemDisplay(pProto->dwTypeID);
			if( P_VALID(pItem) && P_VALID(pInfo) )
			{
				m_pUnitItemBack[i]->SetInvisible(FALSE);
				m_pUnitItem[i]->RefreshItem(pProto->dwTypeID, 1, pItem->byQuality);
				m_pUnitItem[i]->SetBtnData(ESBT_Item, pProto->dwID, pProto->dwTypeID, pProto->nIndexInServer);
				m_pStcItemName[i]->SetText(pInfo->szName);
				DWORD dwTime = CalcTimeDiff(tagDWORDTime(DWORD(pProto->dwTimeSaleEnd)), ServerTime::Inst()->CalCurrentServerDwordTime());
				if( dwTime>0 && pProto->dwTimeSaleEnd!=GT_INVALID
					&& pProto->nSalePrice!=GT_INVALID && pProto->nPrice!=pProto->nSalePrice )
					_stprintf(szBuff, g_StrTable[_T("MallTrade2")], pProto->nPrice, pProto->nSalePrice);
				else
					_stprintf(szBuff, g_StrTable[_T("MallTrade3")], pProto->nPrice);
				m_pStcInfo[i]->SetText(szBuff);
				
				//热
				if( pProto->bHot )
					m_pUnitItem[i]->SetHotMallItem(true);
				else
					m_pUnitItem[i]->SetHotMallItem(false);
				//新
				if( pProto->bNew )
					m_pUnitItem[i]->SetNewMallItem(true);
				else
					m_pUnitItem[i]->SetNewMallItem(false);
				//促
				if( JudgePromotePro(pProto) )
					m_pStcState[i][EMIST_Promote]->SetPic(szPromoteHave);
				else
					m_pStcState[i][EMIST_Promote]->SetPic(szPromoteNot);
				//团
				if( JudgeGroupBuyPro(pProto) )
					m_pStcState[i][EMIST_GroupBuy]->SetPic(szGroupHave);
				else
					m_pStcState[i][EMIST_GroupBuy]->SetPic(szGroupNot);
				//赠
				if( JudgePresentPro(pProto) )
					m_pStcState[i][EMIST_Present]->SetPic(szPresentHave);
				else
					m_pStcState[i][EMIST_Present]->SetPic(szPresentNot);
				//限
				if( JudgeNumPro(pProto) )
					m_pStcState[i][EMIST_Num]->SetPic(szNumHave);
				else 
					m_pStcState[i][EMIST_Num]->SetPic(szNumNot);
				//兑
				if( JudgeExchangePro(pProto) )
					m_pStcState[i][EMIST_Exchange]->SetPic(szExchangeHove);
				else 
					m_pStcState[i][EMIST_Exchange]->SetPic(szExchangeNot);

			}
		}
		++i;
	}
	SetCurrentPage();
}

//显示当前页的礼包
void MallFrame::ShowCurPageMallPack(INT nPage)
{
	HideCtrlMallPack();
	m_nSpecialPage[m_nCurMenu+SPECIAL_TYPE] = nPage;
	TMap<DWORD, tagMallPackProto*> mapAll = MallMgr::Inst()->GetPackItem();
	TMap<DWORD, tagMallPackProto*>::TMapIterator it = mapAll.Begin();
	int i=0; 
	for(; i<m_nSpecialPage[m_nCurMenu+SPECIAL_TYPE]*MALL_MAXPACK_PERPAGE; ++i)
	{
		++it;
		if( i > mapAll.Size() )
			break;
	}
	i = 0;
	tagMallPackProto* pPack = NULL;
	DWORD pos = 0;
	TCHAR szBuff[X_LONG_NAME] = {0};
	TCHAR szTmpPackName[X_SHORT_NAME] = {0};
	while ( i<MALL_MAXPACK_PERPAGE && mapAll.PeekNext(it, pos, pPack) )
	{
		if( P_VALID(pPack) )
		{
			m_pUnitPackBack[i]->SetInvisible(FALSE);
			for(int j=0; j<MALL_PACKPRO_NUM; j++)
			{
				if( pPack->dwTypeID[j]==GT_INVALID || pPack->dwTypeID[j]==0 )
					continue;
				const tagItemProto* pItem = ItemProtoData::Inst()->FindItemProto(pPack->dwTypeID[j]);
				const tagItemDisplayInfo *pInfo = ItemProtoData::Inst()->FindItemDisplay(pPack->dwTypeID[j]);
				if( P_VALID(pItem) && P_VALID(pInfo) )  
				{
					m_pUnitPackSel[i][j]->SetInvisible(FALSE);
					m_pUnitPack[i][j]->SetInvisible(FALSE);
					m_pUnitPack[i][j]->RefreshItem(pPack->dwTypeID[j], pPack->byItemNum[j], pItem->byQuality);
					m_pUnitPack[i][j]->SetBtnData(ESBT_Item, pPack->dwID, pPack->dwTypeID[j], pPack->nIndexInServer);	
				}
			}

			DWORD dwTime = CalcTimeDiff(tagDWORDTime(DWORD(pPack->dwTimeSaleEnd)), ServerTime::Inst()->CalCurrentServerDwordTime());
			if( dwTime>0 && pPack->dwTimeSaleEnd!=GT_INVALID
				&& pPack->nSalePrice!=GT_INVALID && pPack->nPrice!=pPack->nSalePrice )
				_stprintf(szBuff, g_StrTable[_T("MallTrade2")], pPack->nPrice, pPack->nSalePrice);
			else
				_stprintf(szBuff, g_StrTable[_T("MallTrade3")], pPack->nPrice);
			m_pStcPackInfo[i]->SetText(szBuff);
			
			ZeroMemory( szTmpPackName, X_SHORT_NAME );
			_tcscpy( szTmpPackName, pPack->tszPackName );
			m_pStcPackName[i]->SetText( szTmpPackName );
			
			//热
			if( pPack->bHot )
				m_pStcPackHot[i]->SetInvisible(FALSE);
			else
				m_pStcPackHot[i]->SetInvisible(TRUE);
			//新
			if( pPack->bNew )
				m_pStcPackNew[i]->SetInvisible(FALSE);
			else
				m_pStcPackNew[i]->SetInvisible(TRUE);
			//促
			if( JudgePromotePro(pPack) )
				m_pStcPackState[i][EMIST_Promote]->SetPic(szPromoteHave);
			else
				m_pStcPackState[i][EMIST_Promote]->SetPic(szPromoteNot);
			//团
			if( JudgeGroupBuyPro(pPack) )
				m_pStcPackState[i][EMIST_GroupBuy]->SetPic(szGroupHave);
			else
				m_pStcPackState[i][EMIST_GroupBuy]->SetPic(szGroupNot);
			//赠
			if( JudgePresentPro(pPack) )
				m_pStcPackState[i][EMIST_Present]->SetPic(szPresentHave);
			else
				m_pStcPackState[i][EMIST_Present]->SetPic(szPresentNot);
			//限
			if( JudgeNumPro(pPack) )
				m_pStcPackState[i][EMIST_Num]->SetPic(szNumHave);
			else 
				m_pStcPackState[i][EMIST_Num]->SetPic(szNumNot);
			//兑
			if( JudgeExchangePro(pPack) )
				m_pStcPackState[i][EMIST_Exchange]->SetPic(szExchangeHove);
			else 
				m_pStcPackState[i][EMIST_Exchange]->SetPic(szExchangeNot);
		}
		++i;
	}
	SetCurrentPage();
}

void MallFrame::JudgeShowMallItem( INT nMenu )
{
	switch(nMenu)
	{
	case EMIT_Package:
		{
			m_pStcGoodsBackEx->SetInvisible(TRUE);
			m_pStcPackBack->SetInvisible(FALSE);
			m_nCurMenu = nMenu;
			m_nSpecialPage[m_nCurMenu+SPECIAL_TYPE] = 0;
			if( MallMgr::Inst()->GetPackProtoSize()%MALL_MAXPACK_PERPAGE  == 0)
				m_nSpecialMaxPage[m_nCurMenu+SPECIAL_TYPE] = MallMgr::Inst()->GetPackProtoSize() / MALL_MAXPACK_PERPAGE ;
			else
				m_nSpecialMaxPage[m_nCurMenu+SPECIAL_TYPE] = MallMgr::Inst()->GetPackProtoSize() / MALL_MAXPACK_PERPAGE + 1;
			ShowCurPageMallPack(0);
		}
		break;
	case EMIT_All:
		{
			m_pStcGoodsBackEx->SetInvisible(FALSE);
			m_pStcPackBack->SetInvisible(TRUE);
			m_nCurMenu = nMenu;
			m_nSpecialPage[m_nCurMenu+SPECIAL_TYPE] = 0;
			if( MallMgr::Inst()->GetMallProtoSize()%MALL_MAXITEM_PERPAGE == 0)
				m_nSpecialMaxPage[m_nCurMenu+SPECIAL_TYPE] = MallMgr::Inst()->GetMallProtoSize() / MALL_MAXITEM_PERPAGE;
			else 
				m_nSpecialMaxPage[m_nCurMenu+SPECIAL_TYPE] = MallMgr::Inst()->GetMallProtoSize() / MALL_MAXITEM_PERPAGE + 1;
			ShowCurPageMallItemByAll(0);
		}
		break;
	default:
		{
			if(nMenu>=MALL_ITEM_SUBTYPE)
				return;
			m_pStcGoodsBackEx->SetInvisible(FALSE);
			m_pStcPackBack->SetInvisible(TRUE);
			m_nCurMenu = nMenu;
			m_nCurPage[m_nCurMenu] = 0;
			if( m_mapMallItem[m_nCurMenu].Size()%MALL_MAXITEM_PERPAGE == 0 )
				m_nCurMaxPage[m_nCurMenu] = m_mapMallItem[m_nCurMenu].Size() / MALL_MAXITEM_PERPAGE;
			else
				m_nCurMaxPage[m_nCurMenu] = m_mapMallItem[m_nCurMenu].Size() / MALL_MAXITEM_PERPAGE + 1;
			ShowCurPageMallItemByType(0);		
		}
		break;
	}
}

void MallFrame::PageUp()
{
	switch(m_nCurMenu)
	{
	case EMIT_Package:
		{
			--m_nSpecialPage[m_nCurMenu+SPECIAL_TYPE];
			if( m_nSpecialPage[m_nCurMenu+SPECIAL_TYPE] < 0 )
			{
				++m_nSpecialPage[m_nCurMenu+SPECIAL_TYPE];
			}
			ShowCurPageMallPack(m_nSpecialPage[m_nCurMenu+SPECIAL_TYPE]);
		}
		break;
	case EMIT_All:
		{
			--m_nSpecialPage[m_nCurMenu+SPECIAL_TYPE];
			if( m_nSpecialPage[m_nCurMenu+SPECIAL_TYPE] < 0 )
			{
				++m_nSpecialPage[m_nCurMenu+SPECIAL_TYPE];
			}
			ShowCurPageMallItemByAll(m_nSpecialPage[m_nCurMenu+SPECIAL_TYPE]);
		}
		break;
	default:
		{
			--m_nCurPage[m_nCurMenu];
			if (m_nCurPage[m_nCurMenu] < 0)
			{
				++m_nCurPage[m_nCurMenu];
			}
			ShowCurPageMallItemByType(m_nCurPage[m_nCurMenu]);
		}
		break;
	}
}

void MallFrame::PageDown()
{
	switch(m_nCurMenu)
	{
	case EMIT_Package:
		{
			++m_nSpecialPage[m_nCurMenu+SPECIAL_TYPE];
			if( m_nSpecialPage[m_nCurMenu+SPECIAL_TYPE] >= m_nSpecialMaxPage[m_nCurMenu+SPECIAL_TYPE] )
			{
				--m_nSpecialPage[m_nCurMenu+SPECIAL_TYPE];
			}
			ShowCurPageMallPack(m_nSpecialPage[m_nCurMenu+SPECIAL_TYPE]);
		}
		break;
	case EMIT_All:
		{
			++m_nSpecialPage[m_nCurMenu+SPECIAL_TYPE];
			if( m_nSpecialPage[m_nCurMenu+SPECIAL_TYPE] >= m_nSpecialMaxPage[m_nCurMenu+SPECIAL_TYPE] )
			{
				--m_nSpecialPage[m_nCurMenu+SPECIAL_TYPE];
			}
			ShowCurPageMallItemByAll(m_nSpecialPage[m_nCurMenu+SPECIAL_TYPE]);
		}
		break;
	default:
		{
			++m_nCurPage[m_nCurMenu];
			if (m_nCurPage[m_nCurMenu] >= m_nCurMaxPage[m_nCurMenu])
			{
				--m_nCurPage[m_nCurMenu];
			}
			ShowCurPageMallItemByType(m_nCurPage[m_nCurMenu]);
		}
		break;
	}
}

void MallFrame::SetCurrentPage()
{
	switch(m_nCurMenu)
	{
	case EMIT_Package:
		{
			TCHAR szPage[X_SHORT_NAME] = {0};
			_sntprintf(szPage, sizeof(szPage)/sizeof(TCHAR), _T("%d/%d"), m_nSpecialPage[m_nCurMenu+SPECIAL_TYPE]+1, m_nSpecialMaxPage[m_nCurMenu+SPECIAL_TYPE]);
			m_pBtnCPPack->SetText(szPage);
		}
		break;
	case EMIT_All:
		{
			TCHAR szPage[X_SHORT_NAME] = {0};
			_sntprintf(szPage, sizeof(szPage)/sizeof(TCHAR), _T("%d/%d"), m_nSpecialPage[m_nCurMenu+SPECIAL_TYPE]+1, m_nSpecialMaxPage[m_nCurMenu+SPECIAL_TYPE]);
			m_pStcCPItem->SetText(szPage);
		}
		break;
	default:
		{
			TCHAR szPage[X_SHORT_NAME] = {0};
			_sntprintf(szPage, sizeof(szPage)/sizeof(TCHAR), _T("%d/%d"), m_nCurPage[m_nCurMenu]+1, m_nCurMaxPage[m_nCurMenu]);
			m_pStcCPItem->SetText(szPage);
		}
		break;
	}
}

void MallFrame::BuyMallItem()
{
	INT nMaxValue = GT_INVALID;
	switch(m_nCurMenu)
	{
	case EMIT_Package:
		{
			const tagMallPackProto* pItem = MallMgr::Inst()->GetMallPackByID(m_dwSelectItemID);
			if( !P_VALID(pItem) )
				return;
			nMaxValue = 1;
		}
		break;
	case EMIT_All:
	default:
		{
			const tagMallItemProto* pItem = MallMgr::Inst()->GetMallItemByID(m_dwSelectItemID);
			if( !P_VALID(pItem) )
				return;
			const tagItemProto* pProto = ItemProtoData::Inst()->FindItemProto(pItem->dwTypeID);
			if( !P_VALID(pProto) )
				return;
			nMaxValue = pProto->n16MaxLapNum;
		}
		break;
	}
	if(nMaxValue==1)
	{
		tagMsgInputBoxEvent evt(_T("BuyMallItemNum_Event"), NULL);
		evt.eInputType = MBIT_Number;
		evt.eResult = MBF_OK;
		evt.nInputNum = nMaxValue;
		m_pFrameMgr->SendEvent(&evt);
	}
	else
	{
		if( P_VALID(GetObj("BuyMallItemNum_MsgBox")) )
			KillObj("BuyMallItemNum_MsgBox");
		CreateObj("BuyMallItemNum_MsgBox", "MsgInputBox");

		

		TObjRef<MsgInputBox>("BuyMallItemNum_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade4")], 
			_T("BuyMallItemNum_Event"), nMaxValue, (MsgBoxInputType)(MBIT_Number), _T(""), TRUE);
	}
}

void MallFrame::BuyMallItemPromote()
{
	INT nMaxValue = GT_INVALID;
	const tagMallItemProto* pItem = MallMgr::Inst()->GetMallItemByID(m_dwSelectItemID);
	if( !P_VALID(pItem) )
		return;
	const tagItemProto* pProto = ItemProtoData::Inst()->FindItemProto(pItem->dwTypeID);
	if( !P_VALID(pProto) )
		return;
	nMaxValue = pProto->n16MaxLapNum;

	if(nMaxValue==1)
	{
		tagMsgInputBoxEvent evt(_T("BuyMallItemPromoteNum_Event"), NULL);
		evt.eInputType = MBIT_Number;
		evt.eResult = MBF_OK;
		evt.nInputNum = nMaxValue;
		m_pFrameMgr->SendEvent(&evt);
	}
	else
	{
		if( P_VALID(GetObj("BuyMallPromoteItemNum_MsgBox")) )
			KillObj("BuyMallPromoteItemNum_MsgBox");
		CreateObj("BuyMallPromoteItemNum_MsgBox", "MsgInputBox");



		TObjRef<MsgInputBox>("BuyMallPromoteItemNum_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade4")], 
			_T("BuyMallItemPromoteNum_Event"), nMaxValue, (MsgBoxInputType)(MBIT_Number), _T(""), TRUE);
	}
}

void MallFrame::FreeGetMallItem()
{
	const tagMallFreeItem* pItem = MallMgr::Inst()->GetMallFreeItem();
	if( P_VALID(pItem) && m_dwSelectItemID==pItem->dwTypeID)
	{
		tagNC_MallFreeGetItem msg;
		msg.dwTypeID = m_dwSelectItemID;
		m_pSession->Send(&msg);
	}
	else 
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("MallTrade26")]);
}

DWORD MallFrame::OnBuyMallPromoteItemInputEvet(tagMsgInputBoxEvent* pGameEvent)
{
	if(MBF_OK == pGameEvent->eResult && MBIT_Number == pGameEvent->eInputType)
	{
		const tagMallItemProto* pItem = MallMgr::Inst()->GetMallItemByID(m_dwSelectItemID);
		m_n16BuyNum = pGameEvent->nInputNum;
		if( m_n16BuyNum==0 )
		{
			if( P_VALID(GetObj("NotBuyMallItem_MsgBox")) )
				KillObj("NotBuyMallItem_MsgBox");
			CreateObj("NotBuyMallItem_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("NotBuyMallItem_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade61")], 
				_T("NotBuyMallItem_Event"), MsgBoxFlag(MBF_OK), TRUE);
			return 0;
		}
		if( P_VALID(pItem) )
		{
			INT nYuanBao = 0;
			DWORD dwTime = CalcTimeDiff(tagDWORDTime(DWORD(pItem->dwTimeSaleEnd)), ServerTime::Inst()->CalCurrentServerDwordTime());
			if( dwTime==0 || pItem->dwTimeSaleEnd==GT_INVALID)
				nYuanBao = pItem->nPrice;
			else 
				nYuanBao = pItem->nSalePrice;
			if( m_n16BuyNum > pItem->byNum )
			{
				if( P_VALID(GetObj("NotBuyMallItem_MsgBox")) )
					KillObj("NotBuyMallItem_MsgBox");
				CreateObj("NotBuyMallItem_MsgBox", "MsgBoxEx");

				TObjRef<MsgBoxEx>("NotBuyMallItem_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade21")], 
					_T("NotBuyMallItem_Event"), MsgBoxFlag(MBF_OK), TRUE);
				return 0;
			}

			if( nYuanBao*m_n16BuyNum <= CurrencyMgr::Inst()->GetBagYuanbao() )
			{
				if( P_VALID(GetObj("AffirmBuyMallItem_MsgBox")) )
					KillObj("AffirmBuyMallItem_MsgBox");
				CreateObj("AffirmBuyMallItem_MsgBox", "MsgBox");
				TCHAR szBuff[X_LONG_NAME] = {0};
				_stprintf( szBuff, g_StrTable[_T("MallTrade5")], nYuanBao*m_n16BuyNum );
				TObjRef<MsgBox>("AffirmBuyMallItem_MsgBox")->Init(_T(""), szBuff, 
					_T("AffirmBuyMallItem_Event"), MsgBoxFlag(MBF_OK|MBF_Cancel), TRUE);
			}
			else 
			{
				if( P_VALID(GetObj("NotBuyMallItem_MsgBox")) )
					KillObj("NotBuyMallItem_MsgBox");
				CreateObj("NotBuyMallItem_MsgBox", "MsgBoxEx");

				TObjRef<MsgBoxEx>("NotBuyMallItem_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade6")], 
					_T("NotBuyMallItem_Event"), MsgBoxFlag(MBF_OK), TRUE);
			}
		}
	}
	return 0;
}

DWORD MallFrame::OnBuyMallItemInputEvent( tagMsgInputBoxEvent* pGameEvent )
{
	if(MBF_OK == pGameEvent->eResult && MBIT_Number == pGameEvent->eInputType)
	{
		switch(m_nCurMenu)
		{
		case EMIT_Package:
			{
				const tagMallPackProto* pPack = MallMgr::Inst()->GetMallPackByID(m_dwSelectItemID);
				m_n16BuyNum = pGameEvent->nInputNum;
				if( m_n16BuyNum==0 )
				{
					if( P_VALID(GetObj("NotBuyMallItem_MsgBox")) )
						KillObj("NotBuyMallItem_MsgBox");
					CreateObj("NotBuyMallItem_MsgBox", "MsgBoxEx");

					TObjRef<MsgBoxEx>("NotBuyMallItem_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade61")], 
						_T("NotBuyMallItem_Event"), MsgBoxFlag(MBF_Cancel), TRUE);
					return 0;
				}
				if( P_VALID(pPack) )
				{
					INT nYuanBao = 0;
					DWORD dwTime = CalcTimeDiff(tagDWORDTime(DWORD(pPack->dwTimeSaleEnd)), ServerTime::Inst()->CalCurrentServerDwordTime());
					if( dwTime==0 || pPack->dwTimeSaleEnd==GT_INVALID)
						nYuanBao = pPack->nPrice;
					else 
						nYuanBao = pPack->nSalePrice;
					if( m_n16BuyNum > pPack->byNum )
					{
						if( P_VALID(GetObj("NotBuyMallItem_MsgBox")) )
							KillObj("NotBuyMallItem_MsgBox");
						CreateObj("NotBuyMallItem_MsgBox", "MsgBoxEx");

						TObjRef<MsgBoxEx>("NotBuyMallItem_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade21")], 
							_T("NotBuyMallItem_Event"), MsgBoxFlag(MBF_Cancel), TRUE);
						return 0;
					}

					if( nYuanBao*m_n16BuyNum <= CurrencyMgr::Inst()->GetBagYuanbao() )
					{
						if( P_VALID(GetObj("AffirmBuyMallItem_MsgBox")) )
							KillObj("AffirmBuyMallItem_MsgBox");
						CreateObj("AffirmBuyMallItem_MsgBox", "MsgBox");
						TCHAR szBuff[X_LONG_NAME] = {0};
						_stprintf( szBuff, g_StrTable[_T("MallTrade5")], nYuanBao*m_n16BuyNum );
						TObjRef<MsgBox>("AffirmBuyMallItem_MsgBox")->Init(_T(""), szBuff, 
							_T("AffirmBuyMallItem_Event"), MsgBoxFlag(MBF_OK|MBF_Cancel), TRUE);
					}
					else 
					{
						if( P_VALID(GetObj("NotBuyMallItem_MsgBox")) )
							KillObj("NotBuyMallItem_MsgBox");
						CreateObj("NotBuyMallItem_MsgBox", "MsgBoxEx");

						TObjRef<MsgBoxEx>("NotBuyMallItem_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade6")], 
							_T("NotBuyMallItem_Event"), MsgBoxFlag(MBF_OK), TRUE);
					}
				}
			}
			break;
		case EMIT_All:
			{
				const tagMallItemProto* pItem = MallMgr::Inst()->GetMallItemByID(m_dwSelectItemID);
				m_n16BuyNum = pGameEvent->nInputNum;
				if( m_n16BuyNum==0 )
				{
					if( P_VALID(GetObj("NotBuyMallItem_MsgBox")) )
						KillObj("NotBuyMallItem_MsgBox");
					CreateObj("NotBuyMallItem_MsgBox", "MsgBoxEx");

					TObjRef<MsgBoxEx>("NotBuyMallItem_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade61")], 
						_T("NotBuyMallItem_Event"), MsgBoxFlag(MBF_Cancel), TRUE);
					return 0;
				}
				if( P_VALID(pItem) )
				{
					INT nYuanBao = 0;
					DWORD dwTime = CalcTimeDiff(tagDWORDTime(DWORD(pItem->dwTimeSaleEnd)), ServerTime::Inst()->CalCurrentServerDwordTime());
					if( dwTime==0 || pItem->dwTimeSaleEnd==GT_INVALID)
						nYuanBao = pItem->nPrice;
					else 
						nYuanBao = pItem->nSalePrice;
					if( m_n16BuyNum > pItem->byNum )
					{
						if( P_VALID(GetObj("NotBuyMallItem_MsgBox")) )
							KillObj("NotBuyMallItem_MsgBox");
						CreateObj("NotBuyMallItem_MsgBox", "MsgBoxEx");

						TObjRef<MsgBoxEx>("NotBuyMallItem_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade21")], 
							_T("NotBuyMallItem_Event"), MsgBoxFlag(MBF_OK), TRUE);
						return 0;
					}

					if( nYuanBao*m_n16BuyNum <= CurrencyMgr::Inst()->GetBagYuanbao() )
					{
						if( P_VALID(GetObj("AffirmBuyMallItem_MsgBox")) )
							KillObj("AffirmBuyMallItem_MsgBox");
						CreateObj("AffirmBuyMallItem_MsgBox", "MsgBox");
						TCHAR szBuff[X_LONG_NAME] = {0};
						_stprintf( szBuff, g_StrTable[_T("MallTrade5")], nYuanBao*m_n16BuyNum );
						TObjRef<MsgBox>("AffirmBuyMallItem_MsgBox")->Init(_T(""), szBuff, 
							_T("AffirmBuyMallItem_Event"), MsgBoxFlag(MBF_OK|MBF_Cancel), TRUE);
					}
					else 
					{
						if( P_VALID(GetObj("NotBuyMallItem_MsgBox")) )
							KillObj("NotBuyMallItem_MsgBox");
						CreateObj("NotBuyMallItem_MsgBox", "MsgBoxEx");

						TObjRef<MsgBoxEx>("NotBuyMallItem_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade6")], 
							_T("NotBuyMallItem_Event"), MsgBoxFlag(MBF_OK), TRUE);
					}
				}
			}
			break;
		default:
			{
				const tagMallItemProto* pItem = MallMgr::Inst()->GetMallItemByID(m_dwSelectItemID);
				m_n16BuyNum = pGameEvent->nInputNum;
				if( m_n16BuyNum==0 )
				{
					if( P_VALID(GetObj("NotBuyMallItem_MsgBox")) )
						KillObj("NotBuyMallItem_MsgBox");
					CreateObj("NotBuyMallItem_MsgBox", "MsgBoxEx");

					TObjRef<MsgBoxEx>("NotBuyMallItem_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade61")], 
						_T("NotBuyMallItem_Event"), MsgBoxFlag(MBF_OK), TRUE);
					return 0;
				}
				if( P_VALID(pItem) )
				{
					INT nYuanBao = 0;
					DWORD dwTime = CalcTimeDiff(tagDWORDTime(DWORD(pItem->dwTimeSaleEnd)), ServerTime::Inst()->CalCurrentServerDwordTime());
					if( dwTime==0 || pItem->dwTimeSaleEnd==GT_INVALID)
						nYuanBao = pItem->nPrice;
					else 
						nYuanBao = pItem->nSalePrice;
					if( m_n16BuyNum > pItem->byNum )
					{
						if( P_VALID(GetObj("NotBuyMallItem_MsgBox")) )
							KillObj("NotBuyMallItem_MsgBox");
						CreateObj("NotBuyMallItem_MsgBox", "MsgBoxEx");

						TObjRef<MsgBoxEx>("NotBuyMallItem_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade21")], 
							_T("NotBuyMallItem_Event"), MsgBoxFlag(MBF_OK), TRUE);
						return 0;
					}

					if( nYuanBao*m_n16BuyNum <= CurrencyMgr::Inst()->GetBagYuanbao() )
					{
						if( P_VALID(GetObj("AffirmBuyMallItem_MsgBox")) )
							KillObj("AffirmBuyMallItem_MsgBox");
						CreateObj("AffirmBuyMallItem_MsgBox", "MsgBox");
						TCHAR szBuff[X_LONG_NAME] = {0};
						_stprintf( szBuff, g_StrTable[_T("MallTrade5")], nYuanBao*m_n16BuyNum );
						TObjRef<MsgBox>("AffirmBuyMallItem_MsgBox")->Init(_T(""), szBuff, 
							_T("AffirmBuyMallItem_Event"), MsgBoxFlag(MBF_OK|MBF_Cancel), TRUE);
					}
					else
					{
						if( P_VALID(GetObj("NotBuyMallItem_MsgBox")) )
							KillObj("NotBuyMallItem_MsgBox");
						CreateObj("NotBuyMallItem_MsgBox", "MsgBoxEx");

						TObjRef<MsgBoxEx>("NotBuyMallItem_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade6")], 
							_T("NotBuyMallItem_Event"), MsgBoxFlag(MBF_OK), TRUE);
					}
				}
			}
			break;
		}
	}
	return 0;
}

DWORD MallFrame::OnAffirmBuyMallItemEvent( tagMsgBoxEvent* pGameEvent )
{
	if(MBF_OK == pGameEvent->eResult)
	{
		if( !m_bSelectPromote )
		{
			switch(m_nCurMenu)
			{
			case EMIT_Package:
				{
					const tagMallPackProto* pPack = MallMgr::Inst()->GetMallPackByID(m_dwSelectItemID);
					OnSendBuyMallPack(pPack, m_n16BuyNum);
				}
				break;
			case EMIT_All:
			default:
				{
					const tagMallItemProto* pItem = MallMgr::Inst()->GetMallItemByID(m_dwSelectItemID);
					OnSendBuyMallItem(pItem, m_n16BuyNum);
				}
				break;
			}
		}
		else
		{
			const tagMallItemProto* pItem = MallMgr::Inst()->GetMallItemByID(m_dwSelectItemID);
			OnSendBuyMallItem(pItem, m_n16BuyNum);
		}
	}
	return 0;
}

DWORD MallFrame::OnMsgMallGiftEvent(tagMsgMallGiftEvent* pGameEvent)
{
	if(MGF_OK == pGameEvent->eRet)
	{
		//没有输入名字
		if(pGameEvent->strName.empty())
		{
			CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("MallTrade7")]);
			return 0;
		}

		//查ID
		DWORD dwRoleID = PlayerNameTab::Inst()->FindIDByName(pGameEvent->strName.c_str());
		if( dwRoleID==GT_INVALID )
		{
			bool bHaveSave = false;
			vector<tagMsgMallGiftEvent>::iterator it = m_vecGiftEvent.begin();
			for( ;it!=m_vecGiftEvent.end(); ++it )
			{
				if(it->strName == pGameEvent->strName)
				{
					bHaveSave = true;
					break;
				}
			}
			if( !bHaveSave )
			{
				m_vecGiftEvent.push_back(*pGameEvent);
			}
			else
			{
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("MallTrade28")]);
			}
			return 0;
		}

		//推荐商品特殊处理
		if( m_bSelectPromote )
		{
			const tagMallItemProto* pItem = MallMgr::Inst()->GetMallItemByID(m_dwSelectItemID);
			if( P_VALID(pItem) )
			{
				INT nYuanBao = 0;
				DWORD dwTime = CalcTimeDiff(tagDWORDTime(DWORD(pItem->dwTimeSaleEnd)), ServerTime::Inst()->CalCurrentServerDwordTime());
				if( dwTime==0 || pItem->dwTimeSaleEnd==GT_INVALID)
					nYuanBao = pItem->nPrice;
				else 
					nYuanBao = pItem->nSalePrice;

				if( m_n16BuyNum==0 )
				{
					if( P_VALID(GetObj("NotBuyMallItem_MsgBox")) )
						KillObj("NotBuyMallItem_MsgBox");
					CreateObj("NotBuyMallItem_MsgBox", "MsgBoxEx");

					TObjRef<MsgBoxEx>("NotBuyMallItem_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade61")], 
						_T("NotBuyMallItem_Event"), MsgBoxFlag(MBF_OK), TRUE);
					return 0;
				}

				if( m_n16BuyNum > pItem->byNum )
				{
					if( P_VALID(GetObj("NotBuyMallItem_MsgBox")) )
						KillObj("NotBuyMallItem_MsgBox");
					CreateObj("NotBuyMallItem_MsgBox", "MsgBoxEx");

					TObjRef<MsgBoxEx>("NotBuyMallItem_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade29")], 
						_T("NotBuyMallItem_Event"), MsgBoxFlag(MBF_OK), TRUE);
					return 0;
				}

				

				if( nYuanBao*pGameEvent->n16Num <= CurrencyMgr::Inst()->GetBagYuanbao() )
				{
					tagNC_MallPresentItem msg;
					msg.dwTypeID = pItem->dwID;
					msg.dwRoleTgtID = dwRoleID;
					msg.nPrice = nYuanBao;
					msg.n16BuyNum = pGameEvent->n16Num;
					_tcsncpy( msg.szLeave, pGameEvent->strWord.c_str(), sizeof(msg.szLeave)/sizeof(TCHAR) );
					msg.nIndexInServer = pItem->nIndexInServer;
					m_pSession->Send(&msg);
				}
				else 
				{
					if( P_VALID(GetObj("NotGiftMallItem_MsgBox")) )
						KillObj("NotGiftMallItem_MsgBox");
					CreateObj("NotGiftMallItem_MsgBox", "MsgBoxEx");

					TObjRef<MsgBoxEx>("NotGiftMallItem_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade8")], 
						_T("NotGiftMallItem_Event"), MsgBoxFlag(MBF_OK), TRUE);
				}
			}
			return 0;
		}

		switch(m_nCurMenu)
		{
		case EMIT_Package:
			{
				const tagMallPackProto* pPack = MallMgr::Inst()->GetMallPackByID(m_dwSelectItemID);
				if( P_VALID(pPack) )  
				{
					INT nYuanBao = 0;
					DWORD dwTime = CalcTimeDiff(tagDWORDTime(DWORD(pPack->dwTimeSaleEnd)), ServerTime::Inst()->CalCurrentServerDwordTime());
					if( dwTime==0 || pPack->dwTimeSaleEnd==GT_INVALID)
						nYuanBao = pPack->nPrice;
					else 
						nYuanBao = pPack->nSalePrice;
					if( m_n16BuyNum==0 )
					{
						if( P_VALID(GetObj("NotBuyMallItem_MsgBox")) )
							KillObj("NotBuyMallItem_MsgBox");
						CreateObj("NotBuyMallItem_MsgBox", "MsgBoxEx");

						TObjRef<MsgBoxEx>("NotBuyMallItem_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade61")], 
							_T("NotBuyMallItem_Event"), MsgBoxFlag(MBF_OK), TRUE);
						return 0;
					}
					if( m_n16BuyNum > pPack->byNum )
					{
						if( P_VALID(GetObj("NotBuyMallItem_MsgBox")) )
							KillObj("NotBuyMallItem_MsgBox");
						CreateObj("NotBuyMallItem_MsgBox", "MsgBoxEx");

						TObjRef<MsgBoxEx>("NotBuyMallItem_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade29")], 
							_T("NotBuyMallItem_Event"), MsgBoxFlag(MBF_OK), TRUE);
						return 0;
					}
					if( nYuanBao*pGameEvent->n16Num <= CurrencyMgr::Inst()->GetBagYuanbao() )
					{
						tagNC_MallPresentPack msg;
						msg.dwTypeID = pPack->dwID;
						msg.dwRoleTgtID = dwRoleID;
						msg.nPrice = nYuanBao;
						msg.n16BuyNum = pGameEvent->n16Num;
						_tcsncpy( msg.szLeave, pGameEvent->strWord.c_str(), sizeof(msg.szLeave)/sizeof(TCHAR) );
						msg.nIndexInServer = pPack->nIndexInServer;
						m_pSession->Send(&msg);
					}
					else 
					{
						if( P_VALID(GetObj("NotGiftMallItem_MsgBox")) )
							KillObj("NotGiftMallItem_MsgBox");
						CreateObj("NotGiftMallItem_MsgBox", "MsgBoxEx");

						TObjRef<MsgBoxEx>("NotGiftMallItem_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade8")], 
							_T("NotGiftMallItem_Event"), MsgBoxFlag(MBF_OK), TRUE);
					}
				}
			}
			break;
		case EMIT_All:
		default:
			{
				const tagMallItemProto* pItem = MallMgr::Inst()->GetMallItemByID(m_dwSelectItemID);
				if( P_VALID(pItem) )
				{
					INT nYuanBao = 0;
					DWORD dwTime = CalcTimeDiff(tagDWORDTime(DWORD(pItem->dwTimeSaleEnd)), ServerTime::Inst()->CalCurrentServerDwordTime());
					if( dwTime==0 || pItem->dwTimeSaleEnd==GT_INVALID)
						nYuanBao = pItem->nPrice;
					else 
						nYuanBao = pItem->nSalePrice;
					if( m_n16BuyNum==0 )
					{
						if( P_VALID(GetObj("NotBuyMallItem_MsgBox")) )
							KillObj("NotBuyMallItem_MsgBox");
						CreateObj("NotBuyMallItem_MsgBox", "MsgBoxEx");

						TObjRef<MsgBoxEx>("NotBuyMallItem_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade61")], 
							_T("NotBuyMallItem_Event"), MsgBoxFlag(MBF_OK), TRUE);
						return 0;
					}
					if( m_n16BuyNum > pItem->byNum )
					{
						if( P_VALID(GetObj("NotBuyMallItem_MsgBox")) )
							KillObj("NotBuyMallItem_MsgBox");
						CreateObj("NotBuyMallItem_MsgBox", "MsgBoxEx");

						TObjRef<MsgBoxEx>("NotBuyMallItem_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade29")], 
							_T("NotBuyMallItem_Event"), MsgBoxFlag(MBF_OK), TRUE);
						return 0;
					}
					if( nYuanBao*pGameEvent->n16Num <= CurrencyMgr::Inst()->GetBagYuanbao() )
					{
						tagNC_MallPresentItem msg;
						msg.dwTypeID = pItem->dwID;
						msg.dwRoleTgtID = dwRoleID;
						msg.nPrice = nYuanBao;
						msg.n16BuyNum = pGameEvent->n16Num;
						_tcsncpy( msg.szLeave, pGameEvent->strWord.c_str(), sizeof(msg.szLeave)/sizeof(TCHAR) );
						msg.nIndexInServer = pItem->nIndexInServer;
						m_pSession->Send(&msg);
					}
					else 
					{
						if( P_VALID(GetObj("NotGiftMallItem_MsgBox")) )
							KillObj("NotGiftMallItem_MsgBox");
						CreateObj("NotGiftMallItem_MsgBox", "MsgBoxEx");

						TObjRef<MsgBoxEx>("NotGiftMallItem_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade8")], 
							_T("NotGiftMallItem_Event"), MsgBoxFlag(MBF_OK), TRUE);
					}
				}
			}
			break;
		}
	}
	return 0;
}

VOID MallFrame::OnSendBuyMallItem( const tagMallItemProto* pItem, INT16 n16Num )
{
	if( P_VALID( pItem ) )
	{
		tagNC_MallBuyItem msg;
		msg.nIndexInServer = pItem->nIndexInServer;
		msg.dwTypeID = pItem->dwID;
		msg.n16BuyNum = n16Num;
		DWORD dwTime = CalcTimeDiff(tagDWORDTime(DWORD(pItem->dwTimeSaleEnd)), ServerTime::Inst()->CalCurrentServerDwordTime());
		if( dwTime==0 || pItem->dwTimeSaleEnd==GT_INVALID)
			msg.nPrice = pItem->nPrice;
		else 
			msg.nPrice = pItem->nSalePrice;
		m_pSession->Send(&msg);
	}
}

VOID MallFrame::OnSendBuyMallPack(const tagMallPackProto* pPack, INT16 n16Num)
{
	tagNC_MallBuyPack msg;
	msg.nIndexInServer = pPack->nIndexInServer;
	msg.dwTypeID = pPack->dwID;
	msg.n16BuyNum = n16Num;
	DWORD dwTime = CalcTimeDiff(tagDWORDTime(DWORD(pPack->dwTimeSaleEnd)), ServerTime::Inst()->CalCurrentServerDwordTime());
	if( dwTime==0 || pPack->dwTimeSaleEnd==GT_INVALID)
		msg.nPrice = pPack->nPrice;
	else 
		msg.nPrice = pPack->nSalePrice;
	m_pSession->Send(&msg);
}

void MallFrame::RefreshFreeItem()
{
	tagMallFreeItem* pProItem = MallMgr::Inst()->GetMallFreeItem();
	if( P_VALID(pProItem) )
	{
		const tagItemProto* pItem = ItemProtoData::Inst()->FindItemProto(pProItem->dwTypeID);
		const tagItemProto* pItemProto = ItemProtoData::Inst()->FindItemProto(pProItem->dwTypeID);
		if( P_VALID(pItem) && P_VALID(pItemProto) )
		{
			m_pStcFetchBack->SetInvisible(FALSE);
			m_pFetchUnitItem->RefreshItem(pProItem->dwTypeID, pProItem->byNum, pItem->byQuality);
			m_pFetchUnitItem->SetBtnData(ESBT_Item, pProItem->dwTypeID, pProItem->nUnitPrice, pProItem->byNum);
			const tagItemDisplayInfo* pInfo = ItemProtoData::Inst()->FindItemDisplay(pProItem->dwTypeID);
			if( P_VALID( pInfo ) )
			{
				m_pBtnFetchTitle->SetText(pInfo->szName);
			}
		}
		else 
		{
			m_pStcFetchBack->SetInvisible(TRUE);
		}
	}
}

void MallFrame::RefreshPromoteItem()
{
	HideCtrlPromoteItem();
	TCHAR szBuff[X_LONG_NAME] = {0};
	for(int i=0; i<MALL_MAX_PROMOTE_ITEM; i++)
	{
		tagMallItemProto* pProItem = MallMgr::Inst()->GetMallPromoteItem(i);
		if( P_VALID(pProItem) )		
		{
			const tagItemProto* pItem = ItemProtoData::Inst()->FindItemProto(pProItem->dwTypeID);
			const tagItemDisplayInfo *pInfo = ItemProtoData::Inst()->FindItemDisplay(pProItem->dwTypeID);
			if( P_VALID(pItem) && P_VALID(pInfo) )
			{
				m_pProItemBack[i]->SetInvisible(FALSE);
				m_pUnitProItem[i]->RefreshItem(pProItem->dwTypeID, 1, pItem->byQuality);
				m_pUnitProItem[i]->SetBtnData(ESBT_Item, pProItem->dwID, pProItem->dwTypeID, pProItem->nIndexInServer);
				m_pProItemName[i]->SetText(pInfo->szName);		
				DWORD dwTime = CalcTimeDiff(tagDWORDTime(DWORD(pProItem->dwTimeSaleEnd)), ServerTime::Inst()->CalCurrentServerDwordTime());
				if( dwTime>0 && pProItem->dwTimeSaleEnd!=GT_INVALID
					&& pProItem->nSalePrice!=GT_INVALID && pProItem->nPrice!=pProItem->nSalePrice )
					_stprintf(szBuff, g_StrTable[_T("MallTrade2")], pProItem->nPrice, pProItem->nSalePrice);
				else 
					_stprintf(szBuff, g_StrTable[_T("MallTrade3")], pProItem->nPrice);
				m_pProItmeInfo[i]->SetText(szBuff);

				//热
				if( pProItem->bHot )
					m_pUnitProItem[i]->SetHotMallItem(true);
				else
					m_pUnitProItem[i]->SetHotMallItem(false);
				//新
				if( pProItem->bNew )
					m_pUnitProItem[i]->SetNewMallItem(true);
				else
					m_pUnitProItem[i]->SetNewMallItem(false);
				//促
				if( JudgePromotePro(pProItem) )
					m_pProItemState[i][EMIST_Promote]->SetPic(szPromoteHave);
				else
					m_pProItemState[i][EMIST_Promote]->SetPic(szPromoteNot);
				//团
				if( JudgeGroupBuyPro(pProItem) )
					m_pProItemState[i][EMIST_GroupBuy]->SetPic(szGroupHave);
				else
					m_pProItemState[i][EMIST_GroupBuy]->SetPic(szGroupNot);
				//赠
				if( JudgePresentPro(pProItem) )
					m_pProItemState[i][EMIST_Present]->SetPic(szPresentHave);
				else
					m_pProItemState[i][EMIST_Present]->SetPic(szPresentNot);
				//限
				if( JudgeNumPro(pProItem) )
					m_pProItemState[i][EMIST_Num]->SetPic(szNumHave);
				else 
					m_pProItemState[i][EMIST_Num]->SetPic(szNumNot);
				//兑
				if( JudgeExchangePro(pProItem) )
					m_pProItemState[i][EMIST_Exchange]->SetPic(szExchangeHove);
				else 
					m_pProItemState[i][EMIST_Exchange]->SetPic(szExchangeNot);
			}
		}
	}
}

DWORD MallFrame::OnNS_MallBuyItem(tagNS_MallBuyItem* pMsg, DWORD dwParam)
{
	switch(pMsg->dwErrorCode)
	{
	case E_Success:
		{
			if( P_VALID(GetObj("LauncherSucess_MsgBox")) )
				KillObj("LauncherSucess_MsgBox");
			CreateObj("LauncherSucess_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherSucess_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade9")], 
				_T("LauncherSucess_Event"), MsgBoxFlag(MBF_OK), TRUE);
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
	case E_Bag_NotEnoughSpace:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("PickGroundItemErr1")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Mall_Item_NotEnough:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade21")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Mall_ID_Error:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade23")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Con_PswNotPass:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade27")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Mall_YuanBao_Error:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade24")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Mall_CreateItem_Failed:// 商品创建失败
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade59")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Mall_CreatePres_Failed:	// 赠品创建失败
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade60")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	default:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("SystemError")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	}
	return 0;
}

DWORD MallFrame::OnNS_MallFreeGetItem( tagNS_MallFreeGetItem* pMsg, DWORD dwParam )
{
	switch(pMsg->dwErrorCode)
	{
	case E_Success:
		{
			if( P_VALID(GetObj("LauncherSucess_MsgBox")) )
				KillObj("LauncherSucess_MsgBox");
			CreateObj("LauncherSucess_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherSucess_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade10")], 
				_T("LauncherSucess_Event"), MsgBoxFlag(MBF_OK), TRUE);
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
	case E_Bag_NotEnoughSpace:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("PickGroundItemErr1")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Mall_Free_AlreadyGet:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade20")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Mall_ID_Error:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade23")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Mall_Free_NotExist:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade25")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Con_PswNotPass:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade27")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Mall_CreateItem_Failed:// 商品创建失败
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade59")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Mall_CreatePres_Failed:	// 赠品创建失败
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade60")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	default:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("SystemError")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	}
	return 0;
}

DWORD MallFrame::OnNS_MallPresentItem( tagNS_MallPresentItem* pMsg, DWORD dwParam )
{
	switch(pMsg->dwErrorCode)
	{
	case E_Success:
		{
			if( P_VALID(GetObj("LauncherSucess_MsgBox")) )
				KillObj("LauncherSucess_MsgBox");
			CreateObj("LauncherSucess_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherSucess_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade11")], 
				_T("LauncherSucess_Event"), MsgBoxFlag(MBF_OK), TRUE);
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
	case E_Bag_NotEnoughSpace:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("PickGroundItemErr1")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Mall_Item_NotEnough:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade21")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Mall_ID_Error:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade23")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Mall_YuanBao_Error:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade24")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Con_PswNotPass:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade27")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Role_Not_ExistIn_World:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade28")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Mall_CreateItem_Failed:// 商品创建失败
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade59")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Mall_CreatePres_Failed:	// 赠品创建失败
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade60")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	default:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("SystemError")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	}
	return 0;
}

DWORD MallFrame::OnNS_MallBuyPack( tagNS_MallBuyPack* pMsg, DWORD dwParam )
{
	switch(pMsg->dwErrorCode)
	{
	case E_Success:
		{
			if( P_VALID(GetObj("LauncherSucess_MsgBox")) )
				KillObj("LauncherSucess_MsgBox");
			CreateObj("LauncherSucess_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherSucess_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade9")], 
				_T("LauncherSucess_Event"), MsgBoxFlag(MBF_OK), TRUE);
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
	case E_Bag_NotEnoughSpace:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("PickGroundItemErr1")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Mall_Pack_NotEnough:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade22")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Mall_ID_Error:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade23")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Mall_YuanBao_Error:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade24")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Con_PswNotPass:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade27")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Mall_CreateItem_Failed:// 商品创建失败
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade59")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Mall_CreatePres_Failed:	// 赠品创建失败
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade60")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	default:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("SystemError")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	}
	return 0;
}

DWORD MallFrame::OnNS_MallPresentPack( tagNS_MallPresentPack* pMsg, DWORD dwParam )
{
	switch(pMsg->dwErrorCode)
	{
	case E_Success:
		{
			if( P_VALID(GetObj("LauncherSucess_MsgBox")) )
				KillObj("LauncherSucess_MsgBox");
			CreateObj("LauncherSucess_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherSucess_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade11")], 
				_T("LauncherSucess_Event"), MsgBoxFlag(MBF_OK), TRUE);
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
	case E_Bag_NotEnoughSpace:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("PickGroundItemErr1")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Mall_Pack_NotEnough:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade22")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Mall_ID_Error:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade23")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Mall_YuanBao_Error:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade24")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Con_PswNotPass:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade27")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Role_Not_ExistIn_World:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade28")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Mall_CreateItem_Failed:// 商品创建失败
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade59")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Mall_CreatePres_Failed:	// 赠品创建失败
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade60")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	default:
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("SystemError")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	}
	return 0;
}

void MallFrame::ShowMallGoodsTips( int nGoodsPos, int nStatePos )
{
	switch(m_nCurMenu)
	{
	case EMIT_Package:
		{
			ASSERT(0 && _T("mall frame goods tips error!\n"));
		}
		break;
	case EMIT_All:
	default:
		{
			DWORD dwID = m_pUnitItem[nGoodsPos]->GetBtnData().GetData1();
			const tagMallItemProto* pItem = MallMgr::Inst()->GetMallItemByID(dwID);
			if( P_VALID(pItem) )
			{
				switch(nStatePos)
				{
				case EMIST_Promote:
					{
						ShowPromoteProTips(pItem, nGoodsPos, nStatePos);
					}
					break;
				case EMIST_GroupBuy:
					{
						ShowGroupBuyProTips(pItem, nGoodsPos, nStatePos);
					}
					break;
				case EMIST_Present:
					{
						ShowPresentProTips(pItem, nGoodsPos, nStatePos);
					}
					break;
				case EMIST_Num:
					{
						ShowNumProTips(pItem, nGoodsPos, nStatePos);
					}
					break;
				case EMIST_Exchange:
					{
						ShowExchangeProTips(pItem, nGoodsPos, nStatePos);
					}
					break;
				}
			}
		}
		break;
	}
}

void MallFrame::ShowMallPromoteGoodsTips( int nGoodsPos, int nStatePos )
{
	DWORD dwID = m_pUnitProItem[nGoodsPos]->GetBtnData().GetData1();
	const tagMallItemProto* pItem = MallMgr::Inst()->GetMallItemByID(dwID);
	if( P_VALID(pItem) )
	{
		switch(nStatePos)
		{
		case EMIST_Promote:
			{
				ShowPromoteProTips(pItem, nGoodsPos, nStatePos, EMITT_Promote);
			}
			break;
		case EMIST_GroupBuy:
			{
				ShowGroupBuyProTips(pItem, nGoodsPos, nStatePos, EMITT_Promote);
			}
			break;
		case EMIST_Present:
			{
				ShowPresentProTips(pItem, nGoodsPos, nStatePos, EMITT_Promote);
			}
			break;
		case EMIST_Num:
			{
				ShowNumProTips(pItem, nGoodsPos, nStatePos, EMITT_Promote);
			}
			break;
		case EMIST_Exchange:
			{
				ShowExchangeProTips(pItem, nGoodsPos, nStatePos, EMITT_Promote);
			}
			break;
		}
	}
}

void MallFrame::ShowMallPackTips(int nGoodsPos, int nStatePos)
{
	DWORD dwID = m_pUnitPack[nGoodsPos][0]->GetBtnData().GetData1();
	const tagMallPackProto* pPack = MallMgr::Inst()->GetMallPackByID(dwID);
	if( P_VALID(pPack) )
	{
		switch(nStatePos)
		{
		case EMIST_Promote:
			{
				ShowPromoteProTips(pPack, nGoodsPos, nStatePos, EMITT_Package);
			}
			break;
		case EMIST_GroupBuy:
			{
				ShowGroupBuyProTips(pPack, nGoodsPos, nStatePos, EMITT_Package);
			}
			break;
		case EMIST_Present:
			{
				ShowPresentProTips(pPack, nGoodsPos, nStatePos, EMITT_Package);
			}
			break;
		case EMIST_Num:
			{
				ShowNumProTips(pPack, nGoodsPos, nStatePos, EMITT_Package);
			}
			break;
		case EMIST_Exchange:
			{
				ShowExchangeProTips(pPack, nGoodsPos, nStatePos, EMITT_Package);
			}
			break;

		}
	}
}

void MallFrame::ShowPromoteProTips( const tagMallItemProtoBase* pItem, int nGoodsPos, int nStatePos, EMamllItemTipsType eType/*=EMITT_All*/ )
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
		switch(eType)
		{
		case EMITT_All:
			ShowTip( m_pStcState[nGoodsPos][nStatePos], strTips.c_str() );
			break;
		case EMITT_Package:			
			ShowTip( m_pStcPackState[nGoodsPos][nStatePos], strTips.c_str() );
			break;
		case EMITT_Promote:
			ShowTip( m_pProItemState[nGoodsPos][nStatePos], strTips.c_str() );
			break;
		}
	}
}

void MallFrame::ShowGroupBuyProTips( const tagMallItemProtoBase* pItem, int nGoodsPos, int nStatePos, EMamllItemTipsType eType/*=EMITT_All*/ )
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
		strTips = strTips + szBuff + _T("\\n");;
	}
	switch(eType)
	{
	case EMITT_All:
		ShowTip( m_pStcState[nGoodsPos][nStatePos], strTips.c_str() );
		break;
	case EMITT_Package:			
		ShowTip( m_pStcPackState[nGoodsPos][nStatePos], strTips.c_str() );
		break;
	case EMITT_Promote:
		ShowTip( m_pProItemState[nGoodsPos][nStatePos], strTips.c_str() );
		break;
	}
}

void MallFrame::ShowPresentProTips( const tagMallItemProtoBase* pItem, int nGoodsPos, int nStatePos, EMamllItemTipsType eType/*=EMITT_All*/ )
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
			switch(eType)
			{
			case EMITT_All:
				ShowTip( m_pStcState[nGoodsPos][nStatePos], strTips.c_str() );
				break;
			case EMITT_Package:			
				ShowTip( m_pStcPackState[nGoodsPos][nStatePos], strTips.c_str() );
				break;
			case EMITT_Promote:
				ShowTip( m_pProItemState[nGoodsPos][nStatePos], strTips.c_str() );
				break;
			}
		}
	}
}

void MallFrame::ShowNumProTips( const tagMallItemProtoBase* pItem, int nGoodsPos, int nStatePos, EMamllItemTipsType eType/*=EMITT_All*/ )
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
	switch(eType)
	{
	case EMITT_All:
		ShowTip( m_pStcState[nGoodsPos][nStatePos], strTips.c_str() );
		break;
	case EMITT_Package:			
		ShowTip( m_pStcPackState[nGoodsPos][nStatePos], strTips.c_str() );
		break;
	case EMITT_Promote:
		ShowTip( m_pProItemState[nGoodsPos][nStatePos], strTips.c_str() );
		break;
	}
}

void MallFrame::ShowExchangeProTips( const tagMallItemProtoBase* pItem, int nGoodsPos, int nStatePos, EMamllItemTipsType eType/*=EMITT_All*/)
{
	TCHAR szBuff[X_LONG_NAME];
	if( pItem->byExNum!=0 && pItem->byExNum!=(BYTE)GT_INVALID )
	{
		/*const tagItemDisplayInfo* pInfo = ItemProtoData::Inst()->FindItemDisplay(pItem->dwExTypeID);
		if( P_VALID(pInfo) )
		{*/
			_stprintf( szBuff, g_StrTable[_T("MallTrade19")], /*pInfo->szName,*/ pItem->byExNum );
			tstring strTips = g_StrTable[_T("TipsDefaultFont")];
			strTips += szBuff;
			switch(eType)
			{
			case EMITT_All:
				ShowTip( m_pStcState[nGoodsPos][nStatePos], strTips.c_str() );
				break;
			case EMITT_Package:			
				ShowTip( m_pStcPackState[nGoodsPos][nStatePos], strTips.c_str() );
				break;
			case EMITT_Promote:
				ShowTip( m_pProItemState[nGoodsPos][nStatePos], strTips.c_str() );
				break;
			}
		//}
	}
}

void MallFrame::HideCtrlMallItem()
{
	for( int i=0; i<MALL_MAXITEM_PERPAGE; ++i )
	{
		m_pUnitItemBack[i]->SetInvisible(TRUE);
	}
}

void MallFrame::HideCtrlPromoteItem()
{
	for( int i=0; i<MALL_MAX_PROMOTE_ITEM; ++i )
	{
		m_pProItemBack[i]->SetInvisible(TRUE);
	}
}

void MallFrame::HideCtrlMallPack()
{
	for( int i=0; i<MALL_MAXPACK_PERPAGE; ++i )
	{
		m_pUnitPackBack[i]->SetInvisible(TRUE);
		for( int j=0; j<MALL_PACKPRO_NUM; ++j )
		{
			m_pUnitPackSel[i][j]->SetInvisible(TRUE);
			m_pUnitPack[i][j]->SetInvisible(TRUE);
		}
	}
}

bool MallFrame::JudgePromotePro( const tagMallItemProtoBase* pItem )
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

bool MallFrame::JudgeGroupBuyPro( const tagMallItemProtoBase* pItem )
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

bool MallFrame::JudgePresentPro( const tagMallItemProtoBase* pItem )
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

bool MallFrame::JudgeNumPro( const tagMallItemProtoBase* pItem )
{
	if( pItem->byNum != (BYTE)GT_INVALID )
	{
		return true;
	}
	else 
	{
		return false;
	}
}

bool MallFrame::JudgeExchangePro( const tagMallItemProtoBase* pItem )
{
	if( pItem->byExNum!=0 && pItem->byExNum!=(BYTE)GT_INVALID)
	{
		return true;
	}

	return false;
}

DWORD MallFrame::OnBagYuanbaoChange( tagRoleYuanbaoChange* pGameEvent )
{
	if(P_VALID(m_pStcYuanBao))
	{
		TCHAR szBuff[X_SHORT_NAME] = {0};
		_sntprintf(szBuff, sizeof(szBuff), _T("%d"), pGameEvent->nYuanbao);
		_itot(CurrencyMgr::Inst()->GetBagYuanbao(), szBuff, 10);
		m_pStcYuanBao->SetText(szBuff) ;

		m_pWnd->SetRefresh(TRUE);
	}
	return 0;
}

DWORD MallFrame::OnExchangeVolumeChange(tagExchangeVolumeChange* pGameEvent)
{
	if( P_VALID(m_pExchangeVolume) )
	{
		TCHAR szBuff[X_SHORT_NAME] = {0};
		_sntprintf(szBuff, sizeof(szBuff), _T("%d"), pGameEvent->nCurExVolume);
		_itot(CurrencyMgr::Inst()->GetExchangeVolume(), szBuff, 10);
		m_pExchangeVolume->SetText(szBuff);

		m_pWnd->SetRefresh(TRUE);
	}
	return 0;
}

void MallFrame::JudgeTryDress( DWORD dwTypeID )
{
	if( !MIsEquipment(dwTypeID) )
		return;
	const tagItemProto* pItem = ItemProtoData::Inst()->FindItemProto(dwTypeID);
	LocalPlayer* pLRole = RoleMgr::Inst()->GetLocalPlayer();
	if( P_VALID(pItem) && P_VALID(pLRole) )
	{
		AvatarEquipNode* pNode = (AvatarEquipNode*)pLRole->GetSceneNode();
		if( pItem->eType==EIT_Fashion && P_VALID(pNode) && pNode->GetAtt().bySex==(BYTE)pItem->eSexLimit )
		{

			tagEquipProto* pEquip = (tagEquipProto*)pItem;
			if( P_VALID(pEquip) )
			{
				EAvatarElement eAvat = (EAvatarElement)MallMgr::Inst()->GetEquipDisplayPos((INT16)pEquip->eEquipPos);
				tagEquipDisplay ep;
				ep.dwTypeID = pEquip->dwTypeID;
				ep.byDisplayPos = eAvat;
				ep.n8ColorID = EC_Start;
				m_pWndTryDress->SetEquip(eAvat, ep);
			}
		}
	}
}

void MallFrame::SetPresentMaxNum()
{
	INT nMaxValue = GT_INVALID;
	if( !m_bSelectPromote )
	{
		switch(m_nCurMenu)
		{
		case EMIT_Package:
			{
				nMaxValue = 1;
			}
			break;
		case EMIT_All:
		default:
			{
				const tagMallItemProto* pItem = MallMgr::Inst()->GetMallItemByID(m_dwSelectItemID);
				if( !P_VALID(pItem) )
					return;
				const tagItemProto* pProto = ItemProtoData::Inst()->FindItemProto(pItem->dwTypeID);
				if( !P_VALID(pProto) )
					return;
				nMaxValue = pProto->n16MaxLapNum;
			}
			break;
		}
	}
	else 
	{
		const tagMallItemProto* pItem = MallMgr::Inst()->GetMallItemByID(m_dwSelectItemID);
		if( !P_VALID(pItem) )
			return;
		const tagItemProto* pProto = ItemProtoData::Inst()->FindItemProto(pItem->dwTypeID);
		if( !P_VALID(pProto) )
			return;
		nMaxValue = pProto->n16MaxLapNum;
	}
	if( nMaxValue>=0 )
		MallMgr::Inst()->SetMallGiftFrameMaxBuyNum(nMaxValue);
}

void MallFrame::ShowWnd( BOOL bVal )
{ 
    if(P_VALID(m_pWnd)) m_pWnd->SetInvisible(!bVal);
    if (bVal)
        YuanbaoTradeMgr::Inst()->BeginNC_SynYBTradeInfo();
}
void MallFrame::SetOperateState( EMallItemOperateState eType/*=EMIOS_NULL*/ )
{	m_eOperateState = eType;		
	switch(m_eOperateState)	
	{	
	case EMIOS_NULL:	
	case EMIOS_Free:		
		{
			m_pBtnExchange->SetEnable(false);		
		}	
	case EMIOS_PackItem:		
		{
			m_pBtnGroupBuy->SetEnable(false);			
			if( m_eOperateState==EMIOS_PackItem )			
			{
				const tagMallPackProto* pItem = MallMgr::Inst()->GetMallPackByID(m_dwSelectItemID);	
				if( P_VALID(pItem) )				
				{					
					if( JudgeExchangePro(pItem) )						
						m_pBtnExchange->SetEnable(true);				
					else
						m_pBtnExchange->SetEnable(false);		
				}
			else 					
				m_pBtnExchange->SetEnable(false);			
			}	
		}		
	break;	
	case EMIOS_NormalItem:	
	case EMIOS_Promote:		
		{
		const tagMallItemProto* pItem = MallMgr::Inst()->GetMallItemByID(m_dwSelectItemID);			
		if( P_VALID(pItem) )			
		{				
			if( pItem->bySmallGroupDiscount!=(BYTE)GT_INVALID|| pItem->byMediumGroupDiscount!=(BYTE)GT_INVALID	
				|| pItem->byBigGroupDiscount!=(BYTE)GT_INVALID )			
			{					
				LocalPlayer* pLocal = RoleMgr::Inst()->GetLocalPlayer();					
				if( P_VALID(pLocal) && pLocal->IsInGuild() )					
				{						
					m_pBtnGroupBuy->SetEnable(true);					
				}				
			}				
			else					
				m_pBtnGroupBuy->SetEnable(false);				
			if( JudgeExchangePro(pItem) )					
				m_pBtnExchange->SetEnable(true);				
			else					
				m_pBtnExchange->SetEnable(false);			
		}			
		else			
		{				
			m_pBtnGroupBuy->SetEnable(false);				
			m_pBtnExchange->SetEnable(false);			
		}		
		}		
		break;	
	}
}

void MallFrame::ExchangeMallItem()
{	
	INT nMaxValue = GT_INVALID;		
	//推荐商品特殊处理	
	if( m_bSelectPromote )  	
	{		
		const tagMallItemProto* pItem = MallMgr::Inst()->GetMallItemByID(m_dwSelectItemID);		
		if( !P_VALID(pItem) )			
			return;		
		const tagItemProto* pProto = ItemProtoData::Inst()->FindItemProto(pItem->dwTypeID);		
		if( !P_VALID(pProto) )			
			return;		
		nMaxValue = pProto->n16MaxLapNum;	
	}	
	else	
	{		
		switch(m_nCurMenu)		
		{		
		case EMIT_Package:			
			{				
				const tagMallPackProto* pItem = MallMgr::Inst()->GetMallPackByID(m_dwSelectItemID);				
				if( !P_VALID(pItem) )					
					return;				
				nMaxValue = 1;			
			}			
			break;		
		case EMIT_All:		
		default:			
			{				
				const tagMallItemProto* pItem = MallMgr::Inst()->GetMallItemByID(m_dwSelectItemID);	
				if( !P_VALID(pItem) )					
					return;				
				const tagItemProto* pProto = ItemProtoData::Inst()->FindItemProto(pItem->dwTypeID);				
				if( !P_VALID(pProto) )					
					return;				
				nMaxValue = pProto->n16MaxLapNum;			
			}			
			break;		
		}	
	}	
	if(nMaxValue==1)	
	{		
		tagMsgInputBoxEvent evt(_T("ExchangePackItemNum_Event"), NULL);		
		evt.eInputType = MBIT_Number;		
		evt.eResult = MBF_OK;		
		evt.nInputNum = nMaxValue;		
		m_pFrameMgr->SendEvent(&evt);	
	}	
	else	
	{		
		if( P_VALID(GetObj("ExchangeMallItemNum_MsgBox")) )			
			KillObj("ExchangeMallItemNum_MsgBox");		
		CreateObj("ExchangeMallItemNum_MsgBox", "MsgInputBox");		
		TObjRef<MsgInputBox>("ExchangeMallItemNum_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade51")], 			
			_T("ExchangeMallItemNum_Event"), nMaxValue, (MsgBoxInputType)(MBIT_Number), _T(""), TRUE);	
	}
}

DWORD MallFrame::OnExchangePackItem( tagMsgInputBoxEvent* pGameEvent )
{	
	if(MBF_OK == pGameEvent->eResult && MBIT_Number == pGameEvent->eInputType)	
	{		
		const tagMallPackProto* pPack = MallMgr::Inst()->GetMallPackByID(m_dwSelectItemID);		
		m_n16BuyNum = pGameEvent->nInputNum;		
		if( P_VALID(pPack) )		
		{						
			if( m_n16BuyNum > pPack->byNum )			
			{				
				if( P_VALID(GetObj("NotBuyMallItem_MsgBox")) )					
					KillObj("NotBuyMallItem_MsgBox");				
				CreateObj("NotBuyMallItem_MsgBox", "MsgBoxEx");				
				TObjRef<MsgBoxEx>("NotBuyMallItem_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade53")], 		
					_T("NotBuyMallItem_Event"), MsgBoxFlag(MBF_OK), TRUE);				
				return 0;			
			}						
			INT nExVolume = 0;			
			nExVolume = pPack->byExNum;			
			if( nExVolume*m_n16BuyNum <= CurrencyMgr::Inst()->GetExchangeVolume() )			
			{				
				if( P_VALID(GetObj("AffirmExchangePackItem_MsgBox")) )			
					KillObj("AffirmExchangePackItem_MsgBox");			
				CreateObj("AffirmExchangePackItem_MsgBox", "MsgBox");			
				TCHAR szBuff[X_LONG_NAME] = {0};		
				_stprintf( szBuff, g_StrTable[_T("MallTrade54")], nExVolume*m_n16BuyNum );		
				TObjRef<MsgBox>("AffirmExchangePackItem_MsgBox")->Init(_T(""), szBuff, 		
					_T("AffirmExchangePackItem_Event"), MsgBoxFlag(MBF_OK|MBF_Cancel), TRUE);		
			}			
			else 			
			{				
				if( P_VALID(GetObj("NotBuyMallItem_MsgBox")) )			
					KillObj("NotBuyMallItem_MsgBox");			
				CreateObj("NotBuyMallItem_MsgBox", "MsgBoxEx");			
				TObjRef<MsgBoxEx>("NotBuyMallItem_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade52")], 			
					_T("NotBuyMallItem_Event"), MsgBoxFlag(MBF_OK), TRUE);			
			}		
		}	
	}
	return 0;
}
DWORD MallFrame::OnExchangeMallItem( tagMsgInputBoxEvent* pGameEvent )
{	if(MBF_OK == pGameEvent->eResult && MBIT_Number == pGameEvent->eInputType)	
	{		
		const tagMallItemProto* pItem = MallMgr::Inst()->GetMallItemByID(m_dwSelectItemID);	
		m_n16BuyNum = pGameEvent->nInputNum;		
		if( P_VALID(pItem) )		
		{			
			if( m_n16BuyNum > pItem->byNum )	
			{			
				if( P_VALID(GetObj("NotBuyMallItem_MsgBox")) )			
					KillObj("NotBuyMallItem_MsgBox");			
				CreateObj("NotBuyMallItem_MsgBox", "MsgBoxEx");			
				TObjRef<MsgBoxEx>("NotBuyMallItem_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade53")], 	
					_T("NotBuyMallItem_Event"), MsgBoxFlag(MBF_OK), TRUE);			
				return 0;			
			}					
			INT nExVolume = 0;			
			nExVolume = pItem->byExNum;			
			if( nExVolume*m_n16BuyNum <= CurrencyMgr::Inst()->GetExchangeVolume() )		
			{				
				if( P_VALID(GetObj("AffirmExchangeMallItem_MsgBox")) )			
					KillObj("AffirmExchangeMallItem_MsgBox");				
				CreateObj("AffirmExchangeMallItem_MsgBox", "MsgBox");			
				TCHAR szBuff[X_LONG_NAME] = {0};			
				_stprintf( szBuff, g_StrTable[_T("MallTrade54")], nExVolume*m_n16BuyNum );		
				TObjRef<MsgBox>("AffirmExchangeMallItem_MsgBox")->Init(_T(""), szBuff, 			
					_T("AffirmExchangeMallItem_Event"), MsgBoxFlag(MBF_OK|MBF_Cancel), TRUE);		
			}			
			else 	
			{		
				if( P_VALID(GetObj("NotBuyMallItem_MsgBox")) )		
					KillObj("NotBuyMallItem_MsgBox");				
				CreateObj("NotBuyMallItem_MsgBox", "MsgBoxEx");			
				TObjRef<MsgBoxEx>("NotBuyMallItem_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade52")], 			
					_T("NotBuyMallItem_Event"), MsgBoxFlag(MBF_OK), TRUE);		
			}	
		}	
	}
	return 0;
}

DWORD MallFrame::OnAffirmExchangePackItem( tagMsgBoxEvent* pGameEvent )
{	
	if(MBF_OK == pGameEvent->eResult)
	{		
		const tagMallPackProto* pPack = MallMgr::Inst()->GetMallPackByID(m_dwSelectItemID);		
		if( P_VALID(pPack) )	
		{			
			tagNC_MallPackExchange msg;			
			msg.dwMallID = pPack->dwID;		
			msg.nIndexInServer = pPack->nIndexInServer;	
			msg.nPrice = pPack->byExNum;	
			m_pSession->Send(&msg);		
		}		
	}
	return 0;
}

DWORD MallFrame::OnAffirmExchangeMallItem( tagMsgBoxEvent* pGameEvent )
{	
	if(MBF_OK == pGameEvent->eResult)	
	{		
		const tagMallItemProto* pItem = MallMgr::Inst()->GetMallItemByID(m_dwSelectItemID);	
		if( P_VALID(pItem) )	
		{		
			tagNC_MallItemExchange msg;			
			msg.dwMallID = pItem->dwID;		
			msg.nIndexInServer = pItem->nIndexInServer;	
			msg.nPrice = pItem->byExNum;		
			msg.n16BuyNum = m_n16BuyNum;		
			m_pSession->Send(&msg);	
		}		
	}
	return 0;
}

DWORD MallFrame::OnNS_MallItemExchange( tagNS_MallItemExchange* pMsg, DWORD dwParam )
{	
	switch(pMsg->dwErrorCode)	
	{	
	case E_Success:		
		{		
			if( P_VALID(GetObj("LauncherSucess_MsgBox")) )		
				KillObj("LauncherSucess_MsgBox");			
			CreateObj("LauncherSucess_MsgBox", "MsgBoxEx");		
			TObjRef<MsgBoxEx>("LauncherSucess_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade55")], 	
				_T("LauncherSucess_Event"), MsgBoxFlag(MBF_OK), TRUE);	
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
	case E_Mall_Item_NotEnough:	
		{		
			if( P_VALID(GetObj("LauncherError_MsgBox")) )			
				KillObj("LauncherError_MsgBox");		
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");		
			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade53")], 	
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);		
		}		
		break;	
	case E_Mall_ID_Error:	
		{			
			if( P_VALID(GetObj("LauncherError_MsgBox")) )	
				KillObj("LauncherError_MsgBox");	
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");	
			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade56")], 	
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);		
		}		
		break;	
	case E_Mall_ExVolume_Error:	
		{			
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");		
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");
			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade58")], 		
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);		
		}		
		break;	
	case E_Mall_Exchange_NotAllowable:		
		{			
			if( P_VALID(GetObj("LauncherError_MsgBox")) )	
				KillObj("LauncherError_MsgBox");		
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");	
			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade57")], 	
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);	
		}		
		break;	
	case E_ExVolume_NotEnough:  //赠点不足		
		{			
			if( P_VALID(GetObj("LauncherError_MsgBox")) )	
				KillObj("LauncherError_MsgBox");	
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");		
			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade52")], 		
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);		
		}		
		break;	
	case E_Mall_CreateItem_Failed:// 商品创建失败	
		{		
			if( P_VALID(GetObj("LauncherError_MsgBox")) )		
				KillObj("LauncherError_MsgBox");			
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");	
			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade59")], 		
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);	
		}		
		break;	
	case E_Mall_CreatePres_Failed:	// 赠品创建失败	
		{		
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");		
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");	
			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade60")], 	
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);	
		}		
		break;	
	case E_Bag_NotEnoughSpace:  //背包空间不足		
		{			
			if( P_VALID(GetObj("LauncherError_MsgBox")) )	
				KillObj("LauncherError_MsgBox");	
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");	
			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("PickGroundItemErr1")], 	
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);	
		}		
		break;	
	case E_Con_PswNotPass: //行囊没解锁		
		{			
			if( P_VALID(GetObj("LauncherError_MsgBox")) )		
				KillObj("LauncherError_MsgBox");		
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");	
			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade47")], 	
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);		
		}		
		break;	
	default:	
		{			
			if( P_VALID(GetObj("LauncherError_MsgBox")) )	
				KillObj("LauncherError_MsgBox");		
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");	
			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("SystemError")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);		
		}		
		break;	
	}
	return 0;
}
DWORD MallFrame::OnNS_MallPackExchange( tagNS_MallPackExchange* pMsg, DWORD dwParam )
{	
	switch(pMsg->dwErrorCode)	
	{	
	case E_Success:		
		{		
			if( P_VALID(GetObj("LauncherSucess_MsgBox")) )	
				KillObj("LauncherSucess_MsgBox");			
			CreateObj("LauncherSucess_MsgBox", "MsgBoxEx");	
			TObjRef<MsgBoxEx>("LauncherSucess_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade55")], 
				_T("LauncherSucess_Event"), MsgBoxFlag(MBF_OK), TRUE);	
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
	case E_Mall_Pack_NotEnough:		
		{		
			if( P_VALID(GetObj("LauncherError_MsgBox")) )	
				KillObj("LauncherError_MsgBox");		
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");		
			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade53")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);	
		}		
		break;	
	case E_Mall_ID_Error:	
		{		
			if( P_VALID(GetObj("LauncherError_MsgBox")) )	
				KillObj("LauncherError_MsgBox");		
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");	
			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade56")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);		
		}		
		break;	
	case E_Mall_ExVolume_Error:	
		{		
			if( P_VALID(GetObj("LauncherError_MsgBox")) )
				KillObj("LauncherError_MsgBox");		
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");	
			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade58")], 	
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);		
		}		
		break;	
	case E_Mall_Exchange_NotAllowable:	
		{		
			if( P_VALID(GetObj("LauncherError_MsgBox")) )		
				KillObj("LauncherError_MsgBox");			
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");	
			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade57")], 	
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);	
		}		
		break;	
	case E_ExVolume_NotEnough:  //赠点不足		
		{
			if( P_VALID(GetObj("LauncherError_MsgBox")) )	
				KillObj("LauncherError_MsgBox");		
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");		
			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade52")], 	
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);		
		}	
		break;	
	case E_Mall_CreateItem_Failed:// 商品创建失败	
		{		
			if( P_VALID(GetObj("LauncherError_MsgBox")) )				
				KillObj("LauncherError_MsgBox");		
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");		
			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade59")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);		
		}		
		break;	
	case E_Mall_CreatePres_Failed:	// 赠品创建失败	
		{		
			if( P_VALID(GetObj("LauncherError_MsgBox")) )	
				KillObj("LauncherError_MsgBox");			
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");	
			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade60")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);		
		}		
		break;	
	case E_Bag_NotEnoughSpace:  //背包空间不足	
		{			
			if( P_VALID(GetObj("LauncherError_MsgBox")) )	
				KillObj("LauncherError_MsgBox");		
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");		
			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("PickGroundItemErr1")], 
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);	
		}		
		break;	
	case E_Con_PswNotPass: //行囊没解锁	
		{			
			if( P_VALID(GetObj("LauncherError_MsgBox")) )	
				KillObj("LauncherError_MsgBox");		
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");		
			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("MallTrade47")], 	
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);	
		}		
		break;	
	default:	
		{		
			if( P_VALID(GetObj("LauncherError_MsgBox")) )		
				KillObj("LauncherError_MsgBox");		
			CreateObj("LauncherError_MsgBox", "MsgBoxEx");		
			TObjRef<MsgBoxEx>("LauncherError_MsgBox")->Init(_T(""), g_StrTable[_T("SystemError")], 	
				_T("LauncherError_Event"), MsgBoxFlag(MBF_OK), TRUE);	
		}
		break;	
	}
	return 0;
}

int EncodeBase64(BYTE *btSrc, int iSrcLen, BYTE *btRet, int *piRetLen);

void MallFrame::YuanBaoAbundance()
{
	//直充url
	TCHAR szIniFileName[X_LONG_NAME] = _T("launcher\\launcher.ini");
	TCHAR szTemp[X_LONG_NAME*2];

	_tcscpy( szTemp, Kernel::Inst()->GetWorkPath() );
	tstring strAppPath = szTemp;

	tstring strIniFileFullName = strAppPath + szIniFileName;
	::GetPrivateProfileString(_T("Launcher"), _T("LoginMode"), NULL,  szTemp, X_LONG_NAME*2, strIniFileFullName.c_str());
	tstring policy = szTemp;
	if( policy == _T("") )
	{
		MessageBox(NULL, _T("Don't find LoginMode !"), _T(""), MB_OK);
		return ;
	}

	INT nPolicy = _ttoi(policy.c_str());
	tstring url;
	switch( nPolicy )
	{
	case 0:
	case 1:
		{
			url = _T("http://passport.gamall.net/billing/pay/pay_long.jsp");
		}
		break;
	case 2:  //迅雷特殊处理
		{
			url = _T("http://pay.xunlei.com/ourproducts_long.html");
			ShellExecute(NULL, _T("open"), url.c_str(), _T(""), NULL, SW_MAXIMIZE);
			return ;
		}
		break;
	}
	
	//读取帐号
	tstring strAccountTemp = SaveAccount::Inst()->GetAccount();

	tstring strBoroghID = SaveAccount::Inst()->GetWorldID();
	/*TObjRef<VarContainer> serInfo = CreateObj("lastlogin", "VarContainer");
	const TCHAR* szFileName = _T("config\\lastlogin.xml");
	if( serInfo->Load(NULL, szFileName) )
	{
		strBoroghID = serInfo->GetString(_T("world_id"), _T("last_login_server"), _T(""));
	}
	else
	{	
		strBoroghID = _T("");
	}
	KillObj("lastlogin");*/

	//Base64编码
	string strAtemp = m_pUtil->UnicodeToAnsi(strAccountTemp.c_str());
	char btSrc[X_LONG_NAME] = {0};
	char btRet[X_LONG_NAME] = {0};
	int iRetLen = sizeof(btRet);
	_snprintf( btSrc, X_LONG_NAME, strAtemp.c_str() );
	EncodeBase64((BYTE*)btSrc, strlen(btSrc), (BYTE*)btRet, &iRetLen);

	tstring strAccount = m_pUtil->AnsiToUnicode(btRet);

	tstring strParam = _T("?useraccount=")+strAccount+_T("&gameserverid=")+strBoroghID;
	tstring strYuanBaoAbundance = url + strParam;
	//打开充值网页

	ShellExecute(NULL, _T("open"), strYuanBaoAbundance.c_str(), _T(""), NULL, SW_MAXIMIZE);
}

void MallFrame::ShowMallNotice( BOOL bVal )
{
	m_pWndNotice->SetInvisible(!bVal);

	//读取URL
	TCHAR szIniFileName[X_LONG_NAME] = _T("launcher\\launcher.ini");
	TCHAR szTemp[X_LONG_NAME*2];

	_tcscpy( szTemp, Kernel::Inst()->GetWorkPath() );
	tstring strAppPath = szTemp;

	tstring strIniFileFullName = strAppPath + szIniFileName;
	::GetPrivateProfileString(_T("Launcher"), _T("MallNoticeURL"), NULL,  szTemp, X_LONG_NAME*2, strIniFileFullName.c_str());
	tstring url = szTemp;
	if( url == _T("") )
	{
		MessageBox(NULL, _T("Don't find mall notice url!"), _T(""), MB_OK);
		return;
	}

	if( m_strMallNotice!=url )
	{
		m_strMallNotice = url;
		m_pHttp_MallNotice->SetPicURL(m_strMallNotice);
	}
}

/*====================================================================
Base64编码函数
btSrc指向被编码的数据缓冲区
iSrcLen被编码的数据的大小（字节）
btRet指向存放Base64编码的数据缓冲区
piRetLen指向存放Base64编码的数据缓冲区的长度
若btRet为NULL函数返回0，piRetLen传回btSrc的Base64编码所需缓冲区的大小
若btRet指向的缓冲区太小，函数返回-1
否则函数返回实际btSrc的Base64编码所需缓冲区的大小
=====================================================================*/
int EncodeBase64(BYTE *btSrc, int iSrcLen, BYTE *btRet, int *piRetLen) 
{
	int i = 0, j = 0, k = 0 ;
	BYTE EncodeBase64Map[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	i = (iSrcLen+2) / 3 * 4 ; //获取btSrc的Base64编码所需缓冲区的大小
	if(btRet != NULL)
	{
		if(*piRetLen < i) //Base64编码所需缓冲区偏小
			return -1 ;
		*piRetLen = i ; //*piRetLen返回btSrc的Base64编码的缓冲区大小
	}
	else
	{
		*piRetLen = i ;
		return 0 ;
	}

	k = iSrcLen - iSrcLen % 3 ;
	for(i=j=0; i<k; i+=3) //编码
	{
		btRet[j++] = EncodeBase64Map[(btSrc[i]>>2)&0x3F] ;
		btRet[j++] = EncodeBase64Map[((btSrc[i]<<4) + (btSrc[i+1]>>4))&0x3F] ;
		btRet[j++] = EncodeBase64Map[((btSrc[i+1]<<2) + (btSrc[i+2]>>6))&0x3F] ;
		btRet[j++] = EncodeBase64Map[btSrc[i+2]&0x3F] ;
	}
	k = iSrcLen - k ;
	if(1 == k)
	{
		btRet[j++] = EncodeBase64Map[(btSrc[i]>>2)&0x3F] ;
		btRet[j++] = EncodeBase64Map[(btSrc[i]<<4)&0x3F] ;
		btRet[j++] = '=' ;
		btRet[j] = '=' ;
	}
	else if(2 == k)
	{
		btRet[j++] = EncodeBase64Map[(btSrc[i]>>2)&0x3F] ;
		btRet[j++] = EncodeBase64Map[((btSrc[i]<<4) + (btSrc[i+1]>>4))&0x3F] ;
		btRet[j++] = EncodeBase64Map[(btSrc[i+1]<<2)&0x3F] ;
		btRet[j] = '=' ;
	}

	return ++j ;
}
//------------------------------------------------------------------------------------------------------------
//元宝交易
//------------------------------------------------------------------------------------------------------------