#pragma once
#include "..\Math\Math.h"

namespace Cool3D
{
	class Frustum;
	/**	\class Occluder
		\brief ΥΪµ²Με
	*/
	class Occluder
	{
	public:
		Occluder(void);
		virtual ~Occluder(void);

		virtual bool IsInFrustum(const Frustum *pFrumstum) const =0;
		virtual void Transform(const Matrix4& toWorld) =0;
	};
}//namespace Cool3D