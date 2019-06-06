#pragma once
#include "..\Cool3D.h"
#include "Noncopyable.h"
#include "RTTIObj.h"


namespace Cool3D
{
	/**	\class RefObj 
		\brief 使用引用计数的对象的基类, 所有派生类都不可自动拷贝
	*/
	class Cool3D_API RefObj : public RTTIObj, private Noncopyable
	{
	public:
		RefObj(void);
		virtual ~RefObj(void);

		int GetRef() const
		{ return m_nRef; }
		void IncRef()
		{ 
			m_nRef++; 
		}
		void DecRef()
		{
			m_nRef--;
		}

	private:
		int m_nRef;

		DECL_RTTI(RefObj);
	};
}//endof namespace Cool3D