// 文本数据文件分析
// .文本格式应为:
//		BEGIN 区块名称 ;注释
//		数据关键字 {数据值字符串} ;值可以含任意字符
//		END
// .每次NextToken()取得下一个关键字
// .NextValueToke()取得紧随关键字的一个值,
//////////////////////////////////////////////////////////////////////

#pragma once
#include "..\Cool3D\vfs\ifs.h"

class TxtFile  
{
public:
	bool GetTokenValue_String(LPCTSTR name,TCHAR* buffer,int bufferlen);
	void RewindSection();
	void NextValueToken();
	void Destroy();
	void OpenSection(LPCTSTR name);
	void Load(IFS *pFS,LPCTSTR filename);
	float GetTokenValue(LPCTSTR name);

	TxtFile();
	~TxtFile();

	TCHAR m_token[512];

	BOOL NextToken();
protected:	
	TCHAR *m_buf;
	TCHAR *m_sectionstart;
	TCHAR *m_curpos;
};
