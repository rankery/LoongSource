// TEditorSheet.cpp : 实现文件
//

#include "stdafx.h"
#include "TerrainEditor.h"
#include "TEditorSheet.h"
#include "Resource.h"
#include ".\teditorsheet.h"
#include "PlugTerrainEditor.h"


// TEditorSheet

IMPLEMENT_DYNAMIC(TEditorSheet, CPropertySheet)
TEditorSheet::TEditorSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

TEditorSheet::TEditorSheet(CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(_T("地形编辑器"), pParentWnd, iSelectPage)
{
}

TEditorSheet::~TEditorSheet()
{
}


BEGIN_MESSAGE_MAP(TEditorSheet, CPropertySheet)
	ON_WM_MOVING()
	ON_WM_MOVE()
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()


// TEditorSheet 消息处理程序

void TEditorSheet::AddPages()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_heightPage.Construct(IDD_PROPPAGE_HEIGHTMAP);
	m_visPage.Construct(IDD_PROPPAGE_VIS);
	m_layerPage.Construct(IDD_PROPPAGE_LAYER);
	m_decoPage.Construct(IDD_PROPPAGE_DECO);
	//m_waterPage.Construct(IDD_PROPPAGE_Water);
	m_riverPage.Construct(IDD_PROPPAGE_River);
	AddPage(&m_heightPage);
	AddPage(&m_layerPage);
	AddPage(&m_decoPage);
	//AddPage(&m_waterPage);
	AddPage(&m_riverPage);
	AddPage(&m_visPage);
}
void TEditorSheet::OnMoving(UINT fwSide, LPRECT pRect)
{
	CPropertySheet::OnMoving(fwSide, pRect);

	// TODO: 在此处添加消息处理程序代码
}

bool TEditorSheet::IsShowEdge()
{
	if(::IsWindow(m_heightPage.m_btnShowEdge))
	{
		return m_heightPage.m_btnShowEdge.GetCheck() == BST_CHECKED;
	}
	else
		return true;
}

bool TEditorSheet::IsShowLayerNum()
{
    if(::IsWindow(m_layerPage.m_btnShowLayerCount))
	{
		return m_layerPage.m_btnShowLayerCount.GetCheck() == BST_CHECKED;
	}
	else
		return false;
}

bool TEditorSheet::IsShowSubPatchEdge()
{
	if(::IsWindow(m_layerPage.m_btnShowSubPatchEdge))
	{
		return m_layerPage.m_btnShowSubPatchEdge.GetCheck() == BST_CHECKED;
	}
	else
		return false;
}

void TEditorSheet::OnMove(int x, int y)
{
	CPropertySheet::OnMove(x, y);

	// TODO: 在此处添加消息处理程序代码
	g_editor->AlignBrushDlg();
}

CString TEditorSheet::GetActiveTools(CString& subName)
{
	switch(GetActiveIndex())
	{
	case 0://height map
		{
			int sel=m_heightPage.m_editTools.GetCurSel();
			if(sel!=-1)
			{
				ASSERT(sel>=0 && sel<HEditToolsNum);
				return HEditToolsName[sel];
			}
		}
		break;
	case 1://layer
		{
			CString selLayer=m_layerPage.GetSelLayerName();
			int selTool=m_layerPage.m_editTools.GetCurSel();
			if(!selLayer.IsEmpty() && selTool!=-1)
			{
				subName=selLayer;
				switch(selTool)
				{
				case 0:
					return _T("layerBrush");
				case 1:
					return _T("layerNoise");
				case 2:
					return _T("layerSmooth");
				}
			}//endof if
		}
		break;
	case 2://deco
		{
			subName=m_decoPage.GetSelLayerName();
			if(m_decoPage.IfEnableNoise())
				return _T("decoNoise");
			else
				return _T("decoBrush");
		}
		break;
	case 3://river
		{
			UINT riverIndex;
			if(m_riverPage.GetSelRiverIndex(riverIndex))
			{
				int selTool=m_riverPage.m_editTools.GetCurSel();
				if(selTool!=-1)
				{
					TCHAR szTemp[10];
					subName=_itot(riverIndex,szTemp,10);
					switch(selTool)
					{
					case 0:
						return _T("riverBrush");
					case 1:
						return _T("riverNoise");
					case 2:
						return _T("riverSmooth");
					}
				}
			}
		}
		break;
	case 5://vis
		break;
	default:
		ASSERT(0 && _T("unknown page"));
	}//endof switch
	return _T("None");
}

void TEditorSheet::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(nID==SC_CLOSE)
	{
		g_editor->ShowWindow(false);
		return;
	}


	CPropertySheet::OnSysCommand(nID, lParam);
}

BOOL TEditorSheet::OnInitDialog()
{
	BOOL ret=CPropertySheet::OnInitDialog();
	CString szTitle;
	GetWindowText(szTitle);
	CString szVer;
#ifdef _DEBUG
	szVer.Format(_T("--Build %s Debug"),__TDATE__);
#else
	szVer.Format(_T("--Build %s Release"),__TDATE__);
#endif
	szTitle+=szVer;
	SetWindowText(szTitle);

	return ret;
}