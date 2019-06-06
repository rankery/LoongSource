#include "StdAfx.h"
#include "HealthyControlFrame.h"
#include "CombatSysUtil.h"
#include "RoleMgr.h"

HealthyControlFrame::HealthyControlFrame(void) : m_Trunk(this)
{
}

HealthyControlFrame::~HealthyControlFrame(void)
{
}

BOOL HealthyControlFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);
	m_pCmdMgr->Register("NS_GetFatigueInfo", (NETMSGPROC)m_Trunk.sfp2(&HealthyControlFrame::OnNS_GetFatigueInfo), _T("NS_GetFatigueInfo"));
    m_pCmdMgr->Register("NS_FatigueLimit", (NETMSGPROC)m_Trunk.sfp2(&HealthyControlFrame::OnNS_FatigueLimit), _T("NS_FatigueLimit"));
	return bRet;
}

BOOL HealthyControlFrame::Destroy()
{
	BOOL bRet = GameFrame::Destroy();
	m_pCmdMgr->UnRegister("NS_GetFatigueInfo", (NETMSGPROC)m_Trunk.sfp2(&HealthyControlFrame::OnNS_GetFatigueInfo));
    m_pCmdMgr->UnRegister("NS_FatigueLimit", (NETMSGPROC)m_Trunk.sfp2(&HealthyControlFrame::OnNS_FatigueLimit));
    if (P_VALID(m_pWnd))
        m_pGUI->DestroyWnd(m_pWnd);
    if (P_VALID(GetObj("HealthyControlMsg")))
        KillObj("HealthyControlMsg");
	return bRet;
}

BOOL HealthyControlFrame::ReloadUI()
{
    // 计算窗体位置
    string x, y;
    stringstream sstream;
    sstream << Device()->GetClientWidth() - 321;
    sstream >> x;
    sstream.clear();
    sstream << 81;
    sstream >> y;
    sstream.clear();
    sstream.str("");

	// 创建
	XmlElement ele;
	tstring strPath = g_strLocalPath + _T("\\ui\\healthytips.xml");
	m_pGUI->LoadXml(&ele, "VFS_System", strPath.c_str());
	m_pGUI->ChangeXml(&ele, "heathytipswin\\healthytips", "ClassName", "StaticEx");
    m_pGUI->ChangeXml(&ele, "heathytipswin\\healthytips", "AlphaWrite", "Write");
    m_pGUI->ChangeXml(&ele, "heathytipswin", "AlignControl", "\\desktop");
    m_pGUI->ChangeXml(&ele, "heathytipswin", "AlignType", "NoAlign");
    m_pGUI->ChangeXml(&ele, "heathytipswin", "LogicPos_x", x.c_str());
    m_pGUI->ChangeXml(&ele, "heathytipswin", "LogicPos_y", y.c_str());

	m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &ele);
    
    m_pWnd->SetInvisible(true);
	m_pStcTips = (StaticEx*)m_pWnd->GetChild(_T("healthytips"));
	m_pStcTips->SetFontHeight(50);

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&HealthyControlFrame::EventHandler));

	return TRUE;
}

VOID HealthyControlFrame::Update()
{

}

DWORD HealthyControlFrame::OnGameEvent( tagGameEvent* pGameEvent )
{
	return 0;
}

DWORD HealthyControlFrame::EventHandler( tagGUIEvent* pEvent )
{
	//GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);

	//switch (pEvent->eEvent)
	//{
	//case EGUIE_MoveIntoCtrl:
 //       int a = m_pUtil->RandomInRange(0, 2);
 //       SetHealthyState((EHealthyState)a);
	//}
	return 0;
}

VOID HealthyControlFrame::SetHealthyState( EHealthyState evalue )
{
	switch (evalue)
	{
	case EHS_Healthy:
        {
            //m_pStcTips->SetText(_T("健康时间"));
            //m_pStcTips->SetTextColor(0xFF00FF00);
            tstring strFilePath = _T("data\\ui\\Main\\L_main-pro-a.bmp");
            m_pStcTips->SetPicFile(strFilePath);
            m_pWnd->SetInvisible(false);
        }

		break;
	case EHS_Fatigue:
        {
		    //m_pStcTips->SetText(_T("疲劳时间"));
		    //m_pStcTips->SetTextColor(0xFFFF8000);
            tstring strFilePath = _T("data\\ui\\Main\\L_main-pro-b.bmp");
            m_pStcTips->SetPicFile(strFilePath);
            m_pWnd->SetInvisible(false);
        }
		break;
	case EHS_Unhealthy:
        {
            //m_pStcTips->SetText(_T("不健康时间"));
            //m_pStcTips->SetTextColor(0xFFFF0000);
            tstring strFilePath = _T("data\\ui\\Main\\L_main-pro-c.bmp");
            m_pStcTips->SetPicFile(strFilePath);
            m_pWnd->SetInvisible(false);
        }
		break;
    default:
        m_pWnd->SetInvisible(true);
        break;
	}
}

DWORD HealthyControlFrame::OnNS_GetFatigueInfo( tagNS_GetFatigueInfo* pMsg, DWORD dwParam )
{
    ShowWnd(TRUE);

	//角色是否进入游戏场景
	BOOL isCreated = P_VALID(RoleMgr::Inst()->GetLocalPlayer());

	//[0~2小时55分)
	if (pMsg->dwCurOLSec < 10500)
	{
		if (isCreated)
			//CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("HealthyControl11")]);
            ShowMsg(g_StrTable[_T("HealthyControl11")]);
		SetHealthyState(EHS_Healthy);
	}
	//2小时55分
	else if (pMsg->dwCurOLSec >= 10500 && pMsg->dwCurOLSec < 10560)
	{
		if (isCreated)
			//CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("HealthyControl21")]);
            ShowMsg(g_StrTable[_T("HealthyControl21")]);
		SetHealthyState(EHS_Healthy);
	}
	//(3小时~5小时)
	else if (pMsg->dwCurOLSec >= 10800 && pMsg->dwCurOLSec < 18000)
	{
		if (isCreated)
		{
			//CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("HealthyControl31")]);
			//CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("HealthyControl32")]);
            tstring strText;
            strText = g_StrTable[_T("HealthyControl31")];
            strText += g_StrTable[_T("HealthyControl32")];
            ShowMsg(strText.c_str());
		}
		SetHealthyState(EHS_Fatigue);
	}
	//(5小时~+∞)
	else if (pMsg->dwCurOLSec >= 18000)
	{
		if (isCreated)
		{
			//CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("HealthyControl41")]);
			//CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("HealthyControl42")]);
			//CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("HealthyControl43")]);
            tstring strText;
            strText = g_StrTable[_T("HealthyControl41")];
            strText += g_StrTable[_T("HealthyControl42")];
            strText += g_StrTable[_T("HealthyControl43")];
            ShowMsg(strText.c_str());
		}
		SetHealthyState(EHS_Unhealthy);
	}
	return 0;
}

DWORD HealthyControlFrame::OnNS_FatigueLimit( tagNS_FatigueLimit* pMsg, DWORD dwParam )
{
    //角色是否进入游戏场景
    BOOL isCreated = P_VALID(RoleMgr::Inst()->GetLocalPlayer());

    switch (pMsg->byLimitCode)
    {
    case E_FatigueLimit_Quest:
        CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("HealthyControlLimit01")]);
    	break;
    case E_FatigueLimit_Stall:
        CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("HealthyControlLimit02")]);
        break;
    }
	return 0;
}

void HealthyControlFrame::ShowWnd( BOOL bShown )
{
    m_pWnd->SetInvisible(!bShown);
}

VOID HealthyControlFrame::ShowMsg( LPCTSTR szMsg )
{
    if (P_VALID(GetObj("HealthyControlMsg")))
        KillObj("HealthyControlMsg");

    CreateObj("HealthyControlMsg", "MsgBox");
    TObjRef<MsgBox>("HealthyControlMsg")->Init(_T(""), szMsg, _T("HealthyControlMsgEvent"), MBF_OK, TRUE);
}