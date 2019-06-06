#pragma once
#include "NavBaseCollider.h"

namespace WorldBase
{
	class NavBintreeNode;
	/**	\class NavBintreeCollider
		\brief ¶þ²æÊ÷µÄÅö×²¼ì²âÆ÷
	*/
	class NavBintreeCollider : public NavBaseCollider
	{
	public:
		NavBintreeCollider();
		virtual ~NavBintreeCollider();

		virtual void ReadFile(IFS *pFS,DWORD hFile);
		virtual const AABBox& GetBigBox();

		virtual bool AABBoxCollideBox(const AABBox& box);
		virtual bool AABBoxCollideBoxTopHighest(const AABBox& box,float& intersectY);
		virtual TResult RayCollideBox(const Ray& ray);
		virtual bool RayCollideBoxOnce(const Ray& ray);

	private:
		NavBintreeNode*		m_pRoot;
	};
}//namespace WorldBase