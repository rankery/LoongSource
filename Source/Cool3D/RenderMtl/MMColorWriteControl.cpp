#include "StdAfx.h"
#include ".\mmcolorwritecontrol.h"
#include "..\Device\IDevice.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(MMColorWriteControl,IMtlModifier);

	MMColorWriteControl::MMColorWriteControl(void)
	{
		m_dwColorMask=0x0000000F;
	}

	MMColorWriteControl::~MMColorWriteControl(void)
	{
	}

	void MMColorWriteControl::Serialize(FArchive& ar)
	{
		ar<<m_dwColorMask;

		IMtlModifier::Serialize(ar);
	}

	void MMColorWriteControl::Deserialize(FArchive& ar)
	{
		ar>>m_dwColorMask;

		IMtlModifier::Serialize(ar);
	}
}//namespace Cool3D