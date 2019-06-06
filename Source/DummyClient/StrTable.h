#pragma once

/**	\class StrTable
	\brief 字符串表
	\remarks 为了适应多语言版本,程序中不能使用汉字,所有程序用字符串必须通过这个

*/
class StrTable
{
	typedef std::map<tstring,tstring> str2str;
public:
	StrTable(void);
	~StrTable(void);

	const TCHAR* operator [](const TCHAR *key);
	void Load(const TCHAR *filename);

private:
	str2str	m_data;
};

