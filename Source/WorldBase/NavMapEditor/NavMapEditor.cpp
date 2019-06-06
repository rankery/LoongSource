#include "stdafx.h"
#include "NavMapEditor.h"
#include "..\NavMap\NavQuadTreeDataBuilder.h"
#include "..\NavMap\NavEntityNode.h"
#include "..\NavMap\NavResMgr.h"
#include "..\GameMap\GameMap.h"
#include "..\NavMap\NavCollider.h"
#include "..\NavMap\PathPointDistMap.h"
#include "..\NavMap\NPCNavGraph.h"
#include "..\NavMap\NPCNavMapGen.h"
#include "..\NavMap\NPCNavGraphGen.h"
#include "..\NavMap\NavColliderDataGen.h"
#include "..\NavMap\PlayerCangoMapGen.h"
#include "..\NavMap\PathFinderByPatchDataGen.h"

namespace WorldBase
{
	NavMapEditor::NavMapEditor()
		: m_pTerrainHMap(NULL)
		, m_pManualCangoMap(NULL)
		, m_pPlayerRoadMap(NULL)
		, m_pWaterArea(NULL)
		, m_pWaterHeight(NULL)
		, m_pGroundType(NULL)
	{
	}

	NavMapEditor::~NavMapEditor()
	{
	}

	void NavMapEditor::BuildSetHeightmap( Heightmap *pHMap )
	{
		ASSERT(pHMap!=NULL);
		m_pTerrainHMap=pHMap;
	}

	void NavMapEditor::BuildSetManualCangoMap( BitMap* pManualCangoMap )
	{
		m_pManualCangoMap=pManualCangoMap;
	}

	void NavMapEditor::BuildSetPlayerRoadMap( BitMap* pPlayerRoadmap )
	{
		m_pPlayerRoadMap=pPlayerRoadmap;
	}

	void NavMapEditor::BuildSetPathPoints( const vector<tagMapPathPoint>& points )
	{
		m_pathPoints=points;
	}

	void NavMapEditor::BuildSetGroundType( Heightmap* pGroundType )
	{
		m_pGroundType=pGroundType;
	}

	void NavMapEditor::BuildSetDoors( const vector<tagMapDoor>& doors )
	{
		m_doors=doors;
	}

	bool NavMapEditor::Build( const TCHAR* szSavePath,const TCHAR* szMapName,IUICallBack* pUI )
	{
		//--生成碰撞数据
		if(m_buildOptions.bColliderData)
		{
			pUI->AddMsg(_T("\t生成碰撞数据..."));
			NavColliderDataGen gener;
			gener.BeginBuild(m_pTerrainHMap,m_pManualCangoMap,m_pGroundType);
			if(!gener.EndBuild(szSavePath,szMapName))
			{
				pUI->AddMsg(_T("失败.\r\n"));
				return false;
			}
			pUI->AddMsg(_T("ok.\r\n"));
		}

		//--保存水面区域和高度数据
		pUI->AddMsg(_T("\t保存水面数据..."));
		if(!SaveWaterData(szSavePath,szMapName))
		{
			pUI->AddMsg(_T("失败.\r\n"));
			return false;
		}
		pUI->AddMsg(_T("ok.\r\n"));

		//--加载Collider
		NavCollider collider;
		collider.LoadFromFile(szMapName,Kernel::Inst()->DefaultFS());
		collider.SetViewZoneSize(MathConst::BigBox.max);
		collider.InvalidViewZone(MathConst::Zero3);

		//--生成NPC格子导航图
		if(m_buildOptions.bNPCNavMap)
		{
			pUI->AddMsg(_T("\t生成NPC格子导航数据...\r\n"));
			NPCNavMapGen gener;
			gener.BeginBuild(m_npcNavMapBuildStartPoints,m_doors,m_pTerrainHMap,m_pManualCangoMap,&collider,m_buildOptions.fNPCPassHeight);
			if(!gener.EndBuild(szSavePath,szMapName,pUI))
			{
				return false;
			}
		}
		
		//--重新加载碰撞数据，因为生成NPC格子导航图时会修改高度图
		collider.Destroy();
		NavResMgr::Inst()->DoGC();//资源池垃圾收集
		collider.LoadFromFile(szMapName,Kernel::Inst()->DefaultFS());
		collider.SetViewZoneSize(MathConst::BigBox.max);
		collider.InvalidViewZone(MathConst::Zero3);

		//--生成NPC路点导航图
		if(m_buildOptions.bNPCNavGraph)
		{
			pUI->AddMsg(_T("\t生成NPC路点数据..."));
			NPCNavGraphGen gener;
			gener.BeginBuild(m_pathPoints,&collider,m_buildOptions.fNPCNavGraphPassHeight);
			if(!gener.EndBuild(szSavePath,szMapName))
			{
				pUI->AddMsg(_T("失败.\r\n"));
				return false;
			}
			pUI->AddMsg(_T("ok.\r\n"));
		}


		//--生成玩家CangoMap
		if(m_buildOptions.bPlayerNavMap)
		{
			pUI->AddMsg(_T("\t生成玩家寻路数据...\r\n"));

			BitMap playerCango;
			BitMap playerRoadMap;
			{
				PlayerCangoMapGen gener;
				gener.BeginBuild(m_playerNavMapBuildStartPoints,m_pTerrainHMap,m_pManualCangoMap,m_pPlayerRoadMap,&collider);
				if(!gener.EndBuild(szSavePath,szMapName,pUI))
				{
					return false;
				}
				gener.GetCangoMap()->Clone(&playerCango);
				gener.GetRoadMap()->Clone(&playerRoadMap);
			}

			//--生成区域寻路数据
			pUI->AddMsg(_T("\t\t生成区域寻路数据..."));
			{
				PathFinderByPatchDataGen gener;
				gener.BeginBuild(&playerCango);
				if(!gener.EndBuild(szSavePath,szMapName,_T("playerpatchpathdata")))
				{
					pUI->AddMsg(_T("失败.\r\n"));
					return false;
				}
			}
			{
				PathFinderByPatchDataGen gener;
				gener.BeginBuild(&playerRoadMap);
				if(!gener.EndBuild(szSavePath,szMapName,_T("playerroadpatchpathdata")))
				{
					pUI->AddMsg(_T("失败.\r\n"));
					return false;
				}
			}
			pUI->AddMsg(_T("ok.\r\n"));
		}

		return true;
	}


	const TCHAR* const NavEditorBasePath=_T(".\\Data\\NavEditor");

	bool NavMapEditor::Save( const TCHAR *szMapName,BitMap *pManualCangoMap,BitMap *pPlayerRoadMap )
	{
		ASSERT(szMapName!=NULL && szMapName[0]!=0);
		ASSERT(pManualCangoMap!=NULL);

		//--保存manualcangomap
		::SetCurrentDirectory(Kernel::Inst()->GetWorkPath());
		TCHAR szPath[512];
		_stprintf(szPath,_T("%s\\%s"),NavEditorBasePath,szMapName);
		::CreateDirectory(NavEditorBasePath,NULL);
		::CreateDirectory(szPath,NULL);
		_stprintf(szPath,_T("%s\\%s\\NEditor.nav"),NavEditorBasePath,szMapName);
		FILE *fp=_tfopen(szPath,_T("wb"));
		if(fp==NULL)
		{
			Kernel::Inst()->SetLastError(_T("NavEditor:导航图存盘文件打开失败:%s."),szPath);
			return false;
		}

		pManualCangoMap->WriteFile(fp);
		fclose(fp);

		//导出image
		_stprintf(szPath,_T("%s\\%s\\cango.bmp"),NavEditorBasePath,szMapName);
		pManualCangoMap->ExportImage(szPath);		

		//--保存roadmap
		_stprintf(szPath,_T("%s\\%s\\PlayerRoadMap.nav"),NavEditorBasePath,szMapName);
		fp=_tfopen(szPath,_T("wb"));
		if(fp==NULL)
		{
			Kernel::Inst()->SetLastError(_T("NavEditor:导航图存盘文件打开失败:%s."),szPath);
			return false;
		}

		pPlayerRoadMap->WriteFile(fp);
		fclose(fp);

		return true;
	}

	bool NavMapEditor::Load( const TCHAR *szMapName,BitMap *pManualCangoMap,BitMap *pPlayerRoadMap )
	{
		ASSERT(szMapName!=NULL && szMapName[0]!=0);
		ASSERT(pManualCangoMap!=NULL);

		::SetCurrentDirectory(Kernel::Inst()->GetWorkPath());
		TCHAR szPath[512];
		_stprintf(szPath,_T("%s\\%s\\NEditor.nav"),NavEditorBasePath,szMapName);

		//--
		IFS *pFS=Kernel::Inst()->DefaultFS();
		DWORD hFile=pFS->Open(szPath);
		if(hFile==NULL)
		{
			Kernel::Inst()->SetLastError(_T("NavEditor:导航图存盘文件打开失败:%s."),szPath);
			return false;
		}

		pManualCangoMap->ReadFile(pFS,hFile);
		pFS->Close(hFile);

		//--
		_stprintf(szPath,_T("%s\\%s\\PlayerRoadMap.nav"),NavEditorBasePath,szMapName);

		hFile=pFS->Open(szPath);
		if(hFile!=NULL)
		{
			pPlayerRoadMap->ReadFile(pFS,hFile);
			pFS->Close(hFile);
		}

		return true;
	}

	bool NavMapEditor::LoadNPCNavMap(const TCHAR *szMapName, BitMap *pNPCNavBMap, Heightmap* pNPCNavHMap)
	{
		ASSERT(szMapName!=NULL && szMapName[0]!=0);
		ASSERT(pNPCNavBMap!=NULL && pNPCNavHMap!=NULL);

		::SetCurrentDirectory(Kernel::Inst()->GetWorkPath());
		TCHAR szPath[512];
		_stprintf(szPath,_T("%s\\%s\\npccango.bmap"),NavEditorBasePath,szMapName);

		IFS *pFS=Kernel::Inst()->DefaultFS();
		DWORD hFile=pFS->Open(szPath);
		if(hFile==NULL)
		{
			Kernel::Inst()->SetLastError(_T("NavEditor:NPC格子导航图bit map加载失败:%s."),szPath);
			return false;
		}

		pNPCNavBMap->ReadFile(pFS,hFile);
		

		_stprintf(szPath,_T("%s\\%s\\npcnavmap.hmap"),NavEditorBasePath,szMapName);
		hFile=pFS->Open(szPath);
		if(hFile==NULL)
		{
			Kernel::Inst()->SetLastError(_T("NavEditor:NPC格子导航图height map加载失败:%s."),szPath);
			return false;
		}

		pNPCNavHMap->ReadFromFile(pFS,hFile);

		pFS->Close(hFile);
		return true;
	}

	bool NavMapEditor::LoadPlayerNavMap(const TCHAR *szMapName, BitMap *pPlayerBMap, Heightmap* pPlayerHMap)
	{
		ASSERT(szMapName!=NULL && szMapName[0]!=0);
		ASSERT(pPlayerBMap!=NULL && pPlayerHMap!=NULL);

		::SetCurrentDirectory(Kernel::Inst()->GetWorkPath());
		TCHAR szPath[512];
		_stprintf(szPath,_T("%s\\%s\\playercango.bmap"),NavEditorBasePath,szMapName);

		IFS *pFS=Kernel::Inst()->DefaultFS();
		DWORD hFile=pFS->Open(szPath);
		if(hFile==NULL)
		{
			Kernel::Inst()->SetLastError(_T("NavEditor:玩家寻路导航图bit map加载失败:%s."),szPath);
			return false;
		}

		pPlayerBMap->ReadFile(pFS,hFile);


		_stprintf(szPath,_T("%s\\%s\\playernavmap.hmap"),NavEditorBasePath,szMapName);
		hFile=pFS->Open(szPath);
		if(hFile==NULL)
		{
			Kernel::Inst()->SetLastError(_T("NavEditor:玩家寻路导航图height map加载失败:%s."),szPath);
			return false;
		}

		pPlayerHMap->ReadFromFile(pFS,hFile);

		pFS->Close(hFile);
		return true;
	}

	void NavMapEditor::BuildSetNPCNavMapBuildStartPoints( const list<tagNPCNavMapBuildStartPoint*>& points )
	{
		m_npcNavMapBuildStartPoints=points;
	}

	void NavMapEditor::BuildSetPlayerNavMapBuildStartPoints( const list<tagPlayerNavMapBuildStartPoint*>& points )
	{
		m_playerNavMapBuildStartPoints=points;
	}

	void NavMapEditor::BuildSetWaterArea( BitMap* pBitmap )
	{
		m_pWaterArea=pBitmap;
	}

	void NavMapEditor::BuildSetWaterHeight( Heightmap* pHMap )
	{
		m_pWaterHeight=pHMap;
	}

	bool NavMapEditor::SaveWaterData(const TCHAR* szSavePath,const TCHAR* szMapName)
	{
		::SetCurrentDirectory(Kernel::Inst()->GetWorkPath());

		//--保存水面区域数据
		TCHAR szPath[512];
		_stprintf(szPath,_T("%snav\\waterarea.bmap"),szSavePath);
		FILE* fp=_tfopen(szPath,_T("wb"));
		if(fp==NULL)
			return false;
		m_pWaterArea->WriteFile(fp);
		fclose(fp);
		
		//--保存水面高度数据
		_stprintf(szPath,_T("%snav\\waterheight.hmap"),szSavePath);
		fp=_tfopen(szPath,_T("wb"));
		if(fp==NULL)
			return false;
		m_pWaterHeight->WriteToFile(fp);
		fclose(fp);

		//--生成预览信息
		::CreateDirectory(NavEditorBasePath,NULL);
		_stprintf(szPath,_T("%s\\%s"),NavEditorBasePath,szMapName);
		::CreateDirectory(szPath,NULL);

		//bitmap
		//m_pWaterArea->SafeSetValue(0,0,true);//test
		_stprintf(szPath,_T("%s\\%s\\waterarea.bmp"),NavEditorBasePath,szMapName);
		m_pWaterArea->ExportImage(szPath);

		return true;
	}

	
}//namespace WorldBase