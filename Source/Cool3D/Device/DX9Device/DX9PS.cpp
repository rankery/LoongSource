#include "StdAfx.h"
#include ".\dx9ps.h"
#include "DX9Device.h"
#include "..\..\Util\Exception.h"
#include "..\..\VFS\IFS.h"
#include <d3dx9shader.h>

namespace Cool3D
{
	namespace Detail
	{
		DX9PS::DX9PS(void)
		{
			m_pPS=NULL;
			m_pConstTable=NULL;
		}

		DX9PS::~DX9PS(void)
		{
			SAFE_RELEASE(m_pPS);
			SAFE_RELEASE(m_pConstTable);
		}

		void DX9PS::CreateFromHLSL(const char* szTextBuffer,UINT bufSize,const char* szProfile,const char* szMainFun)
		{
			ID3DXBuffer *pCode=NULL;
			ID3DXBuffer *pErrorMsg=NULL;
			
			//--检查Device是否支持指定版本号
			string szVer=szProfile;
			ASSERT(strlen(szProfile)>4);
			if(szProfile[3]<='1'
				&&Device()->CapsPixelShaderVersion()>=D3DPS_VERSION(3,0))
			{
				szVer="ps_2_0";
			}

			//--compile to a code buffer
			HRESULT hr=D3DXCompileShader(szTextBuffer,
				bufSize,
				NULL,//Macros
				NULL,//include
				szMainFun,
				szVer.c_str(),
				0,//flags of compile
				&pCode,
				&pErrorMsg,
				&m_pConstTable);
			if(FAILED(hr))
			{
				if(pErrorMsg!=NULL)
					LOGA("DX9PS compile failed,msg=%s\r\ncode=%s\r\n.",
					(const char*)pErrorMsg->GetBufferPointer(),szTextBuffer);
				else
					LOGA("DX9PS compile failed,code=%s.\r\n",szTextBuffer);
				THROW_FAILED(hr,NULL);
			}

			//--create device ps object
			ASSERT(pCode!=NULL);
			hr=DX9Dev()->CreatePixelShader((DWORD*)pCode->GetBufferPointer(),&m_pPS);
			THROW_FAILED(hr,NULL);

			SAFE_RELEASE(pCode);
			SAFE_RELEASE(pErrorMsg);
		}

		void DX9PS::CreateFromFile(const TCHAR* szSrcFile,const char* szProfile,const char* szMainFun)
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

		void DX9PS::SetConstValue(const char* szName,const void *pVal,UINT bytes)
		{
			ASSERT(m_pConstTable!=NULL);
			HRESULT hr=m_pConstTable->SetValue(DX9Dev(),szName,pVal,bytes);
			THROW_FAILED(hr,NULL);
		}

		void DX9PS::SetFloat(const char* szName,const float val)
		{
			ASSERT(m_pConstTable!=NULL);
			HRESULT hr=m_pConstTable->SetFloat(DX9Dev(),szName,val);
			THROW_FAILED(hr,NULL);
		}
		
		void DX9PS::SetFloatArray(const char* szName,const float* pVal,UINT count)
		{
			ASSERT(m_pConstTable!=NULL);
			HRESULT hr=m_pConstTable->SetFloatArray(DX9Dev(),szName,pVal,count);
			THROW_FAILED(hr,NULL);
		}

		void DX9PS::SetVector(const char* szName,const Vector4* pVector)
		{
			ASSERT(m_pConstTable!=NULL);
			HRESULT hr=m_pConstTable->SetVector(DX9Dev(),szName,pVector);
			THROW_FAILED(hr,NULL);
		}

		void DX9PS::SetVectorArray(const char* szName,const Vector4* pVector,UINT count)
		{
			ASSERT(m_pConstTable!=NULL);
			HRESULT hr=m_pConstTable->SetVectorArray(DX9Dev(),szName,pVector,count);
			THROW_FAILED(hr,NULL);
		}

		void DX9PS::SetMatrix(const char* szName,const Matrix4* pMat)
		{
			ASSERT(m_pConstTable!=NULL);
			HRESULT hr=m_pConstTable->SetMatrix(DX9Dev(),szName,pMat);
			THROW_FAILED(hr,NULL);
		}

		void DX9PS::SetMatrixArray(const char* szName,const Matrix4* pMat,UINT count)
		{
			ASSERT(m_pConstTable!=NULL);
			HRESULT hr=m_pConstTable->SetMatrixArray(DX9Dev(),szName,pMat,count);
			THROW_FAILED(hr,NULL);
		}

		void DX9PS::SetColor4f(const char* szName,const Color4f& color)
		{
			SetConstValue(szName,&color,sizeof(color));
		}

		void DX9PS::SetBool( const char* szName,const BOOL val )
		{
			ASSERT(m_pConstTable!=NULL);
			HRESULT hr=m_pConstTable->SetBool(DX9Dev(),szName,val);
			THROW_FAILED(hr,NULL);
		}

		void DX9PS::SetBoolArray( const char* szName,const BOOL* pVal,UINT count )
		{
			ASSERT(m_pConstTable!=NULL);
			HRESULT hr=m_pConstTable->SetBoolArray(DX9Dev(),szName,pVal,count);
			THROW_FAILED(hr,NULL);
		}

		void DX9PS::SetInt( const char* szName,const INT val )
		{
			ASSERT(m_pConstTable!=NULL);
			HRESULT hr=m_pConstTable->SetInt(DX9Dev(),szName,val);
			THROW_FAILED(hr,NULL);
		}

		void DX9PS::SetIntArray( const char* szName,const INT* pVal,UINT count )
		{
			ASSERT(m_pConstTable!=NULL);
			HRESULT hr=m_pConstTable->SetIntArray(DX9Dev(),szName,pVal,count);
			THROW_FAILED(hr,NULL);
		}
	}
}//endof namespace