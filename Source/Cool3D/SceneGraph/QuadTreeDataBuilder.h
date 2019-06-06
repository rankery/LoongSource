#pragma once
#include "quadtreedata.h"

namespace Cool3D
{
	class SceneNode;
	class QuadTreeNodeBuilder;
	class QuadTreeDataBuilder :	public QuadTreeData
	{
	public:
		QuadTreeDataBuilder(void);
		virtual ~QuadTreeDataBuilder(void);

		void Begin();
		void AddNode(SceneNode *pNode);
		bool End(const TCHAR* szSaveFile,int& baseNodeID);

		int BuildGetNumNodes(){ return (int)m_allNode.size();}

	private:
		bool SaveToFile(const TCHAR* szFileName);
		void LogTreeInfo();
	protected:
		vector<SceneNode *>		m_allNode;
		QuadTreeNodeBuilder		*m_pRoot;
	};
}//namespace Cool3D