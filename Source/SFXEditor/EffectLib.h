#pragma once
#include "..\Cool3D\SpecialEffect\EffectAll.h"
#include "..\Cool3D\Kernel.h"
using namespace Cool3D;

EffectBase *_CloneEffect(EffectBase *pEft);

class EffectLib
{
public:
	EffectLib(void);
	~EffectLib(void);

	bool LoadFile(const TCHAR* szFileName,bool bFreeOld=true);
	bool SaveFile(const TCHAR* szFileName);
	void AddEft(EffectBase* pEft)
	{
		if(FindEft(pEft->GetName())==NULL)
			m_eftList.push_back(pEft);
		else
		{
			//发现有重名的
			CString szNewName;
			szNewName.Format(_T("%s_%d"),pEft->GetClassName(),GetTickCount());
			if(szNewName.GetLength()>=32)
				szNewName=szNewName.Mid(szNewName.GetLength());
			ASSERT(FindEft(szNewName)==NULL);
			pEft->Rename(szNewName);
			m_eftList.push_back(pEft);
		}
	}

	void RemoveEft(const TCHAR* szName)
	{
		for(vector<EffectBase*>::iterator iter=m_eftList.begin();
			iter!=m_eftList.end();++iter)
		{
			EffectBase *pEft=(*iter);
			if(_tcsicmp(pEft->GetName(),szName)==0)
			{
				m_eftList.erase(iter);
				delete pEft;
				return;
			}
		}
	}

	EffectBase* FindEft(const TCHAR* szName)
	{
		for(vector<EffectBase*>::iterator iter=m_eftList.begin();
			iter!=m_eftList.end();++iter)
		{
			EffectBase *pEft=(*iter);
			if(_tcsicmp(pEft->GetName(),szName)==0)
				return pEft;
		}
		return NULL;
	}

	EffectBase *CloneEft(const TCHAR* szName)
	{
		EffectBase *pEft=FindEft(szName);
		if(pEft==NULL)
			return NULL;
		return _CloneEffect(pEft);
	}

	void UpdateList(CListBox *pRenderList,CListBox *pPathList)
	{
		pRenderList->ResetContent();
		pPathList->ResetContent();
		for(vector<EffectBase*>::iterator iter=m_eftList.begin();
			iter!=m_eftList.end();++iter)
		{
			EffectBase *pEft=(*iter);
			if(pEft->IS_KIND_OF(EffectPathCtrl))
				pPathList->AddString(pEft->GetName());
			else
				pRenderList->AddString(pEft->GetName());
		}
	}

	void Destroy()
	{
		for(vector<EffectBase*>::iterator iter=m_eftList.begin();
			iter!=m_eftList.end();++iter)
		{
			EffectBase *pEft=(*iter);
			delete pEft;
		}
		m_eftList.clear();
	}

private:
	vector<EffectBase*>	m_eftList;
};
