#include "StdAfx.h"
#include ".\ResVertShader.h"
#include "..\Device\IDevice.h"
#include "..\Device\IVertShader.h"
#include "..\Kernel.h"
#include "..\Util\DebugMisc.h"

namespace Cool3D
{
	IMP_RTTI(ResVertShader,ResBase);

	ResVertShader::ResVertShader(const TCHAR* szName):ResBase(szName)
	{
		m_pVS=NULL;
	}

	ResVertShader::~ResVertShader(void)
	{
		SAFE_DELETE(m_pVS);
	}

	UINT ResVertShader::CreateContent(DWORD param)
	{
		char szVer[10];
		sprintf(szVer,"vs_%d_%d",LOWORD(param),HIWORD(param));

		m_pVS=Device()->NewVS();
		m_pVS->CreateFromFile(GetName(),szVer,"main");

		ResBase::CreateContent(param);
		m_size=1;

		return m_size;
	}

}//namespace Cool3D