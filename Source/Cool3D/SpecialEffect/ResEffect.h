#pragma once
#include "..\ressys\resrenderable.h"

namespace F3D
{
	/**	ResEffect or SGEffectNode?????
	*/
	class ResEffect : public ResRenderable
	{
	public:
		ResEffect(const char* szName);
		virtual ~ResEffect(void);
	};
}//namespace F3D