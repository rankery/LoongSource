
#include "StdAfx.h"
#include ".\idraw2d.h"
#include ".\draw2dImp.h"

namespace Cool3D
{
	static Draw2DImp g_draw2D;
	IDraw2D* IDraw2D::Inst()
	{
		return &g_draw2D;
	}

}//namespace Cool3D