#pragma once
#include "irendermtl.h"
#include "..\Math\Color.h"
#include "..\Device\IDevice.h"

namespace Cool3D
{
	/**	\class MConstColor
		\brief 颜色材质
	*/
	class Cool3D_API MConstColor :	public IRenderMtl
	{
	public:
		MConstColor(void);
		virtual ~MConstColor(void);

		//--ISerializable
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);
	public:
		tagRenderColor	m_color;
		bool			m_bUsingVertColor;//关闭灯光,颜色材质,使用Vertexcolor代替,vertex stream必须有Diffuse成员

		DECL_RTTI_DYNC(MConstColor);
	};
}//namespace Cool3D