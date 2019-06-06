#include "StdAfx.h"
#include ".\terrainmtlmultilayers.h"
#include "..\Device\IDevice.h"
#include "..\Device\IDeviceTex.h"
#include "..\RenderMtl\MBitmap.h"
#include "..\Image\ImageTGA.h"
#include "..\ResSys\ResPixelShader.h"
#include "..\ResSys\ResMgr.h"
#include "..\Cool3DOption.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(TerrainMtlMultiLayers,IMaterial);

	TerrainMtlMultiLayers::TerrainMtlMultiLayers(void)
	{
		m_numLayers=0;
		m_pAlphaMap=NULL;
		m_pLightMap=NULL;
		m_pPS=NULL;

		m_color.m_bEnableLight=true;
		m_color.m_bTwoSide=false;
		m_color.m_color.diffuse=Color4f(1.0f,1.0f,1.0f,1.0f);
		m_color.m_color.ambient=Color4f(1.0f,0.7f,0.7f,0.7f);
		m_color.m_color.power=8;
		m_color.m_color.specular=Color4f(1.0f,1.0f,1.0f,1.0f);
	}

	TerrainMtlMultiLayers::~TerrainMtlMultiLayers(void)
	{
		for(vector<tagLayer>::iterator iter=m_layers.begin();
			iter!=m_layers.end();++iter)
		{
			SAFE_DELETE(iter->m_pTileTex);
		}
		SAFE_DELETE(m_pAlphaMap);
		SAFE_DELETE(m_pLightMap);
		m_layers.clear();
		m_numLayers=0;
		RES_MGR_RELEASE_PS(m_pPS);
	}

	void TerrainMtlMultiLayers::Serialize(FArchive& ar)
	{
		ASSERT(0 && _T("not implement yet"));
		IMaterial::Serialize(ar);
	}

	void TerrainMtlMultiLayers::Deserialize(FArchive& ar)
	{
		ASSERT(0 && _T("not implement yet"));
		IMaterial::Deserialize(ar);
	}

	bool TerrainMtlMultiLayers::IsResourceReady() const
	{
		if(m_numLayers==0 || (int)m_layers.size()<m_numLayers)//已有的layer少于预期的layer个数
			return false;

		//只要有任意一个layer的任意一个贴图没有ok,则返回false
		size_t i;
		for(i=0;i<m_layers.size();i++)
		{
			const tagLayer& layer=m_layers[i];
			if(!layer.m_pTileTex->IsResourceReady())
				return false;
		}

		if(m_pAlphaMap!=NULL
			&& !m_pAlphaMap->IsResourceReady())
			return false;

		if(m_pLightMap!=NULL
			&& !m_pLightMap->IsResourceReady())
			return false;
		
		if(m_pPS==NULL
			||!m_pPS->IsCreated())
			return false;

		return true;
	}

	void TerrainMtlMultiLayers::BeginLayers(int numLayers)
	{
		m_numLayers=numLayers;
	}

	void TerrainMtlMultiLayers::EndLayers()
	{}

	void TerrainMtlMultiLayers::AddLayer(const TCHAR* szDiffuseTexPath,BYTE uvSetIndex)
	{
		MBitmap *pDiffMtl=new MBitmap;
		pDiffMtl->SetTexture(0,szDiffuseTexPath);
		pDiffMtl->m_mipFilter=ETFilter_LINEAR;
		pDiffMtl->m_minFilter=pDiffMtl->m_magFilter=ETFilter_LINEAR;
		pDiffMtl->m_addressU=pDiffMtl->m_addressV=pDiffMtl->m_addressW=ETAddress_Wrap;
				
		tagLayer newLayer={pDiffMtl,uvSetIndex};
		m_layers.push_back(newLayer);
	}

	void TerrainMtlMultiLayers::LoadAlphaMap(const TCHAR* szPath)
	{
		SAFE_DELETE(m_pAlphaMap);
		m_pAlphaMap=new MBitmap;
		m_pAlphaMap->SetTexture(0,szPath);
		m_pAlphaMap->m_mipFilter=ETFilter_POINT;
		m_pAlphaMap->m_minFilter=m_pAlphaMap->m_magFilter=ETFilter_LINEAR;
		m_pAlphaMap->m_addressU=
			m_pAlphaMap->m_addressV=
			m_pAlphaMap->m_addressW=ETAddress_Clamp;
	}

	void TerrainMtlMultiLayers::LoadLightMap(const TCHAR* szPath)
	{
		SAFE_DELETE(m_pLightMap);
		m_pLightMap=new MBitmap;
		m_pLightMap->SetTexture(0,szPath);
		m_pLightMap->m_mipFilter=ETFilter_POINT;
		m_pLightMap->m_minFilter=m_pLightMap->m_magFilter=ETFilter_LINEAR;
		m_pLightMap->m_addressU=
			m_pLightMap->m_addressV=
			m_pLightMap->m_addressW=ETAddress_Clamp;
	}

	void TerrainMtlMultiLayers::LoadShaders(int numLayers)
	{
		ASSERT(numLayers>0 && numLayers<=4);
		RES_MGR_RELEASE_PS(m_pPS);

		TCHAR szPath[512];
		if(Cool3DOption::Inst()->GetOption(Cool3DOption::EO_LightMap)==Cool3DOption::ELM_Open)
		{
			if(Cool3DOption::Inst()->GetOption(Cool3DOption::EO_TerrainSpecular)==Cool3DOption::ETS_Open)
				_stprintf(szPath,_T("data\\system\\terrain\\shader\\multitex%d_s_l.ps"),numLayers);
			else
				_stprintf(szPath,_T("data\\system\\terrain\\shader\\multitex%d_l.ps"),numLayers);
		}
		else
		{
			if(Cool3DOption::Inst()->GetOption(Cool3DOption::EO_TerrainSpecular)==Cool3DOption::ETS_Open)
				_stprintf(szPath,_T("data\\system\\terrain\\shader\\multitex%d_s.ps"),numLayers);
			else
				_stprintf(szPath,_T("data\\system\\terrain\\shader\\multitex%d.ps"),numLayers);
		}
		m_pPS=ResMgr::Inst()->NewPS(szPath,MAKELONG(1,4));
	}

}//namespace Cool3D