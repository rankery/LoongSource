#pragma once

class ClanTreasureFrame;

#include "ProgressEx.h"
#include "ReputeFrame.h"
#include "PlayerNameTab.h"
#include "..\WorldDefine\msg_famehall.h"

const INT MAX_RANK_PLACE			=	17;		/*!< 窗体每页容纳的最多声望排名项目数 */
const INT MAX_FAMOUS_TOP_PLACE		=	12;		/*!< 窗体首页容纳的最多名人数 */
const INT MAX_FAMOUS_COMMON_PLACE	=	19;		/*!< 窗体名人其它页容纳最多名人数 */

/*!
	\struct tagOpenClanTreasure
	\brief 通知打开氏族珍宝列表
	\base tagGameEvent
	\sa tagGameEvent
*/
struct tagOpenClanTreasure : public tagGameEvent
{
	ECLanType			eClanTypeValue;			/*!< 氏族类型 */
	tagOpenClanTreasure(LPCTSTR szEventName, ECLanType eType, GameFrame* pSenderFrame)
		: tagGameEvent(szEventName, pSenderFrame), eClanTypeValue(eType){}
};

/*! 声望数据结构 */
struct tagRankData
{
	DWORD		dwRoleID;		/*!< 角色ID */
	tstring		strRoleName;	/*!< 角色名 */
	INT			nReputeValue;	/*!< 声望值 */
};

/*! 名人数据结构 */
struct tagFameData
{
	DWORD		dwNameID;		/*!< 姓名ID */
	tstring		strName;		/*!< 姓名 */
};

/*!
	\class FameHallFrame
	\brief 名人堂界面
	\author hyu
	\base GameFrame
	\remark
*/
class FameHallFrame : public GameFrame
{
public:
	FameHallFrame(void);
	~FameHallFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();

    /*! 刷新数据 */
    VOID UpdateFameHall();

private:


	/*! 处理打开/关闭本窗体的消息 */
	DWORD OnOpenFamousEvent(tagGameEvent* pGameEvent);
	DWORD OnCloseFamousEvent(tagGameEvent* pGameEvent);

	/*! 处理氏族珍宝界面的消息 */
	DWORD OnClanTreaOpen(tagGameEvent* pGameEvent);
	DWORD OnClanTreaClose(tagGameEvent* pGameEvent);

	/*! 显示窗体 */
	VOID ShowWnd(bool bVal);

	/*! 处理GUI事件 */
	DWORD EventHandler(tagGUIEvent* pEvent);

	/*! 设置当前选择的氏族页 */
	VOID SetClan(ECLanType eVal);

	/*! 声望排名列表相关 */

	/*! 发送获得声望列表请求 */
	VOID BeginGetReputRank(ECLanType eVal);
	/*! 返回声望排名列表 */
	DWORD OnNS_GetReputeTop(tagNS_GetReputeTop* pMsg, DWORD param);
	/*! 计算容器相关其它数据 */
	VOID FillReputRank(ECLanType eVal);
	/*! 请求获得NameTable角色名字事件返回处理 */
	DWORD OnGetRankName(tagRoleGetNameEvent* pGameEvent);
	/*! 将容器的声望排名数据更新到界面 */
	VOID RefreshRankToUI(ECLanType eVal);
	/*! 选择上一页 */
	VOID PrevRankPage();
	/*! 选择下一页 */
	VOID NextRankPage();
	/*! 清除容器中的排名数据 */
	VOID ClearRankData(ECLanType eVal);
	/*! 清除容器中所有排名数据 */
	VOID SafeDeleteRankData();

	/*! 名人堂列表相关 */

	/*! 发送获得名人堂列表请求 */
	VOID BeginGetFame(ECLanType eVal);
	/*! 返回名人堂列表 */
	DWORD OnNS_GetFameHallRoles(tagNS_GetFameHallRoles* pMsg, DWORD param);
	/*! 计算容器相关其它数据 */
	VOID FillFameData(ECLanType eVal);
	/*! 请求获得NameTablebyNameid名字事件返回处理 */
	DWORD OnGetNameByNameID(tagGetNameByNameID* pGameEvent);
	/*! 将容器的名人堂列表更新到界面 */
	VOID RefreshFameToUI(ECLanType eVal);
	/*! 选择上一页 */
	VOID PrevFamePage();
	/*! 选择下一页 */
	VOID NextFamePage();
	/*! 清除容器中的名人数据 */
	VOID ClearFameData(ECLanType eVal);
	/*! 清除容器中所有名人数据 */
	VOID SafeDeleteFameData();

	TSFPTrunk<FameHallFrame>	m_Trunk;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<Util>				m_pUtil;
	TObjRef<GameFrameMgr>		m_pFrameMgr;
	TObjRef<NetSession>			m_pSession;
	TObjRef<NetCmdMgr>			m_pCmdMgr;

	/*! GUI控件 */
	GUIWnd*						m_pWnd;										/*!< 本窗体 */

	GUIPushButton*				m_pBtClans[ECLT_NUM];						/*!< 氏族分页按钮 */

	GUIStatic*					m_pStcRoleName[MAX_RANK_PLACE];				/*!< 玩家名 */
	ProgressEx*					m_pPrgrss[MAX_RANK_PLACE];					/*!< 声望值 */
	GUIStatic*					m_pStcReputeLevel[MAX_RANK_PLACE];			/*!< 声望级别 */
	GUIStatic*					m_pStcRankPage;								/*!< 页数 */
	GUIButton*					m_pBtRankPrevPage;							/*!< 上一页 */
	GUIButton*					m_pBtRankNextPage;							/*!< 下一页 */

	GUIStatic*					m_pStcFameTopPage;							/*!< 名人首页 */
	GUIStatic*					m_pStcFamousTop[MAX_FAMOUS_TOP_PLACE];		/*!< 名人首页名位 */
	GUIStatic*					m_pStcFameCommonPage;						/*!< 名人第其它页 */
	GUIStatic*					m_pStcFamousCommon[MAX_FAMOUS_COMMON_PLACE];/*!< 名人其它页名位 */
	GUIStatic*					m_pStcFamousPage;							/*!< 页数 */
	GUIButton*					m_pBtFamousPrevPage;						/*!< 上一页 */
	GUIButton*					m_pBtFamousNextPage;						/*!< 下一页 */

	GUIButton*					m_pBtEnterClanTreasure;						/*!< 进入氏族珍宝按钮 */
	GUIButton*					m_pBtClose;									/*!< 窗体关闭按钮 */

	/*! 氏族珍宝相关 */
	ClanTreasureFrame*			m_pClanTreasure;							/*!< 氏族珍宝Frame */
	bool						m_bisClanTreasureOpen;						/*!< 氏族珍宝窗体状态 */

	/*! 容器 */
	ECLanType					m_eSelectedClan;							/*!< 当前选择的氏族 */

	vector<tagRankData*>		m_vecRankArray[ECLT_NUM];					/*!< 存放声望排名数据 */
	map<DWORD, tagRankData*>	m_mapRoleIDtoRank[ECLT_NUM];				/*!< 通过roleid快速查找排名数据 */
	INT							m_nRankPageCount[ECLT_NUM];					/*!< 声望排名最大页数 */
	INT							m_nRankCurrPage[ECLT_NUM];					/*!< 声望排名当前页 */
	DWORD						m_dwRankUpdateTime[ECLT_NUM];				/*!< 服务器的声望排名时间戳 */

	vector<tagFameData*>		m_vecFameNameIDs[ECLT_NUM];					/*!< 存放名人堂数据 */
	map<DWORD, tagFameData*>	m_mapNameIDtoFame[ECLT_NUM];				/*!< 通过nameid快速查找排名数据 */
	INT							m_nFamePageCount[ECLT_NUM];					/*!< 名人最大页数 */
	INT							m_nFameCurrPage[ECLT_NUM];					/*!< 名人当前页 */
	DWORD						m_dwFameUpdateTime[ECLT_NUM];				/*!< 服务器的名人排名时间戳 */

};
