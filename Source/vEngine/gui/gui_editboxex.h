//-----------------------------------------------------------------------------
//!\file gui_editboxex.h
//!\author Lyp
//!
//!\date 2004-12-08
//! last 2008-01-21
//!
//!\brief 界面系统 多行 edit box 控件
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine {

enum EGUIEditChar
{
	EGEC_Char		= 0,	// 普通字符
	EGEC_Normal		= 1000,
	EGEC_Return		= 1001,	// 硬回车
	EGEC_VReturn	= 1002,	// 软回车
};

//-----------------------------------------------------------------------------
// 每个字符的信息
//-----------------------------------------------------------------------------
struct tagGUIEditCharInfo	// editbox char info
{
	EGUIEditChar	eType;		// 类型
	INT				nWidth;		// 字宽
	tagGUIEditCharInfo() { eType = EGEC_Char; nWidth = 0; }
};

//-----------------------------------------------------------------------------
// 界面系统 edit box 控件
//-----------------------------------------------------------------------------
class VENGINE_API GUIEditBoxEx : public GUIStatic
{
public:
	virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
	virtual VOID Destroy();
	virtual VOID Update();
	virtual VOID Render();
	virtual BOOL OnInputMessage(tagGUIInputMsg* pMsg);
	virtual VOID SendEvent(tagGUIEvent* pEvent);
	virtual VOID AddChild(GUIWnd* pChild);	// 添加子窗口
	virtual GUIWnd* OnActive();	
	virtual VOID OnDeactive(GUIWnd* pWnd);
	virtual BOOL Resize(tagPoint& ptSize); // 改变控件大小,传入新的大小

	// 对外的文字处理接口
	VOID		Clear();	// 删除所有文本
	VOID		SetText(LPCTSTR szText);	// 直接设置文本框内容
	VOID		AddText(LPCTSTR szText) { AddString(m_strText.size()-1, szText); }	// 添加文本
	tstring&	GetText(BOOL bVirtualReturn);	// bVirtualReturn:是否需要虚拟回车

	// 基本操作以及信息获取
	INT			GetLineNum() { return m_nLine; }	// 得到文本总行数
	VOID		DelFrontLine(INT nLine);	// 删除前面多少行
	INT			GetCurLine() { return m_nCurFirstLine; }	// 得到当前显示的第一行是实际的哪一行
	VOID		SetCurLine(INT n, BOOL bSendEvent=TRUE);	// 设置当前行
	INT			GetCursorLine() { return this->GetLine(m_nCursorPos); }	// 得到光标所在行
	BOOL		IsShowLastLine() { return (m_nCurFirstLine+m_nVisibleLine >= m_nLine-2); }
	VOID		SelectAll(); // 全部选中
	
	// 书签
	VOID		SetBookmark(INT nLine, DWORD dwColor);
	BOOL		IsSetBookmark(INT nLine) { return m_mapBookmark.IsExist(nLine); }

	VOID		EnterLock() { EnterCriticalSection(&m_Lock); }
	VOID		LeaveLock() { LeaveCriticalSection(&m_Lock); }

	GUIEditBoxEx();

protected:
	// --参与主要计算的变量
	std::vector<tagGUIEditCharInfo> m_vecChar;	// 每字信息
	INT		m_nSelStart;						// 选择开始处
	INT		m_nSelEnd;							// 选择结束处
	INT		m_nCursorPos;						// 光标位置
	INT		m_nDispStart;						// 当前从哪一字开始显示
	INT		m_nLine;							// 一共多少行
	INT		m_nCurFirstLine;					// 当前显示的第一行是实际哪一行
	INT		m_nVisibleLine;						// 一共可显示的行数

	// --不参与主要计算的变量
	bool	m_bShowCursor;						// 当前是否显示光标(光标是闪烁的)
	BOOL	m_bFixWidth;						// 是否等宽
	INT		m_nEngWidth;						// 英文宽度
	INT		m_nChnWidth;						// 中文宽度
	tstring	m_strOut;							// 输出用的字符串

	TMap<INT, DWORD>	m_mapBookmark;			// 书签
	CRITICAL_SECTION	m_Lock;					// 为保证IMSG宏的多线程同步
	

	// 向指定位置，指定行添加字符串，返回增加字符量
	INT		AddString(INT nPos, tstring str);
	INT		FindFirstCharThisLine(INT nPos);	// 得到指定位置所在行的第一个字
	INT		FindFirstChar(INT nLine);			// 得到指定行的第一个字
	INT		FindReturnThisLine(INT nPos);		// 得到指定位置所在行的换行符位置
	INT		GetLine(INT nPos);					// 由指定位置得到行号,必须确保m_nDispStart以及m_nCurFirstLine正确
	INT		ReMakeup(INT nPos, INT nEndPos);	// 从第几个字开始重新排版,返回增加字符量
	VOID	ChangeDisplayLineStart(INT nLine, BOOL bSendEvent=TRUE);	// 修改从那一行开始显示(nLine为相对值)
	VOID	ChangeCursorPos(INT nPos);			// 修改鼠标光标位置(nPos为相对值)
	VOID	DelString(INT nPos, INT nEndPos);	// 删除指定字符
	INT		GetCharIndex(tagPoint pt);			// 由view坐标得到最接近字符的序列号
	VOID	CopyToClipboard();					// 将选定内容拷到剪贴板 
	VOID	PasteFromClipboard();				// 从剪贴板拷贝文本到当前位置
	LPCTSTR	GetText(INT nStart, INT& nLen);		// 得到文本框实际内容(去掉了软回车), nLen:[in]需要取出长度，[out]实际取出长度


	//-----------------------------------------------------------------------------
	// 可编辑属性
	//-----------------------------------------------------------------------------
	BOOL		m_bReadOnly;		// 是否只读
	DWORD		m_dwSelectColor;	// 选字背景颜色
	DWORD		m_dwCursorColor;	// 光标颜色
	DWORD		m_dwCursorTimer;	// 光标闪烁间隔
	INT			m_nLineHeight;		// 行高
    INT			m_nMaxTextCount;	// 最多容纳字节数
};



}	// namespace vEngine {

