#pragma once


#include "THeightMapPage.h"
#include "TLayerPage.h"
#include "TDecoPage.h"
#include "TWaterPage.h"
#include "TRiverPage.h"
#include "TVisPage.h"
// TEditorSheet

class TEditorSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(TEditorSheet)

public:
	TEditorSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	TEditorSheet(CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~TEditorSheet();

	void AddPages();
	CString GetActiveTools(CString& subName);
	bool IsShowEdge();
	bool IsShowLayerNum();
	bool IsShowSubPatchEdge();
	TRiverPage* GetRiverPage()	{	return &m_riverPage;}
protected:
	THeightMapPage	m_heightPage;
	TVisPage		m_visPage;
	TLayerPage		m_layerPage;
	TDecoPage		m_decoPage;
	TWaterPage		m_waterPage;
	TRiverPage		m_riverPage;
protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
};


