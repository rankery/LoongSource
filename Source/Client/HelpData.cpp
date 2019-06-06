#include "stdafx.h"
#include "HelpData.h"

HelpData::~HelpData()
{

	Release();


}
bool HelpData::LoadFromFile()
{
	Release();
	tstring strCategory = g_strLocalPath + _T("\\help\\help_category_name.xml");
	tstring strSubCategory = g_strLocalPath + _T("\\help\\help_path.xml");

	list<tstring> listCategory;
	TObjRef<VarContainer> varCategory = CreateObj("Category", "VarContainer");
	if (!varCategory->Load("VFS_System", strCategory.c_str(), "id", &listCategory))
	{
		KillObj("Category");
		return false;
	}
	list<tstring>::iterator end = listCategory.end();
	for (list<tstring>::iterator iter = listCategory.begin();
		iter != end; ++iter)
	{
		tagCategory *pCategory = new tagCategory();
		pCategory->id = varCategory->GetInt(_T("id"), iter->c_str(), 0);
		pCategory->name = varCategory->GetString(_T("name"), iter->c_str(), _T(""));
		m_vecCategories.push_back(pCategory);
	}
	
	KillObj("Category");

	list<tstring> listSubCategory;
	TObjRef<VarContainer> varSubCategory = CreateObj("SubCategory", "VarContainer");
	if (!varSubCategory->Load("VFS_System", strSubCategory.c_str(), "id", &listSubCategory))
	{
		KillObj("SubCategory");
		return false;
	}
	list<tstring>::iterator iEnd = listSubCategory.end();
	for (list<tstring>::iterator iter = listSubCategory.begin();
		iter != iEnd; ++iter)
	{
		tagSubCategory *pSubCategory = new tagSubCategory();
		pSubCategory->id = varSubCategory->GetInt(_T("id"), iter->c_str(), 0);
		pSubCategory->parent = varSubCategory->GetInt(_T("category"), iter->c_str(), 0);
		pSubCategory->name = varSubCategory->GetString(_T("name"), iter->c_str(), _T(""));
		pSubCategory->path = varSubCategory->GetString(_T("path"), iter->c_str(), _T(""));
		pSubCategory->picnum = varSubCategory->GetInt(_T("picnum"), iter->c_str(), 0);
		m_vecSubCategories.push_back(pSubCategory); 
	}

	KillObj("SubCategory");

	return true;
}

tstring HelpData::FindParentCategoryName(const tagSubCategory *pSubCategory) const
{
	int parent = pSubCategory->parent;
	std::vector<tagCategory*>::const_iterator end = m_vecCategories.end();
	for (std::vector<tagCategory*>::const_iterator iter = m_vecCategories.begin(); iter != end; ++iter)
	{
		if ((*iter)->id == parent)
			return (*iter)->name;
	}
	return _T("");
}

void HelpData::Release()
{
	std::vector<tagCategory*>::iterator end = m_vecCategories.end();
	for (std::vector<tagCategory*>::iterator iter = m_vecCategories.begin(); iter != end; ++iter)
	{
		SAFE_DEL(*iter);
	}

	std::vector<tagSubCategory*>::iterator subend = m_vecSubCategories.end();
	for (std::vector<tagSubCategory*>::iterator iter = m_vecSubCategories.begin(); iter != subend; ++iter)
	{
		SAFE_DEL(*iter);
	}
}
