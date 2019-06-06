// EffectAttacherView.h : CEffectAttacherView 类的接口
//
#pragma once
#include "afxwin.h"
#include "ChooseEffectDialog.h"
#include "EffectAttachingFile.h"
using namespace Cool3D;

class CEffectAttacherView : public CFormView
{
protected: // 仅从序列化创建
	CEffectAttacherView();
	DECLARE_DYNCREATE(CEffectAttacherView)
public:
	enum{ IDD = IDD_EFFECTATTACHER_FORM };

// 属性
public:
	CEffectAttacherDoc* GetDocument() const;
// 操作
public:
// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnInitialUpdate(); // 构造后第一次调用

// 实现
public:
	virtual ~CEffectAttacherView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	BOOL			m_bDrag;
	CPoint			m_DragPt;
	BOOL			m_bArcBall;
// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
private:
	CStatic		m_drawWnd;
	CChooseEffectDialog		m_chooseEffectDlg;
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint _point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	CListBox m_StructList;
	CListBox m_TrackList;
protected:
	bool UpdateTrackListBox(void);
	bool UpdateStructListBox(void);
public:
	//afx_msg void OnLbnDblclkStructList();
//	afx_msg void OnLbnDblclkTrackList();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedAttachEffect();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnFileSave(LPCTSTR szPath);
	afx_msg void OnBnClickedAttacheffectthroughfile();
	CListBox m_Effects_ListBox;
	bool UpdateEffectsListBox(void);
	afx_msg void OnBnClickedDeleteeffect();
//	afx_msg void OnClose();
	afx_msg void OnBnClickedPlaytrack();
	afx_msg void OnBnClickedSaveeffecttofile();
	afx_msg void OnLbnSelchangeStructList();
};


#ifndef _DEBUG  // EffectAttacherView.cpp 的调试版本
inline CEffectAttacherDoc* CEffectAttacherView::GetDocument() const
   { return reinterpret_cast<CEffectAttacherDoc*>(m_pDocument); }
#endif

