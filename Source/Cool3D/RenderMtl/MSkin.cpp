#include "StdAfx.h"
#include ".\MSkin.h"
#include "..\ResSys\ResMgr.h"
#include "..\ResSys\ResVertShader.h"
#include "..\ResSys\ResPixelShader.h"
#include "..\Cool3DOption.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(MSkin,MGeneral);

	MSkin::MSkin(void)
	{
		for(int i=0;i<ES_Num;i++)
		{
			m_pVS[i]=NULL;
			m_pPS[i]=NULL;
		}
		m_shader=ES_None;
		m_pBlendLayer=NULL;
	}

	MSkin::~MSkin(void)
	{
		Destroy();
	}

	bool MSkin::IsResourceReady() const
	{
		if(!MGeneral::IsResourceReady())
			return false;
		for(int i=0;i<ES_Num;i++)
		{
			if(m_pVS[i]!=NULL
				&&!m_pVS[i]->IsCreated())
				return false;
			if(m_pPS[i]!=NULL
				&&!m_pPS[i]->IsCreated())
				return false;
			if(m_pBlendLayer!=NULL
				&&!m_pBlendLayer->IsResourceReady())
				return false;
		}
		return true;
	}

	void MSkin::Serialize(FArchive& ar)
	{
		ASSERT(0 && _T("not support"));
	}

	void MSkin::Deserialize(FArchive& ar)
	{
		ASSERT(0 && _T("not support"));
	}

	DWORD MSkin::GetRenderTrait1() const
	{
		if(m_shader==ES_None)
			return IMaterial::GetRenderTrait1();
		else
			return (DWORD)m_pVS[m_shader];
	}

	void MSkin::Destroy()
	{
		for(int i=0;i<ES_Num;i++)
		{
			RES_MGR_RELEASE_VS(m_pVS[i]);
			RES_MGR_RELEASE_PS(m_pPS[i]);
		}
		MGeneral::Destroy();
		SAFE_DELETE(m_pBlendLayer);
	}

	void MSkin::LoadShaders()
	{
		if(Cool3DOption::Inst()->GetOption(Cool3DOption::EO_NormalMap)==Cool3DOption::ENM_Open
			&&m_pBump!=NULL)
		{
			RES_MGR_RELEASE_VS(m_pVS[ES_Bump_Direct_Skin]);
			RES_MGR_RELEASE_VS(m_pVS[ES_Bump_Direct_Point1_Skin]);
			RES_MGR_RELEASE_PS(m_pPS[ES_Bump_Direct_Skin]);
			RES_MGR_RELEASE_PS(m_pPS[ES_Bump_Direct_Point1_Skin]);

			m_pVS[ES_Bump_Direct_Skin]			=ResMgr::Inst()->NewVS(_T("data\\system\\shader\\bump_direct_skin.vs"),MAKELONG(2,0));
			m_pVS[ES_Bump_Direct_Point1_Skin]	=ResMgr::Inst()->NewVS(_T("data\\system\\shader\\bump_direct_point1_skin.vs"),MAKELONG(2,0));
			m_pPS[ES_Bump_Direct_Skin]			=ResMgr::Inst()->NewPS(_T("data\\system\\shader\\bump_direct.ps"),MAKELONG(2,0));
			m_pPS[ES_Bump_Direct_Point1_Skin]	=ResMgr::Inst()->NewPS(_T("data\\system\\shader\\bump_direct_point1.ps"),MAKELONG(2,0));
		}
		else
		{
			if(Cool3DOption::Inst()->GetOption(Cool3DOption::EO_HWSkin)==Cool3DOption::EHWS_Open)
			{
				RES_MGR_RELEASE_VS(m_pVS[ES_Direct_Skin]);
				RES_MGR_RELEASE_VS(m_pVS[ES_Direct_Point1_Skin]);
				RES_MGR_RELEASE_PS(m_pPS[ES_Direct_Skin]);
				RES_MGR_RELEASE_PS(m_pPS[ES_Direct_Point1_Skin]);

				m_pVS[ES_Direct_Skin]			=ResMgr::Inst()->NewVS(_T("data\\system\\shader\\direct_skin.vs"),MAKELONG(2,0));
				m_pVS[ES_Direct_Point1_Skin]	=ResMgr::Inst()->NewVS(_T("data\\system\\shader\\direct_point1_skin.vs"),MAKELONG(2,0));
			}
		}
	}

}//namespace Cool3D