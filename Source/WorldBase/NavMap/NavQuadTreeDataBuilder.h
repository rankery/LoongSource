#pragma once
#include "navquadtreedata.h"

namespace WorldBase
{
	class NavSceneNode;
	class NavQuadTreeNodeBuilder;
	class NavQuadTreeDataBuilder :	public NavQuadTreeData
	{
	public:
		NavQuadTreeDataBuilder(void);
		virtual ~NavQuadTreeDataBuilder(void);

		void Begin();
		void AddNode(NavSceneNode *pNode);
		bool End(const TCHAR* szSaveFile);

		int BuildGetNumNodes(){ return (int)m_allNode.size();}

	private:
		bool SaveToFile(const TCHAR* szFileName);
		void LogTreeInfo();
	private:
		vector<NavSceneNode *>		m_allNode;
		NavQuadTreeNodeBuilder*		m_pRoot;
	};
}//namespace WorldBase