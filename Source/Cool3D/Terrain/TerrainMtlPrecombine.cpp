#include "StdAfx.h"
#include ".\TerrainMtlPrecombine.h"
#include "..\ResSys\ResPixelShader.h"
#include "..\ResSys\ResMgr.h"
#include "..\Cool3DOption.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(TerrainMtlPrecombine,IMaterial);

	TerrainMtlPrecombine::TerrainMtlPrecombine(void)
	{
		m_pPrecombineTex = NULL;
		m_pPS=NULL;

		m_color.m_bEnableLight=true;
		m_color.m_bTwoSide=false;
		m_color.m_color.diffuse=Color4f(1.0f,1.0f,1.0f,1.0f);
		m_color.m_color.ambient=Color4f(1.0f,0.7f,0.7f,0.7f);
		m_color.m_color.power=8;
		m_color.m_color.specular=Color4f(1.0f,1.0f,1.0f,1.0f);
	}

	TerrainMtlPrecombine::~TerrainMtlPrecombine(void)
	{
		SAFE_DELETE(m_pPrecombineTex);
		RES_MGR_RELEASE_PS(m_pPS);
	}

	bool TerrainMtlPrecombine::IsResourceReady() const
	{
		if(m_pPrecombineTex==NULL
			||!m_pPrecombineTex->IsResourceReady())
			return false;
		if(m_pPS!=NULL
			&&!m_pPS->IsCreated())
			return false;
		return true;
	}

	void TerrainMtlPrecombine::Serialize(FArchive& ar)
	{
		IMaterial::Serialize(ar);
	}

	void TerrainMtlPrecombine::Deserialize(FArchive& ar)
	{
		IMaterial::Deserialize(ar);
	}

	void TerrainMtlPrecombine::LoadShader()
	{
		RES_MGR_RELEASE_PS(m_pPS);

		if(Cool3DOption::Inst()->GetOption(Cool3DOption::EO_TerrainDetail)==Cool3DOption::ETD_MultiLayers
			&&Cool3DOption::Inst()->GetOption(Cool3DOption::EO_TerrainSpecular)==Cool3DOption::ETS_Open)
		{
			TCHAR szPath[512];
			_stprintf(szPath,_T("data\\system\\terrain\\shader\\precombine_s.ps"));
			m_pPS=ResMgr::Inst()->NewPS(szPath,MAKELONG(1,4));
		}
	}

}//namespace Cool3D