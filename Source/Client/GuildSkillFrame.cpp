#include "stdafx.h"
#include "GuildSkillFrame.h"
#include "GuildMgr.h"
#include "..\WorldDefine\guild_define.h"
#include "GuildFrame.h"
#include "SkillMgr.h"
#include "IconStatic.h"
#include "GuildDefine.h"

GuildSkillFrame::GuildSkillFrame()
	
{


}

GuildSkillFrame::~GuildSkillFrame()
{

}

BOOL GuildSkillFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	if (!GameFrame::Init(pMgr, pFather, dwParam))
		return FALSE;
	m_pGUI->RegisterEventHandler(m_pPatch->GetFullName(), (VUIEVENTHANDLE)m_Trunk.sfp1(&GuildSkillFrame::EventHandler));
	
	return TRUE;	
}

BOOL GuildSkillFrame::Destroy()
{
	m_pGUI->UnRegisterEventHandler(m_pPatch->GetFullName(), (VUIEVENTHANDLE)m_Trunk.sfp1(&GuildSkillFrame::EventHandler));
	GameFrame::Destroy();
	return TRUE;
}

BOOL GuildSkillFrame::ReloadUI()
{
	
	
	
}

BOOL GuildSkillFrame::EscCancel()
{
	return FALSE;
}

BOOL GuildSkillFrame::EventHandler(tagGUIEvent *pEvent)
{
	
}

void GuildSkillFrame::Refresh()
{
	
}

void GuildSkillFrame::SetAllDefaultItemIcon()
{

}