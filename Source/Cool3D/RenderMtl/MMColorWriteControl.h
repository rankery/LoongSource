#pragma once
#include "imtlmodifier.h"

namespace Cool3D
{
	class Cool3D_API MMColorWriteControl :	public IMtlModifier
	{
	public:
		MMColorWriteControl(void);
		virtual ~MMColorWriteControl(void);

		//--ISerializable
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);

	public:
		unsigned int m_dwColorMask;

		DECL_RTTI_DYNC(MMColorWriteControl);
	};
}//namespace Cool3D