#pragma once
#include "..\Public\MathBaseDefine.h"
#include "..\Math\Color.h"

namespace Cool3D
{
	/**	\class IPixelShader
		\brief Pixel Shader∂‘œÛ
	*/
	class IPixelShader
	{
	public:
		IPixelShader(void);
		virtual ~IPixelShader(void);

		virtual void CreateFromHLSL(const char* szTextBuffer,UINT bufSize,const char* szProfile,const char* szMainFun) =0;
		virtual void CreateFromFile(const TCHAR* szSrcFile,const char* szProfile,const char* szMainFun) =0;

		virtual void SetConstValue(const char* szName,const void *pVal,UINT bytes) =0;
		virtual void SetFloat(const char* szName,const float val) =0;
		virtual void SetFloatArray(const char* szName,const float* pVal,UINT count) =0;
		virtual void SetVector(const char* szName,const Vector4* pVector) =0;
		virtual void SetVectorArray(const char* szName,const Vector4* pVector,UINT count) =0;
		virtual void SetMatrix(const char* szName,const Matrix4* pMat) =0;
		virtual void SetMatrixArray(const char* szName,const Matrix4* pMat,UINT count) =0;
		virtual void SetColor4f(const char* szName,const Color4f& color) =0;
		virtual void SetBool(const char* szName,const BOOL val) =0;
		virtual void SetBoolArray(const char* szName,const BOOL* pVal,UINT count) =0;
		virtual void SetInt(const char* szName,const INT val) =0;
		virtual void SetIntArray(const char* szName,const INT* pVal,UINT count) =0;
	};
}//namespace Cool3D