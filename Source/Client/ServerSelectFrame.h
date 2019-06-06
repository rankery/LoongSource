#pragma once

#define RECOMMEND_GAMEWORLD 8
#define BOROUGH_MAX			44
#define GAMEWORLD_MAX		16

struct tagGameWorld											//游戏世界
{
	DWORD	dwWorldRank;									//游戏世界的编号
	TCHAR	szWorldName[X_SHORT_NAME];						//游戏世界的名
	DWORD	dwWorldID;										//大区ID
	INT		nStatus;										//游戏世界的状态				
	DWORD	dwStatus;										//状态的颜色
	DWORD	dwWorldIP;										//游戏世界的IP
	DWORD	dwWorldPort;									//游戏世界的port
	BYTE	byRecommend;									//是否为推荐游戏世界
	DWORD	dwRecoRank;										//推荐游戏世界的编号
	BYTE	byGreenFlag;									//绿色标记
};

struct tagBorough											//大区
{
	DWORD					dwBoroughRank;					//游戏世界的编号
	TCHAR					szBoroughName[X_SHORT_NAME];	//大区名
	DWORD					dwBoroughID;					//大区ID
	INT						dwWorldNum;						//大区下游戏世界个数
	vector<tagGameWorld>	vGameWorld;						//大区下游戏世界
};
/** 大区、服务器选择
*/

class Client;
class ServerSelectFrame : public GameFrame
{
public:
	ServerSelectFrame();
	~ServerSelectFrame();

	//--GameFrame
	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();

private:
	BOOL EventHandler(tagGUIEvent* pEvent);

	// 得到服务器返回的游戏世界状态颜色
	DWORD GetWorldStatusColor(const INT nWorldStatus);

	// 计算日期
	void CalDate();

	//确认登录
	void ConfirmLogin();

	//帐号注册
	void AccountRegister();

	//帐号充值
	void AccountAbundance();

	//退出游戏
	void ExitGame();

	//读取大区游戏世界的配置文件
	void LoadConfigFile();

	//读取上次登录过的游戏世界
	void LoadLastLogin();

	//设置推荐游戏世界状态
	void SetRecommedPushButtonState();

	//设置游戏世界按钮状态
	void SetWorldPushButtonState();

	//显示游戏世界按钮
	void ShowWorldPushButton();

	//显示今天\明天的活动任务
	void ShowServerActive();

private:
	TSFPTrunk<ServerSelectFrame>m_Trunk;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<Util>				m_pUtil;
	TObjRef<GameFrameMgr>		m_pFrameMgr;
	TObjRef<StreamTransport>	m_pStream;
	TObjRef<VarContainer>		m_pVarContainer;
	TObjRef<VarContainer>		m_pVarIPSaver;
	TObjRef<Client>				m_pClient;

	GUIWnd*						m_pWnd;
	GUIWnd*						m_pWndCaption;
	GUIPushButton*				m_pRecommend[RECOMMEND_GAMEWORLD];	//推荐游戏世界按钮
	GUIPushButton*				m_pBorough[BOROUGH_MAX];			//大区按钮
	GUIPushButton*				m_pGameWorld[GAMEWORLD_MAX];		//游戏世界按钮
	GUIPushButton*				m_pLastLogin;						//上次登录的游戏世界按钮		
	GUIStatic*					m_pLastServer;						//上次登录的大区
	GUIButton*					m_pOK;								//确认登录
	GUIButton*					m_pRegister;						//注册
	GUIButton*					m_pAbundance;						//充值
	GUIButton*					m_pExit;							//离开
	GUIListBox*					m_pToday;							//今日活动
	GUIListBox*					m_pTomorrow;

	GUIWnd*						m_pWnd_Back;						//大背景
	GUIStatic*					m_pStc_Pic;							//背景贴图

	INT							m_nBorough;							//选择的大号编号
	INT							m_nGameWorld;						//选择的游戏世界编号
	INT							m_nRecoWorld;						//选择的推荐游戏世界编号
	vector<tagBorough>			m_vBrough;							//大区
	vector<tagGameWorld>		m_vRecommend;						//推荐的游戏世界

	tstring						m_strRegister;						//注册帐号的网址
	tstring						m_strAbundance;						//帐号充值的网址

	tstring						m_strLastIP;						//上次登录的ip
	DWORD						m_dwLastPort;						//上次登录的port
	tstring						m_strLastWorldName;					//上次登录的游戏世界
	tstring						m_strLastServer;					//上次登录的大区

	tstring						m_strGameWorld;						//选择的游戏世界名
	tstring						m_strIP;							//选择的游戏世界IP
	DWORD						m_dwPort;							//选择的游戏世界Port
};