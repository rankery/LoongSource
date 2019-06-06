#include "StdAfx.h"
#include ".\mbitmapframes.h"
#include "..\ResSys\ResMgr.h"
#include "..\ResSys\ResTexture.h"
#include "..\Kernel.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(MBitmapFrames,MBitmap);

	MBitmapFrames::MBitmapFrames(void)
	{
		m_fps=16.0f;
	}

	MBitmapFrames::~MBitmapFrames(void)
	{
		m_pBitmap=NULL;

		for(int i=0;i<(int)m_texFrames.size();i++)
		{
			ResTexture *pTex=m_texFrames[i];
			ResMgr::Inst()->ReleaseTexture(pTex);
		}
		m_texFrames.clear();
	}

	void MBitmapFrames::UpdateModifiers(float deltaTime)
	{
		float frameTime=1.0f/m_fps;
		int curFrm=int(Kernel::Inst()->GetAccumTime()/frameTime);
		curFrm%=(int)m_texFrames.size();

		//更新当前帧
		if(m_pBitmap!=m_texFrames[curFrm])
		{
			m_pBitmap=m_texFrames[curFrm];
			InvalidateRenderCache();
		}
	}

	void MBitmapFrames::LoadFrames(const TCHAR* szBaseName,bool bSysFS,int numFrm,int numLevels)
	{
		for(int i=0;i<numFrm;i++)
		{
			TCHAR szPath[512];
			_stprintf(szPath,_T("%s%02d.tga"),szBaseName,i);
			ResTexture *pTex=ResMgr::Inst()->NewTexture(szPath,numLevels);
			m_texFrames.push_back(pTex);
		}
	}

}//namespace Cool3D