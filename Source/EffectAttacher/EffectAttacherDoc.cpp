// EffectAttacherDoc.cpp :  CEffectAttacherDoc 类的实现
//

#include "stdafx.h"
#include "EffectAttacher.h"
#include "EffectAttacherDoc.h"

#include "Cool3DModels.h"

#include "EffectAttacherView.h"
#include ".\effectattacherdoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEffectAttacherDoc

IMPLEMENT_DYNCREATE(CEffectAttacherDoc, CDocument)

BEGIN_MESSAGE_MAP(CEffectAttacherDoc, CDocument)
END_MESSAGE_MAP()


// CEffectAttacherDoc 构造/析构

CEffectAttacherDoc::CEffectAttacherDoc()
{
	// TODO: 在此添加一次性构造代码

}

CEffectAttacherDoc::~CEffectAttacherDoc()
{
}

BOOL CEffectAttacherDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}


// CEffectAttacherDoc 序列化

void CEffectAttacherDoc::Serialize(CArchive& ar)
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
		Cool3DModels::Inst()->loadModel(szPath);
		int result  = szPath.Find(_T(".fskel"));
		if(result != -1)
			szPath.Replace(_T(".fskel"), _T(".csv"));
		result = szPath.Find(_T(".FSKEL"));
		if(result != -1)
			szPath.Replace(_T(".FSKEL"), _T(".csv"));
		// del by bwen: 2006.4.11
		//result = szPath.Find(".FAK");
		//if(result != -1)
		//	szPath.Replace(".FAK", ".csv");
		//result = szPath.Find(".fak");
		//if(result != -1)
		//	szPath.Replace(".fak", ".csv");

		if(szPath.Find(_T(".csv")) != -1)
		{
			if(m_cstrCSVFilePath != szPath)
			{
				m_aniMsgTable.Clear();
				//g_viewWnd->m_actMsgDlg.ClearAll();
			}

			m_aniMsgTable.LoadFromFile(Kernel::Inst()->DefaultFS(), szPath);
			m_cstrCSVFilePath = szPath;
		}
	}

	AnimationCtrl *pAni=Cool3DModels::Inst()->GetSceneNodeAni();

	/*
	//开始第一个Track
	if(pAni!=NULL)
	{
		//pAni->OpenTrack("walk", EAni_Loop);
		pAni->SetTimeScale(1);
	}
	else
		AfxMessageBox("77886699467");
	*/

	//CListBox * MainWndTrackList = (CListBox *) AfxGetApp()->GetMainWnd()->GetDescendantWindow(IDC_TRACK_LIST);
	//MainWndTrackList->AddString("123123");
}

AniMsgTable * CEffectAttacherDoc::GetAniMsgTable()
{
	return &m_aniMsgTable;
}

CString CEffectAttacherDoc::GetCSVFilePath()
{
	return m_cstrCSVFilePath;
}

// CEffectAttacherDoc 诊断

#ifdef _DEBUG
void CEffectAttacherDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CEffectAttacherDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CEffectAttacherDoc 命令

void CEffectAttacherDoc::OnCloseDocument()
{
	// TODO: 在此添加专用代码和/或调用基类
	if(!Cool3DModels::Inst()->ApplicationClose())
	{
		AfxMessageBox(_T("退出时程序遇到问题！"));
	}


	CDocument::OnCloseDocument();
}
