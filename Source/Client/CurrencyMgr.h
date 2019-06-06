#pragma once
#include "..\WorldDefine\reputation.h"

struct tagNS_GetRoleInitState_Money;
struct tagNS_BagSilver;
struct tagNS_BagYuanBao;
struct tagNS_WareSilver;
struct tagNS_BaiBaoYuanBao;
struct tagNS_ClanCon;
struct tagNS_ExchangeVolume;

struct tagRoleSilverChange : public tagGameEvent
{
	INT64 n64Silver;
	INT64 n64Inc;

	tagRoleSilverChange(LPCTSTR szEventName, GameFrame* pSenderFrame)
		: tagGameEvent(szEventName, pSenderFrame){}
};

struct tagRoleYuanbaoChange : public tagGameEvent
{
	INT nYuanbao;
	INT nInc;

	tagRoleYuanbaoChange(LPCTSTR szEventName, GameFrame* pSenderFrame)
		: tagGameEvent(szEventName, pSenderFrame){}
};

struct tagRoleClanContributeChange : public tagGameEvent
{
	ECLanType eClanType;
	INT nCurClanCon;
	INT nChangeClanCon;

	tagRoleClanContributeChange(LPCTSTR szEventName, GameFrame* pSenderFrame)
		: tagGameEvent(szEventName, pSenderFrame){}
};

// by add xtian 2009-6-17
struct tagExchangeVolumeChange : public tagGameEvent
{
	INT32		nCurExVolume;		// 当前总的赠点
	INT32		nChangeExVolume;	// 变化的赠点
	tagExchangeVolumeChange(LPCTSTR szEventName, GameFrame* pSenderFrame)
		: tagGameEvent(szEventName, pSenderFrame){}
};

/** \class CurrencyMgr
	\brief 货币管理器 包括银两，元宝，氏族贡献值
*/
class CurrencyMgr
{
public:
	CurrencyMgr(void);
	~CurrencyMgr(void);

	VOID Init();
	VOID Destroy();

	//----------------------------------------------------
	//获取
	//----------------------------------------------------
	//获取背包中的银两
	INT64 GetBagSilver(){return m_n64BagSilver;}
	//获取背包中的元宝
	INT	GetBagYuanbao(){return m_nBagYuanbao;}
	//获取仓库中的银两
	INT64 GetWareSilver(){return m_n64WareSilver;}
	//获取仓库中的元宝
	INT	GetBaibaoYuanbao(){return m_nBaibaoYuanbao;}
	//获取取氏族贡献
	INT GetClanContribute(ECLanType eCtype){return JDG_VALID(ECLT, eCtype) ? m_nClanContribute[eCtype] : -1;}
    INT GetClanContributeEx(ERContributionType eRCtype){return JDG_VALID(ERCT, eRCtype) ? m_nClanContribute[eRCtype - ERCT_BEGIN] : -1;}
	//获得兑换卷
	INT GetExchangeVolume() { return m_nExchangeVolume; }
	
	// 金钱界面显示
	tstring GetSilverDisplay(INT64 n64Silver);
	// 元宝界面显示
	tstring GetYuanbaoDisplay(INT nYuanbao);

	static CurrencyMgr* Inst();

private:

	//----------------------------------------------------
	//赋值
	//----------------------------------------------------
	//设置背包中的银两
	VOID SetBagSilver(INT64 n64Value, INT64 n64Inc = 0);
	//设置背包中的元宝
	VOID SetBagYuanbao(INT nValue, INT nInc = 0);
	//设置仓库中的银两
	VOID SetWareSilver(INT64 n64Value, INT64 n64Inc = 0);
	//设置仓库中的元宝
	VOID SetBaibaoYuanbao(INT nValue, INT nInc = 0);
	//设置氏族贡献
	VOID SetClanContribute(ECLanType eCtype, INT nValue, INT nInc = 0);
	//设置兑换卷 by add xtian 2009-6-17
	VOID SetExchangeVolume(INT nValue, INT nInc = 0);

	//----------------------------------------------------
	//网络消息处理
	//----------------------------------------------------
	//处理玩家货币初始化
	DWORD NetInitRoleCurrency(tagNS_GetRoleInitState_Money* pNetCmd, DWORD);
	//处理玩家背包中的银两变化
	DWORD NetGetRoleBagSilver(tagNS_BagSilver* pNetCmd, DWORD);
	//处理玩家背包中的元宝变化
	DWORD NetGetRoleBagYuanbao(tagNS_BagYuanBao* pNetCmd, DWORD);
	//处理玩家仓库中的银两变化
	DWORD NetGetRoleWareSilver(tagNS_WareSilver* pNetCmd, DWORD);
	//处理玩家仓库中的元宝变化
	DWORD NetGetRoleBaibaoYuanbao(tagNS_BaiBaoYuanBao* pNetCmd, DWORD);
	//处理玩家氏族贡献变化
	DWORD NetGetRoleClanContribute(tagNS_ClanCon* pNetCmd, DWORD);
	//处理玩家兑换卷变化 by add xtian 2009-6-17
	DWORD NetGetRoleExchangeVolume(tagNS_ExchangeVolume* pNetCmd, DWORD);

private:
	TSFPTrunk<CurrencyMgr>		m_Trunk;
	TObjRef<Util>				m_pUtil;
	TObjRef<NetCmdMgr>			m_pCmdMgr;
	TObjRef<GameFrameMgr>		m_pFrameMgr;
	TObjRef<NetSession>			m_pSession;

	INT64	m_n64BagSilver;
	INT64	m_n64WareSilver;
	INT		m_nBagYuanbao;
	INT		m_nBaibaoYuanbao;
	INT		m_nClanContribute[ECLT_NUM];
	INT		m_nExchangeVolume;
};
