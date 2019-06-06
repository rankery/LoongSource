#include "StdAfx.h"
#include ".\restexture.h"
#include "..\util\DebugMisc.h"
#include "..\util\Exception.h"
#include "..\Util\Filename.h"
#include "..\Kernel.h"
#include "..\Device\IDevice.h"
#include "..\Device\IDeviceTex.h"
#include "..\VFS\IFS.h"
#include "ResMgr.h"

namespace Cool3D
{
	IMP_RTTI(ResTexture,ResBase);

	ResTexture::ResTexture(const TCHAR* szName):ResBase(szName)
	{
		m_pTex=NULL;
	}

	ResTexture::~ResTexture(void)
	{
		if(m_pTex!=ResMgr::Inst()->GetTexFallBack())
		{
			SAFE_DELETE(m_pTex);
		}
	}

	void ResTexture::AttachTex(IDeviceTex *pTex)
	{
		SAFE_DELETE(m_pTex);
		m_pTex=pTex;
		m_bCreated=true;
	}

	UINT ResTexture::CreateContent(DWORD param)
	{
		try
		{
			UINT numLevels		 = (UINT)LOWORD(param);
			ETexDetail texDetail = (ETexDetail)HIWORD(param);

			m_pTex=Device()->NewTexture();
			m_pTex->LoadFromFile(GetSafeFS(),GetName(),numLevels,true,true,texDetail);
		}
		catch(Exception e)
		{
			//--发生错误,使用默认贴图代替
			m_pTex=ResMgr::Inst()->GetTexFallBack();
		}

		ResBase::CreateContent(param);

		m_size=m_pTex->GetSizeInBytes();
		return m_size;
	}

	void ResTexture::Reload()
	{
		SAFE_DELETE(m_pTex);
		CreateContent(0);
	}

}//namespace Cool3D