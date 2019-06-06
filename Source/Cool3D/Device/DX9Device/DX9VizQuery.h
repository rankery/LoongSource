#pragma once
#include "..\idevicevizquery.h"
#include <d3d9.h>

namespace Cool3D
{
	namespace Detail
	{
		class DX9VizQuery : public IDeviceVizQuery
		{
		public:
			DX9VizQuery(void);
			virtual ~DX9VizQuery(void);

			//--IDeviceVizQuery
			virtual void Create();
			virtual bool Begin();
			virtual bool End();
			virtual DWORD GetResultSync();
			virtual bool GetResultAsync(DWORD& out);
			void PreDeviceReset();
			void PostDeviceReset();
		private:
			IDirect3DQuery9* m_pQuery;
			DWORD            m_dwMultySample;
			bool             m_bWaitingForResult;
		};
	}
}//namespace Cool3D