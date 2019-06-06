#include "StdAfx.h"
#include "NavColliderDataGen.h"
#include "NavEntityNode.h"
#include "NavCarrierNode.h"
#include "NavQuadTreeDataBuilder.h"
#include "..\GameMap\GameMap.h"
#include "NavSurface.h"
#include "HalfByteMap.h"
#include "NavGenBuilder.h"
#include "..\..\Cool3D\Util\FileUtil.h"

namespace WorldBase
{
	NavColliderDataGen::NavColliderDataGen(void)
	{
	}

	NavColliderDataGen::~NavColliderDataGen(void)
	{
	}

	void NavColliderDataGen::BeginBuild(Heightmap* pHMap,BitMap* pManualCangoMap,Heightmap* pGroundType)
	{
		m_pHMap=pHMap;
		m_pManualCangoMap=pManualCangoMap;
		m_pGroundType=pGroundType;
	}

	bool NavColliderDataGen::EndBuild( const TCHAR* szSavePath,const TCHAR* szMapName )
	{
		//--保存heightmap
		TCHAR szHMapFile[512];
		_stprintf(szHMapFile,_T("%snav\\terrain.hmap"),szSavePath);
		FILE* fp=_tfopen(szHMapFile,_T("wb"));
		if(fp!=NULL)
		{
			m_pHMap->WriteToFile(fp);
			fclose(fp);
		}
		else
		{
			return false;
		}

		//--删掉nav目录下的*.nav
		TCHAR szNavFile[512];
		_stprintf(szNavFile,_T("%snav\\*.nav"),szSavePath);
		DeleteFiles(szNavFile);

		//--生成地表类型
		if(!BuildGroundTypeMap(szSavePath,szMapName))
			return false;

		//--生成QuadTreeSG
		if(!BuildQuadTreeSG(szSavePath,szMapName))
			return false;

		//--保存不可行走区域
		if(!BuildCanGoMap(szSavePath))
			return false;

		//--生成modelmap
		if(!BuildModelMap(szSavePath,szMapName))
			return false;

		return true;
	}

	bool NavColliderDataGen::BuildQuadTreeSG( const TCHAR* szSavePath,const TCHAR* szMapName )
	{
		//--加载节点
		NavQuadTreeDataBuilder navBuilder;
		navBuilder.Begin();

		//--添加地物节点
		vector<NavEntityNode*> entityNodes;
		LoadEntityNodes(szSavePath,szMapName,entityNodes);
		for(int i=0;i<(int)entityNodes.size();i++)
		{
			navBuilder.AddNode(entityNodes[i]);
		}

		//--
		TCHAR szPath[512];
		_stprintf(szPath,_T("%snav\\navmap.fsg"),szSavePath);
		if(!navBuilder.End(szPath))
			return false;

		return true;
	}

	bool NavColliderDataGen::BuildModelMap( const TCHAR* szSavePath,const TCHAR* szMapName )
	{
		BitMap modelMap;
		modelMap.Create(m_pHMap->Width(),m_pHMap->Height(),false);

		vector<NavEntityNode*> entityNodes;
		LoadEntityNodes(szSavePath,szMapName,entityNodes);
		for(int i=0;i<(int)entityNodes.size();i++)
		{
			NavEntityNode* pNode=entityNodes[i];

			AABBox box;
			pNode->GetBox(box);

			POINT pt1=m_pHMap->World2Tile(box.min);
			POINT pt2=m_pHMap->World2Tile(box.max);
			for(int z=pt1.y;z<=pt2.y;z++)
			{
				for(int x=pt1.x;x<=pt2.x;x++)
				{
					modelMap.SafeSetValue(x,z,true);
				}
			}

			SAFE_DELETE(pNode);
		}


		TCHAR szPath[512];
		_stprintf(szPath,_T("%sNav\\model.bmap"),szSavePath);
		FILE* fp=_tfopen(szPath,_T("wb"));
		if(fp==NULL)
			return false;
		modelMap.WriteFile(fp);
		fclose(fp);
		return true;
	}

	bool NavColliderDataGen::BuildCanGoMap( const TCHAR* szSavePath )
	{
		TCHAR szPath[512];
		_stprintf(szPath,_T("%sNav\\cango.bmap"),szSavePath);
		FILE* fp=_tfopen(szPath,_T("wb"));
		if(fp==NULL)
			return false;
		m_pManualCangoMap->WriteFile(fp);
		fclose(fp);

		return true;
	}

	inline ESurfaceType _ConvertSurfaceType(int type)
	{
		switch(type)
		{
		case 0:	return EST_None;
		case 1:	return EST_Snow;
		case 2:	return EST_Rock;
		case 3:	return EST_Earth;
		case 4:	return EST_Grass;
		case 5:	return EST_Sand;
		case 6:	return EST_Ice;
		case 7:	return EST_Metal;
		case 8:	return EST_Wood;
		}
		return EST_Earth;
	}

	bool NavColliderDataGen::BuildGroundTypeMap( const TCHAR* szSavePath,const TCHAR* szMapName )
	{
		HalfByteMap groundTypeMap;
		groundTypeMap.Create(m_pGroundType->Width(),m_pGroundType->Height(),0);
		for(UINT y=0;y<groundTypeMap.Height();y++)
		{
			for(UINT x=0;x<groundTypeMap.Width();x++)
			{
				int type=m_pGroundType->GetValue(x,y);
				groundTypeMap.SetValue(x,y,_ConvertSurfaceType(type));
			}
		}

		TCHAR szPath[512];
		_stprintf(szPath,_T("%sNav\\groundtype.data"),szSavePath);
		FILE* fp=_tfopen(szPath,_T("wb"));
		if(fp==NULL)
			return false;
		groundTypeMap.WriteFile(fp);
		fclose(fp);

		//for test
		//groundTypeMap.ExportImage(_T("d:\\groundtype.bmp"));

		return true;
	}

	bool _BuildCarrierWorldBox( const tagMapCarrier& carrier,NavCarrierNode* pCarrier,GameMap& gMap,AABBox& out )
	{
		bool bResult=false;

		out.max=Vector3(-FLOAT_MAX,-FLOAT_MAX,-FLOAT_MAX);
		out.min=Vector3(FLOAT_MAX,FLOAT_MAX,FLOAT_MAX);

		const vector<tagMapWayPoint>& wayPoints=gMap.GetWayPoints();
		for(size_t i=0;i<wayPoints.size();++i)
		{
			const tagMapWayPoint& wayPoint=wayPoints[i];
			if(_tcscmp(wayPoint.szName,carrier.szWayPoint)==0)
			{
				AABBox box;

				Transform trans;
				trans.Rotate(carrier.fYaw,0.0f,0.0f);
				trans.Translate(wayPoint.vPos);

				pCarrier->SetWorldMat(*trans.GetMatrix());
				pCarrier->GetBox(box);

				out.Merge(box);
				bResult=true;
			}
		}

		return bResult;
	}

	void NavColliderDataGen::LoadEntityNodes( const TCHAR* szSavePath,const TCHAR* szMapName,vector<NavEntityNode*>& out )
	{
		TCHAR szMapFile[512];
		GameMap gMap;
		_stprintf(szMapFile,_T("%s%s.mb"),szSavePath,szMapName);
		gMap.LoadFromFile(Kernel::Inst()->DefaultFS(),szMapFile);

		//--static objects
		int i;
		const vector<tagStaticMapObj>& staticObjs=gMap.GetStaticMapObjs();
		for(i=0;i<(int)staticObjs.size();i++)
		{
			const tagStaticMapObj& obj=staticObjs[i];

			TCHAR szDrive[512];
			TCHAR szDir[512];
			TCHAR szFilePath[512];
			TCHAR szFileExt[20];
			_tsplitpath(obj.szMdlPath,szDrive,szDir,szFilePath,szFileExt);

			//--fscn
			if(_tcsicmp(szFileExt,_T(".fscn"))==0)
			{
				NavEntityNode *pNode=NULL;

				Transform trans;
				trans.Scale(obj.fScale[0],obj.fScale[1],obj.fScale[2]);
				trans.Rotate(obj.fRotate[0],obj.fRotate[1],obj.fRotate[2]);
				Vector3 pos(obj.fPos[0],obj.fPos[1],obj.fPos[2]);
				trans.Translate(pos);

				if( obj.fRotate[1]>0.01f || obj.fRotate[1]<-0.01f 
					|| obj.fRotate[2]>0.01f || obj.fRotate[2]<-0.01f )//模型绕x轴或者z轴旋转超过1度，需要重新生成nav
				{
					// 生成新的Nav路径
					TCHAR szNavPath[512];
					_stprintf(szNavPath,_T("%sNav\\%s_%d.Nav"),szSavePath,szFilePath,obj.dwMapID);
					if(_taccess(szNavPath,4)==-1)//file not exist(not readable)
					{
						EntityNode node;
						node.LoadRes(obj.szMdlPath);
						node.SetWorldMat(trans.GetMatrix());

						VertStream vb;
						vector<WORD> ib;
						node.GetPosBuffer(&vb);
						node.GetIndexBuffer(ib);

						NavGenBuilder navGen;
						navGen.SetBoxSize((NavGenBuilder::EBoxSize)obj.byBoxSize);
						navGen.Build(&vb,ib);
						navGen.WriteFile(szNavPath);
					}

					pNode=new NavEntityNode;
					pNode->LoadRes(szNavPath);

					// 由于aabbox已经是世界空间的，所以转换矩阵设置为单位矩阵
					Matrix4 identityMat;
					D3DXMatrixIdentity(&identityMat);
					pNode->SetWorldMat(identityMat);
					pNode->SetScale(1.0f,1.0f,1.0f);
				}
				else
				{
					TCHAR szNavPath[512];
					_stprintf(szNavPath,_T("%s%s%s.Nav"),szDrive,szDir,szFilePath);
					if(_taccess(szNavPath,4)==-1)//file not exist(not readable)
						continue;

					pNode=new NavEntityNode;
					pNode->LoadRes(szNavPath);
					pNode->SetWorldMat(*trans.GetMatrix());
					pNode->SetScale(obj.fScale[0],obj.fScale[1],obj.fScale[2]);
				}

				pNode->SetSurfaceType(_ConvertSurfaceType(obj.bSurfaceType));

				DWORD dwFlags=0;
				if(obj.bPathfinding)
					dwFlags|=NavSceneNode::EFlag_SupportPlayerPathFind;
				if(!obj.bNotSupportNPCNavMap)
					dwFlags|=NavSceneNode::EFlag_SupportNPCNavMap;
				dwFlags|=NavSceneNode::EFlag_NotDoor;
				dwFlags|=NavSceneNode::EFlag_NotCarrier;
				pNode->SetFlags(dwFlags);

				out.push_back(pNode);
			}
		}

		//--doors
		const vector<tagMapDoor>& doors=gMap.GetMapDoor();
		for(i=0;i<(int)doors.size();i++)
		{
			const tagMapDoor& door=doors[i];

			TCHAR szDrive[512];
			TCHAR szDir[512];
			TCHAR szFilePath[512];
			TCHAR szFileExt[20];
			_tsplitpath(door.szModelPath,szDrive,szDir,szFilePath,szFileExt);

			//--fak
			if(_tcsicmp(szFileExt,_T(".fak"))==0)
			{
				TCHAR szNavPath[512];
				_stprintf(szNavPath,_T("%s%s%s.Nav"),szDrive,szDir,szFilePath);
				if(_taccess(szNavPath,4)==-1)//file not exist(not readable)
					continue;

				NavEntityNode *pNode=new NavEntityNode;
				pNode->LoadRes(szNavPath);

				Transform trans;
				trans.Scale(door.fScale[0],door.fScale[1],door.fScale[2]);
				trans.Rotate(door.fYaw,0.0f,0.0f);
				trans.Translate(door.vPos);

				pNode->SetWorldMat(*trans.GetMatrix());
				pNode->SetScale(door.fScale[0],door.fScale[1],door.fScale[2]);
				pNode->SetSurfaceType(EST_None);

				//flags
				DWORD dwFlags=0;
				dwFlags|=NavSceneNode::EFlag_Door;
				dwFlags|=NavSceneNode::EFlag_NotCarrier;
				pNode->SetFlags(dwFlags);

				//objid
				pNode->SetObjID(door.dwObjID);

				out.push_back(pNode);
			}
		}

		//--carrier
		const vector<tagMapCarrier>& carriers=gMap.GetMapCarrier();
		for(i=0;i<(int)carriers.size();i++)
		{
			const tagMapCarrier& carrier=carriers[i];

			TCHAR szDrive[512];
			TCHAR szDir[512];
			TCHAR szFilePath[512];
			TCHAR szFileExt[20];
			_tsplitpath(carrier.szModelPath,szDrive,szDir,szFilePath,szFileExt);

			//--fscn
			if(_tcsicmp(szFileExt,_T(".fscn"))==0)
			{
				TCHAR szNavPath[512];
				_stprintf(szNavPath,_T("%s%s%s.Nav"),szDrive,szDir,szFilePath);
				if(_taccess(szNavPath,4)==-1)//file not exist(not readable)
					continue;

				NavCarrierNode *pNode=new NavCarrierNode;
				pNode->LoadRes(szNavPath);

				Transform trans;
				trans.Rotate(carrier.fYaw,0.0f,0.0f);

				pNode->SetWorldMat(*trans.GetMatrix());
				pNode->SetSurfaceType(EST_None);

				//flags
				DWORD dwFlags=0;
				dwFlags|=NavSceneNode::EFlag_Carrier;
				dwFlags|=NavSceneNode::EFlag_NotDoor;
				pNode->SetFlags(dwFlags);

				//objid
				pNode->SetObjID(carrier.dwObjID);

				//worldbox
				AABBox worldBox;
				if(!_BuildCarrierWorldBox(carrier,pNode,gMap,worldBox))
				{
					SAFE_DELETE(pNode);
					continue;
				}
				pNode->SetWorldBox(worldBox);

				out.push_back(pNode);
			}
		}
	}

}//namespace WorldBase

