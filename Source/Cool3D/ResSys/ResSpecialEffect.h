#pragma once
#include "..\Cool3D.h"
#include ".\ResBase.h"
#include "..\VFS\MemFS.h"

namespace Cool3D
{
	class SGEffectNode;
	/**	\class ResSpecialEffect 
		\brief 特效资源
	*/
	class Cool3D_API ResSpecialEffect :	public ResBase
	{
	public:
		ResSpecialEffect(const TCHAR* szName);
		virtual ~ResSpecialEffect(void);

		//--ResBase
		virtual UINT CreateContent(DWORD param);

		//--
		void DeserializeEffectNode(SGEffectNode* pEffectNode);

	private:
		MemFS	m_memFS;

		DECL_RTTI(ResSpecialEffect);
	};
}//namespace Cool3D