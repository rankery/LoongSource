#pragma once

#pragma warning(disable:4251)

#include "F3DMaxExp.h"

using namespace std;

class F3D_MAX_API AniTrackDlg  
{
public:
	AniTrackDlg();
	virtual ~AniTrackDlg();

	void Create();
	//add before call create
	void AddTrackName(const TCHAR* szName);
	virtual void DelTrack(const TCHAR* szName) =0;
protected:
	static int CALLBACK DialogProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);

	vector<tstring>	m_tracks;
};

