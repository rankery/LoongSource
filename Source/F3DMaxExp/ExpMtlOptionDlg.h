#pragma once
#include "F3DMaxExp.h"
#include <string>

using namespace std;

class F3D_MAX_API ExpMtlOptionDlg  
{
public:
	ExpMtlOptionDlg();
	virtual ~ExpMtlOptionDlg();

	int Run(const TCHAR* szExportFileName);

private:
	static int CALLBACK DialogProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
	static void UpdateFullPath(HWND hwndDlg);
	static bool AutoSetOutPathByExportPath(HWND hwndDlg);
private:
	static tstring	m_szExportFilePath;
};

