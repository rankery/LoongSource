// OnlineSum.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "GMServerMgr.h"

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "CHS");

	vEngine::InitNetwork();
	vEngine::InitEngine();

	if ( g_GMServerMgr.Init() ) 
	{
		g_GMServerMgr.Update();
	}

	g_GMServerMgr.Destroy();

	return true;

}
