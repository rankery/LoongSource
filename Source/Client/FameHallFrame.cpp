#include "StdAfx.h"
#include "FameHallFrame.h"

FameHallFrame::FameHallFrame(void) : m_Trunk(this)
{
}

FameHallFrame::~FameHallFrame(void)
{
	/* 清除排名容器数据 */
	SafeDeleteRankData();

	/* 清除名人容器数据 */
	SafeDeleteFameData();
}

BOOL FameHallFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);

	/* 注册开关窗体事件回调函数 */
	m_pFrameMgr->RegisterEventHandle(_T("SetFamousOpen"), (FRAMEEVENTPROC)m_Trunk.sfp1(&FameHallFrame::OnOpenFamousEvent));
	m_pFrameMgr->RegisterEventHandle(_T("SetFamousClose"), (FRAMEEVENTPROC)m_Trunk.sfp1(&FameHallFrame::OnCloseFamousEvent));
	m_pFrameMgr->RegisterEventHandle(_T("ClanTreaOpen"), (FRAMEEVENTPROC)m_Trunk.sfp1(&FameHallFrame::OnClanTreaOpen));
	m_pFrameMgr->RegisterEventHandle(_T("ClanTreaClose"), (FRAMEEVENTPROC)m_Trunk.sfp1(&FameHallFrame::OnClanTreaClose));

	m_pFrameMgr->RegisterEventHandle(_T("tagRoleGetNameEvent"), (FRAMEEVENTPROC)m_Trunk.sfp1(&FameHallFrame::OnGetRankName));
	m_pFrameMgr->RegisterEventHandle(_T("GetNameByNameID"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&FameHallFrame::OnGetNameByNameID));

	/* 注册网络事件回调函数 */
	m_pCmdMgr->Register("NS_GetReputeTop",		(NETMSGPROC)m_Trunk.sfp2(&FameHallFrame::OnNS_GetReputeTop), _T("NS_GetReputeTop"));
	m_pCmdMgr->Register("NS_GetFameHallRoles",  (NETMSGPROC)m_Trunk.sfp2(&FameHallFrame::OnNS_GetFameHallRoles), _T("NS_GetFameHallRoles"));

	return bRet;
}

BOOL FameHallFrame::Destroy()
{
	BOOL bRet = GameFrame::Destroy();

	/* 清除排名容器数据 */
	SafeDeleteRankData();

	/* 清除名人容器数据 */
	SafeDeleteFameData();

	/* 注销开关窗体事件回调函数 */
	m_pFrameMgr->UnRegisterEventHandler(_T("SetFamousOpen"), (FRAMEEVENTPROC)m_Trunk.sfp1(&FameHallFrame::OnOpenFamousEvent));
	m_pFrameMgr->UnRegisterEventHandler(_T("SetFamousClose"), (FRAMEEVENTPROC)m_Trunk.sfp1(&FameHallFrame::OnCloseFamousEvent));
	m_pFrameMgr->UnRegisterEventHandler(_T("ClanTreaOpen"), (FRAMEEVENTPROC)m_Trunk.sfp1(&FameHallFrame::OnClanTreaOpen));
	m_pFrameMgr->UnRegisterEventHandler(_T("ClanTreaClose"), (FRAMEEVENTPROC)m_Trunk.sfp1(&FameHallFrame::OnClanTreaClose));

	m_pFrameMgr->UnRegisterEventHandler(_T("tagRoleGetNameEvent"), (FRAMEEVENTPROC)m_Trunk.sfp1(&FameHallFrame::OnGetRankName));
	m_pFrameMgr->UnRegisterEventHandler(_T("GetNameByNameID"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&FameHallFrame::OnGetNameByNameID));

	/* 注销网络事件回调函数 */

	m_pCmdMgr->UnRegister("NS_GetReputeTop", (NETMSGPROC)m_Trunk.sfp2(&FameHallFrame::OnNS_GetReputeTop));
	m_pCmdMgr->UnRegister("NS_GetFameHallRoles",  (NETMSGPROC)m_Trunk.sfp2(&FameHallFrame::OnNS_GetFameHallRoles));

    /* 清除名人堂窗体 */
    if (P_VALID(m_pWnd))
    {
        m_pGUI->UnRegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&FameHallFrame::EventHandler));
        m_pGUI->AddToDestroyList(m_pWnd);
        m_pWnd = NULL;
    }

	return bRet;
}

BOOL FameHallFrame::ReloadUI()
{
	char	cbuff[128];
	TCHAR	tbuff[128];

	/* 加载GUI界面 */
	XmlElement ele;
	tstring strPath = g_strLocalPath + _T("\\ui\\famous.xml");
	m_pGUI->LoadXml(&ele, "VFS_System", strPath.c_str());
	
	/* 更改控件 */
	for (INT i = 1; i <= MAX_RANK_PLACE; ++i)
	{
		_snprintf(cbuff, sizeof(cbuff)/sizeof(char), "famousbackpic\\rankingbackpic\\rank%d\\propic\\pro", i);
		m_pGUI->ChangeXml(&ele, cbuff, "ClassName", "ProgressEx");
	}

	/* 创建 */
	m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &ele);

	/* 氏族分页 */
	m_pBtClans[ECLT_XuanYuan]	=	(GUIPushButton*)m_pWnd->GetChild(_T("famousbackpic\\famousfra\\xuanyuan"));
	m_pBtClans[ECLT_ShenNong]	=	(GUIPushButton*)m_pWnd->GetChild(_T("famousbackpic\\famousfra\\shennong"));
	m_pBtClans[ECLT_FuXi]		=	(GUIPushButton*)m_pWnd->GetChild(_T("famousbackpic\\famousfra\\fuxi"));
	m_pBtClans[ECLT_SanMiao]	=	(GUIPushButton*)m_pWnd->GetChild(_T("famousbackpic\\famousfra\\sanmiao"));
	m_pBtClans[ECLT_JiuLi]		=	(GUIPushButton*)m_pWnd->GetChild(_T("famousbackpic\\famousfra\\jiuli"));
	m_pBtClans[ECLT_YueZhi]		=	(GUIPushButton*)m_pWnd->GetChild(_T("famousbackpic\\famousfra\\yuezhi"));
	m_pBtClans[ECLT_NvWa]		=	(GUIPushButton*)m_pWnd->GetChild(_T("famousbackpic\\famousfra\\nvwa"));
	m_pBtClans[ECLT_GongGong]	=	(GUIPushButton*)m_pWnd->GetChild(_T("famousbackpic\\famousfra\\gonggong"));

	/* 声望排名 */
	for(INT i = 1; i <= MAX_RANK_PLACE; ++i)
	{
		INT index = i - 1;
		/* 声望排名玩家名字 */
		_sntprintf(tbuff, sizeof(tbuff)/sizeof(TCHAR), _T("famousbackpic\\rankingbackpic\\rank%d"), i);
		m_pStcRoleName[index]	=	(GUIStatic*)m_pWnd->GetChild(tbuff);
		/* 声望级别 */
		_sntprintf(tbuff, sizeof(tbuff)/sizeof(TCHAR), _T("famousbackpic\\rankingbackpic\\rank%d\\propic\\word"), i);
		m_pStcReputeLevel[index] =	(GUIStatic*)m_pWnd->GetChild(tbuff);
		/* 声望值 */
		_sntprintf(tbuff, sizeof(tbuff)/sizeof(TCHAR), _T("famousbackpic\\rankingbackpic\\rank%d\\propic\\pro"), i);
		m_pPrgrss[index]		=	(ProgressEx*)m_pWnd->GetChild(tbuff);

        m_pPrgrss[index]->SetSpeed(100000);
	}
	/* 页数 */
	m_pStcRankPage				=	(GUIStatic*)m_pWnd->GetChild(_T("famousbackpic\\rankingbackpic\\pagepic\\new"));
	/* 上一页 */
	m_pBtRankPrevPage			=	(GUIButton*)m_pWnd->GetChild(_T("famousbackpic\\rankingbackpic\\pagepic\\backbutt"));
	/* 下一页 */
	m_pBtRankNextPage			=	(GUIButton*)m_pWnd->GetChild(_T("famousbackpic\\rankingbackpic\\pagepic\\nextbutt"));

	/* 名人首页列表 */
	m_pStcFameTopPage			=	(GUIStatic*)m_pWnd->GetChild(_T("famousbackpic\\famousshowpic\\top"));
	for (INT i = 1; i <= MAX_FAMOUS_TOP_PLACE; ++i)
	{
		INT index = i - 1;
		/* 首页名人名字 */
		_sntprintf(tbuff, sizeof(tbuff)/sizeof(TCHAR), _T("famousbackpic\\famousshowpic\\top\\name%d"), i);
		m_pStcFamousTop[index]	=	(GUIStatic*)m_pWnd->GetChild(tbuff);
	}
	/* 名人其它页列表 */
	m_pStcFameCommonPage		=	(GUIStatic*)m_pWnd->GetChild(_T("famousbackpic\\famousshowpic\\common"));
	for (INT i = 0; i < MAX_FAMOUS_COMMON_PLACE; ++i)
	{
		/* 其它页名人名字 */
		_sntprintf(tbuff, sizeof(tbuff)/sizeof(TCHAR), _T("famousbackpic\\famousshowpic\\common\\name_%c"), _T('a') + i);
		m_pStcFamousCommon[i]	=	(GUIStatic*)m_pWnd->GetChild(tbuff);
	}
	/* 页数 */
	m_pStcFamousPage			=	(GUIStatic*)m_pWnd->GetChild(_T("famousbackpic\\famousshowpic\\new"));
	/* 上一页 */
	m_pBtFamousPrevPage			=	(GUIButton*)m_pWnd->GetChild(_T("famousbackpic\\famousshowpic\\backbutt"));
	/* 下一页 */
	m_pBtFamousNextPage			=	(GUIButton*)m_pWnd->GetChild(_T("famousbackpic\\famousshowpic\\nextbutt"));

	/* 氏族珍宝 */
	m_pBtEnterClanTreasure		=	(GUIButton*)m_pWnd->GetChild(_T("famousbackpic\\rankingbackpic\\weaponshowbutt"));

	/* 关闭 */
	m_pBtClose					=	(GUIButton*)m_pWnd->GetChild(_T("famousbackpic\\closebutt"));

	/* 注册事件处理函数 */
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&FameHallFrame::EventHandler));

	/* 默认选轩辕分页 */
	m_pBtClans[ECLT_XuanYuan]->SetState(EGUIBS_PushDown);
	SetClan(ECLT_XuanYuan);
	for (INT i = 0; i < ECLT_NUM; ++i)
	{
		m_nRankPageCount[i]		=	1;
		m_nRankCurrPage[i]		=	1;
		m_nFamePageCount[i]		=	1;
		m_nFameCurrPage[i]		=	1;

		m_dwRankUpdateTime[i]	=	0;
		m_dwFameUpdateTime[i]	=	0;
	}

	/* 默认隐藏窗体 */
	ShowWnd(false);

	/*! 创建氏族珍宝Frame */
	m_pClanTreasure = (ClanTreasureFrame*)m_pFrameMgr->CreateFrame(m_strName.c_str(), _T("ClanTreasure"), _T("ClanTreasureFrame"), 0);
	if (!P_VALID(m_pClanTreasure))
		IMSG(_T("Create ClanTreasureFrame failed"));

	m_bisClanTreasureOpen		=	false;

	return TRUE;
}

VOID FameHallFrame::Update()
{

}

VOID FameHallFrame::UpdateFameHall()
{
    RefreshRankToUI(m_eSelectedClan);
    BeginGetReputRank(m_eSelectedClan);
    RefreshFameToUI(m_eSelectedClan);
    BeginGetFame(m_eSelectedClan);
}


DWORD FameHallFrame::EventHandler( tagGUIEvent* pEvent )
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);

	switch (pEvent->eEvent)
	{
	case EGUIE_Click:
		if (pWnd == m_pBtClose)
		{
			/* 关闭窗体 */
			ShowWnd(false);


		}
		else if (pWnd == m_pBtEnterClanTreasure)
		{
			/* 进入氏族珍宝界面 */
			if (m_bisClanTreasureOpen)
				m_pFrameMgr->SendEvent(&tagGameEvent(_T("SetClanTreasureClose"), this));
			else
				m_pFrameMgr->SendEvent(&tagOpenClanTreasure(_T("SetClanTreasureOpen"), m_eSelectedClan, this));
		}
        else if (pWnd == m_pBtFamousNextPage)
        {
            /* 下一页 */
            NextFamePage();
        }
        else if (pWnd == m_pBtFamousPrevPage)
        {
            /* 上一页 */
            PrevFamePage();
        }
		else if (pWnd == m_pBtRankNextPage)
		{
            /* 下一页 */
            NextRankPage();
		}
		else if (pWnd == m_pBtRankPrevPage)
		{
            /* 上一页 */
            PrevRankPage();
		}
		break;
	case EGUIE_PushButtonDown:
		if (pEvent->dwParam1 != 1)	/* 不是按下不处理 */
			return 0;

		for (int i = 0; i < ECLT_NUM; ++i)
		{
			if (pWnd == m_pBtClans[i])
			{
				/* 处理氏族分页按下 */
				SetClan((ECLanType)i);
                /* 关闭氏族珍宝界面 */
                if (m_bisClanTreasureOpen)
                    m_pFrameMgr->SendEvent(&tagGameEvent(_T("SetClanTreasureClose"), this));
				break;
			}
		}
		break;
	}

	return 0;
}

DWORD FameHallFrame::OnOpenFamousEvent( tagGameEvent* pGameEvent )
{
	ShowWnd(true);

	return 0;
}

VOID FameHallFrame::ShowWnd( bool bVal )
{
	/* 设置窗体可见性 */
	m_pWnd->SetInvisible(!bVal);

	if (bVal)
	{
		/* 置顶 */
		m_pWnd->FlipToTop();

		/* 刷新窗体 */
        m_pWnd->SetRefresh(TRUE);

		/* 给声望窗体发送打开事件 */
		m_pFrameMgr->SendEvent(&tagGameEvent(_T("FamousUIOpen"), this));

        /* 刷新数据 */
        UpdateFameHall();
	}
	else
	{
		/* 同时关闭氏族珍宝窗体 */
		if (m_bisClanTreasureOpen)
			m_pFrameMgr->SendEvent(&tagGameEvent(_T("SetClanTreasureClose"), this));

		/* 给声望窗体发送关闭事件 */
		m_pFrameMgr->SendEvent(&tagGameEvent(_T("FamousUIClose"), this));
	}
}

DWORD FameHallFrame::OnCloseFamousEvent( tagGameEvent* pGameEvent )
{
	ShowWnd(false);

	return 0;
}

DWORD FameHallFrame::OnClanTreaOpen( tagGameEvent* pGameEvent )
{
	m_bisClanTreasureOpen = true;

	return 0;
}

DWORD FameHallFrame::OnClanTreaClose( tagGameEvent* pGameEvent )
{
	m_bisClanTreasureOpen = false;

	return 0;
}

VOID FameHallFrame::SetClan( ECLanType eVal )
{
	m_eSelectedClan = eVal;
    UpdateFameHall();
}

VOID FameHallFrame::BeginGetReputRank( ECLanType eVal )
{
	/* 发送获得声望排名列表请求 */
	tagNC_GetReputeTop e;
	e.byClanType	=	(BYTE)eVal;
	e.dwUpdateTime	=	m_dwRankUpdateTime[eVal];
	if (m_pSession->IsConnect())
		m_pSession->Send(&e);
}

VOID FameHallFrame::BeginGetFame( ECLanType eVal )
{
	/* 发送获得名人列表请求 */
	tagNC_GetFameHallRoles e;
	e.byClanType	=	(BYTE)eVal;
	e.dwUpdateTime	=	m_dwFameUpdateTime[eVal];
	if (m_pSession->IsConnect())
		m_pSession->Send(&e);
}

DWORD FameHallFrame::OnNS_GetReputeTop( tagNS_GetReputeTop* pMsg, DWORD param )
{
	ECLanType eClan = (ECLanType)pMsg->byClanType;
	if (!JDG_VALID(ECLT, eClan))
		return 0;

	/* 如果无变化，则返回 */
	if (pMsg->byErrCode == E_FrameHall_RepOrderUnchanged)
		return 0;
    else
        /* 有变化，则保存时间戳 */
        m_dwRankUpdateTime[eClan] = pMsg->dwUpdateTime;

	/* 清空容器 */
	ClearRankData(eClan);

	for (INT i = 0, offset = 0; i < (INT)pMsg->byRoleNum; ++i, ++offset)
	{
		/* 分配空间 */
		tagRepRankData rank;

		/* 拷贝变长列表 */
		memcpy(&rank, pMsg->byData + sizeof(tagRepRankData) * offset, sizeof(tagRepRankData));

		/* 基本信息填入容器 */
		tagRankData* pRankData = new tagRankData();
		pRankData->dwRoleID		=	rank.dwRoleID;
		pRankData->nReputeValue	=	(INT)rank.dwReputValue;
		pRankData->strRoleName	=	PlayerNameTab::Inst()->FindNameByID(rank.dwRoleID);
		/* 如果从缓冲里查不到名字,则放入查找表中,待得到名字返回事件处理 */
		if (pRankData->strRoleName.empty())
			m_mapRoleIDtoRank[eClan].insert(make_pair(rank.dwRoleID, pRankData));
		m_vecRankArray[eClan].push_back(pRankData);

		/* 处理容器其它数据生成 */
		FillReputRank(eClan);

		/* 刷新UI */
		if (eClan == m_eSelectedClan)
			RefreshRankToUI(m_eSelectedClan);
	}
	return 0;
}

DWORD FameHallFrame::OnNS_GetFameHallRoles( tagNS_GetFameHallRoles* pMsg, DWORD param )
{
	ECLanType eClan = (ECLanType)pMsg->eClanType;
	if (!JDG_VALID(ECLT, eClan))
		return 0;

	/* 如果无变化，则返回 */
	if (pMsg->byErrCode == E_FrameHall_MemberOrderUnchanged)
		return 0;
	else
		/* 有变化，则保存时间戳 */
		m_dwFameUpdateTime[eClan] = pMsg->dwUpdateTime;

    /* 清空容器 */
    ClearFameData(eClan);

	for (INT i = 0, offset = 0; i < (INT)pMsg->byRoleNum; ++i, ++offset)
	{
		/* 分配空间 */
		DWORD dwFameNameID;

		/* 拷贝变长列表 */
		memcpy(&dwFameNameID, pMsg->byData + sizeof(DWORD) * offset, sizeof(DWORD));

		/* 基本信息填入容器 */
		tagFameData* pFameData = new tagFameData();
		pFameData->dwNameID		=	dwFameNameID;
		pFameData->strName		=	PlayerNameTab::Inst()->FindNameByNameID(dwFameNameID);
		/* 如果从缓冲里查不到名字,则放入查找表中,待得到名字返回事件处理 */
		if (pFameData->strName.empty())
			m_mapNameIDtoFame[eClan].insert(make_pair(dwFameNameID, pFameData));
		m_vecFameNameIDs[eClan].push_back(pFameData);

		/* 处理容器其它数据生成 */
		FillFameData(eClan);

		/* 刷新UI */
		if (eClan == m_eSelectedClan)
			RefreshFameToUI(m_eSelectedClan);
	}
	return 0;
}

VOID FameHallFrame::FillReputRank( ECLanType eVal )
{
	/* 计算页码数 */
	INT rankCount = m_vecRankArray[eVal].size();
	if (0 == rankCount)
	{
		m_nRankPageCount[eVal]	=	1;
		m_nRankCurrPage[eVal]	=	1;
		return;
	}
	/* 处理整页和零页 */
	if (rankCount % MAX_RANK_PLACE == 0)
		m_nRankPageCount[eVal]	=	rankCount / MAX_RANK_PLACE;
	else
		m_nRankPageCount[eVal]	=	rankCount / MAX_RANK_PLACE + 1;
	
	/* 选中第一页 */
	m_nRankCurrPage[eVal]		=	1;
}

VOID FameHallFrame::FillFameData( ECLanType eVal )
{
	/* 计算页码数 */
	INT fameCount = m_vecFameNameIDs[eVal].size();

	/* 处理首页 */
	if (fameCount <= MAX_FAMOUS_TOP_PLACE)
	{
		m_nFamePageCount[eVal]	=	1;
		m_nFameCurrPage[eVal]	=	1;
		return;
	}

	/* 处理整页和零页 */
	INT commonCount = fameCount - MAX_FAMOUS_TOP_PLACE;

	/* 计算页数时别忘了第一页 */
	if (commonCount % MAX_FAMOUS_COMMON_PLACE == 0)
		m_nFamePageCount[eVal]	=	m_vecFameNameIDs[eVal].size() / MAX_FAMOUS_COMMON_PLACE + 1;
	else
		m_nFamePageCount[eVal]	=	m_vecFameNameIDs[eVal].size() / MAX_FAMOUS_COMMON_PLACE + 2;
	
	/* 选中第一页 */
	m_nFameCurrPage[eVal]		=	1;
}

DWORD FameHallFrame::OnGetRankName( tagRoleGetNameEvent* pGameEvent )
{
	if (!pGameEvent->bResult)
		return 0;

	for (INT i = 0; i < ECLT_NUM; ++i)
	{
		/* 查找是否有请求查询的名字 */
		map<DWORD, tagRankData*>::iterator it =
			m_mapRoleIDtoRank[(ECLanType)i].find(pGameEvent->dwRoleID);
		if (it != m_mapRoleIDtoRank[(ECLanType)i].end())
		{
			/* 将名字存入容器 */
			(it->second)->strRoleName = pGameEvent->strRoleName;

			/* 删除查找容器中的对象 */
			m_mapRoleIDtoRank[(ECLanType)i].erase(it);

			/* 刷新界面 */
			if ((ECLanType)i == m_eSelectedClan)
				RefreshRankToUI(m_eSelectedClan);
		}
	}
	return 0;
}

DWORD FameHallFrame::OnGetNameByNameID( tagGetNameByNameID* pGameEvent )
{
	if (!pGameEvent->bResult)
		return 0;

	for (INT i = 0; i < ECLT_NUM; ++i)
	{
		/* 查找是否有请求查询的名字 */
		map<DWORD, tagFameData*>::iterator it =
			m_mapNameIDtoFame[(ECLanType)i].find(pGameEvent->dwNameID);
		if (it != m_mapNameIDtoFame[(ECLanType)i].end())
		{
			/* 将名字存入容器 */
			(it->second)->strName = pGameEvent->strName;

			/* 删除查找容器对象 */
			m_mapNameIDtoFame[(ECLanType)i].erase(it);

			/* 刷新界面 */
			if ((ECLanType)i == m_eSelectedClan)
				RefreshFameToUI(m_eSelectedClan);
		}
	}
	return 0;
}

VOID FameHallFrame::RefreshRankToUI( ECLanType eVal )
{
	/* 非选择氏族则返回 */
	if (m_eSelectedClan != eVal)
		return;
	
	/* 清空界面 */
    for (INT i = 0; i < MAX_RANK_PLACE; ++i)
	{
		m_pStcRoleName[i]->SetText(_T(""));
        m_pStcRoleName[i]->SetRefresh(TRUE);
		m_pPrgrss[i]->SetValue(0);
		m_pStcReputeLevel[i]->SetText(_T(""));
        m_pStcReputeLevel[i]->SetRefresh(TRUE);
	}

	/* 刷新页码 */
	TCHAR tcbuff[16];
	_itot(m_nRankCurrPage[m_eSelectedClan], tcbuff, 10);
	m_pStcRankPage->SetText(tcbuff);

	/* 无列表则返回 */
	INT rankCount = m_vecRankArray[eVal].size();
	if (0 == rankCount)
		return;

	/* 跟据当前页计算列表显示范围 */
    INT nBegin  = (m_nRankCurrPage[eVal] - 1) * MAX_RANK_PLACE;
    INT nEnd    = nBegin + MAX_RANK_PLACE;
    if (nEnd > rankCount)
        nEnd = rankCount;

	vector<tagRankData*>::iterator itBegin	=   m_vecRankArray[eVal].begin() + nBegin;
    vector<tagRankData*>::iterator itEnd    =   m_vecRankArray[eVal].begin() + nEnd;

	/* 显示到控件 */
	INT index = 0;
	for (vector<tagRankData*>::iterator it = itBegin; it != itEnd; ++it)
	{
		/* 计算声望数据 */
		tagReputeLevel rlevel;
		ReputeMgr::Inst()->UlityCalReputeLevel(rlevel, (*it)->nReputeValue);
		/* 设置角色名 */
		m_pStcRoleName[index]->SetText((*it)->strRoleName.c_str());
        m_pStcRoleName[index]->SetRefresh(TRUE);
		/* 设置进度条颜色 */
		ReputeMgr::Inst()->UlitySetPrgrssColor(m_pPrgrss[index], rlevel.eColor);
		/* 设置进度条上限 */
		m_pPrgrss[index]->SetMaxValue((FLOAT)rlevel.nDiffValue);
		/* 设置进度条当前值 */
		m_pPrgrss[index]->SetValue((FLOAT)((*it)->nReputeValue - rlevel.nFrom));
		/* 设置声望级别文字 */
		m_pStcReputeLevel[index]->SetText(rlevel.szLevel);
        m_pStcReputeLevel[index]->SetRefresh(TRUE);
		/* 下一个控件 */
		++index;
	}
}

VOID FameHallFrame::RefreshFameToUI( ECLanType eVal )
{
	/* 非选择氏族则返回 */
	if (m_eSelectedClan != eVal)
		return;

	/* 清空界面 */
	for (INT i = 0; i < MAX_FAMOUS_TOP_PLACE; ++i)
    {
		m_pStcFamousTop[i]->SetText(_T(""));
        m_pStcFamousTop[i]->SetRefresh(TRUE);
    }

	for (INT i = 0; i < MAX_FAMOUS_COMMON_PLACE; ++i)
    {
		m_pStcFamousCommon[i]->SetText(_T(""));
        m_pStcFamousCommon[i]->SetRefresh(TRUE);
    }

	/* 刷新页码 */
	TCHAR tcbuff[16];
	_itot(m_nFameCurrPage[m_eSelectedClan], tcbuff, 10);
	m_pStcFamousPage->SetText(tcbuff);
    m_pStcFamousPage->SetRefresh(TRUE);

	/* 无列表则返回 */
	INT fameCount = m_vecFameNameIDs[eVal].size();
	if (0 == fameCount)
		return;

	/* 跟据当前页计算列表显示范围 */
    INT nBegin;
    INT nEnd;
	vector<tagFameData*>::iterator itBegin;
	vector<tagFameData*>::iterator itEnd;

	if (1 == m_nFameCurrPage[eVal])
	{
        nBegin  =   0;
        nEnd    =   MAX_FAMOUS_TOP_PLACE;
	}
	else
	{
        nBegin  =   MAX_FAMOUS_TOP_PLACE + (m_nFameCurrPage[eVal] - 2) * MAX_FAMOUS_COMMON_PLACE;
        nEnd    =   nBegin + MAX_FAMOUS_COMMON_PLACE;
	}

    if (nEnd > fameCount)
        nEnd    =   fameCount;

    itBegin     =   m_vecFameNameIDs[eVal].begin() + nBegin;
    itEnd       =   m_vecFameNameIDs[eVal].begin() + nEnd;

	/* 显示到控件 */
	if (1 == m_nFameCurrPage[eVal])
	{
		INT index = 0;
		for (vector<tagFameData*>::iterator it = itBegin; it != itEnd; ++it)
        {
			m_pStcFamousTop[index]->SetText((*it)->strName.c_str());
            m_pStcFamousTop[index]->SetRefresh(TRUE);
            ++index;
        }

        m_pStcFameTopPage->SetInvisible(FALSE);
        m_pStcFameCommonPage->SetInvisible(TRUE);
	}
	else
	{
		INT index = 0;
		for (vector<tagFameData*>::iterator it = itBegin; it != itEnd; ++it)
        {
			m_pStcFamousCommon[index]->SetText((*it)->strName.c_str());
            m_pStcFamousCommon[index]->SetRefresh(TRUE);
            ++index;
        }

        m_pStcFameTopPage->SetInvisible(TRUE);
        m_pStcFameCommonPage->SetInvisible(FALSE);
	}
}

VOID FameHallFrame::PrevRankPage()
{
	/* 页码减一 */
	if (m_nRankCurrPage[m_eSelectedClan] > 1)
    {
		--m_nRankCurrPage[m_eSelectedClan];

	    /* 刷新界面 */
	    RefreshRankToUI(m_eSelectedClan);
    }
}

VOID FameHallFrame::PrevFamePage()
{
	/* 页码减一 */
	if (m_nFameCurrPage[m_eSelectedClan] > 1)
    {
		--m_nFameCurrPage[m_eSelectedClan];

	    /* 刷新界面 */
	    RefreshFameToUI(m_eSelectedClan);
    }
}

VOID FameHallFrame::NextRankPage()
{
	/* 页码加一 */
	if (m_nRankCurrPage[m_eSelectedClan] < m_nRankPageCount[m_eSelectedClan])
    {
		++m_nRankCurrPage[m_eSelectedClan];

	    /* 刷新界面 */
	    RefreshRankToUI(m_eSelectedClan);
    }
}

VOID FameHallFrame::NextFamePage()
{
	/* 页码加一 */
	if (m_nFameCurrPage[m_eSelectedClan] < m_nFameCurrPage[m_eSelectedClan])
    {
		++m_nFameCurrPage[m_eSelectedClan];

	    /* 刷新界面 */
	    RefreshFameToUI(m_eSelectedClan);
    }
}

VOID FameHallFrame::ClearRankData( ECLanType eVal )
{
	/* 释放空间 */
	for (vector<tagRankData*>::iterator it = m_vecRankArray[eVal].begin();
		it != m_vecRankArray[eVal].end(); ++it)
	{
		delete (*it);
		(*it) = NULL;
	}

	/* 清空容器 */
	m_vecRankArray[eVal].clear();
	m_mapRoleIDtoRank[eVal].clear();
}

VOID FameHallFrame::ClearFameData( ECLanType eVal )
{
	/* 释放空间 */
	for (vector<tagFameData*>::iterator it = m_vecFameNameIDs[eVal].begin();
		it != m_vecFameNameIDs[eVal].end(); ++it)
	{
		delete (*it);
		(*it) = NULL;
	}

	/* 清空容器 */
	m_vecFameNameIDs[eVal].clear();
	m_mapNameIDtoFame[eVal].clear();
}

VOID FameHallFrame::SafeDeleteRankData()
{
	for (INT i = 0; i < ECLT_NUM; ++i)
		ClearRankData((ECLanType)i);
}

VOID FameHallFrame::SafeDeleteFameData()
{
	for (INT i = 0; i < ECLT_NUM; ++i)
		ClearFameData((ECLanType)i);
}
