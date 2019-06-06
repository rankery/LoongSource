#pragma once
#include "RenderSys.h"

namespace F3D
{
	/**	\class SpeedTreeRender
		\brief 针对speed tree系统对渲染节点数组进行优化
	*/
	class SpeedTreeRender
	{
	public:
		SpeedTreeRender(void);
		virtual ~SpeedTreeRender(void);

		void Clear();
		void AttachTree(const RenderItem& item);									 
		void RenderAllTrees(SceneGraph *pSG);

		void SetWindStrength(float str)	{	m_fWindStrength=str;}
		
		static void Init();
		static bool s_bWindEnable;
	protected:
		typedef vector<RenderItem> TreeType;
		vector<TreeType>	m_trees;
		float				m_fWindStrength;

	protected:
		void UpdateWind();
		void RenderTreeType(TreeType& treeType);
	};
}//namespace F3D