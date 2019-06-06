//-----------------------------------------------------------------------------
//!\file gui_tree.h
//!\author Lyp
//!
//!\date 2007-05-17
//! last 2005-05-17
//!
//!\brief 界面系统 tree 控件
//-----------------------------------------------------------------------------
#pragma once


namespace vEngine {
//-----------------------------------------------------------------------------
// 界面系统tree控件item元素定义
//-----------------------------------------------------------------------------
struct VENGINE_API tagGUITreeItem
{
	tstring						strShow;			// 显示文本
	tstring						strText;			// 基本文本
	tstring						strName;			// 全名
	DWORD						dwColor;			// 文本颜色
	std::list<tagGUITreeItem*>	listChildren;		// 子节点列表
	tagGUITreeItem*				pFather;			// 父节点
	INT							nLayer;				// 层数
	tagRect						rcPlus;				// plus显示的位置
	tagRect						rcText;				// text显示的位置
	INT							nTextWidth;			// 字符串显示长度
	bool						bExpended;			// 是否展开

	tagGUITreeItem() 
	{ pFather = 0; nLayer = 0; nTextWidth = 0; bExpended = true; }
};

//-----------------------------------------------------------------------------
// 界面系统tree控件
//-----------------------------------------------------------------------------
class VENGINE_API GUITree : public GUIStatic
{
public:
	virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
	virtual BOOL OnInputMessage(tagGUIInputMsg* pMsg);
	virtual VOID Destroy();
	virtual VOID Render();
	virtual VOID SendEvent(tagGUIEvent* pEvent);

	// 插入项目(如果szFaterName==NULL则表示添加根节点)
	// 注意！任何节点下不能有同名子节点
	// 返回节点ID, GT_INVALID代表插入失败
	DWORD InsterItem(LPCTSTR szFatherName, LPCTSTR szText, LPCTSTR szShow);

	// 删除项目
	BOOL RemoveItem(LPCTSTR szFullName);

	// 设置当前显示的第一行是实际的哪一行
	BOOL SetCurrentTextRow(INT nY, BOOL bSendEvent=TRUE);

	// 设置当前选中的item,并发送ItemClick事件
	BOOL SetCurrentSelItem(DWORD dwID, BOOL bSendEvent=TRUE);
	// 得到当前选中节点
	DWORD GetCurrentSelItem();

	// 得到指定节点
	tagGUITreeItem* GetItem(DWORD dwID) {return m_mapItem.Peek(dwID);}


	GUITree();

protected:

	// 渲染辅助函数
	VOID Render(tagGUITreeItem* pItem, INT& nCurRow, INT& nCurY);
	// 计算一个节点及其子节点所占行
	INT CalItemTakeupRow(tagGUITreeItem* pItem);
	// 计算一个节点起始行(root算第0行)
	INT CalItemStartRow(tagGUITreeItem* pItem);
	BOOL CalItemStartRowHelp(tagGUITreeItem* pItem, tagGUITreeItem* pTargetItem, INT& nRow); // 辅助

	// 移除一个指定节点
	BOOL RemoveItem(tagGUITreeItem* pItem);

	TObjRef<Util>					m_pUtil;
	TMap<DWORD, tagGUITreeItem*>	m_mapItem;
	TList<tagGUITreeItem*>			m_listVisibleItem;
	
	tagGUITreeItem*		m_pRootItem;			// 根节点
	tagGUITreeItem*		m_pCurSelItem;			// 当前选中的节点
	INT					m_nCurrentTextRow;		// 当前显示的第一行是实际的多少行
	INT					m_nTotalRow;			// 总行数
	
	tagGUIImage*		m_pPlusPic;				// 加号图片
	tagGUIImage*		m_pMinusPic;			// 减号图片
	tagGUIImage*		m_pBagPic;				// 包图片


	//-----------------------------------------------------------------------------
	// 可编辑属性
	//-----------------------------------------------------------------------------
	INT				m_nRowHeight;				// 行高
	tstring		m_strSelectStickPic;			// 选择棒图片文件名
	tagRect		m_rcSelectStickPic;				// 选择棒图片使用区域(图片坐标)
	DWORD			m_dwSelectStickColor;		// 选择棒颜色
	DWORD			m_dwPicInterval;			// 图片间隔
	DWORD			m_dwPicTextInterval;		// 图片文字间隔
	tstring			m_strPlusPic;				// 加号图片文件名
	tagRect			m_rcPlusPic;				// 加号图片使用区域(图片坐标)
	DWORD			m_dwPlusPicColor;			// 加号图片绘制颜色
	tstring			m_strMinusPic;				// 减号图片文件名
	tagRect			m_rcMinusPic;				// 减号图片使用区域(图片坐标)
	DWORD			m_dwMinusPicColor;			// 减号图片绘制颜色
	tstring			m_strBagPic;				// 包图片文件名
	tagRect			m_rcBagPic;					// 包图片使用区域(图片坐标)
	DWORD			m_dwBagPicColor;			// 包图片绘制颜色
};


}	// namespace vEngine {
