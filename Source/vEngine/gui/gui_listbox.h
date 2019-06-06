//-----------------------------------------------------------------------------
//!\file gui_listbox.h
//!\author Lyp
//!
//!\date 2004-12-08
//! last 2007-04-23
//!
//!\brief 界面系统 list box 控件
//-----------------------------------------------------------------------------
#pragma once


namespace vEngine {
//-----------------------------------------------------------------------------
// 界面系统list控件item元素定义
//-----------------------------------------------------------------------------
struct VENGINE_API tagGUIListItem
{
	tstring					strText;		// 文本
	DWORD					dwColor;		// 文本颜色
	std::list<tstring>		listOptionText;	// 备选文本
	tagGUIImage*			pPic;			// 图片
	DWORD					dwPicColor;		// 图片颜色
	BOOL					bEditable;		// 是否可编辑
	
	tagGUIListItem() 
	{ pPic = NULL; bEditable = false; }
};


//-----------------------------------------------------------------------------
// 界面系统 list box 控件
//-----------------------------------------------------------------------------
class VENGINE_API GUIListBox : public GUIStatic
{
public:
	virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
	virtual VOID Destroy();
	virtual VOID Render();
	virtual BOOL OnInputMessage(tagGUIInputMsg* pMsg);
	virtual VOID SendEvent(tagGUIEvent* pEvent);
	virtual VOID AddChild(GUIWnd* pChild);	// 添加子窗口
	virtual VOID DelChild(GUIWnd* pChild);

	// 设置列数和默认列宽
	VOID	SetColNum(INT nColNum, INT nDefaultColWidth);
	// 设置指定列的宽度,找不到指定列返回FALSE
	BOOL	SetColWidth(INT nColIndex, INT nColWidth);

	// 如果szText == NULL,则清空单元格
	VOID	SetText(INT nRow, INT nCol, LPCTSTR szText, DWORD dwColor=GT_INVALID);
	// 得到指定单元格的字符串内容,指定单元格无内容,返回GT_INVALID,
	LPCTSTR	GetText(INT nRow, INT nCol);
	// 设置单元格信息,
	VOID SetItemPic(INT nRow, INT nCol, LPCTSTR szPic, const tagRect& rc, DWORD dwColor=0xffffffff);
	VOID SetItemEditable(INT nRow, INT nCol, BOOL bEditable);
	VOID SetItemOptionText(INT nRow, INT nCol, std::list<tstring>& listOptionText);

	// 添加空白行
	VOID InsertRow(INT nRow);
	// 删除指定行
	VOID DelRow(INT nRow);
	// 删除所有内容
	VOID Clear();

	// 得到当前总行数
	INT	GetTotalRowNum() { return m_nTotalTextRow; }
	// 得到当前显示的第一行是实际的哪一行
	BOOL GetCurrentTextRow() { return m_nCurrentTextRow; }
	// 设置当前显示的第一行是实际的哪一行
	BOOL SetCurrentTextRow(INT nY, BOOL bSendEvent=TRUE);
	// 得到当前选中行
	INT	 GetCurSelectedRow() { return m_nCurSelectedRow; }
	// 设置当前选中行
	VOID SetCurSelectedRow(INT n) { m_nCurSelectedRow = n; }
	// 得到行高
	INT GetRowHeight() { return m_nRowHeight; }

	GUIListBox();

protected:

	// 激活内部的编辑器
	VOID ActiveEditor(INT nRow, INT nCol, tagPoint& ptPos,tagPoint& ptSize);
	DWORD SwapColor(DWORD dwColor32)
	{ return (dwColor32 & 0x00ff00)|((dwColor32&0xff0000)>>16)|((dwColor32 & 0xff)<<16); }


	TObjRef<VarContainer>	m_pGlobeVar;

	TMap<DWORD, tagGUIListItem*>  m_mapItem;	// 所有文本
	TMap<DWORD, INT> m_mapWidth;				// 每列的宽度
	
	GUIEditBox*		m_pEditBox;					// 编辑框
	GUIComboBox*	m_pComboBox;				// 组合框
	tagGUIImage*	m_pSelectStickPic;			// 选择棒图片

	INT m_nCurrentTextRow;						// 当前显示的第一行是实际的文本的多少行
	INT m_nTotalRow;							// 总共现在有多少行(包括最后的空白行)
	INT m_nTotalTextRow;						// 实际上具有内容的行数
	INT	m_nCurSelectedSegment;					// 当前鼠标选中的竖分割线
	INT	m_nCurSelectedRow;						// 当前选择的行
	INT	m_nCurEditRow;							// 当前编辑的行
	INT	m_nCurEditCol;							// 当前编辑的列
	INT	m_nCurSelectedRowWhenMouseDown;			// 当鼠标左键按下的时候选择的行

	//-----------------------------------------------------------------------------
	// 可编辑属性
	//-----------------------------------------------------------------------------
	BOOL		m_bCanMoveCol;					// 列分隔线是否可移动
	BOOL		m_bMoveSelect;					// 是否移动鼠标就能选择
	BOOL		m_bOpenFileDialog;				// 是否自动打开文件对话框
	BOOL		m_bOpenColorDialog;				// 是否自动打开色彩对话框
	INT			m_nRowHeight;					// 行高

	tstring		m_strSelectStickPic;			// 选择棒图片文件名
	tagRect		m_rcSelectStickPic;				// 选择棒图片使用区域(图片坐标)
	DWORD		m_dwSelectStickColor;			// 选择棒颜色
	DWORD		m_dwSelectStickActiveColor;		// 选择棒激活颜色
	DWORD		m_dwRowLineColor;				// 分隔线颜色
	DWORD		m_dwSelectTextColor;			// 选中的文本颜色
	DWORD		m_dwSelectShadeColor;			// 选中的文本阴影颜色
	tagPoint	m_ptPicOffset;					// 栏目图片偏移
	tagPoint	m_ptPicSize;					// 栏目图片显示大小
};


}	// namespace vEngine {
