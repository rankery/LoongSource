#include "StdAfx.h"
#include ".\MKeyFrame.h"
#include "..\ResSys\ResMgr.h"
#include "..\ResSys\ResVertShader.h"
#include "..\ResSys\ResPixelShader.h"
#include "..\Cool3DOption.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(MKeyFrame,MGeneral);

	MKeyFrame::MKeyFrame(void)
	{
		for(int i=0;i<ES_Num;i++)
		{
			m_pVS[i]=NULL;
			m_pPS[i]=NULL;
		}
		m_shader=ES_None;
	}

	MKeyFrame::~MKeyFrame(void)
	{
		Destroy();
	}

	bool MKeyFrame::IsResourceReady() const
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
		}
		return true;
	}

	void MKeyFrame::Serialize(FArchive& ar)
	{
		ASSERT(0 && _T("not support"));
	}

	void MKeyFrame::Deserialize(FArchive& ar)
	{
		ASSERT(0 && _T("not support"));
	}

	DWORD MKeyFrame::GetRenderTrait1() const
	{
		if(m_shader==ES_None)
			return IMaterial::GetRenderTrait1();
		else
			return (DWORD)m_pVS[m_shader];
	}

	void MKeyFrame::Destroy()
	{
		for(int i=0;i<ES_Num;i++)
		{
			RES_MGR_RELEASE_VS(m_pVS[i]);
			RES_MGR_RELEASE_PS(m_pPS[i]);
		}
		MGeneral::Destroy();
	}

	void MKeyFrame::LoadShaders()
	{
		if(Cool3DOption::Inst()->GetOption(Cool3DOption::EO_NormalMap)==Cool3DOption::ENM_Open
			&&m_pBump!=NULL)
		{
			RES_MGR_RELEASE_VS(m_pVS[ES_Direct]);
			RES_MGR_RELEASE_VS(m_pVS[ES_DirectPoint1]);

			RES_MGR_RELEASE_PS(m_pPS[ES_Direct]);
			RES_MGR_RELEASE_PS(m_pPS[ES_DirectPoint1]);

			m_pVS[ES_Direct]		=ResMgr::Inst()->NewVS(_T("data\\system\\shader\\bump_direct.vs"),MAKELONG(1,1));
			m_pVS[ES_DirectPoint1]	=ResMgr::Inst()->NewVS(_T("data\\system\\shader\\bump_direct_point1.vs"),MAKELONG(1,1));

			m_pPS[ES_Direct]		=ResMgr::Inst()->NewPS(_T("data\\system\\shader\\bump_direct.ps"),MAKELONG(2,0));
			m_pPS[ES_DirectPoint1]	=ResMgr::Inst()->NewPS(_T("data\\system\\shader\\bump_direct_point1.ps"),MAKELONG(2,0));
		}
	}


}//namespace Cool3D