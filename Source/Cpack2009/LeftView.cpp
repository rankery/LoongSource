// LeftView.cpp : implementation of the CLeftView class
//

#include "stdafx.h"
#include "CPack.h"

#include "CPackDoc.h"
#include "LeftView.h"
#include "CPackView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "cpkcore\cpkcore.h"
#include "cpkcore\util.h"

/////////////////////////////////////////////////////////////////////////////
// CLeftView

IMPLEMENT_DYNCREATE(CLeftView, CTreeView)

BEGIN_MESSAGE_MAP(CLeftView, CTreeView)
	//{{AFX_MSG_MAP(CLeftView)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CTreeView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CTreeView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CTreeView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLeftView construction/destruction

CLeftView::CLeftView()
{
	// TODO: add construction code here
}

CLeftView::~CLeftView()
{
}

BOOL CLeftView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style |= TVS_HASLINES;
	cs.style |= TVS_LINESATROOT;
	cs.style |= TVS_HASBUTTONS;
	cs.style |= TVS_SHOWSELALWAYS;

	return CTreeView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CLeftView drawing

void CLeftView::OnDraw(CDC* pDC)
{
	CCPackDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
	pDC = pDC;


}


/////////////////////////////////////////////////////////////////////////////
// CLeftView printing

BOOL CLeftView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CLeftView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CLeftView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CLeftView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();

	// TODO: You may populate your TreeView with items by directly accessing
	//  its tree control through a call to GetTreeCtrl().
	CTreeCtrl& tree = this->GetTreeCtrl();
	CCPackDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	tree.SetImageList( pDoc->GetIconList(), TVSIL_NORMAL );

	ReAddAllDir();
}


void CLeftView::OnDestroy() 
{
	CTreeView::OnDestroy();
	
	// TODO: Add your message handler code here
}


void CLeftView::AddDirToTreeCtrl(DWORD dwFatherCRC, HTREEITEM FatherHTI, int nIcon,
								 int nSelIcon)
{
	DWORD dwChildNum = 0;
	DWORD dwChildChildNum = 0;

	CCPackDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->GetCpk()->GetFirstLevelChildDir(dwFatherCRC, dwChildNum);

	DWORD* pdwChildArray = new DWORD[dwChildNum];
	pDoc->GetCpk()->GetFirstLevelChildDir(dwFatherCRC, dwChildNum, pdwChildArray);


	static TCHAR szTemp[MAX_PATH];
	for (INT n=0; n<(INT)dwChildNum; n++)
	{
		pDoc->GetCpk()->GetSimpleFileNameFromCRC(szTemp, pdwChildArray[n]);
		HTREEITEM hti = 
			GetTreeCtrl().InsertItem(szTemp, nIcon, nSelIcon, FatherHTI);
		
		dwChildChildNum = 0;
		pDoc->GetCpk()->GetFirstLevelChildDir(pdwChildArray[n], dwChildChildNum);
		
		// 递归
		if (dwChildChildNum > 0)
			AddDirToTreeCtrl(pdwChildArray[n], hti, nIcon, nSelIcon);
	}


	delete[] pdwChildArray;
	pdwChildArray = NULL;
}



/////////////////////////////////////////////////////////////////////////////
// CLeftView diagnostics

#ifdef _DEBUG
void CLeftView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CLeftView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CCPackDoc* CLeftView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCPackDoc)));
	return (CCPackDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLeftView message handlers

DROPEFFECT CLeftView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CTreeView::OnDragEnter(pDataObject, dwKeyState, point);
}



void CLeftView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// TODO: Add your specialized code here and/or call the base class
	pSender = pSender;
	lHint = lHint;
	pHint = pHint;

	SelectCurDir();
}


// 重新添加所有目录结构
void CLeftView::ReAddAllDir() 
{
	CCPackDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	this->GetTreeCtrl().DeleteAllItems();
	
	SHFILEINFO sfi, sfisel;
	SHGetFileInfo("*.", FILE_ATTRIBUTE_DIRECTORY | SFGAO_FOLDER, &sfi, sizeof(sfi),
		SHGFI_SYSICONINDEX | SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES);
	SHGetFileInfo("*.", FILE_ATTRIBUTE_DIRECTORY | SFGAO_FOLDER, &sfisel, sizeof(sfisel),
		SHGFI_SYSICONINDEX | SHGFI_ICON | SHGFI_OPENICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES);


	DWORD dwCurCPKDirCRC = 0;
	DWORD dwChildNum = 0;
	DWORD dwChildChildNum = 0;
	HTREEITEM htiRoot = GetTreeCtrl().InsertItem(_T("Package Root"), sfi.iIcon, sfisel.iIcon);
	HTREEITEM hti;

	pDoc->GetCpk()->GetFirstLevelChildDir(dwCurCPKDirCRC, dwChildNum);
	DWORD* pdwChildArray = new DWORD[dwChildNum];
	pDoc->GetCpk()->GetFirstLevelChildDir(dwCurCPKDirCRC, dwChildNum, pdwChildArray);

	static TCHAR szTemp[MAX_PATH];
	for (INT n=0; n<(INT)dwChildNum; n++)
	{
		pDoc->GetCpk()->GetSimpleFileNameFromCRC(szTemp, pdwChildArray[n]);
		hti = this->GetTreeCtrl().InsertItem(szTemp, sfi.iIcon, sfisel.iIcon, htiRoot);

		dwChildChildNum = 0;
		pDoc->GetCpk()->GetFirstLevelChildDir(pdwChildArray[n], dwChildChildNum);
		
		if (dwChildChildNum > 0)
			AddDirToTreeCtrl(pdwChildArray[n], hti, sfi.iIcon, sfisel.iIcon);
	}

	DestroyIcon(sfi.hIcon);
	DestroyIcon(sfisel.hIcon);
	delete[] pdwChildArray;
	pdwChildArray = NULL;

	// 选择当前的目录
	this->SelectCurDir();
}



// 选择当前的目录
void CLeftView::SelectCurDir()
{
	static TCHAR szFullName[MAX_PATH];

	CCPackDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	HTREEITEM htiRoot = this->GetTreeCtrl().GetRootItem();

	DWORD dwCurDirCRC = pDoc->GetCpk()->GetCurDirCRC();

	if (dwCurDirCRC == 0)	// 现在处于根目录
	{
		this->GetTreeCtrl().SelectItem(htiRoot);
		return;
	}

	pDoc->GetCpk()->GetFullNameFromCRC(szFullName, dwCurDirCRC);
	
	CString cs = szFullName;
	CString csTemp;
	DWORD dwCRC = 0;

	HTREEITEM hChildItem = this->GetTreeCtrl().GetChildItem(htiRoot);

	// 从根目录开始往下搜索

	BOOL bChild = FALSE;	//是否包含目录
	while (cs.GetLength() > 0)
	{
		if( cs.Find('\\') != -1 )
		{
			csTemp = cs;
			cs = cs.Right(cs.GetLength() - cs.Find('\\') -1);
			csTemp = csTemp.Left(csTemp.Find('\\'));

			bChild = TRUE;
		}
		else
		{
			bChild = FALSE;
			csTemp = cs;
		}
		

		while( lstrcmp((LPCTSTR)csTemp, GetTreeCtrl().GetItemText(hChildItem)) != 0 && hChildItem )
			hChildItem = this->GetTreeCtrl().GetNextItem(hChildItem, TVGN_NEXT);

		if ( bChild == TRUE )
			hChildItem = this->GetTreeCtrl().GetChildItem(hChildItem);
		else
			goto out;
	}

out:
	this->GetTreeCtrl().SelectItem(hChildItem);

}


// 得到TreeCtrl项的全名
void CLeftView::GetTreeItemFullName(HTREEITEM hti, CString& strOut)
{
	strOut = this->GetTreeCtrl().GetItemText(hti);
	
	do
	{
		hti = this->GetTreeCtrl().GetParentItem(hti);
		if (hti != NULL)
			strOut = this->GetTreeCtrl().GetItemText(hti) + _T("\\") + strOut;

	}while(hti);

	CString csRoot(_T("Package Root\\"));
	strOut = strOut.Right(strOut.GetLength() - csRoot.GetLength());

}


// 支持用户粘贴文件到包中
void CLeftView::OnEditPaste() 
{
	// TODO: Add your command handler code here
	COleDataObject ole;
	
	// ole will auto deattach to clipboard at destruction time 
	BOOL bResult = ole.AttachClipboard();	
	if (bResult == FALSE)
		ASSERT(0);

	bResult = ole.IsDataAvailable(CF_HDROP);

	if (bResult == TRUE)	// have file paste
	{
		HGLOBAL h = ole.GetGlobalData(CF_HDROP);
		((CMainFrame*)this->GetParentFrame())
			->GetRightPane()->DropFiles((HDROP)h);
	}
}



// 左边树状目录选择改变，右边视图相应变化
void CLeftView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	// TODO: Add your control notification handler code here
	HTREEITEM ht = pNMTreeView->itemNew.hItem;
	CString cs;
	
	this->GetTreeItemFullName(ht, cs);
	
	CCPackDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pDoc->GetCpk()->SetCurDir(cs);
	pDoc->UpdateAllViews(this);
	
	this->SelectCurDir();
	*pResult = 0;
}

