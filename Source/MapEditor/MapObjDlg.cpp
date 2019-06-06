// MapObjDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MapEditor.h"
#include "MapObjDlg.h"
#include ".\mapobjdlg.h"
#include ".\PlugMapEditor.h"
#include "SelAttDlg.h"
#include "SetChopFieldType.h"
#include "WayPointWnd.h"
#include "FilterSelect.h"
#include "FileAttribute.h"
#include "ResourceGather.h"
// CMapObjDlg 对话框
//显示物体列表
void CMapObjDlg::DisplayList(MapObjEdit::EType type)
{
	typedef list<MapObjEdit*> EDIT;
	EDIT  pList ;
	CString szBuffer;
	int nIndex = 0;
	pList=g_mapEditor->GetMap()->GetMapObjEditList();
	EDIT::iterator pIter =pList.begin(); 
	for(; pIter != pList.end(); ++pIter)
	{
		if ((*pIter)->GetType() == type)
		{
			DWORD ID = (*pIter)->GetMapID();
			szBuffer.Format(_T("%x"), ID);
			int row = AddObjIDToObjList(szBuffer);
			if(type == MapObjEdit::NPC)
			{
				tagMapNPC* p = (tagMapNPC*)(*pIter)->GetMapNPC();
				if(p->bLock)
					m_objList.SetItem(row, LOCK_COL, LVIF_IMAGE, NULL, EImg_Lock, 0, 0, 0, 0);
				else
					m_objList.SetItem(row, LOCK_COL, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
				if(p->bFlag)
					m_objList.SetItem(row, FLAG_COL, LVIF_IMAGE, NULL, EImg_Flag, 0, 0, 0, 0);
				else
					m_objList.SetItem(row, FLAG_COL, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
			}
			else if(type == MapObjEdit::StaticObj)
			{
				tagStaticMapObj* p = (tagStaticMapObj*)(*pIter)->GetStaticMapObj();
				if(p->bLock)
					m_objList.SetItem(row, LOCK_COL, LVIF_IMAGE, NULL, EImg_Lock, 0, 0, 0, 0);
				else
					m_objList.SetItem(row, LOCK_COL, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
				if(p->bFlag)
					m_objList.SetItem(row, FLAG_COL, LVIF_IMAGE, NULL, EImg_Flag, 0, 0, 0, 0);
				else
					m_objList.SetItem(row, FLAG_COL, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
			}
			else if(type == MapObjEdit::DynamicBlock)
			{
				tagMapDynamicBlock* p = (tagMapDynamicBlock*)(*pIter)->GetDynamicBlock();
				if(p->bLock)
					m_objList.SetItem(row, LOCK_COL, LVIF_IMAGE, NULL, EImg_Lock, 0, 0, 0, 0);
				else
					m_objList.SetItem(row, LOCK_COL, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
				if(p->bFlag)
					m_objList.SetItem(row, FLAG_COL, LVIF_IMAGE, NULL, EImg_Flag, 0, 0, 0, 0);
				else
					m_objList.SetItem(row, FLAG_COL, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
			}
			else  if(type == MapObjEdit::MapDoor)
			{
				tagMapDoor* p = (tagMapDoor*)(*pIter)->GetMapDoor();
				if(p->bLock)
					m_objList.SetItem(row, LOCK_COL, LVIF_IMAGE, NULL, EImg_Lock, 0, 0, 0, 0);
				else
					m_objList.SetItem(row, LOCK_COL, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
				if(p->bFlag)
					m_objList.SetItem(row, FLAG_COL, LVIF_IMAGE, NULL, EImg_Flag, 0, 0, 0, 0);
				else
					m_objList.SetItem(row, FLAG_COL, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
			}
			else if(type == MapObjEdit::MapCarrier)
			{
				tagMapCarrier* p = (tagMapCarrier*)(*pIter)->GetMapCarrier();
				if(p->bLock)
					m_objList.SetItem(row, LOCK_COL, LVIF_IMAGE, NULL, EImg_Lock, 0, 0, 0, 0);
				else
					m_objList.SetItem(row, LOCK_COL, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
				if(p->bFlag)
					m_objList.SetItem(row, FLAG_COL, LVIF_IMAGE, NULL, EImg_Flag, 0, 0, 0, 0);
				else
					m_objList.SetItem(row, FLAG_COL, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
				if( _tcslen(p->szWayPoint)!=0 )
				{
					list<tagMapWayPoint*> WayPointList;
					g_mapEditor->GetMap()->GetWayPointList(WayPointList);
					int nItem=0;
					for(list<tagMapWayPoint*>::iterator iter=WayPointList.begin();iter!=WayPointList.end();++iter)
					{									
						if( _tcscmp((*iter)->szName, p->szWayPoint)==0 && (*iter)->byIndex==0 )
						{
							(*pIter)->SetWorldPos((*iter)->vPos);
							(*pIter)->SetYaw(p->fYaw);
						}

					}							
				}
			}
			else  if(type == MapObjEdit::MapTrrigerEffect)
			{
				tagMapTriggerEffect* p = (tagMapTriggerEffect*)(*pIter)->GetMapTriggerEffect();
				if(p->bLock)
					m_objList.SetItem(row, LOCK_COL, LVIF_IMAGE, NULL, EImg_Lock, 0, 0, 0, 0);
				else
					m_objList.SetItem(row, LOCK_COL, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
				if(p->bFlag)
					m_objList.SetItem(row, FLAG_COL, LVIF_IMAGE, NULL, EImg_Flag, 0, 0, 0, 0);
				else
					m_objList.SetItem(row, FLAG_COL, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
			}
		}
	}
}

//显示导航点列表
void CMapObjDlg::DisplayWayPointList()
{
	list<tagMapWayPoint*> WayPointList;
	g_mapEditor->GetMap()->GetWayPointList(WayPointList);
	int nItem=0;
	for(list<tagMapWayPoint*>::iterator iter=WayPointList.begin();iter!=WayPointList.end();++iter)
	{
		CString szBuffer;
		szBuffer.Format(_T("%x"), (*iter)->dwObjID);
		m_objList.InsertItem(nItem,szBuffer, 0);	
		szBuffer.Format(_T("%s %d"), (*iter)->szName,(*iter)->byIndex);
		m_objList.SetItemText(nItem,1,szBuffer.GetBuffer());
		if((*iter)->bLock)
			m_objList.SetItem(nItem, LOCK_COL, LVIF_IMAGE, NULL, EImg_Lock, 0, 0, 0, 0);
		else
			m_objList.SetItem(nItem, LOCK_COL, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
		if((*iter)->bFlag)
			m_objList.SetItem(nItem, FLAG_COL, LVIF_IMAGE, NULL, EImg_Flag, 0, 0, 0, 0);
		else
			m_objList.SetItem(nItem, FLAG_COL, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
		nItem++;
	}
}

//显示触发器列表
void CMapObjDlg::DisplayTriggerList()
{
	list<tagMapTrigger*> TriggerList;
	g_mapEditor->GetMap()->GetTriggerList(TriggerList);
	int nItem=0;
	for(list<tagMapTrigger*>::iterator iter=TriggerList.begin();iter!=TriggerList.end();++iter)
	{
		CString szBuffer;
		szBuffer.Format(_T("%x"), (*iter)->dwObjID);
		m_objList.InsertItem(nItem,szBuffer, 0);
		if((*iter)->bLock)
			m_objList.SetItem(nItem, LOCK_COL, LVIF_IMAGE, NULL, EImg_Lock, 0, 0, 0, 0);
		else
			m_objList.SetItem(nItem, LOCK_COL, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
		if((*iter)->bFlag)
			m_objList.SetItem(nItem, FLAG_COL, LVIF_IMAGE, NULL, EImg_Flag, 0, 0, 0, 0);
		else
			m_objList.SetItem(nItem, FLAG_COL, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
		nItem++;
	}
}

//--显示音效列表
void CMapObjDlg::DisplaySound()
{
	list<tagMapSound*> SoundList;
	g_mapEditor->GetMap()->GetSoundList(SoundList);
	int nItem=0;
	for(list<tagMapSound*>::iterator iter=SoundList.begin();iter!=SoundList.end();++iter)
	{
		CString szBuffer;
		szBuffer.Format(_T("%x"), (*iter)->dwObjID);
		m_objList.InsertItem(nItem,szBuffer, 0);
		if((*iter)->bLock)
			m_objList.SetItem(nItem, LOCK_COL, LVIF_IMAGE, NULL, EImg_Lock, 0, 0, 0, 0);
		else
			m_objList.SetItem(nItem, LOCK_COL, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
		if((*iter)->bFlag)
			m_objList.SetItem(nItem, FLAG_COL, LVIF_IMAGE, NULL, EImg_Flag, 0, 0, 0, 0);
		else
			m_objList.SetItem(nItem, FLAG_COL, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
		nItem++;
	}
}

//--显示点光源列表
void CMapObjDlg::DisplayPointLightList()
{
	list<MapObjEdit*> PointLightList;
	g_mapEditor->GetMap()->GetPointLightList(PointLightList);
	int nItem=0;
	for(list<MapObjEdit*>::iterator iter=PointLightList.begin(); iter!=PointLightList.end(); ++iter)
	{
		CString szBuffer;
		szBuffer.Format(_T("%x"), (*iter)->GetMapID());
		m_objList.InsertItem(nItem,szBuffer, 0);
		tagMapPointLight* pPointLgt = (tagMapPointLight*)(*iter)->GetPointLgtObj();
		if(pPointLgt->bLock)
			m_objList.SetItem(nItem, LOCK_COL, LVIF_IMAGE, NULL, EImg_Lock, 0, 0, 0, 0);
		else
			m_objList.SetItem(nItem, LOCK_COL, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
		if(pPointLgt->bFlag)
			m_objList.SetItem(nItem, FLAG_COL, LVIF_IMAGE, NULL, EImg_Flag, 0, 0, 0, 0);
		else
			m_objList.SetItem(nItem, FLAG_COL, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
		nItem++;
	}
}

//--显示地图区域列表
void CMapObjDlg::DisplayMapRectList()
{
	list<MapObjEdit*> mapRectList;
	g_mapEditor->GetMap()->GetMapRectList(mapRectList);
	int nItem = 0;
	for(list<MapObjEdit*>::iterator iter= mapRectList.begin(); iter!=mapRectList.end(); ++iter)
	{
		CString szBuffer;
		szBuffer.Format(_T("%x"), (*iter)->GetMapID());
		m_objList.InsertItem(nItem, szBuffer, 0);
		const tagMapArea* pMapRect = (*iter)->GetMapRect();
		if(pMapRect->bLock)
			m_objList.SetItem(nItem, LOCK_COL, LVIF_IMAGE, NULL, EImg_Lock, 0, 0, 0, 0);
		else
			m_objList.SetItem(nItem, LOCK_COL, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
		if(pMapRect->bFlag)
			m_objList.SetItem(nItem, FLAG_COL, LVIF_IMAGE, NULL, EImg_Flag, 0, 0, 0, 0);
		else
			m_objList.SetItem(nItem, FLAG_COL, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
		nItem++;
	}

}	

//--显示寻路导航点
void CMapObjDlg::DisplayPathPoint()
{
	list<MapObjEdit*> PathPointList;
	g_mapEditor->GetMap()->GetPathPointList(PathPointList);
	int nItem = 0;
	for(list<MapObjEdit*>::iterator iter=PathPointList.begin(); iter!=PathPointList.end(); ++iter)
	{
		CString szBuffer;
		szBuffer.Format(_T("%x"), (*iter)->GetMapID());
		m_objList.InsertItem(nItem, szBuffer, 0);
		const tagMapPathPoint* pathPoint = (*iter)->GetPathPoint();
		if(pathPoint->bLock)
			m_objList.SetItem(nItem, LOCK_COL, LVIF_IMAGE, NULL, EImg_Lock, 0, 0, 0, 0);
		else
			m_objList.SetItem(nItem, LOCK_COL, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
		if(pathPoint->bFlag)
			m_objList.SetItem(nItem, FLAG_COL, LVIF_IMAGE, NULL, EImg_Flag, 0, 0, 0, 0);
		else
			m_objList.SetItem(nItem, FLAG_COL, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
		nItem++;
	}
}

//--显示刷怪点
void CMapObjDlg::DisplaySpawnPoint()
{
	list<MapObjEdit*> SpawnPointList;
	g_mapEditor->GetMap()->GetSpawnPointList(SpawnPointList);
	int nItem = 0;
	for(list<MapObjEdit*>::iterator iter=SpawnPointList.begin(); iter!=SpawnPointList.end(); ++iter)
	{
		CString szBuffer;
		szBuffer.Format(_T("%x"), (*iter)->GetMapID());
		m_objList.InsertItem(nItem, szBuffer, 0);
		nItem++;
	}
}

void CMapObjDlg::DisplayNPCNav()
{
	list<tagNPCNavMapBuildStartPoint*> NPCNavList;
	NPCNavList = g_mapEditor->GetMap()->GetNPCNavMapBuildStartPoint();
	int nItem = 0;
	for(list<tagNPCNavMapBuildStartPoint*>::iterator iter= NPCNavList.begin();
		iter!=NPCNavList.end(); iter++)
	{
		CString szBuffer;
		szBuffer.Format(_T("%x"), (*iter)->dwObjID);
		m_objList.InsertItem(nItem, szBuffer, 0);
		nItem++;
	}
}

void CMapObjDlg::DisplayPlayerNav()
{
	list<tagPlayerNavMapBuildStartPoint*> PlayerNavList;
	PlayerNavList = g_mapEditor->GetMap()->GetPlayerNavMapBuildStartPoint();
	int nItem = 0;
	for(list<tagPlayerNavMapBuildStartPoint*>::iterator iter= PlayerNavList.begin();
		iter!=PlayerNavList.end(); iter++)
	{
		CString szBuffer;
		szBuffer.Format(_T("%x"), (*iter)->dwObjID);
		m_objList.InsertItem(nItem, szBuffer, 0);
		nItem++;
	}
}

//--显示事件触发器
void CMapObjDlg::DisplayEventTrigger()
{
	list<MapObjEdit*> EvetTriggerList;
	g_mapEditor->GetMap()->GetEventTriggerList(EvetTriggerList);
	int nItem = 0;
	for(list<MapObjEdit*>::iterator iter=EvetTriggerList.begin();
		iter!=EvetTriggerList.end(); ++iter)
	{
		CString szBuffer;
		szBuffer.Format(_T("%x"), (*iter)->GetMapID());
		m_objList.InsertItem(nItem, szBuffer, 0);
		nItem++;
	}
}


//全局变量
tagStaticMapObj		g_StaticMapObj;
tagMapDynamicBlock	g_DynamicBlock;
tagMapNPC			g_MapNpc;
tagMapDoor			g_MapDoor;
tagMapCarrierEx		g_MapCarrier;
tagMapTriggerEffect g_MapTriggerEffect;
vector<tagResGather>CMapObjDlg::m_vecRes;
TCHAR szMapFile[MAX_PATH] = _T("NewMbFile.mb");
const UINT TIMER_ID =  10011;

IMPLEMENT_DYNAMIC(CMapObjDlg, CDialog)


CMapObjDlg::CMapObjDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMapObjDlg::IDD, pParent)
	, m_bShowStaticPL(TRUE)
	, m_bShowDynamicPL(FALSE)
	, m_bShowFog(FALSE)
	, m_bFollowMove(FALSE)
{
	m_Cool3DView		= NULL;
	m_pParent		= NULL;
	m_pPreCreated	= NULL;
}

CMapObjDlg::~CMapObjDlg()
{			
	SAFE_DELETE(m_Cool3DView);	
}

void CMapObjDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_objList, m_objList);
	DDX_Control(pDX, IDC_objTypeList, m_objTypeList);
	DDX_Control(pDX, IDC_propList,   m_propList);
	DDX_Control(pDX, IDC_BUTTON_NEWMAP, m_NEWMAP);
	DDX_Control(pDX, IDC_BUTTON_CopyAdd, m_CopyAdd);
	DDX_Control(pDX, IDC_BnAdative, m_BtnAdative);
	DDX_Control(pDX, IDC_BUTTON_ResGather, m_BtnResGather);
	DDX_Check(pDX, IDC_ShowStaticPL, m_bShowStaticPL);
	DDX_Check(pDX, IDC_ShowDynamicPL, m_bShowDynamicPL);
	DDX_Check(pDX, IDC_ShowFog, m_bShowFog);
	DDX_Check(pDX, IDC_CHECK_FOLLOWMOVE, m_bFollowMove);
}

BEGIN_MESSAGE_MAP(CMapObjDlg, CDialog)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_Add, OnBnClickedButtonAdd)
	ON_CBN_SELCHANGE(IDC_objTypeList, OnCbnSelchangeobjtypelist)
    ON_BN_CLICKED(IDC_Del, OnBnClickedDel)
    ON_NOTIFY(NM_CLICK, IDC_objList, OnNMClickobjlist)
    ON_BN_CLICKED(IDC_Load, OnBnClickedLoad)
    ON_BN_CLICKED(IDC_Commit, OnBnClickedCommit)
    ON_BN_CLICKED(IDC_Save, OnBnClickedSave)
    ON_NOTIFY(NM_DBLCLK, IDC_objList, OnNMDblclkobjlist)
    ON_BN_CLICKED(IDC_BUTTON_NEWMAP, OnBnClickedButtonNewmap)
	ON_BN_CLICKED(IDC_BUTTON_CopyAdd, OnBnClickedButtonCopyadd)
	ON_BN_CLICKED(IDC_BUTTON_Filter, OnBnClickedButtonFilter)
	ON_BN_CLICKED(IDC_MapAtt, OnBnClickedMapatt)
	ON_BN_CLICKED(IDC_BnAdative, OnBnClickedBnadative)
	ON_BN_CLICKED(IDC_BUTTON_ResGather, OnBnClickedButtonResgather)
	ON_MESSAGE(WM_DELADAPTIVE_DLG,OnDelAdativeDlgMsg)
	ON_MESSAGE(WM_DELRG_DLG, OnDelRGDlgMsg)
	ON_MESSAGE(WM_NEWOBJ, OnNewObjMsg)
	ON_MESSAGE(WM_SHOWMODEL, OnShowModel)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_objList, OnColumnclickobjlist)
	ON_WM_KEYDOWN()
	ON_BN_CLICKED(IDC_ShowStaticPL, &CMapObjDlg::OnBnClickedShowstaticpl)
	ON_BN_CLICKED(IDC_ShowDynamicPL, &CMapObjDlg::OnBnClickedShowdynamicpl)
	ON_BN_CLICKED(IDC_ShowFog, &CMapObjDlg::OnBnClickedShowfog)
	ON_BN_CLICKED(IDC_CHECK_FOLLOWMOVE, &CMapObjDlg::OnBnClickedCheckFollowmove)
END_MESSAGE_MAP()

// CMapObjDlg 消息处理程序
BOOL CMapObjDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	//初始化对象类型列表

	//--list
	m_objList.InsertColumn(0, _T("ID"), LVCFMT_LEFT,  70);
	m_objList.InsertColumn(1, _T("对象名称"), LVCFMT_LEFT,  113);
	m_objList.InsertColumn(2, _T("锁"),	LVCFMT_LEFT, 25);
	m_objList.InsertColumn(3, _T("标"), LVCFMT_LEFT, 25);
	m_objList.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_SUBITEMIMAGES
		|LVS_EX_HEADERDRAGDROP/*|LVS_SHOWSELALWAYS*/);

	m_objTypeList.InsertString(GMAPOBJ_Npc,		   _T("NPC"));
	m_objTypeList.InsertString(GMAPOBJ_StaticObj , _T("静态对象"));
	m_objTypeList.InsertString(GMAPOBJ_DynamicBlock, _T("动态障碍物"));
	m_objTypeList.InsertString(GMAPOBJ_WAYPOINT,   _T("导航点"));
	m_objTypeList.InsertString(GMAPOBJ_Trigger,    _T("触发器"));
	m_objTypeList.InsertString(GMAPOBJ_Sound,      _T("音效"));
	m_objTypeList.InsertString(GMAPOBJ_PointLight, _T("点光源"));
	m_objTypeList.InsertString(GMAPOBJ_MapRect, _T("地图区域"));
	m_objTypeList.InsertString(GMAPOBJ_PathPoint, _T("寻路导航点"));
	m_objTypeList.InsertString(GMAPOBJ_SpawnPoint, _T("刷怪点"));
	m_objTypeList.InsertString(GMAPOBJ_EventTrigger, _T("事件触发器"));
	m_objTypeList.InsertString(GMAPOBJ_NPCNavStartPoint, _T("NPC格子导航图生成起点"));
	m_objTypeList.InsertString(GMAPOBJ_PlayerNavStartPoint, _T("玩家寻路导航图生成起点"));
	m_objTypeList.InsertString(GMAPOBJ_MapDoor, _T("门"));	
	m_objTypeList.InsertString(GMAPOBJ_MapCarrier, _T("载具"));	
	m_objTypeList.InsertString(GMAPOBJ_MapTriggerEffect, _T("触发特效"));	


	SetTimer(TIMER_ID, 20, NULL);
	m_Cool3DView = new SimpleView;


	
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	((CButton*)(GetDlgItem(IDC_CHECK_COPY_ADD)))->EnableWindow(false);
    //各个标志位重置
	m_bIsBuildWayPoint=false;
	m_bIsBuildTrigger=false;
	m_bIsBuildSound=false;
	m_bIsBuildPointLight=false;
	m_bCopyBuildPointLgt=false;
	m_bIsBuildMapRect = false;
	m_bIsBuildPathPoint=false;
	m_bIsBuildSpawnPoint=false;
	m_bIsBuildNpcNav=false;
	m_bIsBuildPlayerNav=false;
	m_szStaticFileOpenType=_T("");
	m_PreObjListSelectedIndex=-1;


	m_IsNpc = true;
	m_IsDynaBlock = true;
	m_IsStaticObj = true;
	m_IsQuad = false;
	m_IsMazeZone = false;
	m_IsLock = true;
	m_IsFlag = true;
	m_IsMapRect = true;
	m_IsTrigger = true;
	m_IsWayPoint = true;
	m_IsMapDoor = true;
	m_IsMapTriggerEffect = true;
	g_mapEditor->GetMap()->SetIsDrawMapRect(m_IsMapRect);
	g_mapEditor->GetMap()->SetIsDrawTrigger(m_IsTrigger);
	g_mapEditor->GetMap()->SetIsDrawWayPoint(m_IsWayPoint);

	//文件属性
	m_szMapID = _T("");			
	m_dwMapIndex = 0;
	m_fSunModulus = 1.0f;
	m_SunLight.type = Light_Direct;
	m_SunLight.pos = Vector3(-1000.0f, 1000.0f, -1000.0f);
	m_SunLight.diffuse = Color4f(1.0f, 1.0f, 1.0f, 1.0f);
	m_SunLight.ambient = Color4f(1.0f, 0.4f, 0.4f, 0.4f);
	m_SunLight.specular = Color4f(1.0f, 1.0f, 1.0f, 1.0f);
	m_SkyCol = Color4f(1.0f, 1.0f, 1.0, 1.0);
	m_dwSkyShadeCol = 0xffffffff;
	m_bRenderSkyShade = true;
	m_dwDynamicDiffCol = 0xffffffff;
	m_dwDynamicAmbCol = 0xff808080;
	m_dwDynamicSpecCol = 0xffffffff;
	m_SunLight.dir = -m_SunLight.pos;
	m_szSkyBoxTopTex = _T("");
	m_szSkyBoxFrontRightTex = _T("");
	m_szSkyBoxBackLeftTex = _T("");
	m_vecSkyBoxSize = Vector3(1000,1000,1000);
	m_vecSkyBoxOff = Vector3(0,0,0);
	m_adaptiveDlg = NULL;
	m_pResGatherDlg = NULL;
	m_fSkyYaw = 0;
	m_bMapAttOk = FALSE;
	m_Fog.color = Color4f(1.0f, 1.0f, 1.0f, 1.0f);
	m_Fog.start = 0.1f;
	m_Fog.end = 0.75f;
	m_Fog.density  = 1.0f;

	CBitmap bmp1;
	bmp1.LoadBitmap(IDB_BITMAP1);
	m_imgList.Create(16, 16, ILC_COLOR8, 0, 4); 
	m_imgList.Add( &bmp1, RGB(255,0,255) );	
	m_objList.SetImageList( &m_imgList, LVSIL_SMALL);

	m_byColumn = -1;
	m_bColEqual = false;
	m_hAccelTable = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));

	m_dist=800;

	((CButton*)(GetDlgItem(IDC_CHECK_FOLLOWMOVE)))->SetCheck(BST_CHECKED);
	m_bFollowMove = ((CButton*)(GetDlgItem(IDC_CHECK_FOLLOWMOVE)))->GetCheck();
	
	m_dwGlobalID = 0;

	return TRUE; 
}

//更新物体属性列表
void CMapObjDlg::UpdateList(MapObjEdit* pSelected)
{
	if(pSelected == NULL)
		return;
	m_propList.DeleteAllItem();

	//更新物体类型列表
	if(pSelected->GetType()!=MapObjEdit::NPC && m_objTypeList.GetCurSel() != pSelected->GetType())
	{
		m_objTypeList.SetCurSel(pSelected->GetType());
		OnCbnSelchangeobjtypelist();
	}
	if(pSelected->GetType()==MapObjEdit::NPC && m_objTypeList.GetCurSel() != pSelected->GetType())
	{
		m_objTypeList.SetCurSel(GMAPOBJ_Npc);
		OnCbnSelchangeobjtypelist();
	}
    //更新物体属性列表
	CString szBuffer;
	int i = 0;
	for(; i < m_objList.GetItemCount(); i++)
	{
		szBuffer = m_objList.GetItemText(i, 0);
		int nIndex = _tcstoul(szBuffer, NULL, 16);
		m_objList.SetItemState(i, 0, LVIS_SELECTED);
		if(pSelected->GetMapID()== nIndex)
		{
			m_objList.SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			switch(m_objTypeList.GetCurSel())
			{
			case GMAPOBJ_Npc:
				memset(&g_MapNpc, 0, sizeof(g_MapNpc));
				pSelected->GetData(g_MapNpc);
				m_propList.DisplayNPC(&g_MapNpc);
				break;
			case GMAPOBJ_StaticObj:
				memset(&g_StaticMapObj, 0, sizeof(g_StaticMapObj));
				pSelected->GetData(g_StaticMapObj);
				m_propList.DisplayStaticObj(&g_StaticMapObj);
				break;
			case GMAPOBJ_DynamicBlock:
				memset(&g_DynamicBlock, 0, sizeof(g_DynamicBlock));
				pSelected->GetData(g_DynamicBlock);
				m_propList.DisplayDynamicBlock(&g_DynamicBlock);
				break;
			case GMAPOBJ_MapDoor:
				memset(&g_MapDoor, 0, sizeof(g_MapDoor));
				pSelected->GetData(g_MapDoor);
				m_propList.DisplayMapDoor(&g_MapDoor);
				break;
			case GMAPOBJ_MapCarrier:
				memset(&g_MapCarrier, 0, sizeof(g_MapCarrier));
				pSelected->GetData(g_MapCarrier.carrier);
				m_propList.DisplayMapCarrier(&g_MapCarrier.carrier);
				break;
			case GMAPOBJ_MapTriggerEffect:
				memset(&g_MapTriggerEffect, 0, sizeof(g_MapTriggerEffect));
				pSelected->GetData(g_MapTriggerEffect);
				m_propList.DisplayMapTriggerEffect(&g_MapTriggerEffect);
				
				break;
			}
			m_objList.EnsureVisible(i, FALSE);
			break;
		}
	}
}

//更新导航点属性列表
void CMapObjDlg::UpdatePropListForWayPoint(tagMapWayPoint* pWayPoint)
{
	m_propList.DeleteAllItem();
	if(m_objTypeList.GetCurSel()!=GMAPOBJ_WAYPOINT)
	{
		m_objTypeList.SetCurSel(GMAPOBJ_WAYPOINT);
		OnCbnSelchangeobjtypelist();
	}
	CString szBuffer;
	for(int i = 0; i < m_objList.GetItemCount(); i++)
	{
		m_objList.SetItemState(i, 0, LVIS_SELECTED);
		szBuffer = m_objList.GetItemText(i, 0);
		CString szID;
		szID.Format(_T("%x"), pWayPoint->dwObjID);
		if(szID==szBuffer)
		{
			m_objList.SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			m_propList.DisplayWayPointObj(pWayPoint);
			m_objList.EnsureVisible(i, FALSE);
			break;
		}
	}
}

//更新触发器属性列表
void CMapObjDlg::UpdatePropListForTrigger(tagMapTrigger* pTrigger)
{
	m_propList.DeleteAllItem();
	if(m_objTypeList.GetCurSel()!=GMAPOBJ_Trigger)
	{
		m_objTypeList.SetCurSel(GMAPOBJ_Trigger);
		OnCbnSelchangeobjtypelist();
	}
	CString szBuffer;
	for(int i = 0; i < m_objList.GetItemCount(); i++)
	{
		m_objList.SetItemState(i, 0, LVIS_SELECTED);
		szBuffer = m_objList.GetItemText(i, 0);
		CString szID;
		szID.Format(_T("%x"),pTrigger->dwObjID);
		if(szID==szBuffer)
		{
			m_objList.SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			m_propList.DisplayTriggerObj(pTrigger);
			m_objList.EnsureVisible(i, FALSE);
			break;
		}
	}
}

//--更新地图区域列表
void CMapObjDlg::UpdatePropListForMapRect(MapObjEdit* pEdit)
{
	if(pEdit==NULL)
		return;
	const tagMapArea* pMapRect = pEdit->GetMapRect();
	m_propList.DeleteAllItem();
	if(m_objTypeList.GetCurSel()!=GMAPOBJ_MapRect)
	{
		m_objTypeList.SetCurSel(GMAPOBJ_MapRect);
		OnCbnSelchangeobjtypelist();
	}
	CString szBuffer;
	for(int i = 0; i < m_objList.GetItemCount(); i++)
	{
		m_objList.SetItemState(i, 0, LVIS_SELECTED);
		szBuffer = m_objList.GetItemText(i, 0);
		CString szID;
		szID.Format(_T("%x"),pMapRect->dwObjID);
		if(szID==szBuffer)
		{
			m_objList.SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			m_propList.DisplayMapRectObj(pMapRect);
			m_objList.EnsureVisible(i, FALSE);
			break;
		}
	}
}

//--更新音效列表
void CMapObjDlg::UpdatePropListForSound(tagMapSound* pSound)
{
	m_propList.DeleteAllItem();
	if(m_objTypeList.GetCurSel()!=GMAPOBJ_Sound)
	{
		m_objTypeList.SetCurSel(GMAPOBJ_Sound);
		OnCbnSelchangeobjtypelist();
	}
	CString szBuffer;
	for(int i = 0; i < m_objList.GetItemCount(); i++)
	{
		m_objList.SetItemState(i, 0, LVIS_SELECTED);
		szBuffer = m_objList.GetItemText(i, 0);
		CString szID;
		szID.Format(_T("%x"),pSound->dwObjID);
		if(szID==szBuffer)
		{
			m_objList.SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			m_propList.DisplaySoundObj(pSound);
			m_objList.EnsureVisible(i, FALSE);
			break;
		}
	}
}

//--更新点光源列表
void CMapObjDlg::UpdatePropListForPointLight(MapObjEdit* pEdit)
{
	if(pEdit==NULL)
		return;
	tagMapPointLight* pPointLgt = (tagMapPointLight*)pEdit->GetPointLgtObj();
	m_propList.DeleteAllItem();
	if(m_objTypeList.GetCurSel()!=GMAPOBJ_PointLight)
	{
		m_objTypeList.SetCurSel(GMAPOBJ_PointLight);
		OnCbnSelchangeobjtypelist();
	}
	CString szBuffer;
	for(int i = 0; i < m_objList.GetItemCount(); i++)
	{
		m_objList.SetItemState(i, 0, LVIS_SELECTED);
		szBuffer = m_objList.GetItemText(i, 0);
		CString szID;
		szID.Format(_T("%x"),pPointLgt->dwID);
		if(szID==szBuffer)
		{
			m_objList.SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			m_propList.DisplayPointLightObj(pPointLgt);
			m_objList.EnsureVisible(i, FALSE);
			break;
		}
	}
}

void CMapObjDlg::UpdatePropListForPathPoint(MapObjEdit* pEdit)
{
	if(pEdit==NULL)
		return;
	tagMapPathPoint* pPathPoint = (tagMapPathPoint*)pEdit->GetPathPoint();
	m_propList.DeleteAllItem();
	if(m_objTypeList.GetCurSel()!=GMAPOBJ_PathPoint)
	{
		m_objTypeList.SetCurSel(GMAPOBJ_PathPoint);
		OnCbnSelchangeobjtypelist();
	}
	CString szBuffer;
	for(int i = 0; i < m_objList.GetItemCount(); i++)
	{
		m_objList.SetItemState(i, 0, LVIS_SELECTED);
		szBuffer = m_objList.GetItemText(i, 0);
		CString szID;
		szID.Format(_T("%x"),pPathPoint->dwObjID);
		if(szID==szBuffer)
		{
			m_objList.SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			m_propList.DisplayPathPoint(pPathPoint);
			m_objList.EnsureVisible(i, FALSE);
			break;
		}
	}
}

//--更新刷怪点
void CMapObjDlg::UpdatePropListForSpawnPoint(MapObjEdit* pEdit)
{
	if(pEdit==NULL)
		return;
	tagMapSpawnPoint* pSpawnPoint = (tagMapSpawnPoint*)pEdit->GetSpawnPoint();
	m_propList.DeleteAllItem();
	if(m_objTypeList.GetCurSel()!=GMAPOBJ_SpawnPoint)
	{
		m_objTypeList.SetCurSel(GMAPOBJ_SpawnPoint);
		OnCbnSelchangeobjtypelist();
	}
	CString szBuffer;
	for(int i = 0; i < m_objList.GetItemCount(); i++)
	{
		m_objList.SetItemState(i, 0, LVIS_SELECTED);
		szBuffer = m_objList.GetItemText(i, 0);
		CString szID;
		szID.Format(_T("%x"),pSpawnPoint->dwObjID);
		if(szID==szBuffer)
		{
			m_objList.SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			m_propList.DisplaySpawnPoint(pSpawnPoint);
			m_objList.EnsureVisible(i, FALSE);
			break;
		}
	}
}

void CMapObjDlg::UpdateProListForNPCNav(tagNPCNavMapBuildStartPoint* pNpcNav)
{
	if(pNpcNav==NULL)
		return;

	m_propList.DeleteAllItem();
	if(m_objTypeList.GetCurSel()!=GMAPOBJ_NPCNavStartPoint)
	{
		m_objTypeList.SetCurSel(GMAPOBJ_NPCNavStartPoint);
		OnCbnSelchangeobjtypelist();
	}
	CString szBuffer;
	for(int i = 0; i < m_objList.GetItemCount(); i++)
	{
		m_objList.SetItemState(i, 0, LVIS_SELECTED);
		szBuffer = m_objList.GetItemText(i, 0);
		CString szID;
		szID.Format(_T("%x"),pNpcNav->dwObjID);
		if(szID==szBuffer)
		{
			m_objList.SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			m_propList.DisplayNPCNav(pNpcNav);
			m_objList.EnsureVisible(i, FALSE);
			break;
		}
	}
}

void CMapObjDlg::UpdateProListForPlayerNav(tagPlayerNavMapBuildStartPoint* pPlayerNav)
{
	if(pPlayerNav==NULL)
		return;

	m_propList.DeleteAllItem();
	if(m_objTypeList.GetCurSel()!=GMAPOBJ_PlayerNavStartPoint)
	{
		m_objTypeList.SetCurSel(GMAPOBJ_PlayerNavStartPoint);
		OnCbnSelchangeobjtypelist();
	}
	CString szBuffer;
	for(int i = 0; i < m_objList.GetItemCount(); i++)
	{
		m_objList.SetItemState(i, 0, LVIS_SELECTED);
		szBuffer = m_objList.GetItemText(i, 0);
		CString szID;
		szID.Format(_T("%x"),pPlayerNav->dwObjID);
		if(szID==szBuffer)
		{
			m_objList.SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			m_propList.DisplayPlayerNav(pPlayerNav);
			m_objList.EnsureVisible(i, FALSE);
			break;
		}
	}
}

//--更新事件触发器
void CMapObjDlg::UpdatePropListForEventTrigger(MapObjEdit* pEdit)
{
	if(pEdit==NULL)
		return;
	tagMapEventTrigger* pEventTrigger = (tagMapEventTrigger*)pEdit->GetEventTrigger();
	m_propList.DeleteAllItem();
	if(m_objTypeList.GetCurSel()!=GMAPOBJ_EventTrigger)
	{
		m_objTypeList.SetCurSel(GMAPOBJ_EventTrigger);
		OnCbnSelchangeobjtypelist();
	}
	CString szBuffer;
	for(int i = 0; i < m_objList.GetItemCount(); i++)
	{
		m_objList.SetItemState(i, 0, LVIS_SELECTED);
		szBuffer = m_objList.GetItemText(i, 0);
		CString szID;
		szID.Format(_T("%x"),pEventTrigger->dwObjID);
		if(szID==szBuffer)
		{
			m_objList.SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			m_propList.DisplayEventTrigger(pEventTrigger);
			m_objList.EnsureVisible(i, FALSE);
			break;
		}
	}
}


//鼠标操作
void CMapObjDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(m_adaptiveDlg!=NULL)
		m_adaptiveDlg->m_static.SetFocus();
	m_Cool3DView->OnLButtonDown(nFlags, point);
}

void CMapObjDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_Cool3DView->OnLButtonUp(nFlags, point);
}

void CMapObjDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	m_Cool3DView->OnMouseMove(nFlags, point);
}

BOOL CMapObjDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	m_Cool3DView->OnMouseWheel(nFlags, zDelta, pt);
	return true;
}

void CMapObjDlg::OnRButtonDown(UINT nFlags, CPoint point)
{

	m_Cool3DView->OnRButtonDown(nFlags, point);
}

void CMapObjDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	m_Cool3DView->OnRButtonUp(nFlags, point);
}


//显示/隐藏NPC
void CMapObjDlg::ShowNPC(bool bVal)
{
	typedef list<MapObjEdit*> EDIT;
	EDIT  pList ;
	pList=g_mapEditor->GetMap()->GetMapObjEditList();
	EDIT::iterator pIter =pList.begin(); 
	for(; pIter != pList.end(); ++pIter)
	{
		if((*pIter)->GetType() == MapObjEdit::NPC)
		{
			if((*pIter)->GetSGNode())
				(*pIter)->GetSGNode()->Hide(!bVal);
			else if((*pIter)->GetSGEffectNode() && bVal)
					(*pIter)->GetSGEffectNode()->Play();
			else if((*pIter)->GetSGEffectNode())
					(*pIter)->GetSGEffectNode()->Close();
		}
	}
}

//显示隐藏怪
void CMapObjDlg::ShowDynamicBlock(bool bVal)
{
	typedef list<MapObjEdit*> EDIT;
	EDIT  pList ;
	pList=g_mapEditor->GetMap()->GetMapObjEditList();
	EDIT::iterator pIter =pList.begin(); 
	for(; pIter != pList.end(); ++pIter)
	{
		DWORD ID = (*pIter)->GetMapID();
		if((*pIter)->GetType() == MapObjEdit::DynamicBlock)
		{
			if((*pIter)->GetSGNode())
				(*pIter)->GetSGNode()->Hide(!bVal);
			else if((*pIter)->GetSGEffectNode() && bVal)
					(*pIter)->GetSGEffectNode()->Play();
			else if((*pIter)->GetSGEffectNode())
					(*pIter)->GetSGEffectNode()->Close();
		}
	}
}

void CMapObjDlg::ShowMapDoor(bool bVal)
{
	typedef list<MapObjEdit*> EDIT;
	EDIT  pList ;
	pList=g_mapEditor->GetMap()->GetMapObjEditList();
	EDIT::iterator pIter =pList.begin(); 
	for(; pIter != pList.end(); ++pIter)
	{
		DWORD ID = (*pIter)->GetMapID();
		if((*pIter)->GetType() == MapObjEdit::MapDoor)
		{
			if((*pIter)->GetSGNode())
				(*pIter)->GetSGNode()->Hide(!bVal);
			else if((*pIter)->GetSGEffectNode() && bVal)
				(*pIter)->GetSGEffectNode()->Play();
			else if((*pIter)->GetSGEffectNode())
				(*pIter)->GetSGEffectNode()->Close();
		}
	}
}

void CMapObjDlg::ShowMapTriggerEffect(bool bVal)
{
	typedef list<MapObjEdit*> EDIT;
	EDIT  pList ;
	pList=g_mapEditor->GetMap()->GetMapObjEditList();
	EDIT::iterator pIter =pList.begin(); 
	for(; pIter != pList.end(); ++pIter)
	{
		DWORD ID = (*pIter)->GetMapID();
		if((*pIter)->GetType() == MapObjEdit::MapTrrigerEffect)
		{
			if((*pIter)->GetSGNode())
				(*pIter)->GetSGNode()->Hide(!bVal);
			else if((*pIter)->GetSGEffectNode() && bVal)
				(*pIter)->GetSGEffectNode()->Play();
			else if((*pIter)->GetSGEffectNode())
				(*pIter)->GetSGEffectNode()->Close();
		}
	}
}

//显示/隐藏静态物体
void CMapObjDlg::ShowStaticObj(bool bVal)
{
	typedef list<MapObjEdit*> EDIT;
	EDIT  pList ;
	pList=g_mapEditor->GetMap()->GetMapObjEditList();
	EDIT::iterator pIter =pList.begin(); 
	for(; pIter != pList.end(); ++pIter)
	{
		
		if((*pIter)->GetType() == MapObjEdit::StaticObj)
		{
			if((*pIter)->GetSGNode())
				(*pIter)->GetSGNode()->Hide(!bVal);
			else if((*pIter)->GetSGEffectNode() && bVal)
					(*pIter)->GetSGEffectNode()->Play();
			else if((*pIter)->GetSGEffectNode())
					(*pIter)->GetSGEffectNode()->Close();
		}
	}
}

//--显示/隐藏已锁定地图对象
void CMapObjDlg::ShowLockOjb(bool bVal)
{
	typedef list<MapObjEdit*> EDIT;
	EDIT  pList ;
	pList=g_mapEditor->GetMap()->GetMapObjEditList();
	EDIT::iterator pIter =pList.begin(); 
	for(; pIter != pList.end(); ++pIter)
	{
		switch((*pIter)->GetType())
		{
		case MapObjEdit::NPC:
			{
				tagMapNPC* pNPC = (tagMapNPC*)(*pIter)->GetMapNPC();
				if(pNPC->bLock)
				{
					if((*pIter)->GetSGNode())
						(*pIter)->GetSGNode()->Hide(!bVal);
					else if((*pIter)->GetSGEffectNode() && bVal)
						(*pIter)->GetSGEffectNode()->Play();
					else if((*pIter)->GetSGEffectNode())
						(*pIter)->GetSGEffectNode()->Close();
				}
			}
			break;
		case MapObjEdit::StaticObj:
			{
				tagStaticMapObj* pStaticObj = (tagStaticMapObj*)(*pIter)->GetStaticMapObj();
				if(pStaticObj->bLock)
				{
					if((*pIter)->GetSGNode())
						(*pIter)->GetSGNode()->Hide(!bVal);
					else if((*pIter)->GetSGEffectNode() && bVal)
						(*pIter)->GetSGEffectNode()->Play();
					else if((*pIter)->GetSGEffectNode())
						(*pIter)->GetSGEffectNode()->Close();
				}
			}
			break;
		case MapObjEdit::DynamicBlock:
			{
				tagMapDynamicBlock* pDynaBlock = (tagMapDynamicBlock*)(*pIter)->GetDynamicBlock();
				if(pDynaBlock->bLock)
				{
					if((*pIter)->GetSGNode())
						(*pIter)->GetSGNode()->Hide(!bVal);
					else if((*pIter)->GetSGEffectNode() && bVal)
						(*pIter)->GetSGEffectNode()->Play();
					else if((*pIter)->GetSGEffectNode())
						(*pIter)->GetSGEffectNode()->Close();
				}
			}
			break;
		case MapObjEdit::MapDoor:
			{
				tagMapDoor* pDoor = (tagMapDoor*)(*pIter)->GetMapDoor();
				if(pDoor->bLock)
				{
					if((*pIter)->GetSGNode())
						(*pIter)->GetSGNode()->Hide(!bVal);
					else if((*pIter)->GetSGEffectNode() && bVal)
						(*pIter)->GetSGEffectNode()->Play();
					else if((*pIter)->GetSGEffectNode())
						(*pIter)->GetSGEffectNode()->Close();
				}
			}
			break;
		}
	}

}
//--显示/隐藏已标记地图对象
void CMapObjDlg::ShowFlagObj(bool bVal)
{
	typedef list<MapObjEdit*> EDIT;
	EDIT  pList ;
	pList=g_mapEditor->GetMap()->GetMapObjEditList();
	EDIT::iterator pIter =pList.begin(); 
	for(; pIter != pList.end(); ++pIter)
	{
		switch((*pIter)->GetType())
		{
		case MapObjEdit::NPC:
			{
				tagMapNPC* pNPC = (tagMapNPC*)(*pIter)->GetMapNPC();
				if(pNPC->bFlag)
				{
					if((*pIter)->GetSGNode())
						(*pIter)->GetSGNode()->Hide(!bVal);
					else if((*pIter)->GetSGEffectNode() && bVal)
						(*pIter)->GetSGEffectNode()->Play();
					else if((*pIter)->GetSGEffectNode())
						(*pIter)->GetSGEffectNode()->Close();
				}
			}
			break;
		case MapObjEdit::StaticObj:
			{
				tagStaticMapObj* pStaticObj = (tagStaticMapObj*)(*pIter)->GetStaticMapObj();
				if(pStaticObj->bFlag)
				{
					if((*pIter)->GetSGNode())
						(*pIter)->GetSGNode()->Hide(!bVal);
					else if((*pIter)->GetSGEffectNode() && bVal)
						(*pIter)->GetSGEffectNode()->Play();
					else if((*pIter)->GetSGEffectNode())
						(*pIter)->GetSGEffectNode()->Close();
				}
			}
			break;
		case MapObjEdit::DynamicBlock:
			{
				tagMapDynamicBlock* pDynaBlock = (tagMapDynamicBlock*)(*pIter)->GetDynamicBlock();
				if(pDynaBlock->bFlag)
				{
					if((*pIter)->GetSGNode())
						(*pIter)->GetSGNode()->Hide(!bVal);
					else if((*pIter)->GetSGEffectNode() && bVal)
						(*pIter)->GetSGEffectNode()->Play();
					else if((*pIter)->GetSGEffectNode())
						(*pIter)->GetSGEffectNode()->Close();
				}
			}
			break;
		case MapObjEdit::MapDoor:
			{
				tagMapDoor* pDoor = (tagMapDoor*)(*pIter)->GetMapDoor();
				if(pDoor->bFlag)
				{
					if((*pIter)->GetSGNode())
						(*pIter)->GetSGNode()->Hide(!bVal);
					else if((*pIter)->GetSGEffectNode() && bVal)
						(*pIter)->GetSGEffectNode()->Play();
					else if((*pIter)->GetSGEffectNode())
						(*pIter)->GetSGEffectNode()->Close();
				}
			}
			break;
		}
	}
}

//绘制场景
void CMapObjDlg::OnTimer(UINT nIDEvent)
{
	POSITION pos = m_objList.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		CString szBuffer;
		int nItem = m_objList.GetNextSelectedItem(pos);
		if(m_PreObjListSelectedIndex!=nItem)
		{
			OnNMClickobjlist(NULL,NULL);
			m_PreObjListSelectedIndex=nItem;
		}
	}

	CDialog::OnTimer(nIDEvent);
}

//点击复制添加按钮操作
void CMapObjDlg::OnBnClickedButtonCopyadd()
{
	int type = m_objTypeList.GetCurSel();
	MapObjEdit* pOldEdit = NULL;
	MapObjEdit* pNewEdit = NULL;
	GameMapEditor*  pMap= g_mapEditor->GetMap();
	ASSERT(pMap != NULL);
	CString szBuffer;
	switch(type)
	{
	case GMAPOBJ_Npc:
		{	
			pOldEdit = pMap->GetSelectedObj();
			if(pOldEdit==NULL)
			{
				if(m_objTypeList.GetCurSel() == GMAPOBJ_Npc)
					AfxMessageBox(_T("请选择NPC!"));
				SetFocus();
				return;
			}
			tagMapNPC oldNPC;
			pOldEdit->GetData(oldNPC);
			BeginMFCRes();


			g_MapNpc.dwObjID   = m_dwGlobalID++;
			g_MapNpc.dwTypeID  = oldNPC.dwTypeID;
			g_MapNpc.fYaw = oldNPC.fYaw;
			g_MapNpc.bLock = oldNPC.bLock;
			g_MapNpc.bFlag = oldNPC.bFlag;
			g_MapNpc.bCollide = oldNPC.bCollide;
			_tcscpy_s(g_MapNpc.szName, sizeof(g_MapNpc.szName)/sizeof(TCHAR), oldNPC.szName);

			pNewEdit = new MapObjEdit;
			pNewEdit->Create(g_MapNpc, pMap);

			Vector3 pos;
			pos = g_MapNpc.vPos;
			
			pNewEdit->SetWorldPos(pos);
	
			const tagCreatureProto* pAtt = pMap->GetAttDataBase()->FindCreatureAtt(g_MapNpc.dwTypeID);
			if( pAtt )
			{
				tstring szMdlPath = CreatureDataReader::GetCreatureMdlPath(pAtt);
				m_Cool3DView->LoadMdl(szMdlPath.c_str());	
			}
			EndMFCRes();
		}
		break;
	case GMAPOBJ_StaticObj:
		{
			pOldEdit = pMap->GetSelectedObj();
			if(pOldEdit==NULL)
			{
				AfxMessageBox(_T("请选择静态地物!"));
				SetFocus();
				return;
			}
			tagStaticMapObj oldStaticObj;			
			pOldEdit->GetData(oldStaticObj);
			g_StaticMapObj.dwMapID = m_dwGlobalID++; 
			g_StaticMapObj.bLock = oldStaticObj.bLock;
			g_StaticMapObj.bFlag = oldStaticObj.bFlag;
			g_StaticMapObj.bShowMiniMap = oldStaticObj.bShowMiniMap;
			g_StaticMapObj.bLightMap = oldStaticObj.bLightMap;
			g_StaticMapObj.byViewLevel = oldStaticObj.byViewLevel;
			g_StaticMapObj.bPathfinding = oldStaticObj.bPathfinding;
			g_StaticMapObj.bSurfaceType = oldStaticObj.bSurfaceType;
			g_StaticMapObj.fMtlDiffuseFactor = oldStaticObj.fMtlDiffuseFactor;
			g_StaticMapObj.bNoShadow = oldStaticObj.bNoShadow;
			g_StaticMapObj.bNotSupportNPCNavMap = oldStaticObj.bNotSupportNPCNavMap;
			g_StaticMapObj.byBoxSize = oldStaticObj.byBoxSize;
			_tcscpy_s(g_StaticMapObj.szMdlPath, sizeof(g_StaticMapObj.szMdlPath)/sizeof(TCHAR), oldStaticObj.szMdlPath);
			pNewEdit = new MapObjEdit;
			g_StaticMapObj.dwPickItemID = oldStaticObj.dwPickItemID;
			for(int i=0; i<3; i++)
			{
				g_StaticMapObj.fRotate[i] = oldStaticObj.fRotate[i];
				g_StaticMapObj.fScale[i] = oldStaticObj.fScale[i];
			}
			_tcscpy_s(g_StaticMapObj.szScriptName, sizeof(g_StaticMapObj.szScriptName)/sizeof(TCHAR), oldStaticObj.szScriptName);
			pNewEdit->Create(g_StaticMapObj, pMap);

			CString szMdlName;
			szMdlName.Format(_T("%s"), g_StaticMapObj.szMdlPath);
			int nlength=szMdlName.GetLength();
			int nNum=szMdlName.ReverseFind(_T('.'));
			CString szfiletype=szMdlName.Right(nlength-nNum-1);

			m_szStaticFileOpenType=_T("*.")+szfiletype;

			if(szfiletype!=_T("sfx") && szfiletype!=_T("SFX"))
			{
				m_Cool3DView->LoadMdl(szMdlName.GetBuffer());
			}
		}
		break;
	case GMAPOBJ_MapTriggerEffect:
		{
			pOldEdit = pMap->GetSelectedObj();
			if(pOldEdit==NULL)
			{
				AfxMessageBox(_T("请选择触发特效!"));
				SetFocus();
				return;
			}
			tagMapTriggerEffect oldTEObj;			
			pOldEdit->GetData(oldTEObj);
			g_MapTriggerEffect.dwObjID = m_dwGlobalID++; 
			g_MapTriggerEffect.bLock = oldTEObj.bLock;
			g_MapTriggerEffect.bFlag = oldTEObj.bFlag;
			_tcscpy_s(g_MapTriggerEffect.szMdlPath, sizeof(g_MapTriggerEffect.szMdlPath)/sizeof(TCHAR), oldTEObj.szMdlPath);
			pNewEdit = new MapObjEdit;
			for(int i=0; i<3; i++)
			{
				g_MapTriggerEffect.fRotate[i] = oldTEObj.fRotate[i];
				g_MapTriggerEffect.fScale[i] = oldTEObj.fScale[i];
			}			
			pNewEdit->Create(g_MapTriggerEffect, pMap);
		}
		break;
	case GMAPOBJ_DynamicBlock:
		{
			pOldEdit = pMap->GetSelectedObj();
			if(pOldEdit==NULL)
			{
				AfxMessageBox(_T("请选择动态障碍物!"));
				SetFocus();
				return;
			}
			tagMapDynamicBlock oldDynaBlock;
			pOldEdit->GetData(oldDynaBlock);

			g_DynamicBlock.dwObjID = m_dwGlobalID++; 
			g_DynamicBlock.bLock = oldDynaBlock.bLock;
			g_DynamicBlock.bFlag = oldDynaBlock.bFlag;
			g_DynamicBlock.fYaw = oldDynaBlock.fYaw;
			for(int i=0; i<3; i++)
			{
				g_DynamicBlock.fScale[i] = oldDynaBlock.fScale[i];
			}
			_tcscpy_s(g_DynamicBlock.szModelPath, sizeof(g_DynamicBlock.szModelPath)/sizeof(TCHAR), oldDynaBlock.szModelPath);
			pNewEdit = new MapObjEdit;
			pNewEdit->Create(g_DynamicBlock, pMap);

			CString szMdlName;
			szMdlName.Format(_T("%s"), g_DynamicBlock.szModelPath);
			int nlength=szMdlName.GetLength();
			int nNum=szMdlName.ReverseFind(_T('.'));
			CString szfiletype=szMdlName.Right(nlength-nNum-1);

			m_szStaticFileOpenType=_T("*.")+szfiletype;

			if(szfiletype!=_T("sfx") && szfiletype!=_T("SFX"))
			{
				m_Cool3DView->LoadMdl(szMdlName.GetBuffer());
			}
		}
		break;
	case GMAPOBJ_MapDoor:
		{
			pOldEdit = pMap->GetSelectedObj();
			if(pOldEdit==NULL)
			{
				AfxMessageBox(_T("请选择要复制的门!"));
				SetFocus();
				return;
			}
			tagMapDoor oldMapDoor;
			pOldEdit->GetData(oldMapDoor);

			g_MapDoor.dwObjID = m_dwGlobalID++; 
			g_MapDoor.bLock = oldMapDoor.bLock;
			g_MapDoor.bFlag = oldMapDoor.bFlag;
			g_MapDoor.fYaw = oldMapDoor.fYaw;
			for(int i=0; i<3; i++)
			{
				g_MapDoor.fScale[i] = oldMapDoor.fScale[i];
			}
			_tcscpy_s(g_MapDoor.szModelPath, sizeof(g_MapDoor.szModelPath)/sizeof(TCHAR), oldMapDoor.szModelPath);
			pNewEdit = new MapObjEdit;
			pNewEdit->Create(g_MapDoor, pMap);

			CString szMdlName;
			szMdlName.Format(_T("%s"), g_MapDoor.szModelPath);
			int nlength=szMdlName.GetLength();
			int nNum=szMdlName.ReverseFind(_T('.'));
			CString szfiletype=szMdlName.Right(nlength-nNum-1);

			m_szStaticFileOpenType=_T("*.")+szfiletype;

			if(szfiletype!=_T("sfx") && szfiletype!=_T("SFX"))
			{
				m_Cool3DView->LoadMdl(szMdlName.GetBuffer());
			}
		}
		break;
	case GMAPOBJ_Sound:
		{
			tagMapSound* pOldSound = pMap->GetSelectedSound();
			if(pOldSound==NULL)
			{
				AfxMessageBox(_T("请选择音乐!"));
				SetFocus();
				return;
			}
			tagMapSound* pSound=new tagMapSound;
			_tcscpy_s(pSound->szFileName,sizeof(pSound->szFileName)/sizeof(TCHAR),pOldSound->szFileName);
			pSound->fRange = pOldSound->fRange;
			pSound->fNoAtteRange = pOldSound->fNoAtteRange;
			pSound->fVolume = pOldSound->fVolume;
			pSound->bFlag = pOldSound->bFlag;
			pSound->bLock = pOldSound->bLock;
			pMap->SetSelectedSound(pSound);
			m_bIsBuildSound=true;
			m_objTypeList.EnableWindow(false);	
			pMap->SetDrawSoundFlag(true);
		}
		break;
	case GMAPOBJ_PointLight:
		{
			m_bCopyBuildPointLgt = true;
			m_objTypeList.EnableWindow(false);	
			pMap->SetDrawPointLightFlag(true);	
		}
		break;
	default:
		{
			AfxMessageBox(_T("请选择一个地图对象!"));
			SetFocus();
		}
		break;
	}
	if(pNewEdit)
	{
		SetPreCreate(pNewEdit);
		g_mapEditor->GetMap()->SetSelectObj(pNewEdit);
		szBuffer.Format(_T("%x"), pNewEdit->GetMapID());
		AddObjIDToObjList(szBuffer);
		//OnCbnSelchangeobjtypelist();
		m_objTypeList.EnableWindow(false);
	}
}

//按钮操作
//新增物体
void CMapObjDlg::OnBnClickedButtonAdd()
{
	if(g_mapEditor->GetMap()->GetHeightMap()->GetHeightmap()==NULL) 
	{
		AfxMessageBox(_T("请先导入地形"));
		SetFocus();
		return ;
	}

    //--new a waypoint
	if(NewWayPoint()) return;
	//--new a trigger
	if(NewTrigger())  return;
	//--new a sound
	if(NewSound())    return;
	//--
	if(NewNPCNavStartPoint()) return;
	//--
	if(NewPlayerNavStartPoint()) return;
	//--new a object
	if(NewObject())   return;	
}

bool CMapObjDlg::NewNPCNavStartPoint()
{
	//处理触发器的添加
	if(m_objTypeList.GetCurSel()==GMAPOBJ_NPCNavStartPoint)
	{
		//错误检测
		if(g_mapEditor->GetMap()->GetHeightMap()->GetHeightmap()==NULL) 
		{
			AfxMessageBox(_T("请先导入地形"));
			return true;
		}
		if(m_bIsBuildNpcNav) return true;
		DWORD objID=0;
		m_bIsBuildNpcNav=true;
		m_objTypeList.EnableWindow(false);
		return true;
	}
	return false;
}

bool CMapObjDlg::NewPlayerNavStartPoint()
{
	//处理触发器的添加
	if(m_objTypeList.GetCurSel()==GMAPOBJ_PlayerNavStartPoint)
	{
		//错误检测
		if(g_mapEditor->GetMap()->GetHeightMap()->GetHeightmap()==NULL) 
		{
			AfxMessageBox(_T("请先导入地形"));
			return true;
		}
		if(m_bIsBuildPlayerNav) return true;
		DWORD objID=0;
		m_bIsBuildPlayerNav=true;
		m_objTypeList.EnableWindow(false);
		return true;
	}
	return false;
}

//new a waypoint
bool CMapObjDlg::NewWayPoint()
{
	CString szstr;
	//处理导航点添加
	if(m_objTypeList.GetCurSel()==GMAPOBJ_WAYPOINT)
	{
		//错误检测
		if(g_mapEditor->GetMap()->GetHeightMap()->GetHeightmap()==NULL) 
		{
			AfxMessageBox(_T("请先导入地形"));
			return true;
		}
		BeginMFCRes();
		WayPointWnd wayPtWnd(this);
		if(IDOK==wayPtWnd.DoModal())
		{
			m_strWayPtName = wayPtWnd.m_strWayPtName;
		}

		if(m_strWayPtName==_T("")) 
		{
			AfxMessageBox(_T("请先输入导航点名称"));
			return true;
		}
		if(m_strWayPtName.GetLength()>31) 
		{
			AfxMessageBox(_T("输入的导航点名称过长，请减少名字长度"));
			return true;
		}
		if(g_mapEditor->GetMap()->IsExistWayPointWithSameName(m_strWayPtName.GetBuffer()))
		{
			AfxMessageBox(_T("输入的导航点名称已经存在,请换一个名称"));
			return true;
		}
		m_bIsBuildWayPoint=true;
		m_objTypeList.EnableWindow(false);
		EndMFCRes();
		return true;
	}
	return false;
}

//new a trigger
bool CMapObjDlg::NewTrigger()
{
	//处理触发器的添加
	if(m_objTypeList.GetCurSel()==GMAPOBJ_Trigger)
	{
		//错误检测
		if(g_mapEditor->GetMap()->GetHeightMap()->GetHeightmap()==NULL) 
		{
			AfxMessageBox(_T("请先导入地形"));
			return true;
		}
		if(m_bIsBuildTrigger) return true;
		DWORD objID=0;
		m_bIsBuildTrigger=true;
		m_objTypeList.EnableWindow(false);
		g_mapEditor->GetMap()->ClearTriggerNum();
		return true;
	}
	return false;
}


//new a object
bool CMapObjDlg::NewObject()
{
	CString szstr;
	//--向场景中增加一个对象
	MapObjEdit* pEdit = NULL;
	GameMapEditor*  pMap= g_mapEditor->GetMap();
	DWORD nIndex  = 0;
	int nType = m_objTypeList.GetCurSel();
	CString szBuffer, szMdlName;

	ASSERT(pMap != NULL);

	typedef list<MapObjEdit*> EDIT;
	EDIT pList;
	pList=pMap->GetMapObjEditList();
	EDIT::reverse_iterator pIter =pList.rbegin();
	switch(nType)
	{
	case CB_ERR:
		AfxMessageBox(_T("请先选择一个类型"));
		return true;
	case GMAPOBJ_Npc:
		{
			DWORD attID=0;
			BeginMFCRes();
			CSelAttDlg selAttDlg(this);
			selAttDlg.MyInit(_T("请选择一个Npc属性..."),MapObjEdit::NPC);
			selAttDlg.SetLastUsedNpcAtt(g_MapNpc.dwTypeID);
			if(IDOK==selAttDlg.DoModal())
			{
				attID=selAttDlg.GetAttSel();
				//资源汇总判断
				TCHAR szBuf[100];				
				_itot_s(attID, szBuf, 100, 10);
				CString temp = szBuf;
				vector<tagResGather>::iterator ite = m_vecRes.begin();
				bool bHad = false;
				for(; ite!=m_vecRes.end(); ++ite)
				{
					if(_tcscmp(temp.GetBuffer(), (*ite).szObj) == 0)
					{
						bHad = true;
						break;
					}
				}
				if(!bHad)
				{
					tagResGather tempRG;
					_itot_s(attID, szBuf, 100, 10);
					CString temp = szBuf;
					_tcscpy_s(tempRG.szObj, sizeof(tempRG.szObj)/sizeof(TCHAR), temp.GetBuffer());
					tempRG.eType = (GMAPOBJ_TYPE)nType;
					m_vecRes.push_back(tempRG);
					if(m_pResGatherDlg)
						m_pResGatherDlg->UpdateList();
				}

				g_MapNpc.dwObjID  = m_dwGlobalID++;
				g_MapNpc.dwTypeID  = attID;
				g_MapNpc.bCollide = false;
				ZeroMemory(g_MapNpc.szName,	X_SHORT_NAME);
				pEdit = new MapObjEdit;				
				pEdit->Create(g_MapNpc, pMap);

				Vector3 pos;
				pos = g_MapNpc.vPos;
				pEdit->SetWorldPos(pos);
				if(((CButton*)(GetDlgItem(IDC_CHECK_COPY_ADD)))->GetCheck())
				{
					pEdit->SetYaw(g_MapNpc.fYaw);
				}
				const tagCreatureProto* pAtt = pMap->GetAttDataBase()->FindCreatureAtt(g_MapNpc.dwTypeID);
				if( pAtt )
				{
					tstring szMdlPath = CreatureDataReader::GetCreatureMdlPath(pAtt);
					m_Cool3DView->LoadMdl(szMdlPath.c_str());	
				}
				EndMFCRes();
			}
		}
		break;
	case GMAPOBJ_StaticObj:
		{
			g_StaticMapObj.dwMapID = m_dwGlobalID++;
			g_StaticMapObj.bLightMap = true;
			g_StaticMapObj.byViewLevel = ESNVL_Auto;
			g_StaticMapObj.bShowMiniMap = false;
			g_StaticMapObj.bLock = false;
			g_StaticMapObj.bFlag = false;
			g_StaticMapObj.bPathfinding = false;
			g_StaticMapObj.bSurfaceType = 0;
			g_StaticMapObj.fMtlDiffuseFactor = 1.5f;
			g_StaticMapObj.bNoShadow = false;
			g_StaticMapObj.bNotSupportNPCNavMap = false;
			g_StaticMapObj.byBoxSize = 0;

			CFileDialog dlg(TRUE,NULL,m_szStaticFileOpenType,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,
				_T("(*.FSM)文件|*.FSM|(*.spt)文件|*.spt|(*.FSCN)文件|*.FSCN|(*.FAK)文件|*.FAK|(*.SFX)文件|*.SFX|"),
				this,0);
			if(IDOK == dlg.DoModal())
			{
				szMdlName = dlg.GetPathName();
				szMdlName.Delete(0, _tcslen(Kernel::Inst()->GetWorkPath()));
			}
			else
				return true;
			//资源汇总判断
			vector<tagResGather>::iterator ite = m_vecRes.begin();
			bool bHad = false;
			for(; ite!=m_vecRes.end(); ++ite)
			{
				if(_tcscmp(szMdlName.GetBuffer(), (*ite).szObj) == 0)
				{
					bHad = true;
					break;
				}
			}
			if(!bHad)
			{
				tagResGather tempRG;
				_tcscpy_s(tempRG.szObj, sizeof(tempRG.szObj)/sizeof(TCHAR), szMdlName.GetBuffer());
				tempRG.eType = (GMAPOBJ_TYPE)nType;
				m_vecRes.push_back(tempRG);
				if(m_pResGatherDlg)
					m_pResGatherDlg->UpdateList();
			}
	
			memset(g_StaticMapObj.szMdlPath, 0, MAX_PATH);
			_tcscpy_s(g_StaticMapObj.szMdlPath, MAX_PATH, szMdlName.GetBuffer() );
			pEdit = new MapObjEdit;

			Vector3 pos;
			pos.x = g_StaticMapObj.fPos[0];
			pos.y = g_StaticMapObj.fPos[1];
			pos.z = g_StaticMapObj.fPos[2];
			if(!((CButton*)(GetDlgItem(IDC_CHECK_COPY_ADD)))->GetCheck())
			{
				g_StaticMapObj.fRotate[0]=0.0f;
				g_StaticMapObj.fRotate[1]=0.0f;
				g_StaticMapObj.fRotate[2]=0.0f;

				g_StaticMapObj.fScale[0]=1.0f;
				g_StaticMapObj.fScale[1]=1.0f;
				g_StaticMapObj.fScale[2]=1.0f;
			}
			pEdit->Create(g_StaticMapObj, pMap);
			pEdit->SetWorldPos(pos);

			//--判别文件类型
			int nlength=szMdlName.GetLength();
			int nIndex=szMdlName.ReverseFind(_T('.'));
			CString szfiletype=szMdlName.Right(nlength-nIndex-1);

			m_szStaticFileOpenType=_T("*.")+szfiletype;

			if(szfiletype!=_T("sfx") && szfiletype!=_T("SFX"))
			{
				m_Cool3DView->LoadMdl(szMdlName.GetBuffer());
			}
		}
		break;
	case GMAPOBJ_MapTriggerEffect:
		{
			g_MapTriggerEffect.dwObjID = m_dwGlobalID++;
			CFileDialog dlg(TRUE,NULL,m_szStaticFileOpenType,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,
				_T("(*.SFX)文件|*.SFX|"),this,0);

			if(IDOK == dlg.DoModal())
			{
				szMdlName = dlg.GetPathName();
				szMdlName.Delete(0, _tcslen(Kernel::Inst()->GetWorkPath()));
			}
			else
				return true;

			memset(g_MapTriggerEffect.szMdlPath, 0, MAX_PATH);
			_tcscpy_s(g_MapTriggerEffect.szMdlPath, MAX_PATH, szMdlName.GetBuffer() );
			pEdit = new MapObjEdit;

			Vector3 pos;
			pos.x = g_MapTriggerEffect.fPos[0];
			pos.y = g_MapTriggerEffect.fPos[1];
			pos.z = g_MapTriggerEffect.fPos[2];
			if(!((CButton*)(GetDlgItem(IDC_CHECK_COPY_ADD)))->GetCheck())
			{
				g_MapTriggerEffect.fRotate[0]=0.0f;
				g_MapTriggerEffect.fRotate[1]=0.0f;
				g_MapTriggerEffect.fRotate[2]=0.0f;

				g_MapTriggerEffect.fScale[0]=1.0f;
				g_MapTriggerEffect.fScale[1]=1.0f;
				g_MapTriggerEffect.fScale[2]=1.0f;
			}
			pEdit->Create(g_MapTriggerEffect, pMap);
			pEdit->SetWorldPos(pos);
		}
		break;
	case GMAPOBJ_DynamicBlock:
		{
			g_DynamicBlock.dwObjID = m_dwGlobalID++;

			CFileDialog dlg(TRUE,NULL,m_szStaticFileOpenType,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,
				_T("(*.FSM)文件|*.FSM|(*.spt)文件|*.spt|(*.FSCN)文件|*.FSCN|(*.FAK)文件|*.FAK|(*.SFX)文件|*.SFX|"),
				this,0);
			if(IDOK == dlg.DoModal())
			{
				szMdlName = dlg.GetPathName();
				szMdlName.Delete(0, _tcslen(Kernel::Inst()->GetWorkPath()));
			}
			else
				return true;
			//资源汇总判断
			vector<tagResGather>::iterator ite = m_vecRes.begin();
			bool bHad = false;
			for(; ite!=m_vecRes.end(); ++ite)
			{
				if(_tcscmp(szMdlName.GetBuffer(), (*ite).szObj) == 0)
				{
					bHad = true;
					break;
				}
			}
			if(!bHad)
			{
				tagResGather tempRG;
				_tcscpy_s(tempRG.szObj, sizeof(tempRG.szObj)/sizeof(TCHAR), szMdlName.GetBuffer());
				tempRG.eType = (GMAPOBJ_TYPE)nType;
				m_vecRes.push_back(tempRG);
				if(m_pResGatherDlg)
					m_pResGatherDlg->UpdateList();
			}

			memset(g_DynamicBlock.szModelPath, 0, X_LONG_NAME);
			_tcscpy_s(g_DynamicBlock.szModelPath, X_LONG_NAME, szMdlName.GetBuffer() );
			pEdit = new MapObjEdit;

			Vector3 pos;
			pos = g_DynamicBlock.vPos;
			if(!((CButton*)(GetDlgItem(IDC_CHECK_COPY_ADD)))->GetCheck())
			{
				g_DynamicBlock.fYaw = 0.0f;
				g_DynamicBlock.fScale[0] = 1.0f;
				g_DynamicBlock.fScale[1] = 1.0f;
				g_DynamicBlock.fScale[2] = 1.0f;
			}
			pEdit->Create(g_DynamicBlock, pMap);
			pEdit->SetWorldPos(pos);

			//--判别文件类型
			int nlength=szMdlName.GetLength();
			int nIndex=szMdlName.ReverseFind(_T('.'));
			CString szfiletype=szMdlName.Right(nlength-nIndex-1);

			m_szStaticFileOpenType=_T("*.")+szfiletype;

			if(szfiletype!=_T("sfx") && szfiletype!=_T("SFX"))
			{
				m_Cool3DView->LoadMdl(szMdlName.GetBuffer());
			}
		}
		break;
	case GMAPOBJ_MapDoor:
		{
			g_MapDoor.dwObjID = m_dwGlobalID++;

			CFileDialog dlg(TRUE,NULL,m_szStaticFileOpenType,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,
				_T("(*.FSM)文件|*.FSM|(*.spt)文件|*.spt|(*.FSCN)文件|*.FSCN|(*.FAK)文件|*.FAK|(*.SFX)文件|*.SFX|"),
				this,0);
			if(IDOK == dlg.DoModal())
			{
				szMdlName = dlg.GetPathName();
				szMdlName.Delete(0, _tcslen(Kernel::Inst()->GetWorkPath()));
			}
			else
				return true;
			//资源汇总判断
			vector<tagResGather>::iterator ite = m_vecRes.begin();
			bool bHad = false;
			for(; ite!=m_vecRes.end(); ++ite)
			{
				if(_tcscmp(szMdlName.GetBuffer(), (*ite).szObj) == 0)
				{
					bHad = true;
					break;
				}
			}
			if(!bHad)
			{
				tagResGather tempRG;
				_tcscpy_s(tempRG.szObj, sizeof(tempRG.szObj)/sizeof(TCHAR), szMdlName.GetBuffer());
				tempRG.eType = (GMAPOBJ_TYPE)nType;
				m_vecRes.push_back(tempRG);
				if(m_pResGatherDlg)
					m_pResGatherDlg->UpdateList();
			}

			memset(g_MapDoor.szModelPath, 0, X_LONG_NAME);
			_tcscpy_s(g_MapDoor.szModelPath, X_LONG_NAME, szMdlName.GetBuffer() );
			pEdit = new MapObjEdit;

			Vector3 pos;
			pos = g_MapDoor.vPos;
			if(!((CButton*)(GetDlgItem(IDC_CHECK_COPY_ADD)))->GetCheck())
			{
				g_MapDoor.fYaw = 0.0f;
				g_MapDoor.fScale[0] = 1.0f;
				g_MapDoor.fScale[1] = 1.0f;
				g_MapDoor.fScale[2] = 1.0f;
			}
			g_MapDoor.bInitState = FALSE;
			g_MapDoor.bBlockingTileNPC = FALSE;
			pEdit->Create(g_MapDoor, pMap);
			pEdit->SetWorldPos(pos);

			//--判别文件类型
			int nlength=szMdlName.GetLength();
			int nIndex=szMdlName.ReverseFind(_T('.'));
			CString szfiletype=szMdlName.Right(nlength-nIndex-1);

			m_szStaticFileOpenType=_T("*.")+szfiletype;

			if(szfiletype!=_T("sfx") && szfiletype!=_T("SFX"))
			{
				m_Cool3DView->LoadMdl(szMdlName.GetBuffer());
			}
		}
		break;
	case GMAPOBJ_MapCarrier:
		{
			g_MapCarrier.carrier.dwObjID = m_dwGlobalID++;

			CFileDialog dlg(TRUE,NULL,m_szStaticFileOpenType,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,
				_T("(*.FSM)文件|*.FSM|(*.spt)文件|*.spt|(*.FSCN)文件|*.FSCN|(*.FAK)文件|*.FAK|(*.SFX)文件|*.SFX|"),
				this,0);
			if(IDOK == dlg.DoModal())
			{
				szMdlName = dlg.GetPathName();
				szMdlName.Delete(0, _tcslen(Kernel::Inst()->GetWorkPath()));
			}
			else
				return true;

			memset(g_MapCarrier.carrier.szModelPath, 0, X_LONG_NAME);
			_tcscpy_s(g_MapCarrier.carrier.szModelPath, X_LONG_NAME, szMdlName.GetBuffer() );
			memset(g_MapCarrier.carrier.szWayPoint, 0, X_SHORT_NAME);
			pEdit = new MapObjEdit;
			g_MapCarrier.carrier.fYaw = 0.0f;
			g_MapCarrier.carrier.bSurfaceType = 0;
			g_MapCarrier.carrier.fSpeed = 500.0f;
			Vector3 pos(0,0,0);
	
			pEdit->Create(g_MapCarrier.carrier, pMap);
			pEdit->SetWorldPos(pos);

			//--判别文件类型
			int nlength=szMdlName.GetLength();
			int nIndex=szMdlName.ReverseFind(_T('.'));
			CString szfiletype=szMdlName.Right(nlength-nIndex-1);

			m_szStaticFileOpenType=_T("*.")+szfiletype;

			if(szfiletype!=_T("sfx") && szfiletype!=_T("SFX"))
			{
				m_Cool3DView->LoadMdl(szMdlName.GetBuffer());
			}

		}
		break;
	case GMAPOBJ_PointLight:
		{
			m_bIsBuildPointLight = true;
			m_objTypeList.EnableWindow(false);
			pMap->SetDrawPointLightFlag(true);	
			pMap->SetSelectedPointLight(NULL);
		}
		break;
	case GMAPOBJ_MapRect:
		{
			//错误检测
			if(g_mapEditor->GetMap()->GetHeightMap()->GetHeightmap()==NULL) 
			{
				AfxMessageBox(_T("请先导入地形"));
				return true;
			}
			m_bIsBuildMapRect=true;
			m_objTypeList.EnableWindow(false);
			pMap->SetSelectedMapRect(NULL);
			pMap->ClearMapAreaNum();
		}
		break;
	case GMAPOBJ_PathPoint:
		{
			m_bIsBuildPathPoint = true;
			m_objTypeList.EnableWindow(false);
			pMap->SetSelectedPathPoint(NULL);
		}
		break;
	case GMAPOBJ_SpawnPoint:
		{
			m_bIsBuildSpawnPoint = true;
			m_objTypeList.EnableWindow(false);
			pMap->SetSelectedSpawnPoint(NULL);
		}
		break;
	case GMAPOBJ_EventTrigger:
		{
			tagMapEventTrigger eventTrigger;
			eventTrigger.dwObjID = m_dwGlobalID++;
			eventTrigger.eCondition = ETC_Null;
			eventTrigger.dwConditionParam1 = 0;
			eventTrigger.dwConditionParam2 = 0;
			eventTrigger.eResult = ETR_Null;
			eventTrigger.dwResultParam1 = 0;
			eventTrigger.dwResultParam2 = 0;
			MapObjEdit* pEditEvent = NULL;
			pEditEvent = new MapObjEdit;
			pEditEvent->Create(eventTrigger, pMap);
			szBuffer.Format(_T("%x"), eventTrigger.dwObjID);
			AddObjIDToObjList(szBuffer);
			UpdatePropListForEventTrigger(pEditEvent);
		}
		break;
	default:
		ASSERT(0 && "Unknow Type!");
		break;
	}
	//--
	if(pEdit)
	{
		SetPreCreate(pEdit);
		pMap->SetSelectObj(pEdit);
		szBuffer.Format(_T("%x"), pEdit->GetMapID());
	    AddObjIDToObjList(szBuffer);
		m_objTypeList.EnableWindow(false);
		return true;
	}
	return false;
}

//-new a sound
bool CMapObjDlg::NewSound()
{
	if(m_objTypeList.GetCurSel()==GMAPOBJ_Sound)
	{
		CString szFileName;
		CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,
			_T("(*.*)|*.*|"),this,0);
		if(IDOK == dlg.DoModal())
		{
			szFileName = dlg.GetPathName();
			szFileName.Delete(0, _tcslen(Kernel::Inst()->GetWorkPath()));
		}
		else
			return true;

		tagMapSound*  pSound=new tagMapSound;
	    _tcscpy_s(pSound->szFileName,sizeof(pSound->szFileName)/sizeof(TCHAR),szFileName);
	
		//资源汇总判断
		vector<tagResGather>::iterator ite = m_vecRes.begin();
		bool bHad = false;
		for(; ite!=m_vecRes.end(); ++ite)
		{
			if(_tcscmp(szFileName.GetBuffer(), (*ite).szObj) == 0)
			{
				bHad = true;
				break;
			}
		}
		if(!bHad)
		{
			tagResGather tempRG;
			_tcscpy_s(tempRG.szObj,sizeof(tempRG.szObj)/sizeof(TCHAR),szFileName.GetBuffer());
			tempRG.eType = GMAPOBJ_Sound;
			m_vecRes.push_back(tempRG);
			if(m_pResGatherDlg)
				m_pResGatherDlg->UpdateList();
		}
        
		g_mapEditor->GetMap()->SetSelectedSound(pSound);
		m_bIsBuildSound=true;
		m_objTypeList.EnableWindow(false);
		g_mapEditor->GetMap()->SetDrawSoundFlag(true);
		return true;
	}
	return false;
}


void CMapObjDlg::OnCbnSelchangeobjtypelist()
{
	m_PreObjListSelectedIndex=-1;
	m_objList.DeleteAllItems();
	m_propList.DeleteAllItem();
	((CButton*)(GetDlgItem(IDC_CHECK_COPY_ADD)))->EnableWindow(false);
	int nType = m_objTypeList.GetCurSel();
	GameMapEditor*  pMap= g_mapEditor->GetMap();
	ASSERT(pMap != NULL);
	switch(nType)
	{
	case GMAPOBJ_Npc:
	//case GMAPOBJ_Monster:
		DisplayList(MapObjEdit::NPC);
		((CButton*)(GetDlgItem(IDC_CHECK_COPY_ADD)))->EnableWindow(true);
		m_CopyAdd.EnableWindow(true);
		break;
	case GMAPOBJ_StaticObj:
		DisplayList(MapObjEdit::StaticObj);
		((CButton*)(GetDlgItem(IDC_CHECK_COPY_ADD)))->EnableWindow(true);
		m_CopyAdd.EnableWindow(true);
		break;
	case GMAPOBJ_DynamicBlock:
		DisplayList(MapObjEdit::DynamicBlock);
		((CButton*)(GetDlgItem(IDC_CHECK_COPY_ADD)))->EnableWindow(true);
		m_CopyAdd.EnableWindow(true);
		break;
	case GMAPOBJ_MapDoor:
		DisplayList(MapObjEdit::MapDoor);
		((CButton*)(GetDlgItem(IDC_CHECK_COPY_ADD)))->EnableWindow(true);
		m_CopyAdd.EnableWindow(true);
		break;
	case GMAPOBJ_MapCarrier:
		DisplayList(MapObjEdit::MapCarrier);
		((CButton*)(GetDlgItem(IDC_CHECK_COPY_ADD)))->EnableWindow(true);
		m_CopyAdd.EnableWindow(false);
		break;
	case GMAPOBJ_MapTriggerEffect:
		DisplayList(MapObjEdit::MapTrrigerEffect);
		((CButton*)(GetDlgItem(IDC_CHECK_COPY_ADD)))->EnableWindow(true);
		m_CopyAdd.EnableWindow(false);
		break;
	case GMAPOBJ_WAYPOINT:
		DisplayWayPointList();
		m_CopyAdd.EnableWindow(false);
		break;	
	case GMAPOBJ_Trigger:
		DisplayTriggerList();
		m_CopyAdd.EnableWindow(false);
		break;	
	case GMAPOBJ_Sound:
		DisplaySound();
		m_CopyAdd.EnableWindow(true);
		break;
	case GMAPOBJ_PointLight:
		DisplayPointLightList();
		m_CopyAdd.EnableWindow(true);
		break;
	case GMAPOBJ_MapRect:
		DisplayMapRectList();
		m_CopyAdd.EnableWindow(false);
		break;
	case GMAPOBJ_PathPoint:
		DisplayPathPoint();
		m_CopyAdd.EnableWindow(false);
		break;
	case GMAPOBJ_SpawnPoint:
		DisplaySpawnPoint();
		m_CopyAdd.EnableWindow(false);
		break;
	case GMAPOBJ_EventTrigger:
		DisplayEventTrigger();
		m_CopyAdd.EnableWindow(false);
		break;
	case GMAPOBJ_NPCNavStartPoint:
		DisplayNPCNav();
		m_CopyAdd.EnableWindow(false);
		break;
	case GMAPOBJ_PlayerNavStartPoint:
		DisplayPlayerNav();
		m_CopyAdd.EnableWindow(false);
		break;
	default:
		m_CopyAdd.EnableWindow(false);
		break;
	}
	m_byColumn = -1;
	g_mapEditor->SetPickFlag();
	m_pParent->Invalidate();
}

void CMapObjDlg::OnBnClickedDel()
{	
	POSITION pos = m_objList.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		CString szBuffer;
		int nItem = m_objList.GetNextSelectedItem(pos);
		szBuffer = m_objList.GetItemText(nItem , 0);
		
		//删除一般物体
	    if(m_objTypeList.GetCurSel()!=GMAPOBJ_WAYPOINT 
			&& m_objTypeList.GetCurSel()!=GMAPOBJ_Trigger 
			&& m_objTypeList.GetCurSel()!=GMAPOBJ_Sound 
			&& m_objTypeList.GetCurSel()!=GMAPOBJ_PointLight 
		   && m_objTypeList.GetCurSel()!=GMAPOBJ_MapRect
		   && m_objTypeList.GetCurSel()!=GMAPOBJ_PathPoint
		   && m_objTypeList.GetCurSel()!=GMAPOBJ_SpawnPoint
		   && m_objTypeList.GetCurSel()!=GMAPOBJ_EventTrigger
		   && m_objTypeList.GetCurSel()!=GMAPOBJ_NPCNavStartPoint
		   && m_objTypeList.GetCurSel()!=GMAPOBJ_PlayerNavStartPoint)
		{
		   GameMapEditor* pMap = g_mapEditor->GetMap();
		   ASSERT(pMap != NULL);

		   MapObjEdit* pEdit = pMap->GetSelectedObj();
		   if(pEdit != NULL)
		   {
		    	pMap->RemoveMapObjEdit(pEdit);
			    pMap->SetSelectObj(NULL);
		   }
		   else 
		   {
			    MapObjEdit::EType type;
			    if(m_objTypeList.GetCurSel() == GMAPOBJ_Npc)
				   type = MapObjEdit::NPC;
			    if(m_objTypeList.GetCurSel() == GMAPOBJ_StaticObj)
				   type = MapObjEdit::StaticObj;
				if(m_objTypeList.GetCurSel() == GMAPOBJ_DynamicBlock)
					type = MapObjEdit::DynamicBlock;
				if(m_objTypeList.GetCurSel() == GMAPOBJ_MapDoor)
					type = MapObjEdit::MapDoor;
				if(m_objTypeList.GetCurSel() == GMAPOBJ_MapCarrier)
					type = MapObjEdit::MapCarrier;
				if(m_objTypeList.GetCurSel() == GMAPOBJ_MapTriggerEffect)
					type = MapObjEdit::MapTrrigerEffect;
			    int nIndex  = _tcstoul(szBuffer, NULL, 16);
			    pEdit = pMap->RemoveMapObjEdit(type, nIndex);
		   }

		   if(g_mapEditor->GetLock() == pEdit)
		 	  g_mapEditor->SetLock(NULL);
		   if(m_pPreCreated == pEdit)
			  m_pPreCreated = NULL;
		   delete pEdit;
		}
        //删除导航点
		if(m_objTypeList.GetCurSel()==GMAPOBJ_WAYPOINT)
		{
			DWORD nIndex = _tcstoul(szBuffer, NULL, 16);
			g_mapEditor->GetMap()->DeleteWayPoint(nIndex);
		}
        //删除触发器
		if(m_objTypeList.GetCurSel()==GMAPOBJ_Trigger)
		{
			DWORD nIndex = _tcstoul(szBuffer, NULL, 16);
			g_mapEditor->GetMap()->DeleteTrigger(nIndex);
		}
		//删除音效
		if(m_objTypeList.GetCurSel()==GMAPOBJ_Sound)
		{
			DWORD nIndex = _tcstoul(szBuffer, NULL, 16);
			g_mapEditor->GetMap()->DeleteSound(nIndex);
		}
		//删除点光源
		if(m_objTypeList.GetCurSel()==GMAPOBJ_PointLight)
		{
			DWORD dwID = _tcstoul(szBuffer, NULL, 16);
			g_mapEditor->GetMap()->DeletePointLight(dwID);
		}
		//删除地图区域
		if(m_objTypeList.GetCurSel()==GMAPOBJ_MapRect)
		{
			DWORD dwID = _tcstoul(szBuffer, NULL, 16);
			g_mapEditor->GetMap()->DeleteMapRect(dwID);
		}
		//删除寻路导航点
		if(m_objTypeList.GetCurSel()==GMAPOBJ_PathPoint)
		{
			DWORD dwID = _tcstoul(szBuffer, NULL, 16);
			g_mapEditor->GetMap()->DeletePathPoint(dwID);
		}
		//删除刷怪点
		if(m_objTypeList.GetCurSel()==GMAPOBJ_SpawnPoint)
		{
			DWORD dwID = _tcstoul(szBuffer, NULL, 16);
			g_mapEditor->GetMap()->DeleteSpawnPoint(dwID);
		}
		//删除事件触发器
		if(m_objTypeList.GetCurSel()==GMAPOBJ_EventTrigger)
		{
			DWORD dwID = _tcstoul(szBuffer, NULL, 16);
			g_mapEditor->GetMap()->DeleteEventTrigger(dwID);
		}
		//
		if(m_objTypeList.GetCurSel()==GMAPOBJ_NPCNavStartPoint)
		{
			DWORD dwID = _tcstoul(szBuffer, NULL, 16);
			g_mapEditor->GetMap()->DeleteNPCNavStartPoint(dwID);
		}
		if(m_objTypeList.GetCurSel()==GMAPOBJ_PlayerNavStartPoint)
		{
			DWORD dwID = _tcstoul(szBuffer, NULL, 16);
			g_mapEditor->GetMap()->DeletePlayerNavStartPoint(dwID);
		}
		//删除其他
		//。。。。。。。。。

		m_propList.DeleteAllItem();
		m_objList.DeleteItem(nItem);
		if(m_objList.GetItemState(nItem, LVIS_SELECTED) == nItem)
			m_objList.SetItemState(nItem,0, LVIS_SELECTED);
	}
	m_pParent->Invalidate();
}

void CMapObjDlg::OnNMClickobjlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	int row = 0;
	int col = 0;
	if(pNMHDR!=NULL)
	{
		NMLISTVIEW* pView =(NMLISTVIEW*)pNMHDR; 

		row = pView->iItem;		//row: 文本框成员变量，用来存放行
		col = pView->iSubItem;  //col: 文本框成员变量，用来存放列  
	}

	POSITION pos = m_objList.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		m_propList.DeleteAllItem();
		int nItem = m_objList.GetNextSelectedItem(pos);
		CString szBuffer = m_objList.GetItemText(nItem , 0);

		//导航点特别处理
		if(m_objTypeList.GetCurSel()==GMAPOBJ_WAYPOINT)
		{
			tagMapWayPoint * pWayPoint;
			DWORD nObjId= _tcstoul(szBuffer, NULL, 16);
			pWayPoint=g_mapEditor->GetMap()->GetWayPointData(nObjId);
			if(pWayPoint)
			{
				g_mapEditor->GetMap()->SetCurPickedWayPoint(pWayPoint);
				m_propList.DisplayWayPointObj(pWayPoint);
				if(LOCK_COL == col)
				{
					if(pWayPoint->bLock)
						m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
					else
						m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Lock, 0, 0, 0, 0);
					pWayPoint->bLock = !pWayPoint->bLock;
				}
				else if(FLAG_COL == col)
				{	
					if(pWayPoint->bLock)
						m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
					else
						m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Flag, 0, 0, 0, 0);
					pWayPoint->bFlag = !pWayPoint->bFlag;
				}
			}
			return;
		}
		//触发器处理
		if(m_objTypeList.GetCurSel()==GMAPOBJ_Trigger)
		{
			tagMapTrigger * pTrigger=NULL;
			DWORD nIndex = _tcstoul(szBuffer, NULL, 16);
			pTrigger=g_mapEditor->GetMap()->GetTriggerData(nIndex);
			if(pTrigger)
			{
				g_mapEditor->GetMap()->SetCurPickedTrigger(pTrigger);
				m_propList.DisplayTriggerObj(pTrigger);
				if(LOCK_COL == col)
				{
					if(pTrigger->bLock)
						m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
					else
						m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Lock, 0, 0, 0, 0);
					pTrigger->bLock = !pTrigger->bLock;
				}
				else if(FLAG_COL == col)
				{
					if(pTrigger->bFlag)
						m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
					else
						m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Flag, 0, 0, 0, 0);
					pTrigger->bFlag = !pTrigger->bFlag;
				}
			}
			return;
		}
		//地图区域处理
		if(m_objTypeList.GetCurSel()==GMAPOBJ_MapRect)
		{
			MapObjEdit* pEdit = NULL;
			tagMapArea* pMapRect = NULL;
			DWORD nIndex = _tcstoul(szBuffer, NULL, 16);
			pEdit = g_mapEditor->GetMap()->FindMapRect(nIndex);
			if(pEdit==NULL)
				return;
			pMapRect = (tagMapArea*)pEdit->GetMapRect();
			if(pMapRect)
			{
				g_mapEditor->GetMap()->SetSelectedMapRect(pEdit);
				m_propList.DisplayMapRectObj(pMapRect);
				if(LOCK_COL == col)
				{
					if(pMapRect->bLock)
						m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
					else
						m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Lock, 0, 0, 0, 0);
					pMapRect->bLock = !pMapRect->bLock;
				}
				else if(FLAG_COL == col)
				{
					if(pMapRect->bFlag)
						m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
					else
						m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Flag, 0, 0, 0, 0);
					pMapRect->bFlag = !pMapRect->bFlag;
				}
			}

		}
		//音效处理
		if(m_objTypeList.GetCurSel()==GMAPOBJ_Sound)
		{
			tagMapSound * pSound=NULL;
			DWORD nIndex = _tcstoul(szBuffer, NULL, 16);
			pSound=g_mapEditor->GetMap()->FindSound(nIndex);
			if(pSound)
			{
				g_mapEditor->GetMap()->SetSelectedSound(pSound);
				m_propList.DisplaySoundObj(pSound);
				if(LOCK_COL == col)
				{
					if(pSound->bLock)
						m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
					else
						m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Lock, 0, 0, 0, 0);
					pSound->bLock = !pSound->bLock;
				}
				else if(FLAG_COL == col)
				{
					if(pSound->bFlag)
						m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
					else
						m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Flag, 0, 0, 0, 0);
					pSound->bFlag = !pSound->bFlag;
				}
			}
			return;
		}

		//寻路导航点
		if(m_objTypeList.GetCurSel()==GMAPOBJ_PathPoint)
		{
			MapObjEdit* pEdit = NULL;
			tagMapPathPoint* pPathPoint = NULL;
			DWORD nIndex = _tcstoul(szBuffer, NULL, 16);
			pEdit=g_mapEditor->GetMap()->FindPathPoint(nIndex);
			pPathPoint = (tagMapPathPoint*)pEdit->GetPathPoint();
			if(pPathPoint)
			{
				g_mapEditor->GetMap()->SetSelectedPathPoint(pEdit);
				m_propList.DisplayPathPoint(pPathPoint);
				if(LOCK_COL == col)
				{
					if(pPathPoint->bLock)
						m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
					else
						m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Lock, 0, 0, 0, 0);
					pPathPoint->bLock = !pPathPoint->bLock;
				}
				else if(FLAG_COL == col)
				{
					if(pPathPoint->bFlag)
						m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
					else
						m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Flag, 0, 0, 0, 0);
					pPathPoint->bFlag = !pPathPoint->bFlag;
				}
			}
		}

		//刷怪点
		if(m_objTypeList.GetCurSel()==GMAPOBJ_SpawnPoint)
		{
			MapObjEdit* pEdit = NULL;
			tagMapSpawnPoint* pSpawnPoint = NULL;
			DWORD nIndex = _tcstoul(szBuffer, NULL, 16);
			pEdit=g_mapEditor->GetMap()->FindSpawnPoint(nIndex);
			pSpawnPoint = (tagMapSpawnPoint*)pEdit->GetSpawnPoint();
			if(pSpawnPoint)
			{
				g_mapEditor->GetMap()->SetSelectedSpawnPoint(pEdit);
				m_propList.DisplaySpawnPoint(pSpawnPoint);
			}
		}
		
		//
		if(m_objTypeList.GetCurSel()==GMAPOBJ_NPCNavStartPoint)
		{
			tagNPCNavMapBuildStartPoint* pEdit = NULL;
			DWORD nIndex = _tcstoul(szBuffer, NULL, 16);
			pEdit = g_mapEditor->GetMap()->FindNPCNavStartPoint(nIndex);
			if(pEdit)
			{
				g_mapEditor->GetMap()->SetSelectedNPCNavStartPoint(pEdit);
				m_propList.DisplayNPCNav(pEdit);
			}
		}

		//
		if(m_objTypeList.GetCurSel()==GMAPOBJ_PlayerNavStartPoint)
		{
			tagPlayerNavMapBuildStartPoint* pEdit = NULL;
			DWORD nIndex = _tcstoul(szBuffer, NULL, 16);
			pEdit = g_mapEditor->GetMap()->FindPlayerNavStartPoint(nIndex);
			if(pEdit)
			{
				g_mapEditor->GetMap()->SetSelectedPlayerNavStartPoint(pEdit);
				m_propList.DisplayPlayerNav(pEdit);
			}
		}



		//点光源处理
		if(m_objTypeList.GetCurSel()==GMAPOBJ_PointLight)
		{
			MapObjEdit* pEdit = NULL;
			tagMapPointLight* pPointLight=NULL;
			DWORD nIndex = _tcstoul(szBuffer, NULL, 16);
			pEdit=g_mapEditor->GetMap()->FindPointLight(nIndex);
			pPointLight = (tagMapPointLight*)pEdit->GetPointLgtObj();
			if(pPointLight)
			{
				g_mapEditor->GetMap()->SetSelectedPointLight(pEdit);
				m_propList.DisplayPointLightObj(pPointLight);
				if(LOCK_COL == col)
				{
					if(pPointLight->bLock)
						m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
					else
						m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Lock, 0, 0, 0, 0);
					pPointLight->bLock = !pPointLight->bLock;
				}
				else if(FLAG_COL == col)
				{
					if(pPointLight->bFlag)
						m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
					else
						m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Flag, 0, 0, 0, 0);
					pPointLight->bFlag = !pPointLight->bFlag;
				}
			}
			return;
		}

		if(m_objTypeList.GetCurSel()==GMAPOBJ_EventTrigger)
		{
			MapObjEdit* pEdit = NULL;
			tagMapEventTrigger* pEventTrigger = NULL;
			DWORD nIndex = _tcstoul(szBuffer, NULL, 16);
			pEdit = g_mapEditor->GetMap()->FindEventTrigger(nIndex);
			pEventTrigger = (tagMapEventTrigger*)pEdit->GetEventTrigger();
			if(pEventTrigger)
			{
				m_propList.DisplayEventTrigger(pEventTrigger);
			}
		}

	    //处理一般物体
		DWORD nIndex = _tcstoul(szBuffer, NULL, 16);
		Vector3 pos;

		typedef list<MapObjEdit*> EDIT;
		EDIT pList;
		pList=g_mapEditor->GetMap()->GetMapObjEditList();
		EDIT::iterator pIter = pList.begin();
		switch(m_objTypeList.GetCurSel())
		{
		case GMAPOBJ_Npc:	
				for(; pIter != pList.end(); ++pIter)
				{
				  if((*pIter)->GetType() == MapObjEdit::NPC)
					{
						if((*pIter)->GetMapID() == nIndex)
						{
							memset(&g_MapNpc, 0, sizeof(g_MapNpc));
							(*pIter)->GetData(g_MapNpc);
							m_propList.DisplayNPC(&g_MapNpc);
							pos = g_MapNpc.vPos;						

							//const tagNPCAtt *pAtt;=g_mapEditor->GetMap()->GetAttDataBase()->FindNpcAtt(g_MapNpc.dwAttID);
							//tstring szMdlPath;//=GAttDataBase::GetNpcMdlPath(pAtt);
							//m_Cool3DView->LoadMdl(szMdlPath.c_str());																				
							g_mapEditor->GetMap()->SetSelectObj(*pIter);

							tagMapNPC* p = (tagMapNPC*)(*pIter)->GetMapNPC();
							if(LOCK_COL == col)
							{
								if(p->bLock)						
									m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
								else					
									m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Lock, 0, 0, 0, 0);
								p->bLock = !p->bLock;
							}					
							else if(FLAG_COL == col)
							{
								if(p->bFlag)						
									m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);					
								else						
									m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Flag, 0, 0, 0, 0);					
								p->bFlag = !p->bFlag;
							}
							break;
						}
					}
				}
			
			break;
		case GMAPOBJ_StaticObj:
			for(; pIter != pList.end(); ++pIter)
				if((*pIter)->GetType() == MapObjEdit::StaticObj)
				{
					if((*pIter)->GetMapID() == nIndex)
					{
						memset(&g_StaticMapObj, 0, sizeof(g_StaticMapObj));
						(*pIter)->GetData(g_StaticMapObj);
						m_propList.DisplayStaticObj(&g_StaticMapObj);

						pos.x = g_StaticMapObj.fPos[0];
						pos.y = g_StaticMapObj.fPos[1];
						pos.z = g_StaticMapObj.fPos[2];	

						//--判别文件类型
						CString szMdlName=CString(g_StaticMapObj.szMdlPath);
						int nlength=szMdlName.GetLength();
						int nIndex=szMdlName.ReverseFind(_T('.'));
						CString szfiletype=szMdlName.Right(nlength-nIndex-1);

						if(szfiletype!=_T("sfx") && szfiletype!=_T("SFX"))
						{
							m_Cool3DView->LoadMdl(szMdlName.GetBuffer());
						}	
						g_mapEditor->GetMap()->SetSelectObj(*pIter);

						tagStaticMapObj* p = (tagStaticMapObj*)(*pIter)->GetStaticMapObj();
						if(LOCK_COL == col)
						{
							if(p->bLock)						
								m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);							
							else					
								m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Lock, 0, 0, 0, 0);
							p->bLock = !p->bLock;
						}					
						else if(FLAG_COL == col)
						{
							if(p->bFlag)						
								m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);					
							else						
								m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Flag, 0, 0, 0, 0);					
							p->bFlag = !p->bFlag;
						}
						break;
					}
				}
				break;
		case GMAPOBJ_MapTriggerEffect:
			{
				for(; pIter != pList.end(); ++pIter)
					if((*pIter)->GetType() == MapObjEdit::MapTrrigerEffect)
					{
						if((*pIter)->GetMapID() == nIndex)
						{
							memset(&g_MapTriggerEffect, 0, sizeof(g_MapTriggerEffect));
							(*pIter)->GetData(g_MapTriggerEffect);
							m_propList.DisplayMapTriggerEffect(&g_MapTriggerEffect);

							pos.x = g_MapTriggerEffect.fPos[0];
							pos.y = g_MapTriggerEffect.fPos[1];
							pos.z = g_MapTriggerEffect.fPos[2];	
							
							g_mapEditor->GetMap()->SetSelectObj(*pIter);

							tagMapTriggerEffect* p = (tagMapTriggerEffect*)(*pIter)->GetMapTriggerEffect();
							if(LOCK_COL == col)
							{
								if(p->bLock)						
									m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);							
								else					
									m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Lock, 0, 0, 0, 0);
								p->bLock = !p->bLock;
							}					
							else if(FLAG_COL == col)
							{
								if(p->bFlag)						
									m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);					
								else						
									m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Flag, 0, 0, 0, 0);					
								p->bFlag = !p->bFlag;
							}
							break;
						}
					}
			}
			break;
		case GMAPOBJ_DynamicBlock:
			{
				for(; pIter != pList.end(); ++pIter)
					if((*pIter)->GetType() == MapObjEdit::DynamicBlock)
					{
						if((*pIter)->GetMapID() == nIndex)
						{
							memset(&g_DynamicBlock, 0, sizeof(g_DynamicBlock));
							(*pIter)->GetData(g_DynamicBlock);
							m_propList.DisplayDynamicBlock(&g_DynamicBlock);

							pos = g_DynamicBlock.vPos;

							//--判别文件类型
							CString szMdlName=CString(g_DynamicBlock.szModelPath);
							int nlength=szMdlName.GetLength();
							int nIndex=szMdlName.ReverseFind(_T('.'));
							CString szfiletype=szMdlName.Right(nlength-nIndex-1);

							if(szfiletype!=_T("sfx") && szfiletype!=_T("SFX"))
							{
								m_Cool3DView->LoadMdl(szMdlName.GetBuffer());
							}	
							g_mapEditor->GetMap()->SetSelectObj(*pIter);

							tagMapDynamicBlock* p = (tagMapDynamicBlock*)(*pIter)->GetDynamicBlock();
							if(LOCK_COL == col)
							{
								if(p->bLock)						
									m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);							
								else					
									m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Lock, 0, 0, 0, 0);
								p->bLock = !p->bLock;
							}					
							else if(FLAG_COL == col)
							{
								if(p->bFlag)						
									m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);					
								else						
									m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Flag, 0, 0, 0, 0);					
								p->bFlag = !p->bFlag;
							}
							break;
						}
					}
			}
			break;
		case GMAPOBJ_MapDoor:
			{
				for(; pIter != pList.end(); ++pIter)
					if((*pIter)->GetType() == MapObjEdit::MapDoor)
					{
						if((*pIter)->GetMapID() == nIndex)
						{
							memset(&g_MapDoor, 0, sizeof(g_MapDoor));
							(*pIter)->GetData(g_MapDoor);
							m_propList.DisplayMapDoor(&g_MapDoor);

							pos = g_MapDoor.vPos;

							//--判别文件类型
							CString szMdlName=CString(g_MapDoor.szModelPath);
							int nlength=szMdlName.GetLength();
							int nIndex=szMdlName.ReverseFind(_T('.'));
							CString szfiletype=szMdlName.Right(nlength-nIndex-1);

							if(szfiletype!=_T("sfx") && szfiletype!=_T("SFX"))
							{
								m_Cool3DView->LoadMdl(szMdlName.GetBuffer());
							}	
							g_mapEditor->GetMap()->SetSelectObj(*pIter);

							tagMapDoor* p = (tagMapDoor*)(*pIter)->GetMapDoor();
							if(LOCK_COL == col)
							{
								if(p->bLock)						
									m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);							
								else					
									m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Lock, 0, 0, 0, 0);
								p->bLock = !p->bLock;
							}					
							else if(FLAG_COL == col)
							{
								if(p->bFlag)						
									m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);					
								else						
									m_objList.SetItem(row, col, LVIF_IMAGE, NULL, EImg_Flag, 0, 0, 0, 0);					
								p->bFlag = !p->bFlag;
							}
							break;
						}
					}
			}
			break;
		case GMAPOBJ_MapCarrier:
			{
				for(; pIter != pList.end(); ++pIter)
					if((*pIter)->GetType() == MapObjEdit::MapCarrier)
					{
						if((*pIter)->GetMapID() == nIndex)
						{
							memset(&g_MapCarrier, 0, sizeof(g_MapCarrier));
							(*pIter)->GetData(g_MapCarrier.carrier);
							m_propList.DisplayMapCarrier(&(g_MapCarrier.carrier));

							pos = g_MapDoor.vPos;

							//--判别文件类型
							CString szMdlName=CString(g_MapCarrier.carrier.szModelPath);
							int nlength=szMdlName.GetLength();
							int nIndex=szMdlName.ReverseFind(_T('.'));
							CString szfiletype=szMdlName.Right(nlength-nIndex-1);

							if(szfiletype!=_T("sfx") && szfiletype!=_T("SFX"))
							{
								m_Cool3DView->LoadMdl(szMdlName.GetBuffer());
							}	
							g_mapEditor->GetMap()->SetSelectObj(*pIter);

							break;
						}
					}
			}
			break;
		default:
			return ;
		}
	}
	m_pParent->Invalidate();
}



void CMapObjDlg::OnBnClickedLoad()
{
	if(g_mapEditor->GetMap()->GetHeightMap()->GetHeightmap()==NULL) 
	{
		AfxMessageBox(_T("请先导入地形"));
		return;
	}
	if(IDYES == MessageBox(_T("确定不保存当前正在编辑的数据吗？") ,_T("提示"), MB_YESNO) )
	{
		ResetAllInfo();
		m_objTypeList.SetCurSel(CB_ERR);
		m_objList.DeleteAllItems();
		m_propList.DeleteAllItem();
		m_pPreCreated = NULL;
		g_mapEditor->SetLock(NULL);

		//读取地图对象文件
		if(IDYES == MessageBox(_T("需要载入地图对象文件吗？") ,_T("提示"), MB_YESNO))
		{
		   CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT| OFN_NOCHANGEDIR,
			               _T("mb文件|*.mb||"),this,0);
		   if(IDOK == dlg.DoModal())
		   {
			   GameMapEditor*  pMap= g_mapEditor->GetMap();			
			   bool bLoadSucc = pMap->LoadFromEditFile(dlg.GetPathName().GetBuffer());		//加载地图文件
			   m_bMapAttOk = TRUE;
			   m_fSunModulus = pMap->GetSunModulus();						//加载太阳光增强系数
			   m_fSkyYaw = pMap->GetSkyYaw();								//加载天空盒旋转角度

			   pMap->GetSunLight(m_SunLight);
			   SceneGraph*  pSG =	pMap->GetSceneGraph();
			   ASSERT(pSG != NULL);
				tagLight light = pSG->GetSunLight();
				light.ambient = m_SunLight.ambient;
				light.diffuse = m_SunLight.diffuse;
				light.diffuse.R*=m_fSunModulus;
				light.diffuse.G*=m_fSunModulus;
				light.diffuse.B*=m_fSunModulus;
				light.specular = m_SunLight.specular;
				light.pos = m_SunLight.pos;
				light.dir = -m_SunLight.pos;
				Vec3Normalize(light.dir);
			   pSG->SetSunLight(light);								//设置太阳光

			    pMap->GetFog(m_Fog);
				tagDistFog fog = pSG->GetSceneFog();
				fog.color = m_Fog.color;
				fog.density = m_Fog.density;
				fog.start = m_Fog.start;
				fog.end = m_Fog.end;
				FileAttribute fileAtt(this);
				m_dist=fileAtt.m_dist;
				fog.start*=m_dist*50.0f;
				fog.end*=m_dist*50.0f;
				pSG->SetSceneFog(fog);


			   //天空盒
				m_szSkyBoxTopTex = CString(pMap->GetSkyBoxTop());
				m_szSkyBoxFrontRightTex = CString(pMap->GetSkyBoxFrontRight());
				m_szSkyBoxBackLeftTex = CString(pMap->GetSkyBoxBackLeft());;
				pMap->GetSkyBoxSize(m_vecSkyBoxSize);
				pMap->GetSkyBoxOff(m_vecSkyBoxOff);

				pMap->SetSkyBox(m_szSkyBoxTopTex.GetBuffer(), m_szSkyBoxFrontRightTex.GetBuffer(), \
					m_szSkyBoxBackLeftTex.GetBuffer(), m_vecSkyBoxSize, m_vecSkyBoxOff);
				pMap->SetSkyYaw(m_fSkyYaw);
				pMap->GetSkyCol(m_SkyCol);

				m_bRenderSkyShade = pMap->IsRenderSkyShade();
				m_dwSkyShadeCol = pMap->GetSkyShadeCol();
				pMap->SetRenderSkyShade(m_bRenderSkyShade);
				pMap->SetSkyShadeCol(m_dwSkyShadeCol);
	
				m_dwDynamicDiffCol = pMap->GetDynamicDiffCol();
				m_dwDynamicAmbCol = pMap->GetDynamicAmbCol();
				m_dwDynamicSpecCol = pMap->GetDynamicSpecCol();

				 m_dwGlobalID = pMap->GetGlobalID();	
				/*m_dwGlobalID = 0;
				list<MapObjEdit*> pList = pMap->GetMapObjEditList();
				list<MapObjEdit*>::iterator iter = pList.begin();
				for(; iter!=pList.end(); iter++)
				{
					(*iter)->SetObjID(m_dwGlobalID++);
				}*/

				 if( !bLoadSucc )
				 {
					 AfxMessageBox(_T("部分NPC加载失败,请查看NPCxml表"));
				 }
		   }
		}
	}	
	m_pParent->Invalidate();
}

void CMapObjDlg::OnBnClickedCommit()
{
	POSITION pos = m_objList.GetFirstSelectedItemPosition();
	
	if (pos != NULL)
	{
		int nItem = m_objList.GetNextSelectedItem(pos);
		CString szBuffer = m_objList.GetItemText(nItem , 0);

		//处理导航点的修改
		if(m_objTypeList.GetCurSel()==GMAPOBJ_WAYPOINT)
		{
			ModifyWayPointProp(szBuffer);
			return;
		}
		//处理触发器的修改
		if(m_objTypeList.GetCurSel()==GMAPOBJ_Trigger)
		{
			ModifyTriggerProp(szBuffer);
			return;
		}
		//处理音效的修改
		if(m_objTypeList.GetCurSel()==GMAPOBJ_Sound)
		{
			ModifySoundProp(szBuffer);
			return;
		}
		//处理点光源的修改
		if(m_objTypeList.GetCurSel()==GMAPOBJ_PointLight)
		{
			ModifyPointLightProp(szBuffer);
			return;
		}
		//处理地图区域的修改
		if(m_objTypeList.GetCurSel()==GMAPOBJ_MapRect)
		{
			ModifyMapRectProp(szBuffer);
			return;
		}
		//处理寻路导航点的修改
		if(m_objTypeList.GetCurSel()==GMAPOBJ_PathPoint)
		{
			ModifyPathPointProp(szBuffer);
			return;
		}
		if(m_objTypeList.GetCurSel()==GMAPOBJ_SpawnPoint)
		{
			ModifySpawnPoint(szBuffer);
			return;
		}
		//处理事件触发器的修改
		if(m_objTypeList.GetCurSel()==GMAPOBJ_EventTrigger)
		{
			ModifyEventTrigger(szBuffer);
			return;
		}
		//
		if(m_objTypeList.GetCurSel()==GMAPOBJ_NPCNavStartPoint)
		{
			ModifyNPCNav(szBuffer);
			return;
		}
		//--
		if(m_objTypeList.GetCurSel()==GMAPOBJ_PlayerNavStartPoint)
		{
			ModifyPlayerNav(szBuffer);
			return;
		}

        //处理其他类型的修改
		int nIndex = _tcstoul(szBuffer, NULL, 16);
		MapObjEdit::EType type;

		//确定物体类型
		if(m_objTypeList.GetCurSel() == GMAPOBJ_Npc)
			type = MapObjEdit::NPC;
		if(m_objTypeList.GetCurSel() == GMAPOBJ_StaticObj)
			type = MapObjEdit::StaticObj;
		if(m_objTypeList.GetCurSel() == GMAPOBJ_DynamicBlock)
			type = MapObjEdit::DynamicBlock;
		if(m_objTypeList.GetCurSel() == GMAPOBJ_MapDoor)
			type = MapObjEdit::MapDoor;
		if(m_objTypeList.GetCurSel() == GMAPOBJ_MapCarrier)
			type = MapObjEdit::MapCarrier;
		if(m_objTypeList.GetCurSel() == GMAPOBJ_MapTriggerEffect)
			type = MapObjEdit::MapTrrigerEffect;
		
		//查找物体
		MapObjEdit* pEdit = g_mapEditor->GetMap()->FindMapObjEdit(type,nIndex);
		if(pEdit==NULL)  
			return;

		Vector3 pos;
	    float fYaw;
		bool bIsLoadMDL=false;
		switch(type)
		{
			case MapObjEdit::NPC:
				{
					tagMapNPC* p = (tagMapNPC*)pEdit->GetMapNPC();
					if(p->bLock!=true)
					{
						if(!m_propList.GetModifiedNPCObjProp(p,pos,fYaw,bIsLoadMDL))
							return;
					
						if(bIsLoadMDL)
						{
							const tagCreatureProto* pAtt =g_mapEditor->GetMap()->GetAttDataBase()->FindCreatureAtt(p->dwTypeID);
							if(pAtt!=NULL)
							{
								tstring szMdlPath = CreatureDataReader::GetCreatureMdlPath(pAtt);
								pEdit->LoadMdl(szMdlPath.c_str());
								m_Cool3DView->LoadMdl(szMdlPath.c_str());
								m_objList.DeleteAllItems();
								DisplayList(MapObjEdit::NPC);
							}
							else
								return;
						}  
						pEdit->SetWorldPos(pos);
						pEdit->SetYawForNPC(fYaw);
					}
					else
					{
						AfxMessageBox(_T("该地图对象已锁定,不能修改!"));
						return;
					}
				}
				break;
			case MapObjEdit::StaticObj:
				{
					tagStaticMapObj* p = (tagStaticMapObj*)pEdit->GetStaticMapObj();
					if(p->bLock!=true)
					{
						if(!m_propList.GetModifiedStaticObjProp(p,pos,bIsLoadMDL))
							return;
						if(bIsLoadMDL)	
						{
							pEdit->LoadMdl(p->szMdlPath);
							if(pEdit->GetSGEffectNode())
							{
								pEdit->GetSGEffectNode()->Play(); 
							}
							else
							{
								m_Cool3DView->LoadMdl(p->szMdlPath);
							}
						}
						pEdit->SetMdlScale(Vector3(p->fScale[0], p->fScale[1], p->fScale[2]));
						pEdit->SetWorldPos(pos);
						pEdit->SetMtlDiffuseFactor(p->fMtlDiffuseFactor);
						UpdateList(pEdit);
					}		
					else
					{
						AfxMessageBox(_T("该地图对象已锁定,不能修改!"));
						return;
					}
				}
				break;
			case MapObjEdit::MapTrrigerEffect:
				{
					tagMapTriggerEffect* p = (tagMapTriggerEffect*)pEdit->GetMapTriggerEffect();
					if(p->bLock!=true)
					{
						if(!m_propList.GetModifyedMapTriggerEffect(p,pos,bIsLoadMDL))
							return;
						if(bIsLoadMDL)	
						{
							pEdit->LoadMdl(p->szMdlPath);
							if(pEdit->GetSGEffectNode())
							{
								pEdit->GetSGEffectNode()->Play(); 
							}
							else
							{
								m_Cool3DView->LoadMdl(p->szMdlPath);
							}
						}
						pEdit->SetMdlScale(Vector3(p->fScale[0], p->fScale[1], p->fScale[2]));
						pEdit->SetWorldPos(pos);
						UpdateList(pEdit);
					}		
					else
					{
						AfxMessageBox(_T("该地图对象已锁定,不能修改!"));
						return;
					}
				}
				break;
			case MapObjEdit::DynamicBlock:
				{
					tagMapDynamicBlock* p = (tagMapDynamicBlock*)pEdit->GetDynamicBlock();
					if(p->bLock!=true)
					{
						if(!m_propList.GetModifyedDynamicBlockProp(p,pos,bIsLoadMDL))
							return;
						if(bIsLoadMDL)	
						{
							pEdit->LoadMdl(p->szModelPath);
							if(pEdit->GetSGEffectNode())
							{
								pEdit->GetSGEffectNode()->Play(); 
							}
							else
							{
								m_Cool3DView->LoadMdl(p->szModelPath);
							}
						}
						pEdit->SetMdlScale(Vector3(p->fScale[0], p->fScale[1], p->fScale[2]));
						pEdit->SetWorldPos(pos);
						pEdit->SetYaw(p->fYaw);
					}		
					else
					{
						AfxMessageBox(_T("该地图对象已锁定,不能修改!"));
						return;
					}
				}
				break;
			case MapObjEdit::MapDoor:
				{
					tagMapDoor* p = (tagMapDoor*)pEdit->GetMapDoor();
					if(p->bLock!=true)
					{
						if(!m_propList.GetModifiedMapDoor(p,pos,bIsLoadMDL))
							return;
						if(bIsLoadMDL)	
						{
							pEdit->LoadMdl(p->szModelPath);
							if(pEdit->GetSGEffectNode())
							{
								pEdit->GetSGEffectNode()->Play(); 
							}
							else
							{
								m_Cool3DView->LoadMdl(p->szModelPath);
							}
						}
						pEdit->SetMdlScale(Vector3(p->fScale[0], p->fScale[1], p->fScale[2]));
						pEdit->SetWorldPos(pos);
						pEdit->SetYaw(p->fYaw);
						pEdit->PlayAnimationTrack();
					}		
					else
					{
						AfxMessageBox(_T("该门已锁定,不能修改!"));
						return;
					}
				}
				break;
			case MapObjEdit::MapCarrier:
				{
					tagMapCarrier* p = (tagMapCarrier*)pEdit->GetMapCarrier();
					if(p->bLock!=true)
					{
						if(!m_propList.GetModifiedMapCarrier(p,bIsLoadMDL))
							return;
						if(bIsLoadMDL)	
						{
							pEdit->LoadMdl(p->szModelPath);
							if(pEdit->GetSGEffectNode())
							{
								pEdit->GetSGEffectNode()->Play(); 
							}
							else
							{
								m_Cool3DView->LoadMdl(p->szModelPath);
							}
						}
						if( _tcslen(p->szWayPoint)!=0 )
						{
							list<tagMapWayPoint*> WayPointList;
							g_mapEditor->GetMap()->GetWayPointList(WayPointList);
							int nItem=0;
							for(list<tagMapWayPoint*>::iterator iter=WayPointList.begin();iter!=WayPointList.end();++iter)
							{									
								if( _tcscmp((*iter)->szName, p->szWayPoint)==0 && (*iter)->byIndex==0 )
								{
									pEdit->SetWorldPos((*iter)->vPos);
									pEdit->SetYaw(p->fYaw);
								}

							}							
						}
					}		
					else
					{
						AfxMessageBox(_T("该载具已锁定,不能修改!"));
						return;
					}
					break;
				}
		}
	}
}

void CMapObjDlg::OnBnClickedSave()
{				

		if(IDYES == MessageBox(_T("需要保存地图对象文件吗？") ,_T("提示"), MB_YESNO))
		{
			CFileDialog dlg(TRUE, NULL, NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR, 
							_T("*.mb|*.mb||"),this, 0); 
			if(IDOK == dlg.DoModal())
			{
				CString szName = dlg.GetPathName();
				//set map's id
				GameMapEditor* pMap = g_mapEditor->GetMap();			
				pMap->SetSunLight(m_SunLight);
				pMap->SetFog(m_Fog);
				pMap->SetSkyCol(m_SkyCol);
				pMap->SetSkyBox(m_szSkyBoxTopTex.GetBuffer(), m_szSkyBoxFrontRightTex.GetBuffer(), \
					m_szSkyBoxBackLeftTex.GetBuffer(), m_vecSkyBoxSize, m_vecSkyBoxOff);
				pMap->SetSunModulus(m_fSunModulus);
				pMap->SetSkyYaw(m_fSkyYaw);
				pMap->SetSkyShadeCol(m_dwSkyShadeCol);
				pMap->SetRenderSkyShade(m_bRenderSkyShade);
				pMap->SetDynamicDiffCol(m_dwDynamicDiffCol);
				pMap->SetDynamicAmbCol(m_dwDynamicAmbCol);
				pMap->SetDynamicSpecCol(m_dwDynamicSpecCol);
				pMap->SetGlobalID(m_dwGlobalID);
				pMap->WriteEditFile(szName.GetBuffer());	
		
			}
		}	
}

void CMapObjDlg::OnNMDblclkobjlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	POSITION pos=m_objList.GetFirstSelectedItemPosition();
	Vector3 Vecpos(0,0,0);
	if(pos!=NULL)
	{
		int item=m_objList.GetNextSelectedItem(pos);
		CString szID=m_objList.GetItemText(item,0);

		//NPC、静态、动态障碍物、怪物处理
		if(m_objTypeList.GetCurSel()==GMAPOBJ_Npc 
			|| m_objTypeList.GetCurSel()==GMAPOBJ_StaticObj
			|| m_objTypeList.GetCurSel()==GMAPOBJ_DynamicBlock
			|| m_objTypeList.GetCurSel()==GMAPOBJ_MapDoor
			|| m_objTypeList.GetCurSel()==GMAPOBJ_MapCarrier
			|| m_objTypeList.GetCurSel()==GMAPOBJ_MapTriggerEffect)
		{
	  	   WORD id=_tstoi(szID);
		   g_mapEditor->GetMap()->SelectObjByID(id);
		   MapObjEdit *pSelObj=g_mapEditor->GetMap()->GetSelectedObj();
		   if(pSelObj)
		   {
			   Vecpos=pSelObj->GetWorldPos();
		   }
		   if(pSelObj->GetSGEffectNode())
		   {
			   pSelObj->GetSGEffectNode()->Play(); 
		   }
		}
		//导航点处理
		if(m_objTypeList.GetCurSel()==GMAPOBJ_WAYPOINT)
		{
			DWORD nIndex = _tcstoul(szID, NULL, 16);
			tagMapWayPoint *pWayPoint=g_mapEditor->GetMap()->GetWayPointData(nIndex);
			if(pWayPoint)
			{
				g_mapEditor->GetMap()->SetCurPickedWayPoint(pWayPoint);
				Vecpos = pWayPoint->vPos;
			}
		}
		//触发器处理
		if(m_objTypeList.GetCurSel()==GMAPOBJ_Trigger)
		{
			DWORD nIndex = _tcstoul(szID, NULL, 16);
			tagMapTrigger *pTrigger=g_mapEditor->GetMap()->GetTriggerData(nIndex);
			if(pTrigger)
			{
			    g_mapEditor->GetMap()->SetCurPickedTrigger(pTrigger);
				Vecpos = Vector3(pTrigger->region[0].x, pTrigger->region[0].y, pTrigger->region[0].z);
			}
		}
		//地图区域处理
		if(m_objTypeList.GetCurSel()==GMAPOBJ_MapRect)
		{
			DWORD nIndex = _tcstoul(szID, NULL, 16);
			MapObjEdit* pEdit = g_mapEditor->GetMap()->FindMapRect(nIndex);
			if(pEdit==NULL)
				return;
			const tagMapArea* pMapRect = pEdit->GetMapRect();
			if(pMapRect)
			{
				g_mapEditor->GetMap()->SetSelectedMapRect(pEdit);
				Vecpos = Vector3(pMapRect->region[0].x, pMapRect->region[0].y, pMapRect->region[0].z);
			}

		}
		//音效处理
		if(m_objTypeList.GetCurSel()==GMAPOBJ_Sound)
		{
			DWORD nIndex = _tcstoul(szID, NULL, 16);
			tagMapSound *pSound=g_mapEditor->GetMap()->FindSound(nIndex);
			if(pSound)
			{
				g_mapEditor->GetMap()->SetSelectedSound(pSound);
				Vecpos=Vector3(pSound->fPos[0],pSound->fPos[1],pSound->fPos[2]);
			}
		}
		//寻路导航点
		if(m_objTypeList.GetCurSel()==GMAPOBJ_PathPoint)
		{	
			DWORD nIndex = _tcstoul(szID, NULL, 16);
			MapObjEdit* pEdit = g_mapEditor->GetMap()->FindPathPoint(nIndex);
			if(pEdit==NULL)
				return;
			tagMapPathPoint* pPathPoint = (tagMapPathPoint*)pEdit->GetPathPoint();
			if(pPathPoint)
			{
				g_mapEditor->GetMap()->SetSelectedPathPoint(pEdit);
				Vecpos=Vector3(pPathPoint->vPos.x, pPathPoint->vPos.y, pPathPoint->vPos.z);
			}
		}
		//刷怪点
		if(m_objTypeList.GetCurSel()==GMAPOBJ_SpawnPoint)
		{	
			DWORD nIndex = _tcstoul(szID, NULL, 16);
 			MapObjEdit* pEdit = g_mapEditor->GetMap()->FindSpawnPoint(nIndex);
			if(pEdit==NULL)
				return;
			tagMapSpawnPoint* pSpawnPoint = (tagMapSpawnPoint*)pEdit->GetSpawnPoint();
			if(pSpawnPoint)
			{
				g_mapEditor->GetMap()->SetSelectedSpawnPoint(pEdit);
				Vecpos=Vector3(pSpawnPoint->vPos.x, pSpawnPoint->vPos.y, pSpawnPoint->vPos.z);
			}
		}
		//点光源处理
		if(m_objTypeList.GetCurSel()==GMAPOBJ_PointLight)
		{
			DWORD nIndex = _tcstoul(szID, NULL, 16);
			MapObjEdit* pEdit = g_mapEditor->GetMap()->FindPointLight(nIndex);
			if(pEdit==NULL)
				return;
			tagMapPointLight *pPointLight= (tagMapPointLight*)pEdit->GetPointLgtObj();
			if(pPointLight)
			{
				g_mapEditor->GetMap()->SetSelectedPointLight(pEdit);
				Vecpos=Vector3(pPointLight->fPos[0], pPointLight->fPos[1], pPointLight->fPos[2]);
			}
		}
		//
		if(m_objTypeList.GetCurSel()==GMAPOBJ_NPCNavStartPoint)
		{
			DWORD nIndex = _tcstoul(szID, NULL, 16);
			tagNPCNavMapBuildStartPoint* pEdit = g_mapEditor->GetMap()->FindNPCNavStartPoint(nIndex);
			if(pEdit==NULL)
				return;
			if(pEdit)
			{
				g_mapEditor->GetMap()->SetSelectedNPCNavStartPoint(pEdit);
				Vecpos = pEdit->vPos;
			}
		}
		//
		if(m_objTypeList.GetCurSel()==GMAPOBJ_PlayerNavStartPoint)
		{
			DWORD nIndex = _tcstoul(szID, NULL, 16);
			tagPlayerNavMapBuildStartPoint* pEdit = g_mapEditor->GetMap()->FindPlayerNavStartPoint(nIndex);
			if(pEdit==NULL)
				return;
			if(pEdit)
			{
				g_mapEditor->GetMap()->SetSelectedPlayerNavStartPoint(pEdit);
				Vecpos = pEdit->vPos;
			}
		}
		//设置照相机位置
		CameraBase *pCamera=g_mapEditor->GetCamera();
		CameraEuler *pCE=(CameraEuler *)pCamera;//todo:确认这个转换是安全的
		pCE->SetLookAtPos(Vecpos);
	}
	*pResult = 0;
}

void CMapObjDlg::ResetAllInfo()
{
	m_szMapID = _T("");			
	m_dwMapIndex = 0;
	m_fSunModulus = 1.0f;
	m_SunLight.pos = Vector3(-1000, 1000, -1000);
	m_SunLight.diffuse = Color4f(1.0f, 1.0f, 1.0f, 1.0f);
	m_SunLight.ambient = Color4f(1.0f, 0.4f, 0.4f, 0.4f);
	m_SunLight.specular = Color4f(1.0f, 1.0f, 1.0f, 1.0f);
	m_SkyCol = Color4f(1.0f, 1.0f, 1.0f, 1.0f);
	m_dwSkyShadeCol = 0xffffffff;
	m_bRenderSkyShade = false;
	m_dwDynamicDiffCol = 0xffffffff;
	m_dwDynamicAmbCol = 0xff808080;
	m_dwDynamicSpecCol = 0xffffffff;
	m_SunLight.dir = -m_SunLight.pos;
	m_fSkyYaw = 0;
	m_bMapAttOk = FALSE;
	m_Fog.color = Color4f(1.0f, 1.0f, 1.0f, 1.0f);
	m_Fog.start = 0.1f;
	m_Fog.end = 0.75f;
	m_Fog.density  = 1.0f;
	
	m_dwGlobalID = 0;

	m_szSkyBoxTopTex = _T("");
	m_szSkyBoxFrontRightTex = _T("");
	m_szSkyBoxBackLeftTex = _T("");
	m_vecSkyBoxSize = Vector3(1000,1000,1000);
	m_vecSkyBoxOff = Vector3(0,0,0);
	m_objTypeList.SetCurSel(CB_ERR);
	m_objList.DeleteAllItems();
	m_propList.DeleteAllItem();
	m_pPreCreated = NULL;
	g_mapEditor->SetLock(NULL);
	g_mapEditor->GetMap()->SetSelectObj(NULL);
	g_mapEditor->GetMap()->ClearEditList(true);
	g_mapEditor->GetMap()->ClearWayPointList();
	g_mapEditor->GetMap()->ClearTriggerList();
	g_mapEditor->GetMap()->ClearSoundList();
	g_mapEditor->GetMap()->ClearPointLightList();
	g_mapEditor->GetMap()->ClearMapRectList();
	g_mapEditor->GetMap()->ClearPathPointList();
	g_mapEditor->GetMap()->ClearSpawnPointList();
	g_mapEditor->GetMap()->ClearEventTrigger();
	g_mapEditor->GetMap()->ClearNPCNavStartPoint();
	g_mapEditor->GetMap()->ClearPlayerNavStartPoint();

	g_mapEditor->SetPickFlag();
	m_vecRes.clear();
	g_mapEditor->GetMap()->GetSceneGraph()->SetSkyNode(NULL);
}

void CMapObjDlg::OnBnClickedButtonNewmap()
{
	// TODO: 在此添加控件通知处理程序代码
	if(MessageBox(_T("确定建立新地图吗？ 当前如果有工作，则将会被删除！"),_T("警告"),MB_YESNO|MB_ICONWARNING)==IDYES)
	{
		ResetAllInfo();
	}
}
	
//modify prop list for way point
void CMapObjDlg::ModifyWayPointProp(CString szID)
{
	tagMapWayPoint* pWayPoint; 
	pWayPoint=g_mapEditor->GetMap()->GetCurPickedWayPoint();
	if(pWayPoint)
	{
		if(pWayPoint->bLock==true)
		{
			AfxMessageBox(_T("该导航点已锁定,不能修改!"));
			return;
		}
	}
	tagMapWayPoint WayPoint;
	if(!m_propList.GetModifiedWayPointProp(WayPoint))
		return;

	if(_tcscmp(pWayPoint->szName,WayPoint.szName)==0 && pWayPoint->byIndex==WayPoint.byIndex)
	{
		g_mapEditor->GetMap()->ModifyWayPointProp(WayPoint);
	}
	else
	{
		if(g_mapEditor->GetMap()->GetWayPointData(WayPoint.szName,WayPoint.byIndex)!=NULL)
		{
			AfxMessageBox(_T("修改后的导航点的名字和索引已经存在"));
		}
		else
		{
			g_mapEditor->GetMap()->ModifyWayPointProp(pWayPoint->dwObjID, pWayPoint->szName, pWayPoint->byIndex,WayPoint);	
		}
	}
	m_objList.DeleteAllItems();
	DisplayWayPointList();
}


int CMapObjDlg::AddObjIDToObjList(CString szBuffer)
{
	int nItem=m_objList.GetItemCount();
	m_objList.InsertItem(nItem, szBuffer, 0);
	if(m_objTypeList.GetCurSel()==GMAPOBJ_Npc )
	{
		int nIndex = _tcstoul(szBuffer, NULL, 16);
		MapObjEdit* pEdit= g_mapEditor->GetMap()->FindMapObjEdit(MapObjEdit::NPC,nIndex);
        ASSERT(pEdit!=NULL);

		CreatureDataReader *pDB = g_mapEditor->GetMap()->GetAttDataBase();
		ASSERT(pDB!=NULL);

		const map<DWORD, tagCreatureProto>* npcMap;
		npcMap = pDB->GetCreatureMap();
		map<DWORD, tagCreatureProto>::const_iterator it = npcMap->find(pEdit->GetMapNPC()->dwTypeID);
		if(it != npcMap->end())
		{
			m_objList.SetItemText(nItem,1,it->second.szName);
		}
		else
		{
			m_objList.SetItemText(nItem,1,_T("未知名"));
		}
	}
	else if(m_objTypeList.GetCurSel()==GMAPOBJ_StaticObj)
	{
		int nIndex = _tcstoul(szBuffer, NULL, 16);
		MapObjEdit* pEdit= g_mapEditor->GetMap()->FindMapObjEdit(MapObjEdit::StaticObj,nIndex);
		ASSERT(pEdit!=NULL);
		tagStaticMapObj* pStaticObj = (tagStaticMapObj*)pEdit->GetStaticMapObj();
		CString szPath = pStaticObj->szMdlPath;

		int nlength=szPath.GetLength();
		int nNum=szPath.ReverseFind(_T('\\'));
		CString szName=szPath.Right(nlength-nNum-1);

		m_objList.SetItemText(nItem,1,szName.GetBuffer());
	}
	else if(m_objTypeList.GetCurSel()==GMAPOBJ_MapTriggerEffect)
	{
		int nIndex = _tcstoul(szBuffer, NULL, 16);
		MapObjEdit* pEdit= g_mapEditor->GetMap()->FindMapObjEdit(MapObjEdit::MapTrrigerEffect,nIndex);
		ASSERT(pEdit!=NULL);
		tagMapTriggerEffect* pStaticObj = (tagMapTriggerEffect*)pEdit->GetMapTriggerEffect();
		CString szPath = pStaticObj->szMdlPath;

		int nlength=szPath.GetLength();
		int nNum=szPath.ReverseFind(_T('\\'));
		CString szName=szPath.Right(nlength-nNum-1);

		m_objList.SetItemText(nItem,1,szName.GetBuffer());
	}
	else if(m_objTypeList.GetCurSel()==GMAPOBJ_DynamicBlock)
	{
		int nIndex = _tcstoul(szBuffer, NULL, 16);
		MapObjEdit* pEdit= g_mapEditor->GetMap()->FindMapObjEdit(MapObjEdit::DynamicBlock,nIndex);
		ASSERT(pEdit!=NULL);
		tagMapDynamicBlock* pDynaBlock = (tagMapDynamicBlock*)pEdit->GetDynamicBlock();
		CString szPath = pDynaBlock->szModelPath;

		int nlength=szPath.GetLength();
		int nNum=szPath.ReverseFind(_T('\\'));
		CString szName=szPath.Right(nlength-nNum-1);

		m_objList.SetItemText(nItem,1,szName.GetBuffer());
	}
	else if(m_objTypeList.GetCurSel()==GMAPOBJ_MapDoor)
	{
		int nIndex = _tcstoul(szBuffer, NULL, 16);
		MapObjEdit* pEdit= g_mapEditor->GetMap()->FindMapObjEdit(MapObjEdit::MapDoor,nIndex);
		ASSERT(pEdit!=NULL);
		tagMapDoor* pDoor = (tagMapDoor*)pEdit->GetMapDoor();
		CString szPath = pDoor->szModelPath;

		int nlength=szPath.GetLength();
		int nNum=szPath.ReverseFind(_T('\\'));
		CString szName=szPath.Right(nlength-nNum-1);

		m_objList.SetItemText(nItem,1,szName.GetBuffer());
	}
	else if( m_objTypeList.GetCurSel()==GMAPOBJ_MapCarrier )
	{
		int nIndex = _tcstoul(szBuffer, NULL, 16);
		GameMapEditor* pMap = g_mapEditor->GetMap();
		MapObjEdit* pEdit= pMap->FindMapObjEdit(MapObjEdit::MapCarrier,nIndex);
		ASSERT(pEdit!=NULL);
		tagMapCarrier* pCar= (tagMapCarrier*)pEdit->GetMapCarrier();
		CString szPath = pCar->szModelPath;

		int nlength=szPath.GetLength();
		int nNum=szPath.ReverseFind(_T('\\'));
		CString szName=szPath.Right(nlength-nNum-1);

		m_objList.SetItemText(nItem,1,szName.GetBuffer());
	}
	else if(m_objTypeList.GetCurSel()==GMAPOBJ_WAYPOINT)
	{
		int nIndex = _tcstoul(szBuffer, NULL, 16);
		tagMapWayPoint* pWayPoint = g_mapEditor->GetMap()->FindWayPoint(nIndex);
		ASSERT(pWayPoint);
		CString szName;
		szName.Format(_T("%s %d"),pWayPoint->szName,pWayPoint->byIndex);
		m_objList.SetItemText(nItem,1,szName.GetBuffer());
	}
	return nItem;
}

void CMapObjDlg::ModifyTriggerProp(CString szID)
{	
	tagMapTrigger* pTrigger = g_mapEditor->GetMap()->GetCurPickedTrigger();
	if(pTrigger)
	{
		if(pTrigger->bLock)
		{
			AfxMessageBox(_T("该触发器已锁定,不能修改!"));
			return;
		}
	}
	tagMapTrigger Trigger;
	DWORD nIndex = _tcstoul(szID, NULL, 16);
	if(!m_propList.GetModifiedTriggerProp(Trigger))
		return;
    Trigger.dwObjID=nIndex;
	g_mapEditor->GetMap()->ModifyTriggerProp(Trigger);
}

//--修改地图区域的属性
void CMapObjDlg::ModifyMapRectProp(CString szID)
{
	MapObjEdit* pEdit = g_mapEditor->GetMap()->GetSelectedMapRect();
	if(pEdit==NULL)
		return;
	DWORD nIndex = _tcstoul(szID, NULL, 16);
	tagMapArea* pMapRect = (tagMapArea*)pEdit->GetMapRect();
	if(pMapRect && pMapRect->bLock)
	{
		AfxMessageBox(_T("该地图区域已锁定,不能修改!"));
		return;
	}
	m_propList.GetModifyedMapRectProp(pMapRect);
}

//--修改音效的属性
void CMapObjDlg::ModifySoundProp(CString szID)
{
	DWORD nIndex = _tcstoul(szID, NULL, 16);
	tagMapSound * pSound=g_mapEditor->GetMap()->FindSound(nIndex);
	if(pSound)
	{
		if(pSound->bLock==true)
		{
			AfxMessageBox(_T("该音效已锁定,不能修改!"));
			return;
		}
	}
	m_propList.GetModifiedSoundProp(pSound);
}

//--修改点光源的属性
void CMapObjDlg::ModifyPointLightProp(CString szID)
{
	DWORD nIndex = _tcstoul(szID, NULL, 16);
	MapObjEdit* pEdit = g_mapEditor->GetMap()->FindPointLight(nIndex);
	tagMapPointLight * pPointLight= (tagMapPointLight*)pEdit->GetPointLgtObj();
	if(pPointLight)
	{
		if(pPointLight->bLock)
		{
			AfxMessageBox(_T("该点光源已锁定,不能修改!"));
			return;
		}
	}
	m_propList.GetModifiedPointLightProp(pPointLight);
	pEdit->LoadLightMdl();
}

//--修改寻路导航点
void CMapObjDlg::ModifyPathPointProp(CString szID)
{
	DWORD nIndex = _tcstoul(szID, NULL, 16);
	MapObjEdit* pEdit = g_mapEditor->GetMap()->FindPathPoint(nIndex);
	tagMapPathPoint* pPathPoint = (tagMapPathPoint*)pEdit->GetPathPoint();
	if(pPathPoint)
	{
		m_propList.GetModifyedPathPointProp(pPathPoint);
	}
}

//--修改刷怪点
void CMapObjDlg::ModifySpawnPoint(CString szID)
{
	DWORD nIndex = _tcstoul(szID, NULL, 16);
	MapObjEdit* pEdit = g_mapEditor->GetMap()->FindSpawnPoint(nIndex);
	tagMapSpawnPoint* pSpawnPoint= (tagMapSpawnPoint*)pEdit->GetSpawnPoint();
	if(pSpawnPoint)
	{
		m_propList.GetModifyedSpawnPointProp(pSpawnPoint);
	}
}

void CMapObjDlg::ModifyNPCNav(CString szID)
{
	DWORD nIndex = _tcstoul(szID, NULL, 16);
	tagNPCNavMapBuildStartPoint* pEdit = g_mapEditor->GetMap()->FindNPCNavStartPoint(nIndex);
	if(pEdit)
	{
		m_propList.GetModifyedNPCNav(pEdit);
	}
}

void CMapObjDlg::ModifyPlayerNav(CString szID)
{
	DWORD nIndex = _tcstoul(szID, NULL, 16);
	tagPlayerNavMapBuildStartPoint* pEdit = g_mapEditor->GetMap()->FindPlayerNavStartPoint(nIndex);
	if(pEdit)
	{
		m_propList.GetModifyedPlayerNav(pEdit);
	}
}

//--修改事件触发器
void CMapObjDlg::ModifyEventTrigger(CString szID)
{
	DWORD nIndex = _tcstoul(szID, NULL, 16);
	MapObjEdit* pEdit = g_mapEditor->GetMap()->FindEventTrigger(nIndex);
	tagMapEventTrigger* pEventTrigger = (tagMapEventTrigger*)pEdit->GetEventTrigger();
	if(pEventTrigger)
	{
		m_propList.GetModifyedEventTrigger(pEventTrigger);
	}
}

//--得到当前是否锁定对象
bool CMapObjDlg::GetIsLockOnObject()
{
	if(((CButton*)(GetDlgItem(IDC_CHECK_LockSelected)))->GetCheck())
		return true;
	return false;
}

//--是否显示/隐藏NPC
bool CMapObjDlg::IsShowNPC()
{
	if(m_IsNpc)
		return true;
	return false;
}
//--是否显示//隐藏怪
bool CMapObjDlg::IsShowDynamicBlock()
{
	if(m_IsDynaBlock)
		return true;
	return false;
}
//--是否显示/隐藏静态物体
bool CMapObjDlg::IsShowStaticObj()
{
	if(m_IsStaticObj)
		return true;
	return false;
}
bool CMapObjDlg::IsShowMapDoor()
{
	if(m_IsMapDoor)
		return true;
	return false;
}
bool CMapObjDlg::IsShowMapTriggerEffect()
{
	if(m_IsMapTriggerEffect)
		return true;
	return false;
}
//--是否显示/隐藏地物方块
bool CMapObjDlg::IsShowQuad()
{
	if(m_IsQuad)
		return true;
	return false;
}
//--是否显示/隐藏迷宫区域
bool CMapObjDlg::IsShowMazeZone()
{
	if(m_IsMazeZone)
		return true; 
	return false;
}
//--是否显示/隐藏已锁定地图对象
bool CMapObjDlg::IsShowLockObj()
{
	if(m_IsLock)
		return true;
	return false;
}
//--是否显示/隐藏已标记地图对象
bool CMapObjDlg::IsShowFlagObj()
{
	if(m_IsFlag)
		return true;
	return false;
}
//--是否显示/隐藏地图区域
bool CMapObjDlg::IsShowMapRect()
{
	if(m_IsMapRect)
		return true;
	return false;
}


GMAPOBJ_TYPE CMapObjDlg::GetCurGMAPOBJTYPE()
{
	return (GMAPOBJ_TYPE)m_objTypeList.GetCurSel();
}


void CMapObjDlg::OnBnClickedButtonFilter()
{
	BeginMFCRes();
	FilterSelect filter(this);
	if(IDOK==filter.DoModal())
	{
		m_IsNpc = filter.GetIsShowNpc();
		m_IsDynaBlock = filter.GetIsShowDynamicBlock();
		m_IsStaticObj = filter.GetIsShowStaticObj();
		m_IsMapDoor = filter.GetIsShowMapDoor();
		m_IsQuad = filter.GetIsShowQuad();
		m_IsLock = filter.GetIsShowLock();
		m_IsFlag = filter.GetIsShowFlag();
		m_IsMapRect = filter.GetIsShowMapRect();
		m_IsTrigger = filter.GetIsShowTrigger();
		m_IsWayPoint = filter.GetIsShowWayPoint();
		m_IsMapTriggerEffect = filter.GetIsShowMapTriggerEffect();
		g_mapEditor->GetMap()->SetIsDrawMapRect(m_IsMapRect);
		g_mapEditor->GetMap()->SetIsDrawTrigger(m_IsTrigger);
		g_mapEditor->GetMap()->SetIsDrawWayPoint(m_IsWayPoint);

		//处理显示/隐藏NPC
		if(m_IsNpc)
			ShowNPC(true);
		else
			ShowNPC(false);

		//处理显示/隐藏怪
		if(m_IsDynaBlock)
			ShowDynamicBlock(true);
		else
			ShowDynamicBlock(false);

		if(m_IsMapDoor)
			ShowMapDoor(true);
		else 
			ShowMapDoor(false);

		//处理显示/隐藏静态物体
		if(m_IsStaticObj)
			ShowStaticObj(true);
		else
			ShowStaticObj(false);

		//处理显示/隐藏已锁定地图对象
		if(m_IsLock)
			ShowLockOjb(true);
		else
			ShowLockOjb(false);	

		//处理显示/隐藏已标记地图对象
		if(m_IsFlag)
			ShowFlagObj(true);
		else
			ShowFlagObj(false);

		if(m_IsMapTriggerEffect)
			ShowMapTriggerEffect(true);
		else
			ShowMapTriggerEffect(false);
	}
	EndMFCRes();
}

// 地图属性按钮
void CMapObjDlg::OnBnClickedMapatt()
{
	FileAttribute fileAtt(this);
	if(m_bMapAttOk)
	{
		fileAtt.SetOperationType(true);
		fileAtt.SetSunPos(m_SunLight.pos);
		Color4f diffuse=m_SunLight.diffuse;
		fileAtt.SetSunDiffuse(diffuse);
		Color4f ambient=m_SunLight.ambient;
		fileAtt.SetAmbient(ambient);
		fileAtt.SetSunSpecular(m_SunLight.specular);
		fileAtt.SetSkyBox(m_szSkyBoxTopTex, m_szSkyBoxFrontRightTex, m_szSkyBoxBackLeftTex, m_vecSkyBoxSize, m_vecSkyBoxOff);
		fileAtt.SetSunModulus(m_fSunModulus);
		fileAtt.SetSkyYaw(m_fSkyYaw);
		fileAtt.SetFogAttribute(m_Fog.start, m_Fog.end, m_Fog.density);
		fileAtt.SetFogColor(m_Fog.color);
		fileAtt.m_dist=m_dist;
		fileAtt.SetSkyCol(m_SkyCol);
		fileAtt.SetRenderSkyShade(m_bRenderSkyShade);
		fileAtt.SetSkyShadeCol(m_dwSkyShadeCol);
		fileAtt.SetDynamicDiffCol(m_dwDynamicDiffCol);
		fileAtt.SetDynamicAmbCol(m_dwDynamicAmbCol);
		fileAtt.SetDynamicSpecCol(m_dwDynamicSpecCol);
	}
	else
	{
		fileAtt.SetOperationType(false);
		m_szMapID.Format(_T("%d"), m_dwMapIndex);
		fileAtt.SetSunPos(m_SunLight.pos);
		Color4f diffuse=m_SunLight.diffuse;
		fileAtt.SetSunDiffuse(diffuse);
		Color4f ambient=m_SunLight.ambient;
		fileAtt.SetAmbient(ambient);
		fileAtt.SetSunSpecular(m_SunLight.specular);
		fileAtt.SetSkyBox(m_szSkyBoxTopTex, m_szSkyBoxFrontRightTex, m_szSkyBoxBackLeftTex, m_vecSkyBoxSize, m_vecSkyBoxOff);
		fileAtt.SetSunModulus(m_fSunModulus);
		fileAtt.SetSkyYaw(m_fSkyYaw);
		fileAtt.SetFogAttribute(m_Fog.start, m_Fog.end, m_Fog.density);
		fileAtt.SetFogColor(m_Fog.color);
		fileAtt.m_dist=m_dist;
		fileAtt.SetSkyCol(m_SkyCol);
		fileAtt.SetRenderSkyShade(m_bRenderSkyShade);
		fileAtt.SetSkyShadeCol(m_dwSkyShadeCol);
		fileAtt.SetDynamicDiffCol(m_dwDynamicDiffCol);
		fileAtt.SetDynamicAmbCol(m_dwDynamicAmbCol);
		fileAtt.SetDynamicSpecCol(m_dwDynamicSpecCol);
	}
	BeginMFCRes();
	if(IDOK==fileAtt.DoModal())
	{
		m_bMapAttOk = TRUE;
		m_fSunModulus = fileAtt.GetSunModulus();
		m_fSkyYaw = fileAtt.GetSkyYaw();
		GameMapEditor*  pMap= g_mapEditor->GetMap();
		SceneGraph*  pSG =	pMap->GetSceneGraph();

		m_SunLight = pSG->GetSunLight();
		m_SunLight.pos = fileAtt.GetSunPos();
		m_SunLight.diffuse = fileAtt.GetSunDiffuse();
		m_SunLight.ambient = fileAtt.GetAmbient();;
		m_SunLight.specular = fileAtt.GetSunSpecular();
		m_SunLight.dir = -m_SunLight.pos;
		Vec3Normalize(m_SunLight.dir);
		fileAtt.GetSkyBox(m_szSkyBoxTopTex, m_szSkyBoxFrontRightTex, m_szSkyBoxBackLeftTex, m_vecSkyBoxSize, m_vecSkyBoxOff);
		
		tagLight sunLight=m_SunLight;
		sunLight.diffuse.R*=m_fSunModulus;
		sunLight.diffuse.G*=m_fSunModulus;
		sunLight.diffuse.B*=m_fSunModulus;
		pSG->SetSunLight(sunLight);

		pMap->SetSkyBox(m_szSkyBoxTopTex.GetBuffer(), m_szSkyBoxFrontRightTex.GetBuffer(), 
			m_szSkyBoxBackLeftTex.GetBuffer(), m_vecSkyBoxSize, m_vecSkyBoxOff);
		pMap->SetSkyYaw(m_fSkyYaw);

		m_Fog = pSG->GetSceneFog();
		m_Fog.color = fileAtt.GetFogColor();
		fileAtt.GetFogAttribute(m_Fog.start, m_Fog.end, m_Fog.density);

		m_dist=fileAtt.m_dist;
		tagDistFog fog=m_Fog;
		fog.start*=m_dist*50.0f;
		fog.end*=m_dist*50.0f;
		pSG->SetSceneFog(fog);
		m_SkyCol = fileAtt.GetSkyCol();

		m_bRenderSkyShade = fileAtt.IsRenderSkyShade();
		m_dwSkyShadeCol = fileAtt.GetSkyShadeCol();
		pMap->SetRenderSkyShade(m_bRenderSkyShade);
		pMap->SetSkyShadeCol(m_dwSkyShadeCol);
		m_dwDynamicDiffCol = fileAtt.GetDynamicDiffCol();
		pMap->SetDynamicDiffCol(m_dwDynamicDiffCol);
		m_dwDynamicAmbCol = fileAtt.GetDynamicAmbCol();
		pMap->SetDynamicAmbCol(m_dwDynamicAmbCol);
		m_dwDynamicSpecCol = fileAtt.GetDynamicSpecCol();
		pMap->SetDynamicSpecCol(m_dwDynamicSpecCol);
	}
	EndMFCRes();
}

void CMapObjDlg::OnBnClickedBnadative()
{
	m_BtnAdative.EnableWindow(false);
	BeginMFCRes();
	AdaptiveDlg* adaptiveDlg = new AdaptiveDlg(this);
	m_adaptiveDlg = adaptiveDlg;
	adaptiveDlg->Create(AdaptiveDlg::IDD, this);
	adaptiveDlg->ShowWindow(SW_SHOW);
	
	adaptiveDlg->SetSimapleView(m_Cool3DView, false);
	EndMFCRes();
}
LONG CMapObjDlg::OnDelAdativeDlgMsg(WPARAM wP,LPARAM lP)
{
	m_BtnAdative.EnableWindow(true);
	m_adaptiveDlg = NULL;
	return 0;
}

void CMapObjDlg::OnBnClickedButtonResgather()
{
	m_BtnResGather.EnableWindow(false);
	BeginMFCRes();
	m_pResGatherDlg = new ResourceGather(this);
	m_pResGatherDlg->Create(ResourceGather::IDD,this);
	m_pResGatherDlg->ShowWindow(SW_SHOW);
	EndMFCRes();
}
LONG CMapObjDlg::OnNewObjMsg(WPARAM wP,LPARAM lP)
{
	BeginMFCRes();
	MapObjEdit* pNewEdit = NULL;
	GameMapEditor*  pMap= g_mapEditor->GetMap();
	ASSERT(pMap != NULL);
	CString szBuffer;
	tagResGather tempRG;
	tempRG = m_pResGatherDlg->GetRG();
	GMAPOBJ_TYPE eType = tempRG.eType;
	switch(eType)
	{
	case GMAPOBJ_Npc:
		{			
			if(tempRG.eType==GMAPOBJ_Npc && tempRG.eType!=m_objTypeList.GetCurSel())
			{
				m_objTypeList.SetCurSel(GMAPOBJ_Npc);
				OnCbnSelchangeobjtypelist();
			}
			
			DWORD attID=0;
			attID = _tstoi(tempRG.szObj);
			g_MapNpc.dwObjID = m_dwGlobalID++;
			g_MapNpc.dwTypeID = attID;
			g_MapNpc.bCollide = false;
			g_MapNpc.bFlag = false;
			g_MapNpc.bLock = false;
			pNewEdit = new MapObjEdit;
			pNewEdit->Create(g_MapNpc, pMap);

			Vector3 pos;
			pos = g_MapNpc.vPos;
			pNewEdit->SetWorldPos(pos);
			if(((CButton*)(GetDlgItem(IDC_CHECK_COPY_ADD)))->GetCheck())
			{
				pNewEdit->SetYaw(g_MapNpc.fYaw);
			}

			const tagCreatureProto* pAtt = pMap->GetAttDataBase()->FindCreatureAtt(g_MapNpc.dwTypeID);
			if( pAtt )
			{
				tstring szMdlPath = CreatureDataReader::GetCreatureMdlPath(pAtt);
				m_Cool3DView->LoadMdl(szMdlPath.c_str());	
			}
		}
		break;
	case GMAPOBJ_StaticObj:
		{
			if(tempRG.eType==GMAPOBJ_StaticObj && tempRG.eType!=m_objTypeList.GetCurSel())
			{
				m_objTypeList.SetCurSel(GMAPOBJ_StaticObj);
				OnCbnSelchangeobjtypelist();
			}

			g_StaticMapObj.dwMapID = m_dwGlobalID++; 
			_tcscpy_s(g_StaticMapObj.szMdlPath, sizeof(tempRG.szObj)/sizeof(TCHAR), tempRG.szObj);
			pNewEdit = new MapObjEdit;

			Vector3 pos;
			pos.x = g_StaticMapObj.fPos[0];
			pos.y = g_StaticMapObj.fPos[1];
			pos.z = g_StaticMapObj.fPos[2];
			if(!((CButton*)(GetDlgItem(IDC_CHECK_COPY_ADD)))->GetCheck())
			{
				g_StaticMapObj.fRotate[0]=0.0f;
				g_StaticMapObj.fRotate[1]=0.0f;
				g_StaticMapObj.fRotate[2]=0.0f;

				g_StaticMapObj.fScale[0]=1.0f;
				g_StaticMapObj.fScale[1]=1.0f;
				g_StaticMapObj.fScale[2]=1.0f;
			}
			pNewEdit->Create(g_StaticMapObj, pMap);
			pNewEdit->SetWorldPos(pos);

			//--判别文件类型
			CString szMdlName = tempRG.szObj;
			int nlength=szMdlName.GetLength();
			int nNum=szMdlName.ReverseFind(_T('.'));
			CString szfiletype=szMdlName.Right(nlength-nNum-1);

			m_szStaticFileOpenType=_T("*.")+szfiletype;

			if(szfiletype!=_T("sfx") && szfiletype!=_T("SFX"))
			{
				m_Cool3DView->LoadMdl(szMdlName.GetBuffer());
			}
		}
		break;
	case GMAPOBJ_DynamicBlock:
		{
			if(tempRG.eType==GMAPOBJ_DynamicBlock && tempRG.eType!=m_objTypeList.GetCurSel())
			{
				m_objTypeList.SetCurSel(GMAPOBJ_DynamicBlock);
				OnCbnSelchangeobjtypelist();
			}

			g_DynamicBlock.dwObjID = m_dwGlobalID++; 
			_tcscpy_s(g_DynamicBlock.szModelPath, sizeof(tempRG.szObj)/sizeof(TCHAR), tempRG.szObj);
			pNewEdit = new MapObjEdit;

			Vector3 pos;
			pos = g_DynamicBlock.vPos;
			
			if(!((CButton*)(GetDlgItem(IDC_CHECK_COPY_ADD)))->GetCheck())
			{
				g_DynamicBlock.fYaw = 0.0f;
				g_DynamicBlock.fScale[0]=1.0f;
				g_DynamicBlock.fScale[1]=1.0f;
				g_DynamicBlock.fScale[2]=1.0f;
			}
			pNewEdit->Create(g_DynamicBlock, pMap);
			pNewEdit->SetWorldPos(pos);

			//--判别文件类型
			CString szMdlName = tempRG.szObj;
			int nlength=szMdlName.GetLength();
			int nNum=szMdlName.ReverseFind(_T('.'));
			CString szfiletype=szMdlName.Right(nlength-nNum-1);

			m_szStaticFileOpenType=_T("*.")+szfiletype;

			if(szfiletype!=_T("sfx") && szfiletype!=_T("SFX"))
			{
				m_Cool3DView->LoadMdl(szMdlName.GetBuffer());
			}
		}
		break;
	case GMAPOBJ_MapDoor:
		{
			if(tempRG.eType==GMAPOBJ_MapDoor && tempRG.eType!=m_objTypeList.GetCurSel())
			{
				m_objTypeList.SetCurSel(GMAPOBJ_MapDoor);
				OnCbnSelchangeobjtypelist();
			}

			g_MapDoor.dwObjID = m_dwGlobalID++; 
			_tcscpy_s(g_MapDoor.szModelPath, sizeof(tempRG.szObj)/sizeof(TCHAR), tempRG.szObj);
			pNewEdit = new MapObjEdit;

			Vector3 pos;
			pos = g_MapDoor.vPos;

			if(!((CButton*)(GetDlgItem(IDC_CHECK_COPY_ADD)))->GetCheck())
			{
				g_MapDoor.fYaw = 0.0f;
				g_MapDoor.fScale[0]=1.0f;
				g_MapDoor.fScale[1]=1.0f;
				g_MapDoor.fScale[2]=1.0f;
			}
			g_MapDoor.bInitState = FALSE;
			g_MapDoor.bBlockingTileNPC = FALSE;
			pNewEdit->Create(g_MapDoor, pMap);
			pNewEdit->SetWorldPos(pos);

			//--判别文件类型
			CString szMdlName = tempRG.szObj;
			int nlength=szMdlName.GetLength();
			int nNum=szMdlName.ReverseFind(_T('.'));
			CString szfiletype=szMdlName.Right(nlength-nNum-1);

			m_szStaticFileOpenType=_T("*.")+szfiletype;

			if(szfiletype!=_T("sfx") && szfiletype!=_T("SFX"))
			{
				m_Cool3DView->LoadMdl(szMdlName.GetBuffer());
			}
		}
		break;
	case GMAPOBJ_Sound:
		{
			if(tempRG.eType==GMAPOBJ_Sound && tempRG.eType!=m_objTypeList.GetCurSel())
			{
				m_objTypeList.SetCurSel(GMAPOBJ_Sound);
				OnCbnSelchangeobjtypelist();
			}
			tagMapSound*  pSound=new tagMapSound;
			pSound->fRange = 1000;
			_tcscpy_s(pSound->szFileName,sizeof(pSound->szFileName)/sizeof(TCHAR),tempRG.szObj);
			pMap->SetSelectedSound(pSound);
			m_bIsBuildSound=true;
			m_objTypeList.EnableWindow(false);
		}
		break;
	default:
		ASSERT(0 && "Unknow Type!");
		break;
	}

	if(pNewEdit)
	{
		SetPreCreate(pNewEdit);
		g_mapEditor->GetMap()->SetSelectObj(pNewEdit);
		szBuffer.Format(_T("%x"), pNewEdit->GetMapID());
		AddObjIDToObjList(szBuffer);
		m_objTypeList.EnableWindow(false);
	}
	EndMFCRes();
	return 0;
}
LONG CMapObjDlg::OnDelRGDlgMsg(WPARAM wP,LPARAM lP)
{
	m_BtnResGather.EnableWindow(TRUE);
	m_pResGatherDlg = NULL;
	return 0;
}
LONG CMapObjDlg::OnShowModel(WPARAM wP, LPARAM lP)
{
	CString szMdlPath = m_pResGatherDlg->GetModelPath();
	bool bShow = m_pResGatherDlg->GetIsShow();		//静态物体可以显示
	if( _tcslen(szMdlPath)==0 || !bShow )
		return 0;
	m_Cool3DView->LoadMdl(szMdlPath.GetBuffer());
	if(m_adaptiveDlg == NULL)
		OnBnClickedBnadative();
	return 1;
}


void CMapObjDlg::hideNpcStaObjByLock(MapObjEdit::EType type)
{
	typedef list<MapObjEdit*> EDIT;
	EDIT  pList ;
	CString szBuffer;
	int nIndex = 0;
	pList=g_mapEditor->GetMap()->GetMapObjEditList();
	EDIT::iterator pIter =pList.begin(); 
	for(; pIter != pList.end(); ++pIter)
	{
		if ((*pIter)->GetType() == type)
		{
			DWORD ID = (*pIter)->GetMapID();

			szBuffer.Format(_T("%x"), ID);
			int row;
			if(type == MapObjEdit::NPC)
			{
				tagMapNPC* p = (tagMapNPC*)(*pIter)->GetMapNPC();
				if(p->bLock)
				{
					row = AddObjIDToObjList(szBuffer);
					m_objList.SetItem(row, LOCK_COL, LVIF_IMAGE, NULL, EImg_Lock, 0, 0, 0, 0);
					if(p->bFlag)
						m_objList.SetItem(row, FLAG_COL, LVIF_IMAGE, NULL, EImg_Flag, 0, 0, 0, 0);
					else
						m_objList.SetItem(row, FLAG_COL, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
				}
			}
			else if(type == MapObjEdit::StaticObj)
			{
				tagStaticMapObj* p = (tagStaticMapObj*)(*pIter)->GetStaticMapObj();
				if(p->bLock)
				{
					row = AddObjIDToObjList(szBuffer);
					m_objList.SetItem(row, LOCK_COL, LVIF_IMAGE, NULL, EImg_Lock, 0, 0, 0, 0);
					if(p->bFlag)
						m_objList.SetItem(row, FLAG_COL, LVIF_IMAGE, NULL, EImg_Flag, 0, 0, 0, 0);
					else
						m_objList.SetItem(row, FLAG_COL, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
				}
			}
		}
	}
}
void CMapObjDlg::hideNpcStaObjByFlag(MapObjEdit::EType type)
{
	typedef list<MapObjEdit*> EDIT;
	EDIT  pList ;
	CString szBuffer;
	int nIndex = 0;
	pList=g_mapEditor->GetMap()->GetMapObjEditList();
	EDIT::iterator pIter =pList.begin(); 
	for(; pIter != pList.end(); ++pIter)
	{
		if ((*pIter)->GetType() == type)
		{
			DWORD ID = (*pIter)->GetMapID();

			szBuffer.Format(_T("%x"), ID);
			int row;
			if(type == MapObjEdit::NPC)
			{
				tagMapNPC* p = (tagMapNPC*)(*pIter)->GetMapNPC();
				if(p->bFlag)
				{
					row = AddObjIDToObjList(szBuffer);
					m_objList.SetItem(row, FLAG_COL, LVIF_IMAGE, NULL, EImg_Flag, 0, 0, 0, 0);
					if(p->bLock)
						m_objList.SetItem(row, LOCK_COL, LVIF_IMAGE, NULL, EImg_Lock, 0, 0, 0, 0);
					else
						m_objList.SetItem(row, LOCK_COL, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
				}
			}
			else if(type == MapObjEdit::StaticObj)
			{
				tagStaticMapObj* p = (tagStaticMapObj*)(*pIter)->GetStaticMapObj();
				if(p->bFlag)
				{
					row = AddObjIDToObjList(szBuffer);
					m_objList.SetItem(row, FLAG_COL, LVIF_IMAGE, NULL, EImg_Flag, 0, 0, 0, 0);
					if(p->bLock)
						m_objList.SetItem(row, LOCK_COL, LVIF_IMAGE, NULL, EImg_Lock, 0, 0, 0, 0);
					else
						m_objList.SetItem(row, LOCK_COL, LVIF_IMAGE, NULL, EImg_Null, 0, 0, 0, 0);
				}
			}
		}
	}
}

bool bSortNPC(const tagMapNPC* npc1, const tagMapNPC* npc2)
{
	return npc1->dwTypeID < npc2->dwTypeID;
}

void CMapObjDlg::OnColumnclickobjlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	const int column = pNMLV->iSubItem;
	m_bColEqual = m_byColumn == column ? !m_bColEqual : true;
	m_byColumn = column;
	if(m_bColEqual)
	{
		switch(m_byColumn)
		{
		case 1:
			{
				m_objList.DeleteAllItems();
				m_propList.DeleteAllItem();
				switch(m_objTypeList.GetCurSel())
				{
				case GMAPOBJ_Npc:
					{
						typedef list<MapObjEdit*> EDIT;
						EDIT  pList ;
						CString szBuffer;
						int nIndex = 0;
						pList=g_mapEditor->GetMap()->GetMapObjEditList();
						EDIT::iterator pIter =pList.begin(); 
						for(; pIter != pList.end(); ++pIter)
						{
							if ((*pIter)->GetType() == MapObjEdit::NPC)
							{								
								tagMapNPC* p = (tagMapNPC*)(*pIter)->GetMapNPC();
								if(p != NULL)
								{
									m_vecSortNPC.push_back(p);
								}
	
							}
						}
						sort(m_vecSortNPC.begin(), m_vecSortNPC.end(), bSortNPC);

						for(vector<tagMapNPC*>::iterator it=m_vecSortNPC.begin();
							it!=m_vecSortNPC.end(); ++it)
						{
							DWORD ID = (*it)->dwObjID;

							szBuffer.Format(_T("%x"), ID);
							AddObjIDToObjList(szBuffer);
						}
						m_vecSortNPC.clear();
					}
					break;
				}
			}
			break;
		case 2://锁
			{
				m_objList.DeleteAllItems();
				m_propList.DeleteAllItem();
				switch(m_objTypeList.GetCurSel())
				{
				case GMAPOBJ_Npc:
					hideNpcStaObjByLock(MapObjEdit::NPC);
					break;
				case GMAPOBJ_StaticObj:
					hideNpcStaObjByLock(MapObjEdit::StaticObj);
					break;
				default:
					OnCbnSelchangeobjtypelist();
					break;
				}
			}
			break;
		case 3://标
			{
				m_objList.DeleteAllItems();
				m_propList.DeleteAllItem();
				switch(m_objTypeList.GetCurSel())
				{
				case GMAPOBJ_Npc:
					hideNpcStaObjByFlag(MapObjEdit::NPC);
					break;
				case GMAPOBJ_StaticObj:
					hideNpcStaObjByFlag(MapObjEdit::StaticObj);
					break;

				default:
					OnCbnSelchangeobjtypelist();
					break;
				}
			}
			break;
		}		
	}
	else
	{
		OnCbnSelchangeobjtypelist();
	}
}

//回车即应用
void CMapObjDlg::OnOK() 
{
	if((GetKeyState(VK_RETURN)&0x80)!=0)
	{
		OnBnClickedCommit();
	}
}

//快捷键处理
BOOL CMapObjDlg::PreTranslateMessage(MSG* pMsg)
{
	if (m_hAccelTable)
	{
		if (::TranslateAccelerator(*this, m_hAccelTable, pMsg)) 
		{
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}


void CMapObjDlg::OnBnClickedShowstaticpl()
{
	UpdateData();

	SceneGraph *pSG=g_mapEditor->GetMap()->GetSceneGraph();
	ASSERT(pSG!=NULL);

	if(pSG->IS_STATIC_CLASS(NullSceneGraph))
	{
		NullSceneGraph *pNullSG=(NullSceneGraph*)pSG;
		if(m_bShowStaticPL)
		{
			pNullSG->OpenStaticPL();
		}
		else
		{
			pNullSG->CloseStaticPL();
		}
	}
}

void CMapObjDlg::OnBnClickedShowdynamicpl()
{
	UpdateData();

	SceneGraph *pSG=g_mapEditor->GetMap()->GetSceneGraph();
	ASSERT(pSG!=NULL);

	if(pSG->IS_STATIC_CLASS(NullSceneGraph))
	{
		NullSceneGraph *pNullSG=(NullSceneGraph*)pSG;
		if(m_bShowDynamicPL)
		{
			pNullSG->OpenDynamicPL();
		}
		else
		{
			pNullSG->CloseDynamicPL();
		}
	}
}

void CMapObjDlg::OnBnClickedShowfog()
{
	UpdateData();

	Kernel::Inst()->GetRenderSys()->EnableFog(m_bShowFog==TRUE);
}

void CMapObjDlg::OnBnClickedCheckFollowmove()
{
	UpdateData();
	
	m_bFollowMove = ((CButton*)(GetDlgItem(IDC_CHECK_FOLLOWMOVE)))->GetCheck();
}
