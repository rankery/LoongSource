#include "stdafx.h"
#include "ClientMgr.h"
#include "Player.h"

//-----------------------------------------------------------------------------
// main entrance
//-----------------------------------------------------------------------------
INT APIENTRY _tWinMain(HINSTANCE hInst, HINSTANCE, LPTSTR, INT)
{
	vEngine::InitNetwork();
	vEngine::InitEngine();

	_set_se_translator( vEngine::SEH_Translation ); 
	//vEngine::Register(TWrap<Player>(),			"Player");
	try
	{
		g_pClientMgr = new ClientMgr;
		if (g_pClientMgr->Init(hInst) == TRUE)
		{
			g_pClientMgr->Update();
		}
		g_pClientMgr->Destroy();
		SAFE_DEL(g_pClientMgr);
	}
	catch (vEngine::Exception)
	{
		throw;
	}
	vEngine::DestroyEngine();
	vEngine::DestroyNetwork();

	return 0;
}