
#include "StdAfx.h"
#include "DX9Init.h"

#include "..\..\Util\DebugMisc.h"
#include "..\..\Util\Exception.h"
#include "DX9BaseDef.h"
#include "..\..\Kernel.h"

namespace Cool3D
{
	namespace Detail
	{
		const DWORD MINI_VSVERSION=D3DVS_VERSION(1,1);	//vertex shader version required
		const DWORD MINI_PSVERSION=D3DPS_VERSION(1,4);	//pixel shader version required

		DX9Init::DX9Init()
		{
			m_hDeviceWnd=NULL;
			m_pD3D=NULL;
			m_nAdapter=D3DADAPTER_DEFAULT;
		}

		DX9Init::~DX9Init()
		{

		}

		IDirect3DDevice9* DX9Init::CreateDevice(IDirect3D9 *pD3D,tagDeviceCfg *pCfg,D3DPRESENT_PARAMETERS *d3dPP)
		{
			DX9Init initer;
			return initer._CreateDevice(pD3D,pCfg,d3dPP);
		}

		IDirect3DDevice9* DX9Init::_CreateDevice(IDirect3D9 *pD3D,tagDeviceCfg *pCfg,D3DPRESENT_PARAMETERS *d3dPP)
		{
			ASSERT(NULL!= pD3D);
			m_pD3D=pD3D;

			HWND hWnd=pCfg->hWnd;
			if(!::IsWindow(hWnd))
				throw EXCEPTION(_T("Invalid device window handle."));
			m_hDeviceWnd=hWnd;

			//--手动切换显示模式
			if(!pCfg->bWindowed)
			{
				int bits=32;
				switch(pCfg->pixelFormat)
				{
				case EPF_R5G6B5:
				case EPF_A1R5G5B5:
					bits=16;
					break;
				case EPF_R8G8B8:
					bits=24;
					break;
				case EPF_A8R8G8B8:
				case EPF_X8R8G8B8:
					bits=32;
					break;
				}

				DEVMODE dmScreenSettings;								// Device Mode
				memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
				dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
				dmScreenSettings.dmPelsWidth	= pCfg->nWidth;			// Selected Screen Width
				dmScreenSettings.dmPelsHeight	= pCfg->nHeight;		// Selected Screen Height
				dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
				dmScreenSettings.dmDisplayFrequency = pCfg->nFullScreenHZ;
				dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT|DM_DISPLAYFREQUENCY;

				// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
				if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
				{
					Kernel::Inst()->SetLastError(_T("Chanage display mode failed."));
				}
			}
			else
			{
				//---
				RECT rc;
				::GetClientRect(hWnd,&rc);
				if(rc.right-rc.left<=0
					|| rc.bottom-rc.top<=0)
					throw EXCEPTION(_T("Window client size is zero."));

				pCfg->nWidth = rc.right - rc.left;
				pCfg->nHeight = rc.bottom - rc.top;
			}


			HRESULT hr;

			D3DPRESENT_PARAMETERS d3dpp; 
			bool bFind=FindBestPresentParam(pCfg,d3dpp);
			if(!bFind)
			{
				throw EXCEPTION(_T("No compatible device format found."));
			}

			DWORD BehaviorFlags=FindBestBehaviorFlags();
			BehaviorFlags|=D3DCREATE_MULTITHREADED;

			// Set default settings 
			UINT AdapterToUse=D3DADAPTER_DEFAULT;
			D3DDEVTYPE DeviceType=D3DDEVTYPE_HAL;
#if 0
			// Look for 'NVIDIA NVPerfHUD' adapter 
			// If it is present, override default settings 
			for (UINT Adapter=0;Adapter<pD3D->GetAdapterCount();Adapter++) 
			{ 
				D3DADAPTER_IDENTIFIER9 Identifier; 
				HRESULT Res=pD3D->GetAdapterIdentifier(Adapter,0,&Identifier); 
				if (stricmp(Identifier.Description,"NVIDIA NVPerfHUD")==0) 
				{ 
					AdapterToUse=Adapter; 
					DeviceType=D3DDEVTYPE_REF; 
					break; 
				} 
			} 

			AdapterToUse=pD3D->GetAdapterCount()-1; 
			DeviceType=D3DDEVTYPE_REF; 
#endif 

			*d3dPP = d3dpp;
			IDirect3DDevice9 *pDevice=NULL;
			hr=m_pD3D->CreateDevice( AdapterToUse, DeviceType, m_hDeviceWnd,BehaviorFlags,&d3dpp, &pDevice );
			THROW_FAILED(hr,NULL);

			LOG(_T("D3D Device create succeeded...\r\n"));
			return pDevice;
		}

		bool DX9Init::FindBestPresentParam(tagDeviceCfg *pCfg, D3DPRESENT_PARAMETERS &d3dpp)
		{
			D3DFORMAT DisplayFormat=PF2DX9(pCfg->pixelFormat);
			D3DFORMAT DepthBufferFormat=ZF2DX9(pCfg->depthFormat);

			//--抗锯齿级别
			D3DMULTISAMPLE_TYPE MultiSampleType=MultiSampleType2DX9(pCfg->multiSampleType);
			DWORD MultiSampleQuality=0;
			HRESULT hr=m_pD3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, 
				D3DDEVTYPE_HAL,
				DisplayFormat, 
				FALSE, 
				MultiSampleType, &MultiSampleQuality);
			if(D3D_OK==hr) 
			{
				if(MultiSampleQuality>0)
					--MultiSampleQuality;
			}
			else
			{
				MultiSampleType=D3DMULTISAMPLE_NONE;
				MultiSampleQuality=0;
			}

			//--垂直同步？
			UINT PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;
			if(pCfg->bVerticalSync)
				PresentationInterval=D3DPRESENT_INTERVAL_ONE;

			ZeroMemory(&d3dpp,sizeof(d3dpp));
			d3dpp.BackBufferCount=1;
			d3dpp.BackBufferFormat=DisplayFormat;
			d3dpp.BackBufferHeight=pCfg->nHeight;
			d3dpp.BackBufferWidth=pCfg->nWidth;
			d3dpp.EnableAutoDepthStencil=TRUE;
			d3dpp.Flags=0;
			d3dpp.FullScreen_RefreshRateInHz=0;
			d3dpp.MultiSampleQuality=MultiSampleQuality;
			d3dpp.MultiSampleType=MultiSampleType ;
			d3dpp.PresentationInterval=PresentationInterval;
			d3dpp.SwapEffect=D3DSWAPEFFECT_DISCARD;	// D3D规定开了MultiSample必须用Discard方式
			d3dpp.AutoDepthStencilFormat=DepthBufferFormat;

			if(pCfg->bWindowed) // 窗口方式
			{
				d3dpp.Windowed=TRUE;
			}
			else
			{
				d3dpp.Windowed=FALSE;
			}

			ASSERT(NULL!=m_hDeviceWnd);
			d3dpp.hDeviceWindow=m_hDeviceWnd;

			return true;
		}

		DWORD DX9Init::FindBestBehaviorFlags()
		{
			ASSERT(m_pD3D!=NULL);
			D3DCAPS9 caps;
			HRESULT hr=m_pD3D->GetDeviceCaps(m_nAdapter,D3DDEVTYPE_HAL,&caps);
			if(FAILED(hr))
			{
				return D3DCREATE_SOFTWARE_VERTEXPROCESSING;
			}

			if(caps.VertexShaderVersion < MINI_VSVERSION)
			{
				return D3DCREATE_SOFTWARE_VERTEXPROCESSING;
			}

			/*if(caps.PixelShaderVersion < MINI_PSVERSION)
			m_style=DEV_LOW;
			else
			m_style=DEV_HIGH;
			*/

			return D3DCREATE_HARDWARE_VERTEXPROCESSING;
		}
	}
}//namespace Cool3D