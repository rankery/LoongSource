#pragma once
#include "..\Math\GeomDefine.h"

namespace F3D
{
	class EntityNode;
	class EffectNode;
	class Frustum;
	/**	\class QuadTreeNode 
		\brief 场景树形结构中的一个节点
	*/
	class QuadTreeNode
	{
	public:
		QuadTreeNode(QuadTreeNode *pFather);
		virtual ~QuadTreeNode(void);

		const vector<EffectNode*>& GetEffectNode() const
		{ return m_effectArray; }
		QuadTreeNode *GetChild(UINT i) const;

		bool IsInFrustum(const Frustum *pFrumstum) const;
		
	protected:
		QuadTreeNode		*m_pFather;
		QuadTreeNode		*m_pChildren[4];
		vector<EffectNode*> m_effectArray;//影响此节点渲染效果的节点

		AABBox				m_nodeBox;//此Node的Bounding box
	};
}//namespace F3D