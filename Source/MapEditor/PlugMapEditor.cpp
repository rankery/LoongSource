#include "StdAfx.h"
#include ".\plugmapeditor.h"
#include <io.h>
#include "..\Cool3D\EditorSupport\LightMapTempGen.h"
#include "..\Cool3D\EditorSupport\LightMapping.h"
#include "..\Cool3D\EditorSupport\Radiosity.h"
#include "..\Cool3D\EditorSupport\LightMapBlur.h"
#include "..\Cool3D\EditorSupport\AmbientOcclusion.h"
#include "..\Cool3D\Util\FileUtil.h"
#include "..\Cool3D\EditorSupport\TRiverEditor.h"
#include "..\WorldBase\NavMap\NavNullSceneGraph.h"

CPlugMapEditor* g_mapEditor = NULL;
CPlugMapEditor::CPlugMapEditor(void)
{
	g_mapEditor				= this;
	m_pEngine				= NULL;
	m_bDraged				= false;
	m_pLockSelected         = NULL;

	m_bBuildWayPoint        = false;
	m_bBuildTrigger         = false;
	m_bBuildMapRect			= false;
	m_bDragedSoundBox       = false;
	m_bDragedWayPoint       = false;
	m_bDragedTrigger        = false;
	m_bDragedMapRect		= false;
	m_bPickObject           = false;
	m_bPickChopField        = false;
	m_bDragedChopField      = false;
	m_bDragedGather			= false;
	m_bFirstMove			= true;
	m_bIsEqualType			= true;
	m_bDragedPointlight		= false;
	m_bDragedPathPoint		= false;
	m_bDragedSpawnPoint		= false;
	m_bDragedDynamicBlock	= false;
	m_bMoveTriggerPoint		= false;
	m_bMoveMapAreaPoint		= false;
	m_bDragedNPCNav			= false;
	m_bDragedPlayerNav		= false;
	m_nCarrierPoint			= 0;
	m_MapCarrier			= NULL;
}

CPlugMapEditor::~CPlugMapEditor(void)
{
}

void CPlugMapEditor::AttachEngineInterface(void* pIterface,void *pExInterface)
{
	m_pEngine = (GameMapEditor*)pIterface;
}

static HINSTANCE g_hLastInst = NULL;

void BeginMFCRes()
{
	g_hLastInst = AfxGetResourceHandle();
	HINSTANCE hMyInst = GetModuleHandle(_T("MapEditor.dlp"));
	AfxSetResourceHandle(hMyInst);
}

void EndMFCRes()
{
	ASSERT(g_hLastInst != NULL);
	AfxSetResourceHandle(g_hLastInst);
	g_hLastInst = NULL;
}

void CPlugMapEditor::Create(DWORD pParentWnd)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	BeginMFCRes();
	CWnd* pWnd = (CWnd*)pParentWnd;
	if(m_mapObjDlg.Create(IDD_MAPOBJDLG, NULL))
	{
		m_mapObjDlg.AttachParent(pWnd);
	}
	else
	{
		TRACE(_T("地图物件编辑创建 失败!\n"));
	}
	EndMFCRes();
}

void CPlugMapEditor::Destroy()
{
}

void CPlugMapEditor::SetCamera(CameraBase* pCamera)
{
	m_pCamera = pCamera;
}

const TCHAR* CPlugMapEditor::GetClassName()
{
	return _T("MapObjEditor");
}

const TCHAR* CPlugMapEditor::GetUserName()
{
	return _T("地图对象编辑器");
}

void CPlugMapEditor::ShowWindow(bool bShow)
{
	if(bShow)
	{
		RECT rect;
		m_mapObjDlg.GetWindowRect(&rect);		
		m_mapObjDlg.SetWindowPos(NULL, 0, 0, rect.right-rect.left, rect.bottom-rect.top, SWP_SHOWWINDOW);
		m_mapObjDlg.ShowWindow(SW_SHOW);
	}
	else
		m_mapObjDlg.ShowWindow(SW_HIDE);	
}

void CPlugMapEditor::RenderPreview()
{
}

void CPlugMapEditor::Render(bool bActive)
{	
	if(m_mapObjDlg.IsShowStaticObj()&&m_mapObjDlg.IsShowQuad())
		DrawStatiObjQuad();	
	m_pEngine->DrawEdit();	
}
void CPlugMapEditor::DrawStatiObjQuad()
{
	typedef list<MapObjEdit*> EDIT;
	EDIT  pList ;
	pList=g_mapEditor->GetMap()->GetMapObjEditList();
	EDIT::iterator pIter =pList.begin(); 
	for(; pIter != pList.end(); ++pIter)
	{
		if((*pIter)->GetType()==MapObjEdit::StaticObj)
		{
			Cool3D::AABBox box;
			Vector3 objPos=(*pIter)->GetWorldPos();
			if((*pIter)->GetSGNode())
				box = (*pIter)->GetSGNode()->GetAABBox();
			if((*pIter)->GetSGEffectNode())
				(*pIter)->GetSGEffectNode()->GetBox(box);
			DWORD color = 0x0000FFFF;
			Vector3 v1(box.min.x-QUAD_OFFSET, objPos.y, box.max.z+QUAD_OFFSET);
			Vector3 v2(box.min.x-QUAD_OFFSET, objPos.y, box.min.z-QUAD_OFFSET);
			Vector3 v3(box.max.x+QUAD_OFFSET, objPos.y, box.min.z-QUAD_OFFSET);
			Vector3 v4(box.max.x+QUAD_OFFSET, objPos.y, box.max.z+QUAD_OFFSET);
			DrawX::Draw3DLine(v1,v2,color);
			DrawX::Draw3DLine(v2,v3,color);
			DrawX::Draw3DLine(v3,v4,color);
			DrawX::Draw3DLine(v4,v1,color);
		}
	}	
}

void CPlugMapEditor::Update(bool bActive)
{
}

//通过静态物体方块拾取静态物体
bool CPlugMapEditor::PickStaticObjByQuad(UINT x, UINT y)
{
	typedef list<MapObjEdit*> EDIT;
	EDIT  pList ;
	pList=g_mapEditor->GetMap()->GetMapObjEditList();
	EDIT::iterator pIter =pList.begin(); 
	for(; pIter != pList.end(); ++pIter)
	{
		if((*pIter)->GetType()==MapObjEdit::StaticObj)
		{
			Cool3D::AABBox box;

			Vector3 objPos=(*pIter)->GetWorldPos();
			if((*pIter)->GetSGNode())
				box = (*pIter)->GetSGNode()->GetAABBox();
			if((*pIter)->GetSGEffectNode())
				(*pIter)->GetSGEffectNode()->GetBox(box);

			Vector3 v1(box.min.x-QUAD_OFFSET, objPos.y, box.max.z+QUAD_OFFSET);
			Vector3 v2(box.min.x-QUAD_OFFSET, objPos.y, box.min.z-QUAD_OFFSET);
			Vector3 v3(box.max.x+QUAD_OFFSET, objPos.y, box.min.z-QUAD_OFFSET);
			Vector3 v4(box.max.x+QUAD_OFFSET, objPos.y, box.max.z+QUAD_OFFSET);
			Ray ray;
			m_pCamera->GetMouseRay(x,y,ray);
			TResult tr1 = ray.IntersectTriangle(v1, v2, v3);
			TResult tr2 = ray.IntersectTriangle(v1,v3, v4);
			if(tr1.first || tr2.first)
			{
				m_pEngine->SetSelectObj((*pIter));
				return true;
			}
		}
	}	
	return false;
}


//鼠标左击筛选
bool CPlugMapEditor::MouseFilterSelected(MapObjEdit* pSelected)
{
	if(m_mapObjDlg.GetCurGMAPOBJTYPE()==GMAPOBJ_Npc)
	{
		if(!m_mapObjDlg.IsShowNPC()  /*&& IS_NPC(pSelected->GetMapID())*/)	//判断是不是隐藏NPC
		{
			tagMapNPC* pNPC = (tagMapNPC*)pSelected->GetMapNPC();
			if(!pNPC->bLock || !pNPC->bFlag)
			{
				m_pEngine->SetSelectObj(NULL);			
				return true;
			}
		}
	}
	if(m_mapObjDlg.GetCurGMAPOBJTYPE()==GMAPOBJ_StaticObj)	//判断是不是隐藏静态物体
	{
		if(!m_mapObjDlg.IsShowStaticObj() && pSelected->GetType() == MapObjEdit::StaticObj)
		{
			tagStaticMapObj* pStaticObj = (tagStaticMapObj*)pSelected->GetStaticMapObj();
			if(!pStaticObj->bLock || !pStaticObj->bFlag)
			{
				m_pEngine->SetSelectObj(NULL);			
				return true;
			}
		}
	}
	if(m_mapObjDlg.GetCurGMAPOBJTYPE()==GMAPOBJ_DynamicBlock)	//判断是不是隐藏动态障碍物
	{
		if(!m_mapObjDlg.IsShowDynamicBlock() && pSelected->GetType() == MapObjEdit::DynamicBlock)
		{
			tagMapDynamicBlock* pDynaBlock = (tagMapDynamicBlock*)pSelected->GetDynamicBlock();
			if(!pDynaBlock->bLock || !pDynaBlock->bFlag)
			{
				m_pEngine->SetSelectObj(NULL);			
				return true;
			}
		}
	}
	if(m_mapObjDlg.GetCurGMAPOBJTYPE()==GMAPOBJ_MapDoor)
	{
		if(!m_mapObjDlg.IsShowMapDoor() && pSelected->GetType() == MapObjEdit::MapDoor)
		{
			tagMapDoor* pDoor = (tagMapDoor*)pSelected->GetMapDoor();
			if(!pDoor->bLock || !pDoor->bFlag)
			{
				m_pEngine->SetSelectObj(NULL);			
				return true;
			}
		}
	}
	if(!m_mapObjDlg.IsShowLockObj())	//判断是不是隐藏已上锁地图对象
	{
		switch(pSelected->GetType())
		{
		case MapObjEdit::NPC:
			{
				tagMapNPC* pNPC = (tagMapNPC*)pSelected->GetMapNPC();
				if(pNPC->bLock)
					m_pEngine->SetSelectObj(NULL);					
				return true;
			}
			break;
		case MapObjEdit::StaticObj:
			{
				tagStaticMapObj* pStaticObj = (tagStaticMapObj*)pSelected->GetStaticMapObj();
				if(pStaticObj->bLock)
					m_pEngine->SetSelectObj(NULL);					
				return true;
			}
			break;
		case MapObjEdit::DynamicBlock:
			{
				tagMapDynamicBlock* pDynaBlock = (tagMapDynamicBlock*)pSelected->GetDynamicBlock();
				if(pDynaBlock->bLock)
					m_pEngine->SetSelectObj(NULL);					
				return true;
			}
			break;
		case MapObjEdit::MapDoor:
			{
				tagMapDoor* pDoor = (tagMapDoor*)pSelected->GetMapDoor();
				if(pDoor->bLock)
					m_pEngine->SetSelectObj(NULL);					
				return true;
			}
			break;
		}			
	}
	if(!m_mapObjDlg.IsShowFlagObj())	//判断是不是隐藏已标记地图对象
	{
		switch(pSelected->GetType())
		{
		case MapObjEdit::NPC:
			{
				tagMapNPC* pNPC = (tagMapNPC*)pSelected->GetMapNPC();
				if(pNPC->bFlag)
					m_pEngine->SetSelectObj(NULL);					
				return true;
			}
			break;
		case MapObjEdit::StaticObj:
			{
				tagStaticMapObj* pStaticObj = (tagStaticMapObj*)pSelected->GetStaticMapObj();
				if(pStaticObj->bFlag)				
					m_pEngine->SetSelectObj(NULL);					
				return true;
			}
			break;
		case MapObjEdit::DynamicBlock:
			{
				tagMapDynamicBlock* pDynaBlock = (tagMapDynamicBlock*)pSelected->GetDynamicBlock();
				if(pDynaBlock->bFlag)				
					m_pEngine->SetSelectObj(NULL);					
				return true;
			}
			break;
		case MapObjEdit::MapDoor:
			{
				tagMapDoor* pDoor = (tagMapDoor*)pSelected->GetMapDoor();
				if(pDoor->bFlag)				
					m_pEngine->SetSelectObj(NULL);					
				return true;
			}
			break;
		}			
	}
	return false;
}

Vector3 g_v3MouseMove;
void CPlugMapEditor::OnLButtonDown(UINT nFlags, UINT x,UINT y) 
{
	 //建立导航点
	 if(BuildWayPoint(x,y)) return;

	 //拾取导航点
	 if(PickWayPoint(x,y)) return;

	 //开始增加触发器
	 if(m_mapObjDlg.GetIsBuildTrigger())
	 {
		 m_bBuildTrigger=true;
	 }
	 if(m_bBuildTrigger)
	 {
		 Vector3 out;
		 m_pEngine->MousePick(x,y,out);
		 Vector3 vecObjPos;
		 const Heightmap* pHMap = m_pEngine->GetHeightMap()->GetHeightmap();
		 if( pHMap == NULL )
			 return;
		 Ray mRay;
		 m_pCamera->GetMouseRay(x, y, mRay);
		 if( m_pEngine->GetNavSG()->RayCollideBox(mRay, vecObjPos) )
			m_pEngine->SetPosForTrigger(vecObjPos);
		 else
			m_pEngine->SetPosForTrigger(out);
	 }

	 //拾取触发器的点
	 if(PickTriggerPoint(x, y)) return;

	 //拾取触发器
	 if(PickTrigger(x,y)) return;

     //建立音效
	 if(BuildSound(x,y))  return;
	
     //拾取音效
	 if(PickSound(x,y))   return;

	 //建立点光源
	 if(BuildPointLight(x, y)) return;

	 //拾取点光源
	 if(PickPointLight(x, y)) return;

	 //增加地图区域
	 if(m_mapObjDlg.GetIsBuildMapRect())
	 {
		 m_bBuildMapRect = true;
	 }
	 if(m_bBuildMapRect)
	 {
		 Vector3 out;
		 m_pEngine->MousePick(x, y, out);
		 Vector3 vecObjPos;
		 const Heightmap* pHMap = m_pEngine->GetHeightMap()->GetHeightmap();
		 if( pHMap == NULL )
			 return;
		 Ray mRay;
		 m_pCamera->GetMouseRay(x, y, mRay);
		  if( m_pEngine->GetNavSG()->RayCollideBox(mRay, vecObjPos) )
			m_pEngine->SetPosForMapArea(vecObjPos);
		  else
			m_pEngine->SetPosForMapArea(out);
	 }

	 if(PickMapAreaPoint(x, y)) return;

	 //拾取地图区域
	 if(PickMapRect(x, y)) return;

	 //建立寻路导航点
	 if(BuildPathPoint(x, y))return ;

	 if(PickPathPoint(x, y)) return;

	 //建立刷怪点
	 if(BuildSpawnPoint(x, y)) return;

	 if(PickSpawnPoint(x, y)) return;
	 //--
	 if(BuildNPCNavStartPoint(x, y)) return;

	 if(PickNPCNavStartPoint(x, y)) return;
	 //--
	 if(BuildPlayerNavStartPoint(x, y)) return;

	 if(PickPlayerNavStartPoint(x, y)) return;

	 //拾取物体
	 if(!m_bPickObject)  return;
	 if(m_mapObjDlg.GetPreCreate())
	 {
		 Vector3 vecPos;
		 m_pEngine->MousePick(x, y, vecPos);
		 switch(m_mapObjDlg.GetPreCreate()->GetType())
		 {
		 case MapObjEdit::StaticObj:
			 {				
				 m_mapObjDlg.m_objTypeList.EnableWindow(true);
				 m_mapObjDlg.SetPreCreate(NULL);
				 return;
			 }					
			break;
		 case MapObjEdit::MapTrrigerEffect:
			 {
				 m_mapObjDlg.m_objTypeList.EnableWindow(true);
				 m_mapObjDlg.SetPreCreate(NULL);
				 return;
			 }
			 break;
		 case MapObjEdit::NPC:
			 {
				m_mapObjDlg.m_objTypeList.EnableWindow(true);
				m_mapObjDlg.SetPreCreate(NULL);
				return;
			 }
			 break;
		 case MapObjEdit::DynamicBlock:
			 {
				 m_mapObjDlg.m_objTypeList.EnableWindow(true);
				 m_mapObjDlg.SetPreCreate(NULL);
				 return;
			 }
			 break;
		 case MapObjEdit::MapDoor:
			 {
				m_mapObjDlg.m_objTypeList.EnableWindow(true);
				m_mapObjDlg.SetPreCreate(NULL);
				return;
			 }
			 break;
		 case MapObjEdit::MapCarrier:
			 {
				 m_mapObjDlg.m_objTypeList.EnableWindow(true);
				 m_mapObjDlg.SetPreCreate(NULL);
				 return;
			 }
			 break;
		}
	 }

	 if(m_mapObjDlg.GetIsLockOnObject() && m_pEngine->GetSelectedObj())
	 {
		 m_pEngine->GetNavSG()->SetCurNavEntityNode(m_pEngine->GetSelectedObj()->GetNavEntityNode());
		 //记录鼠标第一次点的位置
		 Vector3 vecPos;
		 m_pEngine->MousePick(x, y, vecPos);
		 Vector3 vecObjPos;
		 const Heightmap* pHMap = m_pEngine->GetHeightMap()->GetHeightmap();
		 if( pHMap == NULL )
			 return;
		 Ray mRay;
		 m_pCamera->GetMouseRay(x, y, mRay);
		 bool bCollide = m_pEngine->GetNavSG()->RayCollideBox(mRay, vecObjPos);
		 if(bCollide)
			 g_v3MouseMove = vecObjPos;
		 else 
			 g_v3MouseMove = vecPos;
		 m_bDraged = true;
		 return;
	 }

	 MapObjEdit::EType type=MapObjEdit::None;
	switch(m_mapObjDlg.GetCurGMAPOBJTYPE())
	{
	case GMAPOBJ_StaticObj:
		{
			type = MapObjEdit::StaticObj;
		}
		break;
	case GMAPOBJ_DynamicBlock:
		{
			type = MapObjEdit::DynamicBlock;
		}
		break;
	case GMAPOBJ_Npc:
		{
			type = MapObjEdit::NPC;
		}
		break;
	case GMAPOBJ_MapDoor:
		{
			type = MapObjEdit::MapDoor;
		}
		break;
	case GMAPOBJ_MapCarrier:
		{
			type = MapObjEdit::MapCarrier;
		}
		break;
	case GMAPOBJ_MapTriggerEffect:
		{
			type = MapObjEdit::MapTrrigerEffect;
		}
		break;
	case GMAPOBJ_WAYPOINT:
	case GMAPOBJ_Trigger:
	case GMAPOBJ_Sound:
	case GMAPOBJ_NUM:
		break;
	}

	
	if(m_pEngine->SelectObjByMouseEx(x, y,type))
	{
		MapObjEdit* pSelected = m_pEngine->GetSelectedObj();
		if(MouseFilterSelected(pSelected))
			return;
		if(pSelected==NULL) return;
		m_bDraged = true;
		m_pEngine->GetNavSG()->SetCurNavEntityNode(pSelected->GetNavEntityNode());
		m_mapObjDlg.UpdateList(pSelected);
		//记录鼠标第一次点的位置
		Vector3 vecPos;
		m_pEngine->MousePick(x, y, vecPos);
		Vector3 vecObjPos;
		const Heightmap* pHMap = m_pEngine->GetHeightMap()->GetHeightmap();
		if( pHMap == NULL )
			return;
		Ray mRay;
		m_pCamera->GetMouseRay(x, y, mRay);
		bool bCollide = m_pEngine->GetNavSG()->RayCollideBox(mRay, vecObjPos);
		if(bCollide)
			g_v3MouseMove = vecObjPos;
		else 
			g_v3MouseMove = vecPos;
		//--设置锁定目标;
		m_pLockSelected = m_pEngine->GetSelectedObj();
		if( type == MapObjEdit::MapCarrier )
		{
			tagMapCarrier* p = (tagMapCarrier*)(m_pLockSelected)->GetMapCarrier();	
			if(p!=NULL && _tcslen(p->szWayPoint)!=0 )
			{
				if( m_MapCarrier==p )
				{
					if(++m_nCarrierPoint<(INT)m_vecCarrierPoint.size())
					{
						m_pLockSelected->SetWorldPos(m_vecCarrierPoint[m_nCarrierPoint]);
						m_pLockSelected->SetYaw(p->fYaw);
						return;
					}
					else
					{
						m_nCarrierPoint = 0;
						m_pLockSelected->SetWorldPos(m_vecCarrierPoint[0]);
						m_pLockSelected->SetYaw(p->fYaw);
					}
				}
				else if( m_MapCarrier!=p  )
				{
					m_nCarrierPoint = -1;
					m_MapCarrier = p;
					m_vecCarrierPoint.clear();
				}

				list<tagMapWayPoint*> WayPointList;
				g_mapEditor->GetMap()->GetWayPointList(WayPointList);
				list<tagMapWayPoint*>::iterator nextIter;
				for(list<tagMapWayPoint*>::iterator iter=WayPointList.begin();iter!=WayPointList.end();++iter)
				{	
					if( _tcscmp((*iter)->szName, p->szWayPoint)==0 )
					{
						m_vecCarrierPoint.push_back( (*iter)->vPos );
					}
				}		
			}
		}
	}
	else
	{	if(BST_CHECKED == ((CButton*)(m_mapObjDlg.GetDlgItem(IDC_CHECK_LockSelected)))->GetCheck())
			m_pEngine->SetSelectObj(m_pLockSelected);
	}
	
}

void CPlugMapEditor::OnLButtonUp(UINT nFlags, UINT x,UINT y)
{
	m_bDraged			= false;
	m_bDragedSoundBox	= false;
	m_bDragedWayPoint	= false;
	m_bDragedTrigger	= false;
	m_bDragedMapRect	= false;
	m_bDragedChopField	= false;
	m_bDragedGather		= false;
	m_bFirstMove		= true;
	m_bIsEqualType		= true;
	m_bDragedPointlight	= false;
	m_bDragedPathPoint	= false;
	m_bDragedSpawnPoint = false;
	m_bMoveTriggerPoint = false;
	m_bMoveMapAreaPoint	= false;
	m_bDragedNPCNav		= false;
	m_bDragedPlayerNav	= false;
	
	m_pEngine->GetNavSG()->SetCurNavEntityNode(NULL);
	m_mapObjDlg.UpdateList(m_pEngine->GetSelectedObj());	
	m_mapObjDlg.UpdatePropListForPointLight(m_pEngine->GetSelectedPointLight());
	m_pEngine->ClearPickTriggerPoint();
	m_pEngine->ClearPickMapAreaPoint();
}

void CPlugMapEditor::OnMouseMove(UINT nFlags, UINT x,UINT y)
{
	Vector3 vecPos;
	m_pEngine->MousePick(x, y, vecPos);

	Vector3 vecObjPos;
	const Heightmap* pHMap = m_pEngine->GetHeightMap()->GetHeightmap();
	if( pHMap == NULL )
		return;
	Ray mRay;
	m_pCamera->GetMouseRay(x, y, mRay);
	
	bool bCollide = m_pEngine->GetNavSG()->RayCollideBox(mRay, vecObjPos);
	//for build trigger
	if(m_bBuildTrigger)
	{
		
		if( bCollide )
			m_pEngine->SetCurPosForTrigger(vecObjPos,m_bBuildTrigger);
		else 
			m_pEngine->SetCurPosForTrigger(vecPos,m_bBuildTrigger);
	}

    //for build waypoint
	if(m_bBuildWayPoint)
	{
		if( bCollide )
			m_pEngine->SetCurPosForWayPoint(vecObjPos,true);
		else
			m_pEngine->SetCurPosForWayPoint(vecPos,true);
	}

	if(m_bBuildMapRect)
	{
		if( bCollide )
			m_pEngine->SetCurPosForMapRect(vecObjPos, m_bBuildMapRect);
		else
			m_pEngine->SetCurPosForMapRect(vecPos, m_bBuildMapRect);
	}
	
	if( bCollide )
	{
		m_pEngine->SetCurPosForPickTrigger(vecObjPos);
		m_pEngine->SetCurPosForPickWayPoint(vecObjPos);
		m_pEngine->SetPosForPointLight(vecObjPos);
		m_pEngine->SetPosForSound(vecObjPos);
		m_pEngine->SetCurPosForPathPoint(vecObjPos);
		m_pEngine->SetCurPosForPickMapRect(vecObjPos);
		m_pEngine->SetCurPosForSpawnPoint(vecObjPos);
		m_pEngine->SetCurPosForNPCNavStartPoint(vecObjPos);
		m_pEngine->SetCurPosForPlayerNavStartPoint(vecObjPos);
	}
	else
	{	
		m_pEngine->SetCurPosForPickTrigger(vecPos);
		m_pEngine->SetCurPosForPickWayPoint(vecPos);
		m_pEngine->SetPosForPointLight(vecPos);
		m_pEngine->SetPosForSound(vecPos);
		m_pEngine->SetCurPosForPathPoint(vecPos);
		m_pEngine->SetCurPosForPickMapRect(vecPos);
		m_pEngine->SetCurPosForSpawnPoint(vecPos);
		m_pEngine->SetCurPosForNPCNavStartPoint(vecPos);
		m_pEngine->SetCurPosForPlayerNavStartPoint(vecPos);
	}	
	//移动拾取的触发器的点
	if(MovePickedTriggerPoint(x, y)) return;
    //移动拾取的触发器
	if(MovePickedTrigger(x,y))  return;
    //移动拾取的导航点
	if(MovePickedWayPoint(x,y)) return;
	//移动拾取的音效盒
	if(MovePickedSound(x,y))    return;
	//移动拾取的点光源
	if(MovePickedPointLight(x,y)) return;
	//移动拾取的地图区域的点
	if(MovePickedMapAreaPoint(x, y)) return;
	//移动拾取的地图区域
	if(MovePickMapRect(x, y)) return;
	//移动寻路导航点
	if(MovePickedPathPoint(x, y)) return;
	//移动刷怪点
	if(MovePickedSpawnPoint(x, y)) return;
	//
	if(MovePickedNPCNavStartPoint(x, y)) return;
	//--
	if(MovePickedPlayerNavStartPoint(x, y)) return;


	//移动物体　
	if(!m_bPickObject)  return;
	if(m_mapObjDlg.GetPreCreate())
	{
		 m_pEngine->GetNavSG()->SetCurNavEntityNode(m_mapObjDlg.GetPreCreate()->GetNavEntityNode());
		switch(m_mapObjDlg.GetPreCreate()->GetType())
		{
		case MapObjEdit::StaticObj:
			{
				if( bCollide )
					m_mapObjDlg.GetPreCreate()->SetWorldPos(vecObjPos);
				else
					m_mapObjDlg.GetPreCreate()->SetWorldPos(vecPos);
				return;
			}
			break;
		case MapObjEdit::MapTrrigerEffect:
			{
				if( bCollide )
					m_mapObjDlg.GetPreCreate()->SetWorldPos(vecObjPos);
				else
					m_mapObjDlg.GetPreCreate()->SetWorldPos(vecPos);
				return;
			}
			break;
		case MapObjEdit::NPC:
			{
				if( bCollide )
					m_mapObjDlg.GetPreCreate()->SetWorldPos(vecObjPos);
				else
					m_mapObjDlg.GetPreCreate()->SetWorldPos(vecPos);
				return;
			}	
			break;
		case MapObjEdit::DynamicBlock:
			{
				if( bCollide )
					m_mapObjDlg.GetPreCreate()->SetWorldPos(vecObjPos);
				else
					m_mapObjDlg.GetPreCreate()->SetWorldPos(vecPos);
				return;
			}	
			break;
		case MapObjEdit::MapDoor:
			{
				if( bCollide )
					m_mapObjDlg.GetPreCreate()->SetWorldPos(vecObjPos);
				else
					m_mapObjDlg.GetPreCreate()->SetWorldPos(vecPos);
				return;
			}
			break;
		case MapObjEdit::MapCarrier:
			{
				/*if( bCollide )
					m_mapObjDlg.GetPreCreate()->SetWorldPos(vecObjPos);
				else
					m_mapObjDlg.GetPreCreate()->SetWorldPos(vecPos);*/
				return;
			}
			break;
		}
		
	}
	if(m_bDraged)
	{
		MapObjEdit* pSelected = m_pEngine->GetSelectedObj();
		if(pSelected)
		{
			switch(pSelected->GetType())
			{
			case MapObjEdit::NPC:
				{
					tagMapNPC* pNPC = (tagMapNPC*)pSelected->GetMapNPC();
					if(pNPC->bLock)
						return;
					Vector3 oldPos = pSelected->GetWorldPos();
					if(m_bFirstMove)
					{
						pSelected->SetWorldPos(oldPos);
						m_bFirstMove = false;
					}			
					else if(m_bIsEqualType)
					{
						pSelected->SetWorldPos(oldPos);
						m_bIsEqualType = false;
					}
					else
					{
						if( bCollide )
						{
							Vector3 v3Pos = oldPos;
							v3Pos.x -= (g_v3MouseMove.x - vecObjPos.x);
							v3Pos.y = vecObjPos.y;
							v3Pos.z -= (g_v3MouseMove.z - vecObjPos.z);
							g_v3MouseMove = vecObjPos;
							pSelected->SetWorldPos(v3Pos);
							//pSelected->SetWorldPos(vecObjPos);
						}
						else
						{
							Vector3 v3Pos = oldPos;
							v3Pos.x -= (g_v3MouseMove.x - vecPos.x);
							v3Pos.y = vecPos.y;
							v3Pos.z -= (g_v3MouseMove.z - vecPos.z);
							g_v3MouseMove = vecPos;
							pSelected->SetWorldPos(v3Pos);
							//pSelected->SetWorldPos(vecPos);
						}
					}
				}
				break;
			case MapObjEdit::StaticObj:
				{
					tagStaticMapObj* pStaticObj = (tagStaticMapObj*)pSelected->GetStaticMapObj();
					if(pStaticObj->bLock)
						return;

					Vector3 oldPos = pSelected->GetWorldPos();
					if(m_bFirstMove)
					{
						pSelected->SetWorldPos(oldPos);
						m_bFirstMove = false;
					}			
					else if(m_bIsEqualType)
					{
						pSelected->SetWorldPos(oldPos);
						m_bIsEqualType = false;
					}
					else
					{
						byte byMoveType = 0;
						if( bCollide )
							byMoveType = 1;
						else
							byMoveType = 2;

						AABBox selAABBox;
						if(pSelected->GetSGNode())
							selAABBox = pSelected->GetSGNode()->GetAABBox();
						else if(pSelected->GetSGEffectNode())
							pSelected->GetSGEffectNode()->GetBox(selAABBox);
						
						list<MapObjEdit*> listMapObjEdit = m_pEngine->GetMapObjEditList();
						list<MapObjEdit*>::iterator pIter = listMapObjEdit.begin();
						for(; pIter!=listMapObjEdit.end(); pIter++)
						{
							//移动静态地物时，如果静态地物的AABB里有其他地图对象，则地图对象也移动
							if(m_mapObjDlg.GetFollowMove())
							{	
								if(*pIter == pSelected)
									continue;
								AABBox box;
								if((*pIter)->GetSGNode())
									box = (*pIter)->GetSGNode()->GetAABBox();
								else if((*pIter)->GetSGEffectNode())
									(*pIter)->GetSGEffectNode()->GetBox(box);
								if(box.min.x+10 >= selAABBox.min.x
									&&box.min.y+10 >= selAABBox.min.y
									&&box.min.z+10 >= selAABBox.min.z
									&&box.max.x+10 <= selAABBox.max.x
									&&box.max.y+10 <= selAABBox.max.y
									&&box.max.z+10 <= selAABBox.max.z)
								{
									Vector3 newDisPos;
									if(byMoveType==1)
									//	newDisPos = vecObjPos - oldPos;
										newDisPos = g_v3MouseMove - vecObjPos;
									else if(byMoveType == 2)
									//	newDisPos = vecPos - oldPos;
										newDisPos = g_v3MouseMove - vecPos;

									Vector3 newPos = (*pIter)->GetWorldPos();
									newPos-=newDisPos;
									(*pIter)->SetWorldPos(newPos);
								}
							}

							//被标记的地物会一起移动
							if((*pIter)->GetType()==MapObjEdit::StaticObj)
							{
								tagStaticMapObj* pObj = (tagStaticMapObj*)(*pIter)->GetStaticMapObj();
								if(pObj->bFlag)
								{
									Vector3 newDisPos;
									if(byMoveType==1)
									//	newDisPos = vecObjPos - oldPos;
										newDisPos = (g_v3MouseMove - vecObjPos);
									else if(byMoveType == 2)
									//	newDisPos = vecPos - oldPos;
										newDisPos = (g_v3MouseMove - vecPos);

									Vector3 newPos = (*pIter)->GetWorldPos();
									newPos-=newDisPos;
									if(newPos.y<0)
										newPos.y = 0;
									(*pIter)->SetWorldPos(newPos);
								}
							}

						}
						if(byMoveType == 1)
						{
							Vector3 v3Pos = oldPos;
							v3Pos.x -= (g_v3MouseMove.x - vecObjPos.x);
							v3Pos.y = vecObjPos.y;
							v3Pos.z -= (g_v3MouseMove.z - vecObjPos.z);
							g_v3MouseMove = vecObjPos;
							pSelected->SetWorldPos(v3Pos);
							//pSelected->SetWorldPos(vecObjPos);
						}
						else if(byMoveType == 2)
						{
							Vector3 v3Pos = oldPos;
							v3Pos.x -= (g_v3MouseMove.x - vecPos.x);
							v3Pos.y = vecPos.y;
							v3Pos.z -= (g_v3MouseMove.z - vecPos.z);
							g_v3MouseMove = vecPos;
							pSelected->SetWorldPos(v3Pos);
							//pSelected->SetWorldPos(vecPos);
						}
					}
				}
				break;
			case MapObjEdit::MapTrrigerEffect:
				{
					tagMapTriggerEffect* pObj = (tagMapTriggerEffect*)pSelected->GetMapTriggerEffect();
					if(pObj->bLock)
						return;

					Vector3 oldPos = pSelected->GetWorldPos();
					if(m_bFirstMove)
					{
						pSelected->SetWorldPos(oldPos);
						m_bFirstMove = false;
					}			
					else if(m_bIsEqualType)
					{
						pSelected->SetWorldPos(oldPos);
						m_bIsEqualType = false;
					}
					else
					{
						byte byMoveType = 0;
						if( bCollide )
							byMoveType = 1;
						else
							byMoveType = 2;

						AABBox selAABBox;
						if(pSelected->GetSGNode())
							selAABBox = pSelected->GetSGNode()->GetAABBox();
						else if(pSelected->GetSGEffectNode())
							pSelected->GetSGEffectNode()->GetBox(selAABBox);

						list<MapObjEdit*> listMapObjEdit = m_pEngine->GetMapObjEditList();
						list<MapObjEdit*>::iterator pIter = listMapObjEdit.begin();
						for(; pIter!=listMapObjEdit.end(); pIter++)
						{
							//移动触发特效时，如果触发特效的AABB里有其他地图对象，则地图对象也移动
							if(m_mapObjDlg.GetFollowMove())
							{	
								if(*pIter == pSelected)
									continue;
								AABBox box;
								if((*pIter)->GetSGNode())
									box = (*pIter)->GetSGNode()->GetAABBox();
								else if((*pIter)->GetSGEffectNode())
									(*pIter)->GetSGEffectNode()->GetBox(box);
								if(box.min.x+10 >= selAABBox.min.x
									&&box.min.y+10 >= selAABBox.min.y
									&&box.min.z+10 >= selAABBox.min.z
									&&box.max.x+10 <= selAABBox.max.x
									&&box.max.y+10 <= selAABBox.max.y
									&&box.max.z+10 <= selAABBox.max.z)
								{
									Vector3 newDisPos;
									if(byMoveType==1)
										newDisPos = g_v3MouseMove - vecObjPos;
									else if(byMoveType == 2)
										newDisPos = g_v3MouseMove - vecPos;

									Vector3 newPos = (*pIter)->GetWorldPos();
									newPos-=newDisPos;
									(*pIter)->SetWorldPos(newPos);
								}
							}

							//被标记的地物会一起移动
							if((*pIter)->GetType()==MapObjEdit::MapTrrigerEffect)
							{
								tagMapTriggerEffect* pObjOther = (tagMapTriggerEffect*)(*pIter)->GetStaticMapObj();
								if(pObjOther->bFlag)
								{
									Vector3 newDisPos;
									if(byMoveType==1)
										newDisPos = (g_v3MouseMove - vecObjPos);
									else if(byMoveType == 2)
										newDisPos = (g_v3MouseMove - vecPos);

									Vector3 newPos = (*pIter)->GetWorldPos();
									newPos-=newDisPos;
									if(newPos.y<0)
										newPos.y = 0;
									(*pIter)->SetWorldPos(newPos);
								}
							}

						}
						if(byMoveType == 1)
						{
							Vector3 v3Pos = oldPos;
							v3Pos.x -= (g_v3MouseMove.x - vecObjPos.x);
							v3Pos.y = vecObjPos.y;
							v3Pos.z -= (g_v3MouseMove.z - vecObjPos.z);
							g_v3MouseMove = vecObjPos;
							pSelected->SetWorldPos(v3Pos);
						}
						else if(byMoveType == 2)
						{
							Vector3 v3Pos = oldPos;
							v3Pos.x -= (g_v3MouseMove.x - vecPos.x);
							v3Pos.y = vecPos.y;
							v3Pos.z -= (g_v3MouseMove.z - vecPos.z);
							g_v3MouseMove = vecPos;
							pSelected->SetWorldPos(v3Pos);
						}
					}
				}
				break;
			case MapObjEdit::DynamicBlock:
				{
					tagMapDynamicBlock* pDynaBlock = (tagMapDynamicBlock*)pSelected->GetDynamicBlock();
					if(pDynaBlock->bLock)
						return;
					Vector3 oldPos = pSelected->GetWorldPos();
					if(m_bFirstMove)
					{
						pSelected->SetWorldPos(oldPos);
						m_bFirstMove = false;
					}			
					else if(m_bIsEqualType)
					{
						pSelected->SetWorldPos(oldPos);
						m_bIsEqualType = false;
					}
					else
					{
						if( bCollide )
						{
							Vector3 v3Pos = oldPos;
							v3Pos.x -= (g_v3MouseMove.x - vecObjPos.x);
							v3Pos.y = vecObjPos.y;
							v3Pos.z -= (g_v3MouseMove.z - vecObjPos.z);
							g_v3MouseMove = vecObjPos;
							pSelected->SetWorldPos(v3Pos);
							//pSelected->SetWorldPos(vecObjPos);
						}
						else
						{
							Vector3 v3Pos = oldPos;
							v3Pos.x -= (g_v3MouseMove.x - vecPos.x);
							v3Pos.y = vecPos.y;
							v3Pos.z -= (g_v3MouseMove.z - vecPos.z);
							g_v3MouseMove = vecPos;
							pSelected->SetWorldPos(v3Pos);
							//pSelected->SetWorldPos(vecPos);
						}
					}
				}
				break;
			case MapObjEdit::MapDoor:
				{
					tagMapDoor* pDoor = (tagMapDoor*)pSelected->GetMapDoor();
					if(pDoor->bLock)
						return;
					Vector3 oldPos = pSelected->GetWorldPos();
					if(m_bFirstMove)
					{
						pSelected->SetWorldPos(oldPos);
						m_bFirstMove = false;
					}			
					else if(m_bIsEqualType)
					{
						pSelected->SetWorldPos(oldPos);
						m_bIsEqualType = false;
					}
					else
					{
						if( bCollide )
						{
							Vector3 v3Pos = oldPos;
							v3Pos.x -= (g_v3MouseMove.x - vecObjPos.x);
							v3Pos.y = vecObjPos.y;
							v3Pos.z -= (g_v3MouseMove.z - vecObjPos.z);
							g_v3MouseMove = vecObjPos;
							pSelected->SetWorldPos(v3Pos);
							//pSelected->SetWorldPos(vecObjPos);
						}
						else
						{
							Vector3 v3Pos = oldPos;
							v3Pos.x -= (g_v3MouseMove.x - vecPos.x);
							v3Pos.y = vecPos.y;
							v3Pos.z -= (g_v3MouseMove.z - vecPos.z);
							g_v3MouseMove = vecPos;
							pSelected->SetWorldPos(v3Pos);
							//pSelected->SetWorldPos(vecPos);
						}
					}
				}	
				break;
			}
		}
	}
}

void CPlugMapEditor::OnMouseWheel(UINT nFlags, short zDelta, UINT x,UINT y)
{
	//设置音效盒高度
	if((GetKeyState(VK_LMENU)&0x80) != 0
		&& m_pEngine->GetSelectedSound()!=NULL
		&& !m_pEngine->GetSelectedSound()->bLock)
	{
		m_pEngine->GetSelectedSound()->fPos[1]+=zDelta;
		if(m_pEngine->GetSelectedSound()->fPos[1] <= 0)
			m_pEngine->GetSelectedSound()->fPos[1] = 0;
		m_mapObjDlg.UpdatePropListForSound(m_pEngine->GetSelectedSound());
	}

	//设置寻路导航点高度
	if((GetKeyState(VK_LMENU)&0x80) != 0
		&& m_pEngine->GetSelectedPathPoint()!=NULL)
	{
		MapObjEdit* pEdit = m_pEngine->GetSelectedPathPoint();
		tagMapPathPoint* pPathPoint = (tagMapPathPoint*)pEdit->GetPathPoint();
		int hight = 0;
		if(zDelta>0)
			hight = 10;
		else
			hight = -10;
		pPathPoint->vPos.y+=hight;
		if(pPathPoint->vPos.y<0)
			pPathPoint->vPos.y = 0;
		m_mapObjDlg.UpdatePropListForPathPoint(pEdit);
	}

	//设置点光源高度
	if((GetKeyState(VK_LMENU)&0x80) != 0
		&& m_pEngine->GetSelectedPointLight()!=NULL
		&& !m_pEngine->GetSelectedPointLight()->GetPointLgtObj()->bLock)
	{
		MapObjEdit* pEdit = m_pEngine->GetSelectedPointLight();
		tagMapPointLight* pPointLgt = (tagMapPointLight*)pEdit->GetPointLgtObj();
		int hight = 0;
		if(zDelta>0)
			hight = 20;
		else
			hight = -20;
		pPointLgt->fPos[1]+=hight;
		if(pPointLgt->fPos[1] <= 0)
			pPointLgt->fPos[1] = 0;
		m_mapObjDlg.UpdatePropListForPointLight(pEdit);
		pEdit->LoadLightMdl();
	}
	//点光源沿X轴
	if( (GetKeyState(0x58)&0x80) != 0
		&& m_pEngine->GetSelectedPointLight()!=NULL
		&& !m_pEngine->GetSelectedPointLight()->GetPointLgtObj()->bLock )
	{
		MapObjEdit* pEdit = m_pEngine->GetSelectedPointLight();
		tagMapPointLight* pPointLgt = (tagMapPointLight*)pEdit->GetPointLgtObj();
		int hight = 0;
		if(zDelta>0)
			hight = 5;
		else
			hight = -5;
		pPointLgt->fPos[0]+=hight;
		if(pPointLgt->fPos[0] <= 0)
			pPointLgt->fPos[0] = 0;
		m_mapObjDlg.UpdatePropListForPointLight(pEdit);
		pEdit->LoadLightMdl();
	}
	//点光源沿Z轴
	if( (GetKeyState(0x5A)&0x80) != 0 
		&& m_pEngine->GetSelectedPointLight()!=NULL
		&& !m_pEngine->GetSelectedPointLight()->GetPointLgtObj()->bLock )
	{
		MapObjEdit* pEdit = m_pEngine->GetSelectedPointLight();
		tagMapPointLight* pPointLgt = (tagMapPointLight*)pEdit->GetPointLgtObj();
		int hight = 0;
		if(zDelta>0)
			hight = 5;
		else
			hight = -5;
		pPointLgt->fPos[2]+=hight;
		if(pPointLgt->fPos[2] <= 0)
			pPointLgt->fPos[2] = 0;
		m_mapObjDlg.UpdatePropListForPointLight(pEdit);
		pEdit->LoadLightMdl();
	}
	//设置导航点高度
	if( (GetKeyState(VK_LMENU)&0x80) != 0 
		&& m_pEngine->GetCurPickedWayPoint()!=NULL )
	{
		tagMapWayPoint* pWayPoint = m_pEngine->GetCurPickedWayPoint();
		int hight = 0;
		if(zDelta>0)
			hight = 20;
		else
			hight = -20;
		pWayPoint->vPos.y+=hight;
		if( pWayPoint->vPos.y <=0 )
		{
			pWayPoint->vPos.y = 0;
		}
		m_mapObjDlg.UpdatePropListForWayPoint(pWayPoint);
	}
	//设置地物的高度
	if((GetKeyState(VK_LMENU)&0x80) != 0)
	{	
		MapObjEdit* pEdit = m_pEngine->GetSelectedObj();
		if(!pEdit)
			return;
		switch(m_mapObjDlg.GetCurGMAPOBJTYPE())
		{
		case GMAPOBJ_StaticObj:
			{
				tagStaticMapObj* p = (tagStaticMapObj*)pEdit->GetStaticMapObj();
				if(!p->bLock)
				{
					int hight = 0;
					if(zDelta>0)
						hight = 10;
					else
						hight = -10;
					p->fPos[1] += hight;
					pEdit->SetWorldPos(pEdit->GetWorldPos());
				}
			}
			break;
		case GMAPOBJ_MapTriggerEffect:
			{
				tagMapTriggerEffect* p = (tagMapTriggerEffect*)pEdit->GetMapTriggerEffect();
				if(!p->bLock)
				{
					int hight = 0;
					if(zDelta>0)
						hight = 10;
					else
						hight = -10;
					p->fPos[1] += hight;
					pEdit->SetWorldPos(pEdit->GetWorldPos());
				}
			}
			break;
		case GMAPOBJ_DynamicBlock:
			{
				tagMapDynamicBlock* p = (tagMapDynamicBlock*)pEdit->GetDynamicBlock();
				if(!p->bLock)
				{
					int hight = 0;
					if(zDelta>0)
						hight = 10;
					else
						hight = -10;
					p->vPos.y += hight;
					pEdit->SetWorldPos(pEdit->GetWorldPos());
				}
			}
			break;
		case GMAPOBJ_MapDoor:
			{
				tagMapDoor* p = (tagMapDoor*)pEdit->GetMapDoor();
				if(!p->bLock)
				{
					int hight = 0;
					if(zDelta>0)
						hight = 10;
					else
						hight = -10;
					p->vPos.y += hight;
					pEdit->SetWorldPos(pEdit->GetWorldPos());
				}
			}
			break;
		}
		m_mapObjDlg.UpdateList(m_pEngine->GetSelectedObj());
		return;
	}

	//地物沿X轴方向移动
	if((GetKeyState(0x58)&0x80) != 0)
	{	
		MapObjEdit* pEdit = m_pEngine->GetSelectedObj();
		if(pEdit==NULL)
			return;
		switch(m_mapObjDlg.GetCurGMAPOBJTYPE())
		{
		case GMAPOBJ_Npc:
			{
				tagMapNPC* p = (tagMapNPC*)pEdit->GetMapNPC();
				if(!p->bLock)
				{
					int hight = 0;
					if(zDelta>0)
						hight = 10;
					else
						hight = -10;
					p->vPos.x += hight;
					pEdit->SetWorldPos(pEdit->GetWorldPos());
				}
			}
			break;
		case GMAPOBJ_StaticObj:
			{
				tagStaticMapObj* p = (tagStaticMapObj*)pEdit->GetStaticMapObj();
				if(!p->bLock)
				{
					int hight = 0;
					if(zDelta>0)
						hight = 10;
					else
						hight = -10;
					p->fPos[0] += hight;
					pEdit->SetWorldPos(pEdit->GetWorldPos());
				}
			}
			break;
		case GMAPOBJ_MapTriggerEffect:
			{
				tagMapTriggerEffect* p = (tagMapTriggerEffect*)pEdit->GetMapTriggerEffect();
				if(!p->bLock)
				{
					int hight = 0;
					if(zDelta>0)
						hight = 10;
					else
						hight = -10;
					p->fPos[0] += hight;
					pEdit->SetWorldPos(pEdit->GetWorldPos());
				}
			}
			break;
		case GMAPOBJ_DynamicBlock:
			{
				tagMapDynamicBlock* p = (tagMapDynamicBlock*)pEdit->GetDynamicBlock();
				if(!p->bLock)
				{
					int hight = 0;
					if(zDelta>0)
						hight = 10;
					else
						hight = -10;
					p->vPos.x += hight;
					pEdit->SetWorldPos(pEdit->GetWorldPos());
				}
			}
			break;
		case GMAPOBJ_MapDoor:
			{
				tagMapDoor* p = (tagMapDoor*)pEdit->GetMapDoor();
				if(!p->bLock)
				{
					int hight = 0;
					if(zDelta>0)
						hight = 10;
					else
						hight = -10;
					p->vPos.x += hight;
					pEdit->SetWorldPos(pEdit->GetWorldPos());
				}
			}
			break;
		}
		m_mapObjDlg.UpdateList(m_pEngine->GetSelectedObj());
		return;
	}
	
	//地物沿Z轴方向移动
	if((GetKeyState(0x5A)&0x80) != 0)
	{	
		MapObjEdit* pEdit = m_pEngine->GetSelectedObj();
		if(pEdit==NULL)
			return;
		switch(m_mapObjDlg.GetCurGMAPOBJTYPE())
		{
		case GMAPOBJ_Npc:
			{
				tagMapNPC* p = (tagMapNPC*)pEdit->GetMapNPC();
				if(!p->bLock)
				{
					int hight = 0;
					if(zDelta>0)
						hight = 10;
					else
						hight = -10;
					p->vPos.z += hight;
					pEdit->SetWorldPos(pEdit->GetWorldPos());
				}
			}
			break;
		case GMAPOBJ_StaticObj:
			{
				tagStaticMapObj* p = (tagStaticMapObj*)pEdit->GetStaticMapObj();
				if(!p->bLock)
				{
					int hight = 0;
					if(zDelta>0)
						hight = 10;
					else
						hight = -10;
					p->fPos[2]+=hight;
					pEdit->SetWorldPos(pEdit->GetWorldPos());
				}
			}
			break;
		case GMAPOBJ_MapTriggerEffect:
			{
				tagMapTriggerEffect* p = (tagMapTriggerEffect*)pEdit->GetMapTriggerEffect();
				if(!p->bLock)
				{
					int hight = 0;
					if(zDelta>0)
						hight = 10;
					else
						hight = -10;
					p->fPos[2]+=hight;
					pEdit->SetWorldPos(pEdit->GetWorldPos());
				}
			}
			break;
		case GMAPOBJ_DynamicBlock:
			{
				tagMapDynamicBlock* p = (tagMapDynamicBlock*)pEdit->GetDynamicBlock();
				if(!p->bLock)
				{
					int hight = 0;
					if(zDelta>0)
						hight = 10;
					else
						hight = -10;
					p->vPos.z += hight;
					pEdit->SetWorldPos(pEdit->GetWorldPos());
				}
			}
			break;
		case GMAPOBJ_MapDoor:
			{
				tagMapDoor* p = (tagMapDoor*)pEdit->GetMapDoor();
				if(!p->bLock)
				{
					int hight = 0;
					if(zDelta>0)
						hight = 10;
					else
						hight = -10;
					p->vPos.z += hight;
					pEdit->SetWorldPos(pEdit->GetWorldPos());
				}
			}
			break;
		}
		m_mapObjDlg.UpdateList(m_pEngine->GetSelectedObj());
		return;
	}

	//对静态物进行旋转
	if((GetKeyState(VK_LSHIFT)&0x80) != 0)
	{		
		MapObjEdit* pEdit = m_pEngine->GetSelectedObj();
		if(!pEdit)
			return;
		switch(m_mapObjDlg.GetCurGMAPOBJTYPE())
		{
		case GMAPOBJ_StaticObj:
			{
				tagStaticMapObj* p = (tagStaticMapObj*)pEdit->GetStaticMapObj();
				if(!p->bLock)
				{
					float fRotate = p->fRotate[0];
					if(zDelta < 0)
					{
						fRotate+=D3DX_PI/18.0f;
						while(fRotate> 2 * D3DX_PI) 
							fRotate = 0.0f;
					}
					else if(zDelta > 0)
					{
						fRotate-=D3DX_PI/18.0f;
						while(fRotate< 0.0f) 
							fRotate = 2 * D3DX_PI;
					}
					pEdit->SetYaw(fRotate);
					pEdit->SetWorldPos(pEdit->GetWorldPos());
				}
			}
			break;
		case GMAPOBJ_DynamicBlock:
			{
				tagMapDynamicBlock* p = (tagMapDynamicBlock*)pEdit->GetDynamicBlock();
				if(!p->bLock)
				{
					float fRotate = p->fYaw;
					if(zDelta < 0)
					{
						fRotate+=D3DX_PI/18.0f;
						while(fRotate> 2 * D3DX_PI) 
							fRotate = 0.0f;
					}
					else if(zDelta > 0)
					{
						fRotate-=D3DX_PI/18.0f;
						while(fRotate< 0.0f) 
							fRotate = 2 * D3DX_PI;
					}
					pEdit->SetYaw(fRotate);
					pEdit->SetWorldPos(pEdit->GetWorldPos());
				}
			}
			break;
		case GMAPOBJ_MapCarrier:
			{
				tagMapCarrier* p = (tagMapCarrier*)pEdit->GetMapCarrier();
				if( !p->bLock )
				{
					float fRotate = p->fYaw;
					if(zDelta < 0)
					{
						fRotate+=D3DX_PI/18.0f;
						while(fRotate> 2 * D3DX_PI) 
							fRotate = 0.0f;
					}
					else if(zDelta > 0)
					{
						fRotate-=D3DX_PI/18.0f;
						while(fRotate< 0.0f) 
							fRotate = 2 * D3DX_PI;
					}
					pEdit->SetYaw(fRotate);
					pEdit->SetWorldPos(pEdit->GetWorldPos());
				}
			}
			break;
		}
		m_mapObjDlg.UpdateList(m_pEngine->GetSelectedObj());
		return;
	}
	//对静态物体进行缩放
	MapObjEdit* pEdit = m_pEngine->GetSelectedObj();

	if(pEdit && pEdit->GetType()==MapObjEdit::StaticObj)
	{		
		tagStaticMapObj* p = (tagStaticMapObj*)pEdit->GetStaticMapObj();
		if(p->bLock)		//已锁定
			return;
		const float* scaleOrg = p->fScale;
		float scale = (float)zDelta/1200;
		for(int i=0; i<3; ++i)
		{
			if(scaleOrg[i]+scale <= 0.1)
				p->fScale[i] = 0.1f;
			else
				p->fScale[i] = scaleOrg[i]+scale;
		}

		pEdit->SetMdlScale(Vector3(p->fScale[0], p->fScale[1], p->fScale[2]));
		pEdit->SetWorldPos(pEdit->GetWorldPos());
		m_mapObjDlg.UpdateList(m_pEngine->GetSelectedObj());
	}
	else if(pEdit && pEdit->GetType()==MapObjEdit::DynamicBlock)
	{
		tagMapDynamicBlock* p = (tagMapDynamicBlock*)pEdit->GetDynamicBlock();
		if(p->bLock)
			return;
		const float* scaleOrg = p->fScale;
		float scale = (float)zDelta/1200;
		for(int i=0; i<3; ++i)
		{
			if(scaleOrg[i]+scale <= 0.1)
				p->fScale[i] = 0.1f;
			else
				p->fScale[i] = scaleOrg[i]+scale;
		}
		pEdit->SetMdlScale(Vector3(p->fScale[0], p->fScale[1], p->fScale[2]));
		pEdit->SetWorldPos(pEdit->GetWorldPos());
		m_mapObjDlg.UpdateList(m_pEngine->GetSelectedObj());
	}
	else if(pEdit && pEdit->GetType()==MapObjEdit::MapDoor)
	{
		tagMapDoor* p = (tagMapDoor*)pEdit->GetMapDoor();
		if(p->bLock)
			return;
		const float* scaleOrg = p->fScale;
		float scale = (float)zDelta/1200;
		for(int i=0; i<3; ++i)
		{
			if(scaleOrg[i]+scale <= 0.1)
				p->fScale[i] = 0.1f;
			else
				p->fScale[i] = scaleOrg[i]+scale;
		}
		pEdit->SetMdlScale(Vector3(p->fScale[0], p->fScale[1], p->fScale[2]));
		pEdit->SetWorldPos(pEdit->GetWorldPos());
		m_mapObjDlg.UpdateList(m_pEngine->GetSelectedObj());
	}
}

void CPlugMapEditor::OnRButtonDown(UINT nFlags, UINT x,UINT y)
{
	//取消建立导航点
	if( m_mapObjDlg.GetIsBuildWayPoint() )
	{
		m_bBuildWayPoint=false;
		m_mapObjDlg.SetIsBuildWayPoint(false);
		m_pEngine->SetCurPosForWayPoint(Vector3(0,0,0),false);
		m_mapObjDlg.m_objTypeList.EnableWindow(true);
	}
	if( m_mapObjDlg.GetIsBuildTrigger() )
	{
		BuildTriggerRButton(x, y);
	}

	if(m_mapObjDlg.GetIsBuildMapRect())
	{
		BuildMapRectRButton(x, y);
	}
	
	//取消一切选择
	m_pEngine->SetCurPickedWayPoint(NULL);
	m_pEngine->SetCurPickedTrigger(NULL);
	m_pEngine->ClearPickTriggerPoint();
	m_pEngine->ClearPickMapAreaPoint();
	m_pEngine->SetSelectedMapRect(NULL);
	m_pEngine->SetSelectedSound(NULL);
	m_pEngine->SetSelectedPointLight(NULL);
	m_pEngine->SetDrawPointLightFlag(false);
	m_pEngine->SetDrawSoundFlag(false);
	m_mapObjDlg.SetIsBuildPointLight(false);
	m_pEngine->SetSelectedPathPoint(NULL);
	m_pEngine->SetSelectedSpawnPoint(NULL);
	m_pEngine->SetSelectedNPCNavStartPoint(NULL);
	m_pEngine->SetSelectedPlayerNavStartPoint(NULL);
	if(m_mapObjDlg.GetPreCreate()==NULL)
		m_pEngine->SetSelectObj(NULL);
	if(m_mapObjDlg.m_objTypeList.GetCurSel() == GMAPOBJ_PointLight)
		m_mapObjDlg.m_objTypeList.EnableWindow(true);
	m_bMoveTriggerPoint = false;
	
}

void CPlugMapEditor::OnRButtonUp(UINT nFlags, UINT x,UINT y)
{
}

//建立导航点
bool CPlugMapEditor::BuildWayPoint(UINT x,UINT y)
{
	//增加新的导航点
	if(m_mapObjDlg.GetIsBuildWayPoint())
	{
		m_bBuildWayPoint=true;
	}
	if(m_bBuildWayPoint)
	{
		Vector3 out;
		m_pEngine->MousePick(x,y,out);

		Vector3 vecObjPos;
		const Heightmap* pHMap = m_pEngine->GetHeightMap()->GetHeightmap();
		if( pHMap == NULL )
			return false;
		Ray mRay;
		m_pCamera->GetMouseRay(x, y, mRay);

		tagMapWayPoint *pWayPoint=new tagMapWayPoint;
		CString szName;
		
		szName = m_mapObjDlg.m_strWayPtName;
		_tcscpy_s(pWayPoint->szName,sizeof(pWayPoint->szName)/sizeof(TCHAR),szName);

		int nIndex = 0;

		pWayPoint->dwObjID = m_mapObjDlg.m_dwGlobalID++;
		if(m_pEngine->GetNavSG()->RayCollideBox(mRay, vecObjPos))
			pWayPoint->vPos = vecObjPos;
		else
			pWayPoint->vPos = out;
		pWayPoint->vRange = Vector3(100, 100, 100);
		pWayPoint->fSuspend = 0.0f;
		m_pEngine->AddWayPoint(pWayPoint,nIndex);
		m_pEngine->SetCurPickedWayPoint(pWayPoint);

		CString szID;
		szID.Format(_T("%x"), pWayPoint->dwObjID);
		m_mapObjDlg.AddObjIDToObjList(szID);
		m_mapObjDlg.UpdatePropListForWayPoint(pWayPoint);
		return true;
	}
	return false;
}

//拾取导航点
bool CPlugMapEditor::PickWayPoint(UINT x,UINT y)
{
	if(m_mapObjDlg.m_objTypeList.GetCurSel()==GMAPOBJ_WAYPOINT)
	{
		if(m_mapObjDlg.GetIsLockOnObject() && m_pEngine->GetCurPickedWayPoint())
		{
			m_bDragedWayPoint = true;
			return true;
		}
		Vector3 pos;
		//m_pEngine->MousePick(x,y,pos);
		m_pEngine->PickWayPoint(x, y);
		tagMapWayPoint *pWayPoint=m_pEngine->GetCurPickedWayPoint();
		if(pWayPoint!=NULL)
		{
			m_mapObjDlg.UpdatePropListForWayPoint(pWayPoint);
			m_bDragedWayPoint=true;
			return true;
		}
	}
	return false;
}

//右键建立地图区域
bool CPlugMapEditor::BuildMapRectRButton(UINT x, UINT y)
{
	int nCurPos = m_pEngine->GetCurMapAreaNum();
	if(m_bBuildMapRect && nCurPos>2)
	{
		/*Vector3 startPos, endPos;
		m_pEngine->GetPosForMapArea(startPos, 0);
		m_pEngine->SetPosForMapArea(startPos, nCurPos+1);*/
		
		tagMapArea mapArea;
		MapObjEdit* pEdit = new MapObjEdit;
		mapArea.dwObjID = m_mapObjDlg.m_dwGlobalID++;
		mapArea.eType = EMA_Null;		
		m_pEngine->GetPosForMapArea(mapArea.region);
		mapArea.box = m_pEngine->CalMapAreaAABBox();
		mapArea.fHeight = 1000;
		mapArea.box.max.y = mapArea.fHeight + mapArea.box.min.y;
		pEdit->Create(mapArea, m_pEngine);
		m_pEngine->SetSelectedMapRect(pEdit);

		CString szID;
		szID.Format(_T("%x"),mapArea.dwObjID);
		m_mapObjDlg.AddObjIDToObjList(szID);
		m_mapObjDlg.UpdatePropListForMapRect(pEdit);
		m_mapObjDlg.m_objTypeList.EnableWindow(true);

		//建立成功了
		m_bBuildMapRect=false;
		m_pEngine->SetCurPosForMapRect(Vector3(0,0,0),false);
		m_mapObjDlg.SetIsBuildMapRect(false);
		m_pEngine->ClearMapAreaPos();

	}
	return false;
}

//拾取地图区域的点
bool CPlugMapEditor::PickMapAreaPoint(UINT x, UINT y)
{
	if(m_mapObjDlg.m_objTypeList.GetCurSel()==GMAPOBJ_MapRect && !m_bBuildMapRect)
	{
		bool bHave = false;
		m_pEngine->PickMapAreaPoint(x, y, bHave);
		if(bHave)
		{
			m_bMoveMapAreaPoint = true;
			MapObjEdit* pMapArea = m_pEngine->GetSelectedMapRect();
			if(pMapArea!=NULL)
			{
				m_mapObjDlg.UpdatePropListForMapRect(pMapArea);
			}
			return true;
		}
	}
	return false;	
}

//--拾取地图区域
bool CPlugMapEditor::PickMapRect(UINT x, UINT y)
{
	if(m_mapObjDlg.m_objTypeList.GetCurSel()==GMAPOBJ_MapRect && !m_bBuildMapRect)
	{
		if(!m_mapObjDlg.IsShowMapRect())			//如果筛选为不可能显示地图区域时,不能拾取
			return false;
		if(m_mapObjDlg.GetIsLockOnObject() && m_pEngine->GetSelectedMapRect())
		{
			m_bDragedMapRect = true;
			return true;
		}
		m_pEngine->PickMapRect(x, y);
		MapObjEdit* pEdit = m_pEngine->GetSelectedMapRect();
		if(pEdit!=NULL)
		{
			m_mapObjDlg.UpdatePropListForMapRect(pEdit);
			m_bDragedMapRect = true;
			return true;
		}
	}
	return false;
}

bool CPlugMapEditor::BuildTriggerRButton(UINT x, UINT y)
{
	int nCurPos = m_pEngine->GetCurTriggerNum();
	if(m_bBuildTrigger && nCurPos>2)
	{
		tagMapTrigger *pTrigger=new tagMapTrigger;
		pTrigger->dwObjID = m_mapObjDlg.m_dwGlobalID++;
		pTrigger->eType = EMT_Null;
		m_pEngine->GetPosForTrigger(pTrigger->region);
		pTrigger->box = m_pEngine->CalTriggerAABBox();
		pTrigger->fHeight = 1000;
		pTrigger->dwQuestSerial = 0;
		pTrigger->box.max.y = pTrigger->fHeight + pTrigger->box.min.y;		
		_tcscpy_s(pTrigger->szMapName,sizeof(pTrigger->szMapName)/sizeof(TCHAR),_T(""));
		_tcscpy_s(pTrigger->szScriptName,sizeof(pTrigger->szScriptName)/sizeof(TCHAR),_T(""));
		_tcscpy_s(pTrigger->szWayPoint,sizeof(pTrigger->szWayPoint)/sizeof(TCHAR),_T(""));
		m_pEngine->AddTrigger(pTrigger);
		m_pEngine->SetCurPickedTrigger(pTrigger);

		CString szID;
		szID.Format(_T("%x"),pTrigger->dwObjID);
		m_mapObjDlg.AddObjIDToObjList(szID);
		m_mapObjDlg.UpdatePropListForTrigger(pTrigger);
		m_mapObjDlg.m_objTypeList.EnableWindow(true);

		//建立成功了
		m_bBuildTrigger=false;
		m_pEngine->SetCurPosForTrigger(Vector3(0,0,0),false);
		m_mapObjDlg.SetIsBuildTrigger(false);
		m_pEngine->ClearTriggerPos();
	}
	return false;
}

//拾取触发器
bool CPlugMapEditor::PickTrigger(UINT x,UINT y)
{
	if(m_mapObjDlg.m_objTypeList.GetCurSel()==GMAPOBJ_Trigger && !m_bBuildTrigger)
	{ 
		if(m_mapObjDlg.GetIsLockOnObject() && m_pEngine->GetCurPickedTrigger())
		{
			m_bDragedTrigger = true;
			return true;
		}
		m_pEngine->PickTrigger(x, y);
		tagMapTrigger *pTrigger=m_pEngine->GetCurPickedTrigger();
		if(pTrigger!=NULL)
		{
			m_mapObjDlg.UpdatePropListForTrigger(pTrigger);
			m_bDragedTrigger=true;
			return true;
		}
	}
	return false;
}

//拾取触发器的点
bool CPlugMapEditor::PickTriggerPoint(UINT x, UINT y)
{
	if(m_mapObjDlg.m_objTypeList.GetCurSel()==GMAPOBJ_Trigger && !m_bBuildTrigger)
	{
		bool bHave = false;
		m_pEngine->PickTriggerPoint(x, y, bHave);
		if(bHave)
		{
			m_bMoveTriggerPoint = true;
			tagMapTrigger *pTrigger=m_pEngine->GetCurPickedTrigger();
			if(pTrigger!=NULL)
			{
				m_mapObjDlg.UpdatePropListForTrigger(pTrigger);
			}
			return true;
		}
	}
	return false;
}

//移动拾取的导航点
bool CPlugMapEditor::MovePickedWayPoint(UINT x,UINT y)
{
    Vector3 vecPos;
    m_pEngine->MousePick(x, y, vecPos);

	Vector3 vecObjPos;
	const Heightmap* pHMap = m_pEngine->GetHeightMap()->GetHeightmap();
	if( pHMap == NULL )
		return false;
	Ray mRay;
	m_pCamera->GetMouseRay(x, y, mRay);

    if(m_bDragedWayPoint)
    {
	   m_pEngine->SetCurPosForPickWayPoint(vecPos);
	   tagMapWayPoint *pWayPoint=m_pEngine->GetCurPickedWayPoint();
	   if(pWayPoint!=NULL && !pWayPoint->bLock)
	   {
		   if( m_pEngine->GetNavSG()->RayCollideBox(mRay, vecObjPos) )
				pWayPoint->vPos = vecObjPos;
		   else
			   pWayPoint->vPos = vecPos;
		   m_mapObjDlg.UpdatePropListForWayPoint(pWayPoint);
		   return true;
	   }		
	}
	return false;
}

//移动拾取的触发器的点
bool CPlugMapEditor::MovePickedTriggerPoint(UINT x, UINT y)
{
	Vector3 vecPos;
	m_pEngine->MousePick(x, y, vecPos);
	Vector3 vecObjPos;
	const Heightmap* pHMap = m_pEngine->GetHeightMap()->GetHeightmap();
	if( pHMap == NULL )
		return false;
	Ray mRay;
	m_pCamera->GetMouseRay(x, y, mRay);
	if(m_bMoveTriggerPoint)
	{
		tagMapTrigger *pTrigger=m_pEngine->GetCurPickedTrigger();
		int num = m_pEngine->GetTriggerNum();
		if(pTrigger!=NULL && !pTrigger->bLock)
		{
			if(m_pEngine->GetNavSG()->RayCollideBox(mRay, vecObjPos))
				pTrigger->region[num] = vecObjPos;
			else 
				pTrigger->region[num] = vecPos;
			m_pEngine->ReCalTriggerAABBox(pTrigger);
			return true;
		}
	}
	return false;
}

//移动拾取的触发器
bool CPlugMapEditor::MovePickedTrigger(UINT x,UINT y)
{
	/*Vector3 vecPos;
	m_pEngine->MousePick(x, y, vecPos);

	if(m_bDragedTrigger)
	{
		m_pEngine->SetCurPosForPickTrigger(vecPos);
		tagMapTrigger *pTrigger=m_pEngine->GetCurPickedTrigger();
		if(pTrigger!=NULL && !pTrigger->bLock)
		{
			POINT p;
			m_pEngine->GetTerrainTileXZ(vecPos,p);

			if(p.x-m_nTriggerRelationTileMin.x>0 && 
				p.y-m_nTriggerRelationTileMin.y>0 &&
				p.x+m_nTriggerRelationTileMax.x<(LONG)m_pEngine->GetHeightMap()->GetHMapWidth() &&
				p.y+m_nTriggerRelationTileMax.y<(LONG)m_pEngine->GetHeightMap()->GetHMapHeight())
			{
				pTrigger->rect.left=p.x-m_nTriggerRelationTileMin.x;
				pTrigger->rect.top=p.y-m_nTriggerRelationTileMin.y;
				pTrigger->rect.right=p.x+m_nTriggerRelationTileMax.x;
				pTrigger->rect.bottom=p.y+m_nTriggerRelationTileMax.y;
			}
			m_mapObjDlg.UpdatePropListForTrigger(pTrigger);
			return true;
		}
	}*/
	return false;
}

//移动拾取的地图区域的点
bool CPlugMapEditor::MovePickedMapAreaPoint(UINT x, UINT y)
{
	Vector3 vecPos;
	m_pEngine->MousePick(x, y, vecPos);
	Vector3 vecObjPos;
	const Heightmap* pHMap = m_pEngine->GetHeightMap()->GetHeightmap();
	if( pHMap == NULL )
		return false;
	Ray mRay;
	m_pCamera->GetMouseRay(x, y, mRay);
	if(m_bMoveMapAreaPoint)
	{
		MapObjEdit *pEdit = m_pEngine->GetSelectedMapRect();
		int num = m_pEngine->GetMapAreaNum();
		tagMapArea *pMapArea = (tagMapArea*)pEdit->GetMapRect();
		if(pMapArea!=NULL && !pMapArea->bLock)
		{
			if(m_pEngine->GetNavSG()->RayCollideBox(mRay, vecObjPos))
				pMapArea->region[num] = vecObjPos;
			else 
				pMapArea->region[num] = vecPos;
			m_pEngine->ReCalMapAreaAABBox(pMapArea);
			return true;
		}
	}
	return false;
}

//--移动地图区域
bool CPlugMapEditor::MovePickMapRect(UINT x, UINT y)
{
	/*Vector3 vecPos;
	m_pEngine->MousePick(x, y, vecPos);

	if(m_bDragedMapRect)
	{
		m_pEngine->SetCurPosForPickMapRect(vecPos);
		MapObjEdit *pEdit = m_pEngine->GetSelectedMapRect();
		if(pEdit!=NULL)
		{	
			tagMapRect* pMapRect = (tagMapRect*)pEdit->GetMapRect();
			if(pMapRect!=NULL && !pMapRect->bLock)
			{	
				POINT p;
				m_pEngine->GetTerrainTileXZ(vecPos,p);

				if(p.x-m_ptMapRectTileMin.x>0 && 
					p.y-m_ptMapRectTileMin.y>0 &&
					p.x+m_ptMapRectTileMax.x<(LONG)m_pEngine->GetHeightMap()->GetHMapWidth() &&
					p.y+m_ptMapRectTileMax.y<(LONG)m_pEngine->GetHeightMap()->GetHMapHeight())
				{
					pMapRect->rect.left=p.x-m_ptMapRectTileMin.x;
					pMapRect->rect.top=p.y-m_ptMapRectTileMin.y;
					pMapRect->rect.right=p.x+m_ptMapRectTileMax.x;
					pMapRect->rect.bottom=p.y+m_ptMapRectTileMax.y;
				}
				m_mapObjDlg.UpdatePropListForMapRect(pEdit);
				return true;
			}
		}
	}*/
	return false;
}

//--建立音效
bool CPlugMapEditor::BuildSound(UINT x,UINT y)
{
	Vector3 vecPos;
	m_pEngine->MousePick(x, y, vecPos);
	if(m_mapObjDlg.GetIsBuildSound())
	{
		tagMapSound* pSound=m_pEngine->GetSelectedSound();
		if(pSound==NULL) return false;
		pSound->fPos[0]=vecPos.x;
		pSound->fPos[1]=vecPos.y;
		pSound->fPos[2]=vecPos.z;
		pSound->fRange = 1000;
		pSound->fVolume = 1;
		pSound->fNoAtteRange = pSound->fRange;
		pSound->dwObjID = m_mapObjDlg.m_dwGlobalID++;
		int nIndex = pSound->dwObjID;
		m_pEngine->AddSound(pSound);
		
        m_mapObjDlg.SetIsBuildSound(false);
        CString szID;
		szID.Format(_T("%x"),nIndex);
		m_mapObjDlg.AddObjIDToObjList(szID);
		m_mapObjDlg.UpdatePropListForSound(pSound);

		m_mapObjDlg.m_objTypeList.EnableWindow(true);
		m_pEngine->SetDrawSoundFlag(false);
		m_mapObjDlg.SetFocus();
		return true;
	}
    return false;
}

//--拾取音效
bool CPlugMapEditor::PickSound(UINT x,UINT y)
{
	 if(m_mapObjDlg.m_objTypeList.GetCurSel()==GMAPOBJ_Sound)
	 {
		if(m_mapObjDlg.GetIsLockOnObject() && m_pEngine->GetSelectedSound())
		{
		   m_bDragedSoundBox = true;
		   return true;
		}
		m_pEngine->PickSoundBox(x,y);
	    if(m_pEngine->GetSelectedSound())
		{
		  m_mapObjDlg.UpdatePropListForSound(m_pEngine->GetSelectedSound());
		  m_bDragedSoundBox=true;
		  return true;
		}
	 }
	 return false;
}

//--移动拾取音效
bool CPlugMapEditor::MovePickedSound(UINT x,UINT y)
{
	Vector3 vecPos;
	m_pEngine->MousePick(x, y, vecPos);
	if(m_bDragedSoundBox)
	{
		tagMapSound* pSound = m_pEngine->GetSelectedSound();
		if(pSound!=NULL && !pSound->bLock)
		{
			m_pEngine->GetSelectedSound()->fPos[0]=vecPos.x;
			m_pEngine->GetSelectedSound()->fPos[2]=vecPos.z;
			m_mapObjDlg.UpdatePropListForSound(m_pEngine->GetSelectedSound());
			return true;
		}
	}
	return false;
}

//--
bool CPlugMapEditor::BuildNPCNavStartPoint(UINT x, UINT y)
{
	Vector3 vecPos;
	m_pEngine->MousePick(x, y, vecPos);

	Vector3 vecObjPos;
	const Heightmap* pHMap = m_pEngine->GetHeightMap()->GetHeightmap();
	if( pHMap == NULL )
		return false;
	Ray mRay;
	m_pCamera->GetMouseRay(x, y, mRay);
	bool bCollide = m_pEngine->GetNavSG()->RayCollideBox(mRay, vecObjPos);

	if(m_mapObjDlg.GetIsBuildNPCNavStartPoint())
	{
		tagNPCNavMapBuildStartPoint* npcNav = new tagNPCNavMapBuildStartPoint;
		npcNav->dwObjID = m_mapObjDlg.m_dwGlobalID++;
		if(bCollide)
			npcNav->vPos = vecObjPos;
		else
			npcNav->vPos = vecPos;

		m_pEngine->AddNPCNavStartPoint(npcNav);
		m_mapObjDlg.SetIsBuildNPCNavStartPoint(false);
		CString szID;
		szID.Format(_T("%x"),npcNav->dwObjID);
		m_mapObjDlg.AddObjIDToObjList(szID);
		m_mapObjDlg.UpdateProListForNPCNav(npcNav);
		m_mapObjDlg.m_objTypeList.EnableWindow(true);
		return true;
	}
	return false;
}
bool CPlugMapEditor::PickNPCNavStartPoint(UINT x, UINT y)
{
	if(m_mapObjDlg.m_objTypeList.GetCurSel()==GMAPOBJ_NPCNavStartPoint)
	{
		if(m_mapObjDlg.GetIsLockOnObject() && m_pEngine->GetSelectedNPCNavStartPoint())
		{
			m_bDragedNPCNav = true;
			return true;
		}
		m_pEngine->PickNPCNavStartPoint(x,y);
		if(m_pEngine->GetSelectedNPCNavStartPoint())
		{
			tagNPCNavMapBuildStartPoint* pEdit = m_pEngine->GetSelectedNPCNavStartPoint();
			m_mapObjDlg.UpdateProListForNPCNav(pEdit);
			m_bDragedNPCNav=true;
			return true;
		}
	}
	return false;
}
bool CPlugMapEditor::MovePickedNPCNavStartPoint(UINT x, UINT y)
{
	Vector3 vecPos;
	m_pEngine->MousePick(x, y, vecPos);
	Vector3 vecObjPos;
	const Heightmap* pHMap = m_pEngine->GetHeightMap()->GetHeightmap();
	if( pHMap == NULL )
		return false;
	Ray mRay;
	m_pCamera->GetMouseRay(x, y, mRay);
	bool bCollide = m_pEngine->GetNavSG()->RayCollideBox(mRay, vecObjPos);
	if(m_bDragedNPCNav)
	{
		tagNPCNavMapBuildStartPoint* pEdit = m_pEngine->GetSelectedNPCNavStartPoint();
		if(pEdit==NULL)
			return false;
	
		if(m_bIsEqualType)
		{
			m_bIsEqualType = false;
			return true;
		}
		if(bCollide)
			pEdit->vPos = vecObjPos;
		else
			pEdit->vPos = vecPos;
		m_mapObjDlg.UpdateProListForNPCNav(pEdit);
		return true;

	}
	return false;
}

//--
bool CPlugMapEditor::BuildPlayerNavStartPoint(UINT x, UINT y)
{
	Vector3 vecPos;
	m_pEngine->MousePick(x, y, vecPos);

	Vector3 vecObjPos;
	const Heightmap* pHMap = m_pEngine->GetHeightMap()->GetHeightmap();
	if( pHMap == NULL )
		return false;
	Ray mRay;
	m_pCamera->GetMouseRay(x, y, mRay);
	bool bCollide = m_pEngine->GetNavSG()->RayCollideBox(mRay, vecObjPos);

	if(m_mapObjDlg.GetIsBuildPlayerNavStartPoint())
	{
		tagPlayerNavMapBuildStartPoint* PlayerNav = new tagPlayerNavMapBuildStartPoint;
		PlayerNav->dwObjID = m_mapObjDlg.m_dwGlobalID++;
		if(bCollide)
			PlayerNav->vPos = vecObjPos;
		else
			PlayerNav->vPos = vecPos;

		m_pEngine->AddPlayerNavStartPoint(PlayerNav);
		m_mapObjDlg.SetIsBuildPlayerNavStartPoint(false);
		CString szID;
		szID.Format(_T("%x"),PlayerNav->dwObjID);
		m_mapObjDlg.AddObjIDToObjList(szID);
		m_mapObjDlg.UpdateProListForPlayerNav(PlayerNav);
		m_mapObjDlg.m_objTypeList.EnableWindow(true);
		return true;
	}
	return false;
}
bool CPlugMapEditor::PickPlayerNavStartPoint(UINT x, UINT y)
{
	if(m_mapObjDlg.m_objTypeList.GetCurSel()==GMAPOBJ_PlayerNavStartPoint)
	{
		if(m_mapObjDlg.GetIsLockOnObject() && m_pEngine->GetSelectedPlayerNavStartPoint())
		{
			m_bDragedPlayerNav = true;
			return true;
		}
		m_pEngine->PickPlayerNavStartPoint(x,y);
		if(m_pEngine->GetSelectedPlayerNavStartPoint())
		{
			tagPlayerNavMapBuildStartPoint* pEdit = m_pEngine->GetSelectedPlayerNavStartPoint();
			m_mapObjDlg.UpdateProListForPlayerNav(pEdit);
			m_bDragedPlayerNav=true;
			return true;
		}
	}
	return false;
}
bool CPlugMapEditor::MovePickedPlayerNavStartPoint(UINT x, UINT y)
{
	Vector3 vecPos;
	m_pEngine->MousePick(x, y, vecPos);
	Vector3 vecObjPos;
	const Heightmap* pHMap = m_pEngine->GetHeightMap()->GetHeightmap();
	if( pHMap == NULL )
		return false;
	Ray mRay;
	m_pCamera->GetMouseRay(x, y, mRay);
	bool bCollide = m_pEngine->GetNavSG()->RayCollideBox(mRay, vecObjPos);
	if(m_bDragedPlayerNav)
	{
		tagPlayerNavMapBuildStartPoint* pEdit = m_pEngine->GetSelectedPlayerNavStartPoint();
		if(pEdit==NULL)
			return false;

		if(m_bIsEqualType)
		{
			m_bIsEqualType = false;
			return true;
		}
		if(bCollide)
			pEdit->vPos = vecObjPos;
		else
			pEdit->vPos = vecPos;
		m_mapObjDlg.UpdateProListForPlayerNav(pEdit);
		return true;

	}
	return false;
}




//建立刷怪点
bool CPlugMapEditor::BuildSpawnPoint(UINT x, UINT y)
{
	Vector3 vecPos;
	m_pEngine->MousePick(x, y, vecPos);

	Vector3 vecObjPos;
	const Heightmap* pHMap = m_pEngine->GetHeightMap()->GetHeightmap();
	if( pHMap == NULL )
		return false;
	Ray mRay;
	m_pCamera->GetMouseRay(x, y, mRay);
	bool bCollide = m_pEngine->GetNavSG()->RayCollideBox(mRay, vecObjPos);

	if(m_mapObjDlg.GetIsBuildSpawnPoint())
	{
		tagMapSpawnPoint SpawnPoint;
		SpawnPoint.dwObjID = m_mapObjDlg.m_dwGlobalID++;
		if(bCollide)
			SpawnPoint.vPos = vecObjPos;
		else
			SpawnPoint.vPos = vecPos;
		SpawnPoint.dwGroupID = 0;
		SpawnPoint.bCollide = false;
		MapObjEdit* pEdit = new MapObjEdit;
		pEdit->Create(SpawnPoint, m_pEngine);
		m_mapObjDlg.SetIsBuildSpawnPoint(false);
		CString szID;
		szID.Format(_T("%x"),SpawnPoint.dwObjID);
		m_mapObjDlg.AddObjIDToObjList(szID);
		m_mapObjDlg.UpdatePropListForSpawnPoint(pEdit);
		m_mapObjDlg.m_objTypeList.EnableWindow(true);
		return true;
	}
	return false;
}

//拾取刷怪点
bool CPlugMapEditor::PickSpawnPoint(UINT x, UINT y)
{
	if(m_mapObjDlg.m_objTypeList.GetCurSel()==GMAPOBJ_SpawnPoint)
	{
		if(m_mapObjDlg.GetIsLockOnObject() && m_pEngine->GetSelectedSpawnPoint())
		{
			m_bDragedSpawnPoint = true;
			return true;
		}
		m_pEngine->PickSpawnPoint(x,y);
		if(m_pEngine->GetSelectedSpawnPoint())
		{
			MapObjEdit* pEdit = m_pEngine->GetSelectedSpawnPoint();
			m_mapObjDlg.UpdatePropListForSpawnPoint(pEdit);
			m_bDragedSpawnPoint=true;
			return true;
		}
	}
	return false;
}

//移动刷怪点
bool CPlugMapEditor::MovePickedSpawnPoint(UINT x, UINT y)
{
	Vector3 vecPos;
	m_pEngine->MousePick(x, y, vecPos);
	Vector3 vecObjPos;
	const Heightmap* pHMap = m_pEngine->GetHeightMap()->GetHeightmap();
	if( pHMap == NULL )
		return false;
	Ray mRay;
	m_pCamera->GetMouseRay(x, y, mRay);
	bool bCollide = m_pEngine->GetNavSG()->RayCollideBox(mRay, vecObjPos);
	if(m_bDragedSpawnPoint)
	{
		MapObjEdit* pEdit = m_pEngine->GetSelectedSpawnPoint();
		if(pEdit==NULL)
			return false;
		tagMapSpawnPoint* pSpawnPoint = (tagMapSpawnPoint*)pEdit->GetSpawnPoint();
		if(pSpawnPoint!=NULL )
		{
			if(m_bIsEqualType)
			{
				m_bIsEqualType = false;
				return true;
			}
			if(bCollide)
				pSpawnPoint->vPos = vecObjPos;
			else
				pSpawnPoint->vPos = vecPos;
			m_mapObjDlg.UpdatePropListForSpawnPoint(pEdit);
			return true;
		}
	}
	return false;
}

//建立寻路导航点
bool CPlugMapEditor::BuildPathPoint(UINT x, UINT y)
{
	Vector3 vecPos;
	m_pEngine->MousePick(x, y, vecPos);
	Vector3 vecObjPos;
	const Heightmap* pHMap = m_pEngine->GetHeightMap()->GetHeightmap();
	if( pHMap == NULL )
		return false;
	Ray mRay;
	m_pCamera->GetMouseRay(x, y, mRay);
	bool bCollide = m_pEngine->GetNavSG()->RayCollideBox(mRay, vecObjPos);
	if(m_mapObjDlg.GetIsBuildPathPoint())
	{
		tagMapPathPoint PathPointObj;
		//PathPointObj.fRange = 2000;
		PathPointObj.dwObjID = m_mapObjDlg.m_dwGlobalID++;
		if(bCollide)
			PathPointObj.vPos = vecObjPos;
		else
			PathPointObj.vPos = vecPos;
		MapObjEdit* pEdit = new MapObjEdit;
		pEdit->Create(PathPointObj, m_pEngine);
		m_mapObjDlg.SetIsBuildPathPoint(false);
		CString szID;
		szID.Format(_T("%x"),PathPointObj.dwObjID);
		m_mapObjDlg.AddObjIDToObjList(szID);
		m_mapObjDlg.UpdatePropListForPathPoint(pEdit);
		m_mapObjDlg.m_objTypeList.EnableWindow(true);
		return true;
	}
	return false;
}

//--拾取寻路导航点
bool CPlugMapEditor::PickPathPoint(UINT x, UINT y)
{
	if(m_mapObjDlg.m_objTypeList.GetCurSel()==GMAPOBJ_PathPoint)
	{
		if(m_mapObjDlg.GetIsLockOnObject() && m_pEngine->GetSelectedPathPoint())
		{
			m_bDragedPathPoint = true;
			return true;
		}
		m_pEngine->PickPathPoint(x,y);
		if(m_pEngine->GetSelectedPathPoint())
		{
			MapObjEdit* pEdit = m_pEngine->GetSelectedPathPoint();
			m_mapObjDlg.UpdatePropListForPathPoint(pEdit);
			m_bDragedPathPoint=true;
			return true;
		}
	}
	return false;
}

//移动拾取的寻路导航点
bool CPlugMapEditor::MovePickedPathPoint(UINT x, UINT y)
{
	Vector3 vecPos;
	m_pEngine->MousePick(x, y, vecPos);
	Vector3 vecObjPos;
	const Heightmap* pHMap = m_pEngine->GetHeightMap()->GetHeightmap();
	if( pHMap == NULL )
		return false;
	Ray mRay;
	m_pCamera->GetMouseRay(x, y, mRay);
	bool bCollide = m_pEngine->GetNavSG()->RayCollideBox(mRay, vecObjPos);
	if(m_bDragedPathPoint)
	{
		MapObjEdit* pEdit = m_pEngine->GetSelectedPathPoint();
		if(pEdit==NULL)
			return false;
		tagMapPathPoint* pPathPoint = (tagMapPathPoint*)pEdit->GetPathPoint();
		if(pPathPoint!=NULL && !pPathPoint->bLock)
		{
			if(m_bIsEqualType)
			{
				m_bIsEqualType = false;
				return true;
			}
			if(bCollide)
				pPathPoint->vPos = vecObjPos;
			else
				pPathPoint->vPos = vecPos;
			m_mapObjDlg.UpdatePropListForPathPoint(pEdit);
			return true;
		}
	}
	return false;
}

//--建立点光源
bool CPlugMapEditor::BuildPointLight(UINT x, UINT y)
{
	Vector3 vecPos;
	m_pEngine->MousePick(x, y, vecPos);
	if(m_mapObjDlg.GetIsBuildPointLight())
	{
		tagMapPointLight PointLgtObj;
		PointLgtObj.dwID = m_mapObjDlg.m_dwGlobalID++;
		for(int i=0; i<4; i++)
		{
			PointLgtObj.ambient[i] = 0.0f;
			PointLgtObj.diffuse[i] = 1.0f;
			PointLgtObj.specular[i] = 1.0f;
		}
		PointLgtObj.attenuation[1] = 0.002f;
		PointLgtObj.range = 2000.0f;
		PointLgtObj.fPos[0]=vecPos.x;
		PointLgtObj.fPos[1]=vecPos.y;
		PointLgtObj.fPos[2]=vecPos.z;
		PointLgtObj.bDynamicPL = true;
		PointLgtObj.fIncModulus = 1.0f;
		PointLgtObj.fnoBlockRange = 0.0f;

		MapObjEdit* pEdit = new MapObjEdit;
		pEdit->Create(PointLgtObj, m_pEngine);

		m_mapObjDlg.SetIsBuildPointLight(false);
		CString szID;
		szID.Format(_T("%x"),PointLgtObj.dwID);
		m_mapObjDlg.AddObjIDToObjList(szID);
		m_mapObjDlg.UpdatePropListForPointLight(pEdit);

		m_mapObjDlg.m_objTypeList.EnableWindow(true);
		m_pEngine->SetDrawPointLightFlag(false);
		return true;
	}
	else if( m_mapObjDlg.GetCopyBuildPointLgt() )
	{
		
		MapObjEdit* pEdit = m_pEngine->GetSelectedPointLight();
		if(pEdit==NULL)
		{
			AfxMessageBox(_T("请选择点光源!"));
			m_mapObjDlg.SetCopyBuildPointLgt(false);
			m_mapObjDlg.m_objTypeList.EnableWindow(true);
			m_pEngine->SetDrawPointLightFlag(false);
			m_mapObjDlg.SetFocus();
			return false;
		}
		tagMapPointLight* pOldPointLight = (tagMapPointLight*)pEdit->GetPointLgtObj();

		tagMapPointLight PointLgtObj;
		PointLgtObj.dwID = m_mapObjDlg.m_dwGlobalID++;;
		for(int i=0; i<4; i++)
		{
			PointLgtObj.ambient[i] = pOldPointLight->ambient[i];
			PointLgtObj.diffuse[i] = pOldPointLight->diffuse[i];
			PointLgtObj.specular[i] = pOldPointLight->specular[i];
		}

		PointLgtObj.range = pOldPointLight->range;
		PointLgtObj.bFlag = pOldPointLight->bFlag;
		PointLgtObj.bLock = pOldPointLight->bLock;
		PointLgtObj.bDynamicPL = pOldPointLight->bDynamicPL;
		PointLgtObj.fIncModulus = pOldPointLight->fIncModulus;
		PointLgtObj.fnoBlockRange = pOldPointLight->fnoBlockRange;
		for(int i=0; i<3; ++i)
		{
			PointLgtObj.attenuation[i] = pOldPointLight->attenuation[i];
		}
		PointLgtObj.fPos[0]=vecPos.x;
		PointLgtObj.fPos[1]=pOldPointLight->fPos[1];
		PointLgtObj.fPos[2]=vecPos.z;

		MapObjEdit* pNewEdit = new MapObjEdit;
		pNewEdit->Create(PointLgtObj, m_pEngine);

		m_mapObjDlg.SetCopyBuildPointLgt(false);
		CString szID;
		szID.Format(_T("%x"),PointLgtObj.dwID);
		m_mapObjDlg.AddObjIDToObjList(szID);
		m_mapObjDlg.UpdatePropListForPointLight(pNewEdit);

		m_mapObjDlg.m_objTypeList.EnableWindow(true);
		m_pEngine->SetDrawPointLightFlag(false);
		return true;
	}
	return false;
}
//--拾取点光源
bool CPlugMapEditor::PickPointLight(UINT x, UINT y)
{
	if(m_mapObjDlg.m_objTypeList.GetCurSel()==GMAPOBJ_PointLight)
	{
		if(m_mapObjDlg.GetIsLockOnObject() && m_pEngine->GetSelectedPointLight())
		{
			m_bDragedPointlight = true;
			return true;
		}
		m_pEngine->PickPointLight(x,y);
		if(m_pEngine->GetSelectedPointLight())
		{
			MapObjEdit* pEdit = m_pEngine->GetSelectedPointLight();
			m_mapObjDlg.UpdatePropListForPointLight(pEdit);
			m_bDragedPointlight=true;
			return true;
		}
	}
	return false;
}
//--移动拾取的点光源
bool CPlugMapEditor::MovePickedPointLight(UINT x, UINT y)
{
	Vector3 vecPos;
	m_pEngine->MousePick(x, y, vecPos);
	if(m_bDragedPointlight)
	{
		MapObjEdit* pEdit = m_pEngine->GetSelectedPointLight();
		if(pEdit==NULL)
			return false;
		tagMapPointLight* pPointLight = (tagMapPointLight*)pEdit->GetPointLgtObj();
		if(pPointLight!=NULL && !pPointLight->bLock)
		{
			if(m_bIsEqualType)
			{
				m_bIsEqualType = false;
				return true;
			}
			pPointLight->fPos[0]=vecPos.x;
			pPointLight->fPos[1]=vecPos.y;
			pPointLight->fPos[2]=vecPos.z;
			pEdit->LoadLightMdl();
			return true;
		}
	}
	return false;
}

//--设置拾取显示标识
void CPlugMapEditor::SetPickFlag()
{
	m_pEngine->SetDrawPickFlagForWayPoint(false);
	m_pEngine->SetDrawPickFlagForTrigger(false);
	m_pEngine->SetDrawPointLightFlag(false);			//设置点光源绘制标识
	m_pEngine->SetDrawPickFlagForMapRect(false);
	m_pEngine->SetDrawPathPoint(false);
	m_pEngine->SetDrawSpawnPoint(false);
	m_pEngine->SetDrawNPCNavStartPoint(false);
	m_pEngine->SetDrawPlayerNavStartPoint(false);
	m_pEngine->SetDrawPickFlagForMapRect(false);
	m_bPickObject=false;
	m_bPickChopField=false;
	m_pEngine->ClearTriggerNum();
	m_pEngine->ClearMapAreaNum();

	m_pEngine->SetCurPickedWayPoint(NULL);
	m_pEngine->SetCurPickedTrigger(NULL);
	m_pEngine->SetSelectObj(NULL);
	m_pEngine->SetSelectedSound(NULL);
	m_pEngine->SetSelectedPointLight(NULL);
	m_pEngine->SetSelectedSound(NULL);
	m_pEngine->SetSelectedMapRect(NULL);
	m_pEngine->SetSelectedPathPoint(NULL);
	m_pEngine->SetSelectedSpawnPoint(NULL);
	m_pEngine->SetSelectedNPCNavStartPoint(NULL);
	m_pEngine->SetSelectedPlayerNavStartPoint(NULL);

	if(m_mapObjDlg.m_objTypeList.GetCurSel()==GMAPOBJ_WAYPOINT)
	{
		m_pEngine->SetDrawPickFlagForWayPoint(true);
	}
	else if(m_mapObjDlg.m_objTypeList.GetCurSel()==GMAPOBJ_Trigger)
	{
		m_pEngine->SetDrawPickFlagForTrigger(true);
	}
	else if(m_mapObjDlg.m_objTypeList.GetCurSel()==GMAPOBJ_MapRect)
	{
		m_pEngine->SetDrawPickFlagForMapRect(true);
	}
	else if(m_mapObjDlg.m_objTypeList.GetCurSel()==GMAPOBJ_PathPoint)
	{
		m_pEngine->SetDrawPathPoint(true);
	}
	else if(m_mapObjDlg.m_objTypeList.GetCurSel()==GMAPOBJ_SpawnPoint)
	{
		m_pEngine->SetDrawSpawnPoint(true);
	}
	else if(m_mapObjDlg.m_objTypeList.GetCurSel()==GMAPOBJ_NPCNavStartPoint)
	{
		m_pEngine->SetDrawNPCNavStartPoint(true);
	}
	else if(m_mapObjDlg.m_objTypeList.GetCurSel()==GMAPOBJ_PlayerNavStartPoint)
	{
		m_pEngine->SetDrawPlayerNavStartPoint(true);
	}
	else if(m_mapObjDlg.m_objTypeList.GetCurSel()==GMAPOBJ_Npc 
			|| m_mapObjDlg.m_objTypeList.GetCurSel()==GMAPOBJ_StaticObj  
			|| m_mapObjDlg.m_objTypeList.GetCurSel()==GMAPOBJ_DynamicBlock
			||  m_mapObjDlg.m_objTypeList.GetCurSel()==GMAPOBJ_MapDoor
			||  m_mapObjDlg.m_objTypeList.GetCurSel()==GMAPOBJ_MapCarrier
			|| m_mapObjDlg.m_objTypeList.GetCurSel()==GMAPOBJ_MapTriggerEffect)
	{
		m_bPickObject=true;
		m_MapCarrier = NULL;
	}
}

void CPlugMapEditor::AddMazeToSG(NullSceneGraph *pSG,const TCHAR* szMazeFile)
{
	if(szMazeFile==NULL)
		return;
	
	if(_taccess(szMazeFile,4)==-1)//file not exsit(not readable)
	{
		TRACE(_T("Maze file: %s not found.\r\n"),szMazeFile);
		return;
	}

	FILE* fp = NULL;
	if(_tfopen_s(&fp,szMazeFile, _T("rb"))!=0)
	{
		TRACE(_T("Maze file: %s open failed.\r\n"),szMazeFile);
		return;
	}
	
	/*tagMazeHeader header;
	fread(&header, sizeof(header), 1, fp);

	fseek(fp, header.dwMazeWallObjOffset, SEEK_SET);
	for(int i  = 0; i < header.nNumMazeWallObj; i++)
	{
		tagMazeWallObj MazeWallObj;
		fread(&MazeWallObj, sizeof(tagMazeWallObj), 1, fp);

		EntityNode *pNode=new EntityNode;
		pNode->LoadRes(MazeWallObj.szMdlPath);
		pSG->AttachStaticNode(pNode);

		Transform trans;
		trans.Scale(MazeWallObj.fScale[0],MazeWallObj.fScale[1],MazeWallObj.fScale[2]);
		trans.Rotate(MazeWallObj.fRotate,0,0);
		Vector3 pos(MazeWallObj.fPos[0],MazeWallObj.fPos[1],MazeWallObj.fPos[2]);
		trans.Translate(pos);

		pNode->SetWorldMat(trans.GetMatrix());
	}*/

	fclose(fp);
}


void _FormatTime(DWORD ms,DWORD& hour,DWORD& minute,DWORD& second)
{
	second=ms/1000;
	hour=second/3600;
	second-=hour*3600;

	minute=second/60;
	second-=minute*60;
}


bool CPlugMapEditor::Build(const TCHAR* szMapName,const TCHAR* szSaveBasePath,DWORD dwItems,tagBuildOptions& options,IUICallBack* pUI)
{
	ResMgr::Inst()->EnableIOAsync(false);
	bool bRet=_Build(szMapName,szSaveBasePath,dwItems,options,pUI);
	ResMgr::Inst()->EnableIOAsync(true);
	return bRet;
}

bool CPlugMapEditor::_Build(const TCHAR* szMapName,const TCHAR* szSaveBasePath,DWORD dwItems,tagBuildOptions& options,IUICallBack* pUI)
{
	if(m_pEngine==NULL)
		return false;

	if(dwItems&EBI_MapObj)
	{
		pUI->SetProgress(0,1);

		TCHAR szMapPath[512];
		bool ok=false;

		//--生成HugeQuadTreeSG
		GameMap gMap;
		_stprintf_s(szMapPath,512,_T("%s\\%s\\%s.mb"),szSaveBasePath,szMapName,szMapName);
		gMap.LoadFromFile(Kernel::Inst()->DefaultFS(),szMapPath);
		const vector<tagStaticMapObj>& staticObjs=gMap.GetStaticMapObjs();
		const vector<tagMapPointLight>& pointLgts=gMap.GetPointLight();
		
		TCHAR szTrnPath[512];
		_stprintf_s(szTrnPath,512,_T("%s\\%s\\Trn\\"),szSaveBasePath,szMapName);

		TCHAR szTrnFile[512];
		_stprintf_s(szTrnFile,512,_T("%sterrain.trn"),szTrnPath);

		NullSceneGraph tmpSG;
		TCHAR szFileName[512];
		TCHAR szFileTile[512];
		TCHAR szLightMap[512];
		TCHAR szRenderGroup[512];
		int i=0;
		while(true)
		{
			_stprintf_s(szFileName,512,_T("%sTrn_%03d.patch"),szTrnPath,i);
			if(_taccess(szFileName,4)==-1)//file not exist(not readable)
				break;

			_stprintf_s(szLightMap,512,_T("%s\\%s\\lightmap\\trn_%03d.tga"),
				szSaveBasePath,szMapName,i);

			_stprintf_s(szRenderGroup,512,_T("%sTrn_%03d.rendergroups"),szTrnPath,i);

			tmpSG.LoadTerrainPatch(szTrnFile,szFileName,szRenderGroup,szLightMap);
			i++;
		}

		TRACE(_T("MapBuild:terrain load ok,i=%d\r\n"),i);

		//加载草节点
		int numPatch=i;
		for(i=0;i<numPatch;i++)
		{
			for (int j=0;j<100;j++)
			{
				_stprintf_s(szFileName,512,_T("%sGrass_%03d_%02d.deco"),szTrnPath,i,j);
				if (_taccess(szFileName,4)==-1)
					continue;
				tmpSG.LoadGrassPatch(szFileName);
			}
		}

		//加载河流
		TCHAR szRiverPath[512];
		TCHAR szRiverFile[512];
		_stprintf(szRiverPath,_T("%s\\%s\\river\\"),szSaveBasePath,szMapName);

		for(i=0;i<TRiverEditor::MAX_RIVER;i++)
		{
			int j=0;
			for(j=0;j<numPatch;j++)
			{
				_stprintf(szFileName,_T("%sRiver_%02d_%03d.rpatch"),szRiverPath,i,j);
				if(_taccess(szFileName,4)==-1)
					continue;
				_stprintf(szRiverFile,_T("%sRiver_%02d.river"),szRiverPath,i);

				tmpSG.LoadRiver(szRiverFile,szFileName);
			}
		}

		vector<EntityNode*> staticNodes;
		vector<SGEffectNode*> staticEffectNode;
		bool bHaveErrBox = false;
		for(i=0;i<(int)staticObjs.size();i++)
		{
			const tagStaticMapObj& obj=staticObjs[i];

			CString szMdlName=CString(obj.szMdlPath);

			//判别文件类型
			int nlength=szMdlName.GetLength();
			int nIndex=szMdlName.ReverseFind(_T('.'));
			CString szfiletype=szMdlName.Right(nlength-nIndex-1);

			if(szfiletype!=_T("sfx") && szfiletype!=_T("SFX"))
			{
				//使用EntityNode
				EntityNode *pNode=new EntityNode;
				pNode->LoadRes(obj.szMdlPath);
				pNode->SetMtlDiffuseFactor(obj.fMtlDiffuseFactor);
				tmpSG.AttachStaticNode(pNode,(ESceneNodeViewLevel)obj.byViewLevel);
				staticNodes.push_back(pNode);

				Transform trans;
				trans.Scale(obj.fScale[0],obj.fScale[1],obj.fScale[2]);
				trans.Rotate(obj.fRotate[0],obj.fRotate[1],obj.fRotate[2]);
				Vector3 pos(obj.fPos[0],obj.fPos[1],obj.fPos[2]);
				trans.Translate(pos);

				pNode->SetWorldMat(trans.GetMatrix());

				AABBox boxNode;
				pNode->GetBox(boxNode);
				if( !MathConst::BigBox.Contain(boxNode) ||
					boxNode.min.x > boxNode.max.x ||
					boxNode.min.y > boxNode.max.y ||
					boxNode.min.z > boxNode.max.z )
				{
					LOG( _T("box error![%x] --- %s --- min = (%f,%f,%f), max = (%f,%f,%f)\r\n"),
						 obj.dwMapID,
						 obj.szMdlPath,
						 boxNode.min.x,boxNode.min.y,boxNode.min.z,
						 boxNode.max.x,boxNode.max.y,boxNode.max.z );
					bHaveErrBox = true;
				}

				//生成Lightmap文件名
				if(obj.bLightMap)
				{
					_tsplitpath_s(obj.szMdlPath,NULL,0,NULL,0,szFileTile,sizeof(szFileTile)/sizeof(TCHAR),NULL,0);
					_tcslwr_s(szFileTile,sizeof(szFileTile)/sizeof(TCHAR));
					_stprintf_s(szLightMap,sizeof(szLightMap)/sizeof(TCHAR),_T("%s\\%s\\lightmap\\%s_%d.tga"),
						szSaveBasePath,szMapName,szFileTile,obj.dwMapID);
					pNode->SetLightmap(szLightMap);
				}
			}
			else
			{
				//使用SGEffectNode
				SGEffectNode* pSGEffectNode=new SGEffectNode;
				FArchive ar;
				ar.OpenForRead(Kernel::Inst()->DefaultFS(),obj.szMdlPath);
				pSGEffectNode->Deserialize(ar);
				ar.Close();
				tmpSG.AttachStaticNode(pSGEffectNode,(ESceneNodeViewLevel)obj.byViewLevel);
				staticEffectNode.push_back(pSGEffectNode);	

				Transform trans;
				trans.Scale(obj.fScale[0],obj.fScale[1],obj.fScale[2]);
				trans.Rotate(obj.fRotate[0],obj.fRotate[1],obj.fRotate[2]);
				Vector3 pos(obj.fPos[0],obj.fPos[1],obj.fPos[2]);
				trans.Translate(pos);

				pSGEffectNode->SetWorldMat(trans.GetMatrix());
			}
		}
		if(bHaveErrBox)
		{
			pUI->AddMsg(_T("（有错误的包围盒，请查看日志）"));
			return false;
		}

		//点光源
		tmpSG.OpenDynamicPL();
		tmpSG.OpenStaticPL();
		for(i=0;i<(int)pointLgts.size();i++)
		{
			const tagMapPointLight& lgt=pointLgts[i];

			SGLightNode* pNode=new SGLightNode;

			tagLight light;
			light.type=Light_Point;
			light.pos.x=lgt.fPos[0];
			light.pos.y=lgt.fPos[1];
			light.pos.z=lgt.fPos[2];
			light.range=lgt.range;
			light.diffuse.A = lgt.diffuse[3];
			light.diffuse.R = lgt.diffuse[0]*lgt.fIncModulus;
			light.diffuse.G = lgt.diffuse[1]*lgt.fIncModulus;
			light.diffuse.B = lgt.diffuse[2]*lgt.fIncModulus;
			light.ambient.A = lgt.ambient[3];
			light.ambient.R = lgt.ambient[0];
			light.ambient.G = lgt.ambient[1];
			light.ambient.B = lgt.ambient[2];
			light.specular.A = lgt.specular[3];
			light.specular.R = lgt.specular[0];
			light.specular.G = lgt.specular[1];
			light.specular.B = lgt.specular[2];
			light.attenuation.x=lgt.attenuation[0];
			light.attenuation.y=lgt.attenuation[1];
			light.attenuation.z=lgt.attenuation[2];
			pNode->SetLightModulus(lgt.fIncModulus);
			pNode->SetLight(&light);
			pNode->SetDynamic(lgt.bDynamicPL);
			pNode->SetNoBlockRange(lgt.fnoBlockRange);

			tmpSG.AttachStaticNode(pNode);
		}

		//加载迷宫墙壁
		_stprintf_s(szMapPath,sizeof(szMapPath)/sizeof(TCHAR),_T("%s\\%s\\%s.maze"),szSaveBasePath,szMapName,szMapName);
		AddMazeToSG(&tmpSG,szMapPath);

		//存盘
		ok=tmpSG.SaveAsThreeQuadTreeData(szSaveBasePath,szMapName);
		if(!ok)
			return false;

		pUI->SetProgress(1,1);
	}

	if(dwItems&EBI_LightMap)
	{
		DWORD dwTime = 0;

		if(options.blurTimes<=0)
			options.bBlur=false;
		if(options.radiosityTimes<=0)
			options.bRadiosity=false;

		TCHAR szLightMapPath[512];
		_stprintf_s(szLightMapPath,512,_T("%s\\%s\\lightmap"),szSaveBasePath,szMapName);

		CreateDirectory(szLightMapPath,NULL);

		TCHAR szSGPath[512];
		_stprintf_s(szSGPath,512,_T("%s\\%s\\%s.fsg"),szSaveBasePath,szMapName,szMapName);

		//--清空lightmap目录
		if(options.bLightMapping)
		{
			pUI->AddMsg(_T("\t清空lightmap文件夹..."));
			tstring strLightMapPath = szLightMapPath;
			strLightMapPath += _T("\\");
			if(DeleteAllFiles(strLightMapPath.c_str()))
				pUI->AddMsg(_T("ok\r\n"));
			else
			{
				pUI->AddMsg(_T("失败\r\n"));
				return false;
			}
		}

		//--生成LightMap临时文件
		dwTime = timeGetTime();
		LightMapTempGen temp(pUI);
		if(options.bLightMapping)
		{
			pUI->AddMsg(_T("\t生成临时文件..."));
			if(temp.Start(szMapName))
			{
				dwTime = timeGetTime() - dwTime;
				DWORD dwHour=0,dwMinute=0,dwSecond=0;
				_FormatTime(dwTime,dwHour,dwMinute,dwSecond);
				pUI->AddMsg(_T("ok, 耗时%d小时%d分%d秒\r\n"), dwHour,dwMinute,dwSecond);
			}
			else
			{
				pUI->AddMsg(_T("失败\r\n"));
				return false;
			}
		}

		//--生成AO
		dwTime = timeGetTime();
		AmbientOcclusion ao(pUI);
		if(options.bAmbientOcclusion)
		{
			pUI->AddMsg(_T("\t计算天光..."));
			if(ao.Start(szMapName,options.bSmallAO))
			{
				dwTime = timeGetTime() - dwTime;
				DWORD dwHour=0,dwMinute=0,dwSecond=0;
				_FormatTime(dwTime,dwHour,dwMinute,dwSecond);
				pUI->AddMsg(_T("ok, 耗时%d小时%d分%d秒\r\n"), dwHour,dwMinute,dwSecond);
			}
			else
			{
				pUI->AddMsg(_T("失败\r\n"));
				return false;
			}
		}

		//--计算光照和阴影
		if(options.bLightMapping)
		{
			dwTime = timeGetTime();
			pUI->AddMsg(_T("\t计算光照和阴影..."));

			tagLight sunLgt;
			m_pEngine->GetSunLight(sunLgt);
			float fModulus=m_pEngine->GetSunModulus();
			sunLgt.diffuse.R*=fModulus;
			sunLgt.diffuse.G*=fModulus;
			sunLgt.diffuse.B*=fModulus;
			sunLgt.type = Light_Direct;

			Color4f skyCol;
			m_pEngine->GetSkyCol(skyCol);

			LightMapping light(pUI);
			if(light.Start(szMapName,&sunLgt,skyCol,options.bAmbientOcclusion))
			{
				dwTime = timeGetTime() - dwTime;
				DWORD dwHour=0,dwMinute=0,dwSecond=0;
				_FormatTime(dwTime,dwHour,dwMinute,dwSecond);
				pUI->AddMsg(_T("ok, 耗时%d小时%d分%d秒\r\n"), dwHour,dwMinute,dwSecond);
			}
			else
			{
				pUI->AddMsg(_T("失败\r\n"));
				return false;
			}
		}

		//--删除AOMap临时文件
		if( options.bAmbientOcclusion )
		{
			pUI->AddMsg(_T("\t删除AO临时文件..."));
			ao.DeleteTempFiles(szLightMapPath);
			pUI->AddMsg(_T("ok\r\n"));
		}

		//--计算辐射度
		if(options.bRadiosity)
		{
			pUI->AddMsg(_T("\t光能传递模拟...\r\n"));
			Radiosity radiosity(pUI);
			for(int i=0;i<options.radiosityTimes;i++)
			{
				pUI->AddMsg(_T("\t\t第%d次..."),i+1);
				dwTime = timeGetTime();
				if(radiosity.Start(szMapName))
				{
					dwTime = timeGetTime() - dwTime;
					DWORD dwHour=0,dwMinute=0,dwSecond=0;
					_FormatTime(dwTime,dwHour,dwMinute,dwSecond);
					pUI->AddMsg(_T("ok, 耗时%d小时%d分%d秒\r\n"), dwHour,dwMinute,dwSecond);
				}
				else
				{
					pUI->AddMsg(_T("失败\r\n"));
					return false;
				}
			}
		}

		//--模糊处理
		if(options.bBlur)
		{
			dwTime = timeGetTime();
			pUI->AddMsg(_T("\t模糊处理..."));
			LightMapBlur blur(pUI);
			if(blur.Start(szLightMapPath,options.blurTimes,m_pEngine->GetHeightMap()->GetHMapWidth(),m_pEngine->GetHeightMap()->GetHMapHeight()))
			{
				dwTime = timeGetTime() - dwTime;
				DWORD dwHour=0,dwMinute=0,dwSecond=0;
				_FormatTime(dwTime,dwHour,dwMinute,dwSecond);
				pUI->AddMsg(_T("ok, 耗时%d小时%d分%d秒\r\n"), dwHour,dwMinute,dwSecond);
			}
			else
			{
				pUI->AddMsg(_T("失败\r\n"));
				return false;
			}
		}

		//--删除LightMap临时文件
		if(options.bLightMapping)
		{
			pUI->AddMsg(_T("\t删除临时文件..."));
			temp.DeleteTempFiles(szLightMapPath);
			pUI->AddMsg(_T("ok\r\n"));
		}
	}

	if(dwItems&EBI_MiniMap)
	{
		HugeQuadTreeSG sg;
		sg.LoadFromFile(Kernel::Inst()->DefaultFS(),szMapName);
		AABBox boxSG = sg.GetBox();
		Vector3 ViewZone = boxSG.max - boxSG.min;
		sg.SetViewZoneSize(HugeQuadTreeSG::EVZT_LitNode,ViewZone);
		sg.SetViewZoneSize(HugeQuadTreeSG::EVZT_MidNode,ViewZone);
		sg.SetViewZoneSize(HugeQuadTreeSG::EVZT_BigNode,ViewZone);
		sg.SetViewZoneSize(HugeQuadTreeSG::EVZT_SimpleTerrain,ViewZone);

		CameraBase camera;
		camera.SetOrtho( boxSG.max.x - boxSG.min.x, boxSG.max.z - boxSG.min.z, 1.0f, 2000000.0f );
		Vector3 vLookAt = boxSG.GetCenter();
		Vector3 vFrom = vLookAt;
		vFrom.y = boxSG.max.y * 2.0f;
		camera.SetLookAt( vFrom, vLookAt, MathConst::AxisZ );
		sg.BindCamera( &camera );

		sg.InvalidViewZone();
		sg.SyncInit();

		AABBox boxTerrain;
		boxTerrain.max=Vector3(-FLOAT_MAX,-FLOAT_MAX,-FLOAT_MAX);
		boxTerrain.min=Vector3(FLOAT_MAX,FLOAT_MAX,FLOAT_MAX);
		AABBox nodeBox;
		vector<SceneNode*> vecStaticNodes;
		sg.GetStaticNodes( vecStaticNodes );
		for(size_t i=0;i<vecStaticNodes.size();i++)
		{
			const SceneNode* pNode=vecStaticNodes[i];
			if( pNode->IsStaticClass( _T("TerrainPatch") ) )
			{
				pNode->GetBox(nodeBox);
				boxTerrain.Merge(nodeBox);
			}
		}

		// 使用地块的包围盒（x,z），使用SG的包围盒（y）生成包围盒
		// 拍整个地图的缩略图（直接使用SG的会有问题）
		AABBox boxMap = boxTerrain;
		boxMap.min.y = boxSG.min.y;
		boxMap.max.y = boxSG.max.y;

		TCHAR szFileName[512];
		_stprintf_s(szFileName,512,_T("%s\\%s.tga"),szSaveBasePath,szMapName);
		BuildMiniMap( boxMap, options.nMiniMapSize, szFileName, &sg );

		GameMap gMap;
		_stprintf_s(szFileName,512,_T("%s\\%s\\%s.mb"),szSaveBasePath,szMapName,szMapName);
		gMap.LoadFromFile(Kernel::Inst()->DefaultFS(),szFileName,GameMap::ELO_MapRect);
		const vector<tagMapArea>& vecMapRect=gMap.GetMapRect();
		for( size_t i = 0; i < vecMapRect.size(); i++ )
		{
			const tagMapArea& mapRect = vecMapRect[i];
			if( 0 == mapRect.dwMiniMapSize )
				continue;
			_stprintf_s(szFileName,512,_T("%s\\%s_%x.tga"),szSaveBasePath,szMapName,mapRect.dwObjID);
			Vector3 vCenter = mapRect.box.GetCenter();
			float fHalfSize = max( mapRect.box.max.x - mapRect.box.min.x, mapRect.box.max.z - mapRect.box.min.z ) * 0.5f;
			AABBox boxRect = mapRect.box;
			boxRect.min = Vector3( vCenter.x - fHalfSize, mapRect.box.min.y, vCenter.z - fHalfSize );
			boxRect.max = Vector3( vCenter.x + fHalfSize, mapRect.box.max.y, vCenter.z + fHalfSize );

			BuildMiniMap( boxRect, mapRect.dwMiniMapSize, szFileName, &sg );
		}
	}
	return true;
}

bool CPlugMapEditor::BuildMiniMap( const AABBox& box, const DWORD nRTSize, TCHAR* szFileName, HugeQuadTreeSG* pSG )
{
	IDevice* pDev = Device();
	IRenderTexture* pRT = pDev->NewRenderTexture();
	pRT->Create( nRTSize, nRTSize, EPF_X8R8G8B8, true, false, 1 );

	CameraBase* pCamera = pSG->GetCamera();
	pCamera->SetOrtho( box.max.x - box.min.x, box.max.z - box.min.z, 1.0f, box.max.y - box.min.y );
	Vector3 vLookAt = box.GetCenter();
	Vector3 vFrom = vLookAt;
	vLookAt.y = box.min.y;
	vFrom.y = box.max.y + 1.0f;
	pCamera->SetLookAt( vFrom, vLookAt, MathConst::AxisZ );

	pRT->Begin();
	{
		pDev->Clear( true, true, false, 0xffff0000, 1.0f, 0 );
		pSG->Render( Kernel::Inst()->GetDeltaTime() );
	}
	pRT->End();

	pRT->WriteToFile( szFileName );
	SAFE_DELETE( pRT );
	return true;
}