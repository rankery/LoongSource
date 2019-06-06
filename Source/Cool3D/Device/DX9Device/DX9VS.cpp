#include "StdAfx.h"
#include ".\dx9vs.h"
#include "DX9Device.h"
#include "..\..\Util\Exception.h"
#include "..\..\VFS\IFS.h"
#include <d3dx9shader.h>

namespace Cool3D
{
	namespace Detail
	{
		DX9VS::DX9VS(void)
		{
			m_pVS=NULL;
			m_pConstTable=NULL;
		}

		DX9VS::~DX9VS(void)
		{
			SAFE_RELEASE(m_pVS);
			SAFE_RELEASE(m_pConstTable);
		}
		
		void DX9VS::CreateFromHLSL(const char* szTextBuffer,UINT bufSize,const char* szProfile,const char* szMainFun)
		{
			ID3DXBuffer *pCode=NULL;
			ID3DXBuffer *pErrorMsg=NULL;

			//--compile to a code buffer
			HRESULT hr=D3DXCompileShader(szTextBuffer,
				bufSize,
				NULL,//Macros
				NULL,//include
				szMainFun,
				szProfile,
				0,//flags of compile
				&pCode,
				&pErrorMsg,
				&m_pConstTable);
			if(FAILED(hr))
			{
				if(pErrorMsg!=NULL)
					LOGA("DX9VS compile failed,msg=%s\r\n.",(const char*)pErrorMsg->GetBufferPointer());
				else
					LOG(_T("DX9VS compile failed.\r\n"));
				THROW_FAILED(hr,NULL);
			}

			//--create device vs object
			ASSERT(pCode!=NULL);
			hr=DX9Dev()->CreateVertexShader((DWORD*)pCode->GetBufferPointer(),&m_pVS);
			THROW_FAILED(hr,NULL);

			SAFE_RELEASE(pCode);
			SAFE_RELEASE(pErrorMsg);
		}

		void DX9VS::CreateFromFile(const TCHAR* szSrcFile,const char* szProfile,const char* szMainFun)
		{
			IFS* pFS=ResMgr::Inst()->SafeGetFS(ResMgr::EFST_System);

			DWORD ret = pFS->GetSize( szSrcFile );
			ASSERT( ret != -1);
			ASSERT( ret > 0 );
			DWORD size = ret + 1;
			char * buf = new char[size];
			ret = pFS->LoadFile( buf, szSrcFile );
			ASSERT( ret > 0 );
			buf[size-1]=0;
			
			CreateFromHLSL( buf, size, szProfile, szMainFun );
			delete []buf;
		}

		void DX9VS::SetConstValue(const char* szName,const void *pVal,UINT bytes)
		{
			ASSERT(m_pConstTable!=NULL);
			HRESULT hr=m_pConstTable->SetValue(DX9Dev(),szName,pVal,bytes);
			THROW_FAILED(hr,NULL);
		}

		void DX9VS::SetFloat(const char* szName,const float val)
		{
			ASSERT(m_pConstTable!=NULL);
			HRESULT hr=m_pConstTable->SetFloat(DX9Dev(),szName,val);
			THROW_FAILED(hr,NULL);
		}

		void DX9VS::SetFloatArray(const char* szName,const float* pVal,UINT count)
		{
			ASSERT(m_pConstTable!=NULL);
			HRESULT hr=m_pConstTable->SetFloatArray(DX9Dev(),szName,pVal,count);
			THROW_FAILED(hr,NULL);
		}

		void DX9VS::SetVector(const char* szName,const Vector4* pVector)
		{
			ASSERT(m_pConstTable!=NULL);
			HRESULT hr=m_pConstTable->SetVector(DX9Dev(),szName,pVector);
			THROW_FAILED(hr,NULL);
		}

		void DX9VS::SetVectorArray(const char* szName,const Vector4* pVector,UINT count)
		{
			ASSERT(m_pConstTable!=NULL);
			HRESULT hr=m_pConstTable->SetVectorArray(DX9Dev(),szName,pVector,count);
			THROW_FAILED(hr,NULL);
		}

		void DX9VS::SetMatrix(const char* szName,const Matrix4* pMat)
		{
			ASSERT(m_pConstTable!=NULL);
			HRESULT hr=m_pConstTable->SetMatrix(DX9Dev(),szName,pMat);
			THROW_FAILED(hr,NULL);
		}

		void DX9VS::SetMatrixArray(const char* szName,const Matrix4* pMat,UINT count)
		{
			ASSERT(m_pConstTable!=NULL);
			HRESULT hr=m_pConstTable->SetMatrixArray(DX9Dev(),szName,pMat,count);
			THROW_FAILED(hr,NULL);
		}
	}
}//endof namespace