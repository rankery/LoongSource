#pragma once
#include "..\..\WorldDefine\MapAttDefine.h"
#include "..\NavMap\NPCNavMapDoors.h"
#include "..\NavMap\NavCollider_NPCMove.h"

namespace WorldBase
{
	class NavCollider;
	/** \class NPCNavMapGen
		\brief NPC导航图生成器
	*/
	class NPCNavMapGen
	{
	public:
		NPCNavMapGen(void);
		~NPCNavMapGen(void);

		void BeginBuild(const list<tagNPCNavMapBuildStartPoint*>& points,const vector<tagMapDoor>& doors,Heightmap* pHMap,BitMap* pManualCangoMap,NavCollider* pCollider,float fNPCPassHeight);
		bool EndBuild(const TCHAR* szSavePath,const TCHAR* szMapName,IUICallBack* pUI);

		BitMap* GetCangoMap(){ return &m_cangoMap;}

	private:
		void FloodFill(Vector3& startPos,IUICallBack* pUI);
		void FloodClear(int x,int z);
		bool IfCanGoTile(int x1,int z1,float h1,int x2,int z2);
		bool IfCanGoTile(int x1,int z1,float h1,int x2,int z2,float& h2);
		bool IfCanGoTile(int x,int z);
		bool BuildDoors(const TCHAR* szSavePath,IUICallBack* pUI);
		NPCNavMapDoors::tagDoor* BuildDoor(DWORD dwObjID);

	private:
		NavCollider*		m_pCollider;
		NavCollider_NPCMove	m_moveCollider;
		Heightmap*			m_pHMap;
		BitMap*				m_pManualCangoMap;
		list<tagNPCNavMapBuildStartPoint*>	m_buildStartPoints;
		vector<tagMapDoor> 					m_buildDoors;
		float				m_fNPCPassHeight;//NPC通过高度

		BitMap				m_cangoMap;
		BitMap				m_visitMap;
	};
}//namespace WorldBase

