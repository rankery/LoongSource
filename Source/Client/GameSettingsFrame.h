#pragma once

struct tagComboWHB                                              //分辨率宽，高，位深
{
	DWORD dwPelsWidth;
	DWORD dwPelsHeight;
	DWORD dwBitsPerPel;
	Cool3D::EPixelFormat eFormat;   
	tagComboWHB( DWORD w, DWORD h, DWORD bpp, Cool3D::EPixelFormat fmt ) : 
	         dwPelsWidth( w ), dwPelsHeight( h ), dwBitsPerPel( bpp ), eFormat( fmt ) { }
};

struct tagMultiSampleCfg
{
	EMultisampleType eLevel;
	tstring strShowText;
	tagMultiSampleCfg(){}
	tagMultiSampleCfg( EMultisampleType level, tstring strText ) : eLevel( level ), strShowText( strText ){}
};

/** \class GameSettingsFrame
    \brief 游戏配置界面
*/
class GameSettingsFrame : public GameFrame
{
public:
	GameSettingsFrame();
	~GameSettingsFrame();

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual BOOL EscCancel();
private:
	enum { SCROBAR_TOTAL = 102 , SCROBAR_VISIBLE = 2 };

	TSFPTrunk<GameSettingsFrame>	m_Trunk;
	TObjRef<GUISystem>				m_pGUI;

	GUIWnd*							m_pWnd;						  // 主窗口
	GUIWnd*							m_pPanel;					  // 底版
	GUIButton*                      m_pBn_Close;                  // 关闭按钮
	GUIPushButton*                  m_pBn_GraphicSet;             // 画面配置按钮
	GUIPushButton*                  m_pBn_GameSet;                // 游戏配置按钮
	GUIWnd*                         m_pPanel_GraphicSet;          // 画面配置面板
	GUIWnd*                         m_pPanel_GameSet;			  // 游戏配置面板
	GUIButton*                      m_pBn_AutoCheck;              // 自动检测按钮
	GUIButton*                      m_pBn_OK;                     // 确定按钮
	GUIButton*                      m_pBn_Cancel;                 // 取消按钮
	GUIButton*                      m_pBn_Apply;                  // 应用按钮

	// 画面设置
	GUIComboBox*                    m_pComb_WHB;                  // 分辨率框
	GUIComboBox*                    m_pComb_Hz;                   // 刷新率框
	GUIComboBox*                    m_pComb_MultiSample;          // 抗锯齿框
	GUIPushButton*                  m_pBn_Windowed_Off;           // 窗口模式关
	GUIPushButton*                  m_pBn_Windowed_On;            // 窗口模式开
	GUIPushButton*                  m_pBn_VerSynch_Off;           // 垂直同步关
	GUIPushButton*                  m_pBn_VerSynch_On;            // 垂直同步开
	GUIPushButton*                  m_pBn_TerrainDetail_Low;      // 地表精度低
	GUIPushButton*                  m_pBn_TerrainDetail_High;     // 地表精度高
	GUIPushButton*                  m_pBn_NormalMap_Off;          // 法线贴图关
	GUIPushButton*                  m_pBn_NormalMap_On;           // 法线贴图开
	GUIPushButton*                  m_pBn_HWSkin_Off;             // GPU动画关
	GUIPushButton*                  m_pBn_HWSkin_On;              // GPU动画开
	GUIPushButton*                  m_pBn_TexDetail_Low;          // 贴图精度低
	GUIPushButton*                  m_pBn_TexDetail_Mid;          // 贴图精度中
	GUIPushButton*                  m_pBn_TexDetail_High;         // 贴图精度高
	GUIPushButton*                  m_pBn_TerrainSpecular_Off;    // 地形高亮关
	GUIPushButton*                  m_pBn_TerrainSpecular_On;     // 地形高亮开
	GUIScrollBar*                   m_pScro_FarView;              // 远景视野滚动条
	GUIScrollBar*                   m_pScro_RoleView;             // 角色视野滚动条
	GUIScrollBar*                   m_pScro_EffectDetail;         // 特效细节滚动条
	GUIScrollBar*                   m_pScro_SceneDetail;          // 环境细节滚动条
	GUIPushButton*                  m_pBn_WaterSpecular_Off;      // 水面高亮关
	GUIPushButton*                  m_pBn_WaterSpecular_On;       // 水面高亮开
	GUIPushButton*                  m_pBn_WaterReflect_Off;       // 水面反射关
	GUIPushButton*                  m_pBn_WaterReflect_Mid;       // 水面反射中
	GUIPushButton*                  m_pBn_WaterReflect_High;      // 水面反射高
	GUIPushButton*                  m_pBn_WaterRefract_Off;       // 水面折射关
	GUIPushButton*                  m_pBn_WaterRefract_On;        // 水面折射开
	GUIPushButton*                  m_pBn_Lambency_Off;           // 柔光效果关
	GUIPushButton*                  m_pBn_Lambency_On;            // 柔光效果开
	GUIPushButton*                  m_pBn_Weather_Off;            // 天气效果关
	GUIPushButton*                  m_pBn_Weather_On;             // 天气效果开
	GUIPushButton*                  m_pBn_SunHalo_Off;            // 太阳光晕关
	GUIPushButton*                  m_pBn_SunHalo_On;             // 太阳光晕开
	GUIPushButton*                  m_pBn_DrawGrass_Off;          // 花草显示关
	GUIPushButton*                  m_pBn_DrawGrass_On;           // 花草显示开
	GUIPushButton*                  m_pBn_RoleShadow_Off;         // 角色阴影关
	GUIPushButton*                  m_pBn_RoleShadow_On;          // 角色阴影开

	// 游戏设置
	GUIPushButton*                  m_pBn_Music_Off;			  // 音乐开关
	GUIPushButton*					m_pBn_Music_On;
	GUIScrollBar*					m_pScro_MusicVolume;          // 音乐音量
	GUIPushButton*                  m_pBn_Sound_Off;			  // 音效开关
	GUIPushButton*					m_pBn_Sound_On;
	GUIScrollBar*					m_pScro_SoundVolume;          // 音效音量
	GUIPushButton*                  m_pBn_CombatMusic_Off;		  // 战斗音乐开关
	GUIPushButton*                  m_pBn_CombatMusic_On;
	GUIPushButton*                  m_pBn_AcceptTeamInvite_Off;   // 是否接受组队邀请
	GUIPushButton*                  m_pBn_AcceptTeamInvite_On;
	GUIPushButton*                  m_pBn_AcceptFactionInvite_Off;// 是否接受帮派邀请
	GUIPushButton*                  m_pBn_AcceptFactionInvite_On;
	GUIPushButton*                  m_pBn_AllowFriendAdd_Off;	  // 是否接受好友邀请
	GUIPushButton*                  m_pBn_AllowFriendAdd_On;
	GUIPushButton*                  m_pBn_AcceptTradeRequest_Off; // 是否接交易邀请
	GUIPushButton*                  m_pBn_AcceptTradeRequest_On;
	GUIPushButton*                  m_pBn_SelfInfoOpenType_Private;// 个人信息保密
	GUIPushButton*                  m_pBn_SelfInfoOpenType_Friend; // 仅好友可见
	GUIPushButton*                  m_pBn_SelfInfoOpenType_Public; // 所有人可见
	GUIPushButton*                  m_pBn_ShowSelfName;           // 显示自身名字
	GUIPushButton*                  m_pBn_ShowOtherPlayerName;    // 显示其他玩家名字
	GUIPushButton*                  m_pBn_ShowFactionName;        // 显示帮派名字
	GUIPushButton*                  m_pBn_ShowChatPop;            // 显示聊天泡泡
	GUIPushButton*                  m_pBn_ShowHat;                // 显示帽子
	GUIPushButton*                  m_pBn_ShowMantle;             // 显示披风
	GUIPushButton*                  m_pBn_ShowOtherPalyeTitle;    // 显示其他玩家称号
	GUIPushButton*                  m_pBn_ShowSkillNameFloatTip;  // 显示技能名称飘字
	GUIPushButton*                  m_pBn_ShowHPMPText;           // 显示HP MP文字
	GUIPushButton*                  m_pBn_ShowMonsterName;        // 显示NPC 怪物名字

	BOOL GUIEventHandler(tagGUIEvent* pEvent);				      // 事件响应回调函数

	std::vector<tagComboWHB>        m_vecComboWHB;                // 分辨率下拉框列表
	std::vector<DWORD>              m_vecComboHz;                 // 刷新率下拉框列表
	std::vector<tagMultiSampleCfg>  m_vecComboMultiSample;        // 抗锯齿下拉框列表
	INT                             m_nSelectWHB;                 // 当前选择的分辨率索引
	INT                             m_nSelectHz;                  // 当前选择的刷新率索引
	INT                             m_nSelectMultiSample;         // 当前选择的抗锯齿索引

	tagDWPlayerPersonalSet          m_curPersonalSet;			  // 当前的个人设置
	tagDisplaySet					m_curDisplaySet;              // 当前的装备显示设置
	TObjRef<NetSession>				m_pSession;
	DWORD							m_dwLastApplyTime;
private:
	VOID SafeDestroyUI();
	VOID EnableControls();                                        // 设置控件是否可操作
	BOOL SettingsToUI();                                          // 根据配置信息设置UI状态
	BOOL UIToSettings();                                          // 根据UI状态修改配置信息
	VOID BuildDisplayModeList();                                  // 建立显示模式列表
	UINT PixelFormatToBpp( Cool3D::EPixelFormat eFormat );        // 由像素格式得到位深
	int CalcScrobarStepValue( const int nCurScroValue, const int nMaxStep );
	void ApplySettings();										  // 应用设置
};

