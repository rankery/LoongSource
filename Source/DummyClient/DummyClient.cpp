// DummyClient.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "DummyClient.h"
#include "ClientApp.h"


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	ConfigFile cfgfile(_T(".\\config\\DummyCfg.txt"));

	int nWidth=cfgfile.GetInt(_T("width"));
	int nHeight=cfgfile.GetInt(_T("height"));

	ClientApp app;
	app.Create(nWidth,nHeight,_T("Dummy Client"),false);
	::ShowWindow(app.GetHWnd(),SW_SHOW);
	g_hWnd = app.GetHWnd();
	

#ifdef _DEBUG
	app.MainLoop();
#else
	try
	{
		app.MainLoop();
	}
	catch(Cool3D::Exception e)
	{
		MessageBox(NULL,_T("编辑器发生异常，请将log文件发给程序人员!"),_T("异常"),MB_ICONERROR);
		exit(0);
	}
#endif
}