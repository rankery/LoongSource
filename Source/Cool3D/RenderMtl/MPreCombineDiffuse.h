#pragma once
#include "..\F3D.h"
#include "MBitmap.h"

namespace F3D
{
	/**	\class MPreCombineDiffuse 
		\brief 使用预混合diffuse的地形材质
	*/
	class F3D_API MPreCombineDiffuse : public IRenderMtl
	{
	public:
		MPreCombineDiffuse(void);
		virtual ~MPreCombineDiffuse(void);

		//--IMaterial
		virtual bool IsResourceReady() const;

		MBitmap*	m_pBaseTex;
	private:
		DECL_RTTI_DYNC(MPreCombineDiffuse);
	};
}//namespace F3D