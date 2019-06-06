#pragma once
#include "afxwin.h"
#include "afxcmn.h"

// MiniMap 对话框
#define WM_DELMINIMAP_DLG WM_USER+104

class EditorCameraCtrl;
class MiniMapDlg : public CDialog
{
	DECLARE_DYNAMIC(MiniMapDlg)
public:
	MiniMapDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~MiniMapDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_Terrain };
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);		
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg void OnPaint();
	//双击实现摄像机定位
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	CWnd*			m_pParent;
	int				m_nWidth;				//高度图的范围
	int				m_nHeight;
	TerrainEditor*	m_pTE;
	GameMapEditor*	m_pGameMapEditor;		
	EditorCameraCtrl*	m_pCamera;
	CClientDC*		m_ClientDC;
	CRect			m_rect;					//客户区范围
	int				m_xScale;				//缩放比例
	int				m_zScale;
	float			m_xGrid;				//格子大小
	float			m_zGrid;	
private:
	void GetRGB(float& gene, BYTE& r, BYTE& g, BYTE& b);  //根据高度因子得到RGB颜色		
	void DrawCamera();						//在小地图上绘制摄像机
	float HeightGene(float& h);				//高度因子
	void Update();
	void DrawStaticObjOnMiniMap();
};
