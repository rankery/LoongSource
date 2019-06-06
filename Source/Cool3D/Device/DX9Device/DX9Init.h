#include "DX9Device.h"
#pragma once

namespace Cool3D
{
	namespace Detail
	{
		/** \class DX9Init  
		\brief A helper class for DX9 Device creation
		*/
		class DX9Init  
		{
		public:
			static IDirect3DDevice9* CreateDevice(IDirect3D9 *pD3D,tagDeviceCfg *pCfg,D3DPRESENT_PARAMETERS *d3dPP);
			DX9Init();
			~DX9Init();

		private:
			DWORD FindBestBehaviorFlags();
			/** get a D3DPRESENT_PARAMETERS struct
			\param DeviceInfo required,[out] best d3d present parameter
			\return	If have not find any compitable mode return false
			\remarks Fill a D3DPRESENT_PARAMETER struct,find the max refresh rate
			*/
			bool FindBestPresentParam(tagDeviceCfg *pCfg,D3DPRESENT_PARAMETERS& d3dpp/*out*/);
			IDirect3DDevice9* _CreateDevice(IDirect3D9 *pD3D,tagDeviceCfg *pCfg,D3DPRESENT_PARAMETERS *d3dPP);

			HWND m_hDeviceWnd;
			IDirect3D9 *m_pD3D;
			UINT m_nAdapter;
		};
	}
}//namespace Cool3D