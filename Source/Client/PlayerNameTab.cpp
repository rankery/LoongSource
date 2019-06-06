#include "stdafx.h"
#include "PlayerNameTab.h"
#include "RoleMgr.h"
#include "Role.h"
PlayerNameTab::PlayerNameTab(void):m_Trunk(this)
{
}

PlayerNameTab::~PlayerNameTab(void)
{
}

PlayerNameTab g_playerNameTab;
PlayerNameTab* PlayerNameTab::Inst()
{
	return &g_playerNameTab;
}

void PlayerNameTab::Init()
{
	m_pUtil = TObjRef<Util>();
	m_pSession = TObjRef<NetSession>();
	m_pFrameMgr = TObjRef<GameFrameMgr>();
	m_pCmdMgr = TObjRef<NetCmdMgr>();

	m_pCmdMgr->Register("NS_GetNamebyNameID",		   (NETMSGPROC)m_Trunk.sfp2(&PlayerNameTab::OnNS_GetNamebyNameID),			_T("NS_GetNamebyNameID"));
	m_pCmdMgr->Register("NS_RoleGetSomeName",		   (NETMSGPROC)m_Trunk.sfp2(&PlayerNameTab::OnNS_RoleGetSomeName),			_T("NS_RoleGetSomeName"));
    m_pCmdMgr->Register("NS_GetRoleHeadPicURL",		   (NETMSGPROC)m_Trunk.sfp2(&PlayerNameTab::OnNS_GetRoleHeadPicURL),		_T("NS_GetRoleHeadPicURL"));

	m_paReqRole.first = 0;
	m_paReqRole.second.clear();
}

void PlayerNameTab::Destroy()
{
    m_pCmdMgr->UnRegister("NS_GetNamebyNameID",		     (NETMSGPROC)m_Trunk.sfp2(&PlayerNameTab::OnNS_GetNamebyNameID));
	m_pCmdMgr->UnRegister("NS_RoleGetSomeName",		     (NETMSGPROC)m_Trunk.sfp2(&PlayerNameTab::OnNS_RoleGetSomeName));
    m_pCmdMgr->UnRegister("NS_GetRoleHeadPicURL",		 (NETMSGPROC)m_Trunk.sfp2(&PlayerNameTab::OnNS_GetRoleHeadPicURL));
    m_id2name.clear();
    m_name2id.clear();

	//--清空请求列表
	m_paReqRole.first = 0;
	m_paReqRole.second.clear();
}

void PlayerNameTab::AddRecord(DWORD id,const TCHAR* szName)
{
	tstring strTmp;
	if(szName==NULL || _tcslen(szName)<=0)
		strTmp.assign( _T("GFC_NULL") );
	else
		strTmp.assign(szName);

	m_id2name.insert(std::pair<DWORD,tstring>(id,strTmp));
	m_name2id.insert(std::pair<tstring,DWORD>(strTmp,id));
}


// 每帧更新
void PlayerNameTab::Update()
{
	// 当请求列表不为空，且待发时间大于500毫秒时，向服务器发送角色数据请求
	if( !m_paReqRole.second.empty() && 500 < Kernel::Inst()->GetAccumTimeDW() - m_paReqRole.first )
	{
		tagNC_RoleGetSomeName NetCmdData;
		NetCmdData.nNum = m_paReqRole.second.size() > 50 ? 50 : m_paReqRole.second.size();
		NetCmdData.nUserData = 0;
		// 结构的空间减去代表指针的dwAllID[1]的空间再加上实际上SIZE个大小的DWORD空间等于新结构的大小
		int size = sizeof(tagNC_RoleGetSomeName) - sizeof(DWORD) + sizeof(DWORD) * NetCmdData.nNum;
		// 开辟改大小的数组
		BYTE byAllocData[sizeof(tagNC_RoleGetSomeName) - sizeof(DWORD) + sizeof(DWORD) * 50];
		tagNC_RoleGetSomeName *pRGSCmd = (tagNC_RoleGetSomeName*)byAllocData;

		// 拷贝除了指针之前的所有数据,因为是POD型,是安全的.
		memcpy(pRGSCmd, &NetCmdData, (sizeof(NetCmdData)));
		// ID赋值啦
		for (INT i = 0; i < NetCmdData.nNum; ++i)
		{
			pRGSCmd->dwAllID[i] = m_paReqRole.second.front();
			m_paReqRole.second.pop_front();
		}
		// 设置新的大小
		pRGSCmd->dwSize = size;
		m_pSession->Send(pRGSCmd);
		m_paReqRole.first = Kernel::Inst()->GetAccumTimeDW();
	}
}


DWORD PlayerNameTab::FindIDByName(const TCHAR* szName)
{
	ASSERT(szName!=NULL && GT_VALID(szName));

	map<tstring,DWORD>::iterator it=m_name2id.find((tstring)(szName));
	if(it!=m_name2id.end() && (*it).second!=GT_INVALID )
	{
		return (*it).second;
	}
	else
	{
		TCHAR szBuff[X_SHORT_NAME] = {0};
		_tcsncpy(szBuff, szName, X_SHORT_NAME-1);
		_tcslwr(szBuff);
		DWORD dwCRC = m_pUtil->Crc32(szBuff);
		///已经在请求队列中
		if(m_mapName2IDReq.find(dwCRC)!=m_mapName2IDReq.end())
			return GT_INVALID;

		//如果PlayerNameTab中没有保存对应Name的ID		
		if(m_pSession->IsConnect())
		{
			tagNC_RoleGetID m;
			m.dwRoleNameCrc = dwCRC;
			m_pSession->Send(&m);

			//保存请求缓存
			m_mapName2IDReq[dwCRC]=szName;
		}
		else
			return GT_INVALID;
	}

	return GT_INVALID;
}

const TCHAR* PlayerNameTab::FindNameByID(DWORD id)
{
	if( !IS_PLAYER(id) )
		return _T("");

	map<DWORD,tstring>::iterator it=m_id2name.find(id);
	if( it!=m_id2name.end() && (*it).second!=_T("GFC_NULL") )
	{
		return (*it).second.c_str();
	}
	else
	{
		//如果PlayerNameTab中没有保存对应ID的name
		//--如果请求列表中不存在，则添加
		bool bFind=false;
		for(list<DWORD>::iterator iter=m_paReqRole.second.begin();
			iter!=m_paReqRole.second.end();++iter)
		{
			if(*iter==id)
			{
				bFind=true;
				break;
			}
		}
		if(!bFind)
		{
			m_paReqRole.second.push_back(id);
		}
	}

	return _T("");
}

bool PlayerNameTab::CanGetBundleOfNames(const std::vector<DWORD>& IDs, std::vector<DWORD>& Cantfinds)
{
	// 缓存中不存在，需要向服务器请求名字的ID列表
	std::vector<DWORD> reqlist;
	reqlist.clear();
	Cantfinds.clear();

	std::vector<DWORD>::const_iterator iter = IDs.begin();
	std::vector<DWORD>::const_iterator end = IDs.end();
	
	bool bResult = true;

	for (; iter != end; ++iter)
	{
		DWORD dwReqID = *iter;
		map<DWORD, tstring>::iterator finder = m_id2name.find(dwReqID);
		// 如果没找到，或者是GFC_NULL
		if (finder == m_id2name.end() || finder->second == (_T("GFC_NULL")))
		{
			// 有一个名字没有查到，就算查询失败
			bResult = false;
			// !!!直接加入请求列表，这里不加入请求缓存，因为我想当收到服务器的getsomename消息后，
			// !!!收到的请求的数据是完整的
			reqlist.push_back(dwReqID);
			// 加入不能找到的列表
			Cantfinds.push_back(dwReqID);
		}
	}
	// 如果请求发送列表不为空
	if (!reqlist.empty())
	{
		tagNC_RoleGetSomeName NetCmdData;
		NetCmdData.nNum = static_cast<INT>(reqlist.size());
		// 结构的空间减去代表指针的dwAllID[1]的空间再加上实际上SIZE个大小的DWORD空间等于新结构的大小
		int size = sizeof(tagNC_RoleGetSomeName) - sizeof(DWORD) + sizeof(DWORD) * reqlist.size();
		// 开辟改大小的数组
		BYTE *pAllocData = new BYTE[size];
		tagNC_RoleGetSomeName *pRGSCmd = reinterpret_cast<tagNC_RoleGetSomeName*>(pAllocData);
		ZeroMemory(pRGSCmd, size);
		// 拷贝除了指针之前的所有数据,因为是POD型,是安全的.
		memcpy(pRGSCmd, &NetCmdData, (sizeof(NetCmdData) - sizeof(DWORD)));
		// ID赋值啦
		size_t sizeReq = reqlist.size();
		for (size_t i = 0; i < sizeReq; ++i)
		{
			pRGSCmd->dwAllID[i] = reqlist[i];
		}
		// 设置新的大小
		pRGSCmd->dwSize = size;
		m_pSession->Send(pRGSCmd);
		delete [] pAllocData;
	}
	
	return bResult;
}



const TCHAR* PlayerNameTab::FindNameByNameID( DWORD dwNameID )
{
	map<DWORD, tstring>::const_iterator cit = m_NameID2Name.find(dwNameID);
	if (cit != m_NameID2Name.end())
		return cit->second.c_str();
	else
	{
		// 如果请求列表中有则返回
		if (m_setNameID2NameReq.find(dwNameID) != m_setNameID2NameReq.end())
			return _T("");

		// 如果请求列表中没有则发送请求
		if (m_pSession->IsConnect())
		{
			tagNC_GetNamebyNameID msg;
			msg.dwNameID = dwNameID;
			m_pSession->Send(&msg);

			//保存到请求列表中
			m_setNameID2NameReq.insert(dwNameID);
		}
	}
	return _T("");
}


LPCTSTR PlayerNameTab::FindHeadPicURLbyID( DWORD dwRoleID )
{
    map<DWORD, tstring>::const_iterator cit = m_RoleID2HeadPicUrl.find(dwRoleID);
    if (cit != m_RoleID2HeadPicUrl.end())
        return cit->second.c_str();
    else
    {
        // 如果请求列表里有则返回
        if (m_setRoleID2HeadPic.find(dwRoleID) != m_setRoleID2HeadPic.end())
            return _T("");

        // 如果请求列表中没有则发送请求
        if (m_pSession->IsConnect())
        {
            tagNC_GetRoleHeadPicURL msg;
            msg.dwRoleID = dwRoleID;
            m_pSession->Send(&msg);

            // 保存到请求列表中
            m_setRoleID2HeadPic.insert(dwRoleID);
        }
    }

    return _T("");
}

void PlayerNameTab::OnNS_RoleGetSomeName(tagNS_RoleGetSomeName *pCmd, DWORD)
{
	tagRoleGetSomeNameEvent event(_T("GetSomeNameOK"), NULL);
	// 加入到记录.
	for (int i = 0; i < pCmd->nNum; ++i)
	{
		if( pCmd->IdName[i].szName[0] )
			AddRecord(pCmd->IdName[i].dwID, pCmd->IdName[i].szName);

		if( pCmd->nUserData == 1)
		{
			if( pCmd->IdName[i].szName[0] )
				event.IDs.push_back(pCmd->IdName[i].dwID);
		}
		else
		{
			tagRoleGetNameEvent getName(_T("tagRoleGetNameEvent"), NULL);
			getName.dwRoleID = pCmd->IdName[i].dwID;
			getName.strRoleName = pCmd->IdName[i].szName;
			getName.dwParam = 0;
			getName.bResult = !getName.strRoleName.empty();
			m_pFrameMgr->SendEvent(&getName);

			if(getName.bResult)
			{
				Role* pRole = RoleMgr::Inst()->FindRole(pCmd->IdName[i].dwID);
				if(P_VALID(pRole))
				{
					pRole->SetRoleName(pCmd->IdName[i].szName);
				}
			}
		}
	}
	
	// 发送消息通知消息注册者，获取名字成功.
	if( pCmd->nUserData == 1 )
		m_pFrameMgr->SendEvent(&event);
}

void PlayerNameTab::OnNS_GetNamebyNameID( tagNS_GetNamebyNameID* pNetCmd, DWORD dwParam )
{
	if (pNetCmd->bResult)
	{
		// 插入记录
		tstring strTmp;
		if(pNetCmd->szName == NULL || _tcslen(pNetCmd->szName) <= 0)
			strTmp = _T("");
		else
			strTmp = pNetCmd->szName;

		m_NameID2Name.insert(make_pair(pNetCmd->dwNameID, strTmp));

		// 发送游戏事件
		tagGetNameByNameID e(_T("GetNameByNameID"), NULL);
		e.bResult	=	pNetCmd->bResult;
		e.dwNameID	=	pNetCmd->dwNameID;
		e.strName	=	strTmp;

		m_pFrameMgr->SendEvent(&e);
	}

	// 从请求列表中删除
	m_setNameID2NameReq.erase(pNetCmd->dwNameID);
}

void PlayerNameTab::OnNS_GetRoleHeadPicURL( tagNS_GetRoleHeadPicURL* pNetCmd, DWORD dwParam )
{
    // 插入记录
    TCHAR szImageURL[200] = {_T('\0')};
    memcpy(szImageURL, pNetCmd->byData, (INT)pNetCmd->byHeadUrlSize);
    tstring strImageURL = szImageURL;

    if (!strImageURL.empty())
    {
        m_RoleID2HeadPicUrl.insert(make_pair(pNetCmd->dwRoleID, strImageURL));

    }

    // 从请求列表中删除
    m_setRoleID2HeadPic.erase(pNetCmd->dwRoleID);

    // 发送游戏事件
    tagGetHeadpicByID e(_T("tagGetHeadpicByID"), NULL);
    e.dwRoleID      =   pNetCmd->dwRoleID;
    e.strImageURL   =   strImageURL;

    m_pFrameMgr->SendEvent(&e);
}

void PlayerNameTab::OnNetRoleGetID( tagNS_RoleGetID* pNetCmd, DWORD dwParam )
{
	if( GT_VALID(pNetCmd->dwRoleID) )
	{
		map<DWORD,tstring>::const_iterator it= m_mapName2IDReq.find(pNetCmd->dwRoleNameCrc);
		if(it!=m_mapName2IDReq.end())
		{
			AddRecord(pNetCmd->dwRoleID, pNetCmd->szName);
            m_mapName2IDReq.erase(pNetCmd->dwRoleNameCrc);
			tagRoleGetIDEvent getID(_T("tagRoleGetIDEvent"), NULL);
			getID.dwRoleNameCrc = pNetCmd->dwRoleNameCrc;
			getID.dwRoleID = pNetCmd->dwRoleID;
			getID.dwParam = dwParam;
			m_pFrameMgr->SendEvent(&getID);
		}
	}
	else
	{
		tagGameEvent event(_T("Event_RoleNotExist"), NULL);
		m_pFrameMgr->SendEvent(&event);
	}
	
}
