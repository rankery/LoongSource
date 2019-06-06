
#include "..\Cool3D.h"
#include "GeomDefine.h"
#include "Math.h"

#pragma once

namespace Cool3D
{
	/**	\class Frustum
		\brief define of Frustum which handle Frustum culling
	*/
	class Cool3D_API Frustum
	{
	public:
		Frustum(void);
		~Frustum(void);

		void Extract(Matrix4 *pView,Matrix4 *pPrj);
		bool BoxInFrustum(const AABBox& box) const;
		bool BoxAllInFrustum(const AABBox& box) const;
		bool BoxInFrustum(const OBBox& box) const;
		bool SphereInFrustum(const Sphere& sphere) const;
		Plane& GetNearPlane() { return m_Planes[4]; }

	private:
		Plane m_Planes[6];
	};
}//endof namespace Cool3D
