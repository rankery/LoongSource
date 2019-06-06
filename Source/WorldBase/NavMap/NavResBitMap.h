#pragma once
#include "NavResBase.h"

namespace WorldBase
{
	/** \class NavResBitMap
		\brief Î»Í¼×ÊÔ´
	*/
	class NavResBitMap :
		public NavResBase
	{
	public:
		NavResBitMap(const TCHAR* szName);
		virtual ~NavResBitMap(void);

		//--NavResBase
		virtual UINT CreateContent(DWORD param);

		//--
		BitMap* GetBitMap(){ return m_pBitMap;}

	private:
		void LoadFromFile(IFS *pFS,const TCHAR* szName);

		BitMap* m_pBitMap;

		DECL_RTTI(NavResBitMap);
	};
}//namespace WorldBase