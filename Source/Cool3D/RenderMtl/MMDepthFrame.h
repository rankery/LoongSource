#pragma once
#include "..\Device\IDevice.h"
#include "imtlmodifier.h"

namespace Cool3D
{
	/**	\class MMDepthFrame 
		\brief 深度缓存的状态控制
	*/
	class Cool3D_API MMDepthFrame : public IMtlModifier
	{
	public:
		MMDepthFrame(void);
		virtual ~MMDepthFrame(void);

		//--ISerializable
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);

	public:
		bool		m_depthWriteEnable;
		bool		m_depthTestEnable;
		ECmpFunc	m_depthFunc;

		DECL_RTTI_DYNC(MMDepthFrame);
	};
}//namespace Cool3D