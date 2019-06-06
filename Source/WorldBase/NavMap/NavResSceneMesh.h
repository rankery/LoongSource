#pragma once
#include "NavResBase.h"

namespace WorldBase
{
	class NavResSceneMesh : public NavResBase
	{
	public:
		NavResSceneMesh(const TCHAR* szName);
		virtual ~NavResSceneMesh();

		//--NavResBase
		virtual UINT CreateContent(DWORD param);

		//--
		const AABBox& GetBox();		
		bool AABBoxCollideBox(const AABBox& box);
		bool AABBoxCollideBoxTopHighest(const AABBox& box,float& intersectY);
		TResult RayCollideBox(const Ray& ray);
		bool RayCollideBoxOnce(const Ray& ray);

		void DrawBox();

	private:
		class Member;
		Member *m_p;

		DECL_RTTI(NavResSceneMesh);
	};
}//namespace WorldBase