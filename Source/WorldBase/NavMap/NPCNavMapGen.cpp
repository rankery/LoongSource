#include "StdAfx.h"
#include "NPCNavMapGen.h"
#include "NavCollider.h"
#include "NavSceneNode.h"
#include "..\NavMapEditor\NavMapEditorUtil.h"

namespace WorldBase
{
	const TCHAR* const NavEditorBasePath=_T(".\\Data\\NavEditor");

	NPCNavMapGen::NPCNavMapGen(void)
	{
		m_pHMap=NULL;
		m_pCollider=NULL;
	}

	NPCNavMapGen::~NPCNavMapGen(void)
	{
	}

	void NPCNavMapGen::BeginBuild( const list<tagNPCNavMapBuildStartPoint*>& points,const vector<tagMapDoor>& doors,Heightmap* pHMap,BitMap* pManualCangoMap,NavCollider* pCollider,float fNPCPassHeight )
	{
		m_buildStartPoints=points;
		m_buildDoors=doors;
		m_pHMap=pHMap;
		m_pCollider=pCollider;
		m_pManualCangoMap=pManualCangoMap;
		m_fNPCPassHeight=fNPCPassHeight;
	}

	bool NPCNavMapGen::EndBuild( const TCHAR* szSavePath,const TCHAR* szMapName,IUICallBack* pUI )
	{
		//--build cango map
		pUI->AddMsg(_T("\t\t生成cangomap..."));

		if(m_buildStartPoints.size()==0)
		{
			pUI->AddMsg(_T("失败,未找到NPC格子导航图生成起点\r\n"));
			return false;
		}

		// 关闭所有的门的碰撞
		m_pCollider->EnableAllDoorsCollide(false);

		m_cangoMap.Create(m_pHMap->Width(),m_pHMap->Height(),false);
		m_visitMap.Create(m_pHMap->Width(),m_pHMap->Height(),false);

		int count=0;
		for(list<tagNPCNavMapBuildStartPoint*>::iterator iter=m_buildStartPoints.begin();
			iter!=m_buildStartPoints.end();iter++)
		{
			tagNPCNavMapBuildStartPoint* pPoint=*iter;

			FloodFill(pPoint->vPos,pUI);
			count++;
		}

		pUI->AddMsg(_T("ok\r\n"));


		//--叠加不可行走bitmap
		pUI->AddMsg(_T("\t\t叠加不可行走区域..."));
		NavMapEditorUtil::MergeCangoMap(m_pManualCangoMap,&m_cangoMap);
		pUI->AddMsg(_T("ok\r\n"));

		//--消除缝隙
		pUI->AddMsg(_T("\t\t消除缝隙..."));
		UINT x,z;
		for(z=0;z<m_cangoMap.Height();z++)
		{
			for(x=0;x<m_cangoMap.Width();x++)
			{
				bool bLeft=m_cangoMap.IfIndex(x-1,z) && m_cangoMap.GetValue(x-1,z);
				bool bRight=m_cangoMap.IfIndex(x+1,z) && m_cangoMap.GetValue(x+1,z);
				bool bTop=m_cangoMap.IfIndex(x,z-1) && m_cangoMap.GetValue(x,z-1);
				bool bBottom=m_cangoMap.IfIndex(x,z+1) && m_cangoMap.GetValue(x,z+1);

				if( (!bLeft&&!bRight) || (!bTop&&!bBottom) )
					m_cangoMap.SetValue(x,z,false);

				pUI->SetProgress(x+z*m_cangoMap.Width(),m_cangoMap.Width()*m_cangoMap.Height());
			}
		}
		pUI->AddMsg(_T("ok\r\n"));

		//--消除小的可行走区域
		pUI->AddMsg(_T("\t\t消除小的不可行走区域..."));
		m_visitMap.Create(m_pHMap->Width(),m_pHMap->Height(),false);
		for(z=0;z<m_visitMap.Height();z++)
		{
			for(x=0;x<m_visitMap.Width();x++)
			{
				if(!m_visitMap.GetValue(x,z)
					&&m_cangoMap.GetValue(x,z))
					FloodClear(x,z);

				pUI->SetProgress(x+z*m_visitMap.Width(),m_visitMap.Width()*m_visitMap.Height());
			}
		}
		pUI->AddMsg(_T("ok\r\n"));


		//--将cangomap写入文件
		pUI->AddMsg(_T("\t\t保存cangomap..."));
		TCHAR szPath[512];
		_stprintf(szPath,_T("%sNav\\npccango.bmap"),szSavePath);
		::SetCurrentDirectory(Kernel::Inst()->GetWorkPath());
		FILE* fp=_tfopen(szPath,_T("wb"));
		if(fp==NULL)
		{
			pUI->AddMsg(_T("失败\r\n"));
			return false;
		}
		m_cangoMap.WriteFile(fp);
		fclose(fp);
		pUI->AddMsg(_T("ok\r\n"));

		//--生成门相关数据
		pUI->AddMsg(_T("\t\t生成门数据..."));
		if(!BuildDoors(szSavePath,pUI))
		{
			pUI->AddMsg(_T("失败\r\n"));
			return false;
		}
		pUI->AddMsg(_T("ok\r\n"));


		//--重新保存heightmap
		pUI->AddMsg(_T("\t\t修改高度图..."));
		_stprintf(szPath,_T("%snav\\terrain.hmap"),szSavePath);
		::SetCurrentDirectory(Kernel::Inst()->GetWorkPath());
		fp=_tfopen(szPath,_T("wb"));
		if(fp==NULL)
		{
			pUI->AddMsg(_T("失败\r\n"));
			return false;
		}
		m_pHMap->WriteToFile(fp);
		fclose(fp);
		pUI->AddMsg(_T("ok\r\n"));

		//--生成预览信息
		pUI->AddMsg(_T("\t\t生成预览数据..."));

		::SetCurrentDirectory(Kernel::Inst()->GetWorkPath());
		::CreateDirectory(NavEditorBasePath,NULL);
		_stprintf(szPath,_T("%s\\%s"),NavEditorBasePath,szMapName);
		::CreateDirectory(szPath,NULL);

		//bitmap
		_stprintf(szPath,_T("%s\\%s\\npccango.bmap"),NavEditorBasePath,szMapName);
		::SetCurrentDirectory(Kernel::Inst()->GetWorkPath());
		fp=_tfopen(szPath,_T("wb"));
		if(fp==NULL)
		{
			pUI->AddMsg(_T("失败\r\n"));
			return false;
		}
		m_cangoMap.WriteFile(fp);
		fclose(fp);

		//heightmap
		_stprintf(szPath,_T("%s\\%s\\npcnavmap.hmap"),NavEditorBasePath,szMapName);
		::SetCurrentDirectory(Kernel::Inst()->GetWorkPath());
		fp=_tfopen(szPath,_T("wb"));
		if(fp==NULL)
		{
			pUI->AddMsg(_T("失败\r\n"));
			return false;
		}
		m_pHMap->WriteToFile(fp);
		fclose(fp);

		//image
		_stprintf(szPath,_T("%s\\%s\\npccango.bmp"),NavEditorBasePath,szMapName);
		m_cangoMap.ExportImage(szPath);

		pUI->AddMsg(_T("ok\r\n"));

		return true;
	}

	void NPCNavMapGen::FloodFill( Vector3& startPos,IUICallBack* pUI )
	{
		struct VisitNode
		{
			int x,z;
			float h;
		};

		list<VisitNode> visitList;

		VisitNode nd;
		nd.x=m_pHMap->World2Tile(startPos).x;
		nd.z=m_pHMap->World2Tile(startPos).y;
		nd.h=startPos.y;
		visitList.push_back(nd);

		int count=0;

		while(!visitList.empty())
		{
			nd=visitList.front();
			visitList.pop_front();

			if(m_visitMap.GetValue(nd.x,nd.z)//已访问过
				&&(int)nd.h>=(int)m_pHMap->GetHeight(nd.x,nd.z))//高度比以前高
				continue;

			m_visitMap.SetValue(nd.x,nd.z,true);
			m_pHMap->SetValue(nd.x,nd.z,int(nd.h/m_pHMap->GetYScale()));

			float leftH,rightH,topH,bottomH;
			float leftTopH,leftBottomH,rightTopH,rightBottomH;
			if(IfCanGoTile(nd.x,nd.z,nd.h,nd.x-1,nd.z,leftH)//left
				&&IfCanGoTile(nd.x,nd.z,nd.h,nd.x+1,nd.z,rightH)//right
				&&IfCanGoTile(nd.x,nd.z,nd.h,nd.x,nd.z-1,topH)//top
				&&IfCanGoTile(nd.x,nd.z,nd.h,nd.x,nd.z+1,bottomH)//bottom
				&&IfCanGoTile(nd.x,nd.z,nd.h,nd.x-1,nd.z-1,leftTopH)//left-top
				&&IfCanGoTile(nd.x,nd.z,nd.h,nd.x-1,nd.z+1,leftBottomH)//left-bottom
				&&IfCanGoTile(nd.x,nd.z,nd.h,nd.x+1,nd.z-1,rightTopH)//right-top
				&&IfCanGoTile(nd.x,nd.z,nd.h,nd.x+1,nd.z+1,rightBottomH))//right-bottom
			{
				m_cangoMap.SetValue(nd.x,nd.z,true);

				//left
				VisitNode leftNd;
				leftNd.x=nd.x-1;
				leftNd.z=nd.z;
				leftNd.h=leftH;
				visitList.push_back(leftNd);

				//right
				VisitNode rightNd;
				rightNd.x=nd.x+1;
				rightNd.z=nd.z;
				rightNd.h=rightH;
				visitList.push_back(rightNd);

				//top
				VisitNode topNd;
				topNd.x=nd.x;
				topNd.z=nd.z-1;
				topNd.h=topH;
				visitList.push_back(topNd);

				//bottom
				VisitNode bottomNd;
				bottomNd.x=nd.x;
				bottomNd.z=nd.z+1;
				bottomNd.h=bottomH;
				visitList.push_back(bottomNd);

				//left-top
				VisitNode leftTopNd;
				leftTopNd.x=nd.x-1;
				leftTopNd.z=nd.z-1;
				leftTopNd.h=leftTopH;
				visitList.push_back(leftTopNd);

				//left-bottom
				VisitNode leftBottomNd;
				leftBottomNd.x=nd.x-1;
				leftBottomNd.z=nd.z+1;
				leftBottomNd.h=leftBottomH;
				visitList.push_back(leftBottomNd);

				//right-top
				VisitNode rightTopNd;
				rightTopNd.x=nd.x+1;
				rightTopNd.z=nd.z-1;
				rightTopNd.h=rightTopH;
				visitList.push_back(rightTopNd);

				//right-bottom
				VisitNode rightBottomNd;
				rightBottomNd.x=nd.x+1;
				rightBottomNd.z=nd.z+1;
				rightBottomNd.h=rightBottomH;
				visitList.push_back(rightBottomNd);

			}
			else
			{
				m_cangoMap.SetValue(nd.x,nd.z,false);
			}

			pUI->SetProgress(count++,int(m_cangoMap.Width()*m_cangoMap.Height()));
		}
	}

	bool NPCNavMapGen::IfCanGoTile( int x1,int z1,float h1,int x2,int z2 )
	{
		const Vector3 roleSize=Vector3(50,175,50);

		if(!m_pHMap->IfIndex(x1,z1))
			return false;

		if(!m_pHMap->IfIndex(x2,z2))
			return false;

		Vector3 wpt1=m_pHMap->Tile2World(x1,z1);
		wpt1.y=h1;
		Vector3 wpt2=m_pHMap->Tile2World(x2,z2);
		wpt2.y=h1;

		if(m_moveCollider.IfCanGo(m_pCollider,wpt1,wpt2,roleSize,
			NULL,NULL,true,1000,true,m_fNPCPassHeight)==ENR_Arrive)
		{
			return true;
		}

		return false;
	}

	bool NPCNavMapGen::IfCanGoTile( int x1,int z1,float h1,int x2,int z2,float& h2 )
	{
		const Vector3 roleSize=Vector3(50,175,50);
		const float tileSize=50.0f;

		if(!m_pHMap->IfIndex(x1,z1))
			return false;

		if(!m_pHMap->IfIndex(x2,z2))
			return false;

		Vector3 wpt1=m_pHMap->Tile2World(x1,z1);
		wpt1.y=h1;
		Vector3 wpt2=m_pHMap->Tile2World(x2,z2);
		wpt2.y=h1;

		Vector3 pos;
		if(m_moveCollider.IfCanGo(m_pCollider,wpt1,wpt2,roleSize,
			&pos,NULL,true,1000,true,m_fNPCPassHeight)==ENR_Arrive)
		{
			h2=pos.y;
			return true;
		}

		return false;
	}

	bool NPCNavMapGen::IfCanGoTile( int x,int z )
	{
		if(!m_cangoMap.IfIndex(x,z))
			return false;
		return m_cangoMap.GetValue(x,z);
	}

	void NPCNavMapGen::FloodClear( int x,int z )
	{
		if(!m_cangoMap.GetValue(x,z))
			return;
		if(m_visitMap.GetValue(x,z))
			return;

		list<Point> visitList;
		visitList.push_back(Point(x,z));
		m_visitMap.SetValue(x,z,true);

		list<Point> tileList;
		while(!visitList.empty())
		{
			Point top=visitList.front();
			visitList.pop_front();

			tileList.push_back(top);

			//left
			if(IfCanGoTile(top.x-1,top.y)
				&&!m_visitMap.GetValue(top.x-1,top.y))
			{
				m_visitMap.SetValue(top.x-1,top.y,true);
				visitList.push_back(Point(top.x-1,top.y));
			}
			//top
			if(IfCanGoTile(top.x,top.y-1)
				&&!m_visitMap.GetValue(top.x,top.y-1))
			{
				m_visitMap.SetValue(top.x,top.y-1,true);
				visitList.push_back(Point(top.x,top.y-1));
			}
			//right
			if(IfCanGoTile(top.x+1,top.y)
				&&!m_visitMap.GetValue(top.x+1,top.y))
			{
				m_visitMap.SetValue(top.x+1,top.y,true);
				visitList.push_back(Point(top.x+1,top.y));
			}
			//bottom
			if(IfCanGoTile(top.x,top.y+1)
				&&!m_visitMap.GetValue(top.x,top.y+1))
			{
				m_visitMap.SetValue(top.x,top.y+1,true);
				visitList.push_back(Point(top.x,top.y+1));
			}
		}

		if(tileList.size()<1000)
		{
			for(list<Point>::iterator iter=tileList.begin();
				iter!=tileList.end();iter++)
			{
				m_cangoMap.SetValue(iter->x,iter->y,false);
			}
		}
	}

	bool NPCNavMapGen::BuildDoors( const TCHAR* szSavePath,IUICallBack* pUI )
	{
		//打开所有的门的碰撞
		m_pCollider->EnableAllDoorsCollide(true);

		vector<NPCNavMapDoors::tagDoor*> doors;
		FreePtrVector<NPCNavMapDoors::tagDoor> freeDoors;//用于释放对象指针vector

		for(int i=0;i<(int)m_buildDoors.size();i++)
		{
			tagMapDoor& mapDoor=m_buildDoors[i];

			//是否阻挡格子导航的NPC？
			if(!mapDoor.bBlockingTileNPC)
				continue;

			//生成一个门的数据
			NPCNavMapDoors::tagDoor* pDoor=BuildDoor(mapDoor.dwObjID);
			if(pDoor!=NULL)
				doors.push_back(pDoor);
		}

		//存盘
		if(!NPCNavMapDoors::EditorSave(doors,szSavePath))
		{
			freeDoors(doors);
			return false;
		}
		
		freeDoors(doors);
		return true;
	}

	NPCNavMapDoors::tagDoor* NPCNavMapGen::BuildDoor( DWORD dwObjID )
	{
		//查找SceneNode
		NavSceneNode* pNode=m_pCollider->FindDoor(dwObjID);
		if(pNode==NULL)
			return NULL;

		//计算门的格子区域
		RECT rc;

		AABBox box;
		pNode->GetBox(box);

		POINT leftTop		= m_pHMap->World2Tile(box.min);
		POINT rightBottom	= m_pHMap->World2Tile(box.max);
		rc.left				= leftTop.x;
		rc.top				= leftTop.y;
		rc.right			= rightBottom.x;
		rc.bottom			= rightBottom.y;

		//检查区域是否越界
		if(!m_cangoMap.IfIndex(rc.left,rc.top)
			||!m_cangoMap.IfIndex(rc.right,rc.bottom))
			return NULL;

		//分配一个新的Door对象
		NPCNavMapDoors::tagDoor* pOut=new NPCNavMapDoors::tagDoor;
		pOut->dwObjID=dwObjID;
		pOut->leftTopTile=leftTop;

		//生成打开状态位图
		BitMap* pOpenMap=new BitMap;
		pOpenMap->Create(rc.right-rc.left,rc.bottom-rc.top,false);
		m_cangoMap.CopyTo(*pOpenMap,rc.left,rc.top,pOpenMap->Width(),pOpenMap->Height(),0,0);
		pOut->pOpenStateCangoMap=pOpenMap;

		//生成关闭状态位图
		BitMap* pCloseMap=new BitMap;
		pCloseMap->Create(rc.right-rc.left,rc.bottom-rc.top,false);
		m_cangoMap.CopyTo(*pCloseMap,rc.left,rc.top,pCloseMap->Width(),pCloseMap->Height(),0,0);
		pOut->pCloseStateCangoMap=pCloseMap;

		int x,y;
		int gx,gy;//global x,y
		float h;
		for(y=0;y<(int)pCloseMap->Height();++y)
		{
			for(x=0;x<(int)pCloseMap->Width();++x)
			{
				if(pCloseMap->GetValue(x,y))
				{
					gx=x+rc.left;
					gy=y+rc.top;
					h=m_pHMap->GetSafeHeight(gx,gy);
					if(!IfCanGoTile(gx,gy,h,gx-1,gy)//left
						||!IfCanGoTile(gx,gy,h,gx+1,gy)//right
						||!IfCanGoTile(gx,gy,h,gx,gy-1)//top
						||!IfCanGoTile(gx,gy,h,gx,gy+1)//bottom
						||!IfCanGoTile(gx,gy,h,gx-1,gy-1)//left-top
						||!IfCanGoTile(gx,gy,h,gx-1,gy+1)//left-bottom
						||!IfCanGoTile(gx,gy,h,gx+1,gy-1)//right-top
						||!IfCanGoTile(gx,gy,h,gx+1,gy+1))//right-bottom
					{
						pCloseMap->SafeSetValue(x,y,false);
					}
				}
			}
		}

		return pOut;
	}
}//namespace WorldBase

