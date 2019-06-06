#include "StdAfx.h"
#include ".\MtlCombiner.h"
#include "..\Util\Exception.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(MtlCombiner,IMaterial);

	MtlCombiner::MtlCombiner(void)
	{
		m_pMtl1=NULL;
		m_pMtl2=NULL;
		m_colorOp=ETOP_Disable;
		m_alphaOp=ETOP_Disable;
	}

	MtlCombiner::~MtlCombiner(void)
	{
		SAFE_DELETE(m_pMtl1);
		SAFE_DELETE(m_pMtl2);
	}

	void MtlCombiner::Serialize(FArchive& ar)
	{
		bool mtl1=m_pMtl1!=NULL;
		bool mtl2=m_pMtl2!=NULL;
		ar<<mtl1;
		ar<<mtl2;
		if(mtl1)
		{
			ar<<m_pMtl1->GetClassName();
			m_pMtl1->Serialize(ar);
		}
		if(mtl2)
		{
			ar<<m_pMtl2->GetClassName();
			m_pMtl2->Serialize(ar);
		}
		ar<<m_colorOp;
		ar<<m_alphaOp;

		IMaterial::Serialize(ar);
	}

	void MtlCombiner::Deserialize(FArchive& ar)
	{
		ASSERT(m_pMtl1==NULL);
		ASSERT(m_pMtl2==NULL);

		bool mtl1=false;
		bool mtl2=false;
		ar>>mtl1;
		ar>>mtl2;
		if(mtl1)
		{
			tstring szClass;
			ar>>szClass;
			m_pMtl1=(IMaterial*)RTTFactory::Inst()->CreateObj(szClass.c_str());
			ASSERT(m_pMtl1!=NULL);
			m_pMtl1->Deserialize(ar);
		}
		if(mtl2)
		{
			tstring szClass;
			ar>>szClass;
			m_pMtl2=(IMaterial*)RTTFactory::Inst()->CreateObj(szClass.c_str());
			ASSERT(m_pMtl2!=NULL);
			m_pMtl2->Deserialize(ar);
		}
		int tmp;
		ar>>tmp;	m_colorOp=(ETextureOp)tmp;
		ar>>tmp;	m_alphaOp=(ETextureOp)tmp;

		IMaterial::Deserialize(ar);
	}

	bool MtlCombiner::IsResourceReady() const
	{
		bool mtl1=true;
		bool mtl2=true;
		if(m_pMtl1)
			mtl1=m_pMtl1->IsResourceReady();
		if(m_pMtl2)
			mtl2=m_pMtl2->IsResourceReady();
		return mtl2 && mtl1;
	}
}//namespace Cool3D