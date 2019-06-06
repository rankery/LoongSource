#include "StdAfx.h"
#include ".\gamemap.h"

namespace WorldBase
{
	GameMap::GameMap(void)
	{
		_tcscpy_s(m_szSkyBoxTopTex,sizeof(m_szSkyBoxTopTex)/sizeof(TCHAR), _T(""));
		_tcscpy_s(m_szSkyBoxFrontRightTex,sizeof(m_szSkyBoxFrontRightTex)/sizeof(TCHAR), _T(""));
		_tcscpy_s(m_szSkyBoxBackLeftTex,sizeof(m_szSkyBoxBackLeftTex)/sizeof(TCHAR), _T(""));
	}

	GameMap::~GameMap(void)
	{
		Destroy();
	}

	void GameMap::LoadFromFile(IFS* pFS,const TCHAR* szFileName,DWORD options)
	{
		m_staticMapObjs.clear();
		m_npcs.clear();
		m_waypoints.clear();
		m_triggers.clear();
		m_sounds.clear();
		m_PointLight.clear();
		m_MapRect.clear();
		m_PathPoint.clear();
		m_SpawnPoint.clear();
		m_DynamicBlock.clear();
		m_EventTriggers.clear();
		m_MapAreaEx.clear();
		m_MapTriggerEffect.clear();

		//load header
		DWORD hFile=pFS->Open(szFileName);
		THROW_NULLEX(hFile,_T("GameMap open file failed."),szFileName);

		tagMapHeader header;
		pFS->Read(&header,sizeof(tagMapHeader),hFile);

		ASSERT(sizeof(m_Fog)==sizeof(header.byDistFog));
		memcpy(&m_Fog,header.byDistFog,sizeof(m_Fog));
		ASSERT(sizeof(m_SunLight)==sizeof(header.bySunLight));
		memcpy(&m_SunLight,header.bySunLight,sizeof(m_SunLight));

		//方向光diffuse乘以强度
		m_SunLight.diffuse.R*=header.fSunModulus;
		m_SunLight.diffuse.G*=header.fSunModulus;
		m_SunLight.diffuse.B*=header.fSunModulus;

		_tcscpy_s(m_szSkyBoxTopTex, sizeof(m_szSkyBoxTopTex)/sizeof(TCHAR), header.szSkyBoxTopTex);
		_tcscpy_s(m_szSkyBoxFrontRightTex, sizeof(m_szSkyBoxFrontRightTex)/sizeof(TCHAR), header.szSkyBoxFrontRightTex);
		_tcscpy_s(m_szSkyBoxBackLeftTex, sizeof(m_szSkyBoxBackLeftTex)/sizeof(TCHAR), header.szSkyBoxBackLeftTex);
		m_fSkyBoxXsize = header.fSkyBoxXsize;
		m_fSkyBoxYsize = header.fSkyBoxYsize;
		m_fSkyBoxZsize = header.fSkyBoxZsize;
		m_fSkyBoxOffX = header.fSkyBoxOffX;
		m_fSkyBoxOffY = header.fSkyBoxOffY;
		m_fSkyBoxOffZ = header.fSkyBoxOffZ;
		m_fSunModulus = header.fSunModulus;
		m_fSkyYaw = header.fSkyYaw;

		ASSERT(sizeof(m_SkyCol) == sizeof(header.bySkyCol));
		memcpy(&m_SkyCol, header.bySkyCol, sizeof(m_SkyCol));

		m_bRenderSkyShade = ( 0 != header.byRenderSkyShade );
		m_dwSkyShadeCol = header.dwSkyShadeCol;
		m_dwDynamicDiffCol = header.dwDynamicDiffCol;
		m_dwDynamicAmbCol = header.dwDynamicAmbCol;
		m_dwDynamicSpecCol = header.dwDynamicSpecCol;

		int i;

		//--load npc
		if(options&ELO_Npc)
		{
			pFS->Seek(hFile,header.dwNpcOffset,FILE_BEGIN);
			for(i = 0; i < header.nNumNPC; i++)
			{
				tagMapNPC npc;
				pFS->Read(&npc, sizeof(npc),hFile);

				m_npcs[npc.dwObjID]=npc;
			}
		}

		//load static objs
		if(options&ELO_Static)
		{
			pFS->Seek(hFile,header.dwStaticObjOffset,FILE_BEGIN);
			for(i=0;i<header.nNumStaticObj;i++)
			{
				tagStaticMapObj staticObj;
				pFS->Read(&staticObj,sizeof(staticObj),hFile);

				m_staticMapObjs.push_back(staticObj);
			}
		}

		//load waypoint
		if(options&ELO_WayPoint)
		{
			pFS->Seek(hFile,header.dwWayPointOffset,FILE_BEGIN);
			for(i=0;i<header.nNumWayPoint;i++)
			{
				tagMapWayPoint waypoint;
				pFS->Read(&waypoint,sizeof(waypoint),hFile);
				m_waypoints.push_back(waypoint);
			}
		}

		//load triggers
		if(options&ELO_Trigger)
		{
			pFS->Seek(hFile,header.dwTriggerOffset,FILE_BEGIN);
			for(i=0;i<header.nNumTrigger;i++)
			{
				tagMapTrigger pTriggerObj;
				/*pFS->Read(&trigger,sizeof(trigger),hFile);*/
				tstring szTemp;
				FReadValue(pFS, hFile, pTriggerObj.dwObjID);
				FReadValue(pFS, hFile, pTriggerObj.eType);
				FReadValVector(pFS, hFile, pTriggerObj.region);
				FReadValue(pFS, hFile, pTriggerObj.box.max);
				FReadValue(pFS, hFile, pTriggerObj.box.min);
				FReadValue(pFS, hFile, pTriggerObj.fHeight);

				pFS->Read(pTriggerObj.szMapName, sizeof(pTriggerObj.szMapName), hFile);
				pFS->Read(pTriggerObj.szWayPoint, sizeof(pTriggerObj.szWayPoint), hFile);
				pFS->Read(pTriggerObj.szScriptName, sizeof(pTriggerObj.szScriptName), hFile);

				FReadValue(pFS, hFile, pTriggerObj.dwParam);
				FReadValue(pFS, hFile, pTriggerObj.bLock);
				FReadValue(pFS, hFile, pTriggerObj.bFlag);
				FReadValue(pFS, hFile, pTriggerObj.dwQuestSerial);
				pFS->Read(pTriggerObj.byReserve, sizeof(pTriggerObj.byReserve), hFile);
				m_triggers.push_back(pTriggerObj);
			}
		}

		//load sound
		if(options&ELO_Sound)
		{
			pFS->Seek(hFile,header.dwSoundOffset,FILE_BEGIN);
			for(i=0;i<header.nNumSound;i++)
			{
				tagMapSound sound;
				pFS->Read(&sound,sizeof(sound),hFile);
				m_sounds.push_back(sound);
			}
		}

		//load pointlight
		if(options&ELO_PointLight)
		{
			pFS->Seek(hFile,header.dwPointLightOffset,FILE_BEGIN);
			for(i=0;i<header.nNumPointLight;i++)
			{
				tagMapPointLight pointlight;
				pFS->Read(&pointlight,sizeof(pointlight),hFile);
				m_PointLight.push_back(pointlight);
			}	
		}

		//load maprect by add xtian 2008-5-13
		if(options&ELO_MapRect)
		{
			pFS->Seek(hFile, header.dwMapRectOffset, FILE_BEGIN);
			for(i=0; i<header.nNumMapRect; i++)
			{
				tagMapArea mapRect;
				/*pFS->Read(&maprect, sizeof(maprect), hFile);*/
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
				m_MapRect.push_back(mapRect);
			}

			//--额外信息
			XMLReader varAreaEx;
			Filename mbPath = szFileName;
			TCHAR szMapAreaExPath[256];
			_stprintf( szMapAreaExPath, _T("%smaparea.xml"), mbPath.GetPath().c_str() );

			list<tstring> ExFieldList;
			list<tstring>::iterator iter;
			if(varAreaEx.Load(pFS, szMapAreaExPath, "id", &ExFieldList))
			{
				for(iter = ExFieldList.begin(); iter != ExFieldList.end(); ++iter)
				{
					tagMapAreaEx areaEx;
					areaEx.dwObjID		= varAreaEx.GetDword(_T("id"), (*iter).c_str(), -1);
					areaEx.strTitle		= varAreaEx.GetString(_T("title"), (*iter).c_str(), _T(""));
					areaEx.wInterval	= (WORD)varAreaEx.GetDword(_T("interval"), (*iter).c_str(), 0);
					areaEx.byVol		= (BYTE)varAreaEx.GetDword(_T("volume"), (*iter).c_str(), 100);
					areaEx.byPriority	= (BYTE)varAreaEx.GetDword(_T("priority"), (*iter).c_str(), 1);

					for( int musici=0; musici<3; ++musici )
					{
						TCHAR szBuff[32];
						_stprintf( szBuff, _T("music%d"), musici );
						areaEx.strMusic[musici]	= varAreaEx.GetString(szBuff, (*iter).c_str(), _T(""));
					}

					m_MapAreaEx.insert(make_pair(areaEx.dwObjID,areaEx));
				}
			}
		}

		//load pathpoint by add xtian 2008-8-6
		if(options&ELO_PathPoint)
		{
			pFS->Seek(hFile, header.dwPathPointOffset, FILE_BEGIN);
			for(int i=0; i<header.nNumPathPoint; i++)
			{
				tagMapPathPoint pathpoint;
				pFS->Read(&pathpoint, sizeof(tagMapPathPoint), hFile);
				m_PathPoint.push_back(pathpoint);
			}
		}

		//load spawnpoint by add xtian 2008-8-11
		if(options&ELO_SpawnPoint)
		{
			pFS->Seek(hFile, header.dwSpawnPointOffset, FILE_BEGIN);
			for(int i=0; i<header.nNumSpawnPoint; i++)
			{
				tagMapSpawnPoint spawnpoint;
				pFS->Read(&spawnpoint, sizeof(tagMapSpawnPoint), hFile);
				m_SpawnPoint.push_back(spawnpoint);
			}
		}

		//load dynamicblock by add xtian 2008-8-11
		if(options&ELO_DynamicBlock)
		{
			pFS->Seek(hFile, header.dwDynamicBlockOffset, FILE_BEGIN);
			for(int i=0; i<header.nNumDynamicBlock; i++)
			{
				tagMapDynamicBlock dynaBlock;
				pFS->Read(&dynaBlock, sizeof(tagMapDynamicBlock), hFile);
				m_DynamicBlock.push_back(dynaBlock);
			}
		}

		if(options&ELO_EventTrigger)
		{
			pFS->Seek(hFile, header.dwEventTriggerOffset, FILE_BEGIN);
			for(int i=0; i<header.nNumEventTrigger; i++)
			{
				tagMapEventTrigger eventTrigger;
				pFS->Read(&eventTrigger, sizeof(tagMapEventTrigger), hFile);
				m_EventTriggers.push_back(eventTrigger);
			}
		}

		if(options&ELO_MapDoor)
		{
			pFS->Seek(hFile, header.dwMapDoorOffset, FILE_BEGIN);
			for(int i=0; i<header.nNumMapDoor; i++)
			{
				tagMapDoor door;
				pFS->Read(&door, sizeof(tagMapDoor), hFile);
				m_MapDoor.push_back(door);
			}
		}

		if(options&ELO_MapCarrier) 
		{
			pFS->Seek(hFile, header.dwMapCarrierOffset, FILE_BEGIN);
			for(int i=0; i<header.nNumMapCarrier; i++)
			{
				tagMapCarrier carrier;
				pFS->Read(&carrier, sizeof(tagMapCarrier), hFile);
				m_MapCarrier.push_back(carrier);
			}
		}

		if(options&ELO_MapTriggerEffect)
		{
			pFS->Seek(hFile,header.dwMapTriggerEffectOffset,FILE_BEGIN);
			for(i=0;i<header.nNumMapTriggerEffect;i++)
			{
				tagMapTriggerEffect obj;
				pFS->Read(&obj,sizeof(obj),hFile);

				m_MapTriggerEffect.push_back(obj);
			}
		}

		//--close file
		pFS->Close(hFile);
	}

	void GameMap::Destroy()
	{
		
	}

	/*const tagDynamicMapObj* GameMap:: FindDynamicMapObj(DWORD mapID)
	{
		map<DWORD, tagDynamicMapObj>::iterator pIter = m_dynamicMapObjs.find(mapID);
		if(pIter != m_dynamicMapObjs.end())
			return &pIter->second;
		return NULL;
			
	}*/
	const tagMapNPC* GameMap:: FindMapNpc(DWORD mapID)
	{
		map<DWORD, tagMapNPC>::iterator pIter = m_npcs.find(mapID);
		if(pIter != m_npcs.end())
			return &pIter->second;
		return NULL;
	}

	const tagStaticMapObj* GameMap::FindStaticMapObj(DWORD mapID)
	{
		vector<tagStaticMapObj>::iterator pIter = m_staticMapObjs.begin();
		for(; pIter != m_staticMapObjs.end(); ++pIter)
			if(mapID == (*pIter).dwMapID)
				return &(*pIter);
		return NULL;
	}

	const tagMapAreaEx* GameMap::FindMapAreaEx( DWORD areaID )
	{
		map<DWORD, tagMapAreaEx>::iterator pIter = m_MapAreaEx.find(areaID);
		if(pIter != m_MapAreaEx.end())
			return &pIter->second;
		return NULL;
	}

	bool GameMap::GetMapAreaFog( const int nAreaIndex, tagDistFog& fog )
	{
		ASSERT( nAreaIndex >=0 && nAreaIndex < (int)m_MapRect.size() );
		tagMapArea& area = m_MapRect[nAreaIndex];
		if( !area.bDistFog )
			return false;

		ASSERT( sizeof(fog) == sizeof(area.byDistFog) );
		memcpy( &fog, area.byDistFog, sizeof(fog) );
		return true;
	}
}//namespace WorldBase