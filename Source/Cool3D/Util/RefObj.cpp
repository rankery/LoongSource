#include "StdAfx.h"
#include ".\refobj.h"

namespace Cool3D
{
	IMP_RTTI(RefObj,RTTIObj);

	RefObj::RefObj(void) : m_nRef(0)
	{
	}

	RefObj::~RefObj(void)
	{
	}
}//endof namespace Cool3D