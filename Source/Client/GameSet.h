#pragma once


struct tagSetting
{
	tstring strName;
	DWORD   dwValue;
};

enum EGameSettings
{
	// 画面设置
	EGS_Gra_Start,
	EGS_PelsWidth = EGS_Gra_Start,			// 分辨率宽
	EGS_PelsHeight,                         // 分辨率高
	EGS_BitsPerPel,                         // 位深
	EGS_DisplayFrequency,                   // 刷新率
	EGS_MultiSampleType,                    // 抗锯齿
	EGS_Windowed,                           // 窗口模式
	EGS_VerSynch,                           // 垂直同步
	EGS_TerrainDetail,                      // 地表精度
	EGS_NormalMap,                          // 法线贴图
	EGS_HWSkin,                             // GPU动画
	EGS_TexDetail,                          // 贴图精度
	EGS_TerrainSpecular,                    // 地形高亮
	EGS_FarView,                            // 远景视野
	EGS_RoleView,                           // 角色视野
	EGS_EffectDetail,                       // 特效细节
	EGS_SceneDetail,                        // 环境细节
	EGS_WaterSpecular,                      // 水面高亮
	EGS_WaterReflect,                       // 水面反射
	EGS_WaterRefract,                       // 水面折射
	EGS_Lambency,                           // 柔光效果
	EGS_Weather,                            // 天气效果
	EGS_SunHalo,                            // 太阳光晕
	EGS_RoleShadow,                         // 角色阴影
	EGS_DrawGrass,                          // 花草显示
	EGS_Gra_End = EGS_DrawGrass,

	// 游戏设置
	EGS_Game_Start,
	EGS_Music = EGS_Game_Start,				// 音乐开关
	EGS_Sound,								// 音效开关
	EGS_MusicVol,							// 音乐音量
	EGS_SoundVol,							// 音效音量
	EGS_CombatMusic,						// 战斗音乐
	EGS_ShowSelfName,						// 显示自身名字
	EGS_ShowOtherPlayerName,				// 显示其他玩家名字
	EGS_ShowFactionName,					// 显示帮派名字
	EGS_ShowChatPop,						// 显示聊天泡泡
	EGS_ShowOtherPalyerTitle,				// 显示其他玩家称号
	EGS_ShowSkillNameFloatTip,				// 显示技能名称飘字
	EGS_ShowHPMPText,						// 显示HP MP文字
	EGS_ShowMonsterName,					// 显示怪物名字
	EGS_Game_End = EGS_ShowMonsterName,

	// 小地图配置
	EGS_MM_Start,
	EGS_MM_ShowNatuRes = EGS_MM_Start,  	// 采集点
	EGS_MM_ShowNest,						// 怪物巢穴
	EGS_MM_ShowNormalMonster,				// 怪物
	EGS_MM_ShowBoss,						// BOSS
	EGS_MM_ShowNormalNPC,					// NPC
	EGS_MM_ShowAcceptQuestNPC,				// 可接任务NPC
	EGS_MM_ShowCompleteQuestNPC,			// 可交任务NPC
	EGS_MM_ShowTeammate,					// 队友
	EGS_MM_ShowOtherPlayer,					// 其他玩家
	EGS_MM_End = EGS_MM_ShowOtherPlayer,

	// 其它
	EGS_Console,							// 控制台
	EGS_GmConsle,							// GM控制台
	EGS_QuestHelp,							// 任务帮助
	EGS_MoralityWarning,					// 道德警告
	EGS_Num
};


/** \class GameSet
    \brief 游戏配置，用于管理游戏配置数据
*/
class GameSet
{
public:
	enum { MAX_MUSIC_VOLUME = 100, MAX_SOUND_VOLUME = 100 };
public:
	/** \获得单件实例指针
	*/
	static GameSet* Inst() { static GameSet gs; return &gs; }
	/** \初始化并加载配置
	*/
	BOOL	Init();
	BOOL	Destroy();
	/** \获取配置
	*/
	DWORD	Get( const INT nIndex ) const;
	/** \修改配置
	*/
	BOOL	Set( const INT nIndex, const DWORD val );
	/** \应用所有设置
	*/
	VOID	ApplyAll();
	/** \保存所有设置
	*/
	BOOL	SaveAll();
	/** \生成默认画面设置
	*/
	VOID	BuildDefaultGraphicSettings();
	/** \保存小地图配置
	*/
	BOOL    SaveMiniMapSettings();
	VOID	SaveQuestHelpSet();
	VOID	SaveMorality();
private:
	BOOL	Load();
	BOOL	ApplySet( const INT nIndex ) const;
	DWORD	GetDefaultValue( const INT nIndex ) const;
private:
	TObjRef<VarContainer>           m_pCfgVars;                   //GameConfig变量容器
	std::vector<tagSetting>         m_vecSettings;                //游戏配置数组
	TCHAR                           m_szFilename[MAX_PATH];       //配置文件名
	DWORD                           m_dwPixelShaderVersion;
	tstring                         m_strPixelShaderVersion;
private:
	GameSet();
	~GameSet();
};