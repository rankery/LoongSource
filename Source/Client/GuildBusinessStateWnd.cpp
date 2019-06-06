#include "stdafx.h"
#include "GuildBusinessStateWnd.h"

GuildBusinessStateWnd::GuildBusinessStateWnd()
{

}

GuildBusinessStateWnd::~GuildBusinessStateWnd()
{
	if (P_VALID(m_pWnd))
	{
		m_pGUI->AddToDestroyList(m_pWnd);
		m_pWnd = NULL;
	}
}

BOOL GuildBusinessStateWnd::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	if (!GameFrame::Init(pMgr, pFather, dwParam))
		return FALSE;
	return TRUE;
}

BOOL GuildBusinessStateWnd::Destroy()
{
	return GameFrame::Destroy();
}


BOOL GuildBusinessStateWnd::ReloadUI()
{
	tstring strPath = g_strLocalPath + _T("\\ui\\guild_static.xml");
	m_pWnd = (GUIStatic*)m_pGUI->CreateFromFile("VFS_System", strPath.c_str());
	return TRUE;
}


BOOL GuildBusinessStateWnd::EscCancel()
{
	return FALSE;
}

