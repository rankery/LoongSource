#include "StdAfx.h"
#include ".\MMFrameBlend.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(MMFrameBlend,IMtlModifier);

	MMFrameBlend::MMFrameBlend(void)
	{
		m_blendEnable=false;
		m_src=EBlend_One;
		m_dst=EBlend_Zero;
		m_bMultiPass=false;
	}

	MMFrameBlend::~MMFrameBlend(void)
	{
	}

	void MMFrameBlend::Serialize(FArchive& ar)
	{
		ar<<m_blendEnable;
		ar<<m_src;
		ar<<m_dst;
		ar<<m_bMultiPass;

		IMtlModifier::Serialize(ar);
	}

	void MMFrameBlend::Deserialize(FArchive& ar)
	{
		ar>>m_blendEnable;
		int tmp;
		ar>>tmp;	m_src=(EBlend)tmp;
		ar>>tmp;	m_dst=(EBlend)tmp;
		ar>>m_bMultiPass;

		IMtlModifier::Deserialize(ar);
	}
}//namespace Cool3D