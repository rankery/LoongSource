#pragma once
#include "DummyClient.h"
#include "ClientCamera.h"
#include "MapView.h"
#include ".\Win32App\GWin32App.h"
#include ".\Win32App\GConsole.h"

class MapView;
class ClientApp :
	public GWin32App
{
public:
	ClientApp(void);
	virtual ~ClientApp(void);

	virtual void OnCreate();
	virtual void OnDestroy();
	virtual void OnIdle();
	virtual DWORD OnWin32Msg(HWND hWnd,UINT message, WPARAM wParam, LPARAM lParam);

	void DestroyMap();
	ClientCamera* GetCamera()	{	return &m_camera;}
	SceneGraph* GetSceneGraph()
	{
		if(m_pMapView)
			return m_pMapView->GetSceneGraph();
		return NULL;
	}
	MapView* GetMapView()	{	return m_pMapView;}

	static void _Con_Exit(const vector<tstring>& agvList);
	static void _Con_LoadTrn(const vector<tstring>& agvList);
	static void _Con_LoadMap(const vector<tstring>& agvList);
	static void _Con_WireFrame(const vector<tstring>& agvList);
	static void _Con_Fly(const vector<tstring>& agvList);
	static void _Con_SetEnv(const vector<tstring>& agvList);
	static void _Con_EnableNav(const vector<tstring>& agvList);
	static void _Con_Goto(const vector<tstring>& agvList);
	static void _Con_ShowRole(const vector<tstring>& agvList);
	static void _Con_SaveCamera(const vector<tstring>& agvList);
	static void _Con_LoadCamera(const vector<tstring>& agvList);
	static void _Con_ShowInfo(const vector<tstring>& agvList);
	static void _Con_MoveSpeed(const vector<tstring>& agvList);
	static void _Con_JumpY(const vector<tstring>& agvList);
	static void _Con_Door(const vector<tstring>& agvList);

	void GetSkyCfg(tstring& top,tstring& wrap)
	{
		top=m_szSkyTop;
		wrap=m_szSkyWrap;
	}
	
private:
	virtual bool DoMessage(DWORD msg,DWORD wParam,DWORD lParam);
	bool			m_bWindowActive;
	MapView			*m_pMapView;
	ClientCamera	m_camera;
	static bool		m_bShowInfo;

public:
	static tstring	m_szSkyTop;
	static tstring	m_szSkyWrap;
};

extern GConsole	g_con;

inline ClientApp *App()
{
	return (ClientApp*)GWin32App::Inst();
}