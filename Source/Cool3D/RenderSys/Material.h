#pragma once
#include "..\F3D.h"
#include "..\Math\Math.h"

	class IFS;
namespace F3D
{
	struct tagMtlDesc;
	class ResTexture;
	class IDeviceTex;

	/**	\class Material
	*/
	class F3D_API Material
	{
	public:
		Material(const tagMtlDesc& desc);
		virtual ~Material(void);

		IDeviceTex *GetDiffuse();
		IDeviceTex *GetLighting();

		bool IsTransparent()
		{ return m_bTransparent; }

	private:
		bool		m_bTransparent;
		ResTexture*	m_texDiffuse;
		ResTexture*	m_texLighting;
	};
}//namespace F3D