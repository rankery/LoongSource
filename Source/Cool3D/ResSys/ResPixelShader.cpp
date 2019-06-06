#include "StdAfx.h"
#include ".\ResPixelShader.h"
#include "..\Device\IDevice.h"
#include "..\Kernel.h"
#include "..\Device\IPixelShader.h"
#include "..\Util\DebugMisc.h"

namespace Cool3D
{
	IMP_RTTI(ResPixelShader,ResBase);

	ResPixelShader::ResPixelShader(const TCHAR* szName):ResBase(szName)
	{
		m_pPS=NULL;
	}

	ResPixelShader::~ResPixelShader(void)
	{
		SAFE_DELETE(m_pPS);
	}

	UINT ResPixelShader::CreateContent(DWORD param)
	{
		char szVer[10];
		sprintf(szVer,"ps_%d_%d",LOWORD(param),HIWORD(param));

		m_pPS=Device()->NewPS();
		m_pPS->CreateFromFile(GetName(),szVer,"main");

		ResBase::CreateContent(param);
		m_size=1;

		return m_size;
	}

}//namespace Cool3D