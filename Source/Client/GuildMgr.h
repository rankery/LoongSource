#pragma once

class GuildMembers;
struct tagGuildBase;
struct tagGuildMemberClient;
struct tagGuildMember;
struct tagGuildPower;
struct tagGuildKick;
struct tagGuildAppoint;
struct tagRoleGetNameEvent;
struct tagInviteJoinGuildEvent;
struct tagNS_GuildCreateBroad;
struct tagNS_GuildDismissBroad;
struct tagNS_GuildJoinReq;
struct tagNS_GuildJoinReqRes;
struct tagNS_GuildJoinBroad;
struct tagNS_GuildProcFailed;
struct tagNS_GuildLeaveBroad;
struct tagNS_GuildKickBroad;
struct tagNS_GuildTurnoverBroad;
struct tagNS_GuildDemissBroad;
struct tagNS_GuildAppointBroad;
struct tagNS_GuildTenetChangeBroad;
struct tagNS_GetRoleInitState_Guild;
struct tagNS_GetGuildName;
struct tagNS_GetGuildTenet;
struct tagNS_GuildGetAllMembers;
struct tagNS_GuildGetMemberEx;
struct tagNS_GuildRefMember;
struct tagNS_SpreadGuildAffair;
struct tagNS_SyncGuildInfo;
struct tagNS_GetGuildSkillInfo;
struct tagGuildSkillInfo;
struct tagNS_SetResearchSkill;
struct tagNS_LearnGuildSkill;
struct tagGuildAffairInfo;
struct tagNS_GuildContribution;
struct tagNS_GuildStateSet;
struct tagNS_GuildStateUnset;
struct tagNS_GuildSkillUpgrade;
struct tagNS_GuildSkillLevelUp;
struct tagNS_AcceptCommerce;
struct tagNS_GetTaelInfo;
struct tagNS_CompleteCommerce;
struct tagNS_AbandonCommerce;
struct tagNS_GuildUpgrade;
struct tagNS_GetCofCInfo;
struct tagNS_GetCommodityInfo;
struct tagCommerceInfo;
struct tagGoodInfo;
struct tagCOCInfo;
struct tagNS_GetCommerceRank;
struct tagNS_SwitchCommendation;
struct tagNS_GetGuildWare;
struct tagNS_GuildWarePri;
struct tagNS_GuildExploit;
struct tagNS_GetSomeGuildName;

#include "..\WorldDefine\guild_define.h"
/** \class	GuildMgr
	\brief	帮派管理器
*/
class GuildMgr
{
public:
	GuildMgr(void);
	~GuildMgr(void);

	enum EReqReason
	{
		ERQ_NoSelfRight,			// 自己没权限邀请
		ERQ_AlreadyInSelfGuild,		// 已经在本帮派
		ERQ_AlreadyInOtherGuild,	// 已经在其他帮派
		ERQ_OtherReason				// 其他错误信息
	};

	static GuildMgr* Inst();
	
	VOID Init();
	VOID Destroy();
	
	void Update();
	//
	VOID SortMembers(INT nCol);

	const tagGuildMemberClient* GetMember(INT nIndex) const;
	const tagGuildMemberClient* GetMemberByID(DWORD dwRoleID) const;
	INT GetMemberIndex(DWORD dwRoleID) const;
	
	
	void SendGetSomeGuildName(int nFlag, const std::vector<DWORD> &vecIDList);
	tstring GetGuildName(DWORD dwGuildID);

	// 这条消息不会向服务发出申请的
	bool IsGuildNameExist(DWORD dwGuildID);

	tstring GetGuildTenet() const {return m_strTenet;}
	tstring GetMemberSex(BYTE bySex) const;
	tstring GetMemberPos(INT nPos) const;
	INT32   GetGuildPurchase()const { return m_pGuild->nGroupPurchase; }

	const GuildMembers* GetMembers() const;
	const tagGuildBase*	GetGuildInfo() const;
	const tagGuildMember* GetLpInfo() const;

	bool IsCanJionReq(DWORD dwRoleID, DWORD *pReaseon = NULL) const;

	const tagGuildPower* GetPowerList(INT nPos) const;
	const tagGuildKick*	GetKickList(INT nPos) const;
	const tagGuildAppoint* GetAppointList(INT nPos) const;

	const std::vector<tagGuildAffairInfo*>& GetAffairInfo() const { return m_vecAffairInfo; }
	const std::vector<DWORD>& GetSkill() const { return m_vecSkill; }
	DWORD GetCurrentResearchSkillID() const { return m_dwCurResearchSkillID; }
	INT32 GetCurrentResearchSkillProgress() const { return m_nCurResearchProgress; }
	INT32 GetCurrentOwnCommerceSilver() const { return m_nOwnCommerceSilver; }
	INT GetRunForBusinessLevel() const { return m_nRunForBusinessLevel; }
	DWORD GetCurrentTalkCOCNPCID() const { return m_dwCurTalkCOCNPCID; }
	DWORD GetCurrentCOCID() const { return m_dwCurCOCID; }
	
	const std::map<DWORD, tagCommerceGoodInfo>& GetCurrentCOCGoods() const { return m_mapCOCGoods; }
	const std::map<DWORD, tagCommerceGoodInfo>& GetCurrentPlayerGoods() const { return m_mapPlayerGoods; }

	VOID SendGuildCreate(LPCTSTR szGuildName, DWORD dwNPCID);
	VOID SendGuildDismiss(DWORD dwNPCID);
	VOID SendGuildJoinReq(DWORD dwRoleID);
	VOID SendGuildAllMembersGet();
	VOID SendSpreadGuildAffair(DWORD dwID);
	VOID SendGetGuildSkillInfo();
	VOID SendResearchSkill(DWORD dwID);
	VOID SendLearnGuildSkill(DWORD dwID);
	// 申请帮派信息同步
	VOID SendSyncGuildInfo(const tstring &strFrameName);
	VOID SendGuildSkillUpgrade(INT64 n64Serial);
	// 申请接取跑商
	VOID SendAcceptCommerce();
	// 获取跑商初始信息
	VOID SendGetTaelInfo();
	// 上缴商银
	VOID SendCompleteCommerce();
	// 放弃跑商
	VOID SendQuitCommerce();
	// 获取商会商货信息和玩家商货信息
	VOID SendGetCofCInfo(DWORD dwNPCID);
	// 关闭商会
	VOID SendCloseCOC(DWORD dwNPCID);
	// 买商品
	VOID SendBuyCOCGoods(DWORD dwNPCID, DWORD dwGoodID, BYTE byBuyNum);
	// 卖商品
	VOID SendSellCOCGoods(DWORD dwNPCID, DWORD dwGoodID, BYTE bySellNum);
	// 申请跑商排名信息
	VOID SendGetCommerceRank();
	// 设置跑商嘉奖状态
	VOID SendSwitchCommendation(BOOL b);
	

	// 显示门派相关错误码
	VOID ShowGuildErrorMsg(DWORD dwErrorCode);
	// 聊天频道显示门派信息
	void ShowGuildMsg( const TCHAR* szFormat,... );

	// 查找帮派技能信息;
	const tagGuildSkillInfo* FindSkillInfo(DWORD dwSkillID);
	// 根据玩家等级查找跑商信息
	const tagCommerceInfo *FindCommerceInfo(INT nPlayerLevel);
	// 根据ID查找商品信息
	const tagGoodInfo* FindGoodInfo(DWORD dwGoodID);
	// 根据ID查找商会信息
	const tagCOCInfo* FindCOCInfo(DWORD dwCOCID);
	
	//判断帮派是否处于某种状态
	bool IsInSpecState(EGuildSpecState eState) const;
	// 判断该城市是否被帮会所拥有
	bool IsBeOwned(BYTE byCity);
	// 取得当前打开的商会占领城市ID
	BYTE GetCurrentCOCHoldCity() const { return m_byCurHoldCity; }

    // 仓库相关

    //! 获取当前仓库
    void BeginNC_GetGuildWare(DWORD dwNPCID);
    DWORD OnNS_GetGuildWare(tagNS_GetGuildWare* pMsg, DWORD);
    //! 设置仓库权限
    void BeginNC_GuildWarePri(DWORD npcID, DWORD roleID, BOOL canUse);
    //! 仓库设置变更
    DWORD OnNS_GuildWarePri(tagNS_GuildWarePri* pMsg, DWORD);

    //! 设置是否需要初始化仓库
    void SetifNeedInitWarehouse(bool bVal)
    {
        m_bifNeedInitWarehouse  =   bVal;
        m_dwWarehouseLastUpTime =   0;
    }

    // 本地玩家是否有操作仓库权限
    bool isWareOpretable();

private:
	VOID InitPosPower();
	
	// 初始化帮派事务
	void InitGuildAffairs();
	// 初始化帮派技能表
	void InitGuildSkills();
	// 初始化跑商表
	void InitGuildCommerce();
	// 初始化商品信息表
	void InitGoodsInfo();
	// 初始化商会信息表
	void InitCOCInfo();

	VOID AddLocalPlayer();
	VOID ShowJoinReqMsgBox();

	DWORD OnEventGetRoleName(tagRoleGetNameEvent* pGameEvent);
	DWORD OnEventGuildCreate(tagGameEvent* pGameEvent);
	DWORD OnEventGuildDismiss(tagGameEvent* pGameEvent);
	DWORD OnEventGuildOpen(tagGameEvent* pGameEvent);
	DWORD OnEventGuildClose(tagGameEvent* pGameEvent);
	DWORD OnEventGuildAffairOpen(tagGameEvent *pEvent);
	DWORD OnEventGuildAffairClose(tagGameEvent *pEvent);
	DWORD OnEventPickUpSilverOpen(tagGameEvent *pEvent);
	DWORD OnEventPickUpSilverClose(tagGameEvent *pEvent);
	DWORD OnEventContributeSilverOpen(tagGameEvent *pEvent);
	DWORD OnEventContributeSilverClose(tagGameEvent *pEvent);
	DWORD OnEventCOCOpen(tagGameEvent *pEvent);
	DWORD OnEventCOCClose(tagGameEvent *pEvent);
	DWORD OnEventGuildInvite(tagInviteJoinGuildEvent* pGameEvent);
	DWORD OnEventMsgBoxJoinReq(tagMsgBoxEvent* pGameEvent);
	DWORD OnEventMsgBoxInputName(tagMsgInputBoxEvent* pGameEvent);
	DWORD OnEventMsgBoxDismissCheck(tagMsgBoxEvent* pGameEvent);
	DWORD OnEventMsgBoxQuitCommerceWarning(tagMsgBoxEvent* pGameEvent);
	DWORD OnEventGuildUpgrade(tagGameEvent *pEvent);
	DWORD OnEventQuitCommerce(tagGameEvent *pEvent);
    DWORD OnEventOpenGuildStorage(tagGameEvent *pEvent);
    DWORD OnEventOpenGuildStoragePermission(tagGameEvent *pEvent);
	DWORD OnEventGuildCommerceRankOpen(tagGameEvent *pEvent);
	DWORD OnEventGuildCommerceRankClose(tagGameEvent *pEvent);
	DWORD OnEventGuildGetTenet(tagGameEvent *pEvent);
	

	DWORD OnNetGuildAttInit(tagNS_GetRoleInitState_Guild* pNetCmd, DWORD);
	DWORD OnNetGuildNameGet(tagNS_GetGuildName* pNetCmd, DWORD);
	DWORD OnNetGuildTenetGet(tagNS_GetGuildTenet* pNetCmd, DWORD);
	DWORD OnNetGuildGetAllMembers(tagNS_GuildGetAllMembers* pNetCmd, DWORD);
	DWORD OnNetGuildGetMemberEx(tagNS_GuildGetMemberEx* pNetCmd, DWORD);
	DWORD OnNetGuildRefMember(tagNS_GuildRefMember* pNetCmd, DWORD);
	DWORD OnNetGuildCreateBroad(tagNS_GuildCreateBroad* pNetCmd, DWORD);
	DWORD OnNetGuildDismissBroad(tagNS_GuildDismissBroad* pNetCmd, DWORD);
	DWORD OnNetGuildJoinReq(tagNS_GuildJoinReq* pNetCmd, DWORD);
	DWORD OnNetGuildJoinReqRes(tagNS_GuildJoinReqRes* pNetCmd, DWORD);
	DWORD OnNetGuildJoinBroad(tagNS_GuildJoinBroad* pNetCmd, DWORD);
	DWORD OnNetGuildProcFailed(tagNS_GuildProcFailed* pNetCmd, DWORD);
	DWORD OnNetGuildLeaveBroad(tagNS_GuildLeaveBroad* pNetCmd, DWORD);
	DWORD OnNetGuildKickBroad(tagNS_GuildKickBroad* pNetCmd, DWORD);
	DWORD OnNetGuildTurnoverBroad(tagNS_GuildTurnoverBroad* pNetCmd, DWORD);
	DWORD OnNetGuildDemissBroad(tagNS_GuildDemissBroad* pNetCmd, DWORD);
	DWORD OnNetGuildAppointBroad(tagNS_GuildAppointBroad* pNetCmd, DWORD);
	DWORD OnNetGuildTenetChangeBroad(tagNS_GuildTenetChangeBroad* pNetCmd, DWORD);
	DWORD OnNetGuildSpreadAffair(tagNS_SpreadGuildAffair *pNetCmd, DWORD);
	DWORD OnNetGuildSyncInfo(tagNS_SyncGuildInfo *pNetCmd, DWORD);
	DWORD OnNetGetGuildSkillInfo(tagNS_GetGuildSkillInfo *pNetCmd, DWORD);
	DWORD OnNetSetResearchSkill(tagNS_SetResearchSkill *pNetCmd, DWORD);
	DWORD OnNetLearnGuildSkill(tagNS_LearnGuildSkill *pNetCmd, DWORD);
	DWORD OnNetGuildContribution(tagNS_GuildContribution *pNetCmd, DWORD);
	DWORD OnNetGuildStateSet(tagNS_GuildStateSet *pNetCmd, DWORD);
	DWORD OnNetGuildStateUnSet(tagNS_GuildStateUnset *pNetCmd, DWORD);
	DWORD OnNetGuildSkillUpgrade(tagNS_GuildSkillUpgrade *pNetCmd, DWORD);
	DWORD OnNetGuildSkillLevelUp(tagNS_GuildSkillLevelUp *pNetCmd, DWORD);
	DWORD OnNetAcceptCommerce(tagNS_AcceptCommerce *pNetCmd, DWORD);
	DWORD OnNetGetTaelInfo(tagNS_GetTaelInfo *pNetCmd, DWORD);
	DWORD OnNetCompleteCommerce(tagNS_CompleteCommerce *pNetCmd, DWORD);
	DWORD OnNetAbandonCommerce(tagNS_AbandonCommerce *pNetCmd, DWORD);
	DWORD OnNetGuildUpgrade(tagNS_GuildUpgrade *pNetCmd, DWORD);
	DWORD OnNetGetCommodityInfo(tagNS_GetCommodityInfo *pNetCmd, DWORD);
	DWORD OnNetGetCofCInfo(tagNS_GetCofCInfo *pNetCmd, DWORD);
	DWORD OnNetGetCommerceRank(tagNS_GetCommerceRank *pNetCmd, DWORD);
	DWORD OnNetSwitchCommendation(tagNS_SwitchCommendation *pNetCmd, DWORD);
	DWORD OnNetGuildExploit(tagNS_GuildExploit *pNetCmd, DWORD);
	DWORD OnNetGetSomeGuildName(tagNS_GetSomeGuildName *pNetCmd, DWORD);

	// 清空帮派信息
	VOID ClearGuildInfo();
	// 设置本地玩家职位信息
	VOID SetLocalPlayerGuildPos(INT8 n8Pos);
	// 清空帮派事务表
	VOID ClearAffairs();
	// 清空帮派技能表
	VOID ClearSkills();
	// 清空帮跑跑商表
	VOID ClearCommerces();
	// 清空商品信息表
	VOID ClearGoodInfo();
	// 清空商会信息表
	VOID ClearCOCInfo();
	

private:
	TSFPTrunk<GuildMgr>		m_Trunk;
	TObjRef<NetSession>		m_pSession;
	TObjRef<NetCmdMgr>		m_pCmdMgr;
	TObjRef<GameFrameMgr>	m_pFrameMgr;

	tagGuildBase*					m_pGuild;		//所在帮派信息
	std::map<DWORD, tstring>		m_mapGuildName;	//帮派名称列表
	std::set<DWORD>					m_setNameReq;	//帮派名称请求缓存表
	GuildMembers*					m_pMembers;		//帮派成员列表
	tagGuildMember*					m_pLpInfo;		//本地玩家成员信息
	tstring							m_strTenet;		//帮派遵旨
	std::vector<DWORD>				m_vecSkill;		//当前所有帮派技能
	DWORD							m_dwCurResearchSkillID;	// 当前研究技能
	INT32 							m_nCurResearchProgress; // 当前研究技能进度
	INT32							m_nOwnCommerceSilver;   // 角色拥有商银
	DWORD							m_dwTempCompleteCommercePlayerID;	// 临时保存完成跑商角色ID
	INT32							m_nTempFund;			// 临时保存缴纳商银时帮派增加的资金
	
	DWORD							m_dwCurCOCID;			// 当前打开的商会ID
	DWORD							m_dwCurTalkCOCNPCID;	// 当前对话的商会NPCID
	
	BYTE							m_byCurHoldCity;		// 特产商会
	INT								m_nRunForBusinessLevel; // 接取跑商时玩家等级
	std::map<DWORD, tagCommerceGoodInfo> m_mapCOCGoods;			// 暂存当前打开的商会所拥有商品，与服务器同步
	std::map<DWORD, tagCommerceGoodInfo> m_mapPlayerGoods;		// 角色拥有商品，与服务器同步




	std::vector<tagGuildPower>		m_vecPower;		//帮派职位职能表
	std::vector<tagGuildKick>		m_vecKick;		//帮派职位开革表
	std::vector<tagGuildAppoint>	m_vecAppoint;	//帮派职位任命表
	
	std::vector<tagGuildAffairInfo*> m_vecAffairInfo;	// 帮派事务表
	std::map<DWORD, tagGuildSkillInfo*>	 m_mapSkill;	// 帮派技能表
	std::vector<tagCommerceInfo*> m_vecCommerceInfo;	// 帮派跑商表
	std::map<DWORD, tagGoodInfo*> m_mapGoodInfo;		// 跑商物品表
	std::map<DWORD, tagCOCInfo*> m_mapCOCInfo;			// 商会信息表
	std::deque<tstring>				 m_syncFrameNames;	// 记录申请同步的frame的名字
	

	DWORD							m_dwInviterID;		//请求者ID
	DWORD							m_dwInviterGuild;	//请求者所在帮派
	BOOL							m_bIsInitFinish;	//是否初始化完成
	
	DWORD							m_dwLastSyncTime;	// 上次发送同步帮派信息消息的时间
	DWORD							m_dwLastUpdateTime;	// 上次update的时间
	
	static const DWORD				SYNC_TIME_LIMIT = 0 * 1000;  
	static const DWORD				UPDATE_TIME_LIMIT = 5 * 1000;

    bool                            m_bifNeedInitWarehouse; // 是否需要初始化仓库容器
    DWORD                           m_dwWarehouseLastUpTime;// 仓库上次时间戳

	bool m_bSendGetAllMember;
	bool m_bGetTenet;

};
