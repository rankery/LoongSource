#pragma once
#include "..\..\WorldDefine\creature_define.h"

/** class	CreatureData
	brief	怪物，NPC，地物，宠物原始属性结构存储
*/

namespace WorldBase
{
	class WORLD_BASE_API CreatureDataReader
	{
	public:
		CreatureDataReader(void);
		~CreatureDataReader(void);
			
		/** 载入属性文件
		*/
		void LoadFromFile();
		/** 获取NPC模型文件路径
		*/
		static tstring GetCreatureMdlPath(const tagCreatureProto *pNpcAtt);
		/** 获取NPC蒙皮文件路径
		*/
		static tstring GetCreatureSkinPath(const tagCreatureProto *pNpcAtt,int index);
		/** 查找NPC的原始属性
		*/
		const tagCreatureProto* FindCreatureAtt(DWORD attID)
		{
			return FindInMap(m_creatureMap,attID);
		}
		/** 获取所有的NPC原始属性
		*/
		void GetCreatureMap(map<DWORD,tagCreatureProto>& npcMap)
		{
			npcMap = m_creatureMap;
		}
		const map<DWORD,tagCreatureProto>* GetCreatureMap()
		{
			return &m_creatureMap;
		}

		tagCreatureProto* FindInMap(map<DWORD,tagCreatureProto>& dataMap,DWORD attID)
		{
			map<DWORD, tagCreatureProto> ::iterator pIter = dataMap.find(attID);
			if(pIter != dataMap.end())
				return &pIter->second;
			return NULL;
		}

	private:
		map<DWORD,tagCreatureProto>		m_creatureMap;
	};

} // namespace WorldBase
