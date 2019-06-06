#pragma once
#include "resbase.h"
#include "..\Cool3D.h"

namespace Cool3D
{
	class IPixelShader;
	/**	\class ResPixelShader 
		\brief PixelShader
	*/
	class Cool3D_API ResPixelShader : public ResBase
	{
	public:
		ResPixelShader(const TCHAR* szName);
		virtual ~ResPixelShader(void);

		virtual UINT CreateContent(DWORD param);
		
		IPixelShader *GetPS() const
		{ return m_pPS; }

	protected:
		IPixelShader *m_pPS;

		DECL_RTTI(ResPixelShader);
	};
}//namespace Cool3D