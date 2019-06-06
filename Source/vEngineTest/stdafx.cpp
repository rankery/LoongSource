// stdafx.cpp : 只包括标准包含文件的源文件
// xengine_test.pch 将成为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"
#include "vEngineTest.h"

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------
INT APIENTRY _tWinMain(HINSTANCE hInst, HINSTANCE, LPTSTR, INT)
{
	vEngine::InitNetwork();
	vEngine::InitEngine();
	vEngine::InitDump(_T("58.211.75.101"),_T("anyone"),_T("anyone"));
	_set_se_translator( vEngine::SEH_Translation ); 
	
	try
	{
		LPCTSTR szText = _T("Hello World");
		CTest* pTest = new CTest;
		if( pTest->Init(hInst) )
			pTest->Mainloop(szText);
		pTest->Destroy();
		delete(pTest);
	} 
	catch(vEngine::Exception)
	{
#ifdef _DEBUG
		throw;
#endif
	}

	vEngine::DestroyEngine();
	vEngine::DestroyNetwork();
	return 0;
}


