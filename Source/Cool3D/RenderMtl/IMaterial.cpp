#include "StdAfx.h"
#include ".\imaterial.h"
#include "..\RenderSys\MtlRenderCache.h"
#include "..\Util\Exception.h"
#include "IMtlModifier.h"
#include ".\MMAlphaMask.h"
#include ".\MMFrameBlend.h"
#include "..\Kernel.h"

namespace Cool3D
{
	IMP_RTTI(IMaterial,RTTIObj);

	class IMaterial::Member
	{
	public:
		vector<IMtlModifier*>	m_mods;
	public:
		Member(){}
		~Member()
		{
			for(size_t i=0;i<m_mods.size();i++)
			{
				IMtlModifier *pMod=m_mods[i];
				delete pMod;
			}
			m_mods.clear();
		}

	};
	IMaterial::IMaterial(void)
	{
		m_pFallbackMtl=NULL;
		m_pRenderCache=NULL;
		m_p=new Member;
	}

	IMaterial::~IMaterial(void)
	{
		SAFE_DELETE(m_pRenderCache);
		SAFE_DELETE(m_p);
	}

	void IMaterial::InvalidateRenderCache()
	{
		SAFE_DELETE(m_pRenderCache);
	}

	void IMaterial::AttachRenderCache(MtlRenderCache *pCache)
	{
		SAFE_DELETE(m_pRenderCache);//释放旧的
		m_pRenderCache=pCache;
	}

	void IMaterial::UpdateRenderCache()
	{
		if(m_pRenderCache)
			m_pRenderCache->UpdateMtl(this);
	}

	void IMaterial::AttachModifier(IMtlModifier *pMod)	
	{	
		m_p->m_mods.push_back(pMod);
	}

	bool IMaterial::RemoveModifier( const TCHAR* szName )
	{
		bool bRet=false;

		vector<IMtlModifier*>::iterator iter=m_p->m_mods.begin();
		for(;iter!=m_p->m_mods.end();)
		{
			IMtlModifier* pMod=*iter;
			if(_tcscmp(pMod->GetName(),szName)==0)
			{
				SAFE_DELETE(pMod);
				iter=m_p->m_mods.erase(iter);
				bRet=true;
			}
			else
				++iter;
		}

		return bRet;
	}

	void IMaterial::UpdateModifiers(float deltaTime)
	{
		for(vector<IMtlModifier*>::iterator iter=m_p->m_mods.begin();
			iter!=m_p->m_mods.end();++iter)
		{
			IMtlModifier *pMod=(*iter);
			pMod->Update(deltaTime,this);
		}
	}

	void IMaterial::ResetModAni()
	{
		for(vector<IMtlModifier*>::iterator iter=m_p->m_mods.begin();
			iter!=m_p->m_mods.end();++iter)
		{
			IMtlModifier *pMod=(*iter);
			pMod->ResetAni(this);
		}
	}

	UINT IMaterial::GetNumMods() const	
	{	
		return m_p->m_mods.size();
	}

	const IMtlModifier *IMaterial::GetModifier(UINT i) const
	{
		ASSERT(i<m_p->m_mods.size());
		return m_p->m_mods[i];
	}

	IMtlModifier *IMaterial::GetModifier(const TCHAR* szClassName)
	{
		vector<IMtlModifier*>::iterator iter=m_p->m_mods.begin();
		for(;iter!=m_p->m_mods.end();iter++)
		{
			IMtlModifier *pMod=(*iter);
			if(pMod->IsStaticClass(szClassName))
				return pMod;
		}
		return NULL;
	}

	bool IMaterial::IsResourceReady() const
	{
		return true;
	}

	DWORD IMaterial::GetRenderTrait1() const
	{
		return 0xFFFFFFFF;
	}

	DWORD IMaterial::GetRenderTrait2() const
	{
		return DWORD(this);
	}

	void IMaterial::Serialize(FArchive& ar)
	{
		//--处理modifier数组
		SerialObjPtrVector(ar,m_p->m_mods);
	}

	void IMaterial::Deserialize(FArchive& ar)
	{
		//--必须是没有创建过的
		ASSERT(m_p->m_mods.size()==0);
		ASSERT(m_pRenderCache==NULL);

		//--处理modifier数组
		DeserialObjPtrVector(ar,m_p->m_mods);
	}

	bool IMaterial::SaveToFile(IMaterial *pMtl,const TCHAR* szFileName)
	{
		if(pMtl==NULL)
			return false;
		if(pMtl->GetRTTInfo()->m_pfnCreateObj==NULL)
		{
			Kernel::Inst()->SetLastError(_T("Can't save a mtl none dynamic created."));
			return false;
		}

		FArchive ar;
		ar.OpenForWrite(szFileName);
		ar<<pMtl->GetRTTInfo()->m_szClassName;
		pMtl->Serialize(ar);
		return true;
	}

	IMaterial* IMaterial::LoadFromFile(const TCHAR* szFileName)
	{
		FArchive ar;
		ar.OpenForRead(Kernel::Inst()->DefaultFS(),szFileName);
		tstring szClassName;
		ar>>szClassName;

		IMaterial *pNewMtl=(IMaterial*)RTTFactory::Inst()->CreateObj(szClassName.c_str());
		if(pNewMtl!=NULL)
			pNewMtl->Deserialize(ar);
		return pNewMtl;
	}

}//namespace Cool3D