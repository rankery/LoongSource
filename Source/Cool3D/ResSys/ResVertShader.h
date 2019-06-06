#pragma once
#include "resbase.h"
#include "..\Cool3D.h"

namespace Cool3D
{
	class IVertShader;
	/**	\class ResVertShader 
		\brief VertexShader
	*/
	class Cool3D_API ResVertShader : public ResBase
	{
	public:
		ResVertShader(const TCHAR* szName);
		virtual ~ResVertShader(void);

		virtual UINT CreateContent(DWORD param);
		
		IVertShader *GetVS() const
		{ return m_pVS; }

	protected:
		IVertShader *m_pVS;

		DECL_RTTI(ResVertShader);
	};
}//namespace Cool3D