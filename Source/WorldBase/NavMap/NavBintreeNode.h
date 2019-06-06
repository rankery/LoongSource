#pragma once

namespace WorldBase
{
	class NavBintreeCollider;
	/**	\class NavBintreeNode
		\brief 二叉树空间分割的节点
	*/
	class NavBintreeNode
	{
		enum { EChildNum = 2 };

	public:
		NavBintreeNode(void);
		virtual ~NavBintreeNode(void);

		void ReadFile(IFS *pFS,DWORD hFile);
		inline const AABBox& GetBox(){ return m_box;}

		bool AABBoxCollideBox(const AABBox& box,NavBintreeCollider* pCollider);
		bool AABBoxCollideBoxTopHighest(const AABBox& box,float& intersectY,NavBintreeCollider* pCollider);
		TResult RayCollideBox(const Ray& ray,NavBintreeCollider* pCollider);
		bool RayCollideBoxOnce(const Ray& ray,NavBintreeCollider* pCollider);

	protected:
		AABBox			m_box;
		vector<WORD>	m_content;
		WORD*			m_pContents;//用于优化debug版vector访问速度
		NavBintreeNode	*m_pChildren[EChildNum];
	};
}//namespace WorldBase