// launcherDlg.h : 头文件
//

#pragma once
#include "webview.h"
#include "picbutton.h"
#include "picprogress.h"
#include "DownloadInfo.h"
#include "Config.h"

#define WM_USER_INFO				(WM_USER + 10)
#define WM_USER_PROGRESS_RANGE		(WM_USER + 20)
#define WM_USER_PROGRESS_POS		(WM_USER + 30)
#define WM_USER_DOWNLOAD_SPEED		(WM_USER + 40)
#define WM_USER_LOG					(WM_USER + 50)
#define WM_USER_CAN_UPDATE			(WM_USER + 60)
#define WM_USER_CAN_RUN_GAME		(WM_USER + 70)

#define WM_USER_GAME_VER			(WM_USER + 80)
#define WM_USER_LAUNCHER_VER		(WM_USER + 90)
#define WM_USER_LATEST_GAME_VER		(WM_USER + 100)
#define WM_USER_LATEST_LAUNCHER_VER	(WM_USER + 110)


class Process;

// CLauncherDlg 对话框
class CLauncherDlg : public CDialog
{
// 构造
public:
	CLauncherDlg(CWnd* pParent = NULL);	// 标准构造函数
	virtual ~CLauncherDlg();

	DownloadInfo*	GetDownloadInfoBox() { return &m_DlgDownInfo;}
	CPicButtion* GetButtonUpdate() { return &m_ButtonUpdate; }
	Process* GetProcess() { return m_pProcess; }

// 对话框数据
	enum { IDD = IDD_LAUNCHER_DIALOG };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持



// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	//按钮单击响应
	afx_msg void OnUpdate();
	afx_msg void OnStart();
	afx_msg void OnSetup();
	afx_msg void OnInfo();
	afx_msg void OnQuit();

	DECLARE_MESSAGE_MAP()

	//消息处理
	virtual BOOL PreTranslateMessage(MSG* pMsg); 
	//消息处理
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
	CWebView		m_Web;		// ie控件
	CPoint			m_ptMouse;
	CBitmap			m_BackGround;

	CPicButtion		m_ButtonState;		// 状态
	CPicButtion		m_ButtonProgress;	// 进度

	CPicButtion		m_ButtonGameVer;			// 游戏版本
	CPicButtion		m_ButtonLauncherVer;		// launcher版本
	CPicButtion		m_ButtonLatestGameVer;		// 最新游戏版本
	CPicButtion		m_ButtonLatestLauncherVer;	// 最新launcher版本

	CPicProgress	m_ProgressDownload;
	CPicProgress	m_ProgressInstall;

	CPicButtion		m_ButtonUpdate;
	CPicButtion		m_ButtonStart;
	CPicButtion		m_ButtonSetup;
	CPicButtion		m_ButtonInfo;
	CPicButtion		m_ButtonQuit;

	DownloadInfo	m_DlgDownInfo;
	CConfig			m_DlgConfig;
	Process*		m_pProcess;


	VOID CreateControls();
	HRGN BitmapToRegion (HBITMAP hBmp, COLORREF cTransparentColor = 0, COLORREF cTolerance = 0x101010);

	VOID EnterGame();
};
