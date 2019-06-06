#include "StdAfx.h"
#include ".\DX9VizQuery.h"
#include "..\..\Util\DebugMisc.h"
#include "..\..\Util\Exception.h"
#include "DX9Device.h"


namespace Cool3D
{
	namespace Detail
	{
		DX9VizQuery::DX9VizQuery(void)
		{
			m_pQuery=NULL;
			m_dwMultySample = 1;
			m_bWaitingForResult = false;
		}

		DX9VizQuery::~DX9VizQuery(void)
		{
			MyDX9Dev()->OnVizQueryFree(this);
			SAFE_RELEASE(m_pQuery);
		}	

		void DX9VizQuery::Create()
		{
			SAFE_RELEASE(m_pQuery);

			HRESULT hr=DX9Dev()->CreateQuery(D3DQUERYTYPE_OCCLUSION,&m_pQuery);
			THROW_FAILED(hr,_T("CreateQuery"));

			// 查询结果需要除以多重采样倍数
			DX9Device *pDev = static_cast<DX9Device*>(Device());
			m_dwMultySample = pDev->GetMutiSampleType();
			if( m_dwMultySample < 1 )
				m_dwMultySample = 1;

			m_bWaitingForResult = false;
		}

		bool DX9VizQuery::Begin()
		{
			if( NULL == m_pQuery || m_bWaitingForResult )
				return false;

			HRESULT hr=m_pQuery->Issue(D3DISSUE_BEGIN);
			THROW_FAILED(hr,_T("Issue"));
			return true;
		}

		bool DX9VizQuery::End()
		{
			if( NULL == m_pQuery || m_bWaitingForResult )
				return false;

			HRESULT hr=m_pQuery->Issue(D3DISSUE_END);
			THROW_FAILED(hr,_T("Issue"));
			return true;
		}

		DWORD DX9VizQuery::GetResultSync()
		{
			if( NULL == m_pQuery )
				return 0;

			ASSERT( false == m_bWaitingForResult );
			DWORD numberOfPixelsDrawn=0;

			// Force the driver to execute the commands from the command buffer.
			// Empty the command buffer and wait until the GPU is idle.
			while(S_FALSE == m_pQuery->GetData( &numberOfPixelsDrawn, 
				sizeof(DWORD), D3DGETDATA_FLUSH ));

			return numberOfPixelsDrawn / m_dwMultySample;
		}

		bool DX9VizQuery::GetResultAsync(DWORD& out)
		{
			if( NULL == m_pQuery )
				return false;

			HRESULT hr = m_pQuery->GetData( &out, sizeof(DWORD), D3DGETDATA_FLUSH );
			if( S_OK == hr )
			{
				out /= m_dwMultySample;
				m_bWaitingForResult = false;
				return true;
			}
			else
			{
				m_bWaitingForResult = true;
				return false;
			}
		}

		void DX9VizQuery::PreDeviceReset()
		{
			SAFE_RELEASE(m_pQuery);
		}

		void DX9VizQuery::PostDeviceReset()
		{
			Create();
		}


	}
}//namespace Cool3D