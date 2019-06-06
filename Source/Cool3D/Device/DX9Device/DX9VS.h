#pragma once
#include "..\ivertshader.h"
#include <d3d9.h>
#include <d3dx9shader.h>

namespace Cool3D
{
	namespace Detail
	{
		class DX9VS : public IVertShader
		{
		public:
			DX9VS(void);
			virtual ~DX9VS(void);

			virtual void CreateFromHLSL(const char* szTextBuffer,UINT bufSize,const char* szProfile,const char* szMainFun);
			virtual void CreateFromFile(const TCHAR* szSrcFile,const char* szProfile,const char* szMainFun);

			virtual void SetConstValue(const char* szName,const void *pVal,UINT bytes);
			virtual void SetFloat(const char* szName,const float val);
			virtual void SetFloatArray(const char* szName,const float* pVal,UINT count);
			virtual void SetVector(const char* szName,const Vector4* pVector);
			virtual void SetVectorArray(const char* szName,const Vector4* pVector,UINT count);
			virtual void SetMatrix(const char* szName,const Matrix4* pMat);
			virtual void SetMatrixArray(const char* szName,const Matrix4* pMat,UINT count);

			IDirect3DVertexShader9	*GetDX9() const
			{ return m_pVS; }

		private:
			IDirect3DVertexShader9	*m_pVS;
			ID3DXConstantTable		*m_pConstTable;
		};
	}
}//endof namespace