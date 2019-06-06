#pragma once

class Player;
struct tagNS_JoinGame;

// Frame抽象基类
class FrameBase
{
	friend class FrameMgr;

public:
	FrameBase();
	virtual ~FrameBase() = 0;

	virtual VOID Init() = 0;
	virtual VOID Update() = 0;
	virtual VOID Destroy() = 0;

protected:
	DWORD				m_dwStateTime;
	Player*				m_pPlayer;
	BYTE				m_eFrameType;
};

// LoginFrame
class LoginFrame : public FrameBase
{
public:
	LoginFrame(Player* pPlayer);
	~LoginFrame();

	VOID Init();
	VOID Update();
	VOID Destroy();

private:
	TSFPTrunk<LoginFrame>		m_Trunk;
	TObjRef<Util>				m_pUtil;
	TObjRef<VarContainer>		m_pVarContainer;
	TObjRef<NetCmdMgr>			m_pCmdMgr;

	TObjRef<XClient>			m_pWorldSession;	//与world的连接
	TObjRef<XClient>			m_pLoginSession;	//与login的连接

	enum ELoginState
	{
		ELS_Abandon,
		ELS_WaitRetry,
		ELS_Connect,
		ELS_WaitResponse,
		ELS_WaitProofResult,
		ELS_Queue,
		ELS_ConnectZoneServer,		// 连接区域服务器
		ELS_SentJoinGame,			// 发送JoinGame消息
		ELS_JoinGame,				// 接收到JoinGame消息后
	};

	ELoginState					m_eState;
	DWORD						m_dwZoneIP;			//选择的Zone服务器IP
	DWORD						m_dwZonePort;		//选择的Zone服务器port
	DWORD						m_dwAccountID;		//joingame用到
	DWORD						m_dwVerifyCode;		//joingame用到
	DWORD						m_dwSentJoinGame;

	VOID Connect();
	VOID WaitResponse();
	VOID WaitProofResult();	
	VOID WaitQueue();
	VOID TryIsConnectZoneSer();

	// 处理JoinGame消息
	void NetRecvJoinGame(tagNS_JoinGame* pMsg, LPVOID pPrama); 

	// 显示用户名\密码错误信息
	VOID ShowUserPwdInfo(DWORD nInfo);

	// 断开与zone服务器的连接
	VOID CutOffZone();

	// 断开与login服务器的连接
	VOID CutOffLogin();

	VOID SetState(ELoginState eState);
};

// SelectRoleFrame
struct tagNS_EnumRole;
struct tagNS_SelectRole;
struct tagNS_CreateRole;
struct tagNS_SynchronismTime;

class SelectRoleFrame : public FrameBase
{
public:
	SelectRoleFrame(Player* pPlayer);
	~SelectRoleFrame();

	VOID Init();
	VOID Update();
	VOID Destroy();

private:
	enum ESRState
	{
		ESRS_EnumRole,					//向zone服务器请求玩家的角色信息
		ESRS_SentEnumRole,				//已发送角色信息请求
		ESRS_SentCreateRole,			//发送创建角色信息
		ESRS_SentSelectRole,			//发送选择角色进入游戏世界
		ESRS_RecvEnumRole,				//接收错误的角色列表
		ESRS_RecvSelectRole,			//接收角色进入游戏世界
		ESRS_SelectedRole,				//选择了角色
		ESRS_Back,						//返回到登录
	};

	BOOL EnterGame(DWORD dwLocalRoleID);
	VOID SendEnterGameMsg();
	BOOL ReturnGame();
	BOOL CreateRole();

	VOID SetCurState(ESRState state) { m_eCurState = state; m_dwStateTime = timeGetTime();}

	// 接收玩家的角色信息
	void NetRecvEnumRole(tagNS_EnumRole* pMsg, LPVOID pPrama);

	// 接收创建角色信息
	void NetRecvCreateRole(tagNS_CreateRole* pMsg, LPVOID pPrama);

	// 接收玩家的选择角色信息
	void NetRecvSelectRole(tagNS_SelectRole* pMsg, LPVOID pPrama);

	// 客户端时间同步消息
	void NetRecvSynchronismTime(tagNS_SynchronismTime* pMsg, LPVOID pPrama);

private:
	TSFPTrunk<SelectRoleFrame>	m_Trunk;
	TObjRef<Util>				m_pUtil;
	TObjRef<NetCmdMgr>			m_pCmdMgr;

	TObjRef<XClient>			m_pWorldSession;	//与world的连接

	DWORD						m_nSelectRoleID;
	ESRState					m_eCurState;	
	DWORD						m_dwStateTime;
};

// LoongWorldFrame

struct tagNS_GotoNewMap;
struct tagNS_MoveFailed;
struct tagNS_RoleChat;

class WorldFrame : public FrameBase
{
public:
	enum EWorldState
	{
		EWState_GotoNewMap,			//接收服务发送的gotomap消息
		EWState_Playing,			//可以开始玩了
	};

public:
	WorldFrame(Player* pPlayer);
	~WorldFrame();

	VOID Init();
	VOID Update();
	VOID Destroy();

private:
	TSFPTrunk<WorldFrame>		m_Trunk;
	TObjRef<VarContainer>		m_pVarContainer;
	TObjRef<NetCmdMgr>			m_pCmdMgr;
	EWorldState					m_curState;
	TObjRef<Util>				m_pUtil;

	TObjRef<XClient>			m_pWorldSession;	//与world的连接

	DWORD						m_dwNextChatTime;
	DWORD						m_dwNextMoveTime;

	BYTE						m_byChatChannel;

	DWORD						m_dwLogoutTime;
	DWORD						m_dwOnlineTime;

	void	RegisterALLRecvCmd();
	void	UnRegisterALLRecvCmd();

	void	SwitchToState(EWorldState state);

	// 接收服务器消息
	void	NetRecvGotoMap(tagNS_GotoNewMap* pMsg, LPVOID pPrama);
	void	NetRecvMoveFailed(tagNS_MoveFailed* pMsg, LPVOID pPrama);
	void	NetRecvRoleChat(tagNS_RoleChat* pMsg, LPVOID pPrama);
	void	NetRecvDoNothing(tagNetCmd* pMsg, LPVOID pPrama);

	// 向服务器发送消息
	void	SendChatMsg();
	void	SendMoveMsg();

	// 断开与服务器联接
	void	Disconnection();

	// 下线
	BOOL	ReturnGame();
};
