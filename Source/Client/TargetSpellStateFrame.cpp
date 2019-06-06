#include "stdafx.h"
#include "TargetSpellStateFrame.h"
#include "ActiveStatic.h"
#include "ActiveProgress.h"
#include "CombatEvent.h"
#include "SkillProtoData.h"
#include "ItemProtoData.h"
#include "CombatSys.h"
#include "WorldPickerFrame.h"

TargetSpellStateFrame::TargetSpellStateFrame() :
m_Trunk( this ),
m_pWnd( NULL ),
m_pIcon( NULL ),
m_pName( NULL ),
m_pTimePro( NULL ),
m_dwInterruptTime( GT_INVALID ),
m_dwRoleID( GT_INVALID )
{

}

TargetSpellStateFrame::~TargetSpellStateFrame()
{
}

BOOL TargetSpellStateFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	m_pMgr->RegisterEventHandle( _T("tagRoleSpellStartEvent"),    (FRAMEEVENTPROC)m_Trunk.sfp1(&TargetSpellStateFrame::OnRoleSpellStartEvent));
	m_pMgr->RegisterEventHandle( _T("tagRoleSpellStopEvent"),     (FRAMEEVENTPROC)m_Trunk.sfp1(&TargetSpellStateFrame::OnRoleSpellStopEvent));
	m_pMgr->RegisterEventHandle( _T("tagRolePickEvent"),		  (FRAMEEVENTPROC)m_Trunk.sfp1(&TargetSpellStateFrame::OnRolePickEvent));

	return TRUE;
}

BOOL TargetSpellStateFrame::Destroy()
{
	m_pMgr->UnRegisterEventHandler( _T("tagRoleSpellStartEvent"),  (FRAMEEVENTPROC)m_Trunk.sfp1(&TargetSpellStateFrame::OnRoleSpellStartEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagRoleSpellStopEvent"),   (FRAMEEVENTPROC)m_Trunk.sfp1(&TargetSpellStateFrame::OnRoleSpellStopEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagRolePickEvent"),		   (FRAMEEVENTPROC)m_Trunk.sfp1(&TargetSpellStateFrame::OnRolePickEvent));

	GameFrame::Destroy();

	SafeDestroyUI();

	return TRUE;
}

BOOL TargetSpellStateFrame::ReloadUI()
{
	SafeDestroyUI();

	// 创建
	XmlElement element;
	tstring strPath = g_strLocalPath + _T("\\ui\\t_progressbar.xml");
	if( !m_pGUI->LoadXml( &element, "VFS_System", strPath.c_str() ) )
		return FALSE;

	m_pGUI->ChangeXml( &element, "t_prgwin\\t_prgicon", "ClassName", "ActiveStatic");
	m_pGUI->ChangeXml( &element, "t_prgwin\\t_progpic\\progress", "ClassName", "ActiveProgress");

	m_pWnd = m_pGUI->CreateWnd( _T("\\desktop"), &element );

	m_pIcon		= (ActiveStatic*)m_pWnd->GetChild( _T("t_prgicon") );
	m_pName		= (GUIStatic*)m_pWnd->GetChild( _T("t_prgname") );
	m_pTimePro  = (ActiveProgress*)m_pWnd->GetChild( _T("t_progpic\\progress") );

	// 隐藏
	m_pWnd->SetInvisible( TRUE );

	return TRUE;
}

VOID TargetSpellStateFrame::SafeDestroyUI()
{
	if( P_VALID( m_pWnd ) )
	{
		m_pGUI->DestroyWnd( m_pWnd );
		m_pWnd = NULL;
	}
}

VOID TargetSpellStateFrame::Update()
{
	// 被打断0.5秒后关闭界面
	if( GT_INVALID != m_dwInterruptTime && Kernel::Inst()->GetAccumTimeDW() - m_dwInterruptTime > 500 )
		Close();

	GameFrame::Update();
}

VOID TargetSpellStateFrame::Close()
{
	m_pWnd->SetInvisible( TRUE );
	m_dwInterruptTime = GT_INVALID;
	m_dwRoleID = GT_INVALID;
}

DWORD TargetSpellStateFrame::OnRoleSpellStartEvent(tagRoleSpellStartEvent* pEvent)
{
	CombatSys* pCombatSys = (CombatSys*)m_pMgr->GetFrame(_T("CombatSys"));
	if( !P_VALID( pCombatSys ) )
		return 0;
	if( pCombatSys->GetCurTargetID() != pEvent->dwRoleID )
		return 0;

	tstring strName, strIconFileName = _T("data\\ui\\Icon\\");
	if( pEvent->bSkill )
	{
		const tagSkillProtoClient* pSkillProto=SkillProtoData::Inst()->FindSkillProto( pEvent->dwTypeID );
		if( !P_VALID(pSkillProto) )
			return 0;
		strName = pSkillProto->szName;
		strIconFileName += pSkillProto->szIcon;
		strIconFileName += _T(".tga");
	}
	else
	{
		const tagItemDisplayInfo* pDisplay = ItemProtoData::Inst()->FindItemDisplay( pEvent->dwTypeID );
		if( !P_VALID(pDisplay) )
			return 0;
		strName = pDisplay->szName;
		strIconFileName += pDisplay->szMaleIcon;
		strIconFileName += _T(".tga");
	}
	m_pName->SetText( strName.c_str() );
	m_pIcon->SetPic( strIconFileName );

	m_pTimePro->SetValue( (FLOAT)pEvent->dwCurTime, true );
	m_pTimePro->SetMaxValue( (FLOAT)pEvent->dwTimeCount );
	m_pTimePro->SetSpeed( 1000.0f );
	m_pTimePro->SetValue( (FLOAT)pEvent->dwTimeCount );
	m_pTimePro->SetForePicColor( 0xFFFFFFFF );

	// 打开
	m_pWnd->SetInvisible( FALSE );
	m_dwInterruptTime = GT_INVALID;
	m_dwRoleID = pEvent->dwRoleID;

	return 0;
}

DWORD TargetSpellStateFrame::OnRoleSpellStopEvent(tagRoleSpellStopEvent* pEvent)
{
	if( m_dwRoleID != pEvent->dwRoleID )
		return 0;

	if( pEvent->bInterrupt )
	{
		m_pTimePro->SetForePicColor( 0xFFFF0000 );
		m_pTimePro->SetSpeed( 0.0f );
		m_dwInterruptTime = Kernel::Inst()->GetAccumTimeDW();
	}
	else
	{
		Close();
	}
	return 0;
}

DWORD TargetSpellStateFrame::OnRolePickEvent(tagRolePickEvent* pEvent)
{
	if( GT_INVALID != m_dwRoleID && pEvent->dwRoleID != m_dwRoleID )
		Close();
	return 0;
}
