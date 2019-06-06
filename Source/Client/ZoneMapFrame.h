#pragma once


#include "MapLogicData.h"

class ZoneMapStatic;
struct tagLeaveMapAreaEvent;
struct tagEnterMapAreaEvent;
class AppliedTree;
struct tagMouseMoveEvent;
struct tagSetFlagEvent;
struct tagOpenCloseMapWndEvent;
struct tagOpenMapWndEvent;
struct tagMouseMoveFinishEvent;

/** \class ZoneMapFrame
    \brief 区域地图、世界地图界面
*/
class ZoneMapFrame : public GameFrame
{
private:
	struct tagZoneMapShow
	{
		DWORD 				dwMapID;
		tstring             strShowName;
		tagZoneMapShow() :  dwMapID( GT_INVALID ) {}
	};
	struct tagMarkIndex
	{
		int nType;
		int nMask;
		tagMarkIndex() : nType( 0 ), nMask( 0 ){}
		tagMarkIndex( const int type, const int mask ) : nType( type ), nMask( mask ) {}
	};
public:
	ZoneMapFrame();
	~ZoneMapFrame();

	// dwParam 0-区域地图 1-世界地图
	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();
	virtual BOOL EscCancel();

	VOID SwithToWorldMap();
	VOID SwithToZoneMap( const DWORD dwMapID );
private:
	VOID Open( BOOL bWorldMap );
	VOID Close();
	BOOL GUIEventHandler(tagGUIEvent* pEvent);
	VOID SafeDestroyUI();
	VOID ResetMapData( const DWORD dwMapID, const int nAreaID );
	VOID SetShowMap( const DWORD dwMapID );
	VOID ReBuidAreaList( DWORD dwMapID );
	int  GetCurAreaIndex();
	VOID ReBuidMarksTree( const DWORD dwMapID );
	VOID OnWorldMapClick( GUIWnd* pWnd );
	/** \游戏事件响应
	*/
	DWORD OnOpenMap(tagGameEvent*);
	DWORD OnLeaveMapArea( tagLeaveMapAreaEvent* pEvent );
	DWORD OnEnterMapArea( tagEnterMapAreaEvent* pEvent );
	DWORD OnSetFlagEvent( tagSetFlagEvent* pEvent );
	DWORD OnMouseMoveEvent( tagMouseMoveEvent* pEvent );
	DWORD OnOpenCloseMapWndEvent( tagOpenCloseMapWndEvent* pEvent );
	DWORD OnOpenMapWndEvent( tagOpenMapWndEvent* pEvent );
	DWORD OnMouseMoveFinishEvent( tagMouseMoveFinishEvent* pEvent );
private:
	TSFPTrunk<ZoneMapFrame>	        m_Trunk;
	TObjRef<GUISystem>				m_pGUI;
	GUIWnd*                         m_pWnd;               // 主窗口
	GUIButton*                      m_pBn_Close;          // 关闭按钮
	GUIStatic*                      m_pSt_WoldMap;        // 世界地图控件
	ZoneMapStatic*                  m_pSt_ZoneMap;        // 区域地图控件
	GUIEditBox*                     m_pEb_ShowName;       // 当前显示的地图或区域名称
	GUIListBox*                     m_pLb_MapList;        // 地图名称列表框
	GUIButton*                      m_pBn_Back;           // 返回按钮
	GUIButton*                      m_pBn_Task;           // 可接任务按钮
	AppliedTree*                    m_pTr_Mark;           // 地图标记树

	vector<tagMapShowAreaData>      m_vecZoneMapArea;     // 城市地图列表（用于区域地图）
	vector<tagZoneMapShow>          m_vecZoneMapShow;     // 区域地图列表（用于世界地图）
	int                             m_nShowAreaIndex;     // 当前显示的区域m_vecZoneMapArea中的索引
	map<DWORD, tagMarkIndex>        m_mapMarkIndex;       // KEY:ID in AppliedTree
};

