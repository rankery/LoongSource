#pragma once
#include "proof_policy.h"
#include "proof_policy_orange_define.h"

//-----------------------------------------------------------------------------
// 该验证策略的数据库
//-----------------------------------------------------------------------------
class ProofPolicyOrangeDB
{
public:
	//-------------------------------------------------------------------------
	// CONSTRUCT
	//-------------------------------------------------------------------------
	ProofPolicyOrangeDB() {}
	~ProofPolicyOrangeDB() {}

	//-------------------------------------------------------------------------
	// 初始化
	//-------------------------------------------------------------------------
	BOOL	Init();

	//-------------------------------------------------------------------------
	// 查询
	//-------------------------------------------------------------------------
	BOOL	QueryAccount(LPCSTR szAccountName, string& strPsd, tagProofResult* pResult);
	BOOL	QueryAccount(DWORD dwAccountID, string& strAccoutName,string& strIP);

	BOOL	InsertAccout(LPCSTR szAccountName, LPCSTR szPsd, LPCSTR szMiBao, BOOL bGuard);
	BOOL	UpdateAccout(LPCSTR szAccountName, LPCSTR szPsd, LPCSTR szMiBao, BOOL bGuard);


private:
	DataBase		m_DB;
};

class ProofPolicyOrange :
	public ProofPolicy
{
public:
	ProofPolicyOrange(void): m_Trunk(this), m_nLoginPort(0),m_nLogoutPort(0), m_bTerminateLoginUpdate(FALSE), m_bTerminateLoginConnect(FALSE)
		, m_bTerminateLogoutUpdate(FALSE), m_bTerminateLogoutConnect(FALSE){}
	~ProofPolicyOrange(void){}

public:
	//-------------------------------------------------------------------------
	// 初始化和销毁
	//-------------------------------------------------------------------------
	BOOL	Init(PROOFCALLBACK fn);
	VOID	Destroy();

	//-------------------------------------------------------------------------
	// 验证接口
	//-------------------------------------------------------------------------
	VOID	Proof(DWORD dwClientID, LPCSTR szAccoutName, LPCSTR szPsd, LPCSTR szGUID,DWORD dwIP);

	//-------------------------------------------------------------------------
	// 验证服务器连接状态
	//-------------------------------------------------------------------------
	INT		GetProofServerStatus();

	//-------------------------------------------------------------------------
	// 玩家登出事件
	//-------------------------------------------------------------------------
	VOID	SigPlayerLogoutEvent(DWORD dwAccountID);

private:

	//-----------------------------------------------------------------------------------------------
	// 得到各个数据项 单引号分隔
	//-----------------------------------------------------------------------------------------------
	VOID Proof_GetOrangeData(const string &strFrameString,vector <string> &vecOrangeData,char cDivideSign);

	//-------------------------------------------------------------------------
	// 更新login服务器的线程
	//-------------------------------------------------------------------------
	VOID	ThreadLoginServerUpdate();

	//-------------------------------------------------------------------------
	// 更新logout服务器的线程
	//-------------------------------------------------------------------------
	VOID	ThreadLogoutServerUpdate();

	//-------------------------------------------------------------------------
	// 连接对方login服务器线程
	//-------------------------------------------------------------------------
	VOID	ThreadConnectLoginServer();

	//-------------------------------------------------------------------------
	// 连接对方logout服务器线程
	//-------------------------------------------------------------------------
	VOID	ThreadConnectLogoutServer();

	//-------------------------------------------------------------------------
	// 初始化配置
	//-------------------------------------------------------------------------
	BOOL	InitConfig();

	//-------------------------------------------------------------------------
	// 更新验证列表
	//-------------------------------------------------------------------------
	VOID	UpdateProofList();

	//-------------------------------------------------------------------------
	// 更新login服务器消息
	//-------------------------------------------------------------------------
	VOID	UpdateLoginSession();

	//-------------------------------------------------------------------------
	// 更新logout服务器消息
	//-------------------------------------------------------------------------
	VOID	UpdateLogoutSession();

	//-------------------------------------------------------------------------
	// 处理Orange验证信息
	//-------------------------------------------------------------------------
	VOID	HandleOrangeLogin(LPBYTE pRecv);

	//-------------------------------------------------------------------------
	// 更新登出列表
	//-------------------------------------------------------------------------
	VOID UpdatePlayerLogouList();

	//-----------------------------------------------------------------------------------------------
	// 去掉回车换行
	//-----------------------------------------------------------------------------------------------
	BOOL CheckString(string& str);

private:

	TSFPTrunk<ProofPolicyOrange>	m_Trunk;
	TObjRef<Util>				m_pUtil;
	TObjRef<Thread>				m_pThread;

	//登入
	TObjRef<StreamTransport>	m_pTransLogin;

	//登出
	TObjRef<StreamTransport>	m_pTransLogout;


	//在ThreadUpdate工作 不需要考虑线程安全
	std::map<DWORD, string>		m_mapClientIDVsAccount;	// ClientID和玩家账号的对照表
	//-------------------------------------------------------------------------
	// 线程结束标志
	//-------------------------------------------------------------------------
	volatile BOOL				m_bTerminateLoginUpdate;
	volatile BOOL				m_bTerminateLoginConnect;

	volatile BOOL				m_bTerminateLogoutUpdate;
	volatile BOOL				m_bTerminateLogoutConnect;
	//-------------------------------------------------------------------------
	// 连接参数
	//-------------------------------------------------------------------------
	CHAR						m_szLoginIP[X_IP_LEN];		// 连接orange认证服务器的IP
	INT							m_nLoginPort;				// 连接orange认证服务器的端口

	CHAR						m_szLogoutIP[X_IP_LEN];		// 连接orange认证服务器的IP
	INT							m_nLogoutPort;				// 连接orange认证服务器的端口

	CHAR						m_szServiceCode[X_OG_SC_LEN];		// 由遊戲橘子提供
	CHAR						m_szRegion[X_OG_REGION_LEN];		// 由遊戲橘子提供

	//-------------------------------------------------------------------------
	// 验证数据
	//-------------------------------------------------------------------------
	struct tagPlayerProofData
	{
		DWORD		dwClientID;
		string		strAccoutName;
		string		strPsd;
		BYTE		byAccountData[X_LONG_NAME];		//
		DWORD		dwIP;				//ip

		tagPlayerProofData(DWORD _dwClientID, LPCSTR szAccoutName, LPCSTR szPsd, LPCSTR szGUID,DWORD _dwIP)
		{
			dwClientID		=	_dwClientID;
			strAccoutName	=	szAccoutName;
			strPsd			=	szPsd;
			memcpy(byAccountData,szGUID,X_LONG_NAME);
			dwIP			=	_dwIP;
		}
	};

	TSafeList<tagPlayerProofData*>		m_listProofData;		// 验证用户数据

	TSafeList<DWORD>					m_listPlayerLogout;		// 玩家登出列队			
	//--------------------------------------------------------------------------
	// 数据库
	//--------------------------------------------------------------------------
	ProofPolicyOrangeDB					m_ProofDB;				// 数据库接口
};
