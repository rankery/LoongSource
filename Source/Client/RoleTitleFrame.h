/*! \file RoleTitleFrame.h
	\brief 角色称号界面框架
	\author hyu
	\date 2009-2-2
*/
#pragma once
#include "RoleTitleProtoData.h"
#include "TreeEx.h"
#include "..\WorldDefine\msg_role_title.h"

/*!
	\struct tagTitleList
	\brief 保存的控件中保存的title list信息
*/
struct tagTitleList
{
	bool		bOwend;		/*!< 是否获得 */
    DWORD       dwTypeID;   /*!< 所属类型id */
	DWORD		dwItemID;	/*!< tree上item的id */
};

struct tagTitleTypeActivedCount
{
    tstring     strName;    /*!< 类别名 */
    INT         bCount;     /*!< 激活数 */
    INT         nTotal;     /*!< 总数 */
    DWORD       dwTreeItemID;   /*!< 控件item id*/
};

/*! 
	\class RoleTitleFrame
	\brief 角色称号框架
	\base GameFrame
	\sa RoleTitleFrame, CharacterFrame
*/
class RoleTitleFrame : public GameFrame
{
public:
	/*! 构造函数 */
	RoleTitleFrame(void);

	/*! 析构函数 */
	~RoleTitleFrame(void);

	//! 重写初始化
	/*!
		\param pMgr 游戏框架管理器
		\param pFather 父框架
		\param dwParam 附加参数
		\return 初始化是否成功
	*/
	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);

	/*! 重写销毁 */
	virtual BOOL Destroy();

	/*! 重写加载UI */
	virtual BOOL ReloadUI();

	/*! 重写刷新 */
	virtual VOID Update();

	//! 刷新称号界面
	/*! 在CharactarFrame显示整个窗体时更新当前角色称号 */
	VOID UpdateRoleTitle();

private:
	/*! GUI事件回调 */
	DWORD EventHandler(tagGUIEvent* pEvent);

	/*! 游戏事件回调 */
	DWORD OnGameEvent(tagGameEvent* pGameEvent);

	//! 网络消息回调
    /*! 设置角色使用某称号 */
    DWORD OnNS_ActiveRoleTitle(tagNS_ActiveRoleTitle* pMsg, DWORD dwParam);
	/*! 得到角色已获得的所有称号 */
	DWORD OnNS_GetRoleTitles(tagNS_GetRoleTitles* pMsg, DWORD dwParam);
	/*! 角色获得一个新称号，用来通知客户端 */
	DWORD OnNS_NewTitles(tagNS_NewTitles* pMsg, DWORD dwParam);
    /*! 角色删除一个称号，用来通知客户端 */
    DWORD OnNS_DelTitles(tagNS_DelTitles* pMsg, DWORD dwParam);

	//! 界面过程相关

	/*! 选中某一称号 */
	VOID SelectRoleTitle();
	/*! 点击使用某一称号 */
	VOID ActiveRoleTitle();
	/*! 点击清除某一称号 */
	VOID ClearRoleTitle();
	/*! 初始化所有称号xml数据到控件 */
	VOID InitTree();
	/*! 跟据称号变化刷新控件 */
	VOID UpdateTree();

	//! 私有成员
	TSFPTrunk<RoleTitleFrame>		m_Trunk;
	TObjRef<GUISystem>				m_pGUI;
	TObjRef<Util>					m_pUtil;
	TObjRef<GameFrameMgr>			m_pFrameMgr;
	TObjRef<NetCmdMgr>				m_pCmdMgr;
	TObjRef<NetSession>				m_pSession;

	UINT16							m_un16ActivedTitleID;	/*!< 角色正在使用的称号,空为GT_INVALID */
	map<DWORD, tagTitleList>		m_mapOwnedTitles;		/*!< 保存角色已获得称号的容器 */
    map<DWORD, tagTitleTypeActivedCount> m_mapActivedCount; /*!< 此类title获得的数量 */

	//! GUI控件
	GUIWnd*							m_pWnd;					/*!< 本窗体 */
	GUIWnd*							m_pWndFather;			/*!< 父窗体 */

	TreeEx*							m_pTreeTitleList;		/*!< 称号列表 */
	GUIScrollBar*					m_pScrBarTitleList;		/*!< 滚动条 */
	GUIButton*						m_pBtTitleListUp;		/*!< 上 */
	GUIButton*						m_pBtTitleListDown;		/*!< 下 */

	GUIStaticEx*					m_pStcexDescription;	/*!< 显示称号描述 */
	GUIScrollBar*					m_pScrBarDescription;	/*!< 滚动条 */
	GUIButton*						m_pBtDescriptionUp;		/*!< 上 */
	GUIButton*						m_pBtDescriptionDown;	/*!< 下 */

	GUIButton*						m_pBtClear;				/*!< 清除 */
	GUIButton*						m_pBtUse;				/*!< 使用 */

	GUIStatic*						m_pStcPercentage;		/*!< 达成率 */

};
