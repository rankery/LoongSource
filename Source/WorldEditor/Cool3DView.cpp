#include "StdAfx.h"
#include ".\Cool3Dview.h"
#include "WorldEditorMgr.h"
#include "PluginMgr.h"

Cool3DView::Cool3DView(void)
{
	m_bCreated=false;
	m_pSG=NULL;
}

Cool3DView::~Cool3DView(void)
{

}

Cool3DView g_Cool3DView;
Cool3DView* Cool3DView::Inst()
{
	return &g_Cool3DView;
}

void Cool3DView::InitEngine(HWND hDrawWnd)
{
	if(m_bCreated)
		return;

	HDC hDC=::GetDC(hDrawWnd);
	int screenColorBits=GetDeviceCaps(hDC,BITSPIXEL);
	ReleaseDC(hDrawWnd,hDC);

	ResMgr::Inst()->EnableIOAsync(true);

	//--Create Device
	tagDeviceCfg cfg;
	cfg.bWindowed=true;
	cfg.depthFormat=EDF_D24S8;
	cfg.hWnd=hDrawWnd;
	cfg.nWidth=800;
	cfg.nHeight=600;
	if(screenColorBits==32)
		cfg.pixelFormat=EPF_X8R8G8B8;
	else
		cfg.pixelFormat=EPF_R5G6B5;

	Cool3DOption::Inst()->SetEditorMode(true);
	Cool3DOption::Inst()->SetOption(Cool3DOption::EO_TerrainDetail,Cool3DOption::ETD_Editor);
	Cool3DOption::Inst()->SetOption(Cool3DOption::EO_DebugInfo,Cool3DOption::EDI_Open);

	tagEngineCfg engineCfg;
	_tcscpy(engineCfg.renderSysClass,_T("RenderSysFixed"));
	engineCfg.renderablePoolSize=40*1024;
	engineCfg.texPoolSize=128*1024;
	Kernel::Inst()->CreateEngine(&engineCfg);
	Kernel::Inst()->CreateRenderDevice(&cfg);
	Device()->SetClearParam(true,true,true,0xFF000000,1.0f,0);
	Device()->SetRenderState(ERS_NormalizeNormals,true);	//编辑器中模型放大缩小后需要重新归一化法线

	m_camera.Set(Vector3(64*50,0,64*50),//look at
		//DegToRad(-52.65f),	//yaw
		0,
		-0.53f,	//pitch
		0,4000.0f);
	//m_camera.Set(Vector3(0,0,0),0,0,0,1000);
	m_camera.SetPerspective((float)Device()->GetClientWidth(),(float)Device()->GetClientHeight(),_DegToRad(45),50,4000.0f*50.0f);

	m_pSG=(SceneGraph*)RTTFactory::Inst()->CreateObj(_T("NullSceneGraph"));
	m_pSG->BindCamera(&m_camera);

	AudioSystem::Inst()->SetFS(Kernel::Inst()->DefaultFS());
	/*SoundManager::Inst()->Create();
	if(!SoundManager::Inst()->IsCreated())
	{
		AfxMessageBox(_T("声音系统初始化失败."));
	}*/

	//--
	NavKernel::Inst()->Init(1024*1024*8);

	//--set flag
	m_bCreated=true;
}

void Cool3DView::Destroy()
{
	m_bCreated=false;

	delete m_pSG;
	m_pSG=NULL;
	Kernel::Inst()->FinalDestroy();

}

void Cool3DView::SetBkColor(DWORD color)
{
	Device()->SetClearParam(true,true,true,color,1.0f,0);
}

void Cool3DView::Render()
{
	Kernel::Inst()->Tick();

	if(!m_bCreated)
		return;
	float deltaTime=Kernel::Inst()->GetDeltaTime();

	ResMgr::Inst()->Update();

	Device()->Clear();
	Device()->Begin();

	m_camera.UpdateViewMatrix();
	Device()->MatLoad(EMS_Prj,	m_camera.GetPrj());
	Device()->MatLoad(EMS_View,	m_camera.GetView());

	//--渲染天空盒
	RenderSky();
	//--编辑器渲染，主要渲染地形、装饰物和河流
	WorldEditorMgr::Inst()->Render();
	PluginMgr::Inst()->Render();
	//--
	m_pSG->Render(deltaTime);

	Device()->End();
	Device()->Present();
}

void Cool3DView::RenderSky( void )
{
	SGSkyNode* pNode=m_pSG->GetSkyNode();
	if( pNode==NULL )
		return;

	IDevice *pDev=Device();

	StateBlock state;
	state.SaveState(ERS_FFogEnable);
	state.SaveState(ERS_FLightEnable);
	state.SaveState(ERS_CullMode);
	state.SaveState(ERS_DepthTestEnable);
	state.SaveState(ERS_DepthWriteEnable);

	pDev->FEnableFog(false);
	pDev->FEnableLight(false);
	pDev->SetCullMode(ECull_None);
	pDev->EnableDepthTest(false);
	pDev->EnableDepthWrite(false);

	pDev->MatPush(EMS_World);
	pDev->MatLoad(EMS_World,&pNode->GetWorldMat());
	pDev->FFlushMatrix(true,false,false,false);

	//--render sky
	for(UINT i=0;i<pNode->GetNumRenderNode();i++)
	{
		RenderNode *pRNode=pNode->GetRenderNode(i);
		pRNode->Draw(Kernel::Inst()->GetRenderSys(),pNode,0,NULL,m_pSG);
	}

	pDev->MatPop(EMS_World);
	pDev->FFlushMatrix(true,false,false,false);

	state.Apply();
}