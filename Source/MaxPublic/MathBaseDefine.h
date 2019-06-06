#pragma once
#include <d3dx9math.h>
#pragma comment(lib,"d3dx9.lib")
#include <string>
#include <tchar.h>
using namespace std;

namespace Cool3D
{
	const float FLOAT_MAX=3.402823466e+38f;
	const float FLOAT_EPSILON=1.192092896e-07F;
	const float FLOAT_PI=3.141592654f;

	//Ω«∂»”Îª°∂»ªªÀ„
	inline float _DegToRad(float a) { return a*0.01745329252f;};
	inline float _RadToDeg(float a) { return a*57.29577951f;};

	typedef D3DXVECTOR2		Vector2;
	typedef D3DXVECTOR3		Vector3;
	typedef D3DXVECTOR4		Vector4;
	typedef D3DXQUATERNION	Quaternion;
	typedef D3DXMATRIX		Matrix4;

	struct Matrix2
	{
		float  _11, _12;
		float  _21, _22;
	};

	struct Matrix3x4
	{
		float  m[3][4];
	};

	inline DWORD _MakeFourCCA( string szStr ) 
	{ 
		char str[4];
		memset(str,0,sizeof(str));
		_tcsncpy(str,szStr.c_str(),sizeof(str));
		return (str[3]<<24) | (str[2]<<16) | (str[1]<<8) | str[0]; 
	}

	inline DWORD _MakeFourCC( wstring szStr ) 
	{ 
		WCHAR str[4];
		memset(str,0,sizeof(str));
		wcsncpy(str,szStr.c_str(),sizeof(str)/sizeof(WCHAR));
		char* p=(char*)str;
		return (p[6]<<24) | (p[4]<<16) | (p[2]<<8) | p[0];
	}

	inline string _FourCC2StrA(DWORD dw)
	{
		char str[5];
		str[0]=(char)(dw&0xff); 
		str[1]=(char)((dw>>8)&0xff); 
		str[2]=(char)((dw>>16)&0xff); 
		str[3]=(char)((dw>>24)&0xff);
		str[4]=0;
		return string(str);
	}

	inline wstring _FourCC2Str(DWORD dw)
	{
		WCHAR str[5];
		str[0]=(WCHAR)(dw&0xff); 
		str[1]=(WCHAR)((dw>>8)&0xff); 
		str[2]=(WCHAR)((dw>>16)&0xff); 
		str[3]=(WCHAR)((dw>>24)&0xff);
		str[4]=0;
		return wstring(str);
	}
}//namespace Cool3D