#include "stdafx.h"
#include "AniTrackDlg.h"
#include "resource.h"

AniTrackDlg::AniTrackDlg()
{

}

AniTrackDlg::~AniTrackDlg()
{

}

int CALLBACK AniTrackDlg::DialogProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	static AniTrackDlg *pDlg = NULL;
	switch(uMsg)
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_Del:
			{
				int sel=SendDlgItemMessage(hwndDlg,IDC_LIST,LB_GETCURSEL,0,0);
				if(sel!=LB_ERR)
				{
					TCHAR szName[256];
					memset(szName,0,sizeof(szName));
					SendDlgItemMessage(hwndDlg,IDC_LIST,LB_GETTEXT,sel,(LPARAM)szName);
					pDlg->DelTrack(szName);
					SendDlgItemMessage(hwndDlg,IDC_LIST,LB_DELETESTRING,sel,0);
					SendDlgItemMessage(hwndDlg,IDC_LIST,LB_SETCURSEL,sel,0);
				}
			}
			break;
		case IDOK:
			EndDialog(hwndDlg, IDOK);
			return TRUE;
		}
		break;
	case WM_INITDIALOG:
		{
			CenterWindow(hwndDlg,GetParent(hwndDlg));
			pDlg = (AniTrackDlg *)lParam;
			vector<tstring>& tracks=pDlg->m_tracks;
			for(int i=0;i<tracks.size();i++)
			{
				SendDlgItemMessage(hwndDlg,IDC_LIST,LB_ADDSTRING,0
					,(LPARAM)tracks[i].c_str());
			}
			if(tracks.size()>0)
				SendDlgItemMessage(hwndDlg,IDC_LIST,LB_SETCURSEL,0,0);
		}
		return TRUE;
	case WM_CLOSE:
		EndDialog(hwndDlg, 0);
		return TRUE;
	}//endof switch
	return 0;
}

extern HANDLE g_hInst;
void AniTrackDlg::Create()
{
	DialogBoxParam((HINSTANCE)g_hInst,MAKEINTRESOURCE(IDD_Ani),GetActiveWindow()
		,DialogProc, (LPARAM)this);
}

void AniTrackDlg::AddTrackName(const TCHAR* szName)
{
	m_tracks.push_back(szName);
}