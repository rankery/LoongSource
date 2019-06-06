#pragma once
#include "..\Device\IDevice.h"
#include "imtlmodifier.h"
#include "imaterial.h"

namespace Cool3D
{
	class MMAlphaMask;
	/**	\class MMTransparent 
		\brief 修改材质alpha通道
	*/
	class Cool3D_API MMTransparent :	public IMtlModifier
	{
	public:
		MMTransparent(void);
		virtual ~MMTransparent(void);

		//--IMtlModifier
		virtual void Update(float deltaTime,IMaterial *pOwner);

		DECL_RTTI_DYNC(MMTransparent);
	};
}//namespace Cool3D