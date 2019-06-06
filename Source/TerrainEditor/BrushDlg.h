#pragma once
#include "afxwin.h"

// CBrushDlg 对话框

enum EEditorLerpType
{
	EELerp_None=0,		//无插值
	EELerp_Linear,		//线性插值
	EELerp_Cosine,		//cos插值
	EELerp_Quadratic,	//二次插值
};
enum EEditorShapeType
{
	EEShape_Circle=0,	//圆形笔刷
	EEShape_Quad,		//方形笔刷，具有内外两个Quad
	EEShape_Line,		//线型笔刷，
	EEShape_Plane,		//平滑笔刷
	EEShape_TypeDef,	//自定义四边形，用于编辑四边形区域的斜坡
};
struct tagBrushOpt
{
	int					inner;		//笔刷内径大小
	int					outer;		//笔刷外径大小
	int					strength;
	EEditorShapeType	shape;
	EEditorLerpType		lerp;
};

class CBrushDlg : public CDialog
{
	DECLARE_DYNAMIC(CBrushDlg)

public:
	CBrushDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBrushDlg();

// 对话框数据
	enum { IDD = IDD_BRUSH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	void GetNoiseSet(float& freq);
	void GetSetting(int& inner,int& outer,int *pStr=NULL);
	int GetLerpType();
	int GetShape();

	tagBrushOpt GetBrushOpt() const;
	void SetBrushOpt(const tagBrushOpt& brushOpt);

protected:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedRenoise();

	virtual BOOL OnInitDialog();
	virtual void OnCancel();

	CString m_szInner;
	CString m_szOuter;
	CString m_szStr;
	BOOL m_bLock;
	CString m_szFreq;

	tagBrushOpt m_brushOpt;
public:
	int m_lerpType;
	CComboBox m_hLerpType;
	CComboBox m_hShapeType;
};
