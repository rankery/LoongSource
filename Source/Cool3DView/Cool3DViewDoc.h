// Cool3DViewDoc.h :  CCool3DViewDoc 类的接口
//


#pragma once
#include "..\Cool3D\Cool3DEngine.h"
using namespace Cool3D;

class CCool3DViewDoc : public CDocument
{
protected: // 仅从序列化创建
	CCool3DViewDoc();
	DECLARE_DYNCREATE(CCool3DViewDoc)

// 属性
public:

// 操作
public:

// 重写
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 实现
public:
	virtual ~CCool3DViewDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

public:
	AniMsgTable* GetAniMsgTable();
	AniMsgTable m_aniMsgTable;
	CString m_cstrCSVFilePath;
	CString GetCSVFilePath();
	void SetCSVFilePath(const TCHAR *szPath);
};


