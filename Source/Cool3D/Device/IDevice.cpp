#include "StdAfx.h"
#include ".\idevice.h"

namespace Cool3D
{
	IDevice::IDevice(void)
	{
	}

	IDevice::~IDevice(void)
	{
	}

	float IDevice::NormalizeScreenX(UINT x)
	{
		return  ( ( ( 2.0f * x ) / GetClientWidth()  ) - 1 );
	}
	float IDevice::NormalizeScreenY(UINT y)
	{
		return -( ( ( 2.0f * y ) / GetClientHeight() ) - 1 );
	}

	void IDevice::BitBltBackbuffer(HDC src,int x,int y,int w,int h)
	{}
}//namespace Cool3D