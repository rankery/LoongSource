#pragma once
#include "..\ipixelshader.h"
#include <d3d9.h>
#include <d3dx9shader.h>

namespace Cool3D
{
	namespace Detail
	{
		class DX9PS : public IPixelShader
		{
		public:
			DX9PS(void);
			virtual ~DX9PS(void);

			virtual void CreateFromHLSL(const char* szTextBuffer,UINT bufSize,const char* szProfile,const char* szMainFun);
			virtual void CreateFromFile(const TCHAR* szSrcFile,const char* szProfile,const char* szMainFun);

			virtual void SetConstValue(const char* szName,const void *pVal,UINT bytes);
			virtual void SetFloat(const char* szName,const float val);
			virtual void SetFloatArray(const char* szName,const float* pVal,UINT count);
			virtual void SetVector(const char* szName,const Vector4* pVector);
			virtual void SetVectorArray(const char* szName,const Vector4* pVector,UINT count);
			virtual void SetMatrix(const char* szName,const Matrix4* pMat);
			virtual void SetMatrixArray(const char* szName,const Matrix4* pMat,UINT count);
			virtual void SetColor4f(const char* szName,const Color4f& color);
			virtual void SetBool(const char* szName,const BOOL val);
			virtual void SetBoolArray(const char* szName,const BOOL* pVal,UINT count);
			virtual void SetInt(const char* szName,const INT val);
			virtual void SetIntArray(const char* szName,const INT* pVal,UINT count);

			IDirect3DPixelShader9	*GetDX9() const
			{ return m_pPS; }

		private:
			IDirect3DPixelShader9	*m_pPS;
			ID3DXConstantTable		*m_pConstTable;
		};
	}
}//endof namespace