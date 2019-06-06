#include "StdAfx.h"
#include ".\mmdepthframe.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(MMDepthFrame,IMtlModifier);

	MMDepthFrame::MMDepthFrame(void)
	{
		m_depthWriteEnable	=	true;
		m_depthTestEnable	=	true;
		m_depthFunc	=	ECMP_LessEqual;
	}

	MMDepthFrame::~MMDepthFrame(void)
	{
	}

	
	void MMDepthFrame::Serialize(FArchive& ar)
	{
		ar<<m_depthWriteEnable;
		ar<<m_depthTestEnable;
		ar<<m_depthFunc;

		IMtlModifier::Serialize(ar);
	}

	void MMDepthFrame::Deserialize(FArchive& ar)
	{
		ar>>m_depthWriteEnable;
		ar>>m_depthTestEnable;
		int tmp;
		ar>>tmp;	m_depthFunc=(ECmpFunc)tmp;

		IMtlModifier::Deserialize(ar);
	}
}//namespace Cool3D