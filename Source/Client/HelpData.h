#pragma once

struct tagCategory
{
	int id;
	tstring name;
};

struct tagSubCategory
{
	int id;
	int parent;
	tstring name;
	tstring path;
	int picnum; 
};


class HelpData
{
public:
	~HelpData();
	bool LoadFromFile();
	void Release();
	const std::vector<tagCategory*>& GetCategories() const { return m_vecCategories; }
	const std::vector<tagSubCategory*>& GetSubCategories() const { return m_vecSubCategories; }
	
	tstring FindParentCategoryName(const tagSubCategory *pSubCategory) const;

	
private:
	std::vector<tagCategory*> m_vecCategories;
	std::vector<tagSubCategory*> m_vecSubCategories;

};