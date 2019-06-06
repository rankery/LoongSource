#include "StdAfx.h"
#include ".\mappropertylist.h"
#include ".\PlugMapEditor.h"

CMapPropertyList::CMapPropertyList(void)
{
	m_nTriggerNum = 0;
	m_nMapAreaNum = 0;
}

CMapPropertyList::~CMapPropertyList(void)
{
}



void CMapPropertyList::DisplayNPC(const tagMapNPC* pNPC)
{
	CString szBuffer;
	ASSERT(pNPC != NULL);

	szBuffer.Format(_T("%u"), pNPC->dwTypeID);
	AddPropItem(new CPropertyItem(_T("属性ID"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pNPC->vPos.x);
	AddPropItem(new CPropertyItem(_T("X坐标"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pNPC->vPos.y);
	AddPropItem(new CPropertyItem(_T("Y坐标"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pNPC->vPos.z);
	AddPropItem(new CPropertyItem(_T("Z坐标"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pNPC->fYaw);
	AddPropItem(new CPropertyItem(_T("朝向"), szBuffer, PIT_EDIT, szBuffer));

	AddPropItem(new CPropertyItem(_T("巡逻导航点名称"), pNPC->szName, PIT_EDIT, szBuffer));

	if(pNPC->bCollide)             
		szBuffer = _T("是");
	else 
		szBuffer = _T("否");

	AddPropItem(new CPropertyItem(_T("是否使用碰撞"), szBuffer, PIT_COMBO, _T("是|否|")));

	szBuffer.Format(_T("%f"), pNPC->fRadius);
	AddPropItem(new CPropertyItem(_T("巢穴半径"), szBuffer, PIT_EDIT, szBuffer));


}

void CMapPropertyList::DisplayDynamicBlock(const tagMapDynamicBlock* pDynamicBlock)
{
	CString szBuffer;
	ASSERT(pDynamicBlock != NULL);

	szBuffer.Format(_T("%f"), pDynamicBlock->vPos.x);
	AddPropItem(new CPropertyItem(_T("X坐标"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pDynamicBlock->vPos.y);
	AddPropItem(new CPropertyItem(_T("Y坐标"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pDynamicBlock->vPos.z);
	AddPropItem(new CPropertyItem(_T("Z坐标"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pDynamicBlock->fScale[0]);
	AddPropItem(new CPropertyItem(_T("X缩放"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pDynamicBlock->fScale[1]);
	AddPropItem(new CPropertyItem(_T("Y缩放"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pDynamicBlock->fScale[2]);
	AddPropItem(new CPropertyItem(_T("Z缩放"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), _RadToDeg(pDynamicBlock->fYaw));
	AddPropItem(new CPropertyItem(_T("朝向"), szBuffer, PIT_EDIT, szBuffer));

	AddPropItem(new CPropertyItem(_T("模型名称"), pDynamicBlock->szModelPath , PIT_FILE,_T("")));
}

void CMapPropertyList::DisplayMapDoor(const tagMapDoor* pDoor)
{
	CString szBuffer;
	ASSERT(pDoor != NULL);

	szBuffer.Format(_T("%f"), pDoor->vPos.x);
	AddPropItem(new CPropertyItem(_T("X坐标"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pDoor->vPos.y);
	AddPropItem(new CPropertyItem(_T("Y坐标"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pDoor->vPos.z);
	AddPropItem(new CPropertyItem(_T("Z坐标"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pDoor->fScale[0]);
	AddPropItem(new CPropertyItem(_T("X缩放"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pDoor->fScale[1]);
	AddPropItem(new CPropertyItem(_T("Y缩放"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pDoor->fScale[2]);
	AddPropItem(new CPropertyItem(_T("Z缩放"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), _RadToDeg(pDoor->fYaw));
	AddPropItem(new CPropertyItem(_T("朝向"), szBuffer, PIT_EDIT, szBuffer));

	AddPropItem(new CPropertyItem(_T("模型名称"), pDoor->szModelPath , PIT_FILE,_T("")));

	if(pDoor->bInitState)
		szBuffer = _T("开");
	else 
		szBuffer = _T("关");
	AddPropItem(new CPropertyItem(_T("[关卡]门初始状态"), szBuffer, PIT_COMBO, _T("开|关|")));

	if(pDoor->bBlockingTileNPC)
		szBuffer = _T("是");
	else 
		szBuffer = _T("否");
	AddPropItem(new CPropertyItem(_T("[关卡]阻挡格子导航NPC"), szBuffer, PIT_COMBO, _T("是|否|")));

	szBuffer.Format(_T("%u"), pDoor->dwTypeID);
	AddPropItem(new CPropertyItem(_T("门TypeID"), szBuffer, PIT_EDIT, szBuffer));

}

void CMapPropertyList::DisplayMapCarrier(const tagMapCarrier* carrier)
{
	CString szBuffer;
	ASSERT(carrier != NULL);

	AddPropItem(new CPropertyItem(_T("模型名称"), carrier->szModelPath , PIT_FILE,_T("")));

	szBuffer.Format(_T("%s"), carrier->szWayPoint);
	AddPropItem(new CPropertyItem(_T("导航点名字"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), carrier->fYaw);
	AddPropItem(new CPropertyItem(_T("旋转"), szBuffer, PIT_EDIT, szBuffer));

	if( carrier->bSurfaceType==0 )
		szBuffer = _T("无");
	else if( carrier->bSurfaceType==1 )
		szBuffer = _T("雪地");
	else if( carrier->bSurfaceType==2 )
		szBuffer = _T("石块");
	else if( carrier->bSurfaceType==3 )
		szBuffer = _T("土");
	else if( carrier->bSurfaceType==4 )
		szBuffer = _T("草地");
	else if( carrier->bSurfaceType==5 )
		szBuffer = _T("沙地");
	else if( carrier->bSurfaceType==6 )
		szBuffer = _T("冰面");
	else if( carrier->bSurfaceType==7 )
		szBuffer = _T("金属");
	else
		szBuffer = _T("木头");
	AddPropItem(new CPropertyItem(_T("[关卡]表面类型"), szBuffer, PIT_COMBO, _T("无|雪地|石块|土|草地|沙地|冰面|金属|木头|")));

	szBuffer.Format(_T("%f"), carrier->fSpeed);
	AddPropItem(new CPropertyItem(_T("移动速度"), szBuffer, PIT_EDIT, szBuffer));
}

void CMapPropertyList::DisplayStaticObj(const tagStaticMapObj* pStaticMapObj)
{
	CString szBuffer;
	ASSERT(pStaticMapObj != NULL);

	szBuffer.Format(_T("%f"), pStaticMapObj->fPos[0]);
	AddPropItem(new CPropertyItem(_T("X坐标"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pStaticMapObj->fPos[1]);
	AddPropItem(new CPropertyItem(_T("Y坐标"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pStaticMapObj->fPos[2]);
	AddPropItem(new CPropertyItem(_T("Z坐标"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), _RadToDeg(pStaticMapObj->fRotate[1]));
	AddPropItem(new CPropertyItem(_T("X轴旋转角度"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), _RadToDeg(pStaticMapObj->fRotate[0]));
	AddPropItem(new CPropertyItem(_T("Y轴旋转角度"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), _RadToDeg(pStaticMapObj->fRotate[2]));
	AddPropItem(new CPropertyItem(_T("Z轴旋转角度"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pStaticMapObj->fScale[0]);
	AddPropItem(new CPropertyItem(_T("X缩放"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pStaticMapObj->fScale[1]);
	AddPropItem(new CPropertyItem(_T("Y缩放"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pStaticMapObj->fScale[2]);
	AddPropItem(new CPropertyItem(_T("Z缩放"), szBuffer, PIT_EDIT, szBuffer));

	AddPropItem(new CPropertyItem(_T("模型名称"), pStaticMapObj->szMdlPath, PIT_FILE,_T("")));

	szBuffer = pStaticMapObj->szScriptName;
	AddPropItem(new CPropertyItem(_T("[关卡]触发脚本"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%d"),pStaticMapObj->dwPickItemID);
	AddPropItem(new CPropertyItem(_T("[关卡]可捡起物品"), szBuffer, PIT_EDIT, szBuffer));

	if(pStaticMapObj->bShowMiniMap)
		szBuffer = _T("是");
	else 
		szBuffer = _T("否");
	AddPropItem(new CPropertyItem(_T("小地图中显示"), szBuffer, PIT_COMBO, _T("是|否|")));

	if(pStaticMapObj->bLightMap)
		szBuffer = _T("是");
	else 
		szBuffer = _T("否");
	AddPropItem(new CPropertyItem(_T("LightMap开关"), szBuffer, PIT_COMBO, _T("是|否|")));

	if(pStaticMapObj->byViewLevel==0)
		szBuffer = _T("自动计算");
	else if(pStaticMapObj->byViewLevel==1)
		szBuffer = _T("近");
	else if(pStaticMapObj->byViewLevel==2)
		szBuffer = _T("中");
	else
		szBuffer = _T("远");
	AddPropItem(new CPropertyItem(_T("视野等级"), szBuffer, PIT_COMBO, _T("自动计算|近|中|远|")));


	szBuffer.Format(_T("%f"), pStaticMapObj->fMtlDiffuseFactor);
	AddPropItem(new CPropertyItem(_T("漫反射强度增强系数"), szBuffer, PIT_EDIT, szBuffer));

	if(pStaticMapObj->bNoShadow)
		szBuffer = _T("是");
	else
		szBuffer = _T("否");
	AddPropItem(new CPropertyItem(_T("不产生阴影"), szBuffer, PIT_COMBO, _T("是|否|")));

	if( pStaticMapObj->bSurfaceType==0 )
		szBuffer = _T("无");
	else if( pStaticMapObj->bSurfaceType==1 )
		szBuffer = _T("雪地");
	else if( pStaticMapObj->bSurfaceType==2 )
		szBuffer = _T("石块");
	else if( pStaticMapObj->bSurfaceType==3 )
		szBuffer = _T("土");
	else if( pStaticMapObj->bSurfaceType==4 )
		szBuffer = _T("草地");
	else if( pStaticMapObj->bSurfaceType==5 )
		szBuffer = _T("沙地");
	else if( pStaticMapObj->bSurfaceType==6 )
		szBuffer = _T("冰面");
	else if( pStaticMapObj->bSurfaceType==7 )
		szBuffer = _T("金属");
	else
		szBuffer = _T("木头");
	AddPropItem(new CPropertyItem(_T("[关卡]表面类型"), szBuffer, PIT_COMBO, _T("无|雪地|石块|土|草地|沙地|冰面|金属|木头|")));

	if(pStaticMapObj->bPathfinding)
		szBuffer = _T("是");
	else
		szBuffer = _T("否");
	AddPropItem(new CPropertyItem(_T("[关卡]玩家寻路可通过"), szBuffer, PIT_COMBO, _T("是|否|")));

	if(pStaticMapObj->bNotSupportNPCNavMap)
		szBuffer = _T("是");
	else
		szBuffer = _T("否");
	AddPropItem(new CPropertyItem(_T("[关卡]不支持NPC格子导航图"), szBuffer, PIT_COMBO, _T("是|否|")));

	if( pStaticMapObj->byBoxSize==0 )
		szBuffer = _T("普通");
	else if( pStaticMapObj->byBoxSize==1 )
		szBuffer = _T("小");
	else
		szBuffer = _T("大");
	AddPropItem(new CPropertyItem(_T("[碰撞]重新生成盒子大小"), szBuffer, PIT_COMBO, _T("大|普通|小|")));
}

void CMapPropertyList::DisplayMapTriggerEffect(const tagMapTriggerEffect* pTEffect)
{
	CString szBuffer;
	ASSERT(pTEffect != NULL);

	szBuffer.Format(_T("%f"), pTEffect->fPos[0]);
	AddPropItem(new CPropertyItem(_T("X坐标"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pTEffect->fPos[1]);
	AddPropItem(new CPropertyItem(_T("Y坐标"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pTEffect->fPos[2]);
	AddPropItem(new CPropertyItem(_T("Z坐标"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), _RadToDeg(pTEffect->fRotate[1]));
	AddPropItem(new CPropertyItem(_T("X轴旋转角度"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), _RadToDeg(pTEffect->fRotate[0]));
	AddPropItem(new CPropertyItem(_T("Y轴旋转角度"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), _RadToDeg(pTEffect->fRotate[2]));
	AddPropItem(new CPropertyItem(_T("Z轴旋转角度"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pTEffect->fScale[0]);
	AddPropItem(new CPropertyItem(_T("X缩放"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pTEffect->fScale[1]);
	AddPropItem(new CPropertyItem(_T("Y缩放"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pTEffect->fScale[2]);
	AddPropItem(new CPropertyItem(_T("Z缩放"), szBuffer, PIT_EDIT, szBuffer));

	AddPropItem(new CPropertyItem(_T("模型名称"), pTEffect->szMdlPath, PIT_FILE,_T("")));
}

void CMapPropertyList::DisplayWayPointObj(const tagMapWayPoint* pWayPointObj)
{
	CString szBuffer;
	ASSERT(pWayPointObj != NULL);

	szBuffer.Format(_T("%s"), pWayPointObj->szName);
	AddPropItem(new CPropertyItem(_T("导航点名字"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%d"), pWayPointObj->byIndex);
	AddPropItem(new CPropertyItem(_T("导航点索引"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pWayPointObj->vPos.x);
	AddPropItem(new CPropertyItem(_T("X坐标"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pWayPointObj->vPos.y);
	AddPropItem(new CPropertyItem(_T("Y坐标"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pWayPointObj->vPos.z);
	AddPropItem(new CPropertyItem(_T("Z坐标"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pWayPointObj->vRange.x);
	AddPropItem(new CPropertyItem(_T("X轴方向范围"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pWayPointObj->vRange.y);
	AddPropItem(new CPropertyItem(_T("Y轴方向范围"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pWayPointObj->vRange.z);
	AddPropItem(new CPropertyItem(_T("Z轴方向范围"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pWayPointObj->fSuspend);
	AddPropItem(new CPropertyItem(_T("载具停留时间"), szBuffer, PIT_EDIT, szBuffer));

	
}

void CMapPropertyList::DisplayTriggerObj(const tagMapTrigger* pTriggerObj)
{
	m_nTriggerNum = 0;
	CString szBuffer;
	CString id;
	ASSERT(pTriggerObj != NULL);

	if(pTriggerObj->eType==EMT_Null)             
		szBuffer=_T("空");
	else if(pTriggerObj->eType==EMT_GotoNewMap)  
		szBuffer=_T("场景切换点");
	else if(pTriggerObj->eType==EMT_Script)
		szBuffer=_T("脚本触发");
	
	AddPropItem(new CPropertyItem(_T("触发器类型"), szBuffer, PIT_COMBO, _T("空|场景切换点|脚本触发|")));
	
	for(int i=0; i<(int)pTriggerObj->region.size(); i++)
	{
		if(pTriggerObj->region[i].x<=0.0f 
			|| pTriggerObj->region[i].y<0.0f
			|| pTriggerObj->region[i].z<=0.0f)
			break;
		id.Format(_T("第%d个点X坐标"), i);
		szBuffer.Format(_T("%f"), pTriggerObj->region[i].x);
		AddPropItem(new CPropertyItem(id, szBuffer, PIT_EDIT, szBuffer));

		id.Format(_T("第%d个点Y坐标"), i);
		szBuffer.Format(_T("%f"), pTriggerObj->region[i].y);
		AddPropItem(new CPropertyItem(id, szBuffer, PIT_EDIT, szBuffer));

		id.Format(_T("第%d个点Z坐标"), i);
		szBuffer.Format(_T("%f"), pTriggerObj->region[i].z);
		AddPropItem(new CPropertyItem(id, szBuffer, PIT_EDIT, szBuffer));
		m_nTriggerNum ++;
	}

	szBuffer.Format(_T("%f"), pTriggerObj->fHeight);
	AddPropItem(new CPropertyItem(_T("高度"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%s"), pTriggerObj->szMapName);
	AddPropItem(new CPropertyItem(_T("场景切换点地图名称"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%s"), pTriggerObj->szWayPoint);
	AddPropItem(new CPropertyItem(_T("目标地图位置"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%s"), pTriggerObj->szScriptName);
	AddPropItem(new CPropertyItem(_T("脚本名称"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%u"), pTriggerObj->dwParam);
	AddPropItem(new CPropertyItem(_T("参数"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%u"), pTriggerObj->dwQuestSerial);
	AddPropItem(new CPropertyItem(_T("任务序列号"), szBuffer, PIT_EDIT, szBuffer));
}

void CMapPropertyList::DisplayEventTrigger(const tagMapEventTrigger* pEventTrigger)
{
	CString szBuffer;
	CString id;
	ASSERT(pEventTrigger != NULL);

	if(pEventTrigger->eCondition==ETC_Null)             
		szBuffer=_T("空");

	AddPropItem(new CPropertyItem(_T("触发条件"), szBuffer, PIT_COMBO, _T("空|")));

	szBuffer.Format(_T("%u"), pEventTrigger->dwConditionParam1);
	AddPropItem(new CPropertyItem(_T("条件参数1"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%u"), pEventTrigger->dwConditionParam2);
	AddPropItem(new CPropertyItem(_T("条件参数2"), szBuffer, PIT_EDIT, szBuffer));
		
	if(pEventTrigger->eResult==ETR_Null)             
		szBuffer=_T("空");

	AddPropItem(new CPropertyItem(_T("触发结束"), szBuffer, PIT_COMBO, _T("空|")));

	szBuffer.Format(_T("%u"), pEventTrigger->dwResultParam1);
	AddPropItem(new CPropertyItem(_T("结果参数1"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%u"), pEventTrigger->dwResultParam2);
	AddPropItem(new CPropertyItem(_T("结果参数2"), szBuffer, PIT_EDIT, szBuffer));

}

void CMapPropertyList::DisplayMapRectObj(const tagMapArea* pMapArea)
{
	CString szBuffer;
	CString id;
	m_nMapAreaNum = 0;
	ASSERT(pMapArea != NULL);

	if(pMapArea->eType==EMA_Null)             
		szBuffer=_T("空");
	else if(pMapArea->eType==EMA_PVP)  
		szBuffer=_T("PVP区域");
	else if(pMapArea->eType==EMA_Safe)  
		szBuffer=_T("安全区域");
	else if(pMapArea->eType==EMA_Stall)  
		szBuffer=_T("摆摊区域");
	else if(pMapArea->eType==EMA_Display)  
		szBuffer=_T("显示区域");
	else if(pMapArea->eType==EMA_Prison)	
		szBuffer=_T("牢狱区域");	
	else if(pMapArea->eType==EMA_Script)	
		szBuffer=_T("脚本区域");	

	AddPropItem(new CPropertyItem(_T("触发器类型"), szBuffer, PIT_COMBO, _T("空|PVP区域|安全区域|摆摊区域|显示区域|牢狱区域|脚本区域|")));

	for(int i=0; i<(int)pMapArea->region.size(); i++)
	{
		if(pMapArea->region[i].x<=0.0f 
			|| pMapArea->region[i].y<0.0f
			|| pMapArea->region[i].z<=0.0f)
			break;
		id.Format(_T("第%d个点X坐标"), i);
		szBuffer.Format(_T("%f"), pMapArea->region[i].x);
		AddPropItem(new CPropertyItem(id, szBuffer, PIT_EDIT, szBuffer));

		id.Format(_T("第%d个点Y坐标"), i);
		szBuffer.Format(_T("%f"), pMapArea->region[i].y);
		AddPropItem(new CPropertyItem(id, szBuffer, PIT_EDIT, szBuffer));

		id.Format(_T("第%d个点Z坐标"), i);
		szBuffer.Format(_T("%f"), pMapArea->region[i].z);
		AddPropItem(new CPropertyItem(id, szBuffer, PIT_EDIT, szBuffer));
		m_nMapAreaNum ++;
	}

	szBuffer.Format(_T("%f"), pMapArea->fHeight);
	AddPropItem(new CPropertyItem(_T("高度"), szBuffer, PIT_EDIT, szBuffer));
	szBuffer.Format(_T("%ld"), pMapArea->dwMiniMapSize);
	AddPropItem(new CPropertyItem(_T("缩略图大小"), szBuffer, PIT_EDIT, szBuffer));

	if(pMapArea->bDistFog)
		szBuffer = _T("是");
	else
		szBuffer = _T("否");
	AddPropItem(new CPropertyItem(_T("是否单独设置雾"), szBuffer, PIT_COMBO, _T("是|否|")));

	tagDistFog fog;
	ASSERT( sizeof(fog) == sizeof(pMapArea->byDistFog) );
	memcpy(&fog, pMapArea->byDistFog, sizeof(fog));
	szBuffer.Format(_T("RGB(%d,%d,%d)"),fog.color.R, fog.color.G, fog.color.B);
	AddPropItem(new CPropertyItem(_T("雾颜色"), szBuffer, PIT_COLOR, _T("")));
	szBuffer.Format(_T("%f"), fog.start);
	AddPropItem(new CPropertyItem(_T("雾start"), szBuffer, PIT_EDIT, szBuffer));
	szBuffer.Format(_T("%f"), fog.end);
	AddPropItem(new CPropertyItem(_T("雾end"), szBuffer, PIT_EDIT, szBuffer));
	szBuffer.Format(_T("%f"), fog.density);
	AddPropItem(new CPropertyItem(_T("雾density"), szBuffer, PIT_EDIT, szBuffer));
}

void CMapPropertyList::DisplaySoundObj(const tagMapSound* pSoundObj)
{
	CString szBuffer;
	ASSERT(pSoundObj != NULL);

	szBuffer.Format(_T("%f"), pSoundObj->fPos[0]);
	AddPropItem(new CPropertyItem(_T("X坐标"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pSoundObj->fPos[1]);
	AddPropItem(new CPropertyItem(_T("Y坐标"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pSoundObj->fPos[2]);
	AddPropItem(new CPropertyItem(_T("Z坐标"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pSoundObj->fRange);
	AddPropItem(new CPropertyItem(_T("范围"), szBuffer, PIT_EDIT, szBuffer));

	AddPropItem(new CPropertyItem(_T("音效文件名称"), pSoundObj->szFileName, PIT_FILE,_T("")));

	szBuffer.Format(_T("%f"), pSoundObj->fVolume);
	AddPropItem(new CPropertyItem(_T("音量"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pSoundObj->fNoAtteRange);
	AddPropItem(new CPropertyItem(_T("没有衰减的范围"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%u"), pSoundObj->dwInterval);
	AddPropItem(new CPropertyItem(_T("播放一次音效时间[毫秒]"), szBuffer, PIT_EDIT, szBuffer));
}

void CMapPropertyList::DisplayPathPoint(const tagMapPathPoint* pPathPoint)
{
	CString szBuffer;
	ASSERT(pPathPoint != NULL);

	szBuffer.Format(_T("%f"), pPathPoint->vPos.x);
	AddPropItem(new CPropertyItem(_T("X坐标"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pPathPoint->vPos.y);
	AddPropItem(new CPropertyItem(_T("Y坐标"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pPathPoint->vPos.z);
	AddPropItem(new CPropertyItem(_T("Z坐标"), szBuffer, PIT_EDIT, szBuffer));

	//szBuffer.Format(_T("%f"), pPathPoint->fRange);
	//AddPropItem(new CPropertyItem(_T("范围"), szBuffer, PIT_EDIT, szBuffer));
}

void CMapPropertyList::DisplayNPCNav(const tagNPCNavMapBuildStartPoint* pNPCNav)
{
	CString szBuffer;
	ASSERT(pNPCNav != NULL);

	szBuffer.Format(_T("%f"), pNPCNav->vPos.x);
	AddPropItem(new CPropertyItem(_T("X坐标"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pNPCNav->vPos.y);
	AddPropItem(new CPropertyItem(_T("Y坐标"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pNPCNav->vPos.z);
	AddPropItem(new CPropertyItem(_T("Z坐标"), szBuffer, PIT_EDIT, szBuffer));
}

void CMapPropertyList::DisplayPlayerNav(const tagPlayerNavMapBuildStartPoint* pPlayerNav)
{
	CString szBuffer;
	ASSERT(pPlayerNav != NULL);

	szBuffer.Format(_T("%f"), pPlayerNav->vPos.x);
	AddPropItem(new CPropertyItem(_T("X坐标"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pPlayerNav->vPos.y);
	AddPropItem(new CPropertyItem(_T("Y坐标"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pPlayerNav->vPos.z);
	AddPropItem(new CPropertyItem(_T("Z坐标"), szBuffer, PIT_EDIT, szBuffer));
}

void CMapPropertyList::DisplaySpawnPoint(const tagMapSpawnPoint* pSpawnPoint)
{
	CString szBuffer;
	ASSERT(pSpawnPoint != NULL);
	
	szBuffer.Format(_T("%f"), pSpawnPoint->vPos.x);
	AddPropItem(new CPropertyItem(_T("X坐标"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pSpawnPoint->vPos.y);
	AddPropItem(new CPropertyItem(_T("Y坐标"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pSpawnPoint->vPos.z);
	AddPropItem(new CPropertyItem(_T("Z坐标"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%u"), pSpawnPoint->dwGroupID);
	AddPropItem(new CPropertyItem(_T("怪物组ID"), szBuffer, PIT_EDIT, szBuffer));


	if(pSpawnPoint->bCollide)
		szBuffer = _T("是");
	else
		szBuffer = _T("否");
	AddPropItem(new CPropertyItem(_T("是否碰撞"), szBuffer, PIT_COMBO, _T("是|否|")));

}

void CMapPropertyList::DisplayPointLightObj(const tagMapPointLight* pPointLight)
{
	CString szBuffer;
	ASSERT(pPointLight != NULL);

	szBuffer.Format(_T("%f"), pPointLight->fPos[0]);
	AddPropItem(new CPropertyItem(_T("X坐标"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pPointLight->fPos[1]);
	AddPropItem(new CPropertyItem(_T("Y坐标"), szBuffer, PIT_EDIT, szBuffer));

	szBuffer.Format(_T("%f"), pPointLight->fPos[2]);
	AddPropItem(new CPropertyItem(_T("Z坐标"), szBuffer, PIT_EDIT, szBuffer));


	szBuffer.Format(_T("RGB(%d,%d,%d)"),
		(BYTE)(pPointLight->diffuse[0]*255), (BYTE)(pPointLight->diffuse[1]*255), (BYTE)(pPointLight->diffuse[2]*255));
	AddPropItem(new CPropertyItem(_T("漫反射"), szBuffer, PIT_COLOR, _T("")));

	szBuffer.Format(_T("RGB(%d,%d,%d)"),
		(BYTE)(pPointLight->specular[0]*255), (BYTE)(pPointLight->specular[1]*255), (BYTE)(pPointLight->specular[2]*255));
	AddPropItem(new CPropertyItem(_T("镜面光"), szBuffer, PIT_COLOR, _T("")));

	szBuffer.Format(_T("%f"), pPointLight->range);
	AddPropItem(new CPropertyItem(_T("影响范围"), szBuffer, PIT_EDIT, szBuffer));


	float atten = pPointLight->attenuation[1] /** pPointLight->range*/;
	szBuffer.Format(_T("%f"), atten);
	AddPropItem(new CPropertyItem(_T("衰减"), szBuffer, PIT_EDIT, szBuffer));

	if(pPointLight->bDynamicPL)
		szBuffer = _T("是");
	else 
		szBuffer = _T("否");
	AddPropItem(new CPropertyItem(_T("动态光源 "), szBuffer, PIT_COMBO, _T("是|否|")));

	szBuffer.Format(_T("%f"), pPointLight->fIncModulus);
	AddPropItem(new CPropertyItem(_T("增强系数"), szBuffer, PIT_EDIT, szBuffer));
	
	szBuffer.Format(_T("%f"), pPointLight->fnoBlockRange);
	AddPropItem(new CPropertyItem(_T("无遮挡范围"), szBuffer, PIT_EDIT, szBuffer));
}



bool CMapPropertyList::GetModifiedWayPointProp(tagMapWayPoint& WayPoint)
{
	CString szNewName =  ((CPropertyItem*)GetItemDataPtr(0))->m_curValue;
	CString szNewIndex = ((CPropertyItem*)GetItemDataPtr(1))->m_curValue;
	FLOAT fNewX = (FLOAT)_tstof(((CPropertyItem*)GetItemDataPtr(2))->m_curValue);
	FLOAT fNewY = (FLOAT)_tstof(((CPropertyItem*)GetItemDataPtr(3))->m_curValue);
	FLOAT fNewZ =    (FLOAT)_tstof(((CPropertyItem*)GetItemDataPtr(4))->m_curValue);
	FLOAT fXZone =    (FLOAT)_tstof(((CPropertyItem*)GetItemDataPtr(5))->m_curValue);
	FLOAT fYZone =    (FLOAT)_tstof(((CPropertyItem*)GetItemDataPtr(6))->m_curValue);
	FLOAT fZZone =    (FLOAT)_tstof(((CPropertyItem*)GetItemDataPtr(7))->m_curValue);
	FLOAT fSuspend = (FLOAT)_tstof(((CPropertyItem*)GetItemDataPtr(8))->m_curValue);

	if(szNewName==_T("") || szNewIndex==_T("")) 
	{
		AfxMessageBox(_T("请将属性填写完整"));
		return false;
	}
	if(szNewName.GetLength()>31)
	{
		AfxMessageBox(_T("导航点名字过长"));
		return false;
	}
	if(_ttoi(szNewIndex)<0)
	{
		AfxMessageBox(_T("索引设置错误"));
		return false;
	}
	_tcscpy_s(WayPoint.szName,sizeof(WayPoint.szName)/sizeof(TCHAR),szNewName.GetBuffer());
	WayPoint.byIndex=_ttoi(szNewIndex);

	WayPoint.vPos = Vector3(fNewX, fNewY, fNewZ);
	WayPoint.vRange = Vector3(fXZone, fYZone, fZZone);
	WayPoint.fSuspend = fSuspend;
	return true;
}

bool CMapPropertyList::GetModifiedTriggerProp(tagMapTrigger& Trigger)
{
	CString szNewType	= ((CPropertyItem*) GetItemDataPtr(0))->m_curValue;
	int nIndex = 1;
	for(int i=0; i<m_nTriggerNum; i++)
	{
		Vector3 vPos;
		vPos.x = (FLOAT)_tstof( ((CPropertyItem*)GetItemDataPtr(nIndex++))->m_curValue );
		vPos.y = (FLOAT)_tstof( ((CPropertyItem*)GetItemDataPtr(nIndex++))->m_curValue );
		vPos.z = (FLOAT)_tstof( ((CPropertyItem*)GetItemDataPtr(nIndex++))->m_curValue );
		Trigger.region.push_back(vPos);
		
	}
	Vector3 max = Trigger.region[0], min = Trigger.region[0];
	for(int i=1; i<m_nTriggerNum; i++)
	{
		//最大点
		if(Trigger.region[i].x>=max.x)
			max.x = Trigger.region[i].x;
		if(Trigger.region[i].y>=max.y)
			max.y = Trigger.region[i].y;
		if(Trigger.region[i].z>=max.z)
			max.z = Trigger.region[i].z;

		//最小点
		if(Trigger.region[i].x<=min.x)
			min.x = Trigger.region[i].x;
		if(Trigger.region[i].y<=min.y)
			min.y = Trigger.region[i].y;
		if(Trigger.region[i].z<=min.z)
			min.z = Trigger.region[i].z;	
	}
	Trigger.box.max = max;
	Trigger.box.min = min;

	FLOAT fHeight		= (FLOAT)_tstof( ((CPropertyItem*)GetItemDataPtr(nIndex++))->m_curValue );
	CString szMapName	= ((CPropertyItem*) GetItemDataPtr(nIndex++))->m_curValue;
	CString szWayPoint	= ((CPropertyItem*)GetItemDataPtr(nIndex++))->m_curValue ;
	CString szScriptName= ((CPropertyItem*) GetItemDataPtr(nIndex++))->m_curValue;
	DWORD dwParam		= (DWORD)_tstoi( ((CPropertyItem*)GetItemDataPtr(nIndex++))->m_curValue );
	DWORD dwQuest		= (DWORD)_tstoi( ((CPropertyItem*)GetItemDataPtr(nIndex++))->m_curValue );


	if(szNewType==_T("空"))
		Trigger.eType=EMT_Null;
	else if(szNewType==_T("场景切换点"))
		Trigger.eType=EMT_GotoNewMap;
	else if(szNewType==_T("脚本触发"))
		Trigger.eType=EMT_Script;
	Trigger.fHeight = fHeight;
	Trigger.box.max.y = Trigger.fHeight + Trigger.box.min.y;
	Trigger.dwParam = dwParam;
	Trigger.dwQuestSerial = dwQuest;
	
	_tcscpy_s(Trigger.szMapName,sizeof(Trigger.szMapName)/sizeof(TCHAR),szMapName.GetBuffer());
	_tcscpy_s(Trigger.szWayPoint,sizeof(Trigger.szWayPoint)/sizeof(TCHAR),szWayPoint.GetBuffer());
	_tcscpy_s(Trigger.szScriptName,sizeof(Trigger.szScriptName)/sizeof(TCHAR),szScriptName.GetBuffer());
		
	return true;
}

bool CMapPropertyList::GetModifyedEventTrigger(tagMapEventTrigger* pEventTrigger)
{
	CString szCondition	= ((CPropertyItem*) GetItemDataPtr(0))->m_curValue;
	if(szCondition == _T("空"))
		pEventTrigger->eCondition = ETC_Null;

	pEventTrigger->dwConditionParam1 =  (DWORD)_tstoi( ((CPropertyItem*)GetItemDataPtr(1))->m_curValue );
	pEventTrigger->dwConditionParam2 =  (DWORD)_tstoi( ((CPropertyItem*)GetItemDataPtr(2))->m_curValue );


	CString szResult = ((CPropertyItem*) GetItemDataPtr(3))->m_curValue;
	if(szResult == _T("空"))
		pEventTrigger->eResult = ETR_Null;

	pEventTrigger->dwResultParam1 =  (DWORD)_tstoi( ((CPropertyItem*)GetItemDataPtr(4))->m_curValue );
	pEventTrigger->dwResultParam2 =  (DWORD)_tstoi( ((CPropertyItem*)GetItemDataPtr(5))->m_curValue );

	return true;
}

//--得到修改的地图区域数据
bool CMapPropertyList::GetModifyedMapRectProp(tagMapArea* pMapRect)
{
	CString szNewType	= ((CPropertyItem*) GetItemDataPtr(0))->m_curValue;
	if(szNewType==_T("空"))
		pMapRect->eType = EMA_Null;
	else if(szNewType==_T("PVP区域"))
		pMapRect->eType = EMA_PVP;
	else if(szNewType==_T("安全区域"))
		pMapRect->eType = EMA_Safe;
	else if(szNewType==_T("摆摊区域"))
		pMapRect->eType = EMA_Stall;
	else if(szNewType==_T("显示区域"))
		pMapRect->eType = EMA_Display;
	else if(szNewType==_T("牢狱区域"))
		pMapRect->eType = EMA_Prison;
	else if(szNewType==_T("脚本区域"))
		pMapRect->eType = EMA_Script;

	int nIndex = 1;
	for(int i=0; i<m_nMapAreaNum; i++)
	{
		pMapRect->region[i].x = (FLOAT)_tstof( ((CPropertyItem*)GetItemDataPtr(nIndex++))->m_curValue );
		pMapRect->region[i].y = (FLOAT)_tstof( ((CPropertyItem*)GetItemDataPtr(nIndex++))->m_curValue );
		pMapRect->region[i].z = (FLOAT)_tstof( ((CPropertyItem*)GetItemDataPtr(nIndex++))->m_curValue );

	}
	Vector3 max = pMapRect->region[0], min = pMapRect->region[0];
	for(int i=1; i<m_nMapAreaNum; i++)
	{
		//最大点
		if(pMapRect->region[i].x>=max.x)
			max.x = pMapRect->region[i].x;
		if(pMapRect->region[i].y>=max.y)
			max.y = pMapRect->region[i].y;
		if(pMapRect->region[i].z>=max.z)
			max.z = pMapRect->region[i].z;

		//最小点
		if(pMapRect->region[i].x<=min.x)
			min.x = pMapRect->region[i].x;
		if(pMapRect->region[i].y<=min.y)
			min.y = pMapRect->region[i].y;
		if(pMapRect->region[i].z<=min.z)
			min.z = pMapRect->region[i].z;	
	}
	pMapRect->box.max = max;
	pMapRect->box.min = min;

	FLOAT fHeight		= (FLOAT)_tstof( ((CPropertyItem*)GetItemDataPtr(nIndex++))->m_curValue );
	pMapRect->fHeight = fHeight;
	pMapRect->box.max.y = pMapRect->fHeight + pMapRect->box.min.y;
	pMapRect->dwMiniMapSize = (DWORD)_tstol( ((CPropertyItem*)GetItemDataPtr(nIndex++))->m_curValue );

	// 雾
	CString szIsFog	= ((CPropertyItem*) GetItemDataPtr(nIndex++))->m_curValue;
	if(szIsFog==_T("是"))
		pMapRect->bDistFog = true;
	else if(szIsFog==_T("否"))
		pMapRect->bDistFog = false;
	tagDistFog fog;
	memset( &fog, 0, sizeof(fog) );
	COLORREF clr = CPropertyList::Str2Color(((CPropertyItem*) GetItemDataPtr(nIndex++))->m_curValue);
	fog.color.R = GetRValue(clr);
	fog.color.G = GetGValue(clr);
	fog.color.B = GetBValue(clr);
	fog.color.A = 0xff;
	fog.start   = (float)_tstof( ((CPropertyItem*)GetItemDataPtr(nIndex++))->m_curValue );
	fog.end     = (float)_tstof( ((CPropertyItem*)GetItemDataPtr(nIndex++))->m_curValue );
	fog.density = (float)_tstof( ((CPropertyItem*)GetItemDataPtr(nIndex++))->m_curValue );
	ASSERT( sizeof(fog) == sizeof(pMapRect->byDistFog) );
	memcpy(pMapRect->byDistFog, &fog, sizeof(fog));

	return true;
}


//得到修改NPC数据
bool CMapPropertyList::GetModifiedNPCObjProp(tagMapNPC* p,Vector3& pos,float &fYaw,bool &bIsLoadMDL)
{
	pos.x = (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(1))->m_curValue);
	pos.y = (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(2))->m_curValue);
	pos.z = (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(3))->m_curValue);
	fYaw  =	(FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(4))->m_curValue);

	CPropertyItem* pItem=(CPropertyItem*) GetItemDataPtr(5);
	_tcscpy_s(p->szName, sizeof(p->szName)/sizeof(TCHAR)-1, pItem->m_curValue);

	CString szNewType	= ((CPropertyItem*) GetItemDataPtr(6))->m_curValue;
	if(szNewType==_T("是"))
		p->bCollide = true;
	else if(szNewType==_T("否"))
		p->bCollide = false;

	p->fRadius = (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(7))->m_curValue);

	DWORD id = _ttoi((((CPropertyItem*) GetItemDataPtr(0))->m_curValue)); 
	bIsLoadMDL=false;
	if(id != p->dwTypeID)
	{
		if(g_mapEditor->GetMap()->GetAttDataBase()->FindCreatureAtt(id) == NULL)
		{
			AfxMessageBox(_T("无效的属性ID"));
			return false;
		}
	}

	if(id != p->dwTypeID)
	{
		p->dwTypeID = id;
		bIsLoadMDL=true;
	}



	return true; 
}

//--得到修改的动态障碍物数据
bool CMapPropertyList::GetModifyedDynamicBlockProp(tagMapDynamicBlock* p, Vector3& pos, bool &bIsLoadMDL)
{
	pos.x	=  (float)_tstof(((CPropertyItem*) GetItemDataPtr(0))->m_curValue);
	pos.y	=  (float)_tstof(((CPropertyItem*) GetItemDataPtr(1))->m_curValue);
	pos.z	=  (float)_tstof(((CPropertyItem*) GetItemDataPtr(2))->m_curValue);

	p->fScale[0] =  (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(3))->m_curValue);
	p->fScale[1] =  (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(4))->m_curValue);
	p->fScale[2] =  (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(5))->m_curValue);

	p->fYaw	=  (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(6))->m_curValue);

	p->fYaw = _DegToRad(p->fYaw);

	bIsLoadMDL=false;
	CString temp = ((CPropertyItem*) GetItemDataPtr(7))->m_curValue ;
	if(temp != p->szModelPath)
	{
		memset(&p->szModelPath, 0, sizeof(p->szModelPath));
		memcpy(&p->szModelPath, temp.GetBuffer(), temp.GetLength());
		bIsLoadMDL=true;
	}

	return true;
}

bool CMapPropertyList::GetModifiedMapDoor(tagMapDoor* p, Vector3& pos, bool &bIsLoadMDL)
{
	pos.x	=  (float)_tstof(((CPropertyItem*) GetItemDataPtr(0))->m_curValue);
	pos.y	=  (float)_tstof(((CPropertyItem*) GetItemDataPtr(1))->m_curValue);
	pos.z	=  (float)_tstof(((CPropertyItem*) GetItemDataPtr(2))->m_curValue);

	p->fScale[0] =  (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(3))->m_curValue);
	p->fScale[1] =  (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(4))->m_curValue);
	p->fScale[2] =  (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(5))->m_curValue);

	p->fYaw	=  (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(6))->m_curValue);

	p->fYaw = _DegToRad(p->fYaw);

	bIsLoadMDL=false;
	CString temp = ((CPropertyItem*) GetItemDataPtr(7))->m_curValue ;
	if(temp != p->szModelPath)
	{
		memset(&p->szModelPath, 0, sizeof(p->szModelPath));
		memcpy(&p->szModelPath, temp.GetBuffer(), temp.GetLength());
		bIsLoadMDL=true;
	}
	CString szTemp =  ((CPropertyItem*) GetItemDataPtr(8))->m_curValue;
	if(szTemp==_T("开"))
		p->bInitState = TRUE;
	else 
		p->bInitState = FALSE;

	szTemp =  ((CPropertyItem*) GetItemDataPtr(9))->m_curValue;
	if(szTemp==_T("是"))
		p->bBlockingTileNPC = TRUE;
	else 
		p->bBlockingTileNPC = FALSE;

	p->dwTypeID =  (DWORD)_ttoi(((CPropertyItem*) GetItemDataPtr(10))->m_curValue);


	return true;
}

bool CMapPropertyList::GetModifiedMapCarrier(tagMapCarrier* p, bool &bIsLoadMDL)
{
	bIsLoadMDL=false;
	CString temp = ((CPropertyItem*) GetItemDataPtr(0))->m_curValue ;
	if(temp != p->szModelPath)
	{
		memset(&p->szModelPath, 0, sizeof(p->szModelPath));
		memcpy(&p->szModelPath, temp.GetBuffer(), temp.GetLength());
		bIsLoadMDL=true;
	}

	temp = ((CPropertyItem*) GetItemDataPtr(1))->m_curValue ;
	if(!temp.IsEmpty())
	{
		_tcscpy_s(p->szWayPoint,sizeof(p->szWayPoint)/sizeof(TCHAR)-1,temp);
	}
	else
	{
		memset(p->szWayPoint, 0 ,sizeof(p->szWayPoint));
	}

	p->fYaw =  (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(2))->m_curValue);

	temp = ((CPropertyItem*) GetItemDataPtr(3))->m_curValue;
	if( temp == _T("无") )
		p->bSurfaceType = 0;
	else if( temp == _T("雪地") )
		p->bSurfaceType = 1;
	else if( temp == _T("石块") )
		p->bSurfaceType = 2;
	else if( temp == _T("土") )
		p->bSurfaceType = 3;
	else if( temp == _T("草地") )
		p->bSurfaceType = 4;
	else if( temp == _T("沙地") )
		p->bSurfaceType = 5;
	else if( temp == _T("冰面") )
		p->bSurfaceType = 6;
	else if( temp == _T("金属") )
		p->bSurfaceType = 7;
	else
		p->bSurfaceType = 8;

	p->fSpeed =  (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(4))->m_curValue);

	return true;
}

//得到修改静态物体数据
bool CMapPropertyList::GetModifiedStaticObjProp(tagStaticMapObj* p,Vector3& pos,bool &bIsLoadMDL)
{
	pos.x =  (float)_tstof(((CPropertyItem*) GetItemDataPtr(0))->m_curValue);
	pos.y =  (float)_tstof(((CPropertyItem*) GetItemDataPtr(1))->m_curValue);
	pos.z =  (float)_tstof(((CPropertyItem*) GetItemDataPtr(2))->m_curValue);

	p->fRotate[1] =  (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(3))->m_curValue);
	p->fRotate[0] =  (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(4))->m_curValue);
	p->fRotate[2] =  (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(5))->m_curValue);

	p->fRotate[1]=_DegToRad(p->fRotate[1]);
	p->fRotate[0]=_DegToRad(p->fRotate[0]);
	p->fRotate[2]=_DegToRad(p->fRotate[2]);

	p->fScale[0] =  (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(6))->m_curValue);
	p->fScale[1] =  (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(7))->m_curValue);
	p->fScale[2] =  (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(8))->m_curValue);
	

	bIsLoadMDL=false;

	CString temp = ((CPropertyItem*) GetItemDataPtr(9))->m_curValue ;
	if(temp != p->szMdlPath)
	{
		memset(&p->szMdlPath, 0, sizeof(p->szMdlPath));
		memcpy(&p->szMdlPath, temp.GetBuffer(), temp.GetLength());
		bIsLoadMDL=true;
	}

	temp = ((CPropertyItem*) GetItemDataPtr(10))->m_curValue ;
	if(!temp.IsEmpty())
	{
		_tcscpy_s(p->szScriptName,sizeof(p->szScriptName)/sizeof(TCHAR)-1,temp);
	}
	else
	{
		memset(p->szScriptName,0,sizeof(p->szScriptName));
	}

	temp = ((CPropertyItem*) GetItemDataPtr(11))->m_curValue ;
	p->dwPickItemID = _ttoi(temp);

	CString szTemp =  ((CPropertyItem*) GetItemDataPtr(12))->m_curValue;
	if(szTemp==_T("是"))
		p->bShowMiniMap = true;
	else 
		p->bShowMiniMap = false;

	szTemp = ((CPropertyItem*) GetItemDataPtr(13))->m_curValue;
	if(szTemp == _T("是"))
		p->bLightMap = true;
	else
		p->bLightMap = false;

	szTemp = ((CPropertyItem*) GetItemDataPtr(14))->m_curValue;
	if(szTemp == _T("自动计算"))
		p->byViewLevel = 0;
	else if(szTemp == _T("近"))
		p->byViewLevel = 1;
	else if(szTemp == _T("中"))
		p->byViewLevel = 2;
	else
		p->byViewLevel = 3;

	p->fMtlDiffuseFactor = (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(15))->m_curValue);
	if( p->fMtlDiffuseFactor<=0.0f )
	{
		p->fMtlDiffuseFactor=1.5f;
		szTemp.Format(_T("%f"),p->fMtlDiffuseFactor);
		((CPropertyItem*) GetItemDataPtr(15))->m_curValue=szTemp;
	}

	szTemp = ((CPropertyItem*) GetItemDataPtr(16))->m_curValue;
	if(szTemp == _T("是"))
		p->bNoShadow = true;
	else
		p->bNoShadow = false;

	szTemp = ((CPropertyItem*) GetItemDataPtr(17))->m_curValue;
	if( szTemp == _T("无") )
		p->bSurfaceType = 0;
	else if( szTemp == _T("雪地") )
		p->bSurfaceType = 1;
	else if( szTemp == _T("石块") )
		p->bSurfaceType = 2;
	else if( szTemp == _T("土") )
		p->bSurfaceType = 3;
	else if( szTemp == _T("草地") )
		p->bSurfaceType = 4;
	else if( szTemp == _T("沙地") )
		p->bSurfaceType = 5;
	else if( szTemp == _T("冰面") )
		p->bSurfaceType = 6;
	else if( szTemp == _T("金属") )
		p->bSurfaceType = 7;
	else
		p->bSurfaceType = 8;

	szTemp = ((CPropertyItem*) GetItemDataPtr(18))->m_curValue;
	if(szTemp == _T("是"))
		p->bPathfinding = true;
	else
		p->bPathfinding = false;

	szTemp = ((CPropertyItem*) GetItemDataPtr(19))->m_curValue;
	if(szTemp == _T("是"))
		p->bNotSupportNPCNavMap = true;
	else
		p->bNotSupportNPCNavMap = false;

	szTemp = ((CPropertyItem*) GetItemDataPtr(20))->m_curValue;
	if( szTemp == _T("普通") )
		p->byBoxSize = 0;
	else if( szTemp == _T("小") )
		p->byBoxSize = 1;
	else
		p->byBoxSize = 2;

	return true;
}

bool CMapPropertyList::GetModifyedMapTriggerEffect(tagMapTriggerEffect* p, Vector3& pos, bool &bIsLoadMDL)
{
	pos.x =  (float)_tstof(((CPropertyItem*) GetItemDataPtr(0))->m_curValue);
	pos.y =  (float)_tstof(((CPropertyItem*) GetItemDataPtr(1))->m_curValue);
	pos.z =  (float)_tstof(((CPropertyItem*) GetItemDataPtr(2))->m_curValue);

	p->fRotate[1] =  (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(3))->m_curValue);
	p->fRotate[0] =  (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(4))->m_curValue);
	p->fRotate[2] =  (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(5))->m_curValue);

	p->fRotate[1]=_DegToRad(p->fRotate[1]);
	p->fRotate[0]=_DegToRad(p->fRotate[0]);
	p->fRotate[2]=_DegToRad(p->fRotate[2]);

	p->fScale[0] =  (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(6))->m_curValue);
	p->fScale[1] =  (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(7))->m_curValue);
	p->fScale[2] =  (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(8))->m_curValue);


	bIsLoadMDL=false;

	CString temp = ((CPropertyItem*) GetItemDataPtr(9))->m_curValue ;
	if(temp != p->szMdlPath)
	{
		memset(&p->szMdlPath, 0, sizeof(p->szMdlPath));
		memcpy(&p->szMdlPath, temp.GetBuffer(), temp.GetLength());
		bIsLoadMDL=true;
	}

	return true;
}

//--得到修改的音效数据
bool CMapPropertyList::GetModifiedSoundProp(tagMapSound* pSound)
{
	CString szName =((CPropertyItem*) GetItemDataPtr(4))->m_curValue ;
	if(szName==_T(""))
	{
		AfxMessageBox(_T("音效文件名称不能为空"));
	    return false;
	}
	pSound->fPos[0] = (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(0))->m_curValue);
	pSound->fPos[1] = (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(1))->m_curValue);
	pSound->fPos[2] = (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(2))->m_curValue);
	pSound->fRange	= (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(3))->m_curValue);
	_tcscpy_s(pSound->szFileName,sizeof(pSound->szFileName)/sizeof(TCHAR),szName.GetBuffer());
	pSound->fVolume	= (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(5))->m_curValue);
	pSound->fNoAtteRange	= (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(6))->m_curValue);
	pSound->dwInterval = (DWORD)_tstoi(((CPropertyItem*) GetItemDataPtr(7))->m_curValue);

	return true;
}

bool CMapPropertyList::GetModifyedNPCNav(tagNPCNavMapBuildStartPoint* pNPCNav)
{
	pNPCNav->vPos.x = (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(0))->m_curValue);
	pNPCNav->vPos.y = (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(1))->m_curValue);
	pNPCNav->vPos.z = (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(2))->m_curValue);

	return true;
}

bool CMapPropertyList::GetModifyedPlayerNav(tagPlayerNavMapBuildStartPoint* pPlayerNav)
{
	pPlayerNav->vPos.x = (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(0))->m_curValue);
	pPlayerNav->vPos.y = (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(1))->m_curValue);
	pPlayerNav->vPos.z = (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(2))->m_curValue);

	return true;
}

//--得到修改的寻路导航点
bool CMapPropertyList::GetModifyedPathPointProp(tagMapPathPoint* pPathPoint)
{
	pPathPoint->vPos.x = (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(0))->m_curValue);
	pPathPoint->vPos.y = (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(1))->m_curValue);
	pPathPoint->vPos.z = (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(2))->m_curValue);

	//pPathPoint->fRange = (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(3))->m_curValue);
	return true;
}

//--得到修改的刷怪点
bool CMapPropertyList::GetModifyedSpawnPointProp(tagMapSpawnPoint* pSpawnPoint)
{
	pSpawnPoint->vPos.x = (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(0))->m_curValue);
	pSpawnPoint->vPos.y = (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(1))->m_curValue);
	pSpawnPoint->vPos.z = (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(2))->m_curValue);
	pSpawnPoint->dwGroupID = (DWORD)_tstoi( ((CPropertyItem*)GetItemDataPtr(3))->m_curValue );
	CString szTemp = ((CPropertyItem*) GetItemDataPtr(4))->m_curValue;
	if(szTemp == _T("是"))
		pSpawnPoint->bCollide = true;
	else 
		pSpawnPoint->bCollide = false;
	return true;
}

//--得到修改的点光源数据
bool CMapPropertyList::GetModifiedPointLightProp(tagMapPointLight* pPointLight)
{
	pPointLight->fPos[0] = (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(0))->m_curValue);
	pPointLight->fPos[1] = (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(1))->m_curValue);
	pPointLight->fPos[2] = (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(2))->m_curValue);

	COLORREF clr;

	clr = CPropertyList::Str2Color(((CPropertyItem*) GetItemDataPtr(3))->m_curValue);
	pPointLight->diffuse[0] = GetRValue(clr)/255.0f;
	pPointLight->diffuse[1] = GetGValue(clr)/255.0f;
	pPointLight->diffuse[2] = GetBValue(clr)/255.0f;
	pPointLight->diffuse[3] = 1.0f;

	clr = CPropertyList::Str2Color(((CPropertyItem*) GetItemDataPtr(4))->m_curValue);
	pPointLight->specular[0] = GetRValue(clr)/255.0f;
	pPointLight->specular[1] = GetGValue(clr)/255.0f;
	pPointLight->specular[2] = GetBValue(clr)/255.0f;
	pPointLight->specular[3] =1.0f;

	//影响范围
	pPointLight->range = (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(5))->m_curValue);

	//衰减值
	pPointLight->attenuation[0] = 0.0f;
	pPointLight->attenuation[1] = (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(6))->m_curValue);/*/pPointLight->range*/
	pPointLight->attenuation[2] = 0.0f;

	CString szTemp = ((CPropertyItem*) GetItemDataPtr(7))->m_curValue;
	if(szTemp == _T("是"))
		pPointLight->bDynamicPL = true;
	else 
		pPointLight->bDynamicPL = false;

	FLOAT fModulus = 0.0f;
	fModulus = (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(8))->m_curValue);
	if(fModulus>=2.0f)
		pPointLight->fIncModulus = 2.0f;
	else if(fModulus<0.0f)
		pPointLight->fIncModulus = 0.0f;
	else 
		pPointLight->fIncModulus = fModulus;

	FLOAT fnoBlockRange = 0.0f;
	fnoBlockRange = (FLOAT)_tstof(((CPropertyItem*) GetItemDataPtr(9))->m_curValue);
	if(fnoBlockRange<0.0f)
		pPointLight->fnoBlockRange = 0.0f;
	else 
		pPointLight->fnoBlockRange = fnoBlockRange;

	return true;
}


