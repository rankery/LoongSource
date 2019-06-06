#pragma once
#include "..\Cool3D.h"
#include "MBitmap.h"

namespace Cool3D
{
	/**	\class MSky
		\brief Ìì¿Õ²ÄÖÊ
	*/
	class Cool3D_API MSky :	public MBitmap
	{
	public:
		MSky(void);
		virtual ~MSky(void);
	private:

		DECL_RTTI_DYNC(MSky);
	};
}//namespace Cool3D