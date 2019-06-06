#pragma once
#include "gameframe.h"
#include "GuildMembers.h"

struct tagRoleGetNameEvent;
struct tagGetNameByNameID;
struct tagGuildPosAppointEvent;
struct tagGuildInfoReflashEvent;
struct tagGuildNameGetEvent;
struct tagGuildBase;
struct tagGuildMemberClient;
struct tagUseGuildSkillBookEvent;



class IconStatic;
struct tagGuildSkill;
class Item;

#define GUILD_SKILL_ITEM_NUM_PERPAGE 12

class GuildFrame :
	public GameFrame
{
public:
	typedef vector<GUIButton*> GUIButtonList;
	
	struct tagMemberNameReq
	{
		INT		nIndex;
		DWORD	dwColor;

		tagMemberNameReq(INT nIdx, DWORD dwClr){nIndex = nIdx; dwColor = dwClr;}
	};

	typedef map<DWORD, tagMemberNameReq> NameReqList;  
	
	// 帮派界面状态
	enum EGuildFrameState
	{
		EGFS_MemberList, 
		EGFS_GuildInfo,
		EGFS_GuildSkill
	};

public:
	GuildFrame(void);
	~GuildFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();

	virtual BOOL ReloadUI();
	virtual BOOL EscCancel();

	EGuildFrameState GetFrameState() const { return m_eGuildFrameState; }
	VOID ShowWnd(bool bShow){ m_bShow = bShow; m_pWnd->SetInvisible(!bShow);}
	bool IsShow() const { return m_bShow; }
private:

	VOID EventHandler(tagGUIEvent* pEvent);
	
	// 刷新帮派
	VOID ReflashGuild();
	// 刷新列表
	VOID ReflashList();
	// 刷新列表IMPL
	VOID ReflashListImpl();
	// 刷新功能按钮
	VOID ReflashButton();
	// 刷新技能
	void RefreshSkills();
	

	// 获取离线时间显示
	tstring GetOfflineTime(bool bIsOnline, DWORD dwOfflineTime = GT_INVALID);
	
	// 发送网络消息
	VOID SendGuildAppoint(DWORD dwDstRoleID, INT nPos);
	VOID SendGuildLeave();
	VOID SendGuildKick(DWORD dwDstRoleID);
	VOID SendGuildTurnover(DWORD dwDstRoleID);
	VOID SendGuildDemiss();
	VOID SendGuildTenetChange(tstring strTenetInfo);
	
	// 处理游戏事件
	DWORD OnEventGetRoleName(tagRoleGetNameEvent* pGameEvent);
	DWORD OnEventGetNameByNameID(tagGetNameByNameID* pGameEvent);
	DWORD OnEventGetGuildName(tagGuildNameGetEvent* pGameEvent);
	DWORD OnEventGetGuildTenet(tagGameEvent* pGameEvent);
	DWORD OnEventGuildPosAppoint(tagGuildPosAppointEvent* pGameEvent);
	DWORD OnEventGuildInfoReflash(tagGuildInfoReflashEvent* pGameEvent);
	DWORD OnEventGuildListReflash(tagGameEvent* pGameEvent);
	DWORD OnEventGuildInfoReq(tagGuildInfoReflashEvent* pGameEvent);
	DWORD OnEventMsgBoxInputTenet(tagMsgInputBoxEvent* pGameEvent);
	DWORD OnEventMsgBoxLeaveCheck(tagMsgBoxEvent* pGameEvent);
	DWORD OnEventMsgBoxTurnoverCheck(tagMsgBoxEvent* pGameEvent);
	DWORD OnEventMsgBoxKickCheck(tagMsgBoxEvent* pGameEvent);
	DWORD OnEventMsgBoxDemissCheck(tagMsgBoxEvent* pGameEvent);
	DWORD OnEventRefreshGuildSkills(tagGameEvent *pEvent);
	DWORD OnEventShowGuildFrame(tagGameEvent *pEvent);
	DWORD OnEventHideGuildFrame(tagGameEvent *pEvent);
	DWORD OnEventRefreshGuild(tagGameEvent *pEvent);
	DWORD OnEventUsGuildSkillBook(tagUseGuildSkillBookEvent *pGameEvent);
	DWORD OnEventRefreshBook(tagGameEvent *pGameEvent);
	DWORD OnEventCommerceWarningMsgBox(tagMsgBoxEvent *pEvent);
	// 处理按钮按下事件
	VOID OnClickedLookupInfo();
	VOID OnClickedLeaveGuild();
	VOID OnClickedTurnoverLeader();
	VOID OnClickedAppointMember();
	VOID OnClickedModifyTenet();
	VOID OnClickedKickMember();
	VOID OnClickedDemissPostion();

	VOID DestroyMsgBox();
	void SetSkillDefaultUI();
	void OnSkillButtonClick(int nButton);		// 选中某个技能button
	void OnClickPreOrNextButton(bool bOrder);	// true为后退，false为前进

	// 获取当前行的成员
	const tagGuildMemberClient* GetMember(INT nIndex);
	
private:
	TSFPTrunk<GuildFrame>		m_Trunk;
	TObjRef<NetSession>			m_pSession;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<GameFrameMgr>		m_pFrameMgr;

	NameReqList					m_mapTemp;
	vector<INT>					m_vecIndex;			// 成员索引缓存
	GuildMembers::EListColType	m_eSortType;		// 排序方式


	GUIWnd*						m_pWnd;
	GUIWnd*						m_pWndMembers;
	GUIButton*					m_pBtnClose;
	GUIListBox*					m_pLstMembers;		// 成员列表
	GUIStatic*					m_pStcName;			// 帮派名称
	GUIStatic*					m_pStcLevel;		// 等级
	IconStatic*					m_pStcSymbol;		// 帮派标志
	GUIStatic*					m_pStcFounder;		// 创始人
	GUIStatic*					m_pStcLeader;		// 帮主
	GUIStatic*					m_pStcMemberNum;	// 成员数量	
	GUIStatic*					m_pStcReputation;	// 威望
	

	GUIStatic*					m_pStcFund;			// 资金
	GUIStatic*					m_pStcMaterial;		// 资材
	GUIStatic*					m_pStcPeace;		// 安宁度
	GUIStatic*					m_pStcDailyCost;	// 每日维护费
	GUIEditBoxEx*				m_pEdtTenet;		// 帮派宗旨
	GUIPushButton*				m_pPbnMembers;		// 成员列表
	GUIPushButton*				m_pPbnSkills;		// 帮派技能
	GUIPushButton*				m_pPbnInfomation;	// 帮派信息
	
	GUIButtonList				m_pBtnListIndex;	// 成员列表表头
	GUIButton*					m_pBtnLookupInfo;	// 查看信息
	GUIButton*					m_pBtnLeaveGuild;	// 离开帮派
	GUIButton*					m_pBtnTurnLeader;	// 移交帮主
	GUIButton*					m_pBtnAppointMember;// 任命职位
	GUIButton*					m_pBtnModifyTenet;	// 修改宗旨
	GUIButton*					m_pBtnKickMember;	// 开革成员
	GUIButton*					m_pBtnDemissPostion;// 辞去职务
	GUIPushButton*				m_pPbnOffline;		// 显示离线

	GUIPatch*					m_pGuildMemberPatch;

	const tagGuildBase*			m_pGuild;			// 帮派信息

	DWORD						m_dwDstRoleID;		// 操作的目标ID

	GUIWnd *m_pWndGuild;			// 父窗体
	IconStatic *m_pBtnCurSkillIcon;	// 当前研究技能图标
	GUIStatic *m_pStcCurSkillName;	// 当前研究技能名称
	GUIStatic *m_pStcCurSkillLevel; // 当前研究技能等级
	GUIStatic *m_pStcFundCost;		// 消耗帮派资金
	GUIStatic *m_pStcMaterialCost;	// 消耗帮派资材
	GUIProgress * m_pProResearchProgress; //当前技能研究进度

	IconStatic *m_pBtnBookIcon;		// 放入典籍
	GUIStatic *m_pProgressIncrement; // 进度增量
	GUIButton *m_pLearnBook;		// 精研典籍

	GUIButton *m_pBtnBack;
	GUIButton *m_pBtnNext;

	GUIStatic *m_pStcLearnGoldCost;			// 学习所需金子
	GUIStatic *m_pStcLearnSilverCost;		// 学习所需银子
	GUIStatic *m_pStcLearnContributionNeed; // 学习所需帮派贡献
	GUIStatic *m_pStcLearnGuildFundCost;	// 学习所需帮派资金
	GUIStatic *m_pStcLearnGuildMaterialCost;// 学习所需帮派资材

	GUIButton *m_pBtnLearnSkill;			// 学习技能
	GUIButton *m_pBtnCancelResearch;		// 取消研究
	GUIButton *m_pBtnStartResearch;			// 开始研究

	GUIPatch *m_pSkillPatch;
	IconStatic *m_pIstState[5];				// 帮派状态
	
	IconStatic *m_pSkillItemBack[GUILD_SKILL_ITEM_NUM_PERPAGE];
	IconStatic *m_pSkillItem[GUILD_SKILL_ITEM_NUM_PERPAGE];
	GUIStatic *m_pSkillName[GUILD_SKILL_ITEM_NUM_PERPAGE];
	GUIStatic *m_pSkillLevel[GUILD_SKILL_ITEM_NUM_PERPAGE];
	int m_nCurPageNum;	// 当前翻页数
	DWORD m_dwCurChooseSkill; // 当前选中帮派技能ID;
	std::vector<tagGuildSkill*> m_vecCurPageItem;
	
	EGuildFrameState m_eGuildFrameState;
	bool m_bShow;
	Item *m_pSkillBook;			// 保存典籍的指针

	DWORD m_dwTempRoleID; // 临时保存角色ID
	INT m_nTempPOS;     // 临时保存POSID
};
