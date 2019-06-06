#pragma once
#include "imaterial.h"
#include ".\MBitmap.h"

namespace Cool3D
{
	class MBitmap;
	/** \class MSEMTex
	\brief 球形环境映射
	*/
	class Cool3D_API MSEMTex :	public IMaterial
	{
	public:
		MSEMTex(void);
		virtual ~MSEMTex(void);

		//--IMaterial
		virtual bool IsResourceReady() const;
		virtual DWORD GetRenderTrait1() const;
		virtual DWORD GetRenderTrait2() const;

		//--
		void SetEnvMap(const TCHAR* szPath);

	public:
		MBitmap*			m_pEnvMap;		//球形环境映射贴图
		MBitmap*			m_pBaseMap;		//基本纹理

		DECL_RTTI_DYNC(MSEMTex);
	};
}//namespace Cool3D