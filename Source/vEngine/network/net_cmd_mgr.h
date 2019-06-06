//-----------------------------------------------------------------------------
//!\file net_cmd_mgr.h
//!\author Lyp
//!
//!\date 2005-03-08
//! last 2005-03-08
//!
//!\brief 命令管理
//-----------------------------------------------------------------------------
#pragma once

namespace vEngine {

typedef DWORD (WINAPI* NETMSGPROC)(tagNetCmd*, DWORD);
//-----------------------------------------------------------------------------
// 命令管理
//-----------------------------------------------------------------------------
class VENGINE_API NetCmdMgr
{
public:
	BOOL Init();
	VOID Destroy();

	BOOL Register(LPCSTR szCmd, NETMSGPROC fp, LPCTSTR szDesc);
	BOOL UnRegister(LPCSTR szCmd, NETMSGPROC fp);
	BOOL HandleCmd(tagNetCmd* pMsg, DWORD dwMsgSize, DWORD dwParam);
	
	std::string	GetHadleCmdString(tagNetCmd* pMsg);


protected:
	struct tagCmd
	{
		std::string			strCmd;		// 命令名
		tstring				strDesc;	// 描述
		TList<NETMSGPROC>	listFP;		// 函数指针列表
		DWORD				dwTimes;	// 收到此命令的次数
	};

	TSFPTrunk<NetCmdMgr>	m_Trunk;
	TObjRef<Util>			m_pUtil;
	TObjRef<Log>			m_pLog;
	TObjRef<Console>		m_pConsole;

	TMap<DWORD, tagCmd*>	m_mapProc;

	DWORD List();
	DWORD Find(LPCTSTR szCommand); // 根据ID查找注册的网络命令
};


}