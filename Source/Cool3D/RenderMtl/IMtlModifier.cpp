#include "StdAfx.h"
#include ".\imtlmodifier.h"

namespace Cool3D
{
	IMP_RTTI(IMtlModifier,RTTIObj);

	class IMtlModifier::Member
	{
	public:
		tstring	m_szName;
	public:
		Member()	{}
		~Member()	{}
	};

	IMtlModifier::IMtlModifier(void)
	{
		m_p=new Member;
	}

	IMtlModifier::~IMtlModifier(void)
	{
		delete m_p;
	}

	void IMtlModifier::SetName(const TCHAR* szName)
	{
		m_p->m_szName=szName;
	}

	const TCHAR* IMtlModifier::GetName() const
	{
		return m_p->m_szName.c_str();
	}

	void IMtlModifier::Serialize(FArchive& ar)
	{
		ar<<m_p->m_szName;
	}

	void IMtlModifier::Deserialize(FArchive& ar)
	{
		ar>>m_p->m_szName;
	}

	void IMtlModifier::Update(float deltaTime,IMaterial *pOwner)
	{
	}

	void IMtlModifier::ResetAni(IMaterial *pOwner)
	{}
}//namespace Cool3D