/**********************************************************************
 *<
	FILE: ActorKeyFrameExport.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#include "..\MeshExporter.h"
#include "..\MsgBox.h"
#include "ActorKeyFrameFile.h"
#include <string>
#include "..\..\F3DMaxExp\ExpMtlOptionDlg.h"
#include "..\..\F3DMaxExp\F3DExpMtl.h"
#include "..\..\F3DMaxExp\AniTrackDlg.h"

using namespace Cool3D;
using namespace std;
#define ActorKeyFrameExport_CLASS_ID	Class_ID(0x6627770f, 0x42637c31)

class ActorKeyFrameExport : public SceneExport {
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
		int				DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);


		
		//Constructor/Destructor

		ActorKeyFrameExport();
		~ActorKeyFrameExport();		

	public:
		tstring		m_szActionName;
		int			m_sampleRate;
		bool		m_bIsFirst;

};


class ActorKeyFrameExportClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new ActorKeyFrameExport(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME2); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID		ClassID() { return ActorKeyFrameExport_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("ActorKeyFrameExport"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle

};



static ActorKeyFrameExportClassDesc ActorKeyFrameExportDesc;
ClassDesc2* GetActorKeyFrameExportDesc() { return &ActorKeyFrameExportDesc; }


BOOL CALLBACK ActorKeyFrameExportOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) 
{
	static ActorKeyFrameExport *imp = NULL;

	switch(message) 
	{
		case WM_INITDIALOG:
			imp = (ActorKeyFrameExport *)lParam;
			CenterWindow(hWnd,GetParent(hWnd));
			::SetWindowText(::GetDlgItem(hWnd, IDC_SAMPLERATE), _T("8"));
			::SetCheckBox(hWnd,IDC_RADIOFirst,TRUE);
			return TRUE;

		case WM_COMMAND:
			if(LOWORD(wParam)==IDC_OK)
			{
				::SendMessage(hWnd,WM_CLOSE,0,0);
			}
			break;
		case WM_CLOSE:
			{
				TCHAR szBuf[128] = {0};
				HWND hEdit=::GetDlgItem(hWnd,IDC_ACTION);
				::SendMessage(hEdit,WM_GETTEXT,sizeof(szBuf),(LPARAM)szBuf);
				imp->m_szActionName=szBuf;
				if(imp->m_szActionName.length()<=0
					|| imp->m_szActionName.length()>4)
				{
					MessageBox(hWnd,_T("动作名称长度必须在1到4个字母之间"),_T("Error"),MB_OK);
					return TRUE;
				}

				if( BST_CHECKED == ::GetCheckBox(hWnd, IDC_RADIOFirst) )
				{
					imp->m_bIsFirst = true;
				}
				else
				{
					imp->m_bIsFirst = false;
				}

				hEdit = ::GetDlgItem(hWnd, IDC_SAMPLERATE);
				memset(szBuf, 0, sizeof(szBuf));
				::SendMessage(hEdit,WM_GETTEXT,sizeof(szBuf),(LPARAM)szBuf);

				if(strlen(szBuf) <= 0)
					imp->m_sampleRate = 8;
				else
					imp->m_sampleRate = atoi(szBuf);


				EndDialog(hWnd, 0);
				return TRUE;
			}
	}
	return FALSE;
}


//--- ActorKeyFrameExport -------------------------------------------------------
ActorKeyFrameExport::ActorKeyFrameExport()
{
	m_sampleRate = 8;
	m_bIsFirst = true;
}

ActorKeyFrameExport::~ActorKeyFrameExport() 
{

}

int ActorKeyFrameExport::ExtCount()
{
	//TODO: Returns the number of file name extensions supported by the plug-in.
	return 1;
}

const TCHAR *ActorKeyFrameExport::Ext(int n)
{		
	//TODO: Return the 'i-th' file name extension (i.e. "3DS").
	return _T("FAK");//Animation key frame
	
}

const TCHAR *ActorKeyFrameExport::LongDesc()
{
	//TODO: Return long ASCII description (i.e. "Targa 2.0 Image File")
	return _T("F3D KeyFrame Animation");
}
	
const TCHAR *ActorKeyFrameExport::ShortDesc() 
{			
	//TODO: Return short ASCII description (i.e. "Targa")
	return _T("F3D KeyFrame Animation");
}

const TCHAR *ActorKeyFrameExport::AuthorName()
{			
	//TODO: Return ASCII Author name
	return _T("Yanliang Fang");
}

const TCHAR *ActorKeyFrameExport::CopyrightMessage() 
{	
	// Return ASCII Copyright message
	return _T("");
}

const TCHAR *ActorKeyFrameExport::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR *ActorKeyFrameExport::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int ActorKeyFrameExport::Version()
{				
	//TODO: Return Version number * 100 (i.e. v3.01 = 301)
	return 100;
}

void ActorKeyFrameExport::ShowAbout(HWND hWnd)
{			
	// Optional
}

BOOL ActorKeyFrameExport::SupportsOptions(int ext, DWORD options)
{
	// TODO Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;
}

class ActorDlg : public AniTrackDlg
{
public:
	ActorDlg()	{	m_pFile=NULL;}
	~ActorDlg()	{}

	virtual void DelTrack(const TCHAR* szName)
	{
		m_pFile->RemoveAction(_MakeFourCCA(szName));
	}

	ActorKeyFrameFile *m_pFile;
};

int	ActorKeyFrameExport::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
	//TODO: Implement the actual file Export here and 
	//		return TRUE If the file is exported properly

	//--
	ActorKeyFrameFile file;
	if(file.PreLoad(name))
	{
		ActorDlg dlg;
		dlg.m_pFile=&file;
		set<tstring> tracks;
		file.GetTrackSet(tracks);
		for(set<tstring>::const_iterator iter=tracks.begin();iter!=tracks.end();++iter)
		{
			dlg.AddTrackName(iter->c_str());
		}
		dlg.Create();
	}

	m_szActionName=_T("");
	if(!suppressPrompts)
		DialogBoxParam(hInstance, 
				MAKEINTRESOURCE(IDD_PANEL), 
				GetActiveWindow(), 
				ActorKeyFrameExportOptionsDlgProc, (LPARAM)this);

	if(m_szActionName.length()<=0)
		return 0;

	g_msgBox.Create(hInstance,IDD_EXPORTINFO,IDC_OUT);

	
	//--mtl option
	F3DExpMtl::s_bExportBox=false;
	F3DExpMtl::s_pMsgBox=&g_msgBox;
	ExpMtlOptionDlg dlg;
	if ( dlg.Run(name) < 0 )
		return TRUE;

	if (F3DExpMtl::s_bExportLMUV && !F3DExpMtl::s_bExportBUUV)
		file.SetVertType(EVType_PNT2);
	else if (!F3DExpMtl::s_bExportLMUV && F3DExpMtl::s_bExportBUUV)
		file.SetVertType(EVType_PNGT);

	//--
	SceneContext::Inst()->ClearAll();
	
	g_msgBox.ShowMsg(_T("building nodes list...\r\n"));
	SceneContext::Inst()->BuildNodeList(ei->theScene);

	//--
	g_msgBox.ShowMsg(_T("writing file ...\r\n"));

	Interval animRange = i->GetAnimRange();
	TimeValue start,end;
	start=animRange.Start();
	end=animRange.End();

	INode *pNode=SceneContext::Inst()->GetNode( F3DExpMtl::s_szOriginNode.c_str() );
	DWORD actionName=_MakeFourCCA(m_szActionName);
	file.MergeActionFromSceneContext(actionName, pNode, start, end, m_sampleRate);
	file.SetExportFrame(m_bIsFirst);
	bool ok=false;
	ok=file.Write(name,pNode);

	if(ok)
		g_msgBox.ShowMsg(_T("\r\n写入成功...\r\n"));
	else
		g_msgBox.ShowMsg(_T("\r\n写入失败...\r\n"));
	
	g_msgBox.ShowMsg(_T("已有track:\r\n"));	
	set<tstring> tracks;
	file.GetTrackSet(tracks);
	for(set<tstring>::iterator iter=tracks.begin();iter!=tracks.end();++iter)
	{
		const tstring& str=(*iter);
		g_msgBox.ShowMsg(_T("%s\r\n"),str.c_str());
	}

	//--free memory
	SceneContext::Inst()->ClearAll();

	return ok;
}