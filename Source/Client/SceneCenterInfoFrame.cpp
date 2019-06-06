#include "stdafx.h"
#include "SceneCenterInfoFrame.h"
#include "Render2D.h"
#include "MapEvent.h"
#include "MapLogicData.h"
#include "MapMgr.h"
#define DEFAULT_FONT_NUM 20
class SceneCenterInfoSortFunc
{
public:
	bool operator() (const tagCenterInfo& tip1,const tagCenterInfo& tip2)
	{
		return tip1.dwTime < tip2.dwTime;
	}
};

#define CENTER_INFO_TIME 3000


SceneCenterInfoFrame::SceneCenterInfoFrame( void ):m_Trunk(this)
{
	m_pWndArea		= NULL;
	m_pWnd			= NULL;
	memset( m_pStcInfo, 0x0, sizeof(m_pStcInfo) );
	m_pWndEx		= NULL;
	memset( m_pStcInfoEx, 0x0, sizeof(m_pStcInfoEx) );
}

SceneCenterInfoFrame::~SceneCenterInfoFrame( void )
{
	
}

BOOL SceneCenterInfoFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	m_pMgr->RegisterEventHandle( _T("tagUpdateLPLevel"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&SceneCenterInfoFrame::OnUpdateLPLevel));
	m_pMgr->RegisterEventHandle( _T("tagEnterMapAreaEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&SceneCenterInfoFrame::OnEnterMapAreaEvent));
	return TRUE;
}

BOOL SceneCenterInfoFrame::Destroy()
{
	m_pMgr->UnRegisterEventHandler( _T("tagUpdateLPLevel"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&SceneCenterInfoFrame::OnUpdateLPLevel));
	m_pMgr->UnRegisterEventHandler( _T("tagEnterMapAreaEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&SceneCenterInfoFrame::OnEnterMapAreaEvent));
	GameFrame::Destroy();
	if( P_VALID(m_pWnd) )
		m_pGUI->DestroyWnd(m_pWnd);
	if( P_VALID(m_pWndEx) )
		m_pGUI->DestroyWnd(m_pWndEx);
	if( P_VALID(m_pWndArea) )
		m_pGUI->DestroyWnd(m_pWndArea);
	m_vecCenterInfo.clear();
	m_vecCenterInfoEx.clear();

	return TRUE;
}

BOOL SceneCenterInfoFrame::ReloadUI()
{
	tstring strPath = g_strLocalPath + _T("\\ui\\centerinfo.xml");
	XmlElement element;
	m_pGUI->LoadXml(&element, "VFS_System", strPath.c_str());

	//m_pGUI->ChangeXml(&element, "centerback", "ClassName", "StaticNoInput");
	for(int i=0; i<MAX_SCENE_CENTER_INFO; i++)
	{
		char szBuff[64];
		sprintf(szBuff, "centerinfo%d", i+1);
		m_pGUI->ChangeXml(&element, szBuff, "ClassName", "CGUIStaticEx");
	}
	m_pWnd = (GUIStaticEx*)m_pGUI->CreateWnd(_T("\\desktop"), &element);
	m_pWnd->SetUnClickable(TRUE);

	for(int i=0; i<MAX_SCENE_CENTER_INFO; i++)
	{
		tagCenterInfo info;
		TCHAR szBuff[64];
		_stprintf(szBuff, _T("centerinfo%d"), i+1);
		m_pStcInfo[i] = (GUIStaticEx*)m_pWnd->GetChild( szBuff );
		if( !P_VALID(m_pStcInfo[i]) )
			return FALSE;
		m_pStcInfo[i]->SetInvisible(TRUE);
		m_pStcInfo[i]->SetUnClickable(TRUE);
		m_vecCenterInfo.push_back(info);
	}

	//任务屏幕中央屏显
	tstring strPath1 = g_strLocalPath + _T("\\ui\\info2.xml");
	m_pGUI->LoadXml(&element, "VFS_System", strPath1.c_str());
	//m_pGUI->ChangeXml(&element, "info2_pic", "ClassName", "StaticNoInput");
	for(int i=0; i<MAX_SCENE_CENTER_INFO; i++)
	{
		char szBuff[64];
		sprintf(szBuff, "info%d", i+1);
		m_pGUI->ChangeXml(&element, szBuff, "ClassName", "CGUIStaticEx");
	}
	m_pWndEx = (GUIStaticEx*)m_pGUI->CreateWnd(_T("\\desktop"), &element);
	m_pWndEx->SetUnClickable(TRUE);
	
	for(int i=0; i<MAX_SCENE_CENTER_INFO; i++)
	{
		tagCenterInfo info;
		TCHAR szBuff[64];
		_stprintf(szBuff, _T("info%d"), i+1);
		m_pStcInfoEx[i] = (GUIStaticEx*)m_pWndEx->GetChild( szBuff );
		if( !P_VALID(m_pStcInfo[i]) )
			return FALSE;
		m_pStcInfoEx[i]->SetInvisible(TRUE);
		m_pStcInfoEx[i]->SetUnClickable(TRUE);
		m_vecCenterInfoEx.push_back(info);
	}

	//加载区域地图提示控件
	CreateAreaWnd();	

	return TRUE;
}

VOID SceneCenterInfoFrame::Update()
{
	DWORD dwAccumTime = Kernel::Inst()->GetAccumTimeDW();

	//错误提示
	int nSize = m_vecCenterInfo.size();
	for(int i=0; i<(int)nSize; i++)
	{
		if( m_pStcInfo[i]->IsInvisible()==FALSE 
			&& (dwAccumTime-m_vecCenterInfo[i].dwTime>CENTER_INFO_TIME) )
		{
			m_vecCenterInfo[i].strText = _T("");
			m_vecCenterInfo[i].dwTime = GT_INVALID;
			SortSceneCenterInfo();	
		}
	}
	bool bHave = false;
	for(int i=0; i<(int)nSize; i++)
	{
		if(m_vecCenterInfo[i].strText!=_T(""))
			bHave = true;
	}
	if(!bHave && m_pWnd->IsInvisible()==FALSE)
	{	
		m_pWnd->SetInvisible(TRUE);
	}

	//任务提示
	int nSizeEx = m_vecCenterInfoEx.size();
	for(int i=0; i<(int)nSizeEx; i++)
	{
		if( m_pStcInfoEx[i]->IsInvisible()==FALSE 
			&& (dwAccumTime-m_vecCenterInfoEx[i].dwTime>CENTER_INFO_TIME) )
		{
			m_vecCenterInfoEx[i].strText = _T("");
			m_vecCenterInfoEx[i].dwTime = GT_INVALID;
			SortSceneCenterInfoEx();	
		}
	}
	bool bHaveEx = false;
	for(int i=0; i<(int)nSizeEx; i++)
	{
		if(m_vecCenterInfoEx[i].strText!=_T(""))
			bHaveEx = true;
	}
	if(!bHaveEx && m_pWndEx->IsInvisible()==FALSE)
	{	
		m_pWndEx->SetInvisible(TRUE);
	}
	
	if( m_pWndArea->IsInvisible()==FALSE && dwAccumTime-m_dwAreaTime>CENTER_INFO_TIME)
	{
		m_pWndArea->SetInvisible(TRUE);
	}

}

VOID SceneCenterInfoFrame::OnEvent( tagGameEvent* pEvent )
{
	
}

void SceneCenterInfoFrame::AddCenterinfo( const TCHAR* szInfo )
{
	int i = 0;
	for(; i<(int)m_vecCenterInfo.size(); i++)
	{
		if(m_vecCenterInfo[i].strText==_T("") 
			&& m_vecCenterInfo[i].dwTime==GT_INVALID)
		{
			m_vecCenterInfo[i].strText = szInfo;
			m_vecCenterInfo[i].dwTime = Kernel::Inst()->GetAccumTimeDW();
			break;
		}
	}

	if( i >= MAX_SCENE_CENTER_INFO )
	{
		m_vecCenterInfo[0].strText = szInfo;
		m_vecCenterInfo[0].dwTime = Kernel::Inst()->GetAccumTimeDW();
	}

	//重新排序
	SortSceneCenterInfo();
}

void SceneCenterInfoFrame::AddCenterinfoEx(const TCHAR* szInfo)
{
	int i = 0;
	for(; i<(int)m_vecCenterInfoEx.size(); i++)
	{
		if(m_vecCenterInfoEx[i].strText==_T("") 
			&& m_vecCenterInfoEx[i].dwTime==GT_INVALID)
		{
			m_vecCenterInfoEx[i].strText = szInfo;
			m_vecCenterInfoEx[i].dwTime = Kernel::Inst()->GetAccumTimeDW();
			break;
		}
	}

	if( i >= MAX_SCENE_CENTER_INFO )
	{
		m_vecCenterInfoEx[0].strText = szInfo;
		m_vecCenterInfoEx[0].dwTime = Kernel::Inst()->GetAccumTimeDW();
	}

	//重新排序
	SortSceneCenterInfoEx();
}

void SceneCenterInfoFrame::SortSceneCenterInfo()
{
	SceneCenterInfoSortFunc sortFunc;
	std::sort(m_vecCenterInfo.begin(), m_vecCenterInfo.end(), sortFunc);

	for( int i=0; i<MAX_SCENE_CENTER_INFO; i++ )
	{
		m_pStcInfo[i]->SetInvisible(TRUE);
	}

	for(int i=0; i<(int)m_vecCenterInfo.size(); i++)
	{
		if( m_vecCenterInfo[i].strText!=_T("") )
		{
			m_pWnd->SetInvisible(FALSE);
			m_pStcInfo[i]->SetText(m_vecCenterInfo[i].strText.c_str());
			tagGUIFont* pFont = m_pStcInfo[i]->GetFont();
			tstring strText = m_pStcInfo[i]->GetText();
			m_pRender->GetTextSize(strText.c_str(), pFont, m_vecCenterInfo[i].txtSize);
			m_pStcInfo[i]->Resize(m_vecCenterInfo[i].txtSize);
			m_pStcInfo[i]->Align();
			m_pStcInfo[i]->SetInvisible(FALSE);
			m_pStcInfo[i]->FlipToTop();
		}
	}
}

void SceneCenterInfoFrame::SortSceneCenterInfoEx()
{
	SceneCenterInfoSortFunc sortFunc;
	std::sort(m_vecCenterInfoEx.begin(), m_vecCenterInfoEx.end(), sortFunc);

	for( int i=0; i<MAX_SCENE_CENTER_INFO; i++ )
	{
		m_pStcInfoEx[i]->SetInvisible(TRUE);
	}

	for(int i=0; i<(int)m_vecCenterInfoEx.size(); i++)
	{
		if( m_vecCenterInfoEx[i].strText!=_T("") )
		{
			m_pWndEx->SetInvisible(FALSE);
			m_pStcInfoEx[i]->SetText(m_vecCenterInfoEx[i].strText.c_str());
			tagGUIFont* pFont = m_pStcInfoEx[i]->GetFont();
			tstring strText = m_pStcInfoEx[i]->GetText();
			m_pRender->GetTextSize(strText.c_str(), pFont, m_vecCenterInfoEx[i].txtSize);
			m_pStcInfoEx[i]->Resize(m_vecCenterInfoEx[i].txtSize);
			m_pStcInfoEx[i]->Align();
			m_pStcInfoEx[i]->SetInvisible(FALSE);
			m_pStcInfoEx[i]->FlipToTop();
		}
	}
}

DWORD SceneCenterInfoFrame::OnUpdateLPLevel(tagUpdateLPLevel* pEvent)
{
	if( pEvent->bLevelUp )
	{
		TCHAR szBuff[X_LONG_NAME];
		_stprintf(szBuff, g_StrTable[_T("Role_LvlUp")], pEvent->nCurLevel);
		AddCenterinfo(szBuff);
	}
	return 0;
}

DWORD SceneCenterInfoFrame::OnEnterMapAreaEvent( tagEnterMapAreaEvent* pEvent )
{
	const DWORD dwMapID = MapMgr::Inst()->GetCurMapID();
	const tagMapShowAreaData* pAreaData = MapLogicData::Inst()->FindMapShowAreaData( dwMapID, pEvent->mapAreaID );
	if( P_VALID(pAreaData) )
	{
		tstringstream strTitle;
		strTitle << _T("<color=0xFFFF9600>") << pAreaData->szTitle;
		m_pWndArea->SetText( strTitle.str().c_str() );
		m_pWndArea->SetInvisible(FALSE);
		m_pWndArea->FlipToTop();
		m_dwAreaTime = Kernel::Inst()->GetAccumTimeDW();
		strTitle.clear();
	}
	return 0;
}

void SceneCenterInfoFrame::CreateAreaWnd()
{
	GUIWnd* pDesktop = m_pGUI->GetDesktop();
	int nX=0, nY=0;
	XmlElement ele;

	nX = pDesktop->GetSize().x - 200;

	ele.SetAttribute("Name", "AreaWnd");
	ele.SetAttribute("ClassName", "CGUIStaticEx");
	ele.SetAttribute("Size_x", 1024);
	ele.SetAttribute("Size_y", 40);
	ele.SetAttribute("AlignControl", "\\desktop\\centerback");
	ele.SetAttribute("AlignType", "OuterBottom");
	ele.SetAttribute("LogicPos_x", nX);
	ele.SetAttribute("LogicPos_y", 50);
	/*ele.SetAttribute("FontWidth", 10);*/
	ele.SetAttribute("FontHeight", 28);
	ele.SetAttribute("FontWeight", 400);
	ele.SetAttribute("TextRect_left", 0);
	ele.SetAttribute("TextRect_top", 0);
	ele.SetAttribute("TextRect_right", 1024);
	ele.SetAttribute("TextRect_bottom", 40);
	ele.SetAttribute("TextAlign", "Center");
	ele.SetAttribute("PicColor", 0xFFFFFF);

	m_pWndArea = (GUIStaticEx*)m_pGUI->CreateWnd(_T("\\desktop"), &ele);
	if( P_VALID(m_pWndArea) )
	{
		m_pWndArea->SetInvisible(TRUE);
		m_pWndArea->SetUnClickable(TRUE);
	}
}