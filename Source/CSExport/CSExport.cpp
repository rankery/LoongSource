/**********************************************************************
 *<
	FILE: CSExport.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/
#include "stdafx.h"
#include "CSExport.h"
#include "Resource.h"
#include "MaxExp\ExpSkeleton.h"
#include "MaxExp\FSkelFile.h"
#include "..\F3DMaxExp\MaxMath.h"
#include "..\F3DMaxExp\AniTrackDlg.h"
using namespace std;

#define CSEXPORT_CLASS_ID	Class_ID(0xaf6338f, 0x5b4373)

class CSExport : public SceneExport {
	public:


		static HWND hParams;


		int				ExtCount();					// Number of extensions supported
		const TCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
		const TCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
		const TCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
		const TCHAR *	AuthorName();				// ASCII Author name
		const TCHAR *	CopyrightMessage();			// ASCII Copyright message
		const TCHAR *	OtherMessage1();			// Other message #1
		const TCHAR *	OtherMessage2();			// Other message #2
		unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
		void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box

		BOOL SupportsOptions(int ext, DWORD options);
		int	 DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);


		
		//Constructor/Destructor

		CSExport();
		~CSExport();		

	public:
		tstring		m_szTrackName;
		bool		m_bNewFile;
		int			m_sampleRate;//每秒采样几帧
		bool		m_bAddOffset;//增加位移信息

};


class CSExportClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new CSExport(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID		ClassID() { return CSEXPORT_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("CSExport"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle

};



static CSExportClassDesc CSExportDesc;
ClassDesc2* GetCSExportDesc() { return &CSExportDesc; }


BOOL CALLBACK CSExportOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) 
{
	static CSExport *imp = NULL;
	
	switch(message) 
	{
	case WM_INITDIALOG:
		imp = (CSExport *)lParam;
		CenterWindow(hWnd,GetParent(hWnd));
		return TRUE;
	case WM_COMMAND:
		if(LOWORD(wParam)==IDC_OK)
		{
			bool ok=true;
			//--get track name
			TCHAR szBuf[128];
			HWND hEdit=::GetDlgItem(hWnd,IDC_TrackName);
			::SendMessage(hEdit,WM_GETTEXT,sizeof(szBuf),(LPARAM)szBuf);
			_tcslwr(szBuf);
			imp->m_szTrackName=szBuf;
			if(imp->m_szTrackName.length() <= 0
				|| imp->m_szTrackName.length() >4)
			{
				ok=false;
				MessageBox(hWnd,_T("Track名称的长度必须在1到4之间"),_T("Error"),MB_OK);
			}
			//--
			HWND hCheck=::GetDlgItem(hWnd,IDC_NewFile);
			imp->m_bNewFile=(BST_CHECKED==::SendMessage(hCheck,BM_GETCHECK,0,0));

			//--
			hCheck=::GetDlgItem(hWnd,IDC_AddOffset);
			imp->m_bAddOffset=(BST_CHECKED==::SendMessage(hCheck,BM_GETCHECK,0,0));

			//--
			hEdit=::GetDlgItem(hWnd,IDC_SampleRate);
			memset(szBuf,0,sizeof(szBuf));
			::SendMessage(hEdit,WM_GETTEXT,sizeof(szBuf),(LPARAM)szBuf);
			if(_tcslen(szBuf)>0)
				imp->m_sampleRate=_tstoi(szBuf);			
		
			if(ok)
				::SendMessage(hWnd,WM_CLOSE,0,0);
		}
		break;
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		return TRUE;
	}
	return FALSE;
}


//--- CSExport -------------------------------------------------------
CSExport::CSExport()
{
	m_bNewFile=false;
	m_bAddOffset=false;
	m_sampleRate=8;
}

CSExport::~CSExport() 
{

}

int CSExport::ExtCount()
{
	//TODO: Returns the number of file name extensions supported by the plug-in.
	return 1;
}

const TCHAR *CSExport::Ext(int n)
{		
	//TODO: Return the 'i-th' file name extension (i.e. "3DS").
	return _T("FSkel");
}

const TCHAR *CSExport::LongDesc()
{
	//TODO: Return long ASCII description (i.e. "Targa 2.0 Image File")
	return _T("F3D Skelton File");
}
	
const TCHAR *CSExport::ShortDesc() 
{			
	//TODO: Return short ASCII description (i.e. "Targa")
	return _T("F3D Skelton File");
}

const TCHAR *CSExport::AuthorName()
{			
	//TODO: Return ASCII Author name
	return _T("yanliang");
}

const TCHAR *CSExport::CopyrightMessage() 
{	
	// Return ASCII Copyright message
	return _T("");
}

const TCHAR *CSExport::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR *CSExport::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int CSExport::Version()
{				
	//TODO: Return Version number * 100 (i.e. v3.01 = 301)
	return 100;
}

void CSExport::ShowAbout(HWND hWnd)
{			
	// Optional
}

BOOL CSExport::SupportsOptions(int ext, DWORD options)
{
	// TODO Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;
}

class SkelAniDlg : public AniTrackDlg
{
public:
	SkelAniDlg()	{	m_pFile=NULL;}
	~SkelAniDlg()	{}

	virtual void DelTrack(const TCHAR* szName)
	{
		m_pFile->RemoveTrack(szName);
	}

	FSkelFile *m_pFile;
};

MsgBox	g_msgBox;
int	CSExport::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
	//TODO: Implement the actual file Export here and 
	//		return TRUE If the file is exported properly

	//--load old
	FSkelFile skelFile;
	bool loadOld=false;
	if(!m_bNewFile)
	{
		if(skelFile.LoadFromFile(name))
		{
			loadOld=true;
			const set<tstring>& tracks=skelFile.GetTrackSet();
			SkelAniDlg dlg;
			dlg.m_pFile=&skelFile;
			for(set<tstring>::const_iterator iter=tracks.begin();
			iter!=tracks.end();++iter)
			{
				dlg.AddTrackName(iter->c_str());
			}
			dlg.Create();
		}
	}
	
	//--显示选项
	if(!suppressPrompts)
		DialogBoxParam(hInstance, 
		MAKEINTRESOURCE(IDD_PANEL), 
		GetActiveWindow(), 
		CSExportOptionsDlgProc, (LPARAM)this);
	
	g_msgBox.Create(hInstance,IDD_EXPINFO,IDC_OUT);

	//--build ExpSkeleton
	g_msgBox.ShowMsg(_T("正在输出骨骼信息...\r\n"));
	ExpSkeleton expSkel;
	int numBones=expSkel.BuildFromMaxExportInterface(ei);
	g_msgBox.ShowMsg(_T("--共有骨骼:%d,根节点个数:%d\r\n"),numBones,expSkel.GetNumRoot());

	//--add frames to file
	TimeValue start,end;
	Max_GetAniRange(i,start,end);

	if( m_bAddOffset )
	{
		int numFrm=skelFile.AddTrackOffset(m_szTrackName.c_str(),&expSkel,start,end,m_sampleRate);
		g_msgBox.ShowMsg(_T("Track:%s已经加入位移信息,采样%d帧.\r\n"),m_szTrackName.c_str(),numFrm);
	}
	else
	{
		bool remove=skelFile.RemoveTrack(m_szTrackName.c_str());
		if(remove)
			g_msgBox.ShowMsg(_T("旧的Track已经替代.\r\n"));
		int numFrm=skelFile.AddTrack(m_szTrackName.c_str(),&expSkel,start,end,m_sampleRate);
		g_msgBox.ShowMsg(_T("新Track已经加入,采样%d帧.\r\n"),numFrm);
	}
	
	//--write file
	if(skelFile.WriteToFile(name))
	{
		g_msgBox.ShowMsg(_T("文件写入成功.\r\n已有Track:\r\n"));
		for(set<tstring>::iterator iter=skelFile.m_tracks.begin();
			iter!=skelFile.m_tracks.end();iter++)
			{
				const tstring& str=(*iter);
				g_msgBox.ShowMsg(_T("%s\r\n"),str.c_str());
			}
	}
	else
	{
		g_msgBox.ShowMsg(_T("文件写入失败!\r\n"));
	}

	return TRUE;
}


