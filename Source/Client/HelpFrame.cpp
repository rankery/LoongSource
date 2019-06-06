#include "stdafx.h"
#include "HelpFrame.h"
#include "..\Cool3D\Util\FileUtil.h"
#include "shellapi.h"

#pragma warning(disable: 4129)

HelpFrame::HelpFrame()
	: m_trunk(this)
	, m_pWnd(NULL)
	, m_pMenuTree(NULL)
	, m_pImg(NULL)
	, m_pMallButton(NULL)
	, m_pBackButton(NULL)
	, m_pNextButton(NULL)
	, m_pCloseButton(NULL)
	, m_curItemID(0)
	, m_defaultItemID(0)
	, m_bVisible(false)
{

}

HelpFrame::~HelpFrame()
{
	ReleaseKeyMap();
}

BOOL HelpFrame::Init(GameFrameMgr *pMgr, GameFrame *pFather, DWORD dwParam)
{
	if (!GameFrame::Init(pMgr, pFather, dwParam))
		return FALSE;
	
	m_data.LoadFromFile();
	const std::vector<tagCategory*>& categories = m_data.GetCategories();
	std::vector<tagCategory*>::const_iterator end = categories.end();
	for (std::vector<tagCategory*>::const_iterator iter = categories.begin();
		iter != end; ++iter)
	{
		AddCategory(*iter);
	}
	
	const std::vector<tagSubCategory*>& subCategories = m_data.GetSubCategories();
	bool bFirst = true;
	std::vector<tagSubCategory*>::const_iterator subend = subCategories.end();
	for (std::vector<tagSubCategory*>::const_iterator iter = subCategories.begin(); 
		iter != subend; ++iter)
	{
		if (bFirst)
		{
			m_defaultItemID = AddSubCategory(*iter);
			m_curItemID = m_defaultItemID;
			bFirst = false;
		}
		else
			AddSubCategory(*iter);
	}

	
	Hide();
	m_pBackButton->SetEnable(false);
	m_pNextButton->SetEnable(false);
	m_pMenuTree->SetCurrentSelItem(m_defaultItemID);

	m_pFrameMgr->RegisterEventHandle(_T("Close_NewHelp"), (FRAMEEVENTPROC)m_trunk.sfp1(&HelpFrame::OnEvent_CloseNewHelp));
	m_pFrameMgr->RegisterEventHandle(_T("Open_NewHelp"), (FRAMEEVENTPROC)m_trunk.sfp1(&HelpFrame::OnEvent_OpenNewHelp));
	return TRUE;
}

BOOL HelpFrame::Destroy()
{
	m_pGUI->UnRegisterEventHandler(m_pWnd->GetFullName().c_str(), m_trunk.sfp1(&HelpFrame::EventHandler));

	GameFrame::Destroy();
	if (P_VALID(m_pWnd))
	{
		m_pGUI->AddToDestroyList(m_pWnd);
		m_pWnd = NULL;
	}
	m_pFrameMgr->UnRegisterEventHandler(_T("Close_NewHelp"), (FRAMEEVENTPROC)m_trunk.sfp1(&HelpFrame::OnEvent_CloseNewHelp));
	m_pFrameMgr->UnRegisterEventHandler(_T("Open_NewHelp"), (FRAMEEVENTPROC)m_trunk.sfp1(&HelpFrame::OnEvent_OpenNewHelp));
	return TRUE;
}

BOOL HelpFrame::ReloadUI()
{
	XmlElement element;
	tstring strPath = vEngine::g_strLocalPath + _T("\\ui\\help.xml");
	if( !m_pGUI->LoadXml( &element, "VFS_System", strPath.c_str() ) )
		return FALSE;

	m_pGUI->ChangeXml( &element, "help_win\\help_pic\\pic1\\helpmenupic\\menutree", "ClassName", "AppliedTree");
	m_pGUI->ChangeXml( &element, "help_win\\help_pic\\pic1\\pic2\\img", "ClassName", "IconStatic");
	m_pWnd = m_pGUI->CreateWnd( _T("\\desktop"), &element );

	if( !P_VALID( m_pWnd ) ) 
		return FALSE;

	m_pMenuTree = static_cast<AppliedTree*>(m_pWnd->GetChild(_T("help_pic\\pic1\\helpmenupic\\menutree")));
	m_pImg = static_cast<IconStatic*>(m_pWnd->GetChild(_T("help_pic\\pic1\\pic2\\img")));

	m_pMallButton = static_cast<GUIButton*>(m_pWnd->GetChild(_T("help_pic\\pic1\\mallbutt")));
	m_pMallButton->SetText(g_StrTable[_T("GameInformation")]);
	m_pBackButton = static_cast<GUIButton*>(m_pWnd->GetChild(_T("help_pic\\pic1\\backbutt")));
	m_pNextButton = static_cast<GUIButton*>(m_pWnd->GetChild(_T("help_pic\\pic1\\nextbutt")));
	m_pCloseButton = static_cast<GUIButton*>(m_pWnd->GetChild(_T("help_pic\\closebutt")));
	
	m_pMenuTree->InsterItem(0, _T("Root"), _T("Root"));


	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_trunk.sfp1(&HelpFrame::EventHandler));


	return TRUE;
}

BOOL HelpFrame::EventHandler(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	switch (pEvent->eEvent)
	{
	case EGUIE_ItemClick:
		{
			if (pWnd == m_pMenuTree)
			{
				KeyMap::iterator iter = m_mapKey2Pictures.find(pEvent->dwParam1);
				if (iter != m_mapKey2Pictures.end())
				{
					m_curItemID = iter->first;
					IndexMap::iterator i = m_mapKey2Index.find(pEvent->dwParam1);
					if (i != m_mapKey2Index.end())
					{
						int index = i->second;
						if ( index < (int)(iter->second)->size())
						{
							m_pImg->SetPic((*(iter->second))[index]);
							if (index == 0)
							{
								m_pBackButton->SetEnable(false);
							}
							else
							{
								m_pBackButton->SetEnable(true);
							}
							if (index == (iter->second)->size() - 1)
							{
								m_pNextButton->SetEnable(false);
							}
							else
							{
								m_pNextButton->SetEnable(true);
							}
						}
					}
				}
			}
		} break;
	case EGUIE_Click:
		{
			if (pWnd == m_pBackButton)
			{
				m_pNextButton->SetEnable(true);
				IndexMap::iterator i = m_mapKey2Index.find(m_curItemID);
				
				KeyMap::iterator iter = m_mapKey2Pictures.find(m_curItemID);

				size_t size = iter->second->size();

				if (i->second > 0)
				{
					i->second--;
				}
				if (i->second == 0)
				{
					m_pBackButton->SetEnable(false);
				}
				else
				{
					m_pBackButton->SetEnable(true);
				}
				m_pImg->SetPic((*(iter->second))[i->second]);
			}
			else if (pWnd == m_pNextButton)
			{
				m_pBackButton->SetEnable(true);
				IndexMap::iterator i = m_mapKey2Index.find(m_curItemID);
				
				KeyMap::iterator iter = m_mapKey2Pictures.find(m_curItemID);
				
				size_t size = iter->second->size();
				
				if (i->second < (int)(size - 1))
				{
					i->second++;
				}
				if (i->second == (int)(size - 1))
				{
					m_pNextButton->SetEnable(false);
				}
				else
				{
					m_pNextButton->SetEnable(true);
				}
				m_pImg->SetPic((*(iter->second))[i->second]);
			}
			if (pWnd == m_pCloseButton)
			{
				Hide();
				
			}
			if (pWnd == m_pMallButton)
			{
				OnGameInfoClick();
			}
		}

	}
	return TRUE;
}

void HelpFrame::AddCategory(const tagCategory *pCategory)
{
	m_pMenuTree->InsterItem(_T("\\Root"), pCategory->name.c_str(), pCategory->name.c_str());
}

DWORD HelpFrame::AddSubCategory(const tagSubCategory *pSubCategory)
{
	tstring strFather(_T("\\Root\\"));
	tstring strSub = m_data.FindParentCategoryName(pSubCategory);
	strFather += strSub;
	
	
	DWORD dwID = m_pMenuTree->InsterItem(strFather.c_str(), pSubCategory->name.c_str(), pSubCategory->name.c_str());

	std::vector<tstring> *pFindfiles = new std::vector<tstring>();
	tstring strRootPath = _T("data\\ui\\help\\");
	
	tstring strPath = strRootPath + pSubCategory->path;
	/*_sntprintf(szFind, 512, _T("%s\*.jpg"), strPath.c_str());
	Cool3D::SearchFiles(szFind, *pFindfiles);
	std::sort(pFindfiles->begin(), pFindfiles->end());*/
	int nPicNum = pSubCategory->picnum;
	for (int i = 0; i < nPicNum; ++i)
	{
		TCHAR szFile[512];
		_sntprintf(szFile, 512, _T("%s\%d.jpg"), strPath.c_str(), i);
		pFindfiles->push_back(szFile);
	}

	if (!pFindfiles->empty())
	{
		m_mapKey2Pictures[dwID] = pFindfiles;
		m_mapKey2Index[dwID] = 0;
	}
	return dwID;
}

void HelpFrame::ReleaseKeyMap()
{
	KeyMap::iterator end = m_mapKey2Pictures.end();
	for (KeyMap::iterator iter = m_mapKey2Pictures.begin(); iter != end; ++iter)
	{
		SAFE_DELETE(iter->second);
	}


}

void HelpFrame::OnGameInfoClick()
{
	TCHAR szIniFileName[X_LONG_NAME] = _T("launcher\\launcher.ini");
	TCHAR szTemp[X_LONG_NAME*2];

	_tcscpy( szTemp, Kernel::Inst()->GetWorkPath() );
	tstring strAppPath = szTemp;

	tstring strIniFileFullName = strAppPath + szIniFileName;
	::GetPrivateProfileString(_T("Launcher"), _T("GameInfoURL"), NULL,  szTemp, X_LONG_NAME*2, strIniFileFullName.c_str());
	tstring url = szTemp;
	if( url == _T("") )
	{
		MessageBox(NULL, _T("Can't find game info url!"), _T(""), MB_OK);
		return;
	}

	ShellExecute(NULL, _T("open"), url.c_str(), NULL, NULL, SW_MAXIMIZE);
}

void HelpFrame::OnEvent_OpenNewHelp(tagGameEvent *pEvent)
{
	Show();
	
}

void HelpFrame::OnEvent_CloseNewHelp(tagGameEvent *pEvent)
{
	Hide();
}

BOOL HelpFrame::EscCancel()
{
	if( P_VALID(m_pWnd) && !m_pWnd->IsInvisible() )
	{
		Hide();
		return TRUE;
	}

	return GameFrame::EscCancel();
}