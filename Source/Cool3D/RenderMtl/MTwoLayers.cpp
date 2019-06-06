#include "StdAfx.h"
#include ".\mtwolayers.h"
#include ".\MMAlphaMask.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(MTwoLayers,IMaterial);

	MTwoLayers::MTwoLayers(void)
	{
		m_pColor = NULL;
		m_pLayer0 = NULL;
		m_pLayer1 = NULL;
	}

	MTwoLayers::~MTwoLayers(void)
	{
		SAFE_DELETE(m_pColor);
		SAFE_DELETE(m_pLayer0);
		SAFE_DELETE(m_pLayer1);
	}

	bool MTwoLayers::IsResourceReady() const
	{
		return m_pLayer0!=NULL && m_pLayer0->IsResourceReady() 
			&& m_pLayer1!=NULL && m_pLayer1->IsResourceReady();
	}

	void MTwoLayers::Serialize(FArchive& ar)
	{
		bool bHave=m_pColor!=NULL;
		ar<<bHave;
		if(bHave)
			m_pColor->Serialize(ar);
		bHave=m_pLayer0!=NULL;
		ar<<bHave;
		if(bHave)
			m_pLayer0->Serialize(ar);
		bHave=m_pLayer1!=NULL;
		ar<<bHave;
		if(bHave)
			m_pLayer1->Serialize(ar);

		IMaterial::Serialize(ar);
	}

	void MTwoLayers::Deserialize(FArchive& ar)
	{
		//--必须是没有创建的
		ASSERT(m_pColor==NULL);
		ASSERT(m_pLayer0==NULL);
		ASSERT(m_pLayer1==NULL);

		//--
		bool bHave=false;
		ar>>bHave;
		if(bHave)
		{
			m_pColor=new MConstColor;
			m_pColor->Deserialize(ar);
		}
		ar>>bHave;
		if(bHave)
		{
			m_pLayer0=new MBitmap;
			m_pLayer0->Deserialize(ar);
		}
		ar>>bHave;
		if(bHave)
		{
			m_pLayer1=new MBitmap;
			m_pLayer1->Deserialize(ar);
		}

		IMaterial::Deserialize(ar);
	}


	MMAlphaMask* MTwoLayers::GetAlphaMask()
	{
		MMAlphaMask* pMM=NULL;

		if(m_pLayer0)
		{
			pMM=(MMAlphaMask*)m_pLayer0->GetModifier(_T("MMAlphaMask"));
		}

		return pMM;
	}

}//namespace Cool3D