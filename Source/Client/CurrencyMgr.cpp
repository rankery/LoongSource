#include "StdAfx.h"
#include "CurrencyMgr.h"
#include "..\WorldDefine\role_att.h"
#include "..\WorldDefine\msg_currency.h"
#include "..\WorldDefine\currency_define.h"
#include "YuanbaoTradeMgr.h"
#include "PetManager.h"
#include "RoleMgr.h"

CurrencyMgr::CurrencyMgr(void):m_Trunk(this)
{
	m_n64BagSilver = NULL;
	m_nBagYuanbao = NULL;
	m_n64WareSilver = NULL;
	m_nBaibaoYuanbao = NULL;
	m_nExchangeVolume = NULL;
}

CurrencyMgr::~CurrencyMgr(void)
{
}

VOID CurrencyMgr::Init()
{
	m_pCmdMgr = TObjRef<NetCmdMgr>();
	m_pUtil	= TObjRef<Util>();
	m_pFrameMgr = TObjRef<GameFrameMgr>();
	m_pSession = TObjRef<NetSession>();

	m_pCmdMgr->Register("NS_GetRoleInitState_Money", (NETMSGPROC)m_Trunk.sfp2(&CurrencyMgr::NetInitRoleCurrency),      _T("NS_GetRoleInitState_Money"));
	m_pCmdMgr->Register("NS_BagSilver",				 (NETMSGPROC)m_Trunk.sfp2(&CurrencyMgr::NetGetRoleBagSilver),      _T("NS_BagSilver"));
	m_pCmdMgr->Register("NS_BagYuanBao",			 (NETMSGPROC)m_Trunk.sfp2(&CurrencyMgr::NetGetRoleBagYuanbao),     _T("NS_BagYuanBao"));
	m_pCmdMgr->Register("NS_WareSilver",			 (NETMSGPROC)m_Trunk.sfp2(&CurrencyMgr::NetGetRoleWareSilver),     _T("NS_WareSilver"));
	m_pCmdMgr->Register("NS_BaiBaoYuanBao",			 (NETMSGPROC)m_Trunk.sfp2(&CurrencyMgr::NetGetRoleBaibaoYuanbao),  _T("NS_BaiBaoYuanBao"));
	m_pCmdMgr->Register("NS_ClanCon",			     (NETMSGPROC)m_Trunk.sfp2(&CurrencyMgr::NetGetRoleClanContribute), _T("NS_ClanCon"));
	m_pCmdMgr->Register("NS_ExchangeVolume",		 (NETMSGPROC)m_Trunk.sfp2(&CurrencyMgr::NetGetRoleExchangeVolume), _T("NS_ExchangeVolume"));

}

VOID CurrencyMgr::Destroy()
{
	m_pCmdMgr->UnRegister("NS_GetRoleInitState_Money",	(NETMSGPROC)m_Trunk.sfp2(&CurrencyMgr::NetInitRoleCurrency));
	m_pCmdMgr->UnRegister("NS_BagSilver",				(NETMSGPROC)m_Trunk.sfp2(&CurrencyMgr::NetGetRoleBagSilver));
	m_pCmdMgr->UnRegister("NS_BagYuanBao",				(NETMSGPROC)m_Trunk.sfp2(&CurrencyMgr::NetGetRoleBagYuanbao));
	m_pCmdMgr->UnRegister("NS_WareSilver",				(NETMSGPROC)m_Trunk.sfp2(&CurrencyMgr::NetGetRoleWareSilver));
	m_pCmdMgr->UnRegister("NS_BaiBaoYuanBao",			(NETMSGPROC)m_Trunk.sfp2(&CurrencyMgr::NetGetRoleBaibaoYuanbao));
	m_pCmdMgr->UnRegister("NS_ClanCon",			        (NETMSGPROC)m_Trunk.sfp2(&CurrencyMgr::NetGetRoleClanContribute));
	m_pCmdMgr->UnRegister("NS_ExchangeVolume",			(NETMSGPROC)m_Trunk.sfp2(&CurrencyMgr::NetGetRoleExchangeVolume));

}

CurrencyMgr g_CurrencyMgr; 
CurrencyMgr* CurrencyMgr::Inst()
{
	return &g_CurrencyMgr;
}

VOID CurrencyMgr::SetBagSilver( INT64 n64Value, INT64 n64Inc )
{
	m_n64BagSilver = n64Value;
	tagRoleSilverChange e(_T("Bag_Silver"), NULL);
	e.n64Silver = n64Value;
	e.n64Inc = n64Inc;
	m_pFrameMgr->SendEvent(&e);
}

VOID CurrencyMgr::SetBagYuanbao( INT nValue, INT nInc )
{
	m_nBagYuanbao = nValue;
	tagRoleYuanbaoChange e(_T("Bag_Yuanbao"), NULL);
	e.nYuanbao = nValue;
	e.nInc = nInc;
	m_pFrameMgr->SendEvent(&e);
}

VOID CurrencyMgr::SetWareSilver( INT64 n64Value, INT64 n64Inc )
{
	m_n64WareSilver = n64Value;
	tagRoleSilverChange e(_T("Ware_Silver"), NULL);
	e.n64Silver = n64Value;
	e.n64Inc = n64Inc;
	m_pFrameMgr->SendEvent(&e);
}

VOID CurrencyMgr::SetBaibaoYuanbao( INT nValue, INT nInc )
{
	m_nBaibaoYuanbao = nValue;
	tagRoleYuanbaoChange e(_T("Baibao_Yuanbao"), NULL);
	e.nYuanbao = nValue;
	e.nInc = nInc;
	m_pFrameMgr->SendEvent(&e);

	// 发送游戏事件让onlinetips刷新
	m_pFrameMgr->SendEvent(&tagGameEvent( _T("Refresh_Onlinetips"), NULL ));
}

VOID CurrencyMgr::SetClanContribute(ECLanType eCtype, INT nValue, INT nInc /*= 0*/ )
{
	if (JDG_VALID(ECLT, eCtype))
	{
		m_nClanContribute[eCtype] = nValue;
		tagRoleClanContributeChange e(_T("ClanContributeChange"), NULL);
		e.eClanType = eCtype;
		e.nCurClanCon = nValue;
		e.nChangeClanCon = nInc;
		m_pFrameMgr->SendEvent(&e);
	}
}

DWORD CurrencyMgr::NetInitRoleCurrency( tagNS_GetRoleInitState_Money* pNetCmd, DWORD )
{
	SetBagSilver(pNetCmd->n64BagSilver);
	SetBagYuanbao(pNetCmd->nBagYuanBao);
	SetWareSilver(pNetCmd->n64WareSilver);
	SetBaibaoYuanbao(pNetCmd->nBaiBaoYuanBao);
	SetExchangeVolume(pNetCmd->nExchangeVolume);
	for (int i = ECLT_BEGIN; i < ECLT_NUM; ++i)
		m_nClanContribute[i] = pNetCmd->nCurClanCon[i];

	// 发送声望属性初始化请求 
	tagNC_GetRoleInitState m;
	m.eType = ERIT_Reputation;
	if (m_pSession->IsConnect())
		m_pSession->Send(&m);

    // 发送宠物初始化请求
    PetManager::Inst()->BeginNC_GetPetAttr(RoleMgr::Inst()->GetLocalPlayerID(), GT_INVALID);

    // 发送元宝交易初始化请球
    YuanbaoTradeMgr::Inst()->BeginNC_SynYBTradeInfo();

	return 0;
}

DWORD CurrencyMgr::NetGetRoleBagSilver( tagNS_BagSilver* pNetCmd, DWORD )
{
	SetBagSilver(pNetCmd->n64CurSilver, pNetCmd->n64ChangeSilver);

	return 0;
}

DWORD CurrencyMgr::NetGetRoleBagYuanbao( tagNS_BagYuanBao* pNetCmd, DWORD )
{
	SetBagYuanbao(pNetCmd->nCurYuanBao, pNetCmd->nChangeYuanBao);

	return 0;
}

DWORD CurrencyMgr::NetGetRoleWareSilver( tagNS_WareSilver* pNetCmd, DWORD )
{
	SetWareSilver(pNetCmd->n64CurSilver, pNetCmd->n64ChangeSilver);

	return 0;
}

DWORD CurrencyMgr::NetGetRoleBaibaoYuanbao( tagNS_BaiBaoYuanBao* pNetCmd, DWORD )
{
	SetBaibaoYuanbao(pNetCmd->nCurYuanBao, pNetCmd->nChangeYuanBao);

	return 0;
}

DWORD CurrencyMgr::NetGetRoleClanContribute( tagNS_ClanCon* pNetCmd, DWORD )
{
	SetClanContribute((ECLanType)pNetCmd->byClanType, pNetCmd->nCurClanCon, pNetCmd->nChangeClanCon);

	return 0;
}

tstring CurrencyMgr::GetSilverDisplay( INT64 n64Silver )
{
	TCHAR szSilver[X_LONG_NAME] = {0};
	TCHAR szGold[X_LONG_NAME] = {0};
	INT nSilver = MSilver2DBSilver(n64Silver);
	if (nSilver > 0)
	{
		_sntprintf(szSilver, sizeof(szSilver)/sizeof(TCHAR), g_StrTable[_T("Silver_Display")], nSilver);
	}
	INT nGold = MSilver2DBGold(n64Silver);
	if (nGold > 0)
	{
		_sntprintf(szGold, sizeof(szGold)/sizeof(TCHAR), g_StrTable[_T("Gold_Display")], nGold);
	}
	tstring strRet = szGold;
	strRet += szSilver;
	
	return strRet;
}

tstring CurrencyMgr::GetYuanbaoDisplay( INT nYuanbao )
{
	TCHAR szYuanbao[X_LONG_NAME] = {0};
	if (nYuanbao > 0)
	{
		_sntprintf(szYuanbao, sizeof(szYuanbao)/sizeof(TCHAR), g_StrTable[_T("Yuanbao_Display")], nYuanbao);
	}
	tstring strRet = szYuanbao;
	
	return strRet;
}

DWORD CurrencyMgr::NetGetRoleExchangeVolume( tagNS_ExchangeVolume* pNetCmd, DWORD )
{
	SetExchangeVolume(pNetCmd->nCurExVolume, pNetCmd->nChangeExVolume);

	return 0;
}

VOID CurrencyMgr::SetExchangeVolume( INT nValue, INT nInc /*= 0*/ )
{
	m_nExchangeVolume = nValue;
	tagExchangeVolumeChange evt(_T("tagExchangeVolumeChange"), NULL);
	evt.nCurExVolume = nValue;
	evt.nChangeExVolume = nInc;
	m_pFrameMgr->SendEvent(&evt);
}