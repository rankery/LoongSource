#include "StdAfx.h"
#include ".\clientapp.h"
#include "Mapview.h"
#include "BuiltMapView.h"
#include "..\Cool3D\Util\SpeedAnalyser.h"
#include "..\Cool3D\PostProcess\PostProcessBlur.h"
#include "..\Cool3D\Cool3DOption.h"
#include ".\Win32App\GInput.h"
#include "..\Cool3D\RenderSys\WaterReflectMapRender.h"

GConsole g_con;
bool	 ClientApp::m_bShowInfo=true;

ClientApp::ClientApp(void)
{
	m_bWindowActive=true;
	m_pMapView=NULL;
}

ClientApp::~ClientApp(void)
{
}

void ClientApp::OnCreate()
{
	//--
	g_con.RegCommand(_Con_Exit,_T("exit"),_T("退出客户端程序"));
	g_con.RegCommand(_Con_WireFrame,_T("wire"),_T("使用线框模式渲染,参数0或1"));
	g_con.RegCommand(_Con_Fly,_T("fly"),_T("镜头Fly模式,参数0或1"));
	g_con.RegCommand(_Con_LoadMap,_T("lm"),_T("加载Built成功的地图"));
	g_con.RegCommand(_Con_SetEnv,_T("set"),_T("Load场景设置文件(DummyCfg.txt)"));
	g_con.RegCommand(_Con_Goto,_T("goto"),_T("使得角色瞬移到某点"));
	g_con.RegCommand(_Con_ShowRole,_T("role"),_T("隐藏/显示主角"));
	g_con.RegCommand(_Con_SaveCamera,_T("sc"),_T("保存镜头位置"));
	g_con.RegCommand(_Con_LoadCamera,_T("lc"),_T("加载镜头位置"));
	g_con.RegCommand(_Con_ShowInfo,_T("si"),_T("隐藏/显示资源信息"));
	g_con.RegCommand(_Con_MoveSpeed, _T("ms"), _T("角色移动速度"));
	g_con.RegCommand(_Con_JumpY, _T("jy"), _T("角色跳越速度"));
	g_con.RegCommand(_Con_Door, _T("door"), _T("门测试"));

	ConfigFile cfgfile(_T(".\\config\\DummyCfg.txt"));

	//--Create Cool3D Engine
	Cool3D::ResMgr::Inst()->EnableIOAsync(false);

	Cool3DOption::Inst()->SetOption(Cool3DOption::EO_LightMap,cfgfile.GetInt(_T("LightMap")));
	Cool3DOption::Inst()->SetOption(Cool3DOption::EO_TerrainDetail,cfgfile.GetInt(_T("TerrainDetail")));
	Cool3DOption::Inst()->SetOption(Cool3DOption::EO_Highlight,cfgfile.GetInt(_T("Highlight")));
	Cool3DOption::Inst()->SetOption(Cool3DOption::EO_NormalMap,cfgfile.GetInt(_T("NormalMap")));
	Cool3DOption::Inst()->SetOption(Cool3DOption::EO_HWSkin,cfgfile.GetInt(_T("HWSkin")));
	Cool3DOption::Inst()->SetOption(Cool3DOption::EO_GrassAni,cfgfile.GetInt(_T("GrassAni")));
	Cool3DOption::Inst()->SetOption(Cool3DOption::EO_TexDetail,cfgfile.GetInt(_T("TexDetail")));
	Cool3DOption::Inst()->SetOption(Cool3DOption::EO_TerrainSpecular,cfgfile.GetInt(_T("TerrainSpecular")));
	Cool3DOption::Inst()->SetOption(Cool3DOption::EO_WaterReflect,cfgfile.GetInt(_T("WaterReflectEffect")));
	Cool3DOption::Inst()->SetOption(Cool3DOption::EO_WaterRefract,cfgfile.GetInt(_T("WaterRefractEffect")));
	Cool3DOption::Inst()->SetOption(Cool3DOption::EO_WaterSpecular,cfgfile.GetInt(_T("WaterSpecular")));
	Cool3DOption::Inst()->SetOption(Cool3DOption::EO_DebugInfo,cfgfile.GetInt(_T("ShowDebugInfo")));

	Cool3D::Kernel::Inst()->CreateEngine(&cfgfile);

   	bool bWindowed= cfgfile.GetInt(_T("window"))==1;
	HWND hDrawWnd=GWin32App::GetHWnd();
	ASSERT(hDrawWnd!=NULL);

	//--
	tagDeviceCfg cfg;
	cfg.bWindowed=bWindowed;
	cfg.depthFormat=EDF_D24S8;
	cfg.hWnd=hDrawWnd;
	cfg.nWidth=cfgfile.GetInt(_T("width"));
	cfg.nHeight=cfgfile.GetInt(_T("height"));
	cfg.nFullScreenHZ=0;
	cfg.multiSampleType=(EMultisampleType)cfgfile.GetInt(_T("MultiSampleType"));
	if(bWindowed)
	{
		HDC hDC=::GetDC(hDrawWnd);
		int screenColorBits=GetDeviceCaps(hDC,BITSPIXEL);
		ReleaseDC(hDrawWnd,hDC);

		//--自动检测16bit还是32bit窗口模式
		if(screenColorBits==32)
			cfg.pixelFormat=EPF_X8R8G8B8;
		else
			cfg.pixelFormat=EPF_R5G6B5;
	}
	else
		cfg.pixelFormat=EPF_X8R8G8B8;

	Kernel::Inst()->CreateRenderDevice(&cfg);
	Device()->SetClearParam(true,true,true,0xFF000000,1.0f,0);

	//--load default data
	Kernel::Inst()->GetRenderSys()->EnableFog(true);
	m_camera.SetDefault();

	//--水面效果
	int level=cfgfile.GetInt(_T("WaterReflectEffect"));
	if(level>0)
	{
		Cool3D::WaterReflectMapRender::Inst()->Enable(true);
		Cool3D::WaterReflectMapRender::EEffectLevel effectLevel;
		if(level==1)
			effectLevel=Cool3D::WaterReflectMapRender::EL_Low;
		else
			effectLevel=Cool3D::WaterReflectMapRender::EL_High;
		Cool3D::WaterReflectMapRender::Inst()->SetEffectLevel(effectLevel);
	}
	else
	{
		Cool3D::WaterReflectMapRender::Inst()->Enable(false);
	}

	//--柔光
	if(cfgfile.GetBool(_T("Blur"))&&PostProcessBlur::IsHWSupport())
	{
		PostProcessBlur* pBlur=new PostProcessBlur;
		if( pBlur->Init() )
		{
			Cool3D::Kernel::Inst()->GetRenderSys()->AddPostProcess(pBlur);
		}
		else
		{
			SAFE_DELETE( pBlur );
		}
	}

	NavResMgr::Inst()->Init(1000);

	_set_se_translator(Cool3D::Cool_SEH_Translation);

	g_con.Print(_T("Cool3D engine init ok."));
}

void ClientApp::OnDestroy()
{
	SAFE_DELETE(m_pMapView);
	Kernel::Inst()->FinalDestroy();
}

void ClientApp::OnIdle()
{
	//--update
	Kernel::Inst()->Tick();
	GInput::Inst()->UpdateKBInput();
	GInput::Inst()->UpdateMouseInput();

	Vector3 rolePos(500,500,500);
	if(m_pMapView)
	{
		rolePos=m_pMapView->GetRolePos();
		rolePos.y+=160;
	}
	m_camera.Update(rolePos);

	//--render
	if(!m_bWindowActive)
	{
		Sleep(100);
		return;
	}
	IDevice *pDev=Device();

	pDev->Clear(true,true,true);
	if(pDev->Begin())
	{

		//----draw map
		if(m_pMapView && m_pMapView->GetSceneGraph())
		{
			m_pMapView->Update();
			m_pMapView->GetSceneGraph()->Render(Kernel::Inst()->GetDeltaTime());
		}

		//----draw console
		g_con.Draw();

		if( m_bShowInfo )
		{
			SpeedAnalyser::Inst()->Draw(10,200);
			ScreenPrinter::Inst()->Draw(10,40);

			//show Cool3D lasterror
			tstring szCool3DError=_T("Cool3D last error:");
			szCool3DError+=Kernel::Inst()->GetLastError();
			DrawX::DrawDebugString(szCool3DError.c_str(),
				8,Device()->GetClientHeight()-32);
			//show resmgr info
			ResMgr::Inst()->DrawInfo(8,Device()->GetClientHeight()-32-16);
			//show role info
			if(m_pMapView!=NULL)
			{
				POINT rolePT=m_pMapView->World2Tile(m_pMapView->GetRolePos());
				TCHAR szRoleInfo[512];
				_stprintf_s(szRoleInfo,512,_T("role x=%04d,z=%04d,y=%04d"),rolePT.x,rolePT.y,m_pMapView->GetRolePos().y);
				DrawX::DrawDebugString(szRoleInfo,
					8,Device()->GetClientHeight()-32-32);
			}
		}

		pDev->End();
	}
	pDev->Present();
}

DWORD ClientApp::OnWin32Msg(HWND hWnd,UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_ERASEBKGND:
		return 1;

	case WM_SYSCOMMAND:
		if(wParam==SC_MINIMIZE)
			m_bWindowActive=false;
		else
			m_bWindowActive=true;
		break;
	case WM_DESTROY:
		PostMessage(GAPP_QUIT,0,0);
		break;
	case WM_CHAR:
		g_con.OnChar((TCHAR)wParam);
		return 0;
	}

	return GWin32App::OnWin32Msg(hWnd,message,wParam,lParam);
}

bool ClientApp::DoMessage(DWORD msg,DWORD wParam,DWORD lParam)
{
	return false;
}

void ClientApp::DestroyMap()
{
	SAFE_DELETE(m_pMapView);
}

void ClientApp::_Con_ShowRole(const vector<tstring>& agvList)
{
	if(agvList.size() < 1)
	{
		g_con.Print(_T("参数:是否显示（0不显示，1显示）"));
		return;	
	}

	MapView *pMapView=App()->GetMapView();
	if(pMapView==NULL)
	{
		g_con.Print(_T("必须先打开一个地图."));
	}
	else
	{
		int show = _ttoi(agvList[0].c_str());
		pMapView->ShowRole(show != 0);
	}
}

void ClientApp::_Con_Goto(const vector<tstring>& agvList)
{
	if(agvList.size() < 2)
	{
		g_con.Print(_T("参数:tileX,tileZ"));
		return;	
	}

	MapView *pMapView=App()->GetMapView();
	if(pMapView==NULL)
	{
		g_con.Print(_T("必须先打开一个地图."));
	}
	int tileX=_ttoi(agvList[0].c_str());
	int tileZ=_ttoi(agvList[1].c_str());
	pMapView->SetRolePos(tileX,tileZ);
}

void ClientApp::_Con_Exit(const vector<tstring>& agvList)
{
	ClientApp::Inst()->PostMessage(GAPP_QUIT,0,0);
}

void ClientApp::_Con_LoadMap(const vector<tstring>& agvList)
{
	if(agvList.size() <= 0)
	{
		g_con.Print(_T("参数:map name"));
		return;
	}

	ClientApp *pApp=(ClientApp *)ClientApp::Inst();
	//--清除旧的
	pApp->DestroyMap();

	//--先Load参数
	ConfigFile cfg(_T(".\\config\\DummyCfg.txt"));

	//--设置各种ViewZone的Size
	BuiltMapView::ViewDistSN=cfg.GetInt(_T("ViewDistSN"))*50.0f;
	BuiltMapView::ViewDistMN=cfg.GetInt(_T("ViewDistMN"))*50.0f;
	BuiltMapView::ViewDistBN=cfg.GetInt(_T("ViewDistBN"))*50.0f;
	BuiltMapView::ViewDistDN=cfg.GetInt(_T("ViewDistDN"))*50.0f;
	BuiltMapView::ViewDistST=cfg.GetInt(_T("ViewDistST"))*50.0f;

	BuiltMapView::NoMonster=cfg.GetInt(_T("NoMonster"))!=0;
	pApp->m_camera.SetDefault();

	//--load新的
	tstring szMapName = agvList[0];
	BuiltMapView *pNewMap=new BuiltMapView;
	int ok=pNewMap->LoadMap(szMapName.c_str(),&pApp->m_camera);
	pApp->m_pMapView=pNewMap;

	if(ok)
		g_con.Print(_T("map[%s]已加载."),agvList[0].c_str());
	else
		g_con.Print(_T("map[%s]加载失败."),agvList[0].c_str());
	
	pApp->m_camera.SetDefault();

	vector<tstring> tmp;
	_Con_SetEnv(tmp);
}

void ClientApp::_Con_WireFrame(const vector<tstring>& agvList)
{
	bool wire=true;
	if(agvList.size()>0)
	{
		wire=_ttoi(agvList[0].c_str())!=0;
	}
	
	Device()->SetRenderState(ERS_AntialiasedLine,false);
	if(wire)
		Device()->SetFillMode(EFill_Wireframe);
	else
		Device()->SetFillMode(EFill_Solid);
}

void ClientApp::_Con_Fly(const vector<tstring>& agvList)
{
	if(agvList.size()>=1)
	{
		if(_ttoi(agvList[0].c_str()) !=0)
			App()->GetCamera()->EnableFlyMode(true);
		else
			App()->GetCamera()->EnableFlyMode(false);
	}
	else
		App()->GetCamera()->EnableFlyMode(true);
}

void ClientApp::_Con_ShowInfo( const vector<tstring>& agvList )
{
	if(agvList.size()>=1)
	{
		if(_ttoi(agvList[0].c_str()) !=0)
			m_bShowInfo=true;
		else
			m_bShowInfo=false;
	}
	else
		m_bShowInfo=true;
}

void ClientApp::_Con_Door( const vector<tstring>& agvList )
{
	if(agvList.size() != 2)
	{
		g_con.Print(_T("参数:物件ID 开关(0/1)"));
		return;
	}

	DWORD dwObjID=(DWORD)_ttoi(agvList[0].c_str());
	if(_ttoi(agvList[1].c_str())==1)
		App()->GetMapView()->OpenDoor(dwObjID);
	else
		App()->GetMapView()->CloseDoor(dwObjID);
}

Vector3 Str2Vec3(const TCHAR* szStr)
{
	//将["255,255,255"]字符串转换成RGB
	float c[3]={0,0,0};
	if(szStr)
	{
		int len=_tcslen(szStr);
		if(len>0)
		{
			TCHAR *pNewStr=new TCHAR[len+1];
			_tcscpy_s(pNewStr,len+1,szStr);
			const TCHAR seps[]   = _T(" ,");
			TCHAR* next_token = NULL;
			TCHAR *token=_tcstok_s( pNewStr, seps, &next_token );
			if(token==NULL)
			{
				c[0]=(float)_tstof(szStr);
			}
			else
			{
				int i=0;
				while(token!=NULL
					&& i<3)
				{
					c[i]=(float)_tstof(token);
					i++;
					token = _tcstok_s( NULL, seps, &next_token );
				}
			}//endof else
			delete[] pNewStr;
		}//endof if
	}//endof if
	
	return Vector3(c[0],c[1],c[2]);
}

void ClientApp::_Con_SetEnv(const vector<tstring>& agvList)
{
	SceneGraph *pSG=App()->GetSceneGraph();
	if(pSG==NULL)
	{
		g_con.Print(_T("场景未加载"));
		return;
	}
	ConfigFile cfg(_T(".\\config\\DummyCfg.txt"));

	Cool3D::Cool3DOption::Inst()->SetOption(Cool3DOption::EO_TerrainSpecular,cfg.GetInt(_T("TerrainSpecular")));
}

void ClientApp::_Con_SaveCamera(const vector<tstring>& agvList)
{
	TCHAR szBuf[100];
	Vector3 rolePos=App()->GetMapView()->GetRolePos();
	_stprintf_s(szBuf,100,_T("%f,%f,%f"),rolePos.x,rolePos.y,rolePos.z);
	WritePrivateProfileString(_T("camera"),_T("rolepos"),szBuf,_T(".\\config\\DummyData.txt"));
	_stprintf_s(szBuf,100,_T("%f,%f,%f"),App()->GetCamera()->GetYaw(),
			App()->GetCamera()->GetPitch(),
			App()->GetCamera()->GetDist());
	WritePrivateProfileString(_T("camera"),_T("ypd"),szBuf,_T(".\\config\\DummyData.txt"));
}

void ClientApp::_Con_LoadCamera(const vector<tstring>& agvList)
{
	TCHAR szBuf[100];
	GetPrivateProfileString(_T("camera"),_T("rolepos"),_T("0,0,0"),szBuf,sizeof(szBuf),_T(".\\config\\DummyData.txt"));
	Vector3 rolePos=Str2Vec3(szBuf);
	App()->GetMapView()->SetRolePos(rolePos);

	GetPrivateProfileString(_T("camera"),_T("ypd"),_T("0,0,100"),szBuf,sizeof(szBuf),_T(".\\config\\DummyData.txt"));
	Vector3 ypd=Str2Vec3(szBuf);
	App()->GetCamera()->SetYaw(ypd.x);
	App()->GetCamera()->SetPitch(ypd.y);
	App()->GetCamera()->SetDist(ypd.z);
}

void ClientApp::_Con_MoveSpeed(const vector<tstring>& agvList)
{
	if(agvList.size()>=1)
	{
		float fMoveSpeed = (float)_ttoi(agvList[0].c_str());
		App()->GetMapView()->SetMoveSpeed(fMoveSpeed);
	}
}

void ClientApp::_Con_JumpY(const vector<tstring>& agvList)
{
	if(agvList.size()>=1)
	{
		float fJumpYSpeed = (float)_ttoi(agvList[0].c_str());
		App()->GetMapView()->SetJumpYSpeed(fJumpYSpeed);
	}
}	