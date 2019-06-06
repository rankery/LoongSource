#pragma once

class MiniMapStatic;
struct tagLeaveMapAreaEvent;
struct tagEnterMapAreaEvent;
struct tagSetFlagEvent;
struct tagMouseMoveFinishEvent;

/** \class MiniMapFrame
    \brief 小地图界面
*/
class MiniMapFrame : public GameFrame
{
public:
	MiniMapFrame();
	~MiniMapFrame();

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();
private:
	VOID SafeDestroyUI();
	BOOL OnWnd_MaxSize(tagGUIEvent* pEvent);				      //最大化窗口事件响应函数
	BOOL OnWnd_MiniSize(tagGUIEvent* pEvent);				      //最小化窗口事件响应函数
	BOOL OnWnd_Set(tagGUIEvent* pEvent);				          //小地图配置窗口事件响应函数
	VOID ResetMapData();
	/** \游戏事件响应
	*/
	DWORD OnOpenMap(tagGameEvent*);
	DWORD OnLeaveMapArea( tagLeaveMapAreaEvent* pEvent );
	DWORD OnEnterMapArea( tagEnterMapAreaEvent* pEvent );
	DWORD OnSetFlagEvent( tagSetFlagEvent* pEvent );
	DWORD OnMouseMoveFinishEvent( tagMouseMoveFinishEvent* pEvent );
	/** \热键相应函数
	*/
	DWORD OnZoomIn_HotKey( DWORD dwID, BOOL bDown );
	DWORD OnZoomOut_HotKey( DWORD dwID, BOOL bDown );

	INT EGStoEMME( const INT nEGS ) const;
private:
	TSFPTrunk<MiniMapFrame>	        m_Trunk;
	TObjRef<GameInputMap>			m_pKeyMap;
	TObjRef<GUISystem>				m_pGUI;

	GUIWnd*                         m_pWnd_MaxSize;               //小地图最大化窗口
	MiniMapStatic*                  m_pMSt_MiniMap;               //小地图控件
	GUIStatic*                      m_pSt_MapName;                //地图名字标签
	GUIStatic*                      m_pSt_PosX;                   //玩家X坐标标签
	GUIStatic*                      m_pSt_PosZ;                   //玩家Z坐标标签
	GUIButton*                      m_pBn_MiniSize;               //最小化按钮
	GUIButton*                      m_pBn_Setting;                //小地图配置按钮
	GUIButton*                      m_pBn_AreaMap;                //区域地图按钮
	GUIButton*                      m_pBn_TaskQuery;              //活动任务查询按钮
	GUIButton*                      m_pBn_TaskTrace;              //任务追踪界面按钮
	GUIButton*                      m_pBn_ZoomIn;                 //放大按钮
	GUIButton*                      m_pBn_ZoomOut;                //缩小按钮
	GUIButton*						m_pBn_VipStall;				  //VIP摊位
	GUIButton*						m_pBn_OnlineTip;			  //上线提示
	GUIPushButton*					m_pPBtn_ContinuteSkill;		  //连接技开关
	GUIButton*                      m_pBn_StyleAction;            //个性动作按钮
	GUIButton*						m_pBtnQuestHelp;			  // 任务帮助

	GUIWnd*                         m_pWnd_MiniSize;              //小地图最小化窗口
	GUIStatic*                      m_pSt_MapName_MiniSize;       //最小化窗口中的地图名字标签
	GUIStatic*                      m_pSt_PosX_MiniSize;          //最小化窗口中的X坐标标签
	GUIStatic*                      m_pSt_PosZ_MiniSize;          //最小化窗口中的Z坐标标签
	GUIButton*                      m_pBn_MaxSize;                //最大化按钮

	GUIWnd*                         m_pWnd_Set;                   //小地图配置窗口
	GUIPushButton*                  m_pBn_EleShow[EMME_Num];      //是否显示按钮
	GUIButton*                      m_pBn_Ok_Set;                 //确定
	GUIButton*                      m_pBn_Cancel_Set;             //取消
	GUIButton*                      m_pBn_Close_Set;              //关闭



	tagPoint                        m_ptLocalPlayerPos;
};

