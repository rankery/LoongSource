//-----------------------------------------------------------------------------
//!\file gui_editbox.h
//!\author Lyp
//!
//!\date 2004-12-08
//! last 2007-12-31
//!
//!\brief 界面系统 edit box 控件
//-----------------------------------------------------------------------------
#pragma once


namespace vEngine {
//-----------------------------------------------------------------------------
// 界面系统 edit box 控件
//-----------------------------------------------------------------------------
class VENGINE_API GUIEditBox : public GUIStatic
{
public:
	virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
	virtual VOID Update();
	virtual VOID Render();
	virtual BOOL OnInputMessage(tagGUIInputMsg* pMsg);

	virtual GUIWnd* OnActive();	
	virtual VOID OnDeactive(GUIWnd* pWnd);

	virtual BOOL Resize(tagPoint& ptSize); // 改变控件大小,传入新的大小

	LPCTSTR	GetText() { return m_strText.c_str(); }
	VOID	SetText(LPCTSTR szText);	//直接设置文本框内容
	VOID	Deselect() { m_nSelStart = m_nSelEnd = m_nCursorPos; SetRefresh(); }	// 取消选择

	VOID	SetMaxValue(INT nValue) { m_nMaxNumberValue = nValue; }
	VOID	SetMaxTextCount(INT nValue) { m_nMaxTextCount = nValue; }

	GUIEditBox();

protected:
	bool	m_bShowCursor;					// 当前是否显示光标(光标是闪烁的)
	INT		m_nSelStart;					// 选择开始处
	INT		m_nSelEnd;						// 选择结束处
	INT		m_nCursorPos;					// 光标位置
	INT		m_nDispStart;					// 显示的第一个字符，是实际的第几个字符
	INT		m_nDispEnd;						// 显示的最后一个字符，是实际的第几个字符
	INT		m_nMaxNumberValue;				// 能够输入的最大数值（只针对数值框）
	
	std::vector<INT> m_vecCharSize;			// 每个字的宽度

	VOID	CalCharWidth();					// 如果字符串被修改，需要调用此函数计算每个字符宽度
	VOID	ChangeDisplayStart(INT n);		// 修改从那个字符开始显示(n为相对值，内部会处理双字节)

	VOID	DelSelected();					// 删除选择的字符
	INT		GetCharIndex(INT nViewX);		// 由view坐标得到最接近字符的序列号
	VOID	CopyToClipboard();				// 将选定内容拷到剪贴板 
	VOID	PasteFromClipboard();			// 从剪贴板拷贝文本到当前位置

	//-----------------------------------------------------------------------------
	// 可编辑属性
	//-----------------------------------------------------------------------------
	BOOL		m_bReadOnly;		// 是否只读
	BOOL		m_bPassword;		// 是否密码框
	BOOL		m_bNumerical;		// 是否数字框
	DWORD		m_dwCursorColor;	// 光标颜色
	DWORD		m_dwCursorTimer;	// 光标闪烁间隔
	INT			m_nMaxTextCount;	// 最多容纳字节数
};



}	// namespace vEngine {

