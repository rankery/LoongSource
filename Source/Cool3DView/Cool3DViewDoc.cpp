// Cool3DViewDoc.cpp :  CCool3DViewDoc 类的实现
//

#include "stdafx.h"
#include "Cool3DView.h"

#include "Cool3DViewDoc.h"
#include "Cool3DModels.h"
#include "Cool3DViewWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCool3DViewDoc

IMPLEMENT_DYNCREATE(CCool3DViewDoc, CDocument)

BEGIN_MESSAGE_MAP(CCool3DViewDoc, CDocument)
END_MESSAGE_MAP()


// CCool3DViewDoc 构造/析构

CCool3DViewDoc::CCool3DViewDoc()
{
	// TODO: 在此添加一次性构造代码

}

CCool3DViewDoc::~CCool3DViewDoc()
{
}

BOOL CCool3DViewDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CCool3DViewDoc 序列化

void CCool3DViewDoc::Serialize(CArchive& ar)
{
	const CFile *pFile=ar.GetFile();
	CString szPath=pFile->GetFilePath();
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
		Cool3DModels::Inst()->LoadModel(szPath);
		int result  = szPath.Find(_T(".fskel"));
		if(result != -1)
			szPath.Replace(_T(".fskel"), _T(".csv"));
		result = szPath.Find(_T(".FSKEL"));
		if(result != -1)
			szPath.Replace(_T(".FSKEL"), _T(".csv"));
		result = szPath.Find(_T(".FAK"));
		if(result != -1)
			szPath.Replace(_T(".FAK"), _T(".csv"));
		result = szPath.Find(_T(".fak"));
		if(result != -1)
			szPath.Replace(_T(".fak"), _T(".csv"));

		if(szPath.Find(_T(".csv")) != -1)
		{
			if(m_cstrCSVFilePath != szPath)
			{
				m_aniMsgTable.Clear();
				g_viewWnd->m_actMsgDlg.ClearAll();
			}
			m_aniMsgTable.LoadFromFile(Kernel::Inst()->DefaultFS(), szPath);
			m_cstrCSVFilePath = szPath;
		}
	}
}

AniMsgTable* CCool3DViewDoc::GetAniMsgTable()
{
	return &m_aniMsgTable;
}

CString CCool3DViewDoc::GetCSVFilePath()
{
	return m_cstrCSVFilePath;
}

void CCool3DViewDoc::SetCSVFilePath(const TCHAR *szPath)
{
	m_cstrCSVFilePath=szPath;
}

// CCool3DViewDoc 诊断

#ifdef _DEBUG
void CCool3DViewDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCool3DViewDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CCool3DViewDoc 命令
