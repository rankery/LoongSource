#include "stdafx.h"
#include "GameSet.h"
#include "GameSettingsFrame.h"
#include "MapMgr.h"
#include "SGDynamicProjector.h"
#include "..\Cool3D\PostProcess\PostProcessBlur.h"
#include "RoleMgr.h"
#include "LocalPlayer.h"

GameSettingsFrame::GameSettingsFrame() :
m_Trunk( this ),
m_pWnd( NULL ),
m_pPanel( NULL ),
m_pBn_Close( NULL ),
m_pBn_GraphicSet( NULL ),
m_pBn_GameSet( NULL ),
m_pPanel_GraphicSet( NULL ),
m_pPanel_GameSet( NULL ),
m_pBn_AutoCheck( NULL ),
m_pBn_OK( NULL ),
m_pBn_Cancel( NULL ),
m_pBn_Apply( NULL ),
m_pComb_WHB( NULL ),
m_pComb_Hz( NULL ),
m_pComb_MultiSample( NULL ),
m_pBn_Windowed_Off( NULL ),
m_pBn_Windowed_On( NULL ),
m_pBn_VerSynch_Off( NULL ),
m_pBn_VerSynch_On( NULL ),
m_pBn_TerrainDetail_Low( NULL ),
m_pBn_TerrainDetail_High( NULL ),
m_pBn_NormalMap_Off( NULL ),
m_pBn_NormalMap_On( NULL ),
m_pBn_HWSkin_Off( NULL ),
m_pBn_HWSkin_On( NULL ),
m_pBn_TexDetail_Low( NULL ),
m_pBn_TexDetail_Mid( NULL ),
m_pBn_TexDetail_High( NULL ),
m_pBn_TerrainSpecular_Off( NULL ),
m_pBn_TerrainSpecular_On( NULL ),
m_pScro_FarView( NULL ),
m_pScro_RoleView( NULL ),
m_pScro_EffectDetail( NULL ),
m_pScro_SceneDetail( NULL ),
m_pBn_WaterSpecular_Off( NULL ),
m_pBn_WaterSpecular_On( NULL ),
m_pBn_WaterReflect_Off( NULL ),
m_pBn_WaterReflect_Mid( NULL ),
m_pBn_WaterReflect_High( NULL ),
m_pBn_WaterRefract_Off( NULL ),
m_pBn_WaterRefract_On( NULL ),
m_pBn_Lambency_Off( NULL ),
m_pBn_Lambency_On( NULL ),
m_pBn_Weather_Off( NULL ),
m_pBn_Weather_On( NULL ),
m_pBn_SunHalo_Off( NULL ),
m_pBn_SunHalo_On( NULL ),
m_pBn_DrawGrass_Off( NULL ),
m_pBn_DrawGrass_On( NULL ),
m_pBn_RoleShadow_Off( NULL ),
m_pBn_RoleShadow_On( NULL ),
m_pBn_Music_Off( NULL ),
m_pBn_Music_On( NULL ),
m_pScro_MusicVolume( NULL ),
m_pBn_Sound_Off( NULL ),
m_pBn_Sound_On( NULL ),
m_pScro_SoundVolume( NULL ),
m_pBn_CombatMusic_Off( NULL ),
m_pBn_CombatMusic_On( NULL ),
m_pBn_AcceptTeamInvite_Off( NULL ),
m_pBn_AcceptTeamInvite_On( NULL ),
m_pBn_AcceptFactionInvite_Off( NULL ),
m_pBn_AcceptFactionInvite_On( NULL ),
m_pBn_AllowFriendAdd_Off( NULL ),
m_pBn_AllowFriendAdd_On( NULL ),
m_pBn_AcceptTradeRequest_Off( NULL ),
m_pBn_AcceptTradeRequest_On( NULL ),
m_pBn_SelfInfoOpenType_Private( NULL ),
m_pBn_SelfInfoOpenType_Friend( NULL ),
m_pBn_SelfInfoOpenType_Public( NULL ),
m_pBn_ShowSelfName( NULL ),
m_pBn_ShowOtherPlayerName( NULL ),
m_pBn_ShowFactionName( NULL ),
m_pBn_ShowChatPop( NULL ),
m_pBn_ShowHat( NULL ),
m_pBn_ShowMantle( NULL ),
m_pBn_ShowOtherPalyeTitle( NULL ),
m_pBn_ShowSkillNameFloatTip( NULL ),
m_pBn_ShowHPMPText( NULL ),
m_pBn_ShowMonsterName( NULL ),
m_nSelectWHB( 0 ),
m_nSelectHz( 0 ),
m_nSelectMultiSample( 0 ),
m_dwLastApplyTime( 0 )
{
}

GameSettingsFrame::~GameSettingsFrame()
{
}


//-----------------------------------------------------------------------------
// Init
//-----------------------------------------------------------------------------
BOOL GameSettingsFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	return TRUE;
}

//-----------------------------------------------------------------------------
// Destroy
//-----------------------------------------------------------------------------
BOOL GameSettingsFrame::Destroy()
{
	GameFrame::Destroy();

	SafeDestroyUI();

	return TRUE;
}


//-----------------------------------------------------------------------------
// ReloadUI
//-----------------------------------------------------------------------------
BOOL GameSettingsFrame::ReloadUI()
{
	SafeDestroyUI();

	// 创建
	tstring strPath = g_strLocalPath + _T("\\ui\\systemset.xml");
	m_pWnd = m_pGUI->CreateFromFile( "VFS_System", strPath.c_str() );        //主窗口                            

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&GameSettingsFrame::GUIEventHandler));

	m_pPanel = m_pWnd->GetChild( _T("pic") );                                 //底版
	m_pBn_Close      = (GUIButton*)m_pPanel->GetChild( _T("closebutt") );     //关闭按钮

	m_pBn_GraphicSet = (GUIPushButton*)m_pPanel->GetChild( _T("choose") )->GetChild( _T("psetbut") );      //画面配置按钮
	m_pBn_GameSet    = (GUIPushButton*)m_pPanel->GetChild( _T("choose") )->GetChild( _T("gsetbut") );      //游戏配置按钮
		
	m_pPanel_GraphicSet = m_pPanel->GetChild( _T("picset") );                 //画面配置面板
	m_pPanel_GameSet = m_pPanel->GetChild( _T("gameset") );                   //游戏配置面板

	m_pBn_AutoCheck = (GUIButton*)m_pPanel->GetChild( _T("autotest") );       //自动检测按钮
	m_pBn_OK        = (GUIButton*)m_pPanel->GetChild( _T("ok") );             //确定按钮
	m_pBn_Cancel    = (GUIButton*)m_pPanel->GetChild( _T("cacel") );          //取消按钮
	m_pBn_Apply     = (GUIButton*)m_pPanel->GetChild( _T("apply") );          //应用按钮

	//画面配置中需要用到的控件
	m_pComb_WHB                = (GUIComboBox*  )m_pPanel_GraphicSet->GetChild( _T("reveal\\new1") );                        //分辨率框
	m_pComb_Hz                 = (GUIComboBox*  )m_pPanel_GraphicSet->GetChild( _T("reveal\\new2") );                        //刷新率框
	m_pComb_MultiSample        = (GUIComboBox*  )m_pPanel_GraphicSet->GetChild( _T("reveal\\new3") );                        //抗锯齿框
	m_pBn_Windowed_Off         = (GUIPushButton*)m_pPanel_GraphicSet->GetChild( _T("reveal\\word2\\choose1\\off1") );        //窗口模式关
	m_pBn_Windowed_On          = (GUIPushButton*)m_pPanel_GraphicSet->GetChild( _T("reveal\\word2\\choose1\\on1") );         //窗口模式开
	m_pBn_VerSynch_Off         = (GUIPushButton*)m_pPanel_GraphicSet->GetChild( _T("reveal\\word4\\choose2\\off2") );        //垂直同步关
	m_pBn_VerSynch_On          = (GUIPushButton*)m_pPanel_GraphicSet->GetChild( _T("reveal\\word4\\choose2\\on2") );         //垂直同步开
	m_pBn_TerrainDetail_Low    = (GUIPushButton*)m_pPanel_GraphicSet->GetChild( _T("showset\\word5\\choose3\\off3") );       //地表精度低
	m_pBn_TerrainDetail_High   = (GUIPushButton*)m_pPanel_GraphicSet->GetChild( _T("showset\\word5\\choose3\\on3") );        //地表精度高
	m_pBn_NormalMap_Off        = (GUIPushButton*)m_pPanel_GraphicSet->GetChild( _T("showset\\word9\\choose7\\off7") );       //法线贴图关
	m_pBn_NormalMap_On         = (GUIPushButton*)m_pPanel_GraphicSet->GetChild( _T("showset\\word9\\choose7\\on7") );        //法线贴图开
	m_pBn_HWSkin_Off           = (GUIPushButton*)m_pPanel_GraphicSet->GetChild( _T("showset\\word10\\choose8\\off8") );      //GPU动画关
	m_pBn_HWSkin_On            = (GUIPushButton*)m_pPanel_GraphicSet->GetChild( _T("showset\\word10\\choose8\\on8") );       //GPU动画开
	m_pBn_TexDetail_Low        = (GUIPushButton*)m_pPanel_GraphicSet->GetChild( _T("scenepic\\word20\\choose17\\off13") );   //贴图精度低
	m_pBn_TexDetail_Mid        = (GUIPushButton*)m_pPanel_GraphicSet->GetChild( _T("scenepic\\word20\\choose17\\mid13") );   //贴图精度中
	m_pBn_TexDetail_High       = (GUIPushButton*)m_pPanel_GraphicSet->GetChild( _T("scenepic\\word20\\choose17\\high13") );  //贴图精度高
	m_pBn_TerrainSpecular_Off  = (GUIPushButton*)m_pPanel_GraphicSet->GetChild( _T("showset\\word7\\choose5\\off5") );       //地形高亮关
	m_pBn_TerrainSpecular_On   = (GUIPushButton*)m_pPanel_GraphicSet->GetChild( _T("showset\\word7\\choose5\\on5") );        //地形高亮开
	m_pScro_FarView            = (GUIScrollBar* )m_pPanel_GraphicSet->GetChild( _T("scenepic\\word16\\scr1") );              //远景视野滚动条
	m_pScro_SceneDetail        = (GUIScrollBar* )m_pPanel_GraphicSet->GetChild( _T("scenepic\\word17\\scr2") );              //环境细节滚动条
	m_pScro_EffectDetail       = (GUIScrollBar* )m_pPanel_GraphicSet->GetChild( _T("scenepic\\word18\\scr3") );              //特效细节滚动条
	m_pScro_RoleView           = (GUIScrollBar* )m_pPanel_GraphicSet->GetChild( _T("scenepic\\word20\\scr4") );              //角色视野滚动条
	m_pBn_WaterSpecular_Off    = (GUIPushButton*)m_pPanel_GraphicSet->GetChild( _T("scenepic\\word16\\choose11\\off11") );   //水面高亮关
	m_pBn_WaterSpecular_On     = (GUIPushButton*)m_pPanel_GraphicSet->GetChild( _T("scenepic\\word16\\choose11\\on11") );    //水面高亮开
	m_pBn_WaterReflect_Off     = (GUIPushButton*)m_pPanel_GraphicSet->GetChild( _T("scenepic\\word17\\choose13\\off13") );   //水面反射关
	m_pBn_WaterReflect_Mid     = (GUIPushButton*)m_pPanel_GraphicSet->GetChild( _T("scenepic\\word17\\choose13\\mid13") );   //水面反射中
	m_pBn_WaterReflect_High    = (GUIPushButton*)m_pPanel_GraphicSet->GetChild( _T("scenepic\\word17\\choose13\\high13") );  //水面反射高
	m_pBn_WaterRefract_Off     = (GUIPushButton*)m_pPanel_GraphicSet->GetChild( _T("scenepic\\word18\\choose15\\off15") );   //水面折射关
	m_pBn_WaterRefract_On      = (GUIPushButton*)m_pPanel_GraphicSet->GetChild( _T("scenepic\\word18\\choose15\\on15") );    //水面折射开
	m_pBn_Lambency_Off         = (GUIPushButton*)m_pPanel_GraphicSet->GetChild( _T("scenepic\\word16\\choose12\\off12") );   //柔光效果关
	m_pBn_Lambency_On          = (GUIPushButton*)m_pPanel_GraphicSet->GetChild( _T("scenepic\\word16\\choose12\\on12") );    //柔光效果开
	m_pBn_Weather_Off          = (GUIPushButton*)m_pPanel_GraphicSet->GetChild( _T("scenepic\\word17\\choose14\\off14") );   //天气效果关
	m_pBn_Weather_On           = (GUIPushButton*)m_pPanel_GraphicSet->GetChild( _T("scenepic\\word17\\choose14\\on14") );    //天气效果开
	m_pBn_SunHalo_Off          = (GUIPushButton*)m_pPanel_GraphicSet->GetChild( _T("scenepic\\word18\\choose16\\off16") );   //太阳光晕关
	m_pBn_SunHalo_On           = (GUIPushButton*)m_pPanel_GraphicSet->GetChild( _T("scenepic\\word18\\choose16\\on16") );    //太阳光晕开
	m_pBn_DrawGrass_Off        = (GUIPushButton*)m_pPanel_GraphicSet->GetChild( _T("scenepic\\word20\\choose18\\off14") );   //花草显示关
	m_pBn_DrawGrass_On         = (GUIPushButton*)m_pPanel_GraphicSet->GetChild( _T("scenepic\\word20\\choose18\\on14") );    //花草显示开
	m_pBn_RoleShadow_Off       = (GUIPushButton*)m_pPanel_GraphicSet->GetChild( _T("reveal\\word30\\choose3\\off2") );		 //角色阴影关
	m_pBn_RoleShadow_On        = (GUIPushButton*)m_pPanel_GraphicSet->GetChild( _T("reveal\\word30\\choose3\\on2") );		 //角色阴影开

	BuildDisplayModeList();

	// 抗锯齿列表
	m_vecComboMultiSample.push_back( tagMultiSampleCfg( EMT_None, g_StrTable[_T("Close")] ) );
	m_vecComboMultiSample.push_back( tagMultiSampleCfg( EMT_2x, _T("2x") ) );
	m_vecComboMultiSample.push_back( tagMultiSampleCfg( EMT_4x, _T("4x") ) );
	m_vecComboMultiSample.push_back( tagMultiSampleCfg( EMT_8x, _T("8x") ) );
	m_pComb_MultiSample->GetListBox()->SetResizable( TRUE );
	tagPoint ptMuliSampleSize = m_pComb_MultiSample->GetListBox()->GetSize();
	ptMuliSampleSize.y = m_pComb_MultiSample->GetListBox()->GetRowHeight() * m_vecComboMultiSample.size();
	m_pComb_MultiSample->GetListBox()->Resize( ptMuliSampleSize );
	m_pComb_MultiSample->GetListBox()->SetResizable( FALSE );
	for( size_t i = 0; i < m_vecComboMultiSample.size(); i++ )
	{
		m_pComb_MultiSample->GetListBox()->SetText( i, 0, m_vecComboMultiSample[i].strShowText.c_str() );
		m_pComb_MultiSample->GetListBox()->SetColWidth( i, m_pComb_MultiSample->GetSize().x );
	}

	m_pBn_GraphicSet->SetState( EGUIBS_PushDown );
	m_pScro_FarView->SetValue( SCROBAR_TOTAL, SCROBAR_VISIBLE, 0 );
	m_pScro_RoleView->SetValue( SCROBAR_TOTAL, SCROBAR_VISIBLE, 0 );
	m_pScro_EffectDetail->SetValue( SCROBAR_TOTAL, SCROBAR_VISIBLE, 0 );
	m_pScro_SceneDetail->SetValue( SCROBAR_TOTAL, SCROBAR_VISIBLE, 0 );

	// 游戏配置需要用到的控件
	m_pBn_Music_Off					= (GUIPushButton*)m_pPanel_GameSet->GetChild( _T("gameffect\\word1\\choose1\\off1") );
	m_pBn_Music_On					= (GUIPushButton*)m_pPanel_GameSet->GetChild( _T("gameffect\\word1\\choose1\\on1") );
	m_pScro_MusicVolume				= (GUIScrollBar* )m_pPanel_GameSet->GetChild( _T("gameffect\\scr1") );
	m_pBn_Sound_Off					= (GUIPushButton*)m_pPanel_GameSet->GetChild( _T("gameffect\\word3\\choose2\\off1") );
	m_pBn_Sound_On					= (GUIPushButton*)m_pPanel_GameSet->GetChild( _T("gameffect\\word3\\choose2\\on1") );
	m_pScro_SoundVolume				= (GUIScrollBar* )m_pPanel_GameSet->GetChild( _T("gameffect\\scr2") );
	m_pBn_CombatMusic_Off           = (GUIPushButton*)m_pPanel_GameSet->GetChild( _T("gameffect\\word5\\choose2\\off1") );
	m_pBn_CombatMusic_On            = (GUIPushButton*)m_pPanel_GameSet->GetChild( _T("gameffect\\word5\\choose2\\on1") );
	m_pBn_AcceptTeamInvite_Off		= (GUIPushButton* )m_pPanel_GameSet->GetChild( _T("gameopt\\word1\\choose1\\off1") );
	m_pBn_AcceptTeamInvite_On		= (GUIPushButton* )m_pPanel_GameSet->GetChild( _T("gameopt\\word1\\choose1\\on1") );
	m_pBn_AcceptFactionInvite_Off	= (GUIPushButton* )m_pPanel_GameSet->GetChild( _T("gameopt\\word2\\choose2\\off2") );
	m_pBn_AcceptFactionInvite_On	= (GUIPushButton* )m_pPanel_GameSet->GetChild( _T("gameopt\\word2\\choose2\\on2") );
	m_pBn_AllowFriendAdd_Off		= (GUIPushButton* )m_pPanel_GameSet->GetChild( _T("gameopt\\word3\\choose3\\off3") );
	m_pBn_AllowFriendAdd_On			= (GUIPushButton* )m_pPanel_GameSet->GetChild( _T("gameopt\\word3\\choose3\\on3") );
	m_pBn_AcceptTradeRequest_Off	= (GUIPushButton* )m_pPanel_GameSet->GetChild( _T("gameopt\\word4\\choose4\\off4") );
	m_pBn_AcceptTradeRequest_On		= (GUIPushButton* )m_pPanel_GameSet->GetChild( _T("gameopt\\word4\\choose4\\on4") );
	m_pBn_SelfInfoOpenType_Private	= (GUIPushButton* )m_pPanel_GameSet->GetChild( _T("gameopt\\word6\\choose6\\none") );
	m_pBn_SelfInfoOpenType_Friend	= (GUIPushButton* )m_pPanel_GameSet->GetChild( _T("gameopt\\word6\\choose6\\some") );
	m_pBn_SelfInfoOpenType_Public	= (GUIPushButton* )m_pPanel_GameSet->GetChild( _T("gameopt\\word6\\choose6\\all") );
	m_pBn_ShowSelfName				= (GUIPushButton* )m_pPanel_GameSet->GetChild( _T("displayset\\word1\\butt1") );
	m_pBn_ShowOtherPlayerName		= (GUIPushButton* )m_pPanel_GameSet->GetChild( _T("displayset\\word2\\butt2") );
	m_pBn_ShowFactionName			= (GUIPushButton* )m_pPanel_GameSet->GetChild( _T("displayset\\word3\\butt3") );
	m_pBn_ShowChatPop				= (GUIPushButton* )m_pPanel_GameSet->GetChild( _T("displayset\\word4\\butt4") );
	m_pBn_ShowHat					= (GUIPushButton* )m_pPanel_GameSet->GetChild( _T("displayset\\word5\\butt5") );
	m_pBn_ShowMantle				= (GUIPushButton* )m_pPanel_GameSet->GetChild( _T("displayset\\word6\\butt6") );
	m_pBn_ShowOtherPalyeTitle		= (GUIPushButton* )m_pPanel_GameSet->GetChild( _T("displayset\\word7\\butt7") );
	m_pBn_ShowSkillNameFloatTip     = (GUIPushButton* )m_pPanel_GameSet->GetChild( _T("displayset\\word8\\butt8") );
	m_pBn_ShowHPMPText				= (GUIPushButton* )m_pPanel_GameSet->GetChild( _T("displayset\\word9\\butt9") );
	m_pBn_ShowMonsterName			= (GUIPushButton* )m_pPanel_GameSet->GetChild( _T("displayset\\word10\\butt10") );

	m_pScro_MusicVolume->SetValue( SCROBAR_TOTAL, SCROBAR_VISIBLE, 0 );
	m_pScro_SoundVolume->SetValue( SCROBAR_TOTAL, SCROBAR_VISIBLE, 0 );

	m_pWnd->SetInvisible( FALSE );
	SettingsToUI();

	return TRUE;
}

BOOL GameSettingsFrame::EscCancel()
{
	m_pMgr->AddToDestroyList( this );
	return TRUE;
}

VOID GameSettingsFrame::SafeDestroyUI()
{
	if( P_VALID( m_pWnd ) )
	{
		m_pGUI->DestroyWnd( m_pWnd );
		m_pWnd = NULL;
	}

	m_vecComboWHB.clear();
	m_vecComboHz.clear();
	m_vecComboMultiSample.clear();
	m_nSelectWHB = 0;         
	m_nSelectHz = 0;              
	m_nSelectMultiSample = 0;
}

//-----------------------------------------------------------------------------
//! 游戏设置事件处理
//! 
//-----------------------------------------------------------------------------
BOOL GameSettingsFrame::GUIEventHandler(tagGUIEvent* pEvent)
{
	const GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;

	switch( pEvent->eEvent )
	{
	case EGUIE_Active:
		if( pWnd == m_pComb_WHB->GetEditBox() )
			m_pGUI->SetActive( m_pComb_WHB->GetListBox() );
		else if( pWnd == m_pComb_Hz->GetEditBox() )
			m_pGUI->SetActive( m_pComb_Hz->GetListBox() );
		else if( pWnd == m_pComb_MultiSample->GetEditBox() )
			m_pGUI->SetActive( m_pComb_MultiSample->GetListBox() );
		break;

	case EGUIE_ItemClick:
		if( pWnd == m_pComb_WHB->GetListBox() )
			m_nSelectWHB = pEvent->dwParam1;
		else if( pWnd == m_pComb_Hz->GetListBox() )
			m_nSelectHz = pEvent->dwParam1;
		else if( pWnd == m_pComb_MultiSample->GetListBox() )
			m_nSelectMultiSample = pEvent->dwParam1;
		break;

	case EGUIE_DragEnd:
		if( pWnd == m_pScro_FarView )
		{
			const int nValue = CalcScrobarStepValue( m_pScro_FarView->GetValue(), MapMgr::MAX_VIEW_DIST );
			m_pScro_FarView->SetValue( nValue );
		}
		else if( pWnd == m_pScro_SceneDetail )
		{
			const int nValue = CalcScrobarStepValue( m_pScro_SceneDetail->GetValue(), MapMgr::MAX_VIEW_DIST );
			m_pScro_SceneDetail->SetValue( nValue );
		}
		else if( pWnd == m_pScro_EffectDetail )
		{
			const int nValue = CalcScrobarStepValue( m_pScro_EffectDetail->GetValue(), SGEffectNode::MAX_LOD );
			m_pScro_EffectDetail->SetValue( nValue );
		}
		else if( pWnd == m_pScro_RoleView )
		{
			const int nValue = CalcScrobarStepValue( m_pScro_RoleView->GetValue(), RoleMgr::MAX_VIEW_DIST );
			m_pScro_RoleView->SetValue( nValue );
		}
		break;
	case EGUIE_Click:
		if( pWnd == m_pBn_Close )					        //关闭窗口
		{
			m_pMgr->AddToDestroyList( this );
		}
		if( pWnd == m_pBn_AutoCheck )                       //自动检测
		{
			GameSet::Inst()->BuildDefaultGraphicSettings();
			SettingsToUI();
		}
		else if( pWnd == m_pBn_OK )                         //确定
		{
			UIToSettings();
			ApplySettings();
			GameSet::Inst()->SaveAll();
			m_pMgr->AddToDestroyList( this );
		}
		else if( pWnd == m_pBn_Cancel )                     //取消
		{
			m_pMgr->AddToDestroyList( this );
		}
		else if( pWnd == m_pBn_Apply )                      //应用
		{
			if( timeGetTime() - m_dwLastApplyTime > 2000 )
			{
				UIToSettings();
				ApplySettings();
				GameSet::Inst()->SaveAll();
				m_dwLastApplyTime = timeGetTime();
			}
		}
		break;

	case EGUIE_PushButtonDown:
		if( pWnd  == m_pBn_GraphicSet )                     //按下“画面配置”按钮
		{
			m_pPanel_GraphicSet->SetInvisible( FALSE );
			m_pPanel_GameSet->SetInvisible( TRUE );
			m_pBn_AutoCheck->SetInvisible( FALSE );
		}
		else if( pWnd == m_pBn_GameSet )                    //按下“游戏配置”按钮
		{
			m_pPanel_GraphicSet->SetInvisible( TRUE );
			m_pPanel_GameSet->SetInvisible( FALSE );
			m_pBn_AutoCheck->SetInvisible( TRUE );
		}
		break;
	default:
		break;
	}// switch( pEvent->eEvent )

	return FALSE;
}

VOID GameSettingsFrame::EnableControls()
{
	if( !P_VALID( m_pWnd ) )
		return;

	if( !Cool3D::Cool3DOption::Inst()->IsSupport( Cool3D::Cool3DOption::EO_TerrainDetail, Cool3D::Cool3DOption::ETD_MultiLayers ) )
	{
		m_pBn_TerrainDetail_Low->SetState( EGUIBS_PushDown );
		m_pBn_TerrainDetail_High->SetEnable( FALSE );
	}
	if( !Cool3D::Cool3DOption::Inst()->IsSupport( Cool3D::Cool3DOption::EO_TerrainSpecular, Cool3D::Cool3DOption::ETS_Open ) )
	{
		m_pBn_TerrainSpecular_Off->SetState( EGUIBS_PushDown );
		m_pBn_TerrainSpecular_On->SetEnable( FALSE );
	}
	if( !Cool3D::Cool3DOption::Inst()->IsSupport( Cool3D::Cool3DOption::EO_TexDetail, Cool3D::Cool3DOption::ETexDetail_High ) )
	{
		m_pBn_TexDetail_Low->SetState( EGUIBS_PushDown );
		m_pBn_TexDetail_High->SetEnable( FALSE );
	}
	if( !Cool3D::Cool3DOption::Inst()->IsSupport( Cool3D::Cool3DOption::EO_NormalMap, Cool3D::Cool3DOption::ENM_Open ) )
	{
		m_pBn_NormalMap_Off->SetState( EGUIBS_PushDown );
		m_pBn_NormalMap_On->SetEnable( FALSE );
	}
	if( !Cool3D::Cool3DOption::Inst()->IsSupport( Cool3D::Cool3DOption::EO_HWSkin, Cool3D::Cool3DOption::EHWS_Open ) )
	{
		m_pBn_HWSkin_Off->SetState( EGUIBS_PushDown );
		m_pBn_HWSkin_On->SetEnable( FALSE );
	}
	if( !Cool3D::Cool3DOption::Inst()->IsSupport( Cool3D::Cool3DOption::EO_WaterSpecular, Cool3D::Cool3DOption::EWS_Open ) )
	{
		m_pBn_WaterSpecular_Off->SetState( EGUIBS_PushDown );
		m_pBn_WaterSpecular_On->SetEnable( FALSE );
	}
	if( !Cool3D::Cool3DOption::Inst()->IsSupport( Cool3D::Cool3DOption::EO_WaterReflect, Cool3D::Cool3DOption::EWR_Mid ) )
	{
		m_pBn_WaterReflect_Off->SetState( EGUIBS_PushDown );
		m_pBn_WaterReflect_High->SetEnable( FALSE );
		m_pBn_WaterReflect_Mid->SetEnable( FALSE );
	}
	else if( !Cool3D::Cool3DOption::Inst()->IsSupport( Cool3D::Cool3DOption::EO_WaterReflect, Cool3D::Cool3DOption::EWR_High ) )
	{
		m_pBn_WaterReflect_Mid->SetState( EGUIBS_PushDown );
		m_pBn_WaterReflect_High->SetEnable( FALSE );
	}
	else if( !Cool3D::Cool3DOption::Inst()->IsSupport( Cool3D::Cool3DOption::EO_WaterRefract, Cool3D::Cool3DOption::EWRR_Open ) )
	{
		m_pBn_WaterRefract_Off->SetState( EGUIBS_PushDown );
		m_pBn_WaterRefract_On->SetEnable( FALSE );
	}
	if( !SGDynamicProjector::IsHWSupport() )
	{
		m_pBn_RoleShadow_Off->SetState( EGUIBS_PushDown );
		m_pBn_RoleShadow_On->SetEnable( FALSE );
	}
	if( !PostProcessBlur::IsHWSupport() )
	{
		m_pBn_Lambency_Off->SetState( EGUIBS_PushDown );
		m_pBn_Lambency_On->SetEnable( FALSE );
	}
	if( !SGSunFlare::IsHWSupport() )
	{
		m_pBn_SunHalo_Off->SetState( EGUIBS_PushDown );
		m_pBn_SunHalo_On->SetEnable( FALSE );
	}

	// FOR DEBUG 暂时禁用没有功能的控件
	m_pBn_Weather_On->SetEnable( FALSE );
	m_pBn_Weather_Off->SetEnable( FALSE );
}


BOOL GameSettingsFrame::SettingsToUI()
{
	if( !P_VALID( m_pWnd ) )
		return FALSE;

	// 画面设置
	for( UINT i = 0; i < m_vecComboWHB.size(); i++ )
	{
		if( GameSet::Inst()->Get( EGS_PelsWidth ) == m_vecComboWHB[i].dwPelsWidth &&
			GameSet::Inst()->Get( EGS_PelsHeight ) == m_vecComboWHB[i].dwPelsHeight &&
			GameSet::Inst()->Get( EGS_BitsPerPel ) == m_vecComboWHB[i].dwBitsPerPel )
		{
			tagGUIEvent event(m_pComb_WHB, EGUIE_ItemClick );
			event.dwParam1 = i;
			m_pComb_WHB->SendEvent( &event );
			m_nSelectWHB = i;
			break;
		}
	}

	for( UINT i = 0; i < m_vecComboHz.size(); i++ )
	{
		if( GameSet::Inst()->Get( EGS_DisplayFrequency ) == m_vecComboHz[i] )
		{
			tagGUIEvent event(m_pComb_Hz, EGUIE_ItemClick );
			event.dwParam1 = i;
			m_pComb_Hz->SendEvent( &event );
			m_nSelectHz = i;
			break;
		}
	}
	
	for( UINT i = 0; i < m_vecComboMultiSample.size(); i++ )
	{
		if( GameSet::Inst()->Get( EGS_MultiSampleType ) == m_vecComboMultiSample[i].eLevel )
		{
			tagGUIEvent event(m_pComb_MultiSample, EGUIE_ItemClick );
			event.dwParam1 = i;
			m_pComb_MultiSample->SendEvent( &event );
			m_nSelectMultiSample = i;
			break;
		}
	}

	GameSet::Inst()->Get( EGS_Windowed ) ? m_pBn_Windowed_On->SetState( EGUIBS_PushDown ) : m_pBn_Windowed_Off->SetState( EGUIBS_PushDown );
	GameSet::Inst()->Get( EGS_VerSynch ) ? m_pBn_VerSynch_On->SetState( EGUIBS_PushDown ) : m_pBn_VerSynch_Off->SetState( EGUIBS_PushDown );

	Cool3D::Cool3DOption::ETD_MultiLayers == GameSet::Inst()->Get( EGS_TerrainDetail )   ?  m_pBn_TerrainDetail_High->SetState( EGUIBS_PushDown ) : m_pBn_TerrainDetail_Low->SetState( EGUIBS_PushDown );
	Cool3D::Cool3DOption::ETS_Open        == GameSet::Inst()->Get( EGS_TerrainSpecular ) ?  m_pBn_TerrainSpecular_On->SetState( EGUIBS_PushDown ) : m_pBn_TerrainSpecular_Off->SetState( EGUIBS_PushDown );
	Cool3D::Cool3DOption::ENM_Open        == GameSet::Inst()->Get( EGS_NormalMap )       ?  m_pBn_NormalMap_On->SetState( EGUIBS_PushDown )       : m_pBn_NormalMap_Off->SetState( EGUIBS_PushDown );
	Cool3D::Cool3DOption::EHWS_Open       == GameSet::Inst()->Get( EGS_HWSkin )          ?  m_pBn_HWSkin_On->SetState( EGUIBS_PushDown )          : m_pBn_HWSkin_Off->SetState( EGUIBS_PushDown );
	
	const INT nCurScroFarView = (INT)( (float)GameSet::Inst()->Get( EGS_FarView ) / MapMgr::MAX_VIEW_DIST * ( SCROBAR_TOTAL - SCROBAR_VISIBLE ) );
	m_pScro_FarView->SetValue( SCROBAR_TOTAL, SCROBAR_VISIBLE, nCurScroFarView );

	const INT nCurSceneDetail = (INT)( (float)GameSet::Inst()->Get( EGS_SceneDetail ) / MapMgr::MAX_VIEW_DIST * ( SCROBAR_TOTAL - SCROBAR_VISIBLE ) );
	m_pScro_SceneDetail->SetValue( SCROBAR_TOTAL, SCROBAR_VISIBLE, nCurSceneDetail );

	const INT nCurEffectDetail = (INT)( (float)GameSet::Inst()->Get( EGS_EffectDetail ) / SGEffectNode::MAX_LOD * ( SCROBAR_TOTAL - SCROBAR_VISIBLE ) );
	m_pScro_EffectDetail->SetValue( SCROBAR_TOTAL, SCROBAR_VISIBLE, nCurEffectDetail );

	const INT nCurScroRoleView = (INT)( (float)GameSet::Inst()->Get( EGS_RoleView ) / RoleMgr::MAX_VIEW_DIST * ( SCROBAR_TOTAL - SCROBAR_VISIBLE ) );
	m_pScro_RoleView->SetValue( SCROBAR_TOTAL, SCROBAR_VISIBLE, nCurScroRoleView );

	if( Cool3D::Cool3DOption::ETexDetail_High == GameSet::Inst()->Get( EGS_TexDetail ) )
		m_pBn_TexDetail_High->SetState( EGUIBS_PushDown );
	else if( Cool3D::Cool3DOption::ETexDetail_Mid == GameSet::Inst()->Get( EGS_TexDetail ) )
		m_pBn_TexDetail_Mid->SetState( EGUIBS_PushDown );
	else
		m_pBn_TexDetail_Low->SetState( EGUIBS_PushDown );

	Cool3D::Cool3DOption::EWS_Open == GameSet::Inst()->Get( EGS_WaterSpecular )   ?  m_pBn_WaterSpecular_On->SetState( EGUIBS_PushDown )   : m_pBn_WaterSpecular_Off->SetState( EGUIBS_PushDown );

	if( Cool3D::Cool3DOption::EWR_High == GameSet::Inst()->Get( EGS_WaterReflect ) )
		m_pBn_WaterReflect_High->SetState( EGUIBS_PushDown );
	else if( Cool3D::Cool3DOption::EWR_Mid == GameSet::Inst()->Get( EGS_WaterReflect ) )
		m_pBn_WaterReflect_Mid->SetState( EGUIBS_PushDown );
	else
		m_pBn_WaterReflect_Off->SetState( EGUIBS_PushDown );

	if( Cool3D::Cool3DOption::EWRR_Open == GameSet::Inst()->Get( EGS_WaterRefract ) )
		m_pBn_WaterRefract_On->SetState( EGUIBS_PushDown );
	else
		m_pBn_WaterRefract_Off->SetState( EGUIBS_PushDown );

	if( Cool3D::Cool3DOption::EDG_Open == GameSet::Inst()->Get( EGS_DrawGrass ) )
		m_pBn_DrawGrass_On->SetState( EGUIBS_PushDown );
	else
		m_pBn_DrawGrass_Off->SetState( EGUIBS_PushDown );

	if( GameSet::Inst()->Get( EGS_RoleShadow ) )
		m_pBn_RoleShadow_On->SetState( EGUIBS_PushDown );
	else
		m_pBn_RoleShadow_Off->SetState( EGUIBS_PushDown );

	if( GameSet::Inst()->Get( EGS_Lambency ) )
		m_pBn_Lambency_On->SetState( EGUIBS_PushDown );
	else
		m_pBn_Lambency_Off->SetState( EGUIBS_PushDown );

	if( GameSet::Inst()->Get( EGS_SunHalo ) )
		m_pBn_SunHalo_On->SetState( EGUIBS_PushDown );
	else
		m_pBn_SunHalo_Off->SetState( EGUIBS_PushDown );

	//TODO:天气效果

	// 游戏设置
	if( Cool3D::Cool3DOption::EM_Open == GameSet::Inst()->Get( EGS_Music ) )
		m_pBn_Music_On->SetState( EGUIBS_PushDown );
	else
		m_pBn_Music_Off->SetState( EGUIBS_PushDown );

	const INT nCurScroMusicVolume = INT( (float)GameSet::Inst()->Get( EGS_MusicVol ) / GameSet::MAX_MUSIC_VOLUME * ( SCROBAR_TOTAL - SCROBAR_VISIBLE ) );
	m_pScro_MusicVolume->SetValue( SCROBAR_TOTAL, SCROBAR_VISIBLE, nCurScroMusicVolume );

	if( Cool3D::Cool3DOption::ES_Open == GameSet::Inst()->Get( EGS_Sound ) )
		m_pBn_Sound_On->SetState( EGUIBS_PushDown );
	else
		m_pBn_Sound_Off->SetState( EGUIBS_PushDown );

	const INT nCurScroSoundVolume = INT( (float)GameSet::Inst()->Get( EGS_SoundVol ) / GameSet::MAX_SOUND_VOLUME * ( SCROBAR_TOTAL - SCROBAR_VISIBLE ) );
	m_pScro_SoundVolume->SetValue( SCROBAR_TOTAL, SCROBAR_VISIBLE, nCurScroSoundVolume );

	if( GameSet::Inst()->Get( EGS_CombatMusic ) )
		m_pBn_CombatMusic_On->SetState( EGUIBS_PushDown );
	else
		m_pBn_CombatMusic_Off->SetState( EGUIBS_PushDown );

	LocalPlayer* pLP = RoleMgr::Inst()->GetLocalPlayer();
	if( P_VALID( pLP ) && pLP->IsAttributeInited() )
	{
		m_curPersonalSet = pLP->GetPersonalSet();
		m_curDisplaySet = pLP->GetDisplaySet();
		if( m_curPersonalSet.bAcceptTeamInvite )
			m_pBn_AcceptTeamInvite_On->SetState( EGUIBS_PushDown );
		else
			m_pBn_AcceptTeamInvite_Off->SetState( EGUIBS_PushDown );
		
		if( m_curPersonalSet.bAcceptFactionInvite )
			m_pBn_AcceptFactionInvite_On->SetState( EGUIBS_PushDown );
		else
			m_pBn_AcceptFactionInvite_Off->SetState( EGUIBS_PushDown );

		if( m_curPersonalSet.bAllowFriendAdd )
			m_pBn_AllowFriendAdd_On->SetState( EGUIBS_PushDown );
		else
			m_pBn_AllowFriendAdd_Off->SetState( EGUIBS_PushDown );

		if( m_curPersonalSet.bAcceptTradeRequest )
			m_pBn_AcceptTradeRequest_On->SetState( EGUIBS_PushDown );
		else
			 m_pBn_AcceptTradeRequest_Off->SetState( EGUIBS_PushDown );

		if( EPIS_Public == m_curPersonalSet.eInfoOpenType )
			m_pBn_SelfInfoOpenType_Public->SetState( EGUIBS_PushDown );
		else if( EPIS_Friend == m_curPersonalSet.eInfoOpenType )
			m_pBn_SelfInfoOpenType_Friend->SetState( EGUIBS_PushDown );
		else
			m_pBn_SelfInfoOpenType_Private->SetState( EGUIBS_PushDown );

		m_pBn_ShowHat->SetState( false == m_curDisplaySet.bHideHead ? EGUIBS_PushDown : EGUIBS_PopUp );
		m_pBn_ShowMantle->SetState( false == m_curDisplaySet.bHideBack ? EGUIBS_PushDown : EGUIBS_PopUp );
	}

	m_pBn_ShowSelfName->SetState( TRUE == GameSet::Inst()->Get( EGS_ShowSelfName ) ? EGUIBS_PushDown : EGUIBS_PopUp );	
	m_pBn_ShowOtherPlayerName->SetState( TRUE == GameSet::Inst()->Get( EGS_ShowOtherPlayerName ) ? EGUIBS_PushDown : EGUIBS_PopUp );	
	m_pBn_ShowFactionName->SetState( TRUE == GameSet::Inst()->Get( EGS_ShowFactionName ) ? EGUIBS_PushDown : EGUIBS_PopUp );	
	m_pBn_ShowChatPop->SetState( TRUE == GameSet::Inst()->Get( EGS_ShowChatPop ) ? EGUIBS_PushDown : EGUIBS_PopUp );	
	m_pBn_ShowOtherPalyeTitle->SetState( TRUE == GameSet::Inst()->Get( EGS_ShowOtherPalyerTitle ) ? EGUIBS_PushDown : EGUIBS_PopUp );	
	m_pBn_ShowSkillNameFloatTip->SetState( TRUE == GameSet::Inst()->Get( EGS_ShowSkillNameFloatTip ) ? EGUIBS_PushDown : EGUIBS_PopUp );	
	m_pBn_ShowHPMPText->SetState( TRUE == GameSet::Inst()->Get( EGS_ShowHPMPText ) ? EGUIBS_PushDown : EGUIBS_PopUp );	
	m_pBn_ShowMonsterName->SetState( TRUE == GameSet::Inst()->Get( EGS_ShowMonsterName ) ? EGUIBS_PushDown : EGUIBS_PopUp );	

	EnableControls();

	return TRUE;
}

BOOL GameSettingsFrame::UIToSettings()
{
	if( !P_VALID( m_pWnd ) )
		return FALSE;

	// 画面设置
	if( (UINT)m_nSelectWHB < m_vecComboWHB.size() &&
		m_nSelectWHB >= 0 )
	{
		GameSet::Inst()->Set( EGS_PelsWidth, m_vecComboWHB[m_nSelectWHB].dwPelsWidth );
		GameSet::Inst()->Set( EGS_PelsHeight, m_vecComboWHB[m_nSelectWHB].dwPelsHeight );
		GameSet::Inst()->Set( EGS_BitsPerPel, m_vecComboWHB[m_nSelectWHB].dwBitsPerPel );
	}

	if( (UINT)m_nSelectHz < m_vecComboHz.size() &&
		m_nSelectHz >= 0 )
	{
		GameSet::Inst()->Set( EGS_DisplayFrequency, m_vecComboHz[m_nSelectHz] );
	}

	if( (UINT)m_nSelectMultiSample < m_vecComboMultiSample.size() &&
		m_nSelectMultiSample >= 0 )
	{
		GameSet::Inst()->Set( EGS_MultiSampleType, m_vecComboMultiSample[m_nSelectMultiSample].eLevel );
	}

	if( EGUIBS_PushDown == m_pBn_Windowed_On->GetState() )
	{
		// 窗口模式有三种
		if( FALSE == GameSet::Inst()->Get( EGS_Windowed ) )
			GameSet::Inst()->Set( EGS_Windowed, TRUE );
	}
	else
	{
		GameSet::Inst()->Set( EGS_Windowed, FALSE );
	}

	if( EGUIBS_PushDown == m_pBn_VerSynch_On->GetState() )
		GameSet::Inst()->Set( EGS_VerSynch, 1 );
	else
		GameSet::Inst()->Set( EGS_VerSynch, 0 );

	if( EGUIBS_PushDown == m_pBn_TerrainDetail_High->GetState() )
		GameSet::Inst()->Set( EGS_TerrainDetail, Cool3D::Cool3DOption::ETD_MultiLayers );
	else
		GameSet::Inst()->Set( EGS_TerrainDetail, Cool3D::Cool3DOption::ETD_TwoLayers );

	if( EGUIBS_PushDown == m_pBn_TerrainSpecular_On->GetState() )
		GameSet::Inst()->Set( EGS_TerrainSpecular, Cool3D::Cool3DOption::ETS_Open );
	else
		GameSet::Inst()->Set( EGS_TerrainSpecular, Cool3D::Cool3DOption::ETS_None );

	if( EGUIBS_PushDown == m_pBn_TexDetail_High->GetState() )
		GameSet::Inst()->Set( EGS_TexDetail, Cool3D::Cool3DOption::ETexDetail_High );
	else if( EGUIBS_PushDown == m_pBn_TexDetail_Mid->GetState() )
		GameSet::Inst()->Set( EGS_TexDetail, Cool3D::Cool3DOption::ETexDetail_Mid );
	else
		GameSet::Inst()->Set( EGS_TexDetail, Cool3D::Cool3DOption::ETexDetail_Low );

	if( EGUIBS_PushDown == m_pBn_NormalMap_On->GetState() )
		GameSet::Inst()->Set( EGS_NormalMap, Cool3D::Cool3DOption::ENM_Open );
	else
		GameSet::Inst()->Set( EGS_NormalMap, Cool3D::Cool3DOption::ENM_None );

	if( EGUIBS_PushDown == m_pBn_HWSkin_On->GetState() )
		GameSet::Inst()->Set( EGS_HWSkin, Cool3D::Cool3DOption::EHWS_Open );
	else
		GameSet::Inst()->Set( EGS_HWSkin, Cool3D::Cool3DOption::EHWS_None );
	
	const INT nCurFarView = INT( (float)m_pScro_FarView->GetValue() / ( SCROBAR_TOTAL - SCROBAR_VISIBLE ) * MapMgr::MAX_VIEW_DIST );
	GameSet::Inst()->Set( EGS_FarView, nCurFarView );

	const INT nCurSceneDetail = INT( (float)m_pScro_SceneDetail->GetValue() / ( SCROBAR_TOTAL - SCROBAR_VISIBLE ) * MapMgr::MAX_VIEW_DIST );
	GameSet::Inst()->Set( EGS_SceneDetail, nCurSceneDetail );

	const INT nEffectDetail = INT( (float)m_pScro_EffectDetail->GetValue() / ( SCROBAR_TOTAL - SCROBAR_VISIBLE ) * SGEffectNode::MAX_LOD );
	GameSet::Inst()->Set( EGS_EffectDetail, nEffectDetail );

	const INT nRoleView = INT( (float)m_pScro_RoleView->GetValue() / ( SCROBAR_TOTAL - SCROBAR_VISIBLE ) * RoleMgr::MAX_VIEW_DIST );
	GameSet::Inst()->Set( EGS_RoleView, nRoleView );

	if( EGUIBS_PushDown == m_pBn_WaterSpecular_On->GetState() )
		GameSet::Inst()->Set( EGS_WaterSpecular, Cool3D::Cool3DOption::EWS_Open );
	else
		GameSet::Inst()->Set( EGS_WaterSpecular, Cool3D::Cool3DOption::EWS_None );

	if( EGUIBS_PushDown == m_pBn_WaterReflect_High->GetState() )
		GameSet::Inst()->Set( EGS_WaterReflect, Cool3D::Cool3DOption::EWR_High );
	else if( EGUIBS_PushDown == m_pBn_WaterReflect_Mid->GetState() )
		GameSet::Inst()->Set( EGS_WaterReflect, Cool3D::Cool3DOption::EWR_Mid );
	else
		GameSet::Inst()->Set( EGS_WaterReflect, Cool3D::Cool3DOption::EWR_None );

	if( EGUIBS_PushDown == m_pBn_WaterRefract_On->GetState() )
		GameSet::Inst()->Set( EGS_WaterRefract, Cool3D::Cool3DOption::EWRR_Open );	
	else
		GameSet::Inst()->Set( EGS_WaterRefract, Cool3D::Cool3DOption::EWRR_None );

	if( EGUIBS_PushDown == m_pBn_DrawGrass_On->GetState() )
		GameSet::Inst()->Set( EGS_DrawGrass, Cool3D::Cool3DOption::EDG_Open );
	else
		GameSet::Inst()->Set( EGS_DrawGrass, Cool3D::Cool3DOption::EDG_None );

	if( EGUIBS_PushDown == m_pBn_RoleShadow_On->GetState() )
		GameSet::Inst()->Set( EGS_RoleShadow, TRUE );
	else
		GameSet::Inst()->Set( EGS_RoleShadow, FALSE );

	if( EGUIBS_PushDown == m_pBn_Lambency_On->GetState() )
		GameSet::Inst()->Set( EGS_Lambency, TRUE );
	else
		GameSet::Inst()->Set( EGS_Lambency, FALSE );

	if( EGUIBS_PushDown == m_pBn_SunHalo_On->GetState() )
		GameSet::Inst()->Set( EGS_SunHalo, TRUE );
	else
		GameSet::Inst()->Set( EGS_SunHalo, FALSE );

	// 天气效果

	// 游戏设置
	if( EGUIBS_PushDown == m_pBn_Music_On->GetState() )
		GameSet::Inst()->Set( EGS_Music, Cool3D::Cool3DOption::EM_Open );
	else
		GameSet::Inst()->Set( EGS_Music, Cool3D::Cool3DOption::EM_None );

	const INT nCurMusicVolume = INT( (float)m_pScro_MusicVolume->GetValue() / ( SCROBAR_TOTAL - SCROBAR_VISIBLE ) * GameSet::MAX_MUSIC_VOLUME );
	GameSet::Inst()->Set( EGS_MusicVol, nCurMusicVolume );

	if( EGUIBS_PushDown == m_pBn_Sound_On->GetState() )
		GameSet::Inst()->Set( EGS_Sound, Cool3D::Cool3DOption::ES_Open );
	else
		GameSet::Inst()->Set( EGS_Sound, Cool3D::Cool3DOption::ES_None );

	const INT nCurSoundVolume = INT( (float)m_pScro_SoundVolume->GetValue() / ( SCROBAR_TOTAL - SCROBAR_VISIBLE ) * GameSet::MAX_SOUND_VOLUME );
	GameSet::Inst()->Set( EGS_SoundVol, nCurSoundVolume );

	GameSet::Inst()->Set( EGS_CombatMusic, EGUIBS_PushDown == m_pBn_CombatMusic_On->GetState() );

	LocalPlayer* pLP = RoleMgr::Inst()->GetLocalPlayer();
	if( P_VALID( pLP ) && pLP->IsAttributeInited() )
	{
		m_curPersonalSet.bAcceptTeamInvite = EGUIBS_PushDown == m_pBn_AcceptTeamInvite_On->GetState();
		m_curPersonalSet.bAcceptFactionInvite = EGUIBS_PushDown == m_pBn_AcceptFactionInvite_On->GetState();
		m_curPersonalSet.bAllowFriendAdd = EGUIBS_PushDown == m_pBn_AllowFriendAdd_On->GetState();
		m_curPersonalSet.bAcceptTradeRequest = EGUIBS_PushDown == m_pBn_AcceptTradeRequest_On->GetState();

		if( EGUIBS_PushDown == m_pBn_SelfInfoOpenType_Public->GetState() )
			m_curPersonalSet.eInfoOpenType = EPIS_Public;
		else if( EGUIBS_PushDown == m_pBn_SelfInfoOpenType_Friend->GetState() )
			m_curPersonalSet.eInfoOpenType = EPIS_Friend;
		else
			m_curPersonalSet.eInfoOpenType = EPIS_Private;

		m_curDisplaySet.bHideHead = EGUIBS_PushDown != m_pBn_ShowHat->GetState();
		m_curDisplaySet.bHideBack = EGUIBS_PushDown != m_pBn_ShowMantle->GetState();
	}
	GameSet::Inst()->Set( EGS_ShowSelfName, EGUIBS_PushDown == m_pBn_ShowSelfName->GetState() );
	GameSet::Inst()->Set( EGS_ShowOtherPlayerName, EGUIBS_PushDown == m_pBn_ShowOtherPlayerName->GetState() );
	GameSet::Inst()->Set( EGS_ShowFactionName, EGUIBS_PushDown == m_pBn_ShowFactionName->GetState() );
	GameSet::Inst()->Set( EGS_ShowChatPop, EGUIBS_PushDown == m_pBn_ShowChatPop->GetState() );
	GameSet::Inst()->Set( EGS_ShowOtherPalyerTitle, EGUIBS_PushDown == m_pBn_ShowOtherPalyeTitle->GetState() );
	GameSet::Inst()->Set( EGS_ShowSkillNameFloatTip, EGUIBS_PushDown == m_pBn_ShowSkillNameFloatTip->GetState() );
	GameSet::Inst()->Set( EGS_ShowHPMPText, EGUIBS_PushDown == m_pBn_ShowHPMPText->GetState() );
	GameSet::Inst()->Set( EGS_ShowMonsterName, EGUIBS_PushDown == m_pBn_ShowMonsterName->GetState() );

	return TRUE;
}


VOID GameSettingsFrame::BuildDisplayModeList()
{
	if( !P_VALID( m_pWnd ) )
		return;

	//const UINT szAllowedWH[][2] =
	//{
	//	{ 800, 600 },
	//	{ 1024, 768 },
	//	{ 1280, 1024 },
	//	{ 1440, 900 }
	//};
	//const UINT nAllowedWHCount = sizeof(szAllowedWH) / sizeof(szAllowedWH[0]);

	const UINT szAllowedBpp[] = { 16, 24, 32 };
	const UINT nAllowedBppCount = sizeof(szAllowedBpp) / sizeof(szAllowedBpp[0]);
	
	const UINT szAllowedHz[] = { 60, 70, 75, 85 };
	const UINT nAllowedHzCount = sizeof(szAllowedHz) / sizeof(szAllowedHz[0]);


	m_vecComboWHB.clear();
	m_vecComboHz.clear();

	std::list<tagDisplayMode> listMode;
	Kernel::Inst()->GetDevice()->EnumDisplayModes( listMode, 1024, 768 );
	for( std::list<tagDisplayMode>::iterator itr = listMode.begin(); itr != listMode.end(); itr++ )
	{
		//过滤
		//BOOL bAllowedWH = FALSE;
		BOOL bAllowedBpp = FALSE;
		BOOL bAllowedHz = FALSE;
		//for( UINT i = 0; i < nAllowedWHCount; i++ )
		//{
		//	if( itr->nWidth == szAllowedWH[i][0] &&
		//		itr->nHeight == szAllowedWH[i][1] )
		//	{
		//		bAllowedWH = TRUE;
		//		break;
		//	}
		//}
		//if( !bAllowedWH )
		//	continue;


		for( UINT i = 0; i < nAllowedBppCount; i++ )
		{
			if( PixelFormatToBpp( itr->eFormat ) == szAllowedBpp[i] )
			{
				bAllowedBpp = TRUE;
				break;
			}
		}
		if( !bAllowedBpp )
			continue;

		
		for( UINT i = 0; i < nAllowedHzCount; i++ )
		{
			if( itr->nRefreshRate == szAllowedHz[i] )
			{
				bAllowedHz = TRUE;
				break;
			}
		}
		if( !bAllowedHz )
			continue;


		//填充分辨率下拉框列表
		{
			tagComboWHB WHB( itr->nWidth, itr->nHeight, PixelFormatToBpp( itr->eFormat ), itr->eFormat );
			BOOL bContainWHB = FALSE;
			for( UINT i = 0; i < m_vecComboWHB.size(); i++ )
			{
				if( m_vecComboWHB[i].dwPelsWidth == WHB.dwPelsWidth &&
					m_vecComboWHB[i].dwPelsHeight == WHB.dwPelsHeight &&
					m_vecComboWHB[i].dwBitsPerPel == WHB.dwBitsPerPel )
				{
					bContainWHB = TRUE;
					break;
				}
			}
			if( !bContainWHB )
				m_vecComboWHB.push_back( WHB );
		}

		//填充刷新率支持列表
		{
			BOOL bContainHz = FALSE;
			for( UINT i = 0; i < m_vecComboHz.size(); i++ )
			{
				if( m_vecComboHz[i] == itr->nRefreshRate )
				{
					bContainHz = TRUE;
					break;
				}
			}
			if( !bContainHz )
				m_vecComboHz.push_back( itr->nRefreshRate );
		}
	}
	

	//设置下拉菜单条目
	for( UINT i = 0; i < m_vecComboWHB.size(); i++ )
	{
		tstringstream stm;
		stm<<m_vecComboWHB[i].dwPelsWidth<<_T("x")<<m_vecComboWHB[i].dwPelsHeight<<_T(" ")<<m_vecComboWHB[i].dwBitsPerPel<<_T("位");
		m_pComb_WHB->GetListBox()->SetText( i, 0, stm.str().c_str() );
		m_pComb_WHB->GetListBox()->SetColWidth( i, m_pComb_WHB->GetSize().x );
	}

	//自动调整列表框大小
	tagPoint ptWHBSize = m_pComb_WHB->GetListBox()->GetSize();
	ptWHBSize.y = m_pComb_WHB->GetListBox()->GetRowHeight() * m_vecComboWHB.size();
	m_pComb_WHB->GetListBox()->SetResizable( TRUE );
	m_pComb_WHB->GetListBox()->Resize( ptWHBSize );
	m_pComb_WHB->GetListBox()->SetResizable( FALSE );

	for( UINT i = 0; i < m_vecComboHz.size(); i++ )
	{
		tstringstream stm;
		stm<<m_vecComboHz[i]<<_T("Hz");
		m_pComb_Hz->GetListBox()->SetText( i, 0, stm.str().c_str() );
		m_pComb_Hz->GetListBox()->SetColWidth( i, m_pComb_Hz->GetSize().x );
	}
	tagPoint ptHzSize = m_pComb_Hz->GetListBox()->GetSize();
	ptHzSize.y = m_pComb_Hz->GetListBox()->GetRowHeight() * m_vecComboHz.size();
	m_pComb_Hz->GetListBox()->SetResizable( TRUE );
	m_pComb_Hz->GetListBox()->Resize( ptHzSize );
	m_pComb_Hz->GetListBox()->SetResizable( FALSE );
}

UINT GameSettingsFrame::PixelFormatToBpp( Cool3D::EPixelFormat eFormat )
{
	switch( eFormat )
	{
	case EPF_R5G6B5:
	case EPF_A1R5G5B5:
		return 16;
	case EPF_R8G8B8:
		return 24;
	case EPF_A8R8G8B8:
	case EPF_X8R8G8B8:
		return 32;
	default:
		return 32;
	}
}

int GameSettingsFrame::CalcScrobarStepValue( const int nCurScroValue, const int nMaxStep )
{
	int nResult = 0;
	const int nSetpValue = ( SCROBAR_TOTAL - SCROBAR_VISIBLE ) / nMaxStep;
	if( nCurScroValue % nSetpValue <= nSetpValue / 2 )
	{
		nResult = nCurScroValue - nCurScroValue % nSetpValue;
	}
	else
	{
		nResult = nCurScroValue + ( nSetpValue - nCurScroValue % nSetpValue );
	}
	return nResult;
}

void GameSettingsFrame::ApplySettings()
{
	GameSet::Inst()->ApplyAll();
	LocalPlayer* pLP = RoleMgr::Inst()->GetLocalPlayer();
	if( P_VALID( pLP ) && pLP->IsAttributeInited() )
	{
		if( 0 != memcmp( &pLP->GetDisplaySet(), &m_curDisplaySet, sizeof(m_curDisplaySet) ) )
		{
			tagNC_RoleSetDisplay cmd;
			cmd.sDisplaySet = m_curDisplaySet;
			m_pSession->Send( &cmd );
		}
		//if( 0 != memcmp( &pLP->GetPersonalSet(), &m_curPersonalSet, sizeof(m_curPersonalSet) ) )
		//{
		//	tagNC_SetPersonalSet cmd;
		//	cmd.PersonalSet = m_curDisplaySet;
		//	m_pSession->Send( &cmd );
		//}
	}
}