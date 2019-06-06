#pragma once
#include "..\Device\IDevice.h"
#include "imtlmodifier.h"

namespace Cool3D
{
	/**	\class MMAlphaMask 
		\brief 使用alpha test来实现按照指定的alpha阀值产生透空效果(不处理半透明)
	*/
	class Cool3D_API MMAlphaMask :	public IMtlModifier
	{
	public:
		MMAlphaMask(void);
		virtual ~MMAlphaMask(void);

		//--ISerializable
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);
	public:
		ECmpFunc	m_alphaTestFunc;
		BYTE		m_alphaRef;

		DECL_RTTI_DYNC(MMAlphaMask);
	};
}//namespace Cool3D