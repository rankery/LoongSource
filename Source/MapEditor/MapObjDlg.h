#pragma once


// CMapObjDlg 对话框
#include "resource.h"
#include "..\Cool3D\EditorSupport\SimpleView.h"
#include ".\editdlg.h"
#include ".\MapPropertyList.h"
#include "afxwin.h"
#include "afxcmn.h"
#include ".\AdaptiveDlg.h"

struct tagResGather;
class ResourceGather;
#define LOCK_COL 2
#define FLAG_COL 3
//物体类型定义
enum GMAPOBJ_TYPE
{
	GMAPOBJ_Npc ,
	GMAPOBJ_StaticObj,
	GMAPOBJ_DynamicBlock,	//by add xtian 2008-8-12
	GMAPOBJ_WAYPOINT,
	GMAPOBJ_Trigger, 
	GMAPOBJ_Sound,
	GMAPOBJ_PointLight,
	GMAPOBJ_MapRect,		//by add xtian 2008-5-12
	GMAPOBJ_PathPoint,		//by add xtian 2008-8-6
	GMAPOBJ_SpawnPoint,		//by add xtian 2008-8-11
	GMAPOBJ_EventTrigger,	//by add xtian 2008-9-10
	GMAPOBJ_NPCNavStartPoint, //by add xtian 2008-10-20
	GMAPOBJ_PlayerNavStartPoint, //by add xtian 2008-10-20
	GMAPOBJ_MapDoor,
	GMAPOBJ_MapCarrier,		//by add xtian 2009-3-13
	GMAPOBJ_MapTriggerEffect,//by add xtian 2009-7-21

	GMAPOBJ_NUM,
};

enum Img_Type				//插入图片类型
{
	EImg_Null = 0,
	EImg_Lock,
	EImg_Flag,
};

class CMapObjDlg : public CDialog
{
	DECLARE_DYNAMIC(CMapObjDlg)

public:
	CMapObjDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMapObjDlg();

    // 对话框数据
	enum { IDD = IDD_MAPOBJDLG };

	CListCtrl		 m_objList;     //对象列表框
	CComboBox		 m_objTypeList; //对象类型下拉框
	CMapPropertyList m_propList;    //属性框
	CButton          m_NEWMAP;      //创建新地图
	HACCEL			m_hAccelTable;	//快捷键

	void AttachParent(CWnd* pWnd)
	{
		ASSERT(m_pParent == NULL);
		m_pParent = pWnd;
	}
	MapObjEdit* GetPreCreate() const    { return this->m_pPreCreated;}
	void SetPreCreate(MapObjEdit* pEdit){ m_pPreCreated = pEdit;     } 
	virtual BOOL PreTranslateMessage(MSG* pMsg);		//快捷键消息处理函数

protected:
	
	friend class     CPlugMgr;
	SimpleView		*m_Cool3DView;
	CWnd			*m_pParent;
	MapObjEdit     *m_pPreCreated;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DWORD m_dwMapIndex;      //编辑地图ID
	bool m_bIsBuildWayPoint; //是否建立导航点
	bool m_bIsBuildTrigger;  //是否建立触发器
	bool m_bIsBuildSound;    //是否建立音效
	CString m_szStaticFileOpenType;//文件打开类型
	int m_PreObjListSelectedIndex;//用于控制键盘选择objlist
	bool m_bIsBuildPointLight;	//是否建立点光源
	bool m_bCopyBuildPointLgt;	//是否复制建立点光源
	bool m_bIsBuildMapRect;		//是否建立地图区域
	bool m_bIsBuildPathPoint;	//是否建立寻路航点
	bool m_bIsBuildSpawnPoint;	//是否建立刷怪点
	bool m_bIsBuildNpcNav;		//是否建立NPC格子导航图生成起点
	bool m_bIsBuildPlayerNav;	//是否建立玩家寻路导航图生成起点


	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);

	//鼠标操作
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

	//按钮操作
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedDel();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedLoad();
	afx_msg void OnBnClickedCommit();
	afx_msg void OnBnClickedButtonNewmap();
	

    //下拉框
	afx_msg void OnCbnSelchangeobjtypelist();
	afx_msg void OnNMClickobjlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkobjlist(NMHDR *pNMHDR, LRESULT *pResult);	//双击使摄像机找到地图对象
	afx_msg void OnOK();		//在对象属性修改后按回车,会触发应用按钮事件	
	//筛选按钮
	afx_msg void OnBnClickedButtonFilter();	

	//function
	void ResetAllInfo();

    //--设置/获得是否建立导航点标识
	bool GetIsBuildWayPoint()                       {   return  m_bIsBuildWayPoint;          }  
	void SetIsBuildWayPoint(bool bIsBuildWayPoint)  {   m_bIsBuildWayPoint=bIsBuildWayPoint; }
    //--设置/获得是否建立触发器标识
	bool GetIsBuildTrigger()                        {   return  m_bIsBuildTrigger;           }  
	void SetIsBuildTrigger(bool bIsBuildTrigger)    {   m_bIsBuildTrigger=bIsBuildTrigger;   }
	//--设置／获得是否建立音效
    bool GetIsBuildSound()                          {   return  m_bIsBuildSound;             }  
	void SetIsBuildSound(bool bIsBuildSound)        {   m_bIsBuildSound=bIsBuildSound;       }         
	//--得到当前是否锁定对象
	bool GetIsLockOnObject();
	//--设置/获得是否建立点光源
	bool GetIsBuildPointLight()						{	return m_bIsBuildPointLight;		}
	void SetIsBuildPointLight(bool bIsBuildPointLight){	m_bIsBuildPointLight = bIsBuildPointLight;}
	//--设置/获得是否建立地图区域
	bool GetIsBuildMapRect()						{	return m_bIsBuildMapRect; }
	void SetIsBuildMapRect(bool bIsBuildMapRect)	{	m_bIsBuildMapRect = bIsBuildMapRect; }
	//--设置/获得是否拷贝点光源
	bool GetCopyBuildPointLgt()						{	return m_bCopyBuildPointLgt; }
	void SetCopyBuildPointLgt(bool bVal)			{	m_bCopyBuildPointLgt = bVal; }
	//--设置/获得是否建立寻路导航点
	bool GetIsBuildPathPoint()						{	return m_bIsBuildPathPoint; }
	void SetIsBuildPathPoint(bool bVal)				{	m_bIsBuildPathPoint = bVal; }
	//--设置/获得是否建立刷怪点
	bool GetIsBuildSpawnPoint()						{	return m_bIsBuildSpawnPoint; }
	void SetIsBuildSpawnPoint(bool bVal)			{	m_bIsBuildSpawnPoint = bVal; }
	//--
	bool GetIsBuildNPCNavStartPoint()				{	return m_bIsBuildNpcNav; }
	void SetIsBuildNPCNavStartPoint(bool bVal)		{	m_bIsBuildNpcNav = bVal; }
	//--
	bool GetIsBuildPlayerNavStartPoint()			{	return m_bIsBuildPlayerNav; }
	void SetIsBuildPlayerNavStartPoint(bool bVal)	{	m_bIsBuildPlayerNav = bVal; }

	//--显示物体列表
	void DisplayList(MapObjEdit::EType type);
	//--显示导航点列表
	void DisplayWayPointList();
	//--显示触发器列表
	void DisplayTriggerList();
	//--显示音效列表
	void DisplaySound();
	//--显示点光源列表
	void DisplayPointLightList();
	//--显示地图区域列表
	void DisplayMapRectList();
	//--显示寻路导航点
	void DisplayPathPoint();
	//--显示刷怪点
	void DisplaySpawnPoint();
	//--显示事件触发器
	void DisplayEventTrigger();
	//--
	void DisplayNPCNav();
	//--
	void DisplayPlayerNav();

	//--更新物体属性列表
	void UpdateList(MapObjEdit* pSelected);
	//--更新导航点属性列表
	void UpdatePropListForWayPoint(tagMapWayPoint* pWayPoint);
	//--更新触发器属性列表
	void UpdatePropListForTrigger(tagMapTrigger* pTrigger);
	//--更新音效列表
	void UpdatePropListForSound(tagMapSound* pSound);
	//--更新点光源列表
	void UpdatePropListForPointLight(MapObjEdit* pEdit);
	//--更新地图区域列表
	void UpdatePropListForMapRect(MapObjEdit* pEdit);
	//--更新寻路导航点
	void UpdatePropListForPathPoint(MapObjEdit* pEdit);
	//--更新刷怪点
	void UpdatePropListForSpawnPoint(MapObjEdit* pEdit);
	//--更新事件触发器
	void UpdatePropListForEventTrigger(MapObjEdit* pEdit);
	//--
	void UpdateProListForNPCNav(tagNPCNavMapBuildStartPoint* pNpcNav);
	//--
	void UpdateProListForPlayerNav(tagPlayerNavMapBuildStartPoint* pPlayerNav);
    
	//--增加物体ID到obj列表
	int AddObjIDToObjList(CString szBuffer);
  
	//--修改导航点的属性
	void ModifyWayPointProp(CString szID);
	//--修改触发器的属性
	void ModifyTriggerProp(CString szID);
	//--修改音效的属性
	void ModifySoundProp(CString szID);
	//--修改点光源的属性
	void ModifyPointLightProp(CString szID);
	//--修改地图区域的属性
	void ModifyMapRectProp(CString szID);
	//--修改寻路导航点
	void ModifyPathPointProp(CString szID);
	//--修改刷怪点
	void ModifySpawnPoint(CString szID);
	//--修改事件触发器
	void ModifyEventTrigger(CString szID);
	//--
	void ModifyNPCNav(CString szID);
	//--
	void ModifyPlayerNav(CString szID);

	//--new a waypoint
	bool NewWayPoint();
	//--new a trigger
	bool NewTrigger();
	//--new a object
	bool NewObject();
	//--new a sound
	bool NewSound();
	//--
	bool NewNPCNavStartPoint();
	//--
	bool NewPlayerNavStartPoint();

	//--显示/隐藏NPC
	void ShowNPC(bool bVal);
	//--显示/隐藏动态障碍物
	void ShowDynamicBlock(bool bVal);
	//--显示/隐藏静态物体
	void ShowStaticObj(bool bVal);
	//--显示/隐藏已锁定地图对象
	void ShowLockOjb(bool bVal);
	//--显示/隐藏已标记地图对象
	void ShowFlagObj(bool bVal);
	//--显示/隐藏门
	void ShowMapDoor(bool bVal);
	//--显示/隐藏触发特效
	void ShowMapTriggerEffect(bool bVal);

	//--是否显示/隐藏NPC
	bool IsShowNPC();
	//--是否显示/隐藏动态障碍物
	bool IsShowDynamicBlock();
	//--是否显示/隐藏静态物体
	bool IsShowStaticObj();
	//--是否显示/隐藏地物方块
	bool IsShowQuad();
	//--是否显示/隐藏迷宫区域
	bool IsShowMazeZone();
	//--是否显示/隐藏已锁定地图对象
	bool IsShowLockObj();
	//--是否显示/隐藏已标记地图对象
	bool IsShowFlagObj();
	//--是否显示/隐藏地图区域
	bool IsShowMapRect();
	//--是否显示/隐藏门
	bool IsShowMapDoor();
	//--是否显示/隐藏触发特效
	bool IsShowMapTriggerEffect();

	//得到当前地图对象类型
	GMAPOBJ_TYPE  GetCurGMAPOBJTYPE();
	
	// 复制添加按钮
	CButton m_CopyAdd;
	afx_msg void OnBnClickedButtonCopyadd();		//复制添加按钮事件处理函数
	// 保存导航点名
	CString m_strWayPtName;
	// 显示筛选
	bool m_IsNpc;
	bool m_IsDynaBlock;
	bool m_IsStaticObj;
	bool m_IsQuad;
	bool m_IsMazeZone;
	bool m_IsLock;
	bool m_IsFlag;
	bool m_IsMapRect;
	bool m_IsTrigger;
	bool m_IsWayPoint;
	bool m_IsMapDoor;
	bool m_IsMapTriggerEffect;
	
	// 保存/读盘文件属性
	CString m_szMapID;						// 保存地图ID
	tagLight m_SunLight;					// 太阳光
	Color4f m_SkyCol;						//天光
	DWORD   m_dwSkyShadeCol;				//天空遮罩颜色
	bool    m_bRenderSkyShade;              //是否渲染天空遮罩
	DWORD   m_dwDynamicDiffCol;             // 动态平行光diffuse
	DWORD   m_dwDynamicAmbCol;				// 动态平行光Ambient
	DWORD   m_dwDynamicSpecCol;             // 动态平行光Specular
	CString m_szSkyBoxTopTex;				//天空盒上纹理
	CString m_szSkyBoxFrontRightTex;		//天空盒前右纹理
	CString m_szSkyBoxBackLeftTex;			//天空盒后左纹理
	Vector3 m_vecSkyBoxSize;				//天空盒大小
	Vector3 m_vecSkyBoxOff;					//天空盒偏移
	FLOAT	m_fSunModulus;					//太阳光增强系数
	FLOAT	m_fSkyYaw;						//天空盒旋转角度
	BOOL	m_bMapAttOk;					//是否确定地图属性
	tagDistFog m_Fog;						//雾
	INT		m_dist;							//视野
	
	afx_msg void OnBnClickedMapatt();// 地图属性按钮事件

	//--adaptive
	afx_msg void OnBnClickedBnadative();
	AdaptiveDlg* m_adaptiveDlg;
	afx_msg LONG OnDelAdativeDlgMsg(WPARAM wP,LPARAM lP);//接收Adaptive投递的删除窗体消息
	CButton m_BtnAdative;		

	// 图片
	CImageList m_imgList;								//图片资源列表(锁和标志)

	//--资源汇总
	ResourceGather*	m_pResGatherDlg;			
	CButton				m_BtnResGather;					//资源汇总按钮
	static vector<tagResGather>	m_vecRes;				//保存已添加的地图对象
	afx_msg void OnBnClickedButtonResgather();			//资源汇总按钮事件函数
	afx_msg LONG OnDelRGDlgMsg(WPARAM wP,LPARAM lP);	//接收资源汇总投递的删除窗体消息
	afx_msg LONG OnNewObjMsg(WPARAM wP,LPARAM lP);		//接收资源汇总投递的增加地图对象消息
	afx_msg LONG OnShowModel(WPARAM wP, LPARAM lP);		//接收资源汇总投递的查看模型消息
	
	//--只显示上锁/标识的地图对象
	BYTE m_byColumn;		//单击了哪一个列的CListCtrl按钮	
	bool m_bColEqual;
	afx_msg void OnColumnclickobjlist(NMHDR *pNMHDR, LRESULT *pResult);	//单击CListCtrl列按钮事件
	void hideNpcStaObjByLock(MapObjEdit::EType type);
	void hideNpcStaObjByFlag(MapObjEdit::EType type);

	//--
	afx_msg void OnBnClickedShowstaticpl();
	afx_msg void OnBnClickedShowdynamicpl();
	BOOL m_bShowStaticPL;
	BOOL m_bShowDynamicPL;

	//显示雾
	BOOL m_bShowFog;
	afx_msg void OnBnClickedShowfog();
	
	//是否随地物移动
	BOOL m_bFollowMove;
	afx_msg void OnBnClickedCheckFollowmove();
	BOOL GetFollowMove(){ return m_bFollowMove; }
	
	//全局id
	DWORD	m_dwGlobalID;
	vector<tagMapNPC*>		m_vecSortNPC;
}; 


