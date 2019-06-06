#pragma once
#include "NavCollider.h"
#include "NPCNavMap.h"
#include "PlayerNavMap.h"
#include "NPCNavGraph.h"

namespace WorldBase
{
	/** \class NavMap
		\breif 导航图
	*/
	class WORLD_BASE_API NavMap
	{
	public:
		enum ELoadOptions			//加载选项
		{
			ELO_Collider=1,			//碰撞数据
			ELO_NPCNavMap=2,		//NPC格子导航图
			ELO_NPCNavGraph=4,		//NPC导航网格
			ELO_PlayerNavMap=8,		//玩家自动寻路导航图
			ELO_GroundType=16,		//地表类型
			ELO_NPCNavMapDoors=32,	//NPC格子导航图门数据

			ELO_All=0xFFFFFFFF
		};
	public:
		NavMap(void);
		~NavMap(void);

		/** 加载导航图数据
		*/
		void LoadFromFile(const TCHAR* szMapName,IFS* pMapFS,IFS* pSysFS,DWORD loadOptions=ELO_Collider);
		/** 释放数据
		*/
		void Destroy();
		/** 设置可见区域大小
		*/
		void SetViewZoneSize(const Vector3& size);
		/** 更新可见区域
		*/
		void InvalidViewZone(const Vector3& zoneCenter);
		/** 克隆对象
		*/
		void Clone(NavMap* pTarget);
		/** 获得碰撞检测器
		*/
		NavCollider* GetCollider(){ return &m_collider;}
		/** 获得NPC格子导航图
		*/
		NPCNavMap*		GetNPCNavMap()	{ return &m_npcNavMap;}
		/** 获得基于路点的NPC导航网格(NavMesh)
		*/
		NPCNavGraph*	GetNPCNavGraph(){ return &m_npcNavGraph;}
		/** 获得玩家自动寻路导航图
		*/
		PlayerNavMap* GetPlayerNavMap(){ return &m_playerNavMap;}

	private:
		NavCollider		m_collider;		//碰撞检测器
		NPCNavMap		m_npcNavMap;	//NPC格子导航图
		PlayerNavMap	m_playerNavMap;	//玩家自动寻路导航图
		NPCNavGraph		m_npcNavGraph;	//NPC导航网格
		DWORD			m_loadOptions; 
	};

}//namespace WorldBase
