#pragma once
#define GUI_DEFAULT_ROW_SPACING 20				
enum REBForChatShow
{
	EXUIE_MouseDoubleClick = EGUIE_User + 1030,		//发送鼠标双击richeditbox事件，用于获取关键字
	EXUIE_MouseRClick,
};
class RichEditBoxForChatShow :
	public GUIRichEditBox
{
public:
	RichEditBoxForChatShow(void);
	~RichEditBoxForChatShow(void);
	
	virtual BOOL OnInputMessage(tagGUIInputMsg* pMsg);

	VOID BeNoFlex()						{ m_bFlex = FALSE; }
	VOID BeFlex()						{ m_bFlex = TRUE; }
	BOOL IsFlex()						{ return m_bFlex; }
	const INT& GetTextSize()			{return m_nTextNormalHeight;}			//得到字的高度
	const INT& GetRowSpacing()			{return m_nRowSpacing;}					//得到每行的行高

	void ZoomWnd(INT nOffset);

protected:
	BOOL							m_bFlex;		//是否可拉伸
	BYTE							m_byFlexStatus;	//窗口拉伸状态,-1表示无,0表示鼠标处于上方边缘,1表示处于下方边缘

	void AdptiveCursor(bool bDrag, BOOL bLink);
};
