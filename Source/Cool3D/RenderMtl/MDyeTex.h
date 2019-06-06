#pragma once
#include "imaterial.h"
#include ".\MBitmap.h"

namespace Cool3D
{
	class MConstColor;
	class MBitmap;
	class MMAlphaMask;
	/** \class MDyeTex
		\brief 染色贴图
	*/
	class Cool3D_API MDyeTex :	public IMaterial
	{
	public:
		MDyeTex(void);
		virtual ~MDyeTex(void);

		//--IMaterial
		virtual bool IsResourceReady() const;
		virtual DWORD GetRenderTrait1() const;
		virtual DWORD GetRenderTrait2() const;

		//--
		void SetAlphaMap(const TCHAR* szPath);
		MMAlphaMask* GetAlphaMask();

	public:
		MConstColor*	m_pColor;
		Color4f			m_dyeColor;//染上去的颜色
		MBitmap*		m_pAlphaMap;//表示染色区域的贴图
		MBitmap*		m_pDiffuseMap;//基本纹理

		DECL_RTTI_DYNC(MDyeTex);
	};

}//namespace Cool3D