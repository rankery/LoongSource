
#include "stdafx.h"
#include "RenderStatus.h"

#include "..\Kernel.h"
#include "IDevice.h"
#include "IFont.h"
#include "..\Util\DebugMisc.h"
#include "..\Cool3DOption.h"

namespace Cool3D
{
	RenderStatus::RenderStatus()
	{
		pFont=Device()->NewFont();
		pFont->Create(0, 16, FW_NORMAL, _T("Arial"));	
		FPS=-1;
		texSize=0;
		renderTexSize=0;
		staticVBSize=0;
		dynamicVBSize=0;
		staticIBSize=0;
		dynamicIBSize=0;
		Clear();
	}
	
	RenderStatus::~RenderStatus()
	{
		SAFE_DELETE(pFont);
	}

	void RenderStatus::Draw()
	{
		if(Cool3DOption::Inst()->GetOption(Cool3DOption::EO_DebugInfo)==Cool3DOption::EDI_Open)
		{
#ifdef _DEBUG
			TCHAR *szVer=_T("Debug");
#else
			TCHAR *szVer=_T("Release");
#endif
			static TCHAR szBuf[512];
			_stprintf(szBuf,_T("%s,FPS=%d,Prim=%d,DrawCall=%d,TexSwitch=%d,VBSwitch=%d,tex=%uk,rt=%uk,sVB=%dk,dVB=%dk,sIB=%dk,dIB=%dk")
				,szVer,FPS,numPrim,numDrawCall,numTexSwitch,numVBSwitch,texSize/1024,renderTexSize/1024,staticVBSize/1024,dynamicVBSize/1024,staticIBSize/1024,dynamicIBSize/1024);

			pFont->DrawText(szBuf,2,2,0xFFFFFFFF);
		}		
	}

	void RenderStatus::Clear()
	{
		numDrawCall=numPrim=numVBSwitch=0;	
		numEffectCall=0;
		numTexSwitch=0;
	}

	void RenderStatus::CalFPS()
	{
		static DWORD lastTime=timeGetTime();
		static INT nCounter = 0;
		nCounter++;
		if( timeGetTime()-lastTime >= 1000 )
		{
			FPS = nCounter;
			nCounter = 0;
			lastTime = timeGetTime();
		}
	}
}//namespace Cool3D
