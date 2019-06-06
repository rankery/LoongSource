#include "stdafx.h"
#include "GameSet.h"
#include "MapMgr.h"
#include "RoleMgr.h"
#include "EffectMgr.h"
#include "AudioSys.h"

GameSet::GameSet() :
m_dwPixelShaderVersion( 0 )
{
}

GameSet::~GameSet()
{
	Destroy();
}

BOOL GameSet::Init()
{
	_tcscpy( m_szFilename, _T("config\\ClientCfg.xml" ) );
	m_pCfgVars = "GameConfig";
	m_pCfgVars->SetSaveSupport(TRUE);

	// 为每个配置命名
	m_vecSettings.clear();
	m_vecSettings.resize( EGS_Num );

	// 画面设置
	m_vecSettings[EGS_PelsWidth].strName        = _T("PelsWidth");
	m_vecSettings[EGS_PelsHeight].strName       = _T("PelsHeight");
	m_vecSettings[EGS_BitsPerPel].strName       = _T("BitsPerPel");
	m_vecSettings[EGS_DisplayFrequency].strName = _T("DisplayFrequency");
	m_vecSettings[EGS_MultiSampleType].strName  = _T("MultiSampleType"),
	m_vecSettings[EGS_Windowed].strName         = _T("Windowed");
	m_vecSettings[EGS_VerSynch].strName         = _T("VerSynch");
	m_vecSettings[EGS_TerrainDetail].strName    = _T("TerrainDetail");
	m_vecSettings[EGS_NormalMap].strName        = _T("NormalMap");
	m_vecSettings[EGS_HWSkin].strName           = _T("HWSkin");
	m_vecSettings[EGS_TexDetail].strName        = _T("TexDetail");
	m_vecSettings[EGS_TerrainSpecular].strName  = _T("TerrainSpecular");
	m_vecSettings[EGS_FarView].strName          = _T("FarView");
	m_vecSettings[EGS_RoleView].strName         = _T("RoleView");
	m_vecSettings[EGS_EffectDetail].strName     = _T("EffectDetail");
	m_vecSettings[EGS_SceneDetail].strName      = _T("SceneDetal");
	m_vecSettings[EGS_WaterSpecular].strName    = _T("WaterSpeculart");
	m_vecSettings[EGS_WaterReflect].strName     = _T("WaterReflect");
	m_vecSettings[EGS_WaterRefract].strName     = _T("WaterRefract");
	m_vecSettings[EGS_Lambency].strName         = _T("Lambency");
	m_vecSettings[EGS_Weather].strName          = _T("Weather");
	m_vecSettings[EGS_SunHalo].strName          = _T("SunHalo");
	m_vecSettings[EGS_RoleShadow].strName       = _T("RoleShadow");
	m_vecSettings[EGS_DrawGrass].strName        = _T("DrawGrass");

	// 游戏设置
	m_vecSettings[EGS_Music].strName				 = _T("Music");
	m_vecSettings[EGS_Sound].strName				 = _T("SoundEffect");
	m_vecSettings[EGS_MusicVol].strName				 = _T("MusicVolume");
	m_vecSettings[EGS_SoundVol].strName				 = _T("SoundEffectVolume");
	m_vecSettings[EGS_CombatMusic].strName			 = _T("CombatMusic");
	m_vecSettings[EGS_ShowSelfName].strName			 = _T("ShowSelfName");
	m_vecSettings[EGS_ShowOtherPlayerName].strName	 = _T("ShowOtherPlayerName");
	m_vecSettings[EGS_ShowFactionName].strName		 = _T("ShowFactionName");
	m_vecSettings[EGS_ShowChatPop].strName			 = _T("ShowChatPop");
	m_vecSettings[EGS_ShowOtherPalyerTitle].strName	 = _T("ShowOtherPalyeTitle");
	m_vecSettings[EGS_ShowSkillNameFloatTip].strName = _T("ShowSkillNameFloatTip");
	m_vecSettings[EGS_ShowHPMPText].strName			 = _T("ShowHPMPText");
	m_vecSettings[EGS_ShowMonsterName].strName		 = _T("ShowMonsterName");

	// 小地图配置
	m_vecSettings[EGS_MM_ShowNatuRes].strName			= _T("MM_ShowNatuRes");
	m_vecSettings[EGS_MM_ShowNest].strName				= _T("MM_ShowNest");
	m_vecSettings[EGS_MM_ShowNormalMonster].strName		= _T("MM_ShowNormalMonster");
	m_vecSettings[EGS_MM_ShowBoss].strName				= _T("MM_ShowBoss");
	m_vecSettings[EGS_MM_ShowNormalNPC].strName			= _T("MM_ShowNormalNPC");
	m_vecSettings[EGS_MM_ShowAcceptQuestNPC].strName	= _T("MM_ShowAcceptQuestNPC");
	m_vecSettings[EGS_MM_ShowCompleteQuestNPC].strName	= _T("MM_ShowCompleteQuestNPC");
	m_vecSettings[EGS_MM_ShowTeammate].strName			= _T("MM_ShowTeammate");
	m_vecSettings[EGS_MM_ShowOtherPlayer].strName		= _T("MM_ShowOtherPlayer");

	// 其它
	m_vecSettings[EGS_Console].strName				 = _T("Console");
	m_vecSettings[EGS_GmConsle].strName				 = _T("GmConsole");
	m_vecSettings[EGS_QuestHelp].strName			 = _T("QuestHelp");
	m_vecSettings[EGS_MoralityWarning].strName		 = _T("MoralityWarning");
	
	for( size_t i = 0; i < m_vecSettings.size(); i++ )
		m_vecSettings[i].dwValue = 0;

	// 获取PS版本号
	DWORD dwPixelShaderVersion = 0;
	IDevice* pDev = Device();
	if( NULL == pDev )
		m_dwPixelShaderVersion = Kernel::Inst()->GetPixelShaderVersion();
	else
		m_dwPixelShaderVersion = pDev->CapsPixelShaderVersion();

	tstring strName = _T("value p_s");
	tstringstream stream;
	stream<<D3DSHADER_VERSION_MAJOR(m_dwPixelShaderVersion)<<_T(".")<<D3DSHADER_VERSION_MINOR(m_dwPixelShaderVersion);
	m_strPixelShaderVersion = stream.str();

	Load();
	ApplyAll();

	// FOR DEBUG?? (小地图配置界面没有此选项的开关，始终打开？？)
	m_vecSettings[EGS_MM_ShowNatuRes].dwValue = TRUE;

	return TRUE;
}


BOOL GameSet::Destroy()
{   
	m_vecSettings.clear();
	return TRUE;
}


DWORD GameSet::Get( const INT nIndex ) const
{
	ASSERT( nIndex >= 0 && nIndex < EGS_Num );
	if( nIndex >= (const INT)m_vecSettings.size() )
		return 0;
	return m_vecSettings[nIndex].dwValue;
}

BOOL GameSet::Set( const INT nIndex, const DWORD val )
{
	ASSERT( nIndex >= 0 && nIndex < EGS_Num );
	if( nIndex >= (const INT)m_vecSettings.size() )
		return FALSE;
	m_vecSettings[nIndex].dwValue = val;
	return TRUE;
}

BOOL GameSet::Load()
{
	BOOL bNeedReBuild = !m_pCfgVars->Load( NULL, m_szFilename );
	if( !bNeedReBuild )
	{
		const tstring strPSVersion = m_pCfgVars->GetString( _T("p_s"), NULL, _T("") );
		bNeedReBuild = ( strPSVersion != m_strPixelShaderVersion );
	}
	
	// 读取失败, 没有PS版本信息或与当前硬件不同，创建
	if( bNeedReBuild )
	{
		for( size_t i = 0; i < m_vecSettings.size(); i++ )
			m_vecSettings[i].dwValue = GetDefaultValue( i );
		SaveAll();
		return FALSE;
	}

	BOOL bNeedReSave = FALSE;
	for( size_t i = 0; i < m_vecSettings.size(); i++ )
	{
		DWORD dwValue = m_pCfgVars->GetDword( m_vecSettings[i].strName.c_str(), NULL, GT_INVALID );
		if( GT_INVALID == dwValue )
		{
			dwValue = GetDefaultValue( i );
			bNeedReSave = TRUE;
		}
		m_vecSettings[i].dwValue = dwValue;
	}
	if( bNeedReSave )
		SaveAll();

	if( m_vecSettings[EGS_FarView].dwValue < 0 )
		m_vecSettings[EGS_FarView].dwValue = 0;
	if( m_vecSettings[EGS_FarView].dwValue > MapMgr::MAX_VIEW_DIST )
		m_vecSettings[EGS_FarView].dwValue = MapMgr::MAX_VIEW_DIST;

	if( m_vecSettings[EGS_SceneDetail].dwValue < 0 )
		m_vecSettings[EGS_SceneDetail].dwValue = 0;
	if( m_vecSettings[EGS_SceneDetail].dwValue > MapMgr::MAX_VIEW_DIST )
		m_vecSettings[EGS_SceneDetail].dwValue = MapMgr::MAX_VIEW_DIST;

	if( m_vecSettings[EGS_EffectDetail].dwValue < 0 )
		m_vecSettings[EGS_EffectDetail].dwValue = 0;
	if( m_vecSettings[EGS_EffectDetail].dwValue > SGEffectNode::MAX_LOD )
		m_vecSettings[EGS_EffectDetail].dwValue = SGEffectNode::MAX_LOD;

	if( m_vecSettings[EGS_RoleView].dwValue < 0 )
		m_vecSettings[EGS_RoleView].dwValue = 0;
	if( m_vecSettings[EGS_RoleView].dwValue > RoleMgr::MAX_VIEW_DIST )
		m_vecSettings[EGS_RoleView].dwValue = RoleMgr::MAX_VIEW_DIST;

	if( m_vecSettings[EGS_MusicVol].dwValue	> MAX_MUSIC_VOLUME )
		m_vecSettings[EGS_MusicVol].dwValue	= MAX_MUSIC_VOLUME;
	if( m_vecSettings[EGS_SoundVol].dwValue	> MAX_SOUND_VOLUME )
		m_vecSettings[EGS_SoundVol].dwValue	= MAX_SOUND_VOLUME;

	return TRUE;
}

BOOL GameSet::SaveAll()
{
	for( UINT i = 0; i < m_vecSettings.size(); i++ )
	{
		tstring strName = _T("value ");
		strName += m_vecSettings[i].strName.c_str();
		m_pCfgVars->Set( m_vecSettings[i].dwValue, strName.c_str() );
	}
	m_pCfgVars->Set( m_strPixelShaderVersion.c_str(), _T("value p_s") );

	if( !m_pCfgVars->Save( m_szFilename ) )
	{
		ERR( _T("Fail to save game settings.\n") );
		return FALSE;
	}

	return TRUE;
}

BOOL GameSet::ApplySet( const INT nIndex ) const
{
	ASSERT( nIndex >= 0 && nIndex < EGS_Num );
	
	Cool3D::Cool3DOption::Inst()->SetOption( Cool3D::Cool3DOption::EO_LightMap,		   Cool3DOption::ELM_Open );
	switch( nIndex )
	{
		case EGS_TerrainDetail:		Cool3D::Cool3DOption::Inst()->SetOption( Cool3D::Cool3DOption::EO_TerrainDetail,   (int)m_vecSettings[EGS_TerrainDetail].dwValue ); break;
		case EGS_NormalMap:			Cool3D::Cool3DOption::Inst()->SetOption( Cool3D::Cool3DOption::EO_NormalMap,       (int)m_vecSettings[EGS_NormalMap].dwValue ); break;
		case EGS_HWSkin:			Cool3D::Cool3DOption::Inst()->SetOption( Cool3D::Cool3DOption::EO_HWSkin,          (int)m_vecSettings[EGS_HWSkin].dwValue ); break;
		case EGS_TexDetail:			Cool3D::Cool3DOption::Inst()->SetOption( Cool3D::Cool3DOption::EO_TexDetail,       (int)m_vecSettings[EGS_TexDetail].dwValue ); break;
		case EGS_TerrainSpecular:	Cool3D::Cool3DOption::Inst()->SetOption( Cool3D::Cool3DOption::EO_TerrainSpecular, (int)m_vecSettings[EGS_TerrainSpecular].dwValue ); break;
		case EGS_WaterReflect:		Cool3D::Cool3DOption::Inst()->SetOption( Cool3D::Cool3DOption::EO_WaterReflect,    (int)m_vecSettings[EGS_WaterReflect].dwValue ); break;
		case EGS_WaterSpecular:		Cool3D::Cool3DOption::Inst()->SetOption( Cool3D::Cool3DOption::EO_WaterSpecular,   (int)m_vecSettings[EGS_WaterSpecular].dwValue );	break;
		case EGS_WaterRefract:		Cool3D::Cool3DOption::Inst()->SetOption( Cool3D::Cool3DOption::EO_WaterRefract,    (int)m_vecSettings[EGS_WaterRefract].dwValue ); break;
		case EGS_DrawGrass:			Cool3D::Cool3DOption::Inst()->SetOption( Cool3D::Cool3DOption::EO_DrawGrass,	   (int)m_vecSettings[EGS_DrawGrass].dwValue ); break;
		case EGS_SunHalo:			MapMgr::Inst()->EnableSunFlare( TRUE == m_vecSettings[EGS_SunHalo].dwValue ); break;
		case EGS_FarView:			MapMgr::Inst()->SetViewDist( (int)m_vecSettings[EGS_FarView].dwValue ); break;
		case EGS_SceneDetail:		MapMgr::Inst()->SetSceneDetailLevel( (int)m_vecSettings[EGS_SceneDetail].dwValue ); break;
		case EGS_EffectDetail:		SGEffectNode::sUserLod = SGEffectNode::MAX_LOD - (int)m_vecSettings[EGS_EffectDetail].dwValue; break;
		case EGS_RoleView:			RoleMgr::Inst()->SetViewDistLevel( (int)m_vecSettings[EGS_RoleView].dwValue ); break;
		case EGS_RoleShadow:		RoleMgr::Inst()->EnalbeShadow( m_vecSettings[EGS_RoleShadow].dwValue ); break;
		case EGS_Lambency:
			{
				RenderSys* pRenderSys = Kernel::Inst()->GetRenderSys();
				if( NULL != pRenderSys )
					pRenderSys->EnablePostProcessBlur( TRUE == m_vecSettings[EGS_Lambency].dwValue );
			}
			break;
		case EGS_Music:				
			{
				Cool3D::Cool3DOption::Inst()->SetOption( Cool3D::Cool3DOption::EO_Music, (int)m_vecSettings[EGS_Music].dwValue );
				if( Cool3D::Cool3DOption::EM_Open == m_vecSettings[EGS_Music].dwValue )
					AudioSystem::Inst()->SetSystemMusicVolume( float( m_vecSettings[EGS_MusicVol].dwValue ) / MAX_MUSIC_VOLUME );
				else
					AudioSystem::Inst()->SetSystemMusicVolume( 0.0f );
				
			}
			break;
		case EGS_Sound:
			{
				Cool3D::Cool3DOption::Inst()->SetOption( Cool3D::Cool3DOption::EO_Sound, (int)m_vecSettings[EGS_Sound].dwValue );
				if( Cool3D::Cool3DOption::ES_Open == m_vecSettings[EGS_Sound].dwValue )
					AudioSystem::Inst()->SetSystemSoundVolume( float( m_vecSettings[EGS_SoundVol].dwValue ) / MAX_SOUND_VOLUME );
				else
					AudioSystem::Inst()->SetSystemSoundVolume( 0.0f );
			}
			break;
		case EGS_MusicVol:
			{
				if( Cool3D::Cool3DOption::EM_Open == m_vecSettings[EGS_Music].dwValue )
				{
					const float fMusicVolume = float( m_vecSettings[EGS_MusicVol].dwValue ) / MAX_MUSIC_VOLUME;
					AudioSystem::Inst()->SetSystemMusicVolume( fMusicVolume );
				}
			}
			break;
		case EGS_SoundVol:
			{
				if( Cool3D::Cool3DOption::ES_Open == m_vecSettings[EGS_Sound].dwValue )
				{
					const float fSoundVolume = float( m_vecSettings[EGS_SoundVol].dwValue ) / MAX_SOUND_VOLUME;
					AudioSystem::Inst()->SetSystemSoundVolume( fSoundVolume );
				}
			}
			break;
		case EGS_CombatMusic: TObjRef<AudioSys>()->EnableCombatMusic( m_vecSettings[EGS_CombatMusic].dwValue ); break;
		default:
			break;
	}
	return TRUE;
}

DWORD GameSet::GetDefaultValue( const INT nIndex ) const
{
	ASSERT( nIndex >= 0 && nIndex < EGS_Num );

	// 画面设置
	if( m_dwPixelShaderVersion < D3DPS_VERSION(2,0) )
	{
		switch( nIndex )
		{
			case EGS_TerrainDetail:		return Cool3D::Cool3DOption::ETD_TwoLayers;
			case EGS_NormalMap:			return Cool3D::Cool3DOption::ENM_None;
			case EGS_TerrainSpecular:	return Cool3D::Cool3DOption::ETS_None;
			case EGS_WaterReflect:		return Cool3D::Cool3DOption::EWR_None;
			case EGS_WaterSpecular:		return Cool3D::Cool3DOption::EWS_None;
			case EGS_WaterRefract:		return Cool3D::Cool3DOption::EWRR_None;
			case EGS_RoleShadow:		return FALSE;
			case EGS_DrawGrass:			return FALSE;
			case EGS_TexDetail:			return Cool3D::Cool3DOption::ETexDetail_Low;
			case EGS_FarView:			return 0;
			case EGS_SceneDetail:       return 0;
			case EGS_EffectDetail:		return 0;
			case EGS_RoleView:          return 0;
			case EGS_SunHalo:           return FALSE;
		}
	}
	else if( m_dwPixelShaderVersion < D3DPS_VERSION(3,0) )
	{
		switch( nIndex )
		{
			case EGS_TerrainDetail:		return Cool3D::Cool3DOption::ETD_TwoLayers;
			case EGS_NormalMap:			return Cool3D::Cool3DOption::ENM_None;
			case EGS_TerrainSpecular:	return Cool3D::Cool3DOption::ETS_Open;
			case EGS_WaterReflect:		return Cool3D::Cool3DOption::EWR_None;
			case EGS_WaterSpecular:		return Cool3D::Cool3DOption::EWS_None;
			case EGS_WaterRefract:		return Cool3D::Cool3DOption::EWRR_None;
			case EGS_RoleShadow:		return FALSE;
			case EGS_DrawGrass:			return TRUE;
			case EGS_TexDetail:			return Cool3D::Cool3DOption::ETexDetail_Mid;
			case EGS_FarView:			return MapMgr::MAX_VIEW_DIST / 2;
			case EGS_SceneDetail:       return MapMgr::MAX_VIEW_DIST / 2;
			case EGS_EffectDetail:		return SGEffectNode::MAX_LOD / 2;
			case EGS_RoleView:          return RoleMgr::MAX_VIEW_DIST / 2;
			case EGS_SunHalo:           return TRUE;
		}
	}
	else
	{
		switch( nIndex )
		{
			case EGS_TerrainDetail:		return Cool3D::Cool3DOption::ETD_MultiLayers;
			case EGS_NormalMap:			return Cool3D::Cool3DOption::ENM_Open;
			case EGS_TerrainSpecular:	return Cool3D::Cool3DOption::ETS_Open;
			case EGS_WaterReflect:		return Cool3D::Cool3DOption::EWR_High;
			case EGS_WaterSpecular:		return Cool3D::Cool3DOption::EWS_Open;
			case EGS_WaterRefract:		return Cool3D::Cool3DOption::EWRR_Open;
			case EGS_RoleShadow:		return TRUE;
			case EGS_DrawGrass:			return TRUE;
			case EGS_TexDetail:			return Cool3D::Cool3DOption::ETexDetail_High;
			case EGS_FarView:			return MapMgr::MAX_VIEW_DIST;
			case EGS_SceneDetail:       return MapMgr::MAX_VIEW_DIST;
			case EGS_EffectDetail:		return SGEffectNode::MAX_LOD;
			case EGS_RoleView:          return RoleMgr::MAX_VIEW_DIST;
			case EGS_SunHalo:           return TRUE;
		}
	}
	switch( nIndex )
	{
		case EGS_PelsWidth:					return ::GetSystemMetrics(SM_CXSCREEN);
		case EGS_PelsHeight:				return ::GetSystemMetrics(SM_CYSCREEN);
		case EGS_BitsPerPel:				return 32;
		case EGS_DisplayFrequency:			return 60;
		case EGS_Windowed:					return 3;
		case EGS_VerSynch:					return FALSE;

		case EGS_HWSkin:					return Cool3D::Cool3DOption::EHWS_Open;
		case EGS_Weather:					return FALSE;
		case EGS_MultiSampleType:			return EMT_None;
		case EGS_Lambency:					return FALSE;

		// 游戏设置
		case EGS_Music:						return Cool3D::Cool3DOption::EM_Open;
		case EGS_Sound:						return Cool3D::Cool3DOption::ES_Open;
		case EGS_MusicVol:					return MAX_MUSIC_VOLUME / 2;
		case EGS_SoundVol:					return MAX_SOUND_VOLUME / 2;
		case EGS_CombatMusic:				return TRUE;
		case EGS_ShowSelfName:				return TRUE;
		case EGS_ShowOtherPlayerName:		return TRUE;
		case EGS_ShowFactionName:			return TRUE;
		case EGS_ShowChatPop:				return TRUE;
		case EGS_ShowOtherPalyerTitle:		return TRUE;
		case EGS_ShowSkillNameFloatTip:		return TRUE;
		case EGS_ShowHPMPText:				return TRUE;
		case EGS_ShowMonsterName:			return TRUE;

		// 小地图配置
		case EGS_MM_ShowNatuRes:			return FALSE;
		case EGS_MM_ShowNest:				return FALSE;
		case EGS_MM_ShowNormalMonster:		return FALSE;
		case EGS_MM_ShowNormalNPC:			return FALSE;
		case EGS_MM_ShowBoss:				return FALSE;
		case EGS_MM_ShowAcceptQuestNPC:		return TRUE;
		case EGS_MM_ShowCompleteQuestNPC:	return TRUE;
		case EGS_MM_ShowTeammate:			return TRUE;
		case EGS_MM_ShowOtherPlayer:		return FALSE;

		// 其它
		case EGS_Console:					return FALSE;
		case EGS_GmConsle:					return FALSE;
	}

	ASSERT(0);
	return GT_INVALID;
}

VOID GameSet::ApplyAll()
{
	for( size_t i = 0; i < m_vecSettings.size(); i++ )
		ApplySet( i );
}

VOID GameSet::BuildDefaultGraphicSettings()
{
	for( int i = EGS_Gra_Start; i <= EGS_Gra_End; i++ )
		m_vecSettings[i].dwValue = GetDefaultValue( i );
}

BOOL GameSet::SaveMiniMapSettings()
{
	for( int i = EGS_MM_Start; i<= EGS_MM_End; i++ )
	{
		tstring strName = _T("value ");
		strName += m_vecSettings[i].strName.c_str();
		m_pCfgVars->Set( m_vecSettings[i].dwValue, strName.c_str() );
	}
	if( !m_pCfgVars->Save( m_szFilename ) )
	{
		ERR( _T("Fail to save minimap settings.\n") );
		return FALSE;
	}
	return TRUE;
}

VOID GameSet::SaveQuestHelpSet()
{
	tstring strName = _T("value ");
	strName += m_vecSettings[EGS_QuestHelp].strName.c_str();
	m_pCfgVars->Set( m_vecSettings[EGS_QuestHelp].dwValue, strName.c_str() );
	if( !m_pCfgVars->Save( m_szFilename ) )
	{
		ERR( _T("Fail to save minimap settings.\n") );
	}
}

VOID GameSet::SaveMorality()
{
	tstring strName = _T("value ");
	strName += m_vecSettings[EGS_MoralityWarning].strName.c_str();
	m_pCfgVars->Set( m_vecSettings[EGS_MoralityWarning].dwValue, strName.c_str() );
	if( !m_pCfgVars->Save( m_szFilename ) )
	{
		ERR( _T("Fail to save minimap settings.\n") );
	}
}