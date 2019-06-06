#include "StdAfx.h"
#include ".\effectlib.h"
#include "SFXPlug.h"

EffectLib::EffectLib(void)
{
}

EffectLib::~EffectLib(void)
{
}

bool EffectLib::LoadFile(const TCHAR* szFileName,bool bFreeOld)
{
	if(Kernel::Inst()->DefaultFS()->GetSize(szFileName) <=0)
		return false;//file not exsit
	
	if(bFreeOld)
		Destroy();

	FArchive ar;
	ar.OpenForRead(Kernel::Inst()->DefaultFS(),szFileName);

	//DeserialObjPtrVector(ar,m_eftList);

	CString szError;
	unsigned int size=0;
	ar>>size;
	if(size==0)
		return false;
	
	for(unsigned int i=0;i<size;i++)
	{
		tstring szClassName;
		ar>>szClassName;
		EffectBase* obj=NULL;

		try
		{
			obj=(EffectBase*)RTTFactory::Inst()->CreateObj(szClassName.c_str());
			THROW_NULL(obj,_T("bad class name"));
			obj->Deserialize(ar);
		}
		catch(Exception e)
		{
			szError+=szClassName.c_str();
			szError+=_T(":");
			szError+=e.What();
			szError+=_T("\t\r\n");
		}

		if(obj!=NULL)
			AddEft(obj);
	}//endof for
	ar.Close();

	if(!szError.IsEmpty())
	{
		AfxMessageBox(szError);
	}

	return true;

}

bool EffectLib::SaveFile(const TCHAR* szFileName)
{
	FArchive ar;
	ar.OpenForWrite(szFileName);

	SerialObjPtrVector(ar,m_eftList);
	ar.Close();

	return true;
}

EffectBase *_CloneEffect(EffectBase *pEft)
{
	ASSERT(pEft!=NULL);
	FArchive ar;
	ar.OpenForWrite(_T("SFXEditor.tmp"));
	pEft->Serialize(ar);
	ar.Close();

	EffectBase *pNewEft=(EffectBase *)RTTFactory::Inst()->CreateObj(pEft->GetClassName());
	if(pNewEft!=NULL)
	{
		FArchive ar2;
		ar2.OpenForRead(Kernel::Inst()->DefaultFS(),_T("SFXEditor.tmp"));
		pNewEft->Deserialize(ar2);
		ar2.Close();
	}
	return pNewEft;
}