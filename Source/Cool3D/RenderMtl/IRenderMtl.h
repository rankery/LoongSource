#include "..\Device\IDevice.h"

#pragma once
#include "..\Cool3D.h"
#include "imaterial.h"

namespace Cool3D
{
	/**	\class IRenderMtl 
		\brief Texture等渲染用的材质数据
	*/
	class Cool3D_API IRenderMtl : public IMaterial
	{
	public:
		IRenderMtl(void);
		virtual ~IRenderMtl(void);

		//--ISerializable
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);

	public:
		bool	m_bEnableLight;
		bool	m_bTwoSide;		//是否少双面材质
		DECL_RTTI(IRenderMtl);
	};
}//namespace Cool3D