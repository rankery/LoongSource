#include "StdAfx.h"
#include "PlayerCangoMapGen.h"
#include "NavCollider.h"
#include "NavSceneNode.h"
#include "..\NavMapEditor\NavMapEditorUtil.h"

namespace WorldBase
{
	const TCHAR* const NavEditorBasePath=_T(".\\Data\\NavEditor");

	PlayerCangoMapGen::PlayerCangoMapGen(void)
		: m_pHMap(NULL)
		, m_pCollider(NULL)
	{
	}

	PlayerCangoMapGen::~PlayerCangoMapGen(void)
	{
	}

	void PlayerCangoMapGen::BeginBuild( const list<tagPlayerNavMapBuildStartPoint*>& points,Heightmap* pHMap,BitMap* pManualCangoMap,BitMap* pRoadMap,NavCollider* pCollider )
	{
		m_buildStartPoints=points;
		m_pHMap=pHMap;
		m_pCollider=pCollider;
		m_pManualCangoMap=pManualCangoMap;
		m_pOriginalRoadMap=pRoadMap;
	}

	bool PlayerCangoMapGen::EndBuild( const TCHAR* szSavePath,const TCHAR* szMapName,IUICallBack* pUI )
	{
		//--build cango map
		pUI->AddMsg(_T("\t\t生成cangomap..."));
		
		if(m_buildStartPoints.size()==0)
		{
			pUI->AddMsg(_T("失败,未找到玩家寻路导航图生成起点\r\n"));
			return false;
		}

		// 关闭所有的门的碰撞
		m_pCollider->EnableAllDoorsCollide(false);

		m_cangoMap.Create(m_pHMap->Width(),m_pHMap->Height(),false);
		m_visitMap.Create(m_pHMap->Width(),m_pHMap->Height(),false);
		m_pHMap->CopyTo(m_tmpHMap);

		int count=0;
		for(list<tagPlayerNavMapBuildStartPoint*>::iterator iter=m_buildStartPoints.begin();
			iter!=m_buildStartPoints.end();iter++)
		{
			tagPlayerNavMapBuildStartPoint* pPoint=*iter;

			FloodFill(pPoint->vPos,pUI);
			count++;
		}

		pUI->AddMsg(_T("ok\r\n"));

		//--叠加不可行走bitmap--------------------------------------------------------------
		pUI->AddMsg(_T("\t\t叠加不可行走区域..."));
		NavMapEditorUtil::MergeCangoMap(m_pManualCangoMap,&m_cangoMap);
		pUI->AddMsg(_T("ok\r\n"));

		//--消除小的可行走区域，以免可行走区域太多------------------------------------------
		pUI->AddMsg(_T("\t\t消除小的可行走区域..."));
		m_visitMap.Create(m_pHMap->Width(),m_pHMap->Height(),false);
		UINT x,z;
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

		//--save to file----------------------------------------------------------------------
		pUI->AddMsg(_T("\t\t保存cangomap..."));
		TCHAR szPath[512];
		_stprintf(szPath,_T("%snav\\playercango.bmap"),szSavePath);
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

		//--生成道路区域-------------------------------------------------------------
		pUI->AddMsg(_T("\t\t生成道路区域..."));
		
		m_pOriginalRoadMap->Clone(&m_roadMap);
		NavMapEditorUtil::MergeCangoMap(&m_cangoMap,&m_roadMap);

		//保存
		_stprintf(szPath,_T("%snav\\playerroadmap.bmap"),szSavePath);
		::SetCurrentDirectory(Kernel::Inst()->GetWorkPath());
		fp=_tfopen(szPath,_T("wb"));
		if(fp==NULL)
		{
			pUI->AddMsg(_T("失败\r\n"));
			return false;
		}
		m_roadMap.WriteFile(fp);
		fclose(fp);

		pUI->AddMsg(_T("ok\r\n"));

		//--生成预览信息-------------------------------------------------------------------------
		pUI->AddMsg(_T("\t\t生成预览数据..."));

		::SetCurrentDirectory(Kernel::Inst()->GetWorkPath());
		::CreateDirectory(NavEditorBasePath,NULL);
		_stprintf(szPath,_T("%s\\%s"),NavEditorBasePath,szMapName);
		::CreateDirectory(szPath,NULL);

		//bitmap
		_stprintf(szPath,_T("%s\\%s\\playercango.bmap"),NavEditorBasePath,szMapName);
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
		_stprintf(szPath,_T("%s\\%s\\playernavmap.hmap"),NavEditorBasePath,szMapName);
		::SetCurrentDirectory(Kernel::Inst()->GetWorkPath());
		fp=_tfopen(szPath,_T("wb"));
		if(fp==NULL)
		{
			pUI->AddMsg(_T("失败\r\n"));
			return false;
		}
		m_tmpHMap.WriteToFile(fp);
		fclose(fp);

		//player cango image
		_stprintf(szPath,_T("%s\\%s\\playercango.bmp"),NavEditorBasePath,szMapName);
		m_cangoMap.ExportImage(szPath);	

		//road image
		_stprintf(szPath,_T("%s\\%s\\road.bmp"),NavEditorBasePath,szMapName);
		m_roadMap.ExportImage(szPath);		

		pUI->AddMsg(_T("ok\r\n"));

		return true;
	}

	void PlayerCangoMapGen::FloodFill(Vector3& startPos,IUICallBack* pUI )
	{
		struct VisitNode
		{
			int oldx,oldz;//for test
			float oldh;//for test
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
				&&(int)nd.h<=(int)m_tmpHMap.GetHeight(nd.x,nd.z))//高度比以前低
				continue;

			m_visitMap.SetValue(nd.x,nd.z,true);
			m_tmpHMap.SetValue(nd.x,nd.z,int(nd.h/m_tmpHMap.GetYScale()));

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
				leftNd.oldx=nd.x;
				leftNd.oldz=nd.z;
				leftNd.oldh=nd.h;
				visitList.push_back(leftNd);

				//right
				VisitNode rightNd;
				rightNd.x=nd.x+1;
				rightNd.z=nd.z;
				rightNd.h=rightH;
				rightNd.oldx=nd.x;
				rightNd.oldz=nd.z;
				rightNd.oldh=nd.h;
				visitList.push_back(rightNd);

				//top
				VisitNode topNd;
				topNd.x=nd.x;
				topNd.z=nd.z-1;
				topNd.h=topH;
				topNd.oldx=nd.x;
				topNd.oldz=nd.z;
				topNd.oldh=nd.h;
				visitList.push_back(topNd);

				//bottom
				VisitNode bottomNd;
				bottomNd.x=nd.x;
				bottomNd.z=nd.z+1;
				bottomNd.h=bottomH;
				bottomNd.oldx=nd.x;
				bottomNd.oldz=nd.z;
				bottomNd.oldh=nd.h;
				visitList.push_back(bottomNd);

				//left-top
				VisitNode leftTopNd;
				leftTopNd.x=nd.x-1;
				leftTopNd.z=nd.z-1;
				leftTopNd.h=leftTopH;
				leftTopNd.oldx=nd.x;
				leftTopNd.oldz=nd.z;
				leftTopNd.oldh=nd.h;
				visitList.push_back(leftTopNd);

				//left-bottom
				VisitNode leftBottomNd;
				leftBottomNd.x=nd.x-1;
				leftBottomNd.z=nd.z+1;
				leftBottomNd.h=leftBottomH;
				leftBottomNd.oldx=nd.x;
				leftBottomNd.oldz=nd.z;
				leftBottomNd.oldh=nd.h;
				visitList.push_back(leftBottomNd);

				//right-top
				VisitNode rightTopNd;
				rightTopNd.x=nd.x+1;
				rightTopNd.z=nd.z-1;
				rightTopNd.h=rightTopH;
				rightTopNd.oldx=nd.x;
				rightTopNd.oldz=nd.z;
				rightTopNd.oldh=nd.h;
				visitList.push_back(rightTopNd);

				//right-bottom
				VisitNode rightBottomNd;
				rightBottomNd.x=nd.x+1;
				rightBottomNd.z=nd.z+1;
				rightBottomNd.h=rightBottomH;
				rightBottomNd.oldx=nd.x;
				rightBottomNd.oldz=nd.z;
				rightBottomNd.oldh=nd.h;
				visitList.push_back(rightBottomNd);

			}
			else
			{
				m_cangoMap.SetValue(nd.x,nd.z,false);
			}

			pUI->SetProgress(count++,int(m_cangoMap.Width()*m_cangoMap.Height()));
		}
	}

	bool PlayerCangoMapGen::IfCanGoTile( int x1,int z1,float h1,int x2,int z2,float& h2 )
	{
		const Vector3 roleSize=Vector3(50,200,50);

		if(!m_pHMap->IfIndex(x1,z1))
			return false;

		if(!m_pHMap->IfIndex(x2,z2))
			return false;

		Vector3 wpt1=m_pHMap->Tile2World(x1,z1);
		wpt1.y=h1;
		Vector3 wpt2=m_pHMap->Tile2World(x2,z2);
		wpt2.y=h1;

		list<NavCollider::tagMovePoint> path;
		if( m_pCollider->IfWillSwim( wpt1, roleSize.y ) )
		{
			Vector3 pos;
			ENavResult eRet = m_swimCollider.IfCanGo(m_pCollider,wpt1,wpt2,roleSize,&pos,1000);
			if( eRet == ENR_SwimLanded
				|| eRet == ENR_Arrive )
			{
				h2 = pos.y;
				return true;
			}
		}
		else
		{
			Vector3 pos;
			ENavResult eRet = m_moveCollider.IfCanGo(m_pCollider,wpt1,wpt2,roleSize,&pos,1000,true,true);
			if( eRet == ENR_Arrive
				|| eRet == ENR_WillSwim )
			{
				h2=pos.y;
				return true;
			}
		}

		return false;
	}

	bool PlayerCangoMapGen::IfCanGoTile( int x,int z )
	{
		if(!m_cangoMap.IfIndex(x,z))
			return false;
		return m_cangoMap.GetValue(x,z);
	}

	void PlayerCangoMapGen::FloodClear( int x,int z )
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

		if(tileList.size()<100)
		{
			for(list<Point>::iterator iter=tileList.begin();
				iter!=tileList.end();iter++)
			{
				m_cangoMap.SetValue(iter->x,iter->y,false);
			}
		}
	}
}//namespace WorldBase

