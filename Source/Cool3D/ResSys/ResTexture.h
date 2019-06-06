#pragma once
#include "resbase.h"
#include "..\Cool3D.h"

namespace Cool3D
{
	class IDeviceTex;
	/**	\class ResTexture 
		\brief 具有资源属性的贴图对象
	*/
	class Cool3D_API ResTexture : public ResBase
	{
	public:
		ResTexture(const TCHAR* szName);
		virtual ~ResTexture(void);

		virtual UINT CreateContent(DWORD param);
		
		void Reload();
		/**	关联一个texture,由this管理其生存期
		*/
		void AttachTex(IDeviceTex *pTex);
		IDeviceTex *GetDeviceTex() const
		{ return m_pTex; }

	protected:
		IDeviceTex *m_pTex;

		DECL_RTTI(ResTexture);
	};
}//namespace Cool3D