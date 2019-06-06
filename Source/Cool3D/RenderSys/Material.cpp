#include "StdAfx.h"
#include ".\material.h"
#include "..\ResSys\ResMgr.h"
#include "..\ResSys\ResTexture.h"

#include "\work\public\MtlDesc.h"

namespace F3D
{
	
	Material::Material(const tagMtlDesc& desc)
	{
		if(strlen(desc.m_map[EMap_Diffuse]) > 0)
			m_texDiffuse=ResMgr::Inst()->NewTexture(desc.m_map[EMap_Diffuse]);
		else
			m_texDiffuse=NULL;
		if(strlen(desc.m_map[EMap_Lighting]) > 0)
			m_texLighting=ResMgr::Inst()->NewTexture(desc.m_map[EMap_Lighting]);
		else
			m_texLighting=NULL;

		m_bTransparent=desc.m_bTransparent;
	}

	IDeviceTex *Material::GetDiffuse()
	{
		if(m_texDiffuse==NULL)
			return NULL;
		return m_texDiffuse->GetDeviceTex();
	}

	IDeviceTex *Material::GetLighting()
	{
		if(m_texLighting==NULL)
			return NULL;
		return m_texLighting->GetDeviceTex();
	}

	Material::~Material(void)
	{
	}
}//namespace F3D