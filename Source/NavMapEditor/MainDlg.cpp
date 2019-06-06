// MainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NavMapEditor.h"
#include "PlugNavMapEditor.h"
#include "MainDlg.h"


// CMainDlg dialog

IMPLEMENT_DYNAMIC(CMainDlg, CDialog)

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMainDlg::IDD, pParent)
	, m_szInner(_T("3"))
	, m_szOuter(_T("6"))
	, m_bCango(FALSE)
	, m_bNpcNav(FALSE)
	, m_bPlayerNav(FALSE)
	, m_bEditCango(FALSE)
	, m_bEditPlayerRoad(FALSE)
	, m_bPlayerRoad(FALSE)
	, m_bNPCWayPoint(FALSE)
{

}

CMainDlg::~CMainDlg()
{
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BRUSHSHAPE, m_shapeTypeList);
	DDX_Text(pDX, IDC_INNER, m_szInner);
	DDX_Text(pDX, IDC_OUTER, m_szOuter);
	DDX_Check(pDX, IDC_CHECK_PreLook, m_bCango);
	DDX_Check(pDX, IDC_CHECK_NPCNavPreLook, m_bNpcNav);
	DDX_Check(pDX, IDC_CHECK_PlayerNavPreLook, m_bPlayerNav);
	DDX_Check(pDX, IDC_Edit, m_bEditCango);
	DDX_Check(pDX, IDC_EditManualPlayerNav, m_bEditPlayerRoad);
	DDX_Check(pDX, IDC_CHECK_PreLook_ManualPlayerNav, m_bPlayerRoad);
	DDX_Check(pDX, IDC_CHECK_PreLook_NPCWayPoint, m_bNPCWayPoint);
}


BEGIN_MESSAGE_MAP(CMainDlg, CDialog)
	ON_BN_CLICKED(IDC_SAVE, &CMainDlg::OnBnClickedSave)
	ON_BN_CLICKED(IDC_LOAD, &CMainDlg::OnBnClickedLoad)
	ON_BN_CLICKED(IDC_CLEAR, &CMainDlg::OnBnClickedClear)
	ON_EN_CHANGE(IDC_ShowDist, &CMainDlg::OnEnChangeShowdist)
	ON_BN_CLICKED(IDC_BUTTON2_PlayerNavLoad, &CMainDlg::OnBnClickedPlayerNavLoad)
	ON_BN_CLICKED(IDC_BUTTON1_NPCNavLoad, &CMainDlg::OnBnClickedNpcNavLoad)
	ON_BN_CLICKED(IDC_BUTTON_NPCNavLook, &CMainDlg::OnBnClickedButtonNpcNavlook)
	ON_BN_CLICKED(IDC_BUTTON_PlayerNavLook, &CMainDlg::OnBnClickedButtonPlayerNavLook)
	ON_BN_CLICKED(IDC_ClearManualPlayerNav, &CMainDlg::OnBnClickedClearPlayerRoad)
	ON_BN_CLICKED(IDC_BUTTON_NPCWayPointLoad, &CMainDlg::OnBnClickedButtonNpcwaypointload)
	ON_BN_CLICKED(IDC_BUTTON_CangoLook, &CMainDlg::OnBnClickedButtonCangolook)
	ON_BN_CLICKED(IDC_BUTTON_RoadLook, &CMainDlg::OnBnClickedButtonRoadlook)
END_MESSAGE_MAP()


// CMainDlg message handlers

void CMainDlg::OnBnClickedSave()
{
	if(IDYES!=AfxMessageBox(_T("存盘将覆盖上次保存的内容,继续吗?"),MB_YESNO|MB_ICONWARNING))
		return;

	//--存盘
	if(g_navEditor->Save())
		AfxMessageBox(_T("存盘成功"));
}

void CMainDlg::OnBnClickedLoad()
{
	if(IDYES!=AfxMessageBox(_T("读盘将清空正在编辑的内容,继续吗?"),MB_YESNO|MB_ICONWARNING))
		return;

	//--读盘
	if(g_navEditor->Load())
		AfxMessageBox(_T("读盘成功"));
}

void CMainDlg::OnBnClickedClear()
{
	// TODO: Add your control notification handler code here

	//--清除导航图
	if(IDYES!=AfxMessageBox(_T("以前编辑的可行走区域将被全部清除,继续吗?"),MB_YESNO|MB_ICONWARNING))
		return;
	g_navEditor->ClearCangoMap();
}

void CMainDlg::OnBnClickedClearPlayerRoad()
{
	if(IDYES!=AfxMessageBox(_T("以前编辑的玩家寻路道路区域将被全部清除,继续吗?"),MB_YESNO|MB_ICONWARNING))
		return;

	g_navEditor->ClearPlayerRoadMap();
}

void CMainDlg::GetBrushRadius( int& inner,int& outer )
{
	UpdateData();
	inner=_tstoi(m_szInner);
	outer=_tstoi(m_szOuter);
}

int CMainDlg::GetBrushShape()
{
	return m_shapeTypeList.GetCurSel();
}

BOOL CMainDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	UpdateData(FALSE);

	TCHAR szTxt[512];
	_stprintf(szTxt,_T("%d"),g_drawSize);
	GetDlgItem(IDC_ShowDist)->SetWindowText(szTxt);

	m_shapeTypeList.AddString(_T("圆形"));
	m_shapeTypeList.AddString(_T("方形"));
	m_shapeTypeList.AddString(_T("线型"));
	m_shapeTypeList.AddString(_T("平滑"));
	m_shapeTypeList.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}
void CMainDlg::OnEnChangeShowdist()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString szTxt;
	GetDlgItem(IDC_ShowDist)->GetWindowText(szTxt);
	if(!szTxt.IsEmpty())
	{
		g_drawSize=_tstoi(szTxt);
	}
}

void CMainDlg::OnBnClickedPlayerNavLoad()
{
	// TODO: Add your control notification handler code here
	if(g_navEditor->LoadPlayerNavMap())
		AfxMessageBox(_T("读取玩家寻路导航图成功!"));
}

void CMainDlg::OnBnClickedNpcNavLoad()
{
	// TODO: Add your control notification handler code here
	//--读盘
	if(g_navEditor->LoadNPCNavMap())
		AfxMessageBox(_T("读取NPC格子导航图成功!"));
}

void CMainDlg::OnBnClickedButtonNpcNavlook()
{
	// TODO: Add your control notification handler code here
	g_navEditor->DisplayNPCCangoMap();
}

void CMainDlg::OnBnClickedButtonPlayerNavLook()
{
	// TODO: Add your control notification handler code here
	g_navEditor->DisplayPlayerCangoMap();
}


void CMainDlg::OnBnClickedButtonCangolook()
{
	// TODO: Add your control notification handler code here
	g_navEditor->DisplayManuelCangoMap();
}

void CMainDlg::OnBnClickedButtonRoadlook()
{
	// TODO: Add your control notification handler code here
	g_navEditor->DisplayRoadMap();
}


void CMainDlg::OnBnClickedButtonNpcwaypointload()
{
	try
	{
		g_navEditor->LoadNPCNavGraph();
		AfxMessageBox(_T("读取NPC路点导航图成功!"));
	}
	catch (...)
	{}
}
