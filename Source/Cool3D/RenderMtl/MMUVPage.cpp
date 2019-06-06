#include "StdAfx.h"
#include ".\mmuvpage.h"
#include "..\RenderSys\MtlRenderCache.h"
#include "IMaterial.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(MMUVPage,MMUVTrans);

	MMUVPage::MMUVPage(void)
	{
		m_uDim=m_vDim=1;
		m_fps=10.0f;
		m_curFrm=0;
		m_curFrmTime=0;
	}

	MMUVPage::~MMUVPage(void)
	{
	}

	void MMUVPage::SetParam(int uDim,int vDim,float fps,bool bPlayOnce)
	{
		m_uDim=uDim;
		m_vDim=vDim;
		m_fps=fps;
		m_bPlayOnce=bPlayOnce;

		m_curFrm=0;
		m_curFrmTime=0;

		MMUVTrans::SetScale(1.0f/m_uDim,1.0f/m_vDim);
		MMUVTrans::SetOffset(0,0);
	}

	void MMUVPage::Update(float deltaTime,IMaterial *pOwner)
	{
		//计算当前帧
		m_curFrmTime+=deltaTime;
		if(m_curFrmTime>1.0f/m_fps)
		{
			m_curFrmTime-=1.0f/m_fps;
			m_curFrm++;
			if(m_curFrm>=m_vDim*m_uDim)
			{
				if(m_bPlayOnce)
					m_curFrm=m_vDim*m_uDim-1;
				else
					m_curFrm=0;
			}

			//更新当前帧
			SetFrame(m_curFrm,pOwner);
		}
	}

	void MMUVPage::ResetAni(IMaterial *pOwner)
	{
		m_curFrm=0;
		m_curFrmTime=0;
		SetFrame(0,pOwner);
	}

	void MMUVPage::SetFrame(int frame,IMaterial *pOwner)
	{
		//设置矩阵
		float v=float(frame/m_uDim);
		float u=float(frame%m_uDim);
		u*=1.0f/m_uDim;
		v*=1.0f/m_vDim;
		MMUVTrans::SetOffset(u,v);

		MtlRenderCache *pCache=pOwner->GetRenderCache();
		if(pCache)
			pCache->UpdateMtlMod(this);
	}
}//namespace Cool3D