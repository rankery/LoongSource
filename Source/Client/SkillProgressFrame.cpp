#include "stdafx.h"
#include "SkillProgressFrame.h"
#include "IconStatic.h"
#include "SkillProEvent.h"
#include "SkillMgr.h"
#include "ItemProtoData.h"
#include "ItemMgr.h"

#define HOLD_TIME 1.0f

SkillProgressFrame::SkillProgressFrame( void ):m_Trunk(this)
{
	m_pWndMain				= NULL;
	m_pStcHint				= NULL;
	m_pStcIcon				= NULL;
	m_pSkillPro				= NULL;
	m_pProBack				= NULL;
	m_eState				= ESkillProPhase_NULL;
}

SkillProgressFrame::~SkillProgressFrame( void )
{
	
}

BOOL SkillProgressFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	m_pFrameMgr->RegisterEventHandle( _T("tagSkillProgressEvent"), 
		(FRAMEEVENTPROC)m_Trunk.sfp1(&SkillProgressFrame::OnSkillProgressEvent));

	return TRUE;
}

BOOL SkillProgressFrame::Destroy()
{
	GameFrame::Destroy();
	if( P_VALID(m_pWndMain) )
		m_pGUI->DestroyWnd(m_pWndMain);

	m_pFrameMgr->UnRegisterEventHandler( _T("tagSkillProgressEvent"), 
		(FRAMEEVENTPROC)m_Trunk.sfp1(&SkillProgressFrame::OnSkillProgressEvent));

	return TRUE;
}

BOOL SkillProgressFrame::ReloadUI()
{
	XmlElement element;
	tstring strPath = g_strLocalPath + _T("\\ui\\progressbar.xml");
	m_pGUI->LoadXml(&element, "VFS_System", strPath.c_str());
	m_pGUI->ChangeXml(&element, "prgwin\\prgicon", "ClassName", "IconStatic");

	m_pWndMain	= m_pGUI->CreateWnd(_T("\\desktop"), &element);
	m_pWndMain->SetInvisible(TRUE);
	m_pWndMain->SetUnClickable(TRUE);
	m_pStcHint	= (GUIStatic*)m_pWndMain->GetChild(_T("prgname"));
	m_pStcIcon	= (IconStatic*)m_pWndMain->GetChild(_T("prgicon"));
	m_pProBack	= (GUIStatic*)m_pWndMain->GetChild(_T("progpic"));
	m_pSkillPro	= (GUIProgress*)m_pWndMain->GetChild(_T("progpic\\new"));

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWndMain->GetFullName().c_str(), m_Trunk.sfp1(&SkillProgressFrame::EventHandler));
	return TRUE;
}

VOID SkillProgressFrame::Update()
{
	GameFrame::Update();
	float deltaTime =  Cool3D::Kernel::Inst()->GetDeltaTime();
	switch(m_eState)
	{
	case ESkillProPhase_RecvStartEvent:
		{
			float fcur = m_pSkillPro->GetCurrentValue();
			float fMax = m_pSkillPro->GetMaxValue();
			if(fcur < fMax)
			{
				fcur += deltaTime;
				m_pSkillPro->SetValue(fcur, true);	
				m_pSkillPro->SetRefresh();
			}
			else
			{
				m_pSkillPro->SetValue(fMax, true);	
				m_pSkillPro->SetRefresh();
				m_fStartTime = Kernel::Inst()->GetAccumTime();
				m_eState = ESkillProPhase_Grade;
				m_pSkillPro->SetRefresh(TRUE);
			}
		}
		break;
	case  ESkillProPhase_Grade:
		{
			float fCurTime = Kernel::Inst()->GetAccumTime();
			if( fCurTime-m_fStartTime > 1.0f)
			{
				ShowWnd(FALSE);
			}
			else
			{
				BYTE byGradeCol = CalSkillProGressLerp(fCurTime, m_fStartTime, HOLD_TIME);
				DWORD dwColor = (DWORD)((byGradeCol<<24)|0x00FFFFFF);
				m_pStcIcon->SetPicColor(dwColor);
				m_pProBack->SetPicColor(dwColor);
				m_pSkillPro->SetPicColor(dwColor);
				m_pSkillPro->SetForePicColor(dwColor);
				m_pWndMain->SetRefresh(TRUE);
			}
		}
		break;
	}	
}

VOID SkillProgressFrame::OnEvent( tagGameEvent* pEvent )
{

}

BOOL SkillProgressFrame::EventHandler( tagGUIEvent* pEvent )
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;

	switch( pEvent->eEvent )
	{
	case EGUIE_Click:
		break;
	default:
		break;
	}
	return FALSE;
}

DWORD SkillProgressFrame::OnSkillProgressEvent( tagSkillProgressEvent* pEvent )
{
	if(pEvent->bShow)
	{
		if( pEvent->bSkill )
		{
			//--技能图标和名字的处理
			const tagSkillData* pData = SkillMgr::Inst()->GetSkillDataByTypeID(pEvent->dwTypeID);
			if(!P_VALID(pData))
				return 0;
			tstring strSkillName = pData->pProto->szName;
			m_pStcHint->SetText(strSkillName.c_str());
			m_pStcHint->SetInvisible(FALSE);

			tstring strPic = SkillMgr::Inst()->GetSkillIconPath(pEvent->dwTypeID);
			m_pStcIcon->SetPic(strPic);
			m_pStcIcon->SetPicColor(0xFFFFFFFF);
			m_pStcIcon->SetRefresh();

			//--进度条
			m_pSkillPro->SetMaxValue((FLOAT)pEvent->dwTime/1000);
			m_pSkillPro->SetSpeed(100);
			m_pProBack->SetPicColor(0xFFFFFFFF);
			m_pSkillPro->SetPicColor(0xFFFFFFFF);
			m_pSkillPro->SetForePicColor(0xFFFFFFFF);
			m_pSkillPro->SetValue(0, true);

			//当前状态
			ShowWnd(TRUE);
			m_eState = ESkillProPhase_RecvStartEvent;
			m_pWndMain->SetRefresh(TRUE);
		}
		else 
		{
			const tagItemProto* pItemProto = ItemProtoData::Inst()->FindItemProto(pEvent->dwTypeID);
			if( !P_VALID(pItemProto) )
				return 0;
			tstring strSkillName = pItemProto->pDisplayInfo->szName;
			m_pStcHint->SetText(strSkillName.c_str());
			m_pStcHint->SetInvisible(FALSE);

			tstring strPic = ItemMgr::Inst()->GetItemIconPath(pEvent->dwTypeID);
			m_pStcIcon->SetPic(strPic);
			m_pStcIcon->SetPicColor(0xFFFFFFFF);
			m_pStcIcon->SetRefresh();

			//--进度条
			m_pSkillPro->SetMaxValue((FLOAT)pEvent->dwTime/1000);
			m_pSkillPro->SetSpeed(100);
			m_pProBack->SetPicColor(0xFFFFFFFF);
			m_pSkillPro->SetPicColor(0xFFFFFFFF);
			m_pSkillPro->SetForePicColor(0xFFFFFFFF);
			m_pSkillPro->SetValue(0, true);

			//当前状态
			ShowWnd(TRUE);
			m_eState = ESkillProPhase_RecvStartEvent;
			m_pWndMain->SetRefresh(TRUE);

		}
	}
	else
	{
		m_fStartTime = Kernel::Inst()->GetAccumTime();
		m_eState = ESkillProPhase_Grade;
		m_pStcHint->SetInvisible(TRUE);

		float fMax = m_pSkillPro->GetMaxValue();
		
		m_pSkillPro->SetValue(fMax, true);	
		m_pSkillPro->SetRefresh();
		m_pSkillPro->SetRefresh(TRUE);
	}

	return 0;
}

BYTE SkillProgressFrame::CalSkillProGressLerp( float fCurTime, float fStartTime, float fHoldTime )
{
	float fRaiseR = (fCurTime-fStartTime) / fHoldTime;	
	BYTE byGradeCol = (BYTE)(255*(1.0f-fRaiseR));
	return byGradeCol;
}

void SkillProgressFrame::ShowWnd( BOOL bVal )
{
	if(P_VALID(m_pWndMain)) 
		m_pWndMain->SetInvisible(!bVal); 
	m_eState	= ESkillProPhase_NULL; 
}