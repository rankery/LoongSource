// stdafx.cpp : 只包括标准包含文件的源文件
// xengine_test.pch 将成为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"
#include "vServerTest.h"

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------
INT APIENTRY _tWinMain(HINSTANCE hInst, HINSTANCE, LPTSTR, INT)
{
	vEngine::InitNetwork();
	vEngine::InitEngine();
	_set_se_translator( vEngine::SEH_Translation ); 

	try
	{
		CServerTest* pTest = new CServerTest;
		if( pTest->Init(hInst) )
			pTest->Mainloop();
		pTest->Destroy();
		delete(pTest);
	} 
	catch(vEngine::Exception)
	{
		throw;
	}

	vEngine::DestroyEngine();
	vEngine::DestroyNetwork();
	return 0;
}


