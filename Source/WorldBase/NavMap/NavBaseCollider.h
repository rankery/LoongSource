#pragma once

namespace WorldBase
{
	/**	\class NavBaseCollider
		\brief 碰撞检测器接口类
	*/
	class NavBaseCollider
	{
	public:
		NavBaseCollider();
		virtual ~NavBaseCollider();

		virtual void ReadFile(IFS *pFS,DWORD hFile) = 0;
		virtual const AABBox& GetBigBox() = 0;

		virtual bool AABBoxCollideBox(const AABBox& box) = 0;
		virtual bool AABBoxCollideBoxTopHighest(const AABBox& box,float& intersectY) = 0;
		virtual TResult RayCollideBox(const Ray& ray) = 0;
		virtual bool RayCollideBoxOnce(const Ray& ray) = 0;

		void DrawBox();
		inline const AABBox& GetCollideBox(WORD index)
		{
			ASSERT(index<m_boxes.size());
			return m_pBoxes[index];
		}

	protected:
		vector<AABBox>		m_boxes;
		AABBox*				m_pBoxes;//为了优化Debug版vector的访问速度
	};
}//namespace WorldBase