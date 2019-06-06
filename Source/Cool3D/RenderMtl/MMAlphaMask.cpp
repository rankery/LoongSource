#include "StdAfx.h"
#include ".\mmalphamask.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(MMAlphaMask,IMtlModifier);

	MMAlphaMask::MMAlphaMask(void)
	{
		m_alphaTestFunc=ECMP_Greater;
		m_alphaRef=240;
	}

	MMAlphaMask::~MMAlphaMask(void)
	{
	}

	void MMAlphaMask::Serialize(FArchive& ar)
	{
		ar<<m_alphaTestFunc;
		ar<<m_alphaRef;

		IMtlModifier::Serialize(ar);
	}

	void MMAlphaMask::Deserialize(FArchive& ar)
	{
		int func;
		ar>>func;	m_alphaTestFunc=(ECmpFunc)func;
		ar>>m_alphaRef;

		IMtlModifier::Deserialize(ar);
	}
}//namespace Cool3D