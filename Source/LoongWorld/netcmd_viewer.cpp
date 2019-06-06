#include "StdAfx.h"
#include "netcmd_viewer.h"
//#include "net_define.h"
#include "../WorldDefine/NetCmdViewer_Msg.h"
#include "../WorldDefine/time.h"


NetCmdViewer::NetCmdViewer()
: m_Trunk(this)
{
	m_pMsgQueue = NULL;
	m_bTerminated = TRUE;
	m_nIndex = 0;
	m_pAccountList = new AccountList;
	memset(m_pBuf, 0, sizeof(m_pBuf));
}

	
UINT NetCmdViewer::ThreadWriteLog()
{
	INT nFreeCount = 0;
	while(!m_bTerminated)
	{
		DWORD msgSize;
		LPBYTE lpByte = m_pMsgQueue->GetMsg(msgSize);
		while(lpByte)
		{			
			nFreeCount = 0;
			if(msgSize < sizeof(m_pBuf))
			{
				if(msgSize + m_nIndex > sizeof(m_pBuf) - 1)
					FlushToDisk();
				memcpy(m_pBuf + m_nIndex, lpByte, msgSize);
				m_nIndex += msgSize;
			}	
			m_pMsgQueue->FreeMsg(lpByte);
			lpByte = m_pMsgQueue->GetMsg(msgSize);
		}
		::WaitForSingleObject(m_pMsgQueue->GetEvent(), 200);
		nFreeCount++;
		if(nFreeCount > 5) // free
		{
			FlushToDisk();
			nFreeCount = 0;
		}
	}
	FlushToDisk();
	return 0;
}

void NetCmdViewer::FlushToDisk()
{
	if(m_nIndex > 0 && m_dwFileHandle!= INVALID_FILE_HANDLE)
	{
		m_pDiscIOMgr->Seek( m_dwFileHandle, 0, GT_SEEK_END );
		m_pDiscIOMgr->Write(m_dwFileHandle, m_pBuf, m_nIndex);
	}
	m_nIndex = 0;
}

NetCmdViewer& NetCmdViewer::getInstance()
{
	static NetCmdViewer _instance;
	return _instance;
}

void NetCmdViewer::Log(DWORD accountID, DWORD roleID, const tagNetCmd& msgCmd)
{
	AccountList* pList = m_pAccountList;
	if(std::binary_search(pList->begin(), pList->end(), accountID))
	{
		size_t msgSize = sizeof(NetCmdViewer_Msg)-sizeof(tagNetCmd) + msgCmd.dwSize;
		NetCmdViewer_Msg* pMsg = (NetCmdViewer_Msg*)new BYTE[msgSize];
		pMsg->dwMsgSize = msgSize;
		pMsg->liTime = GetCurrentDWORDTime();
		pMsg->accountID = accountID;
		pMsg->roleID = roleID;
		memcpy(&pMsg->netCmd, &msgCmd, msgCmd.dwSize);
		m_pMsgQueue->AddMsg((LPCVOID)pMsg, msgSize);
	}
}

BOOL NetCmdViewer::Init()
{
	m_pThread		=	"Thread";


	m_pMsgQueue = new MsgQueueTS(TRUE, FALSE);
	m_bTerminated = FALSE;

	CreateFile();

	// 启动线程
	if(!m_pThread->CreateThread(_T("NetCmdViewer"), 
		(vEngine::THREADPROC)m_Trunk.sfp0(&NetCmdViewer::ThreadWriteLog), NULL))
	{
		return FALSE;
	}

	while( !m_pThread->IsThreadActive(_T("NetCmdViewer")) )
	{
		continue;
	}

	RegisterCmd();
	return TRUE;
}

void NetCmdViewer::Destroy()
{
	InterlockedExchange((LPLONG)&m_bTerminated, TRUE);
	m_pThread->WaitForThreadDestroy(_T("ChatLogger"), INFINITE);
	if( m_dwFileHandle != INVALID_FILE_HANDLE )
	{
		m_pDiscIOMgr->Close( m_dwFileHandle );
		m_dwFileHandle = INVALID_FILE_HANDLE;
	}
	SAFE_DEL(m_pMsgQueue);
	SAFE_DEL(m_pAccountList);
	for(std::vector<AccountList*>::const_iterator itor= m_History.begin(); itor != m_History.end(); itor++)
		delete *itor;
	m_History.clear();
}

BOOL NetCmdViewer::CreateFile()
{
	if( m_dwFileHandle != INVALID_FILE_HANDLE )
		m_pDiscIOMgr->Close( m_dwFileHandle );

	// 生成合适的log文件名
	TCHAR szTime[MAX_PATH], szTemp[MAX_PATH];
	GetModuleFileName(NULL, szTemp, MAX_PATH);

	// 截去路径
	TCHAR *pResult = _tcsrchr(szTemp, _T('\\'));
	pResult = pResult ?	pResult+1 :	pResult = (TCHAR *)szTemp;
	_stprintf(m_szDefaultName, _T("log\\%s"), pResult);
	// 截去文件扩展名
	pResult = _tcsrchr(m_szDefaultName, _T('.'));
	if( pResult )
		*pResult = '\0';	
	// 加上时间
	FILETIME CurrentTime;
	GetSystemTimeAsFileTime(&CurrentTime);
	szTime[0] = _T('\0');
	WORD wDate, wTime;
	if (FileTimeToLocalFileTime(&CurrentTime, &CurrentTime) &&
		FileTimeToDosDateTime(&CurrentTime, &wDate, &wTime))
	{
		// 年月日以及时分秒和进程id
		wsprintf(szTime, _T("[%d-%d-%d %02d%02d%02d %05d]netcmdviewer.log"),
			(wDate / 32) & 15, wDate & 31, (wDate / 512) + 1980,
			(wTime >> 11), (wTime >> 5) & 0x3F, (wTime & 0x1F) * 2, 
			GetCurrentProcessId());
		_tcscat(m_szDefaultName, szTime);
	}

	m_dwFileHandle = m_pDiscIOMgr->Create(m_szDefaultName, GENERIC_WRITE, 
		FILE_SHARE_READ, NULL, CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, NULL);

	return ( INVALID_FILE_HANDLE != m_dwFileHandle );
}

void NetCmdViewer::RegisterCmd()
{
	TObjRef<Console> pConsole = "Console";
	pConsole->Register(_T("lognetcmd"), m_Trunk.sfp1(&NetCmdViewer::LogNetCmd), _T("lognetcmd accountid"), 1);
	pConsole->Register(_T("unlognetcmd"), m_Trunk.sfp1(&NetCmdViewer::UnLogNetCmd), _T("unlognetcmd accountid"), 1);
}

DWORD NetCmdViewer::LogNetCmd(LPCTSTR strAccountID)
{
	DWORD accountID = _wtoi(strAccountID);
	if(std::binary_search(m_pAccountList->begin(), m_pAccountList->end(), accountID))
		return 0;
	AccountList* pVect = new AccountList;
	for(AccountList::const_iterator itor= m_pAccountList->begin(); itor != m_pAccountList->end(); itor++)
		pVect->push_back(*itor);
	pVect->push_back(accountID);
	std::sort(pVect->begin(), pVect->end()); //sort
	m_History.push_back(m_pAccountList);
	InterlockedExchange((LPLONG)&m_pAccountList, LONG(pVect));
	return 0;
}

DWORD NetCmdViewer::UnLogNetCmd(LPCTSTR strAccountID)
{
	DWORD accountID = _wtoi(strAccountID);
	if(!std::binary_search(m_pAccountList->begin(), m_pAccountList->end(), accountID))
		return 0;
	AccountList* pVect = new AccountList;
	for(AccountList::const_iterator itor= m_pAccountList->begin(); itor != m_pAccountList->end(); itor++)
	{
		if(*itor != accountID)
			pVect->push_back(*itor);
	}
	// already sorted
	m_History.push_back(m_pAccountList);
	InterlockedExchange((LPLONG)&m_pAccountList, LONG(pVect));
	return 0;
}

