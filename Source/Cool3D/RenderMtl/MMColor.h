#pragma once
#include "..\Device\IDevice.h"
#include "imtlmodifier.h"

namespace Cool3D
{
	/**	\class MMColor 
		\brief 修改材质颜色（不包括alpha通道）
	*/
	class Cool3D_API MMColor :	public IMtlModifier
	{
	public:
		MMColor(void);
		virtual ~MMColor(void);

		//--
		void SetShadowEffect(float shadowDepth,IMaterial* pOwner);
		void RemoveShadowEffect(IMaterial* pOwner);

	private:
		void Update(IMaterial *pOwner);

	public:
		Vector3 m_diffuse;
		Vector3 m_ambient;

	private:
		float m_shadowDepth;

		DECL_RTTI_DYNC(MMColor);
	};
}//namespace Cool3D