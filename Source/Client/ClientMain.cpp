//-----------------------------------------------------------------------------
//!\file ClientMain.cpp
//!\author Lyp
//!
//!\date 2008-03-27
//! last 2008-03-27
//!
//!\brief 龙客户端
//!
//!	Copyright (c) 2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "ClientMain.h"
#include "shellapi.h"

#include "ServerSelectFrame.h"
#include "LoginFrame.h"
#include "HealthyControlFrame.h"
#include "SelectRoleFrame.h"
#include "CreateRoleFrame.h"
#include "WorldFrame.h"
#include "LoadingFrame.h"
#include "Render2D.h"
#include  "NetSession.h"

#include "CreatureData.h"
#include "PetProtoData.h"
#include "SceneEffectData.h"
#include "ItemProtoData.h"
#include "SkillProtoData.h"
#include "MapLogicData.h"
#include "ShopProtoData.h"
#include "MallData.h"
#include "LonghunAttData.h"
#include "RoleTitleProtoData.h"
#include "RoleInfoProtoData.h"


#include "GameSet.h"
#include "SysMenuFrame.h"
#include "SysOptionMenuFrame.h"
#include "ExitPromptFrame.h"
#include "GameSettingsFrame.h"
#include "MiniMapStatic.h"
#include "MiniMapFrame.h"
#include "ActiveStatic.h"
#include "ActiveProgress.h"
#include "PlayerStateFrame.h"
#include "TargetStateFrame.h"
#include "TargetSpellStateFrame.h"
#include "TeammatesStateFrame.h"
#include "PlayerViewWindow.h"
#include "PetShowWindow.h"
#include "RoleHeadPicIcon.h"
#include "ZoneMapData.h"
#include "ZoneMapStatic.h"
#include "ZoneMapFrame.h"
#include "TeamSys.h"
#include "TeammatesStateFrame.h"
#include "QuitFrame.h"
#include "StyleActionFrame.h"

#include "FestivalActivityPromptFrame.h"
#include "RoleLevelUpPromptFrame.h"
#include "LoongBenedictionFrame.h"

#include "ChatFrame.h"
#include "MilleSoundFrame.h"
#include "StaticForCast.h"
#include "EmotionFrame.h"
#include "RichEditBoxForChatInput.h"
#include "RichEditBoxForChatShow.h"
#include "ScrollRichEditBox.h"
#include "ShortcutBarFrame.h"
#include "ShortcutButton.h"
#include "WorldPickerFrame.h"
#include "CombatSys.h"

#include "ItemFrame.h"
#include "PocketFrame.h"
#include "PetBagFrame.h"
#include "PetInformationFrame.h"
#include "PetTradeFrame.h"
#include "PetTradeUIFrame.h"
#include "ThroneFrame.h"
#include "StorageFrame.h"
#include "ToolTipFrame.h"
#include "CharacterFrame.h"
#include "TargetCharacterFrame.h"
#include "StateFrame.h"
#include "EquipmentFrame.h"
#include "ReputeFrame.h"
#include "FameHallFrame.h"
#include "ClanTreasureFrame.h"
#include "RoleTitleFrame.h"
#include "InformationFrame.h"
#include "DispRoleInfoFrame.h"
#include "ItemProduceFrame.h"
#include "EquipConsolidateFrame.h"
#include "DeComposeFrame.h"
#include "ConsolidateData.h"
#include "ProduceData.h"
#include "ComposeStatic.h"
#include "TradeFrame.h"
#include "PlayerTradeFrame.h"
#include "LockStatic.h"
#include "PlayerTryDressWnd.h"
#include "MallFrame.h"
#include "MallGiftFrame.h"
#include "YuanbaoFrame.h"
#include "FilterData.h"
#include "PocketEncryptFrame.h"
#include "TreasureChestFrame.h"
#include "GuildFrame.h"
#include "GuildMemberInfoFrame.h"
#include "GuildMemberPosFrame.h"
#include "GuildAffairFrame.h"
#include "GuildUpgradeFrame.h"
#include "GuildPickUpSilverFrame.h"
#include "GuildContributeSilver.h"
#include "GuildBusinessFrame.h"
#include "GuildBusinessRankFrame.h"
#include "GuildStorageFrame.h"
#include "GuildStorePermitFrame.h"

#include "NoticeFrame.h"
#include "GroundItemTipsFrame.h"
#include "InsuranceFrame.h"
#include "GroupBuyFrame.h"
#include "StallVipFrame.h"
#include "SFXData.h"
#include "PlayerActionData.h"

#include "TalentFrame.h"
#include "TalentTreeFrame.h"
#include "SkillBookFrame.h"

#include "QuestMgr.h"
#include "AppliedTree.h"
#include "..\Cool3D\Util\FileUtil.h"

#include "SocialMgr.h"

#include "HttpPicEx.h"
#include "StaticEx.h"
#include "TreeEx.h"
#include "ProgressEx.h"
#include "DungeonSys.h"
#include "QuestTargetWnd.h"

#include "ButtonEx.h"
#include "DragBtn.h"
#include "ItemButton.h"
#include "ItemMenuButton.h"
#include "SkillButton.h"
#include "PetSkillButton.h"
#include "TalentButton.h"
#include "ShopShelfFrame.h"
#include "StallShelfFrame.h"
#include "StallVendFrame.h"
#include "StallSignboardTipsFrame.h"
#include "RichEditWithZ.h"
#include "ChatShowPop.h"
#include "ToolTipStatic.h"
#include "RoleFloatTipsFrame.h"
#include "SkillProgressFrame.h"
#include "IconStatic.h"
#include "SceneCenterInfoFrame.h"
#include "RoleHeadTipsFrame.h"
#include "DakFrame.h"
#include "DakData.h"
#include "LevelUpData.h"
#include "PersonalityActData.h"
#include "StaticNoInput.h"
#include "CountDownFrame.h"
#include "CursorMgr.h"
#include "AudioSys.h"
#include "ItemMgr.h"
#include "EffectMgr.h"
#include "InstanceQuitFrame.h"
#include "ActivityPvPFrame.h"
#include "RemoteRoleStateFrame.h"
#include "ChatChannelSetFrame.h"
#include "MouseIntoTipsFrame.h"
#include "EquipRecencyFrame.h"
#include "SpecialSkillFrame.h"

#include "PetFrame.h"
#include "PetSkillFrame.h"

#include "OnlineTips.h"

#include "GameInputFrame.h"
#include "..\WorldDefine\msg_gm_cmd.h"
#include "..\WorldDefine\svn_revision.h"
#include "ServerTime.h"
#include "OwnerImeMgr.h"
#include "OwnerImeFrame.h"
#include "LocalPlayer.h"
#include "RoleMgr.h"

#include "GmConsole.h"
#include "PerfLogger.h"
#include "SeleRoleMgr.h"

#include "HelpFrame.h"
#include "QuizFrame.h"
#include "ProfessionFrame.h"
#include "GuildBusinessStateWnd.h"
#include "MonsterSayTable.h"
#include "MapMgr.h"
#include "MarriageFrame.h"
#include "DivorceFrame.h"
#include "RenameFrame.h"
#include "IconStaticHL.h"
#include "GemRemovalFrame.h"
#include "GlintPushButton.h"

extern VOID LuaOpenCommonLib(lua_State*  l);

//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL Client::Init(HINSTANCE hInst)
{
	m_pUtil->Randomize(timeGetTime()*1000);
	CreateObj("GameConfig", "VarContainer");

	CreateObj("VFS_System", "VirtualFileSys");
	CreateObj("VFS_Animation", "VirtualFileSys");
	CreateObj("VFS_Model", "VirtualFileSys");
	CreateObj("VFS_Map", "VirtualFileSys");

	TObjRef<VirtualFileSys>("VFS_System")->SetWorkingPackage(_T("data\\system.cpk"));
	TObjRef<VirtualFileSys>("VFS_Animation")->SetWorkingPackage(_T("data\\system.cpk"));
	TObjRef<VirtualFileSys>("VFS_Model")->SetWorkingPackage(_T("data\\system.cpk"));
	TObjRef<Log>()->Create(NULL);

	tstring strPath = g_strLocalPath + _T("\\string_table.xml");
	g_StrTable.Init("VFS_System", strPath.c_str());


	IFS* pIFS = (IFS*)(VOID*)TObjRef<VirtualFileSys>("VFS_System");
	ResMgr::Inst()->SetFS(ResMgr::EFST_System, pIFS);
	NavResMgr::Inst()->SetFS(NavResMgr::EFST_System, pIFS);

	pIFS = (IFS*)(VOID*)TObjRef<VirtualFileSys>("VFS_Animation");
	ResMgr::Inst()->SetFS(ResMgr::EFST_Animation, pIFS);

	pIFS = (IFS*)(VOID*)TObjRef<VirtualFileSys>("VFS_Model");
	ResMgr::Inst()->SetFS(ResMgr::EFST_Model, pIFS);
	NavResMgr::Inst()->SetFS(NavResMgr::EFST_Model, pIFS);

	pIFS = (IFS*)(VOID*)TObjRef<VirtualFileSys>("VFS_System");
	ResMgr::Inst()->SetFS(ResMgr::EFST_Effect, pIFS);

	pIFS = (IFS*)(VOID*)TObjRef<VirtualFileSys>("VFS_Map");
	ResMgr::Inst()->SetFS(ResMgr::EFST_Map, pIFS);
	NavResMgr::Inst()->SetFS(NavResMgr::EFST_Map, pIFS);

	// 初始化渲染系统
	this->InitRenderSystem(hInst);

	//--初始化NavMap
	const int navPoolBytes=8*1024*1024;
	NavKernel::Inst()->Init(navPoolBytes);

	pIFS = (IFS*)(VOID*)TObjRef<VirtualFileSys>("VFS_System");
	Cool3D::AudioSystem::Inst()->SetFS(pIFS);

	//--引擎资源管理器打开多线程读盘
	ResMgr::Inst()->EnableIOAsync(true);
	NavResMgr::Inst()->EnableIOAsync(true);

	// 设置程序默认路径
	vEngine::SetDefaultDir();
	m_pAudioSys->Init();

	m_pInput->Init();	// 初始化输入系统
	m_pEditor->Init(m_pGUI, TRUE);// 初始化界面编辑器
	m_pConsole->Init("VFS_System", 500, FALSE); // 初始化控制台
	m_pGameScriptMgr->Init();	// 初始化脚本系统
	m_pSession->Init();	// 初始化网络层
	m_pFrameMgr->Init(); // 初始化程序框架

	this->RegisterControl();	// 注册所有界面控件
	this->RegisterFrame();		// 注册所有框架

	// 启动选择服务器模块
	m_pFrameMgr->CreateFrame(_T("Root"), _T("ServerSelect"), _T("ServerSelectFrame"), 0);

	// 启动资源读取线程
	m_bTerminateThreadLoad = FALSE;
	m_hThreadLoad = (HANDLE)_beginthreadex(NULL, 0, (THREADPROC)m_Trunk.sfp1(&Client::ThreadLoad), NULL, 0, NULL);
	if( 0 == m_hThreadLoad || -1 == (INT)m_hThreadLoad )
	{
		ERR(_T("Create Load thread failed"));
		return FALSE;
	}

	CursorMgr::Inst()->Init(hInst);
	

	if( GameSet::Inst()->Get(EGS_Console) )
	{
		m_pInputMap->Register(0, g_StrTable[_T("Hotkey_Console")], (INPUTMAPEVENTHANDLE)
			m_pConsole->GetTrunk().sfp2(&vEngine::Console::Switch), DIK_F11, TRUE, TRUE, 1);
		m_pInputMap->Register(0, g_StrTable[_T("Hotkey_UIEditor")], (INPUTMAPEVENTHANDLE)
			m_pEditor->GetTrunk().sfp2(&vEngine::GUIEditor::Switch), DIK_F12, TRUE, TRUE, 1);
	}

	if( 17173 == GameSet::Inst()->Get(EGS_GmConsle) )
	{
		// 创建GM console框架
		m_pFrameMgr->CreateFrame(_T("Root"), _T("GmConsole"), _T("GmConsoleFrame"), 0);
	}

	m_pInputMap->Register(0, g_StrTable[_T("Hotkey_ScreenShot0")], (INPUTMAPEVENTHANDLE)
		m_Trunk.sfp2(&Client::ScreenShot), DIK_SCROLL, TRUE, TRUE, 1);	// BMP格式截图
	m_pInputMap->Register(1, g_StrTable[_T("Hotkey_ScreenShot1")], (INPUTMAPEVENTHANDLE)
		m_Trunk.sfp2(&Client::ScreenShot), DIK_SYSRQ, TRUE, TRUE, 1);	// JPG格式截图
	
	// todo 临时代码 F10显示/隐藏当前所有GUIWnd，同时摄像机进入fly模式
	m_pInputMap->Register(0, g_StrTable[_T("Hotkey_UI")], (INPUTMAPEVENTHANDLE)
		m_Trunk.sfp2(&Client::SwitchGUIWnd), DIK_F10, TRUE, TRUE, 1);

	// 注册控制台命令
	m_pConsole->Register(_T("gm"),	m_Trunk.sfp1(&Client::GMCommand), _T("gm command"), 1);
	m_pConsole->Register(_T("res"), m_Trunk.sfp1(&Client::LogResPool), _T("log res"), 1);
	m_pConsole->Register(_T("lrole"), m_Trunk.sfp1(&Client::DisplayLocalRoleAtt), _T("lrole att"), 1);
	m_pConsole->Register(_T("headtips"), m_Trunk.sfp1(&Client::HeadTipsCommand), _T("headtips command"), 1);

	// 初始化性能LOG生成器
	PerfLogger::Inst()->Init();

	// 创建tips框架
	m_pFrameMgr->CreateFrame(_T("Root"), _T("Tooltip"), _T("ToolTipFrame"), 0);

	// 创建自定义输入法
	if( OwnerImeMgr::Inst()->IfOwnerIme() )
		m_pFrameMgr->CreateFrame(_T("Root"), _T("OwnerIme"), _T("OwnerImeFrame"), 0);

    // 建立名贴控件默认使用目录
    ::CreateDirectory(_T("temp"), NULL);

	return TRUE;
}



//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
VOID Client::Destroy()
{
    // 清空temp文件夹
    DeleteDirectory(_T("temp"));

	// 游戏结束后打开网页
	TCHAR szURL[MAX_PATH]={0};
	::GetPrivateProfileString(_T("Launcher"), _T("EndURL"), NULL,  szURL, MAX_PATH, 
		_T("launcher\\launcher.ini"));
	tstring strURL = szURL;
	m_pUtil->CutSpaceOfStringHeadAndTail(strURL);
	if( !strURL.empty() )
		ShellExecute(NULL, _T("open"), _T("IEXPLORE"), szURL, NULL, SW_MAXIMIZE);

	m_pFrameMgr->Destroy();
	m_pWindow->Destroy();
	m_pEditor->Destroy();
	m_pConsole->Destroy();
	m_pGUI->Destroy();

	SeleRoleMgr::Inst()->Destroy();
	CursorMgr::Inst()->Destroy();
	OwnerImeMgr::Inst()->Destroy();
	m_pAudioSys->Destroy();
	ServerTime::Inst()->Destroy();

	g_StrTable.Destroy();

	KillObj("GUIRender");
	Kernel::Inst()->FinalDestroy();

	NavKernel::Inst()->Destroy();
}

//-----------------------------------------------------------------------------
// Mainloop
//-----------------------------------------------------------------------------
VOID Client::Mainloop()
{
	DWORD dwMsg=0, dwParam1=0, dwParam2=0;
	while( FALSE == m_pWindow->MsgLoop() )
	{
		// update
		Cool3D::Kernel::Inst()->Tick();
		WorldBase::NavKernel::Inst()->Tick();
		Cool3D::IDevice* pDev=Cool3D::Device();

		// 监视
		m_pConsole->Watch(_T("FPS"), (DWORD)pDev->GetRenderStatus()->FPS);
		m_pConsole->Watch(_T("Client Revision"), CLIENT_BUILD_REVISION);
		m_pConsole->Watch(_T("vEngine Revision"), VENGINE_BUILD_REVISION);
		m_pConsole->Watch(_T("Cool3D Revision"), COOL3D_BUILD_REVISION);
		m_pConsole->Watch(_T("WorldBase Revision"), WORLDBASE_BUILD_REVISION);
		m_pConsole->Watch(_T("WorldDefine Revision"), WORLDDEFINE_BUILD_REVISION);
		m_pConsole->Watch(_T("Primative"), pDev->GetRenderStatus()->numPrim);
		m_pConsole->Watch(_T("Draw call"), pDev->GetRenderStatus()->numDrawCall);
		m_pConsole->Watch(_T("Texture switch"), pDev->GetRenderStatus()->numTexSwitch);
		m_pConsole->Watch(_T("VB switch"), pDev->GetRenderStatus()->numVBSwitch);
		m_pConsole->Watch(_T("Texture"), pDev->GetRenderStatus()->texSize);
		m_pConsole->Watch(_T("Render target"), pDev->GetRenderStatus()->renderTexSize);
		m_pConsole->Watch(_T("UI Render target"), m_pGUIRender->GetRTSize());
		m_pConsole->Watch(_T("Static VB"), pDev->GetRenderStatus()->staticVBSize);
		m_pConsole->Watch(_T("Dynamic VB"), pDev->GetRenderStatus()->dynamicVBSize);
		m_pConsole->Watch(_T("Static IB"), pDev->GetRenderStatus()->staticIBSize);
		m_pConsole->Watch(_T("Dynamic IB"), pDev->GetRenderStatus()->dynamicIBSize);
		m_pConsole->Watch(_T("UI controls"), m_pGUI->GetControlNum());
		m_pConsole->Watch(_T("Connected"), m_pSession->IsConnect());
		m_pConsole->Watch(_T("EffectNum"), EffectMgr::Inst()->GetNumEffect());

		// 更新性能LOG生成器
		PerfLogger::Inst()->Updata();

		BeginSpeedTest(_T("Client::Mainloop.msgloop"));

		while( m_pWindow->PeekWindowsMsg(dwMsg, dwParam1, dwParam2) )
		{

			if( FALSE == m_pGUI->OnWindowsMsg(dwMsg, dwParam1, dwParam2) )
			{
				// 回车,切换输入法,将win32消息投递到聊天系统
				if( dwMsg==WM_INPUTLANGCHANGE || (dwMsg==WM_KEYDOWN && dwParam1==VK_RETURN)  || dwMsg==WM_KEYDOWN && dwParam1==VK_PRIOR)
				{
					ChatFrame* pFrame = (ChatFrame*)m_pFrameMgr->GetFrame(_T("Chat"));
					if( P_VALID(pFrame) )
						pFrame->OnWin32Msg(dwMsg, dwParam1, dwParam2);
				}
			}

			GUIWnd* pWnd = m_pGUI->GetActive();
			if( P_VALID(pWnd) && !pWnd->IsEditable() && dwMsg==WM_IME_COMPOSITION )
			{
				ChatFrame* pFrame = (ChatFrame*)m_pFrameMgr->GetFrame(_T("Chat"));
				if( P_VALID(pFrame) )
				{
					HKL pHkl = pFrame->GetEnglishHKL();
					if( pHkl )
						ActivateKeyboardLayout( pHkl, KLF_SETFORPROCESS );
				}
				continue;
			}

			//全真屏输入法
			OwnerImeMgr::Inst()->OnWindowsMsg(dwMsg, dwParam1, dwParam2);
			
			CursorMgr::Inst()->OnWindowsMsg(dwMsg, dwParam1, dwParam2);

			if( dwMsg == WM_QUIT )
				return;
		}
		EndSpeedTest(_T("Client::Mainloop.msgloop"));

		BeginSpeedTest(_T("Client::Mainloop.update"));
		m_pInput->Update();
		m_pInputMap->Update();
		m_pGUI->Update();
		m_pLuaMgr->Update(Kernel::Inst()->GetDeltaTime());
		BeginSpeedTest(_T("Client::Mainloop.update.gameframe"));
		m_pFrameMgr->Update();
		EndSpeedTest(_T("Client::Mainloop.update.gameframe"));
		m_pGameScriptMgr->Update(Kernel::Inst()->GetDeltaTime());
		m_pSession->Update();
		EndSpeedTest(_T("Client::Mainloop.update"));

		m_pAudioSys->Update( Kernel::Inst()->GetDeltaTimeDW() );

		if( FALSE == m_pWindow->IsWindowActive() )
		{
			Sleep(100);
		}

		// render
		pDev->Begin();
		DWORD dwBackGroundColor = 0xff000000;
		SceneGraph* pSG = MapMgr::Inst()->GetSceneGraph();
		if( NULL != pSG )
		{
			const tagDistFog& fog = pSG->GetSceneFog();
			dwBackGroundColor = DWORD(fog.color);
		}
		pDev->Clear(true,true,true,dwBackGroundColor,1.0f,0);

		BeginSpeedTest(_T("Client::Mainloop.Render3D"));
		m_pFrameMgr->Render3D();
		EndSpeedTest(_T("Client::Mainloop.Render3D"));

		BeginSpeedTest(_T("Client::Mainloop.Render2D"));
		Cool3D::IDraw2D::Inst()->BeginDraw(IDraw2D::EBlend_Alpha, false);	// TODO:有些界面不需要Alpha混合

		DWORD dwUIRenderTime = timeGetTime();
		m_pGUI->Render();
		m_pEditor->Render();
		m_pConsole->Watch(_T("UI RenderTime"), timeGetTime() - dwUIRenderTime);

		Cool3D::IDraw2D::Inst()->EndDraw();
		EndSpeedTest(_T("Client::Mainloop.Render2D"));

		DrawSpeedTest(0,200);

		pDev->End();

		// 设备丢失处理,必须在Present之前处理
		if( pDev->GetDeviceResetFlag() )
		{
			m_pGUI->RedrawAllWnd();
			m_pFrameMgr->SendEvent( &tagGameEvent( _T("DeviceResetEvent"), NULL ) );
			pDev->ClearDeviceResetFlag();
		}

		BeginSpeedTest(_T("Client::Mainloop.Present"));
		pDev->Present();
		EndSpeedTest(_T("Client::Mainloop.Present"));



	}
}


//-----------------------------------------------------------------------------
// RegisterControl
//-----------------------------------------------------------------------------
VOID Client::RegisterControl()
{
    m_pGUI->Register(TWrap<HttpPicEx>(), _T("HttpPicEx"), _T("HttpPicEx"));
	m_pGUI->Register(TWrap<StaticEx>(), _T("StaticEx"), _T("StaticEx"));
	m_pGUI->Register(TWrap<TreeEx>(), _T("TreeEx"), _T("TreeEx"));
	m_pGUI->Register(TWrap<ProgressEx>(), _T("ProgressEx"), _T("ProgressEx"));
	m_pGUI->Register(TWrap<ButtonEx>(), _T("ButtonEx"), _T("ButtonEx"));
	m_pGUI->Register(TWrap<DragBtn>(), _T("DragBtn"), _T("DragBtn"));
	m_pGUI->Register(TWrap<ShortcutButton>(), _T("ShortcutButton"), _T("ShortcutButton"));
	m_pGUI->Register(TWrap<RichEditBoxForChatInput>(), _T("RichEditBoxForChatInput"), _T("RichEditBoxForChatInput"));
	m_pGUI->Register(TWrap<RichEditBoxForChatShow>(), _T("RichEditBoxForChatShow"), _T("RichEditBoxForChatShow"));
	m_pGUI->Register(TWrap<ScrollRichEditBox>(), _T("ScrollRichEditBox"), _T("ScrollRichEditBox"));
	m_pGUI->Register(TWrap<ItemButton>(), _T("ItemButton"), _T("ItemButton"));
	m_pGUI->Register(TWrap<ItemMenuButton>(), _T("ItemMenuButton"), _T("ItemMenuButton"));
	m_pGUI->Register(TWrap<SkillButton>(), _T("SkillButton"), _T("SkillButton"));
    m_pGUI->Register(TWrap<PetSkillButton>(), _T("PetSkillButton"), _T("PetSkillButton"));
	m_pGUI->Register(TWrap<TalentButton>(), _T("TalentButton"), _T("TalentButton"));
	m_pGUI->Register(TWrap<MiniMapStatic>(),  _T("MiniMapStatic"), _T("MiniMapStatic"));
	m_pGUI->Register(TWrap<ActiveStatic>(),   _T("ActiveStatic"),  _T("ActiveStatic"));
	m_pGUI->Register(TWrap<ActiveProgress>(),   _T("ActiveProgress"),  _T("ActiveProgress"));
	m_pGUI->Register(TWrap<ComposeStatic>(),   _T("ComposeStatic"),  _T("ComposeStatic"));
	m_pGUI->Register(TWrap<PlayerViewWindow>(),   _T("PlayerViewWindow"),  _T("PlayerViewWindow"));
    m_pGUI->Register(TWrap<PetShowWindow>(),   _T("PetShowWindow"),  _T("PetShowWindow"));
	m_pGUI->Register(TWrap<AppliedTree>(), _T("AppliedTree"), _T("AppliedTree"));
	m_pGUI->Register(TWrap<ToolTipStatic>(), _T("ToolTipStatic"), _T("ToolTipStatic"));
	m_pGUI->Register(TWrap<RichEditWithZ>(),   _T("RichEditWithZ"),  _T("RichEditWithZ"));
	m_pGUI->Register(TWrap<StaticForCast>(),  _T("StaticForCast"), _T("StaticForCast"));
	m_pGUI->Register(TWrap<RoleHeadPicIcon>(),   _T("RoleHeadPicIcon"),  _T("RoleHeadPicIcon"));
	m_pGUI->Register(TWrap<IconStatic>(),   _T("IconStatic"),  _T("IconStatic"));
	m_pGUI->Register(TWrap<StaticNoInput>(),   _T("StaticNoInput"),  _T("StaticNoInput"));
	m_pGUI->Register(TWrap<ZoneMapStatic>(),   _T("ZoneMapStatic"),  _T("ZoneMapStatic"));
	m_pGUI->Register(TWrap<LockStatic>(),  _T("LockStatic"), _T("LockStatic"));
	m_pGUI->Register(TWrap<PlayerTryDressWnd>(),  _T("PlayerTryDressWnd"), _T("PlayerTryDressWnd"));
	m_pGUI->Register(TWrap<GoodsButton>(),  _T("GoodsButton"), _T("GoodsButton"));
	m_pGUI->Register(TWrap<MallButton>(),  _T("MallButton"), _T("MallButton"));
	m_pGUI->Register(TWrap<SpecialSkillButton>(),  _T("SpecialSkillButton"), _T("SpecialSkillButton"));
	m_pGUI->Register(TWrap<GlintPushButton>(), _T("GlintPushButton"), _T("GlintPushButton") );
	m_pGUI->Register(TWrap<IconStaticHL>(), _T("IconStaticHL"), _T("IconStaticHL") );
}



//-----------------------------------------------------------------------------
// RegisterFrame
//-----------------------------------------------------------------------------
VOID Client::RegisterFrame()
{
	m_pFrameMgr->Register(TWrap<LoginFrame>(), _T("LoginFrame"));
	m_pFrameMgr->Register(TWrap<SelectRoleFrame>(), _T("SelectRoleFrame"));
	m_pFrameMgr->Register(TWrap<HealthyControlFrame>(), _T("HealthyControlFrame"));
	m_pFrameMgr->Register(TWrap<WorldFrame>(), _T("WorldFrame"));
	m_pFrameMgr->Register(TWrap<LoadingFrame>(), _T("LoadingFrame"));
	m_pFrameMgr->Register(TWrap<SysMenuFrame>(), _T("SysMenuFrame"));
	m_pFrameMgr->Register(TWrap<SysOptionMenuFrame>(), _T("SysOptionMenuFrame"));
	m_pFrameMgr->Register(TWrap<ExitPromptFrame>(), _T("ExitPromptFrame"));
	m_pFrameMgr->Register(TWrap<GameSettingsFrame>(), _T("GameSettingsFrame"));
	m_pFrameMgr->Register(TWrap<MiniMapFrame>(), _T("MiniMapFrame"));
	m_pFrameMgr->Register(TWrap<PlayerStateFrame>(), _T("PlayerStateFrame"));
	m_pFrameMgr->Register(TWrap<TargetStateFrame>(), _T("TargetStateFrame"));
	m_pFrameMgr->Register(TWrap<TargetSpellStateFrame>(), _T("TargetSpellStateFrame"));
	m_pFrameMgr->Register(TWrap<TeammatesStateFrame>(), _T("TeammatesStateFrame"));
	m_pFrameMgr->Register(TWrap<CreateRoleFrame>(), _T("CreateRoleFrame"));
	m_pFrameMgr->Register(TWrap<ItemFrame>(), _T("ItemFrame"));
	m_pFrameMgr->Register(TWrap<PocketFrame>(), _T("PocketFrame"));
    m_pFrameMgr->Register(TWrap<PetBagFrame>(), _T("PetBagFrame"));
	m_pFrameMgr->Register(TWrap<ThroneFrame>(), _T("ThroneFrame"));
	m_pFrameMgr->Register(TWrap<StorageFrame>(), _T("StorageFrame"));
	m_pFrameMgr->Register(TWrap<PocketEncryptFrame>(), _T("PocketEncryptFrame"));
	m_pFrameMgr->Register(TWrap<TradeFrame>(), _T("TradeFrame"));
	m_pFrameMgr->Register(TWrap<ShopShelfFrame>(), _T("ShopShelfFrame"));
	m_pFrameMgr->Register(TWrap<PlayerTradeFrame>(), _T("PlayerTradeFrame"));
	m_pFrameMgr->Register(TWrap<ToolTipFrame>(), _T("ToolTipFrame"));
	m_pFrameMgr->Register(TWrap<CharacterFrame>(), _T("CharacterFrame"));
    m_pFrameMgr->Register(TWrap<TargetCharacterFrame>(), _T("TargetCharacterFrame"));
	m_pFrameMgr->Register(TWrap<StateFrame>(), _T("StateFrame"));
	m_pFrameMgr->Register(TWrap<EquipmentFrame>(), _T("EquipmentFrame"));
	m_pFrameMgr->Register(TWrap<ReputeFrame>(), _T("ReputeFrame"));
	m_pFrameMgr->Register(TWrap<FameHallFrame>(), _T("FameHallFrame"));
	m_pFrameMgr->Register(TWrap<ClanTreasureFrame>(), _T("ClanTreasureFrame"));
	m_pFrameMgr->Register(TWrap<RoleTitleFrame>(), _T("RoleTitleFrame"));
	m_pFrameMgr->Register(TWrap<InformationFrame>(), _T("InformationFrame"));
	m_pFrameMgr->Register(TWrap<DispRoleInfoFrame>(), _T("DispRoleInfoFrame"));
	m_pFrameMgr->Register(TWrap<ItemProduceFrame>(), _T("ItemProduceFrame"));
	m_pFrameMgr->Register(TWrap<DeComposeFrame>(), _T("DeComposeFrame"));
	m_pFrameMgr->Register(TWrap<EquipConsolidateFrame>(), _T("EquipConsolidateFrame"));
	m_pFrameMgr->Register(TWrap<ChatFrame>(), _T("ChatFrame"));
	m_pFrameMgr->Register(TWrap<MilleSoundFrame>(), _T("MilleSoundFrame"));
	m_pFrameMgr->Register(TWrap<EmotionFrame>(), _T("EmotionFrame"));
	m_pFrameMgr->Register(TWrap<ShortcutBarFrame>(), _T("ShortcutBarFrame"));
	m_pFrameMgr->Register(TWrap<WorldPickerFrame>(), _T("WorldPickerFrame"));
	m_pFrameMgr->Register(TWrap<CombatSys>(), _T("CombatSysFrame"));
	m_pFrameMgr->Register(TWrap<TalentFrame>(), _T("TalentFrame"));
	m_pFrameMgr->Register(TWrap<TalentTreeFrame>(), _T("TalentTreeFrame"));
	m_pFrameMgr->Register(TWrap<SkillBookFrame>(), _T("SkillBookFrame"));
	m_pFrameMgr->Register(TWrap<ServerSelectFrame>(), _T("ServerSelectFrame"));
	m_pFrameMgr->Register(TWrap<RoleFloatTipsFrame>(), _T("RoleFloatTipsFrame"));
	m_pFrameMgr->Register(TWrap<GameInputFrame>(), _T("GameInputFrame"));
	m_pFrameMgr->Register(TWrap<SkillProgressFrame>(), _T("SkillProgressFrame"));
	m_pFrameMgr->Register(TWrap<SceneCenterInfoFrame>(), _T("SceneCenterInfoFrame"));
	m_pFrameMgr->Register(TWrap<RoleHeadTipsFrame>(),   _T("RoleHeadTipsFrame"));
	m_pFrameMgr->Register(TWrap<DakFrame>(),   _T("DakFrame"));
	m_pFrameMgr->Register(TWrap<SocialMgr>(), _T("SocialMgrFrame"));
	m_pFrameMgr->Register(TWrap<ZoneMapFrame>(),   _T("ZoneMapFrame"));
	m_pFrameMgr->Register(TWrap<MallFrame>(),   _T("MallFrame"));
    m_pFrameMgr->Register(TWrap<MallGiftFrame>(),   _T("MallGiftFrame"));
    m_pFrameMgr->Register(TWrap<YuanbaoFrame>(), _T("YuanbaoFrame"));
	m_pFrameMgr->Register(TWrap<TeammatesStateFrame>(), _T("TeammatesState"));
	m_pFrameMgr->Register(TWrap<StallShelfFrame>(), _T("StallShelfFrame"));
	m_pFrameMgr->Register(TWrap<StallVendFrame>(), _T("StallVendFrame"));
	m_pFrameMgr->Register(TWrap<StallSignboardTipsFrame>(), _T("StallSignboardTipsFrame"));
	m_pFrameMgr->Register(TWrap<QuitFrame>(), _T("QuitFrame"));
	m_pFrameMgr->Register(TWrap<InstanceQuitFrame>(), _T("InstanceQuitFrame"));
	m_pFrameMgr->Register(TWrap<DungeonSys>(), _T("DungeonSysFrame"));
	m_pFrameMgr->Register(TWrap<CountDownFrame>(), _T("CountDownFrame"));
    m_pFrameMgr->Register(TWrap<PetFrame>(), _T("PetFrame"));
    m_pFrameMgr->Register(TWrap<PetSkillFrame>(), _T("PetSkillFrame"));
    m_pFrameMgr->Register(TWrap<PetInformationFrame>(), _T("PetInformationFrame"));
    m_pFrameMgr->Register(TWrap<PetTradeFrame>(), _T("PetTradeFrame"));
    m_pFrameMgr->Register(TWrap<PetTradeUIFrame>(), _T("PetTradeUIFrame"));
	m_pFrameMgr->Register(TWrap<TreasureChestFrame>(), _T("TreasureChestFrame"));
	m_pFrameMgr->Register(TWrap<GuildFrame>(), _T("GuildFrame"));
	m_pFrameMgr->Register(TWrap<GuildMemberInfoFrame>(), _T("GuildMemberInfoFrame"));
	m_pFrameMgr->Register(TWrap<GuildMemberPosFrame>(), _T("GuildMemberPosFrame"));
	m_pFrameMgr->Register(TWrap<GuildAffairFrame>(), _T("GuildAffairFrame"));
	m_pFrameMgr->Register(TWrap<GuildUpgradeFrame>(), _T("GuildUpgradeFrame"));
	m_pFrameMgr->Register(TWrap<GuildPickUpSilverFrame>(), _T("GuildPickUpSilverFrame"));
	m_pFrameMgr->Register(TWrap<GuildContributeSilverFrame>(), _T("GuildContributeSilverFrame"));
	m_pFrameMgr->Register(TWrap<GuildBusinessFrame>(), _T("GuildBusinessFrame"));
	m_pFrameMgr->Register(TWrap<GuildBusinessRankFrame>(), _T("GuildBusinessRankFrame"));
	m_pFrameMgr->Register(TWrap<GuildBusinessStateWnd>(), _T("GuildBusinessStateWnd"));
	m_pFrameMgr->Register(TWrap<NoticeFrame>(), _T("NoticeFrame"));
	m_pFrameMgr->Register(TWrap<InsuranceFrame>(), _T("InsuranceFrame"));
	m_pFrameMgr->Register(TWrap<GroupBuyFrame>(), _T("GroupBuyFrame"));
	m_pFrameMgr->Register(TWrap<OnlineTips>(), _T("OnlineTipsFrame"));
	m_pFrameMgr->Register(TWrap<GroundItemTipsFrame>(), _T("GroundItemTipsFrame"));
	m_pFrameMgr->Register(TWrap<GmConsoleFrame>(), _T("GmConsoleFrame"));
	m_pFrameMgr->Register(TWrap<StallVipFrame>(), _T("StallVipFrame"));
	m_pFrameMgr->Register(TWrap<OwnerImeFrame>(), _T("OwnerImeFrame"));
	m_pFrameMgr->Register(TWrap<RemoteRoleStateFrame>(), _T("RemoteRoleStateFrame"));
	m_pFrameMgr->Register(TWrap<ChatChannelSetFrame>(), _T("ChatChannelSetFrame"));	
	m_pFrameMgr->Register(TWrap<MouseIntoTipsFrame>(), _T("MouseIntoTipsFrame"));	
	m_pFrameMgr->Register(TWrap<ActivityPvPFrame>(), _T("ActivityPvPFrame"));
	m_pFrameMgr->Register(TWrap<EquipRecencyFrame>(), _T("EquipRecencyFrame"));
	m_pFrameMgr->Register(TWrap<SpecialSkillFrame>(), _T("SpecialSkillFrame"));
	m_pFrameMgr->Register(TWrap<StyleActionFrame>(), _T("StyleActionFrame"));

    m_pFrameMgr->Register(TWrap<GuildStorageFrame>(), _T("GuildStorageFrame"));
    m_pFrameMgr->Register(TWrap<GuildStorePermitFrame>(), _T("GuildStorePermitFrame"));

	
	m_pFrameMgr->Register(TWrap<HelpFrame>(), _T("HelpFrame"));
	m_pFrameMgr->Register(TWrap<QuizFrame>(), _T("QuizFrame"));

    m_pFrameMgr->Register(TWrap<ProfessionFrame>(), _T("ProfessionFrame"));

	m_pFrameMgr->Register(TWrap<QuestTargetWnd>(), _T("QuestTargetWnd"));
    m_pFrameMgr->Register(TWrap<MarriageFrame>(), _T("MarriageFrame"));
    m_pFrameMgr->Register(TWrap<DivorceFrame>(), _T("DivorceFrame"));
	m_pFrameMgr->Register(TWrap<RenameFrame>(), _T("RenameFrame"));
	m_pFrameMgr->Register(TWrap<GemRemovalFrame>(),_T("GemRemovalFrame") );

	//注册节日活动上线提示
	m_pFrameMgr->Register(TWrap<FestivalActivityPromptFrame>(), _T("FestivalActivityPromptFrame") );
	//角色升级到某个级别的提示，寻路
	m_pFrameMgr->Register(TWrap<RoleLevelUpPromptFrame>(), _T("RoleLevelUpPromptFrame") );
	// 注册神龙赐福模块
	m_pFrameMgr->Register(TWrap<LoongBenedictionFrame>(), _T("LoongBenedictionFrame") );
	QuestMgr::RegisterFrames(m_pFrameMgr);
	CombatSys::RegisterFrames(m_pFrameMgr);
	SocialMgr::RegisterFrames(m_pFrameMgr);

}


//-----------------------------------------------------------------------------
// InitRenderSystem
//-----------------------------------------------------------------------------
BOOL Client::InitRenderSystem(HINSTANCE hInst)
{
	GameSet::Inst()->Init();
	DWORD dwWindow = GameSet::Inst()->Get(EGS_Windowed);
	INT nWidth = GameSet::Inst()->Get(EGS_PelsWidth);
	INT nHeight = GameSet::Inst()->Get(EGS_PelsHeight);

	RECT rc = {0};

	// 窗口标题
	tstringstream stream;
	stream<<g_pAppName<<_T(" ") <<g_pBuildType<<_T(" ")<<g_pVersion;

	Kernel::Inst()->CreateEngine(( Cool3D::ConfigFile*)NULL);

	switch(dwWindow)
	{
	case 0:	// 全屏
		{
			m_pWindow->Init(m_pGUIRender, nWidth, nHeight, FALSE, FALSE);
			m_pWindow->SetInvisibleIme(TRUE);
			OwnerImeMgr::Inst()->Init();
			OwnerImeMgr::Inst()->EnableOwnerIme();
		}
		break;
	case 1:	// 最佳窗口
		m_pWindow->Init(m_pGUIRender, 0, 0, FALSE, TRUE);		
		break;
	case 2: // 标准窗口
		m_pWindow->Init(m_pGUIRender, nWidth, nHeight, TRUE, TRUE);
		break;
	case 3:	// 假全屏
			m_pWindow->Init(m_pGUIRender, 0, 0, FALSE, FALSE);
		break;
	}

	
	m_pWindow->CreateWnd(stream.str().c_str(), hInst);
	HWND hWnd = (HWND)TObjRef<VarContainer>()->GetDword(_T("HWND"));

	// 得到实际客户区大小
	::GetClientRect(hWnd, (LPRECT)&rc);
	nWidth = rc.right - rc.left;
	nHeight = rc.bottom - rc.top;

	tagDeviceCfg cfg;
	cfg.bWindowed = (dwWindow > 0);
	cfg.depthFormat = EDF_D24S8;
	cfg.hWnd = hWnd;
	cfg.nWidth = nWidth;
	cfg.nHeight = nHeight;
	cfg.nFullScreenHZ = 0;
	cfg.bVerticalSync = TRUE == GameSet::Inst()->Get( EGS_VerSynch );
	cfg.multiSampleType = (EMultisampleType)GameSet::Inst()->Get( EGS_MultiSampleType );
	if(dwWindow>0)
	{
		HDC hDC=::GetDC(hWnd);
		int screenColorBits=GetDeviceCaps(hDC,BITSPIXEL);
		ReleaseDC(hWnd,hDC);

		//--自动检测16bit还是32bit窗口模式
		cfg.pixelFormat = (screenColorBits==32) ? EPF_X8R8G8B8 : EPF_R5G6B5;
	}
	else
		cfg.pixelFormat=EPF_X8R8G8B8;

	Kernel::Inst()->CreateRenderDevice(&cfg);
	Kernel::Inst()->GetRenderSys()->EnableFog(true);

	// 清空一下后台缓冲
	Cool3D::IDevice* pDev=Cool3D::Device();
	pDev->Begin();
	pDev->Clear(true,true,true,0xff000000,1.0f,0);
	pDev->End();
	pDev->Present();
	pDev->Begin();
	pDev->Clear(true,true,true,0xff000000,1.0f,0);
	pDev->End();
	pDev->Present();

	GameSet::Inst()->ApplyAll();

	m_pGUIRender->Init();
	m_pGUI->Init(nWidth, nHeight, m_pGUIRender, "VFS_System");

	// 载入字体
	m_pGUI->LoadFont(_T("kaiti.ttf"));
	m_pGUI->LoadFont(_T("lishu.ttf"));
	m_pGUI->LoadFont(_T("hanyi.ttf"));
	m_pGUI->LoadFont(_T("MSYH.TTF"));
	m_pGUI->LoadFont(_T("msyhb.ttf"));

	return TRUE;
}



//-----------------------------------------------------------------------------
// GUIWnd显示开关
//-----------------------------------------------------------------------------
DWORD Client::SwitchGUIWnd( DWORD dwID, BOOL bDown )
{
	if( bDown )
	{
		WorldFrame* pWolrdFrame = GetWorldFrm();
		if( m_pGUI->GetDesktop()->IsInvisible() )
		{
			m_pGUI->GetDesktop()->SetInvisible(false);	// 显示界面
			
			// 摄像机退出fly模式
			if( P_VALID(pWolrdFrame) && 17173 == GameSet::Inst()->Get(EGS_GmConsle) )
				pWolrdFrame->GetCamera()->EnableFlyMode(false);
		}
		else
		{
			m_pGUI->GetDesktop()->SetInvisible(true);	// 隐藏界面

			// 摄像机进入fly模式
			if( P_VALID(pWolrdFrame) && 17173 == GameSet::Inst()->Get(EGS_GmConsle) )
				pWolrdFrame->GetCamera()->EnableFlyMode(true);
		}
	}

	return 0;
}


//-----------------------------------------------------------------------------
// 截图
//-----------------------------------------------------------------------------
DWORD Client::ScreenShot(DWORD dwID, BOOL bDown)
{
// 	INT* p = 0;
// 	*p = 0;

	if( !bDown )
		return 0;

	// 创建目录，防止玩家将此目录删除之后无法保存图片
	CreateDirectory(_T("screenshot"), NULL);
	vEngine::SetDefaultDir();

	// 如果只使用系统时间作为文件名,那么同一秒内抓的文件会被覆盖
	TCHAR szTime[MAX_PATH];
	FILETIME CurrentTime;
	GetSystemTimeAsFileTime(&CurrentTime);
	szTime[0] = _T('\0');
	WORD wDate, wTime;
	if (FileTimeToLocalFileTime(&CurrentTime, &CurrentTime) &&
		FileTimeToDosDateTime(&CurrentTime, &wDate, &wTime))
	{
		// 年月日以及时分秒和timeGetTime
		wsprintf(szTime, _T("[%d-%d-%d %02d%02d%02d]-%d"),
			(wDate / 32) & 15, wDate & 31, (wDate / 512) + 1980,
			(wTime >> 11), (wTime >> 5) & 0x3F, (wTime & 0x1F) * 2, 
			timeGetTime());
	}

	tstringstream streamPath;
	if( dwID == 0 )
		streamPath << (_T("screenshot\\LoongScreenshot")) << szTime << _T(".bmp");
	else
		streamPath << (_T("screenshot\\LoongScreenshot")) << szTime << _T(".jpg");

	if( Device()->ScreenShot(streamPath.str().c_str(), dwID==1) )
	{
		ChatFrame* pChatFrame = (ChatFrame*)(m_pFrameMgr->GetFrame(_T("Chat")));
		if( P_VALID(pChatFrame) )
		{
			tstring strMsg = g_StrTable[_T("ScreenShoot_OK")];
			strMsg += streamPath.str();
			//添加从服务器接受的聊天数据到相应的显示频道
			pChatFrame->PushInfo(strMsg.c_str(), ESCC_System);
		}
	}

	return 0;
}





//-----------------------------------------------------------------------------
// GM命令处理
//-----------------------------------------------------------------------------
DWORD Client::GMCommand(LPCTSTR szCommand)
{
	if( m_pSession->IsConnect() )
	{
		tagNC_GMCommand cmd;
		lstrcpyn(cmd.szCommand, szCommand, MAX_GM_COMMAND_LEN-1);
		m_pSession->Send(&cmd);
		return 1;
	}
	else
	{
		IMSG(_T("Have not connected to Server yet\r\n"));
		return -1;
	}
}

//-----------------------------------------------------------------------------
// 将3D引擎资源池状态写入日志文件
//-----------------------------------------------------------------------------
DWORD Client::LogResPool( LPCTSTR szCommand )
{
	Cool3D::ResMgr::Inst()->Report();
	Cool3D::IDraw2D::Inst()->Report();
	WorldBase::NavResMgr::Inst()->Report();
	return 0;
}

//-----------------------------------------------------------------------------
// 显示本地角色所有属性
//-----------------------------------------------------------------------------
DWORD Client::DisplayLocalRoleAtt(LPCTSTR szCommand)
{
	LocalPlayer* pLP = RoleMgr::Inst()->GetLocalPlayer();
	if( P_VALID(pLP))
	{
		for( int i=ERA_AttA_Start; i<ERA_End; ++i )
		{
			TCHAR szBuff[X_LONG_NAME] = {0};
			_sntprintf( szBuff, X_LONG_NAME, _T("LocalPlayerAtt_%d"), i );
			TCHAR szShow[X_LONG_NAME] = {0};
			_sntprintf( szShow, X_LONG_NAME, g_StrTable[szBuff], pLP->GetAttribute((ERoleAttribute)i) );
			IMSG(_T("%s\n"), szShow);
		}
	}
	return 0;
}

//-----------------------------------------------------------------------------
// 关闭/开起头顶tips
//-----------------------------------------------------------------------------
DWORD Client::HeadTipsCommand(LPCTSTR szCommand)
{
	RoleHeadTipsFrame* pFrame = (RoleHeadTipsFrame*)m_pFrameMgr->GetFrame(_T("RoleHeadTips"));
	if( P_VALID(pFrame) )
	{
		tstring strTmp = szCommand;
		if( !strTmp.empty() && strTmp.find(_T('0'))!=strTmp.npos )
		{
			pFrame->SetIsShowHeadTips(FALSE);
		}
		else if( !strTmp.empty() && strTmp.find(_T('1'))!=strTmp.npos )		
		{
			pFrame->SetIsShowHeadTips(TRUE);
		}
	}
	return 0;
}

//-----------------------------------------------------------------------------
// 资源读取线程
//-----------------------------------------------------------------------------
UINT Client::ThreadLoad( LPVOID )
{
	DWORD dwTotalTime = timeGetTime();
	
	try
	{
		IMSG(_T("CreatureData Loaded.\r\n"));
		CreatureData::Inst()->LoadFromFile();
		if( m_bTerminateThreadLoad )
			return 0;

		IMSG(_T("PetData Loaded.\r\n"));
		PetProtoData::Inst()->LoadFromFile();
		if( m_bTerminateThreadLoad )
			return 0;

		IMSG(_T("ScnSfxData Loaded.\r\n"));
		SceneEffectData::Inst()->LoadFromFile();
		if( m_bTerminateThreadLoad )
			return 0;

		IMSG(_T("ItemProtoData Loaded.\r\n"));
		ItemProtoData::Inst()->LoadFromFile();
		if( m_bTerminateThreadLoad )
			return 0;

		IMSG(_T("SkillProtoData Loaded.\r\n"));
		SkillProtoData::Inst()->LoadFromFile();
		if( m_bTerminateThreadLoad )
			return 0;

		IMSG(_T("ShopProtoData Loaded.\r\n"));
		ShopProtoData::Inst()->LoadFromFile();
		if( m_bTerminateThreadLoad )
			return 0;

		IMSG(_T("ConsolidateData Loaded.\r\n"));
		ConsolidateData::Inst()->LoadFromFile();
		if( m_bTerminateThreadLoad )
			return 0;

		IMSG(_T("ProduceData Loaded.\r\n"));
		ProduceData::Inst()->LoadFromFile();
		if( m_bTerminateThreadLoad )
			return 0;

		RoleInfoProtoData::inst()->LoadFromFile();
		if (m_bTerminateThreadLoad)
			return 0;

		RoleTitleProtoData::Inst()->LoadFromFile();
		if (m_bTerminateThreadLoad)
			return 0;

		MapLogicData::Inst()->LoadFromFile();
		if( m_bTerminateThreadLoad )
			return 0;

		IMSG(_T("DakData Loaded.\r\n"));
		DakData::Inst()->LoadFromFile();
		if( m_bTerminateThreadLoad )
			return 0;

		IMSG(_T("LevelUpData Loaded.\r\n"));
		LevelUpData::Inst()->LoadFromFile();
		if( m_bTerminateThreadLoad )
			return 0;

		IMSG(_T("PersonalityActData Loaded.\r\n"));
		PersonalityActData::Inst()->LoadFromFile();
		if( m_bTerminateThreadLoad )
			return 0;

		IMSG(_T("ZoneMapData Loaded.\r\n"));
		ZoneMapData::Inst()->LoadFromFile();
		if( m_bTerminateThreadLoad )
			return 0;

		IMSG(_T("QuestMgr Loaded.\r\n"));
		QuestMgr::Inst()->LoadFromFile();
		if( m_bTerminateThreadLoad )
			return 0;

		MallData::Inst()->LoadFromFile();
		if( m_bTerminateThreadLoad )
			return 0;

		FilterData::Inst()->LoadFromFile();
		if( m_bTerminateThreadLoad )
			return 0;

		LonghunAttData::Inst()->LoadFromFile();
		if( m_bTerminateThreadLoad )
			return 0;

		SFXData::Inst()->LoadFromFile();
		if( m_bTerminateThreadLoad )
			return 0;

		PlayerActionData::Inst()->LoadFromFile();
		if( m_bTerminateThreadLoad )
			return 0;
		
		MonsterSayTable::Inst()->Init();
		if( m_bTerminateThreadLoad )
			return 0;
		IMSG(_T("\r\nTotal time = %d\r\n"), timeGetTime()-dwTotalTime);

	} 
	catch(vEngine::Exception)
	{
#ifdef _DEBUG
		throw;	// 方便调试器调试
#endif
	}

	_endthreadex(0);
	return 0;
}


//-----------------------------------------------------------------------------
// 等待资源读取线程完成
//-----------------------------------------------------------------------------
VOID Client::WaitThreadLoad()
{
	WaitForSingleObject(m_hThreadLoad, INFINITE);
}


//-----------------------------------------------------------------------------
// 等待资源读取线程强行结束
//-----------------------------------------------------------------------------
VOID Client::ExitThreadLoad()
{
	m_bTerminateThreadLoad = TRUE;
	WaitForSingleObject(m_hThreadLoad, INFINITE);
}

BOOL Client::DeleteDirectory( LPCTSTR szDirName )
{
    TCHAR cFiles[100] = {0};
    _tcscpy(cFiles, szDirName);
    _tcscat(cFiles, _T("\\*.*"));
    WIN32_FIND_DATA wfd;
    HANDLE hFind = FindFirstFile(cFiles, &wfd);
    TCHAR cTmp[200] = {0};
    if(hFind != INVALID_HANDLE_VALUE)
    {
        BOOL bFind = TRUE;
        while(bFind)
        {
            _tcscpy(cTmp, szDirName);
            _tcscat(cTmp, _T("\\"));
            _tcscat(cTmp, wfd.cFileName);
            SetFileAttributes(cTmp, FILE_ATTRIBUTE_NORMAL);
            DeleteFile(cTmp);
            bFind=FindNextFile(hFind,&wfd);
        }
        FindClose(hFind);
    }

    return TRUE;
}