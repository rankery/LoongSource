#include "StdAfx.h"
#include ".\mconstcolor.h"
#include "..\RenderSys\RenderSys.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(MConstColor,IRenderMtl);

	MConstColor::MConstColor(void)
	{
		m_color=RenderSys::WhiteRenderColor;
		m_bUsingVertColor=false;
	}

	MConstColor::~MConstColor(void)
	{
	}

	void MConstColor::Serialize(FArchive& ar)
	{
		ar<<m_color.ambient;
		ar<<m_color.diffuse;
		ar<<m_color.emissive;
		ar<<m_color.power;
		ar<<m_color.specular;
		ar<<m_bUsingVertColor;

		IRenderMtl::Serialize(ar);
	}

	void MConstColor::Deserialize(FArchive& ar)
	{
		ar>>m_color.ambient;
		ar>>m_color.diffuse;
		ar>>m_color.emissive;
		ar>>m_color.power;
		ar>>m_color.specular;
		ar>>m_bUsingVertColor;

		IRenderMtl::Deserialize(ar);
	}
}//namespace Cool3D