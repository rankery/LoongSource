#include "StdAfx.h"
#include ".\gamemapeditor.h"
//#include "GNavMap.h"
#include "MapObjEdit.h"
#include"stdio.h"
#include "..\..\Cool3D\Util\CSVFile.h"
#undef VFS_READ


namespace WorldBase
{
	GameMapEditor::GameMapEditor(void)
	{
		m_pTerrain=NULL;
		m_pCamera=NULL;
		m_pSelectedObj=NULL;
		m_pSelectWayPoint=NULL;
		m_pSelectTrigger=NULL;
		m_bCurDrawTrigger=false;
		m_bDrawMapRect = false;
		m_bPickTrigger=false;
		m_bPickMapRect=false;
		m_bCurDrawWayPoint=false;
		m_pSelectSound=NULL;
		m_bPickWayPoint=false;
		m_bPickPointLight=false;
		m_bPickSound=false;
		m_bDrawPathPoint = false;
		m_pSGSkyBoxNode = NULL;
		m_pNavSG=NULL;
		m_nCurTriggerNum = 0;
		m_nCurMapAreaNum = 0;
		m_nTriggerNum	= 0;
		m_bPickTriggerPoint = false;
		m_nMapAreaNum	= 0;
		m_bPickMapAreaPoint = false;
		m_pSelectNPCNavStartPoint=NULL;
		m_pSelectPlayerNavStartPoint=NULL;
	}

	GameMapEditor::~GameMapEditor(void)
	{
		Destroy();
	}

	void GameMapEditor::Create(TerrainEditor *pTerrain,NullSceneGraph *pSG,NavNullSceneGraph *pNavSG)
	{
		m_pSceneGraph=pSG;
		m_pCamera=pSG->GetCamera();
		m_pTerrain=pTerrain;
		m_pNavSG=pNavSG;
	}

	void GameMapEditor::LoadAttDataBase()
	{	
		
		m_createData.LoadFromFile();
	
	}

	bool GameMapEditor::MousePick(UINT cursorX,UINT cursorY,Vector3& out)
	{
		return m_pTerrain->MousePick(cursorX,cursorY,m_pCamera,out);
	}

	void GameMapEditor::Destroy()
	{
		for(list<MapObjEdit*>::iterator iter=m_objList.begin();
			iter!=m_objList.end();++iter)
		{
			MapObjEdit *pObj=(*iter);
			delete pObj;
		}
		m_objList.clear();

		ClearWayPointList();
		ClearTriggerList();
		ClearSoundList();
	
		//SAFE_DELETE(m_pSceneGraph);
		GameMap::Destroy();
	}
	
	class ObjPicker
	{
	public:
		float		m_dist;
		MapObjEdit	*m_pSelectedObj;
		Ray			m_ray;

		ObjPicker(Ray mouseRay)
		{
			m_ray=mouseRay;
			m_dist=FLOAT_MAX;
			m_pSelectedObj=NULL;
		}
		~ObjPicker()	{}

		bool operator () (MapObjEdit *pObj)
		{
			EntityNode *pSGNode=pObj->GetSGNode();
			if(pSGNode)
			{
				TResult r=m_ray.Intersect(pSGNode->GetAABBox());
				if(r.first && r.second < FLOAT_MAX)
				{
					m_dist=r.second;
					m_pSelectedObj=pObj;
					return true;
				}
			}
			SGEffectNode *pSGEffectNode=pObj->GetSGEffectNode();
			if(pSGEffectNode)
			{
				Cool3D::AABBox box;
				pSGEffectNode->GetBox(box);

				TResult r=m_ray.Intersect(box);
				if(r.first && r.second < FLOAT_MAX)
				{
					m_dist=r.second;
					m_pSelectedObj=pObj;
					return true;
				}
			}
			return false;
		}
	};
	bool GameMapEditor::SelectObjByMouse(UINT cursorX,UINT cursorY)
	{
		if(m_pCamera==NULL)
			return false;
		m_pSelectedObj=NULL;
		Ray ray;
		m_pCamera->GetMouseRay(cursorX,cursorY,ray);
		ObjPicker picker(ray);
		list<MapObjEdit*>::reverse_iterator piter;
		for(piter=m_objList.rbegin(); piter!=m_objList.rend(); piter++)
		{
			MapObjEdit* pEdit=(*piter);
			if(picker(pEdit))
			{	
				m_pSelectedObj=picker.m_pSelectedObj;
				break;
			}
		}
		return m_pSelectedObj!=NULL;
	}
	bool GameMapEditor::SelectObjByMouseEx(UINT cursorX,UINT cursorY,MapObjEdit::EType type)
	{
		if(m_pCamera==NULL)
			return false;
		m_pSelectedObj=NULL;
		Ray ray;
		m_pCamera->GetMouseRay(cursorX,cursorY,ray);
		ObjPicker picker(ray);
		//list<MapObjEdit*>::reverse_iterator piter;
		list<MapObjEdit*>::iterator piter;
		vector<MapObjEdit*> vSelected;
		//for(piter=m_objList.rbegin(); piter!=m_objList.rend(); piter++)
		for(piter=m_objList.begin(); piter!=m_objList.end(); piter++)
		{
			MapObjEdit* pEdit=(*piter);
			if(type!=MapObjEdit::None && type!=pEdit->GetType())
			{
				continue;
			}
			if(picker(pEdit))
			{
				//m_pSelectedObj=picker.m_pSelectedObj;
				//break;
				vSelected.push_back(picker.m_pSelectedObj);				
			}
		}
		if(vSelected.size()<=0)
			return false;
		vector<MapObjEdit*>::iterator it=vSelected.begin();
		if(it!=vSelected.end())
		{
			m_pSelectedObj = (*it);
			++it;
		}
		AABBox box ;
		if(m_pSelectedObj->GetSGNode())
			box =  m_pSelectedObj->GetSGNode()->GetAABBox();
		else if(m_pSelectedObj->GetNavEntityNode())
			m_pSelectedObj->GetNavEntityNode()->GetBox(box);
		else if(m_pSelectedObj->GetSGEffectNode())
			m_pSelectedObj->GetSGEffectNode()->GetBox(box);
		for(; it!=vSelected.end(); ++it)
		{
			AABBox box1;
			if((*it)->GetSGNode())
				box1 = (*it)->GetSGNode()->GetAABBox();
			else if((*it)->GetNavEntityNode())
				(*it)->GetNavEntityNode()->GetBox(box1);			
			else if((*it)->GetSGNode())
				(*it)->GetSGEffectNode()->GetBox(box1);
			else
				continue;
			/*if( box1.max.y < box.max.y 
				&& (*it)->GetWorldPos().y > -4000)
				m_pSelectedObj = (*it);*/

			//在AABBox里
			if( (box.min.x<box1.min.x && box.min.z<box1.min.z) && (box.max.x>box1.max.z && box.max.z>box1.max.z) 
				&& box.max.y>box1.max.y )
			{
				if( box1.max.y<box.max.y && (*it)->GetWorldPos().y > -4000  )
					m_pSelectedObj = (*it);
			}
			else if( m_pSelectedObj->GetWorldPos().y < (*it)->GetWorldPos().y )
			{
				m_pSelectedObj = (*it);
			}
		}
		vSelected.clear();
		return m_pSelectedObj!=NULL;
	}

	
	bool GameMapEditor::SelectObjByID(WORD mapID)
	{
		return false;
	}
	void GameMapEditor::DrawEdit()
	{
		//画物体包围盒
		if(m_pSelectedObj)
		{
			EntityNode *pSGNode=m_pSelectedObj->GetSGNode();
			if(pSGNode)
			    DrawX::DrawAABBox(pSGNode->GetAABBox());
			SGEffectNode *pSGEffectNode=m_pSelectedObj->GetSGEffectNode();
			if(pSGEffectNode)
			{
				Cool3D::AABBox box;
				pSGEffectNode->GetBox(box);
				DrawX::DrawAABBox(box);
			}

		}
		const Heightmap *pHeightmap=m_pTerrain->GetHeightmap();
		if(!pHeightmap) 
			return;

		//--如果选中的是怪物，则画出脚下的一个框，用来表示随机出生范围
		//const int MonsterRand=10/2;
		if(m_pSelectedObj!=NULL
			&& m_pSelectedObj->GetType()==MapObjEdit::NPC
			&& m_pSelectedObj->GetRadius())
		{
			INT nRadius = (INT)m_pSelectedObj->GetRadius();
			Vector3 objPos=m_pSelectedObj->GetWorldPos();
			POINT center=pHeightmap->World2Tile(objPos);
			m_pTerrain->DrawGroundQuad(center.x-nRadius,center.x+nRadius,
				center.y-nRadius,center.y+nRadius,Color4ub(255,250,0,5));
		}
			
		//--
		if(m_bDrawTrigger)
			DrawTrigger();			//画触发器相关
		if(m_bDrawWayPoint)
			DrawWayPoint();			//画导航点相关
		DrawSound();			//画音效相关
		DrawPointLight();		//画点光源
		if(m_bIsDrawMapRect)
			DrawMapRect();		//画地图区域
		DrawPathPoint();		//画寻路导航点
		DrawSpawnPoint();		//画刷怪点
		DrawNPCNavStartPoint();
		DrawPlayerNavStartPoint();
	}

	//画导航点相关
	void GameMapEditor::DrawWayPoint()
	{
		const Heightmap *pHeightmap=m_pTerrain->GetHeightmap();
		//画导航点拾取区域
		if(m_bPickWayPoint)
		{
			Vector3 v1=m_vCurPickWayPoint+Vector3(-1*DISTENCE_WATPOINT_PICK,0,DISTENCE_WATPOINT_PICK);
			Vector3 v2=m_vCurPickWayPoint+Vector3(DISTENCE_WATPOINT_PICK,0,DISTENCE_WATPOINT_PICK);
			Vector3 v3=m_vCurPickWayPoint+Vector3(DISTENCE_WATPOINT_PICK,0,-1*DISTENCE_WATPOINT_PICK);
			Vector3 v4=m_vCurPickWayPoint+Vector3(-1*DISTENCE_WATPOINT_PICK,0,-1*DISTENCE_WATPOINT_PICK);
			DWORD color=0x0000FFFF;
			DrawX::Draw3DLine(v1,v2,color);	 
			DrawX::Draw3DLine(v2,v3,color);	
			DrawX::Draw3DLine(v3,v4,color);	 
			DrawX::Draw3DLine(v4,v1,color);	

			Vector3 v5=v1+Vector3(0,HEIGHT_SHOW_BOX,0);
			Vector3 v6=v2+Vector3(0,HEIGHT_SHOW_BOX,0);
			Vector3 v7=v3+Vector3(0,HEIGHT_SHOW_BOX,0);
			Vector3 v8=v4+Vector3(0,HEIGHT_SHOW_BOX,0);

			DrawX::Draw3DLine(v5,v6,color);	 
			DrawX::Draw3DLine(v6,v7,color);	
			DrawX::Draw3DLine(v7,v8,color);	 
			DrawX::Draw3DLine(v8,v5,color);	

			DrawX::Draw3DLine(v5,v1,color);	 
			DrawX::Draw3DLine(v6,v2,color);	
			DrawX::Draw3DLine(v7,v3,color);	 
			DrawX::Draw3DLine(v8,v4,color);	
		}

		//draw line of waypoint
		if(m_WayPointList.size()<1)
			return ;

		list<tagMapWayPoint*>::iterator prepIter = m_WayPointList.begin();
		list<tagMapWayPoint*>::iterator pIter = m_WayPointList.begin();
		pIter++;

		for(; pIter != m_WayPointList.end(); ++prepIter,++pIter)
		{
			if(_tcscmp((*prepIter)->szName,(*pIter)->szName)==0)
			{
				Vector3 point1 = (*prepIter)->vPos;
				Vector3 point2 = (*pIter)->vPos;
				DWORD color=0xFFFF00FF;
				DrawX::Draw3DLine(point1,point2,color);
			}
		}
		//draw aabb and zone
		for(pIter = m_WayPointList.begin(); pIter != m_WayPointList.end();++pIter)
		{
			Vector3 point1 = (*pIter)->vPos;

			Cool3D::AABBox box;
			box.max=point1+Vector3((*pIter)->vRange.x, (*pIter)->vRange.y*2, (*pIter)->vRange.z);
			box.min=point1-Vector3((*pIter)->vRange.x, 0, (*pIter)->vRange.z);

			DWORD color=0xFFFF00FF;
			if((*pIter)==m_pSelectWayPoint)
				DrawX::DrawAABBox(box);
			else
				DrawX::DrawAABBox(box,color);

		}    
		//draw line between mouse and laster waypoint
		if(m_bCurDrawWayPoint)	
		{
			Vector3 point = (*prepIter)->vPos;
			DWORD color=0xFFFF00FF;
			DrawX::Draw3DLine(point,m_vCurPosForWayPoint,color);	 
		}
	}

	//画触发器相关
	void GameMapEditor::DrawTrigger()
	{
		//画拾取触发器标识
		if(m_bPickTrigger)
		{
			Cool3D::AABBox box;
			box.max= m_vCurPickTrigger+Vector3(20, 20, 20);
			box.min= m_vCurPickTrigger-Vector3(20, 20, 20);
			DrawX::DrawAABBox(box, 0x00FFFF00);

		}

		list<tagMapTrigger*>::iterator pTriggerIter = m_TriggerList.begin();
		for(; pTriggerIter != m_TriggerList.end(); ++pTriggerIter)
		{
			tagMapTrigger *pTrigger = (*pTriggerIter);
			int nPointNum = (int)pTrigger->region.size();
			for(int i=0; i<nPointNum; i++)
			{
				Vector3 point1 = pTrigger->region[i];
				if(point1.x<=0.0f || point1.y <0.0f || point1.z<=0.0f)
					break;

				Cool3D::AABBox box;
				box.max= point1+Vector3(20, 20, 20);
				box.min= point1-Vector3(20, 20, 20);
				if(m_pSelectTrigger==pTrigger)
				{						
					Cool3D::AABBox box1 = pTrigger->box; 
					//box1.max.y += pTrigger->fHeight;
					DrawX::DrawAABBox(box1);
				}
				if( m_pSelectTrigger==pTrigger
					&& m_bPickTriggerPoint 
					&& m_nTriggerNum==i)
					DrawX::DrawAABBox(box, 0x00FF0000);
				else
					DrawX::DrawAABBox(box,0x00FFFF00);

				Vector3 point2(0,0,0);
				if(i+1<nPointNum)
					point2 = pTrigger->region[i+1];		
				if(i+1<nPointNum)
					DrawX::Draw3DLine(point1,point2,0x00FFFF00);
				else
				{
					Vector3 pointNo1 = pTrigger->region[0];
					DrawX::Draw3DLine(pointNo1,point1,0x00FFFF00);
				}
			}
		}
		
		if(m_bCurDrawTrigger)
		{
			for(int i=0; i<m_nCurTriggerNum; i++)
			{
				Vector3 point1 = m_vTriggerPos[i];
				Cool3D::AABBox box;
				box.max= point1+Vector3(20, 20, 20);
				box.min= point1-Vector3(20, 20, 20);
				DrawX::DrawAABBox(box, 0x00FFFF00);
				

				DWORD color = 0x00FFFF00;
				if(i+1<m_nCurTriggerNum)
					DrawX::Draw3DLine(m_vTriggerPos[i], m_vTriggerPos[i+1], color);	
			}
			Vector3 vTempPos; 
			GetPosForTrigger(vTempPos, m_nCurTriggerNum-1);
			DWORD color = 0x00FFFF00;
			DrawX::Draw3DLine(vTempPos, m_vCurPosForTrigger, color);	 
		}

		
	}

	void GameMapEditor::DrawMapRect()	//画地图区域
	{
		//画拾取触发器标识
		if(m_bPickMapRect)
		{
			Cool3D::AABBox box;
			box.max= m_vCurPickMapRect+Vector3(20, 20, 20);
			box.min= m_vCurPickMapRect-Vector3(20, 20, 20);
			DrawX::DrawAABBox(box, 0x007544D8);

		}

		list<MapObjEdit*>::iterator Iter = m_pMapRectList.begin();
		for(; Iter != m_pMapRectList.end(); ++Iter)
		{
			const tagMapArea *pMapArea = (*Iter)->GetMapRect();
			int nPointNum = (int)pMapArea->region.size();
			for(int i=0; i<nPointNum; i++)
			{
				Vector3 point1 = pMapArea->region[i];
				if(point1.x<=0.0f || point1.y <0.0f || point1.z<=0.0f)
					break;
				Cool3D::AABBox box;
				box.max= point1+Vector3(20, 20, 20);
				box.min= point1-Vector3(20, 20, 20);

				if(m_pSelectMapRect==(*Iter))
				{						
					Cool3D::AABBox box1 = pMapArea->box; 
					//box1.max.y += pMapArea->fHeight;
					DrawX::DrawAABBox(box1);
				}
				if( m_pSelectMapRect==(*Iter)
					&& m_bPickMapAreaPoint 
					&& m_nMapAreaNum==i)
					DrawX::DrawAABBox(box, 0x00FF0000);
				else
					DrawX::DrawAABBox(box,0x007544D8);

				Vector3 point2;
				if(i+1<nPointNum)
					point2 = pMapArea->region[i+1];

				if(i+1<nPointNum)
					DrawX::Draw3DLine(point1,point2,0x007544D8);
				else
				{
					Vector3 pointNo1 = pMapArea->region[0];
					DrawX::Draw3DLine(pointNo1,point1,0x007544D8);
				}
			}
		}

		if(m_bDrawMapRect)
		{
			for(int i=0; i<m_nCurMapAreaNum; i++)
			{
				Vector3 point1 = m_vMapAreaPos[i];
				Cool3D::AABBox box;
				box.max= point1+Vector3(20, 20, 20);
				box.min= point1-Vector3(20, 20, 20);
				DrawX::DrawAABBox(box, 0x007544D8);


				DWORD color = 0x007544D8;
				if(i+1<m_nCurMapAreaNum)
					DrawX::Draw3DLine(m_vMapAreaPos[i], m_vMapAreaPos[i+1], color);	
			}
			Vector3 vTempPos; 
			GetPosForMapArea(vTempPos, m_nCurMapAreaNum-1);
			DWORD color = 0x007544D8;
			DrawX::Draw3DLine(vTempPos, m_vCurPickMapRect, color);	 
		}
	}
	//画音效相关
	void GameMapEditor::DrawSound()
	{
		list<tagMapSound*>::iterator pIter = m_SoundList.begin();
		for(; pIter != m_SoundList.end(); ++pIter)
		{
			tagMapSound* pEdit=(*pIter);
			Cool3D::AABBox box;
			box.max=Vector3(pEdit->fPos[0],pEdit->fPos[1],pEdit->fPos[2])
				+Vector3(DISTENCE_SOUNDBOX_PICK,DISTENCE_SOUNDBOX_PICK,DISTENCE_SOUNDBOX_PICK);
			box.min=Vector3(pEdit->fPos[0],pEdit->fPos[1],pEdit->fPos[2])
				-Vector3(DISTENCE_SOUNDBOX_PICK,DISTENCE_SOUNDBOX_PICK,DISTENCE_SOUNDBOX_PICK);
			
			DWORD color=0x0033660F;
			if(m_pSelectSound==pEdit)
			{
				DrawX::DrawAABBox(box);    
				box.max = Vector3(pEdit->fPos[0],pEdit->fPos[1],pEdit->fPos[2])
					+ Vector3(pEdit->fRange, pEdit->fRange, pEdit->fRange);
				box.min = Vector3(pEdit->fPos[0],pEdit->fPos[1],pEdit->fPos[2])
					- Vector3(pEdit->fRange, pEdit->fRange, pEdit->fRange);
				DrawX::DrawAABBox(box, 0xFFFFDD00); 

				box.max = Vector3(pEdit->fPos[0],pEdit->fPos[1],pEdit->fPos[2])
					+ Vector3(pEdit->fNoAtteRange, pEdit->fNoAtteRange, pEdit->fNoAtteRange);
				box.min = Vector3(pEdit->fPos[0],pEdit->fPos[1],pEdit->fPos[2])
					- Vector3(pEdit->fNoAtteRange, pEdit->fNoAtteRange, pEdit->fNoAtteRange);
				DrawX::DrawAABBox(box, 0xFFFFCCFF); 
			}
			else
			   DrawX::DrawAABBox(box,color);    
		}
		if(m_bPickSound)
		{
			Cool3D::AABBox box;
			box.max=Vector3(m_vCurPosSound.x,m_vCurPosSound.y,m_vCurPosSound.z)
				+Vector3(DISTENCE_SOUNDBOX_PICK,DISTENCE_SOUNDBOX_PICK,DISTENCE_SOUNDBOX_PICK);
			box.min=Vector3(m_vCurPosSound.x,m_vCurPosSound.y,m_vCurPosSound.z)
				-Vector3(DISTENCE_SOUNDBOX_PICK,DISTENCE_SOUNDBOX_PICK,DISTENCE_SOUNDBOX_PICK);

			DWORD colorBlue = 0x0033660F;
			DrawX::DrawAABBox(box,colorBlue);			
		}
	}

	//画寻路导航点
	void GameMapEditor::DrawPathPoint()
	{
		if(m_bDrawPathPoint)
		{
			Cool3D::AABBox box;
			box.max=Vector3(m_vCurPosForPathPoint.x,m_vCurPosForPathPoint.y,m_vCurPosForPathPoint.z)
				+Vector3(DIS_PATHPOINT_XZ, DIS_PATHPOINT_Y, DIS_PATHPOINT_XZ);
			box.min=Vector3(m_vCurPosForPathPoint.x,m_vCurPosForPathPoint.y,m_vCurPosForPathPoint.z)
				-Vector3(DIS_PATHPOINT_XZ, DIS_PATHPOINT_Y, DIS_PATHPOINT_XZ);

			DWORD color=0x00FFB029;			
			DrawX::DrawAABBox(box,color);  
		}

		list<MapObjEdit*>::iterator pIter = m_pPathPointList.begin();
		for(; pIter != m_pPathPointList.end(); ++pIter)
		{
			tagMapPathPoint* pEdit = (tagMapPathPoint*)(*pIter)->GetPathPoint();
			Cool3D::AABBox box;
			box.max = Vector3(pEdit->vPos.x, pEdit->vPos.y, pEdit->vPos.z)
				+Vector3(DIS_PATHPOINT_XZ, DIS_PATHPOINT_Y, DIS_PATHPOINT_XZ);
			box.min = Vector3(pEdit->vPos.x, pEdit->vPos.y, pEdit->vPos.z)
				-Vector3(DIS_PATHPOINT_XZ, DIS_PATHPOINT_Y, DIS_PATHPOINT_XZ);

			DWORD color=0x00FFB029;			
			DrawX::DrawAABBox(box,color);   
			if(m_pSelectPathPoint==(*pIter) || pEdit->bFlag)
			{
				/*Cool3D::AABBox rangeBox;
				rangeBox.max = Vector3(pEdit->vPos.x, pEdit->vPos.y, pEdit->vPos.z)
					+Vector3(pEdit->fRange/2, pEdit->fRange/2, pEdit->fRange/2);
				rangeBox.min = Vector3(pEdit->vPos.x, pEdit->vPos.y, pEdit->vPos.z)
					-Vector3(pEdit->fRange/2, pEdit->fRange/2, pEdit->fRange/2);*/

				DWORD color=0x00FF0000;			
				DrawX::DrawAABBox(box,color);   
			}
		}
		
	}

	void GameMapEditor::DrawSpawnPoint()
	{
		list<MapObjEdit*>::iterator pIter = m_pSpawnPointList.begin();
		for(; pIter != m_pSpawnPointList.end(); ++pIter)
		{
			tagMapSpawnPoint* pEdit = (tagMapSpawnPoint*)(*pIter)->GetSpawnPoint();
			Cool3D::AABBox box;
			box.max = Vector3(pEdit->vPos.x, pEdit->vPos.y, pEdit->vPos.z)
				+Vector3(DIS_SPAWNPOINT_XZ, DIS_SPAWNPOINT_Y*2, DIS_SPAWNPOINT_XZ);
			box.min = Vector3(pEdit->vPos.x, pEdit->vPos.y, pEdit->vPos.z)
				-Vector3(DIS_SPAWNPOINT_XZ, 0, DIS_SPAWNPOINT_XZ);
		
			DrawX::DrawAABBox(box,0x004EBFF8);   
			if(m_pSelectSpawnPoint==(*pIter))
			{		
				DrawX::DrawAABBox(box, 0x00FF0000);   
			}
		}
		if(m_bDrawSpawnPoint)
		{
			Cool3D::AABBox box;
			box.max=Vector3(m_vCurPosForSpawnPoint.x,m_vCurPosForSpawnPoint.y,m_vCurPosForSpawnPoint.z)
				+Vector3(DIS_SPAWNPOINT_XZ, DIS_SPAWNPOINT_Y*2, DIS_SPAWNPOINT_XZ);
			box.min=Vector3(m_vCurPosForSpawnPoint.x,m_vCurPosForSpawnPoint.y,m_vCurPosForSpawnPoint.z)
				-Vector3(DIS_SPAWNPOINT_XZ, 0 ,DIS_SPAWNPOINT_XZ);
		
			DrawX::DrawAABBox(box, 0x004EBFF8);  
		}
	}

	void GameMapEditor::DrawNPCNavStartPoint()
	{
		if(m_bDrawNPCNavStartPoint)
		{
			Cool3D::AABBox box;
			box.max=Vector3(m_vCurPosForNPCNav.x,m_vCurPosForNPCNav.y,m_vCurPosForNPCNav.z)
				+Vector3(DIS_NPCNAVSTARTPOINT_PICK, DIS_NPCNAVSTARTPOINT_PICK*2, DIS_NPCNAVSTARTPOINT_PICK);
			box.min=Vector3(m_vCurPosForNPCNav.x,m_vCurPosForNPCNav.y,m_vCurPosForNPCNav.z)
				-Vector3(DIS_NPCNAVSTARTPOINT_PICK, 0, DIS_NPCNAVSTARTPOINT_PICK);

			DrawX::DrawAABBox(box, 0x0000FF00);  
		}

		list<tagNPCNavMapBuildStartPoint*>::iterator pIter = m_NPCNavStartPointList.begin();
		for(; pIter!=m_NPCNavStartPointList.end(); ++pIter)
		{
			tagNPCNavMapBuildStartPoint* pEdit = (*pIter);
			Cool3D::AABBox box;
			box.max = Vector3(pEdit->vPos.x, pEdit->vPos.y, pEdit->vPos.z)
				+Vector3(DIS_NPCNAVSTARTPOINT_PICK, DIS_NPCNAVSTARTPOINT_PICK*2, DIS_NPCNAVSTARTPOINT_PICK);
			box.min = Vector3(pEdit->vPos.x, pEdit->vPos.y, pEdit->vPos.z)
				-Vector3(DIS_NPCNAVSTARTPOINT_PICK, 0, DIS_NPCNAVSTARTPOINT_PICK);

			DrawX::DrawAABBox(box,0x0000FF00);   
			if(m_pSelectNPCNavStartPoint==(*pIter))
			{		
				DrawX::DrawAABBox(box, 0x00FF0000);   
			}
		}
		
	}

	void GameMapEditor::DrawPlayerNavStartPoint()
	{
		if(m_bDrawPlayerNavStartPoint)
		{
			Cool3D::AABBox box;
			box.max=Vector3(m_vCurPosForPlayerNav.x,m_vCurPosForPlayerNav.y,m_vCurPosForPlayerNav.z)
				+Vector3(DIS_NPCNAVSTARTPOINT_PICK, DIS_NPCNAVSTARTPOINT_PICK*2, DIS_NPCNAVSTARTPOINT_PICK);
			box.min=Vector3(m_vCurPosForPlayerNav.x,m_vCurPosForPlayerNav.y,m_vCurPosForPlayerNav.z)
				-Vector3(DIS_NPCNAVSTARTPOINT_PICK, 0, DIS_NPCNAVSTARTPOINT_PICK);

			DrawX::DrawAABBox(box, 0x00FF99FF);  
		}

		list<tagPlayerNavMapBuildStartPoint*>::iterator pIter = m_PlayerNavStartPointList.begin();
		for(; pIter!=m_PlayerNavStartPointList.end(); ++pIter)
		{
			tagPlayerNavMapBuildStartPoint* pEdit = (*pIter);
			Cool3D::AABBox box;
			box.max = Vector3(pEdit->vPos.x, pEdit->vPos.y, pEdit->vPos.z)
				+Vector3(DIS_NPCNAVSTARTPOINT_PICK, DIS_NPCNAVSTARTPOINT_PICK*2, DIS_NPCNAVSTARTPOINT_PICK);
			box.min = Vector3(pEdit->vPos.x, pEdit->vPos.y, pEdit->vPos.z)
				-Vector3(DIS_NPCNAVSTARTPOINT_PICK, 0, DIS_NPCNAVSTARTPOINT_PICK);

			DrawX::DrawAABBox(box,0x00FF99FF);   
			if(m_pSelectPlayerNavStartPoint==(*pIter))
			{		
				DrawX::DrawAABBox(box, 0x00FF0000);   
			}
		}
		
	}
	
	//画点光源
	void GameMapEditor::DrawPointLight()
	{
		list<MapObjEdit*>::iterator pIter = m_PointLightList.begin();
		for(; pIter != m_PointLightList.end(); ++pIter)
		{
			tagMapPointLight* pEdit=(tagMapPointLight*)(*pIter)->GetPointLgtObj();
			Cool3D::AABBox box;
			box.max=Vector3(pEdit->fPos[0],pEdit->fPos[1],pEdit->fPos[2])
				+Vector3(DISTENCE_POINTLIGHT_PICK,DISTENCE_POINTLIGHT_PICK,DISTENCE_POINTLIGHT_PICK);
			box.min=Vector3(pEdit->fPos[0],pEdit->fPos[1],pEdit->fPos[2])
				-Vector3(DISTENCE_POINTLIGHT_PICK,DISTENCE_POINTLIGHT_PICK,DISTENCE_POINTLIGHT_PICK);

			DWORD colorBlue = 0x0000CCFF;
			if(m_pSelectPointLight==(*pIter))
				DrawX::DrawAABBox(box);    
			else
				DrawX::DrawAABBox(box,colorBlue);   

			Cool3D::AABBox innerBox;
			innerBox.max=Vector3(pEdit->fPos[0],pEdit->fPos[1],pEdit->fPos[2])
				+Vector3(DISTENCE_POINTLIGHT_PICK-90,DISTENCE_POINTLIGHT_PICK-90,DISTENCE_POINTLIGHT_PICK-90);
			innerBox.min=Vector3(pEdit->fPos[0],pEdit->fPos[1],pEdit->fPos[2])
				-Vector3(DISTENCE_POINTLIGHT_PICK-90,DISTENCE_POINTLIGHT_PICK-90,DISTENCE_POINTLIGHT_PICK-90);

			DWORD colorYello = 0x00FFFF00;			
			DrawX::DrawAABBox(innerBox,colorYello); 

			Cool3D::AABBox noBlockBox;
			noBlockBox.max=Vector3(pEdit->fPos[0],pEdit->fPos[1],pEdit->fPos[2])
				+Vector3(pEdit->fnoBlockRange,pEdit->fnoBlockRange, pEdit->fnoBlockRange);
			noBlockBox.min=Vector3(pEdit->fPos[0],pEdit->fPos[1],pEdit->fPos[2])
				-Vector3(pEdit->fnoBlockRange, pEdit->fnoBlockRange, pEdit->fnoBlockRange);

			DWORD noBlockColor = 0x00FF99FF;			
			DrawX::DrawAABBox(noBlockBox, noBlockColor); 

		}
		if(m_bPickPointLight)
		{
			Cool3D::AABBox box;
			box.max=Vector3(m_vCurPosPointLight.x,m_vCurPosPointLight.y,m_vCurPosPointLight.z)
				+Vector3(DISTENCE_POINTLIGHT_PICK,DISTENCE_POINTLIGHT_PICK,DISTENCE_POINTLIGHT_PICK);
			box.min=Vector3(m_vCurPosPointLight.x,m_vCurPosPointLight.y,m_vCurPosPointLight.z)
				-Vector3(DISTENCE_POINTLIGHT_PICK,DISTENCE_POINTLIGHT_PICK,DISTENCE_POINTLIGHT_PICK);

			DWORD colorBlue = 0x0000CCFF;
			DrawX::DrawAABBox(box,colorBlue);
		}
	}


	void GameMapEditor::AddMapObjEdit(MapObjEdit* pEdit)
	{
		m_objList.push_back(pEdit);
	}


	MapObjEdit* GameMapEditor::RemoveMapObjEdit(MapObjEdit::EType Type, DWORD mapID)
	{
		list<MapObjEdit*>::iterator pIter = m_objList.begin();
		for(; pIter != m_objList.end(); ++pIter)
			if((*pIter)->GetType() == Type && (*pIter)->GetMapID() == mapID)
			{
				MapObjEdit* pEdit = *pIter;
				m_objList.erase(pIter);
				if(pEdit->GetSGEffectNode())
				{
					m_pSceneGraph->DetachDynamicNode(pEdit->GetSGEffectNode());
				}
				return pEdit;
			}				
		return NULL;
	}

	bool GameMapEditor::RemoveMapObjEdit(MapObjEdit* pEdit)
	{ 
		list<MapObjEdit*>::iterator pIter = m_objList.begin();
		for(; pIter != m_objList.end(); ++pIter)
			if((*pIter) == pEdit)
			{
				m_objList.erase(pIter);
				if(pEdit->GetSGEffectNode())
				{
					m_pSceneGraph->DetachDynamicNode(pEdit->GetSGEffectNode());
				}
				return true;
			}
		return false;
	}

	//查找物体
	MapObjEdit* GameMapEditor::FindMapObjEdit(MapObjEdit::EType Type, DWORD ObjID)
	{
		list<MapObjEdit*>::iterator pIter = m_objList.begin();
		for(; pIter != m_objList.end(); ++pIter)
		{
			if((*pIter)->GetType() == Type && (*pIter)->GetMapID() == ObjID)
			{
				MapObjEdit* pEdit = *pIter;
				return pEdit;
			}				
		}  
		return NULL;
	}


	const list<MapObjEdit*>& GameMapEditor::GetMapObjEditList()
	{
		return m_objList;
	}

	void GameMapEditor::ClearEditList(bool bAll)
	{
		int nSize = m_objList.size();
		list<MapObjEdit*>::iterator pIter = m_objList.begin();
		for(; pIter != m_objList.end(); )
		{
			MapObjEdit* pEdit = *pIter;
			MapObjEdit::EType type = pEdit->GetType();
			if( !bAll && (type == MapObjEdit::NPC || type == MapObjEdit::StaticObj) )
			{
				pIter = m_objList.erase(pIter);
				delete pEdit;
			}
			else if( bAll )
			{
				pIter = m_objList.erase(pIter);
				delete pEdit;
			}
			else 
			{
				++pIter;
			}
		}

		if( bAll )
		{
			m_objList.clear();
		}

		ResMgr::Inst()->DoGC();

		m_pSelectedObj = NULL;
	}

	bool GameMapEditor::LoadFromEditFile(const TCHAR* szFileName)
	{
		IFS* pFS=Kernel::Inst()->DefaultFS();
		DWORD hFile=pFS->Open(szFileName);
		THROW_NULLEX(hFile,_T("GameMap open file failed."),szFileName);

		tagMapHeader header;

		pFS->Read(&header, sizeof(header), hFile);

		if(strcmp((char*)&header.dwMagic, MapObjFileMagic) != 0)
		{
			pFS->Close(hFile);
			return false;
		}

		ASSERT(m_pSceneGraph != NULL);
		ASSERT(sizeof(m_Fog)==sizeof(header.byDistFog));
		memcpy(&m_Fog,header.byDistFog,sizeof(m_Fog));
		ASSERT(sizeof(m_SunLight)==sizeof(header.bySunLight));
		memcpy(&m_SunLight,header.bySunLight,sizeof(m_SunLight));
		
		m_pSceneGraph->SetSceneFog(m_Fog);
		m_pSceneGraph->SetSunLight(m_SunLight);

		_tcscpy_s(m_szSkyBoxTopTex, sizeof(m_szSkyBoxTopTex)/sizeof(TCHAR),header.szSkyBoxTopTex);
		_tcscpy_s(m_szSkyBoxFrontRightTex, sizeof(m_szSkyBoxFrontRightTex)/sizeof(TCHAR),header.szSkyBoxFrontRightTex);
		_tcscpy_s(m_szSkyBoxBackLeftTex, sizeof(m_szSkyBoxBackLeftTex)/sizeof(TCHAR),header.szSkyBoxBackLeftTex);
		m_fSkyBoxXsize = header.fSkyBoxXsize;
		m_fSkyBoxYsize = header.fSkyBoxYsize;
		m_fSkyBoxZsize = header.fSkyBoxZsize;	
		m_fSkyBoxOffX = header.fSkyBoxOffX;
		m_fSkyBoxOffY = header.fSkyBoxOffY;
		m_fSkyBoxOffZ = header.fSkyBoxOffZ;
		m_fSunModulus = header.fSunModulus;
		m_fSkyYaw = header.fSkyYaw;
		m_dwGlobalID = header.dwGlobalID;

		ASSERT(sizeof(m_SkyCol)==sizeof(header.bySkyCol));
		memcpy( &m_SkyCol, header.bySkyCol, sizeof(m_SkyCol) );

		m_bRenderSkyShade = ( 0 != header.byRenderSkyShade );
		m_dwSkyShadeCol = header.dwSkyShadeCol;
		m_dwDynamicDiffCol = header.dwDynamicDiffCol;
		m_dwDynamicAmbCol = header.dwDynamicAmbCol;
		m_dwDynamicSpecCol = header.dwDynamicSpecCol;

		int i=0;
		bool bLoadSucc = true;
		pFS->Seek(hFile, header.dwNpcOffset, FILE_BEGIN);
		for(i = 0; i < header.nNumNPC; i++)
		{
			tagMapNPC npc;
			pFS->Read(&npc, sizeof(npc), hFile);

			MapObjEdit* pEdit = new MapObjEdit;
	    	if(pEdit->Create(npc, this))
			{
				Vector3 pos;
				pos = npc.vPos;
				pEdit->SetWorldPos(pos);
				pEdit->SetYawForNPC(npc.fYaw);
			}
			else
			{
				delete pEdit;
				bLoadSucc = false;
			}
		}

		// 获得转换列表
		vector<tstring> fsm2fscn;
		vector<tstring> fscn2fsm;
		TCHAR szTransPath[256];
		_stprintf( szTransPath, _T("%sconfig\\fsm2fscn.csv"), Kernel::Inst()->GetWorkPath() );
		GetTransList( pFS, szTransPath, fsm2fscn );
		_stprintf( szTransPath, _T("%sconfig\\fscn2fsm.csv"), Kernel::Inst()->GetWorkPath() );
		GetTransList( pFS, szTransPath, fscn2fsm );
		
		pFS->Seek(hFile, header.dwStaticObjOffset, FILE_BEGIN);
		for(i = 0; i < header.nNumStaticObj; i++)
		{
			tagStaticMapObj staticObj;
			pFS->Read(&staticObj, sizeof(staticObj), hFile);

			//--
			TCHAR szDrive[32];
			TCHAR szDir[256];
			TCHAR szFile[256];
			_tsplitpath( staticObj.szMdlPath, szDrive, szDir, szFile, NULL );
			tstring mdlPath = staticObj.szMdlPath;

			// 把列表中fsm改为fscn
			TCHAR *pRet = NULL;
			for( size_t fsmi=0; fsmi<fsm2fscn.size(); fsmi++ )
			{
				tstring& strFsm = fsm2fscn[fsmi];
				pRet = _tcsstr(szFile,strFsm.c_str());
				if( pRet != NULL )//替换为.fscn
				{
					_stprintf( staticObj.szMdlPath, _T("%s%s%s.fscn"), szDrive, szDir, szFile );
					break;
				}
			}

			// 把列表中fscn改为fsm
			for( size_t fscni=0; fscni<fscn2fsm.size(); fscni++ )
			{
				tstring& strFscn = fscn2fsm[fscni];
				pRet = _tcsstr(szFile,strFscn.c_str());
				if( pRet != NULL )//替换为.fsm
				{
					_stprintf( staticObj.szMdlPath, _T("%s%s%s.fsm"), szDrive, szDir, szFile );
					break;
				}
			}

			//--旧的静态地物材质增强系数都为0.0f，要改为1.5f
			if( staticObj.fMtlDiffuseFactor<=0.0f )
			{
				staticObj.fMtlDiffuseFactor=1.5f;
			}

			MapObjEdit* pEdit = new MapObjEdit;
			if(pEdit->Create(staticObj, this))
			{
				Vector3 pos;
				pos.x = staticObj.fPos[0];
				pos.y = staticObj.fPos[1];
				pos.z = staticObj.fPos[2];
				pEdit->SetWorldPos(pos);			
			}
			else
				delete pEdit;
		}

		pFS->Seek(hFile, header.dwWayPointOffset, FILE_BEGIN);
		tagMapWayPoint *pWayPointObj;
		for(i = 0; i < header.nNumWayPoint; i++)
		{
			pWayPointObj=new tagMapWayPoint;
			pFS->Read(pWayPointObj, sizeof(tagMapWayPoint), hFile);
			int nIndex;
			AddWayPoint(pWayPointObj,nIndex);	
		}

		pFS->Seek(hFile, header.dwTriggerOffset, FILE_BEGIN);
	
		tagMapTrigger *pTriggerObj;
		for(i = 0; i < header.nNumTrigger; i++)
		{			

			pTriggerObj=new tagMapTrigger;
			tstring szTemp;
			FReadValue(pFS, hFile, pTriggerObj->dwObjID);
			FReadValue(pFS, hFile, pTriggerObj->eType);
			FReadValVector(pFS, hFile, pTriggerObj->region);
			FReadValue(pFS, hFile, pTriggerObj->box.max);
			FReadValue(pFS, hFile, pTriggerObj->box.min);
			FReadValue(pFS, hFile, pTriggerObj->fHeight);

			pFS->Read(pTriggerObj->szMapName, sizeof(pTriggerObj->szMapName), hFile);
			pFS->Read(pTriggerObj->szWayPoint, sizeof(pTriggerObj->szWayPoint), hFile);
			pFS->Read(pTriggerObj->szScriptName, sizeof(pTriggerObj->szScriptName), hFile);

			FReadValue(pFS, hFile, pTriggerObj->dwParam);
			FReadValue(pFS, hFile, pTriggerObj->bLock);
			FReadValue(pFS, hFile, pTriggerObj->bFlag);
			FReadValue(pFS, hFile, pTriggerObj->dwQuestSerial);
			pFS->Read(pTriggerObj->byReserve, sizeof(pTriggerObj->byReserve), hFile);
			ReCalTriggerAABBox(pTriggerObj);
			AddTrigger(pTriggerObj);
		}

		pFS->Seek(hFile, header.dwSoundOffset, FILE_BEGIN);
		tagMapSound *pSoundObj;
		for(i = 0; i < header.nNumSound; i++)
		{
			pSoundObj=new tagMapSound;
			pFS->Read(pSoundObj, sizeof(tagMapSound), hFile);
			AddSound(pSoundObj);
		}

		pFS->Seek(hFile, header.dwPointLightOffset, FILE_BEGIN);
		for(i=0; i<header.nNumPointLight; i++)
		{
			tagMapPointLight PointLgtObj;
			pFS->Read(&PointLgtObj, sizeof(PointLgtObj), hFile);

			MapObjEdit* pEdit = new MapObjEdit;
			if(!pEdit->Create(PointLgtObj, this))
				delete pEdit;
		}

		pFS->Seek(hFile, header.dwMapRectOffset, FILE_BEGIN);
		for(int i=0; i<header.nNumMapRect; i++)
		{
			tagMapArea mapRect;
			//pFS->Read(&mapRect, sizeof(mapRect), hFile);
			FReadValue(pFS, hFile, mapRect.dwObjID);
			FReadValue(pFS, hFile, mapRect.eType);
			FReadValVector(pFS, hFile, mapRect.region);
			FReadValue(pFS, hFile, mapRect.box.max);
			FReadValue(pFS, hFile, mapRect.box.min);
			FReadValue(pFS, hFile, mapRect.fHeight);
			FReadValue(pFS, hFile, mapRect.bLock);
			FReadValue(pFS, hFile, mapRect.bFlag);
			FReadValue(pFS, hFile, mapRect.dwMiniMapSize);
			FReadValue(pFS, hFile, mapRect.bDistFog);
			pFS->Read(mapRect.byDistFog, sizeof(mapRect.byDistFog), hFile);
			pFS->Read(mapRect.byReserve, sizeof(mapRect.byReserve), hFile);
			ReCalMapAreaAABBox(&mapRect);

			MapObjEdit* pEdit = new MapObjEdit;
			if(!pEdit->Create(mapRect, this))
				delete pEdit;
		}

		pFS->Seek(hFile, header.dwPathPointOffset, FILE_BEGIN);
		for(int i=0; i<header.nNumPathPoint; i++)
		{
			tagMapPathPoint pathpoint;
			pFS->Read(&pathpoint, sizeof(tagMapPathPoint), hFile);
			MapObjEdit* pEdit = new MapObjEdit;
			if(!pEdit->Create(pathpoint, this))
				delete pEdit;
		}

		pFS->Seek(hFile, header.dwSpawnPointOffset, FILE_BEGIN);
		for(int i=0; i<header.nNumSpawnPoint; i++)
		{
			tagMapSpawnPoint spawnpoint;
			pFS->Read(&spawnpoint, sizeof(tagMapSpawnPoint), hFile);
			MapObjEdit* pEdit = new MapObjEdit;
			if(!pEdit->Create(spawnpoint, this))
				delete pEdit;
		}

		pFS->Seek(hFile, header.dwDynamicBlockOffset, FILE_BEGIN);
		for(i = 0; i < header.nNumDynamicBlock; i++)
		{
			tagMapDynamicBlock dynaBlock;
			pFS->Read(&dynaBlock, sizeof(tagMapDynamicBlock), hFile);

			MapObjEdit* pEdit = new MapObjEdit;
			if(pEdit->Create(dynaBlock, this))
			{
				Vector3 pos;
				pos = dynaBlock.vPos;
				pEdit->SetWorldPos(pos);			
			}
			else
				delete pEdit;
		}

		pFS->Seek(hFile, header.dwEventTriggerOffset, FILE_BEGIN);
		for(i=0; i<header.nNumEventTrigger; i++)
		{
			tagMapEventTrigger eventTrigger;
			pFS->Read(&eventTrigger, sizeof(tagMapEventTrigger), hFile);
			MapObjEdit* pEdit = new MapObjEdit;
			if(!pEdit->Create(eventTrigger, this))
				delete pEdit;
		}

		pFS->Seek(hFile, header.dwNPCNavOffset, FILE_BEGIN);
		tagNPCNavMapBuildStartPoint* pNPCNav;
		for(i=0; i<header.nNumNPCNav; i++)
		{
			pNPCNav = new tagNPCNavMapBuildStartPoint;
			pFS->Read(pNPCNav, sizeof(tagNPCNavMapBuildStartPoint), hFile);
			AddNPCNavStartPoint(pNPCNav);	
		}

		pFS->Seek(hFile, header.dwPlayerNavOffset, FILE_BEGIN);
		tagPlayerNavMapBuildStartPoint* pPlayerNav;
		for(i=0; i<header.nNumPlayerNav; i++)
		{
			pPlayerNav = new tagPlayerNavMapBuildStartPoint;
			pFS->Read(pPlayerNav, sizeof(tagPlayerNavMapBuildStartPoint), hFile);
			AddPlayerNavStartPoint(pPlayerNav);	
		}

		pFS->Seek(hFile, header.dwMapDoorOffset, FILE_BEGIN);
		for(i = 0; i < header.nNumMapDoor; i++)
		{
			tagMapDoor dor;
			pFS->Read(&dor, sizeof(tagMapDoor), hFile);

			MapObjEdit* pEdit = new MapObjEdit;
			if(pEdit->Create(dor, this))
			{
				Vector3 pos;
				pos = dor.vPos;
				pEdit->SetWorldPos(pos);			
			}
			else
				delete pEdit;
		}

		pFS->Seek(hFile, header.dwMapCarrierOffset, FILE_BEGIN);
		for(i = 0; i < header.nNumMapCarrier; i++)
		{
			tagMapCarrier car;
			pFS->Read(&car, sizeof(tagMapCarrier), hFile);

			MapObjEdit* pEdit = new MapObjEdit;
			if(pEdit->Create(car, this))
			{
				Vector3 pos(0,0,0);
				pEdit->SetWorldPos(pos);			
			}
			else
				delete pEdit;
		}

		pFS->Seek(hFile, header.dwMapTriggerEffectOffset, FILE_BEGIN);
		for( i=0; i<header.nNumMapTriggerEffect; ++i )
		{
			tagMapTriggerEffect obj;
			pFS->Read(&obj, sizeof(tagMapTriggerEffect), hFile);
			MapObjEdit* pEdit = new MapObjEdit;
			if(pEdit->Create(obj, this))
			{
				Vector3 pos(obj.fPos[0], obj.fPos[1], obj.fPos[2]);
				pEdit->SetWorldPos(pos);			
			}
			else
				delete pEdit;
		}


		
		pFS->Close(hFile);
		return bLoadSucc;
	}

	bool GameMapEditor::WriteEditFile(const TCHAR* szFileName)
	{
		FILE* fp = NULL;
		if(_tfopen_s(&fp, szFileName, _T("wb"))!=0)
			return false;

		tagMapHeader header;
		
		memcpy(header.byDistFog,&m_Fog,sizeof(header.byDistFog));
		memcpy(header.bySunLight,&m_SunLight,sizeof(header.bySunLight));

		_tcscpy_s(header.szSkyBoxTopTex, sizeof(header.szSkyBoxTopTex)/sizeof(TCHAR), m_szSkyBoxTopTex);
		_tcscpy_s(header.szSkyBoxFrontRightTex, sizeof(header.szSkyBoxFrontRightTex)/sizeof(TCHAR), m_szSkyBoxFrontRightTex);
		_tcscpy_s(header.szSkyBoxBackLeftTex, sizeof(header.szSkyBoxBackLeftTex)/sizeof(TCHAR), m_szSkyBoxBackLeftTex);
		header.fSkyBoxXsize = m_fSkyBoxXsize;
		header.fSkyBoxYsize = m_fSkyBoxYsize;
		header.fSkyBoxZsize = m_fSkyBoxZsize;
		header.fSkyBoxOffX = m_fSkyBoxOffX;
		header.fSkyBoxOffY = m_fSkyBoxOffY;
		header.fSkyBoxOffZ = m_fSkyBoxOffZ;
		header.fSunModulus = m_fSunModulus;
		header.fSkyYaw = m_fSkyYaw;
		header.dwGlobalID = m_dwGlobalID;

		memcpy(header.bySkyCol,&m_SkyCol, sizeof(header.bySkyCol));

		header.byRenderSkyShade = ( m_bRenderSkyShade ? 1 : 0 );
		header.dwSkyShadeCol = m_dwSkyShadeCol;
		header.dwDynamicDiffCol = m_dwDynamicDiffCol;
		header.dwDynamicAmbCol = m_dwDynamicAmbCol;
		header.dwDynamicSpecCol = m_dwDynamicSpecCol;

		memcpy(&header.dwMagic, MapObjFileMagic, sizeof(DWORD));
		fwrite(&header, sizeof(header), 1 , fp);

		typedef list<MapObjEdit*> EDIT;
		EDIT::iterator pIter = m_objList.begin();

		header.dwNpcOffset = ftell(fp);
		header.nNumNPC=0;
		header.nNumNPC=0;

		pIter = m_objList.begin();
		for(; pIter != m_objList.end(); ++pIter)
		{
			if((*pIter)->GetType() == MapObjEdit::NPC)
			{
				header.nNumNPC++;
				fwrite((*pIter)->GetMapNPC(), sizeof(tagMapNPC), 1, fp);
			}
		}

		header.dwStaticObjOffset = ftell(fp);
		header.nNumStaticObj=0;
		
		pIter = m_objList.begin();
		for(;pIter != m_objList.end(); ++pIter)
		{
			if((*pIter)->GetType() == MapObjEdit::StaticObj)
			{
				header.nNumStaticObj++;
				fwrite((*pIter)->GetStaticMapObj(), sizeof(tagStaticMapObj), 1, fp);
			}
		}

		//save way point
		header.dwWayPointOffset=ftell(fp);
		header.nNumWayPoint=0;

		list<tagMapWayPoint*>::iterator pWIter = m_WayPointList.begin();
		for(; pWIter != m_WayPointList.end(); ++pWIter)
		{
			header.nNumWayPoint++;
			fwrite((*pWIter), sizeof(tagMapWayPoint), 1, fp);
		}

		//save trigger
		header.dwTriggerOffset=ftell(fp);
		header.nNumTrigger=0;

		list<tagMapTrigger*>::iterator pTIter = m_TriggerList.begin();
		for(; pTIter != m_TriggerList.end(); ++pTIter)
		{
			header.nNumTrigger++;
			tagMapTrigger* pTrigger = (*pTIter);
			FWriteValue(fp, pTrigger->dwObjID);
			FWriteValue(fp, pTrigger->eType);
			FWriteValVector(fp, pTrigger->region);
			FWriteValue(fp, pTrigger->box.max);
			FWriteValue(fp, pTrigger->box.min);
			FWriteValue(fp, pTrigger->fHeight);

			fwrite(pTrigger->szMapName, sizeof(pTrigger->szMapName), 1, fp);
			fwrite(pTrigger->szWayPoint, sizeof(pTrigger->szWayPoint), 1, fp);
			fwrite(pTrigger->szScriptName, sizeof(pTrigger->szScriptName), 1, fp);

			FWriteValue(fp, pTrigger->dwParam);
			FWriteValue(fp, pTrigger->bLock);
			FWriteValue(fp, pTrigger->bFlag);
			FWriteValue(fp, pTrigger->dwQuestSerial);

			fwrite(pTrigger->byReserve, sizeof(pTrigger->byReserve), 1, fp);
		}

		//save sound
		header.dwSoundOffset=ftell(fp);
		header.nNumSound=0;

		list<tagMapSound*>::iterator pSIter = m_SoundList.begin();
		for(; pSIter != m_SoundList.end(); ++pSIter)
		{
			header.nNumSound++;
			fwrite((*pSIter), sizeof(tagMapSound), 1, fp);
		}

		//save pointlight obj
		header.dwPointLightOffset=ftell(fp);
		header.nNumPointLight=0;

		EDIT::iterator iter = m_PointLightList.begin();
		for(; iter!=m_PointLightList.end(); ++iter)
		{
			header.nNumPointLight++;
			fwrite((*iter)->GetPointLgtObj(), sizeof(tagMapPointLight), 1, fp);
		}

		header.dwMapRectOffset = ftell(fp);
		header.nNumMapRect = 0;
		iter = m_pMapRectList.begin();
		for(; iter!=m_pMapRectList.end(); ++iter)
		{
			header.nNumMapRect++;
			//fwrite((*iter)->GetMapRect(), sizeof(tagMapArea), 1, fp);
			const tagMapArea *pMapArea = (*iter)->GetMapRect();
			FWriteValue(fp, pMapArea->dwObjID);
			FWriteValue(fp, pMapArea->eType);
			FWriteValVector(fp, pMapArea->region);
			FWriteValue(fp, pMapArea->box.max);
			FWriteValue(fp, pMapArea->box.min);
			FWriteValue(fp, pMapArea->fHeight);
			FWriteValue(fp, pMapArea->bLock);
			FWriteValue(fp, pMapArea->bFlag);
			FWriteValue(fp, pMapArea->dwMiniMapSize);
			FWriteValue(fp, pMapArea->bDistFog);
			fwrite(pMapArea->byDistFog, sizeof(pMapArea->byDistFog), 1, fp);
			fwrite(pMapArea->byReserve, sizeof(pMapArea->byReserve), 1, fp);
		}

		header.dwPathPointOffset = ftell(fp);
		header.nNumPathPoint = 0;
		iter = m_pPathPointList.begin();
		for(; iter!=m_pPathPointList.end(); ++iter)
		{
			header.nNumPathPoint++;
			fwrite((*iter)->GetPathPoint(), sizeof(tagMapPathPoint), 1, fp);
		}

		header.dwSpawnPointOffset = ftell(fp);
		header.nNumSpawnPoint = 0;
		iter = m_pSpawnPointList.begin();
		for(; iter!=m_pSpawnPointList.end(); ++iter)
		{
			header.nNumSpawnPoint++;
			fwrite((*iter)->GetSpawnPoint(), sizeof(tagMapSpawnPoint), 1, fp);
		}

		header.dwDynamicBlockOffset = ftell(fp);
		header.nNumDynamicBlock=0;
		pIter = m_objList.begin();
		for(;pIter != m_objList.end(); ++pIter)
		{
			if((*pIter)->GetType() == MapObjEdit::DynamicBlock)
			{
				header.nNumDynamicBlock++;
				fwrite((*pIter)->GetDynamicBlock(), sizeof(tagMapDynamicBlock), 1, fp);
			}
		}

		header.dwEventTriggerOffset = ftell(fp);
		header.nNumEventTrigger=0;
		iter = m_pEventTriggerList.begin();
		for(; iter!=m_pEventTriggerList.end(); ++iter)
		{
			header.nNumEventTrigger++;
			fwrite((*iter)->GetEventTrigger(), sizeof(tagMapEventTrigger), 1, fp);
		}

		header.dwNPCNavOffset = ftell(fp);
		header.nNumNPCNav = 0;
		list<tagNPCNavMapBuildStartPoint*>::iterator pNpcNavIter = m_NPCNavStartPointList.begin();
		for(; pNpcNavIter != m_NPCNavStartPointList.end(); ++pNpcNavIter)
		{
			header.nNumNPCNav++;
			fwrite((*pNpcNavIter), sizeof(tagNPCNavMapBuildStartPoint), 1, fp);
		}

		header.dwPlayerNavOffset = ftell(fp);
		header.nNumPlayerNav = 0;
		list<tagPlayerNavMapBuildStartPoint*>::iterator pPlayerNavIter = m_PlayerNavStartPointList.begin();
		for(; pPlayerNavIter != m_PlayerNavStartPointList.end(); ++pPlayerNavIter)
		{
			header.nNumPlayerNav++;
			fwrite((*pPlayerNavIter), sizeof(tagPlayerNavMapBuildStartPoint), 1, fp);
		}

		header.dwMapDoorOffset = ftell(fp);
		header.nNumMapDoor=0;
		pIter = m_objList.begin();
		for(;pIter != m_objList.end(); ++pIter)
		{
			if((*pIter)->GetType() == MapObjEdit::MapDoor)
			{
				header.nNumMapDoor++;
				fwrite((*pIter)->GetMapDoor(), sizeof(tagMapDoor), 1, fp);
			}
		}

		header.dwMapCarrierOffset = ftell(fp);
		header.nNumMapCarrier=0;
		pIter = m_objList.begin();
		for(;pIter != m_objList.end(); ++pIter)
		{
			if((*pIter)->GetType() == MapObjEdit::MapCarrier)
			{
				header.nNumMapCarrier++;
				fwrite((*pIter)->GetMapCarrier(), sizeof(tagMapCarrier), 1, fp);
			}
		}

		header.dwMapTriggerEffectOffset = ftell(fp);
		header.nNumMapTriggerEffect=0;
		pIter = m_objList.begin();
		for(;pIter != m_objList.end(); ++pIter)
		{
			if((*pIter)->GetType() == MapObjEdit::MapTrrigerEffect)
			{
				header.nNumMapTriggerEffect++;
				fwrite((*pIter)->GetMapTriggerEffect(), sizeof(tagMapTriggerEffect), 1, fp);
			}
		}
		
		fseek(fp, 0, SEEK_SET);
		fwrite(&header, sizeof(header), 1, fp);
		fclose(fp);

		// 把区域地图用到的地图区域数据存入xml，避免读mb
		SaveShowAreaData(szFileName);

		return true;
	}

	void GameMapEditor::SaveShowAreaData(LPCTSTR szFileName)
	{
		XMLReader varLocal;
		varLocal.Load(NULL, _T("config\\local.xml"));
		tstring strLocalPath = varLocal.GetString(_T("local_path"));
		Filename strMapPath = szFileName;
		tstring strMapName = strMapPath.NoPath();
		tstring::size_type pos = strMapName.rfind( _T('.') );
		ASSERT( tstring::npos != pos );
		strMapName.erase( pos );
		TCHAR szPath[MAX_PATH];
		_stprintf( szPath, _T("%s\\map\\%s\\show_area.xml"), strLocalPath.c_str(), strMapName.c_str() );
		XMLReader varShowArea;
		varShowArea.Load(NULL, szPath,"id");
		TCHAR szKey[MAX_PATH];
		TCHAR szVal[32];
		bool bNeedToSave = false;
		for(list<MapObjEdit*>::iterator iter = m_pMapRectList.begin(); iter!=m_pMapRectList.end(); ++iter)
		{
			const tagMapArea *pMapArea = (*iter)->GetMapRect();
			if( 0 == pMapArea->dwMiniMapSize )
				continue;

			_sntprintf( szKey, MAX_PATH, _T("box_min_x %d"), pMapArea->dwObjID );
			_sntprintf( szVal, 32, _T("%.2f"), pMapArea->box.min.x );
			varShowArea.Set( szVal, szKey );

			_sntprintf( szKey, MAX_PATH, _T("box_min_y %d"), pMapArea->dwObjID );
			_sntprintf( szVal, 32, _T("%.2f"), pMapArea->box.min.y );
			varShowArea.Set( szVal, szKey );

			_sntprintf( szKey, MAX_PATH, _T("box_min_z %d"), pMapArea->dwObjID );
			_sntprintf( szVal, 32, _T("%.2f"), pMapArea->box.min.z );
			varShowArea.Set( szVal, szKey );

			_sntprintf( szKey, MAX_PATH, _T("box_max_x %d"), pMapArea->dwObjID );
			_sntprintf( szVal, 32, _T("%.2f"), pMapArea->box.max.x );
			varShowArea.Set( szVal, szKey );

			_sntprintf( szKey, MAX_PATH, _T("box_max_y %d"), pMapArea->dwObjID );
			_sntprintf( szVal, 32, _T("%.2f"), pMapArea->box.max.y );
			varShowArea.Set( szVal, szKey );

			_sntprintf( szKey, MAX_PATH, _T("box_max_z %d"), pMapArea->dwObjID );
			_sntprintf( szVal, 32, _T("%.2f"), pMapArea->box.max.z );
			varShowArea.Set( szVal, szKey );

			_sntprintf( szKey, MAX_PATH, _T("minimap_size %d"), pMapArea->dwObjID );
			varShowArea.Set( pMapArea->dwMiniMapSize, szKey );

			bNeedToSave = true;
		}
		if( bNeedToSave )
			varShowArea.Save( szPath, "id" );
	}

	//添加一个导航点，并返回同一组导航点中的位置索引
	void GameMapEditor::AddWayPoint(tagMapWayPoint *pWayPoint,int & nIndexvalue)
	{
		int nIndex=0;
		list<tagMapWayPoint*>::iterator pIter = m_WayPointList.begin();
		for(; pIter != m_WayPointList.end(); ++pIter)
		{
			if(_tcscmp((*pIter)->szName,pWayPoint->szName)==0)
			{
                  nIndex++;
			}
		}
        pWayPoint->byIndex=nIndex;
		m_WayPointList.push_back(pWayPoint);
		nIndexvalue=nIndex;
	}

	tagMapWayPoint* GameMapEditor::FindWayPoint(DWORD dwIndex)
	{
		list<tagMapWayPoint*>::iterator pIter = m_WayPointList.begin();
		for(; pIter != m_WayPointList.end(); ++pIter)
		{
			tagMapWayPoint* pEdit =(*pIter);
			if(pEdit->dwObjID == dwIndex)
			{
				return (*pIter);
			}
		}
		return NULL;
	}

	//clear way point list
	void GameMapEditor::ClearWayPointList()
	{
		list<tagMapWayPoint*>::iterator pIter = m_WayPointList.begin();
		for(; pIter != m_WayPointList.end(); ++pIter)
		{
			tagMapWayPoint* pEdit =(*pIter);
			delete pEdit;
		}
		m_WayPointList.clear();
		m_pSelectWayPoint=NULL;
	}

    //find a way point data
	tagMapWayPoint* GameMapEditor::GetWayPointData(DWORD nObjId)
	{
		list<tagMapWayPoint*>::iterator pIter = m_WayPointList.begin();
		for(; pIter != m_WayPointList.end(); ++pIter)
		{
			tagMapWayPoint* pEdit =(*pIter);
			if(pEdit->dwObjID == nObjId/*_tcscmp(szName,pEdit->szName)==0 && pEdit->byIndex==nIndex*/)
	            return pEdit;	
		}
		return NULL;
	}

	tagMapWayPoint* GameMapEditor::GetWayPointData(const TCHAR* szName, BYTE byIndex)
	{
		list<tagMapWayPoint*>::iterator pIter = m_WayPointList.begin();
		for(; pIter != m_WayPointList.end(); ++pIter)
		{
			tagMapWayPoint* pEdit =(*pIter);
			if(_tcscmp(szName,pEdit->szName)==0 && pEdit->byIndex==byIndex)
				return pEdit;	
		}
		return NULL;
	}

	// already exist same name?
	bool GameMapEditor::IsExistWayPointWithSameName(TCHAR* szName)
	{
		list<tagMapWayPoint*>::iterator pIter = m_WayPointList.begin();
		for(; pIter != m_WayPointList.end(); ++pIter)
		{
			tagMapWayPoint* pEdit =(*pIter);
			if(_tcscmp(szName,pEdit->szName)==0 )
				return true;	
		}
		return false;
	}

	//得到地形中一点所对应的格子坐标，且格子z坐标已经变换
	void GameMapEditor::GetTerrainTileXZ(Vector3 &pos,POINT &tileXZ)
	{
		const Heightmap *pHeightmap=m_pTerrain->GetHeightmap();
		tileXZ=pHeightmap->World2Tile(pos);	
	}

	//pick way point
	void  GameMapEditor::PickWayPoint(UINT x, UINT y)
	{
		if(m_pCamera==NULL)
			return;
		Ray ray;
		m_pCamera->GetMouseRay(x,y,ray);
		m_pSelectWayPoint=NULL;
		list<tagMapWayPoint*>::iterator pIter = m_WayPointList.begin();
		for(; pIter != m_WayPointList.end(); ++pIter)
		{
			tagMapWayPoint* pEdit =(*pIter);
			const Heightmap *pHeightmap=m_pTerrain->GetHeightmap();
			Cool3D::AABBox box;
			box.max=Vector3(pEdit->vPos.x,pEdit->vPos.y,pEdit->vPos.z)
				+Vector3(pEdit->vRange.x, pEdit->vRange.y*2, pEdit->vRange.z);
			box.min=Vector3(pEdit->vPos.x,pEdit->vPos.y,pEdit->vPos.z)
				-Vector3(pEdit->vRange.x, 0, pEdit->vRange.z);
			TResult r=ray.Intersect(box);
			if(r.first && r.second <FLOAT_MAX)
			{
				m_pSelectWayPoint=(*pIter);
				break;
			}	
		}
	}

	//delete a waypoint
	void GameMapEditor::DeleteWayPoint(DWORD dwObjID/*TCHAR* szName,int nIndex*/)
	{
		list<tagMapWayPoint*>::iterator pIter = m_WayPointList.begin();
		for(; pIter != m_WayPointList.end(); ++pIter)
		{
			tagMapWayPoint* pEdit =(*pIter);
			if(pEdit->dwObjID == dwObjID/*_tcscmp(pEdit->szName,szName)==0 && pEdit->byIndex==nIndex*/)
			{
				if(pEdit==m_pSelectWayPoint) m_pSelectWayPoint=NULL;
				delete pEdit;
				m_WayPointList.erase(pIter);
				break;
			}
		}
	}

	//直接修改特定导航点属性
	void GameMapEditor::ModifyWayPointProp(tagMapWayPoint WayPoint)
	{
		list<tagMapWayPoint*>::iterator pIter = m_WayPointList.begin();
		for(; pIter != m_WayPointList.end(); ++pIter)
		{
			tagMapWayPoint* pEdit =(*pIter);
			if(_tcscmp(pEdit->szName,WayPoint.szName)==0 && pEdit->byIndex==WayPoint.byIndex)
			{
				pEdit->byIndex = WayPoint.byIndex;
				pEdit->vPos = WayPoint.vPos;
				pEdit->vRange = WayPoint.vRange;
				pEdit->fSuspend = WayPoint.fSuspend;
				break;
			}
		}
	}
	//将当前导航点属性完全改变
	void GameMapEditor::ModifyWayPointProp(DWORD dwOriID, TCHAR* szOldName,int nOldIndex,tagMapWayPoint WayPoint)
	{
		//删掉以前的属性
		tagMapWayPoint* pEdit;
		list<tagMapWayPoint*>::iterator pIter = m_WayPointList.begin();

		for(; pIter != m_WayPointList.end(); ++pIter)
		{
			pEdit =(*pIter);
			if(_tcscmp(pEdit->szName,szOldName)==0 && pEdit->byIndex==nOldIndex)
			{
				m_WayPointList.erase(pIter);
				break;
			}
		}
		//修改新属性
	    _tcscpy_s(pEdit->szName,sizeof(pEdit->szName)/sizeof(TCHAR),WayPoint.szName);
		pEdit->byIndex=WayPoint.byIndex;
		pEdit->vPos = WayPoint.vPos;
		pEdit->vRange = WayPoint.vRange;
		pEdit->dwObjID = dwOriID;
		pEdit->fSuspend = WayPoint.fSuspend;
		
		//加入，找到同名组的第一个元素
		int flag=0,num=0;
		list<tagMapWayPoint*>::iterator pPreIter = m_WayPointList.begin();
		pIter = m_WayPointList.begin();
		for(; pIter != m_WayPointList.end(); ++pIter,++pPreIter)
		{
			if(_tcscmp(pEdit->szName,(*pIter)->szName)==0)
			{
                flag=1;
				break;
			}
			num++;
		}
		//如果没有找到合适位置，则添加到list最后
		if(flag==0)
		{
			m_WayPointList.push_back(pEdit);
			return;
		}
		//如果需要添加在list的第1个位置
		if(num==0)
		{
			if(pEdit->byIndex<(*pIter)->byIndex)
			{
				m_WayPointList.push_front(pEdit);
				return;
			}
		}
		//加入同名组的第1个位置
		if(pEdit->byIndex<(*pIter)->byIndex && _tcscmp(pEdit->szName,(*pIter)->szName)==0)
		{
			m_WayPointList.insert(pIter,pEdit);
		}

		//添加在同名组中间位置
		++pIter;
		for(; pIter != m_WayPointList.end(); ++pIter,++pPreIter)
		{
			if(_tcscmp((*pPreIter)->szName,(*pIter)->szName)==0)
			{
				if(pEdit->byIndex>(*pPreIter)->byIndex && pEdit->byIndex<(*pIter)->byIndex)
				{
					m_WayPointList.insert(pIter,pEdit);
					return;
				}
			}
			else
			{
				m_WayPointList.insert(pIter,pEdit);
				return;
			}		
		}
        //加入同名组的最后位置
		if(pEdit->byIndex>(*pPreIter)->byIndex && _tcscmp(pEdit->szName,(*pPreIter)->szName)==0)
		{
			m_WayPointList.push_back(pEdit);
		}

	}

	//add trigger
	void GameMapEditor::AddTrigger(tagMapTrigger *pTrigger)
	{
		m_TriggerList.push_back(pTrigger);
	}

	AABBox GameMapEditor::CalTriggerAABBox()
	{
		AABBox box;
		Vector3 max = m_vTriggerPos[0], min = m_vTriggerPos[0];
		for(int i=1; i<m_nCurTriggerNum; i++)
		{
			//最大点
			if(m_vTriggerPos[i].x>=max.x)
				max.x = m_vTriggerPos[i].x;
			if(m_vTriggerPos[i].y>=max.y)
				max.y = m_vTriggerPos[i].y;
			if(m_vTriggerPos[i].z>=max.z)
				max.z = m_vTriggerPos[i].z;
			
			//最小点
			if(m_vTriggerPos[i].x<=min.x)
				min.x = m_vTriggerPos[i].x;
			if(m_vTriggerPos[i].y<=min.y)
				min.y = m_vTriggerPos[i].y;
			if(m_vTriggerPos[i].z<=min.z)
				min.z = m_vTriggerPos[i].z;	
		}
		box.max = max;
		box.min = min;
		return box;
	}

	//重新计算触发器的AABBOX
	void GameMapEditor::ReCalTriggerAABBox(tagMapTrigger* pTri)
	{
		AABBox box;
		Vector3 max = pTri->region[0], min = pTri->region[0];
		for(int i=1; i<(int)pTri->region.size(); i++)
		{
			if( pTri->region[i].x <=0 
				&& pTri->region[i].y <=0
				&& pTri->region[i].z <=0)
				continue;
			//最大点
			if(pTri->region[i].x>=max.x)
				max.x = pTri->region[i].x;
			if(pTri->region[i].y>=max.y)
				max.y = pTri->region[i].y;
			if(pTri->region[i].z>=max.z)
				max.z = pTri->region[i].z;

			//最小点
			if(pTri->region[i].x<=min.x)
				min.x = pTri->region[i].x;
			if(pTri->region[i].y<=min.y)
				min.y = pTri->region[i].y;
			if(pTri->region[i].z<=min.z)
				min.z = pTri->region[i].z;	
		}
		box.max = max;
		box.min = min;
		pTri->box = box;
		pTri->box.max.y = pTri->fHeight + pTri->box.min.y;
	}

	//重新计算触发器的AABBOX
	void GameMapEditor::ReCalMapAreaAABBox(tagMapArea* pArea)
	{
		AABBox box;
		Vector3 max = pArea->region[0], min = pArea->region[0];
		for(int i=1; i<(int)pArea->region.size(); i++)
		{
			if( pArea->region[i].x <=0 
				&& pArea->region[i].y <=0
				&& pArea->region[i].z <=0)
				continue;
			//最大点
			if(pArea->region[i].x>=max.x)
				max.x = pArea->region[i].x;
			if(pArea->region[i].y>=max.y)
				max.y = pArea->region[i].y;
			if(pArea->region[i].z>=max.z)
				max.z = pArea->region[i].z;

			//最小点
			if(pArea->region[i].x<=min.x)
				min.x = pArea->region[i].x;
			if(pArea->region[i].y<=min.y)
				min.y = pArea->region[i].y;
			if(pArea->region[i].z<=min.z)
				min.z = pArea->region[i].z;	
		}
		box.max = max;
		box.min = min;
		pArea->box = box;
		pArea->box.max.y = pArea->fHeight + pArea->box.min.y;
	}

	void GameMapEditor::ClearTriggerPos()
	{
		m_vTriggerPos.clear();
		/*for(int i=0; i<(int)m_vTriggerPos.size(); i++)
		{
			m_vTriggerPos[i] = Vector3(0, 0, 0);
		}*/
	}

	void GameMapEditor::ClearMapAreaPos()
	{
		m_vMapAreaPos.clear();
		/*for(int i=0; i<(int)m_vMapAreaPos.size(); i++)
		{
			m_vMapAreaPos[i] = Vector3(0, 0, 0);
		}*/
	}

	//计算触发器的AABBOX
	AABBox GameMapEditor::CalMapAreaAABBox()
	{
		AABBox box;
		Vector3 max = m_vMapAreaPos[0], min = m_vMapAreaPos[0];
		for(int i=1; i<m_nCurMapAreaNum; i++)
		{
			//最大点
			if(m_vMapAreaPos[i].x>=max.x)
				max.x = m_vMapAreaPos[i].x;
			if(m_vMapAreaPos[i].y>=max.y)
				max.y = m_vMapAreaPos[i].y;
			if(m_vMapAreaPos[i].z>=max.z)
				max.z = m_vMapAreaPos[i].z;

			//最小点
			if(m_vMapAreaPos[i].x<=min.x)
				min.x = m_vMapAreaPos[i].x;
			if(m_vMapAreaPos[i].y<=min.y)
				min.y = m_vMapAreaPos[i].y;
			if(m_vMapAreaPos[i].z<=min.z)
				min.z = m_vMapAreaPos[i].z;	
		}
		box.max = max;
		box.min = min;
		return box;
	}

	//modify trigger
	void GameMapEditor::ModifyTriggerProp(tagMapTrigger Trigger)
	{
		tagMapTrigger* pTrigger;
		list<tagMapTrigger*>::iterator pIter = m_TriggerList.begin();

		for(; pIter != m_TriggerList.end(); ++pIter)
		{
			pTrigger =(*pIter);
			if(Trigger.dwObjID==pTrigger->dwObjID)
			{
				*pTrigger = Trigger;
				break;
			}
		}
	}

	//delete a trigger
	void GameMapEditor::DeleteTrigger(DWORD dwObjID)
	{
		tagMapTrigger* pTrigger;
		list<tagMapTrigger*>::iterator pIter = m_TriggerList.begin();

		for(; pIter != m_TriggerList.end(); ++pIter)
		{
			pTrigger =(*pIter);
			if(dwObjID==pTrigger->dwObjID)
			{
				if(m_pSelectTrigger==pTrigger) m_pSelectTrigger=NULL;
				delete pTrigger;
				m_TriggerList.erase(pIter);
				break;
			}
		}
	}

	void GameMapEditor::DeleteTrigger(int nIndex)
	{
		tagMapTrigger* pTrigger;
		list<tagMapTrigger*>::iterator pIter = m_TriggerList.begin();
        int num=0;
		for(; pIter != m_TriggerList.end(); ++pIter)
		{
			pTrigger =(*pIter);
			if(num==nIndex)
			{
				if(m_pSelectTrigger==pTrigger) m_pSelectTrigger=NULL;
				delete pTrigger;
				m_TriggerList.erase(pIter);
				break;
			}
			num++;
		}
	}
	
	//clear trigger list
	void GameMapEditor::ClearTriggerList()
	{
		list<tagMapTrigger*>::iterator pIter = m_TriggerList.begin();
		for(; pIter != m_TriggerList.end(); ++pIter)
		{
			tagMapTrigger* pEdit =(*pIter);
			delete pEdit;
		}
		m_TriggerList.clear();
		m_pSelectTrigger=NULL;
	}

	//pick trigger
	void  GameMapEditor::PickTrigger(UINT x, UINT y)
	{
		m_pSelectTrigger=NULL;
		if(m_pCamera==NULL)
			return;
		Ray ray;
		m_pCamera->GetMouseRay(x,y,ray);

		list<tagMapTrigger*>::iterator pIter = m_TriggerList.begin();
		for(; pIter != m_TriggerList.end(); ++pIter)
		{
			tagMapTrigger* pEdit =(*pIter);
			Cool3D::AABBox box;
			box.max		= pEdit->box.max;
			box.max.y	= pEdit->fHeight;
			box.min		= pEdit->box.min;
			TResult r=ray.Intersect(box);
			if(r.first && r.second <FLOAT_MAX)
			{
				m_pSelectTrigger=(*pIter);
				break;
			}	
		}
	}

	//拾取触发器的某一个点
	void GameMapEditor::PickTriggerPoint(UINT x, UINT y, bool& bHave)
	{
		m_pSelectTrigger=NULL;
		if(m_pCamera==NULL)
			return;
		Ray ray;
		m_pCamera->GetMouseRay(x,y,ray);
		bHave = false;
		list<tagMapTrigger*>::iterator pIter = m_TriggerList.begin();
		for(; pIter != m_TriggerList.end(); ++pIter)
		{
			tagMapTrigger* pEdit =(*pIter);
			for(int i=0; i<(int)pEdit->region.size(); i++)
			{
				Cool3D::AABBox box;
				box.max		= pEdit->region[i]+Vector3(20, 20, 20);
				box.min		= pEdit->region[i]-Vector3(20, 20, 20);
				TResult r=ray.Intersect(box);
				if(r.first && r.second <FLOAT_MAX)
				{
					m_pSelectTrigger=(*pIter);
					m_bPickTriggerPoint = true;
					m_nTriggerNum = i;
					bHave = true;
					break;
				}	
			}
		}
	}

	//通过ID查找触发器
	tagMapTrigger* GameMapEditor::GetTriggerData(DWORD dwObjID)
	{
		list<tagMapTrigger*>::iterator pIter = m_TriggerList.begin();
		for(; pIter != m_TriggerList.end(); ++pIter)
		{
			tagMapTrigger* pEdit =(*pIter);
			if(pEdit->dwObjID==dwObjID)
			{
				return pEdit;
			}	
		}
		return NULL;
	}

	//新增加一个地图区域
	void GameMapEditor::AddMapRect(MapObjEdit* pMapRect)
	{
		m_pMapRectList.push_back(pMapRect);
	}

	//删除一个地图区域
	void GameMapEditor::DeleteMapRect(tagMapArea * pMapRect)
	{
		list<MapObjEdit*>::iterator pIter = m_pMapRectList.begin();
		for(; pIter != m_pMapRectList.end(); ++pIter)
		{
			tagMapArea* pEdit =(tagMapArea*)(*pIter)->GetMapRect();
			if(pEdit==pMapRect)
			{
				if(m_pSelectMapRect==(*pIter)) m_pSelectMapRect=NULL;
				delete pEdit;
				m_pMapRectList.erase(pIter);
				break;
			}	
		}
	}

	//通过索引删除一个地图区域
	void GameMapEditor::DeleteMapRect(DWORD dwIndex)
	{
		list<MapObjEdit*>::iterator pIter = m_pMapRectList.begin();
		for(; pIter != m_pMapRectList.end(); ++pIter)
		{
			MapObjEdit* pEdit=(*pIter);
			if(pEdit->GetMapID() == dwIndex)
			{
				if(pEdit==m_pSelectMapRect) m_pSelectMapRect=NULL;
				delete pEdit;
				m_pMapRectList.erase(pIter);
				break;
			}
		}
	}

	//清空地图区域列表
	void GameMapEditor::ClearMapRectList()
	{	
		list<MapObjEdit*>::iterator pIter = m_pMapRectList.begin();
		for(; pIter != m_pMapRectList.end(); ++pIter)
		{
			MapObjEdit* pEdit =(*pIter);
			delete pEdit;
		}
		m_pMapRectList.clear();
		m_pSelectMapRect = NULL;
	}
	//根据索引得到地图区域
	MapObjEdit* GameMapEditor::FindMapRect(DWORD dwIndex)
	{	
		list<MapObjEdit*>::iterator pIter = m_pMapRectList.begin();
		for(; pIter != m_pMapRectList.end(); ++pIter)
		{
			const tagMapArea* pEdit =(*pIter)->GetMapRect();
			if(pEdit->dwObjID == dwIndex)
			{
				return (*pIter);
			}
		}
		return NULL;
	}

	//得到拾取的地图区域
	void GameMapEditor::PickMapRect(UINT x, UINT y)
	{
		m_pSelectMapRect = NULL;
		if(m_pCamera==NULL)
			return;
		Ray ray;
		m_pCamera->GetMouseRay(x,y,ray);

		list<MapObjEdit*>::iterator pIter = m_pMapRectList.begin();
		for(; pIter!=m_pMapRectList.end(); ++pIter)
		{
			tagMapArea* pEdit = (tagMapArea*)(*pIter)->GetMapRect();

			const Heightmap *pHeightmap=m_pTerrain->GetHeightmap();
			Cool3D::AABBox box;
			box.max		= pEdit->box.max;
			box.max.y	= pEdit->fHeight;
			box.min		= pEdit->box.min;
			TResult r=ray.Intersect(box);
			if(r.first && r.second <FLOAT_MAX)
			{
				m_pSelectMapRect=(*pIter);
				break;
			}	
		}
	}

	//拾取地图区域的某一个点
	void GameMapEditor::PickMapAreaPoint(UINT x, UINT y, bool& bHave)
	{
		m_pSelectMapRect=NULL;
		if(m_pCamera==NULL)
			return;
		Ray ray;
		m_pCamera->GetMouseRay(x,y,ray);
		bHave = false;
		list<MapObjEdit*>::iterator pIter = m_pMapRectList.begin();
		for(; pIter != m_pMapRectList.end(); ++pIter)
		{
			tagMapArea* pEdit = (tagMapArea*)(*pIter)->GetMapRect();
			for(int i=0; i<(int)pEdit->region.size(); i++)
			{
				Cool3D::AABBox box;
				box.max		= pEdit->region[i]+Vector3(20, 20, 20);
				box.min		= pEdit->region[i]-Vector3(20, 20, 20);
				TResult r=ray.Intersect(box);
				if(r.first && r.second <FLOAT_MAX)
				{
					m_pSelectMapRect = (*pIter);
					m_bPickMapAreaPoint = true;
					m_nMapAreaNum = i;
					bHave = true;
					break;
				}	
			}
		}		
	}

	//新增加一个寻路导航点
	void GameMapEditor::AddPathPoint(MapObjEdit* pEdit)
	{
		m_pPathPointList.push_back(pEdit);
	}

	//删除一个寻路导航点
	void GameMapEditor::DeletePathPoint(tagMapPathPoint * pPathPoint)
	{
		list<MapObjEdit*>::iterator pIter = m_pPathPointList.begin();
		for(; pIter != m_pPathPointList.end(); ++pIter)
		{
			tagMapPathPoint* pEdit =(tagMapPathPoint*)(*pIter)->GetPathPoint();
			if(pEdit==pPathPoint)
			{
				if(m_pSelectPathPoint==(*pIter)) m_pSelectPathPoint=NULL;
				delete pEdit;
				m_pPathPointList.erase(pIter);
				break;
			}	
		}
	}

	//通过索引删除一个寻路导航点
	void GameMapEditor::DeletePathPoint(DWORD dwIndex)
	{
		list<MapObjEdit*>::iterator pIter = m_pPathPointList.begin();
		for(; pIter != m_pPathPointList.end(); ++pIter)
		{
			MapObjEdit* pEdit=(*pIter);
			if(pEdit->GetMapID() == dwIndex)
			{
				if(pEdit==m_pSelectPathPoint) m_pSelectPathPoint=NULL;
				delete pEdit;
				m_pPathPointList.erase(pIter);
				break;
			}
		}
	}

	//清空寻路导航点
	void GameMapEditor::ClearPathPointList()
	{
		list<MapObjEdit*>::iterator pIter = m_pPathPointList.begin();
		for(; pIter != m_pPathPointList.end(); ++pIter)
		{
			MapObjEdit* pEdit =(*pIter);
			delete pEdit;
		}
		m_pPathPointList.clear();
		m_pSelectPathPoint = NULL;
	}

	//根据索引得到寻路导航点
	MapObjEdit* GameMapEditor::FindPathPoint(DWORD dwIndex)
	{
		list<MapObjEdit*>::iterator pIter = m_pPathPointList.begin();
		for(; pIter != m_pPathPointList.end(); ++pIter)
		{
			const tagMapPathPoint* pEdit =(*pIter)->GetPathPoint();
			if(pEdit->dwObjID == dwIndex)
			{
				return (*pIter);
			}
		}
		return NULL;
	}

	//拾取寻路导航点
	void GameMapEditor::PickPathPoint(UINT x, UINT y)
	{
		if(m_pCamera==NULL)
			return;
		Ray ray;
		m_pCamera->GetMouseRay(x,y,ray);
		m_pSelectPathPoint=NULL;
		list<MapObjEdit*>::iterator pIter = m_pPathPointList.begin();
		for(; pIter != m_pPathPointList.end(); ++pIter)
		{
			tagMapPathPoint* pEdit =(tagMapPathPoint*)(*pIter)->GetPathPoint();
			Cool3D::AABBox box;
			box.max=Vector3(pEdit->vPos.x,pEdit->vPos.y,pEdit->vPos.z)
				+Vector3(DIS_PATHPOINT_XZ, DIS_PATHPOINT_Y, DIS_PATHPOINT_XZ);
			box.min=Vector3(pEdit->vPos.x,pEdit->vPos.y,pEdit->vPos.z)
				-Vector3(DIS_PATHPOINT_XZ, DIS_PATHPOINT_Y, DIS_PATHPOINT_XZ);
			TResult r=ray.Intersect(box);
			if(r.first && r.second <FLOAT_MAX)
			{
				m_pSelectPathPoint=(*pIter);
				break;
			}	
		}	
	}
	
	//添加一个事件触发器
	void GameMapEditor::AddEventTrigger(MapObjEdit* pEdit)
	{
		m_pEventTriggerList.push_back(pEdit);
	}

	//删除一个事件触发器
	void GameMapEditor::DeleteEventTrigger(tagMapEventTrigger* pEventTrigger)
	{
		list<MapObjEdit*>::iterator pIter = m_pEventTriggerList.begin();
		for(; pIter != m_pEventTriggerList.end(); ++pIter)
		{
			const tagMapEventTrigger* pEdit =(*pIter)->GetEventTrigger();
			if(pEdit==pEventTrigger)
			{
				delete pEdit;
				m_pEventTriggerList.erase(pIter);
				break;
			}	
		}
	}

	//通过索引删除一个事件触发器
	void GameMapEditor::DeleteEventTrigger(DWORD dwIndex)
	{
		list<MapObjEdit*>::iterator pIter = m_pEventTriggerList.begin();
		for(; pIter != m_pEventTriggerList.end(); ++pIter)
		{
			MapObjEdit* pEdit=(*pIter);
			if(pEdit->GetMapID() == dwIndex)
			{
				delete pEdit;
				m_pEventTriggerList.erase(pIter);
				break;
			}
		}
	}
	//清空事件触发器
	void GameMapEditor::ClearEventTrigger()
	{
		list<MapObjEdit*>::iterator pIter = m_pEventTriggerList.begin();
		for(; pIter != m_pEventTriggerList.end(); ++pIter)
		{
			MapObjEdit* pEdit =(*pIter);
			delete pEdit;
		}
		m_pEventTriggerList.clear();
	}
	//通过索引得到事件触发器
	MapObjEdit* GameMapEditor::FindEventTrigger(DWORD dwIndex)
	{
		list<MapObjEdit*>::iterator pIter = m_pEventTriggerList.begin();
		for(; pIter != m_pEventTriggerList.end(); ++pIter)
		{
			const tagMapEventTrigger* pEdit = (*pIter)->GetEventTrigger();
			if(pEdit->dwObjID == dwIndex)
			{
				return (*pIter);
			}
		}
		return NULL;
	}

	//--NPC格子导航图生成起点
	void GameMapEditor::AddNPCNavStartPoint(tagNPCNavMapBuildStartPoint* pNPCNav)
	{
		m_NPCNavStartPointList.push_back(pNPCNav);
	}
	void GameMapEditor::DeleteNPCNavStartPoint(tagNPCNavMapBuildStartPoint* pNPCNav)
	{
		list<tagNPCNavMapBuildStartPoint*>::iterator pIter = m_NPCNavStartPointList.begin();
		for(; pIter != m_NPCNavStartPointList.end(); ++pIter)
		{
			tagNPCNavMapBuildStartPoint* pEdit = (*pIter);
			if(pEdit==pNPCNav)
			{
				if(m_pSelectNPCNavStartPoint==pEdit) m_pSelectNPCNavStartPoint=NULL;
				delete pEdit;
				m_NPCNavStartPointList.erase(pIter);
				break;
			}	
		}
	}
	void GameMapEditor::DeleteNPCNavStartPoint(DWORD dwIndex)
	{
		list<tagNPCNavMapBuildStartPoint*>::iterator pIter = m_NPCNavStartPointList.begin();
		for(; pIter != m_NPCNavStartPointList.end(); ++pIter)
		{
			tagNPCNavMapBuildStartPoint* pEdit= (*pIter);
			if(pEdit->dwObjID == dwIndex)
			{
				if(pEdit==m_pSelectNPCNavStartPoint) m_pSelectNPCNavStartPoint=NULL;
				delete pEdit;
				m_NPCNavStartPointList.erase(pIter);
				break;
			}
		}
	}
	void GameMapEditor::ClearNPCNavStartPoint()
	{
		list<tagNPCNavMapBuildStartPoint*>::iterator pIter = m_NPCNavStartPointList.begin();
		for(; pIter != m_NPCNavStartPointList.end(); ++pIter)
		{
			tagNPCNavMapBuildStartPoint* pEdit =(*pIter);
			delete pEdit;
		}
		m_NPCNavStartPointList.clear();
		m_pSelectNPCNavStartPoint = NULL;
	}
	tagNPCNavMapBuildStartPoint* GameMapEditor::FindNPCNavStartPoint(DWORD dwIndex)
	{
		list<tagNPCNavMapBuildStartPoint*>::iterator pIter = m_NPCNavStartPointList.begin();
		for(; pIter != m_NPCNavStartPointList.end(); ++pIter)
		{
			tagNPCNavMapBuildStartPoint* pEdit =(*pIter);
			if(pEdit->dwObjID == dwIndex)
			{
				return (*pIter);
			}
		}
		return NULL;
	}
	void GameMapEditor::PickNPCNavStartPoint(UINT x, UINT y)
	{
		if(m_pCamera==NULL)
			return;
		Ray ray;
		m_pCamera->GetMouseRay(x,y,ray);
		m_pSelectNPCNavStartPoint=NULL;
		list<tagNPCNavMapBuildStartPoint*>::iterator pIter = m_NPCNavStartPointList.begin();
		for(; pIter != m_NPCNavStartPointList.end(); ++pIter)
		{
			tagNPCNavMapBuildStartPoint* pEdit =(*pIter);
			Cool3D::AABBox box;
			box.max=Vector3(pEdit->vPos.x,pEdit->vPos.y,pEdit->vPos.z)
				+Vector3(DIS_NPCNAVSTARTPOINT_PICK, DIS_NPCNAVSTARTPOINT_PICK*2, DIS_NPCNAVSTARTPOINT_PICK);
			box.min=Vector3(pEdit->vPos.x,pEdit->vPos.y,pEdit->vPos.z)
				-Vector3(DIS_NPCNAVSTARTPOINT_PICK, 0, DIS_NPCNAVSTARTPOINT_PICK);
			TResult r=ray.Intersect(box);
			if(r.first && r.second <FLOAT_MAX)
			{
				m_pSelectNPCNavStartPoint=(*pIter);
				break;
			}	
		}	
	}

	//--玩家格子导航图生成起点
	void GameMapEditor::AddPlayerNavStartPoint(tagPlayerNavMapBuildStartPoint* pPlayerNav)
	{
		m_PlayerNavStartPointList.push_back(pPlayerNav);
	}
	void GameMapEditor::DeletePlayerNavStartPoint(tagPlayerNavMapBuildStartPoint* pNPCNav)
	{
		list<tagPlayerNavMapBuildStartPoint*>::iterator pIter = m_PlayerNavStartPointList.begin();
		for(; pIter != m_PlayerNavStartPointList.end(); ++pIter)
		{
			tagPlayerNavMapBuildStartPoint* pEdit = (*pIter);
			if(pEdit==pNPCNav)
			{
				if(m_pSelectPlayerNavStartPoint==pEdit) m_pSelectPlayerNavStartPoint=NULL;
				delete pEdit;
				m_PlayerNavStartPointList.erase(pIter);
				break;
			}	
		}
	}
	void GameMapEditor::DeletePlayerNavStartPoint(DWORD dwIndex)
	{
		list<tagPlayerNavMapBuildStartPoint*>::iterator pIter = m_PlayerNavStartPointList.begin();
		for(; pIter != m_PlayerNavStartPointList.end(); ++pIter)
		{
			tagPlayerNavMapBuildStartPoint* pEdit= (*pIter);
			if(pEdit->dwObjID == dwIndex)
			{
				if(pEdit==m_pSelectPlayerNavStartPoint) m_pSelectPlayerNavStartPoint=NULL;
				delete pEdit;
				m_PlayerNavStartPointList.erase(pIter);
				break;
			}
		}
	}
	void GameMapEditor::ClearPlayerNavStartPoint()
	{
		list<tagPlayerNavMapBuildStartPoint*>::iterator pIter = m_PlayerNavStartPointList.begin();
		for(; pIter != m_PlayerNavStartPointList.end(); ++pIter)
		{
			tagPlayerNavMapBuildStartPoint* pEdit =(*pIter);
			delete pEdit;
		}
		m_PlayerNavStartPointList.clear();
		m_pSelectPlayerNavStartPoint = NULL;
	}
	tagPlayerNavMapBuildStartPoint* GameMapEditor::FindPlayerNavStartPoint(DWORD dwIndex)
	{
		list<tagPlayerNavMapBuildStartPoint*>::iterator pIter = m_PlayerNavStartPointList.begin();
		for(; pIter != m_PlayerNavStartPointList.end(); ++pIter)
		{
			tagPlayerNavMapBuildStartPoint* pEdit =(*pIter);
			if(pEdit->dwObjID == dwIndex)
			{
				return (*pIter);
			}
		}
		return NULL;
	}
	void GameMapEditor::PickPlayerNavStartPoint(UINT x, UINT y)
	{
		if(m_pCamera==NULL)
			return;
		Ray ray;
		m_pCamera->GetMouseRay(x,y,ray);
		m_pSelectPlayerNavStartPoint=NULL;
		list<tagPlayerNavMapBuildStartPoint*>::iterator pIter = m_PlayerNavStartPointList.begin();
		for(; pIter != m_PlayerNavStartPointList.end(); ++pIter)
		{
			tagPlayerNavMapBuildStartPoint* pEdit =(*pIter);
			Cool3D::AABBox box;
			box.max=Vector3(pEdit->vPos.x,pEdit->vPos.y,pEdit->vPos.z)
				+Vector3(DIS_NPCNAVSTARTPOINT_PICK, DIS_NPCNAVSTARTPOINT_PICK*2, DIS_NPCNAVSTARTPOINT_PICK);
			box.min=Vector3(pEdit->vPos.x,pEdit->vPos.y,pEdit->vPos.z)
				-Vector3(DIS_NPCNAVSTARTPOINT_PICK, 0, DIS_NPCNAVSTARTPOINT_PICK);
			TResult r=ray.Intersect(box);
			if(r.first && r.second <FLOAT_MAX)
			{
				m_pSelectPlayerNavStartPoint=(*pIter);
				break;
			}	
		}	
	}


	//--刷怪点相关
	void GameMapEditor::AddSpawnPoint(MapObjEdit* pEdit)
	{
		m_pSpawnPointList.push_back(pEdit);
	}	

	//删除一个刷怪点
	void GameMapEditor::DeleteSpawnPoint(tagMapSpawnPoint* pSpawnPoint)
	{
		list<MapObjEdit*>::iterator pIter = m_pSpawnPointList.begin();
		for(; pIter != m_pSpawnPointList.end(); ++pIter)
		{
			tagMapSpawnPoint* pEdit =(tagMapSpawnPoint*)(*pIter)->GetSpawnPoint();
			if(pEdit==pSpawnPoint)
			{
				if(m_pSelectSpawnPoint==(*pIter)) m_pSelectSpawnPoint=NULL;
				delete pEdit;
				m_pSpawnPointList.erase(pIter);
				break;
			}	
		}
	}

	//通过索引删除一个刷怪点
	void GameMapEditor::DeleteSpawnPoint(DWORD dwIndex)
	{
		list<MapObjEdit*>::iterator pIter = m_pSpawnPointList.begin();
		for(; pIter != m_pSpawnPointList.end(); ++pIter)
		{
			MapObjEdit* pEdit=(*pIter);
			if(pEdit->GetMapID() == dwIndex)
			{
				if(pEdit==m_pSelectSpawnPoint) m_pSelectSpawnPoint=NULL;
				delete pEdit;
				m_pSpawnPointList.erase(pIter);
				break;
			}
		}
	}

	//清空刷怪点列表
	void GameMapEditor::ClearSpawnPointList()
	{
		list<MapObjEdit*>::iterator pIter = m_pSpawnPointList.begin();
		for(; pIter != m_pSpawnPointList.end(); ++pIter)
		{
			MapObjEdit* pEdit =(*pIter);
			delete pEdit;
		}
		m_pSpawnPointList.clear();
		m_pSelectSpawnPoint = NULL;
	}

	//通过索引得到刷怪点
	MapObjEdit* GameMapEditor::FindSpawnPoint(DWORD dwIndex)
	{
		list<MapObjEdit*>::iterator pIter = m_pSpawnPointList.begin();
		for(; pIter != m_pSpawnPointList.end(); ++pIter)
		{
			const tagMapSpawnPoint* pEdit =(*pIter)->GetSpawnPoint();
			if(pEdit->dwObjID == dwIndex)
			{
				return (*pIter);
			}
		}
		return NULL;
	}

	//拾取刷怪点
	void GameMapEditor::PickSpawnPoint(UINT x, UINT y)
	{
		if(m_pCamera==NULL)
			return;
		Ray ray;
		m_pCamera->GetMouseRay(x,y,ray);
		m_pSelectSpawnPoint=NULL;
		list<MapObjEdit*>::iterator pIter = m_pSpawnPointList.begin();
		for(; pIter != m_pSpawnPointList.end(); ++pIter)
		{
			tagMapSpawnPoint* pEdit =(tagMapSpawnPoint*)(*pIter)->GetSpawnPoint();
			Cool3D::AABBox box;
			box.max=Vector3(pEdit->vPos.x,pEdit->vPos.y,pEdit->vPos.z)
				+Vector3(DIS_SPAWNPOINT_XZ, DIS_SPAWNPOINT_Y*2, DIS_SPAWNPOINT_XZ);
			box.min=Vector3(pEdit->vPos.x,pEdit->vPos.y,pEdit->vPos.z)
				-Vector3(DIS_SPAWNPOINT_XZ, 0, DIS_SPAWNPOINT_XZ);
			TResult r=ray.Intersect(box);
			if(r.first && r.second <FLOAT_MAX)
			{
				m_pSelectSpawnPoint=(*pIter);
				break;
			}	
		}	
	}

	//新增一个音效
	void GameMapEditor::AddSound(tagMapSound *pSound)
	{
		m_SoundList.push_back(pSound);
	}

	//删除一个音效
	void GameMapEditor::DeleteSound(tagMapSound *pSound)
	{
		list<tagMapSound*>::iterator pIter = m_SoundList.begin();
		for(; pIter != m_SoundList.end(); ++pIter)
		{
			tagMapSound* pEdit =(*pIter);
			if(pEdit==pSound)
			{
				if(m_pSelectSound==pEdit) m_pSelectSound=NULL;
				delete pEdit;
				m_SoundList.erase(pIter);
				break;
			}	
		}
	}

	//清空音效列表
	void GameMapEditor::ClearSoundList()
	{
		list<tagMapSound*>::iterator pIter = m_SoundList.begin();
		for(; pIter != m_SoundList.end(); ++pIter)
		{
			tagMapSound* pEdit =(*pIter);
			delete pEdit;
		}
		m_SoundList.clear();
		m_pSelectSound=NULL;
	}

	//拾取音效
	void GameMapEditor::PickSoundBox(UINT cursorX,UINT cursorY)
	{
		if(m_pCamera==NULL)
			return;
		Ray ray;
		m_pCamera->GetMouseRay(cursorX,cursorY,ray);
		m_pSelectSound=NULL;
		list<tagMapSound*>::iterator pIter = m_SoundList.begin();
		for(; pIter != m_SoundList.end(); ++pIter)
		{
			tagMapSound* pEdit =(*pIter);
			Cool3D::AABBox box;
			box.max=Vector3(pEdit->fPos[0],pEdit->fPos[1],pEdit->fPos[2])
				    +Vector3(DISTENCE_SOUNDBOX_PICK,DISTENCE_SOUNDBOX_PICK,DISTENCE_SOUNDBOX_PICK);
			box.min=Vector3(pEdit->fPos[0],pEdit->fPos[1],pEdit->fPos[2])
				    -Vector3(DISTENCE_SOUNDBOX_PICK,DISTENCE_SOUNDBOX_PICK,DISTENCE_SOUNDBOX_PICK);
            TResult r=ray.Intersect(box);
			if(r.first && r.second <FLOAT_MAX)
			{
				m_pSelectSound=pEdit;
				break;
			}	
		}	
	}

	//根据索引得到音效
	tagMapSound * GameMapEditor::FindSound(DWORD nIndex)
	{
		list<tagMapSound*>::iterator pIter = m_SoundList.begin();
		for(; pIter != m_SoundList.end(); ++pIter)
		{
			if((*pIter)->dwObjID==nIndex)
			{
				return (*pIter);
			}
		}
		return NULL;
	}

	//删除一个音效通过索引
	void GameMapEditor::DeleteSound(DWORD nIndex)
	{
		list<tagMapSound*>::iterator pIter = m_SoundList.begin();
		for(; pIter != m_SoundList.end(); ++pIter)
		{
			tagMapSound* pSound=(*pIter);
			if(pSound->dwObjID==nIndex)
			{
			     if(pSound==m_pSelectSound) m_pSelectSound=NULL;
				 delete pSound;
				 m_SoundList.erase(pIter);
				 break;
			}
		}
	}

	//点光源相关
	//新增一个点光源
	void GameMapEditor::AddPointLight(MapObjEdit* pPointLight)
	{
		m_PointLightList.push_back(pPointLight);
	}

	//删除一个点光源
	void GameMapEditor::DeletePointLight(tagMapPointLight *pPointLight)
	{
		list<MapObjEdit*>::iterator pIter = m_PointLightList.begin();
		for(; pIter != m_PointLightList.end(); ++pIter)
		{
			tagMapPointLight* pEdit =(tagMapPointLight*)(*pIter)->GetPointLgtObj();
			if(pEdit==pPointLight)
			{
				if(m_pSelectPointLight==(*pIter)) m_pSelectPointLight=NULL;
				delete pEdit;
				m_PointLightList.erase(pIter);
				break;
			}	
		}
	}

	//删除一个点光源通过索引
	void GameMapEditor::DeletePointLight(DWORD nIndex)
	{
		list<MapObjEdit*>::iterator pIter = m_PointLightList.begin();
		for(; pIter != m_PointLightList.end(); ++pIter)
		{
			MapObjEdit* pEdit=(*pIter);
			if(pEdit->GetMapID()==nIndex)
			{
				if(pEdit==m_pSelectPointLight) m_pSelectPointLight=NULL;
				delete pEdit;
				m_PointLightList.erase(pIter);
				break;
			}
		}
	}

	//清空点光源列表
	void GameMapEditor::ClearPointLightList()
	{
		list<MapObjEdit*>::iterator pIter = m_PointLightList.begin();
		for(; pIter != m_PointLightList.end(); ++pIter)
		{
			MapObjEdit* pEdit =(*pIter);
			delete pEdit;
		}
		m_PointLightList.clear();
		m_pSelectPointLight=NULL;
	}

	//拾取点光源
	void GameMapEditor::PickPointLight(UINT x, UINT y)
	{
		if(m_pCamera==NULL)
			return;
		Ray ray;
		m_pCamera->GetMouseRay(x,y,ray);
		m_pSelectPointLight=NULL;
		list<MapObjEdit*>::iterator pIter = m_PointLightList.begin();
		for(; pIter != m_PointLightList.end(); ++pIter)
		{
			tagMapPointLight* pEdit =(tagMapPointLight*)(*pIter)->GetPointLgtObj();
			Cool3D::AABBox box;
			box.max=Vector3(pEdit->fPos[0],pEdit->fPos[1],pEdit->fPos[2])
				+Vector3(DISTENCE_POINTLIGHT_PICK,DISTENCE_POINTLIGHT_PICK,DISTENCE_POINTLIGHT_PICK);
			box.min=Vector3(pEdit->fPos[0],pEdit->fPos[1],pEdit->fPos[2])
				-Vector3(DISTENCE_POINTLIGHT_PICK,DISTENCE_POINTLIGHT_PICK,DISTENCE_POINTLIGHT_PICK);
			TResult r=ray.Intersect(box);
			if(r.first && r.second <FLOAT_MAX)
			{
				m_pSelectPointLight=(*pIter);
				break;
			}	
		}	
	}

	//根据索引得到点光源
	MapObjEdit* GameMapEditor::FindPointLight(DWORD nIndex)
	{
		list<MapObjEdit*>::iterator pIter = m_PointLightList.begin();
		for(; pIter != m_PointLightList.end(); ++pIter)
		{
			tagMapPointLight* pEdit =(tagMapPointLight*)(*pIter)->GetPointLgtObj();
			if(pEdit->dwID==nIndex)
			{
				return (*pIter);
			}
		}
		return NULL;
	}

	MapObjEdit *GameMapEditor::FindRoleByWayPoint(const TCHAR* szWayName)
	{
		/*for(list<MapObjEdit*>::iterator iter=m_objList.begin();
			iter!=m_objList.end();++iter)
		{
			MapObjEdit *pObj=(*iter);
			if(pObj->GetType()==MapObjEdit::NPC)
			{
				const tagMapNPC* pAtt=pObj->GetMapNPC();
				if(_tcscmp(pAtt->szWayName,szWayName)==0)
					return pObj;
			}
		}*/
		return NULL;
	}

	void GameMapEditor::SetSkyBox(const TCHAR* szTopTex, const TCHAR* szFrontRightTex, const TCHAR* szBackLeftTex, 
		const Vector3& drawSize, const Vector3& offSize)
	{
		_tcscpy_s(m_szSkyBoxTopTex, sizeof(m_szSkyBoxTopTex)/sizeof(TCHAR), szTopTex);
		_tcscpy_s(m_szSkyBoxFrontRightTex, sizeof(m_szSkyBoxFrontRightTex)/sizeof(TCHAR), szFrontRightTex);
		_tcscpy_s(m_szSkyBoxBackLeftTex, sizeof(m_szSkyBoxBackLeftTex)/sizeof(TCHAR), szBackLeftTex);
		m_fSkyBoxXsize = drawSize.x;		
		m_fSkyBoxYsize = drawSize.y;
		m_fSkyBoxZsize = drawSize.z;
		m_fSkyBoxOffX = offSize.x;
		m_fSkyBoxOffY = offSize.y;
		m_fSkyBoxOffZ = offSize.z;
		if(_tcslen(m_szSkyBoxTopTex)==0 || 
			_tcslen(m_szSkyBoxFrontRightTex)==0 ||
			_tcslen(m_szSkyBoxBackLeftTex)==0 )
			m_pSGSkyBoxNode = NULL;
		else
			m_pSGSkyBoxNode = new SGSkyBox;
		if(m_pSGSkyBoxNode!=NULL)
		{
			m_pSGSkyBoxNode->Create(m_szSkyBoxTopTex, m_szSkyBoxFrontRightTex,
				m_szSkyBoxBackLeftTex, drawSize);
			m_pSGSkyBoxNode->SetOffset(offSize);
			GetSceneGraph()->SetSkyNode(m_pSGSkyBoxNode);
		}
		else
			GetSceneGraph()->SetSkyNode(NULL);
	}

	//设置天空盒旋转角度
	void GameMapEditor::SetSkyYaw(FLOAT fVal)
	{ 
		m_fSkyYaw = fVal; 
		if(m_pSGSkyBoxNode!=NULL)
			m_pSGSkyBoxNode->SetRotate(_DegToRad(m_fSkyYaw), 0, 0);
	}

	void GameMapEditor::SetRenderSkyShade(const bool bRenderskyShade)
	{ 
		m_bRenderSkyShade = bRenderskyShade;
		if(m_pSGSkyBoxNode!=NULL)
			m_pSGSkyBoxNode->EnableRenderShade(bRenderskyShade);
	}

	void GameMapEditor::SetSkyShadeCol(const DWORD dwSkyShadeCol)
	{ 
		m_dwSkyShadeCol = dwSkyShadeCol; 
		if(m_pSGSkyBoxNode!=NULL)
			m_pSGSkyBoxNode->SetShadeColor(m_dwSkyShadeCol);
	}

	void GameMapEditor::GetTransList( IFS *pFS, LPCTSTR szFileName, vector<tstring>& fsmList )
	{
		fsmList.clear();

		CSVFile csvFile;
		bool ret = csvFile.OpenFile(pFS, szFileName);
		if(!ret)
		{
			csvFile.CloseFile();
		}

		while(true)
		{
			const vector<tstring>& vec = csvFile.GetNextLine();
			if(vec.empty())
				break;

			fsmList.push_back(vec[0]);
		}

		csvFile.CloseFile();
	}
}//namespace WorldBase
