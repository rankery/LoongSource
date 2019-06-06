#pragma once


// CConfig dialog

class VarContainer;
class CConfig : public CDialog
{
	DECLARE_DYNAMIC(CConfig)

public:
	CConfig(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConfig();

	virtual BOOL OnInitDialog();

	VOID Load(LPCTSTR szFileName);
	VOID Save();

// Dialog Data
	enum { IDD = IDD_CONFIG };

	CButton m_ctrlWindow0;		// 窗口模式0
	CButton m_ctrlWindow1;		// 窗口模式1
	CButton m_ctrlWindow2;		// 窗口模式2
	CButton m_ctrlFullScreen;	// 全屏模式
	CStatic m_ctrlTips;

	CButton m_ctrlVerticalSync;		// 垂直同步
	CButton m_ctrlTextureDetail;	// 高精度纹理
	CButton m_ctrlGroundPrecision;	// 高精度地表
	CButton m_ctrlGPUAni;			// GPU动画
	CButton m_ctrlGroundHighLight;	// 地表高亮
	CButton m_ctrlNormalMap;		// 法线贴图
	CButton m_ctrlMusic;			// 音乐
	CButton m_ctrlSound;			// 音效

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

private:
	VarContainer*	m_pVarContainer;
	CString			m_strFileName;
};
