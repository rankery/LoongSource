// CPackDoc.cpp : implementation of the CCPackDoc class
//

#include "stdafx.h"

HWND  g_hWndFileName;
HWND  g_hWndProgress;
HWND  g_hWndInfo;

#include <Mmsystem.h>
#include "CPack.h"

#include "CPackDoc.h"
#include "Mainfrm.h"
#include "LeftView.h"
#include "cpkcore\cpkcore.h"
#include "cpkcore\disciomgr.h"
#include "cpkcore\util.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCPackDoc

IMPLEMENT_DYNCREATE(CCPackDoc, CDocument)

BEGIN_MESSAGE_MAP(CCPackDoc, CDocument)
	//{{AFX_MSG_MAP(CCPackDoc)
	ON_COMMAND(ID_ADD, OnAdd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCPackDoc construction/destruction

CCPackDoc::CCPackDoc()
{
	// TODO: add one-time construction code here
	m_pCpk = NULL;
	m_pCpk = new CPK;
	ASSERT( m_pCpk != NULL );

    // Get a handle to the system small icon image list, and set that as the
    // image list for our list control
	SHFILEINFO rInfo;
	HIMAGELIST hImglist;
    hImglist = (HIMAGELIST)SHGetFileInfo( _T(""), 0, &rInfo, sizeof(SHFILEINFO), 
                            SHGFI_ICON | SHGFI_SMALLICON | SHGFI_SYSICONINDEX);

	m_IconList.Attach( hImglist );
    hImglist = (HIMAGELIST)SHGetFileInfo( _T(""), 0, &rInfo, sizeof(SHFILEINFO), 
                            SHGFI_ICON | SHGFI_LARGEICON | SHGFI_SYSICONINDEX );
	m_BigIconList.Attach( hImglist );
}

CCPackDoc::~CCPackDoc()
{
	// 去掉关联
    if ( NULL != m_IconList.GetSafeHandle() )
        m_IconList.Detach();
    if ( NULL != m_BigIconList.GetSafeHandle() )
        m_BigIconList.Detach();

	SAFE_DEL(m_pCpk);
}


// 新建一个包
BOOL CCPackDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

retry:

	CString cs;
	static TCHAR BASED_CODE szFilter[] = _T("CPack 包文件 (*.cpk) | *.cpk ||");
	CFileDialog f(TRUE, NULL, _T("*.cpk"), OFN_PATHMUSTEXIST | OFN_EXPLORER
		| OFN_HIDEREADONLY, szFilter);

	f.m_ofn.lpstrTitle = _T("新建或者打开一个包 - CPack 文件包装工具");

	INT nResult = f.DoModal();
	if( nResult == IDOK )
	{
		cs = f.GetPathName();
		cs.MakeLower();

		// 如果文件已经存在则打开现有文件
		if( CDiscIOMgr::IsExist((LPCTSTR)cs) )
		{
			if( m_pCpk->IsValidCPK((LPCTSTR)cs) == FALSE )
			{
				goto retry;
			}

			this->SetTitle((LPCTSTR)cs);
			return OnOpenDocument((LPCTSTR)cs);
		}
		else
		{
			if( lstrcmp((LPCTSTR)cs.Right(4), _T(".cpk")) != 0 )
			{
				cs += _T(".cpk");
			}
			this->SetTitle((LPCTSTR)cs);
		}

		m_pCpk->Unload();
		m_pCpk->Create((LPCTSTR)cs);
	}
	else
	{
		if( m_pCpk->IsLoaded() == FALSE )
			exit(0);
	}

	this->UpdateAllViews(NULL);
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CCPackDoc serialization

void CCPackDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCPackDoc diagnostics

#ifdef _DEBUG
void CCPackDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCPackDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCPackDoc commands

#include <cderr.h>
// 用户选择添加文件
void CCPackDoc::OnAdd() 
{
	// TODO: Add your command handler code here
	int nResult = 0;
	BOOL bResult = FALSE;
	static TCHAR szTemp[256*255];
	ZeroMemory(szTemp, sizeof(szTemp));

	CString cs;
	CFileDialog f(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT | OFN_PATHMUSTEXIST
		| OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_HIDEREADONLY);

	f.m_ofn.lpstrFile = szTemp;
	f.m_ofn.nMaxFile = sizeof(szTemp);
	f.m_ofn.lpstrTitle = _T("加入文件到包中"); 

	nResult = f.DoModal();

	if (nResult == IDOK)
	{
		POSITION pos = f.GetStartPosition();
		do
		{
			cs = f.GetNextPathName(pos);
			bResult = m_pCpk->AddFile(cs, m_pCpk->GetCurDirCRC());
			ASSERT(bResult);

		}while (pos != NULL);
			
		// 重新得到目录结构
		((CMainFrame*)(AfxGetApp()->GetMainWnd()))->GetLeftPane()->ReAddAllDir();

		this->UpdateAllViews(NULL);
	}
	else
	{
		DWORD dwErr = CommDlgExtendedError();

		switch(dwErr)
		{
		case 0:
			break;
		case FNERR_BUFFERTOOSMALL:
			Msg(_T("选择的文件太多，缓冲区无法容纳"));
			break;
		default:
			Msg(_T("Unknow common dialog extended error : %lu"), dwErr);
			break;
		}
	}
}



// 用户打开文件包
BOOL CCPackDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if( !CDocument::OnOpenDocument(lpszPathName) )
		return FALSE;
	
	if( FALSE == m_pCpk->IsValidCPK(lpszPathName) )
		return FALSE;

	// TODO: Add your specialized creation code here
	m_pCpk->Unload();
	//DWORD dwTime = timeGetTime();
	m_pCpk->Load(lpszPathName);

	//char szMess[256];
	//sprintf(szMess, "time=%d", timeGetTime()-dwTime);
	//MessageBox(NULL, szMess, "time", MB_OK);
	
	this->SetTitle(lpszPathName);
	return TRUE;
}

