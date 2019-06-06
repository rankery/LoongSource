#pragma once
#include "imtlmodifier.h"

namespace Cool3D
{
	/** \class MMUsingMtlColor
		\brief 关闭灯光，使用材质中diffuse颜色
	*/
	class Cool3D_API MMUsingMtlColor :	public IMtlModifier
	{
	public:
		MMUsingMtlColor(void);
		virtual ~MMUsingMtlColor(void);

		//--IMtlModifier
		virtual void Update(float deltaTime,IMaterial *pOwner);

	public:
		Color4f m_diffuse;

		DECL_RTTI_DYNC(MMUsingMtlColor);
	};
}//namespace Cool3D