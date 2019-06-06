//-----------------------------------------------------------------------------
//!\file gui_wnd.h
//!\author Lyp
//!
//!\date 2004-12-08
//! last 2004-12-08
//!
//!\brief 界面系统框架
//-----------------------------------------------------------------------------
#pragma once


namespace vEngine {
//-----------------------------------------------------------------------------
// 界面系统框架
//-----------------------------------------------------------------------------
class VENGINE_API GUIWnd
{
public:
	//-----------------------------------------------------------------------------
	// 可继承的基本控件行为
	//-----------------------------------------------------------------------------
	virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
	virtual VOID Destroy();
	virtual VOID Update();
	virtual VOID Render();

	virtual BOOL OnInputMessage(tagGUIInputMsg* pMsg); // 处理消息
	virtual GUIWnd* GetWnd(tagPoint& pt);	 //根据给定点坐标给出最顶端控件(除非不可见)

	virtual VOID SendEvent(tagGUIEvent* pEvent);// 向外发送事件
	virtual VOID OnEvent(tagGUIEvent* pEvent); // 接受事件
	virtual GUIWnd* OnActive();	// 被激活通知
	virtual VOID OnDeactive(GUIWnd* pWnd);		// 取消激活通知,pWnd代表将要激活的控件

	virtual GUIWnd* FlipToTop();		// 将本控件翻动到最前面
	virtual BOOL PointInWnd(tagPoint& pt, bool bCheckChild=true); // 指定点是否在本窗体内

	virtual VOID AddChild(GUIWnd* pChild) {m_listChild.push_back(pChild);}	// 添加子窗口
	virtual VOID DelChild(GUIWnd* pChild) {m_listChild.remove(pChild);} // 删除子窗口
	
	virtual BOOL Move(tagPoint& ptClient); // 移动指定偏移量
	virtual BOOL Resize(tagPoint& ptSize); // 改变控件大小,传入新大小

	virtual VOID ClientToView(tagPoint& ptClient); // 将控件client坐标转化成游戏窗口的逻辑坐标(view)
	virtual VOID RefreshViewCoord(); // 刷新逻辑座标(view)
	virtual BOOL Align();	// 自动对齐,如果发生控件的移动,则返回TRUE

	virtual VOID SetInvisible(BOOL b) { m_bInvisible = b; SetRefresh(b); }
	//-----------------------------------------------------------------------------
	// 基本属性操作
	//-----------------------------------------------------------------------------
	DWORD GetID() { return m_dwID; }
	std::list<GUIWnd*>& GetAllChild() { return m_listChild; }	// 得到所有下一级控件的指针,按照Z order
	const tstring& GetFullName() { return m_strFullName; }	// 得到全名
	const tstring& GetName() { return m_strName; }			// 得到名字
	const tstring& GetClassName() { return m_strClassName; }	// 得到类型名字
	VarMgr* GetVar() { return m_pVarMgr; }				// 得到变量管理器
	GUIWnd* GetFather() { return m_pFather; }			// 得到父控件指针
	GUIWnd* GetChild(LPCTSTR szChildName);				// 得到子控件指针，不需要传全名
	const tagPoint& GetSize() { return m_ptSize; }		// 得到控件大小
	const tagPoint& GetClient() { return m_ptClient; }	// 得到client坐标
	const tagPoint& GetView() { return m_ptView; }		// 得到view坐标
	tagGUIFont* GetFont() { return m_pFont; }
	BOOL IsInvisible() { return m_bInvisible; }
	BOOL IsMovable() { return m_bMovable; }
	BOOL IsResizable() { return m_bResizable; }
	BOOL IsTopMost() { return m_bTopMost; }
	BOOL IsBottomMost() { return m_bBottomMost; }
	VOID SetMovable(BOOL b) { m_bMovable = b; }
	VOID SetResizable(BOOL b) { m_bResizable = b; }
	VOID SetTopMost(BOOL b) { m_bTopMost = b; }
	BOOL IsEditable() { return m_bEditable; }
	VOID SetUnClickable(BOOL b) { m_bUnClickable = b; }

	VOID SetMsgHandle(VUIMSGHANDLE fpHandle){ m_fpMsgHandle = fpHandle;}	// 设置为NULL可以清空

	//-----------------------------------------------------------------------------
	// 提供给外部暂存临时变量,控件销毁后,变量也不存在了
	//-----------------------------------------------------------------------------
	std::vector<DWORD>& GetUserData() {return m_vecUserData;}
	VOID SetUserData(INT nIndex, DWORD dwValue);
	DWORD GetUserData(INT nIndex);

	//-----------------------------------------------------------------------------
	// 画布操作
	//-----------------------------------------------------------------------------
	tagGUIImage* GetCanvas() { return m_pCanvas; }
	VOID SetCanvas(tagGUIImage* pImage) { m_pCanvas = pImage; }
	// 设置需要重绘，bCanvas:重绘整个画布
	VOID SetRefresh(BOOL bCanvas=FALSE) {m_bRefresh = TRUE; m_pCanvas->bRefresh = bCanvas?TRUE:m_pCanvas->bRefresh;}
	BOOL IsRefresh() { return (!m_bInvisible && (m_bRefresh || m_pCanvas->bRefresh));}	// 是否需要重绘
	
	GUIWnd();
	virtual ~GUIWnd();

protected:
	TObjRef<Util>		m_pUtil;
	GUISystem*			m_pSystem;
	VarMgr*				m_pVarMgr;
	IGUIRender*			m_pRender;
	
	DWORD				m_dwID;					// Fullname CRC32 id
	tstring				m_strFullName;			// 控件全名
	GUIWnd*				m_pFather;				// 唯一父控件
	VUIMSGHANDLE		m_fpMsgHandle;			// 消息处理回调函数
	std::list<GUIWnd*>	m_listChild;			// 按照Z序排列的所有一级子控件

	tagPoint			m_ptView;				// 游戏窗口相对逻辑坐标
	tagPoint			m_ptLastMouseClickView;	// 最后一次鼠标点击的位置
	BOOL				m_bDrag;				// 是否处于拖动状态
	BOOL				m_bResize;				// 是否处于改变大小状态
	
	tagGUIImage*		m_pCanvas;				// 画布
	BOOL				m_bRefresh;				// 是否需要重画
	BOOL				m_bEditable;			// 是否是可编辑控件
	tagGUIFont*			m_pFont;				// 字体
	
	BOOL				m_bUnClickable;			// 是否无法操作
	std::vector<DWORD>	m_vecUserData;			// 用户数据




	//-----------------------------------------------------------------------------
	// 可编辑属性
	//-----------------------------------------------------------------------------
	tstring				m_strName;			// 控件名
	tstring				m_strClassName;		// 控件类名
	
	BOOL				m_bInvisible;		// 是否不可见
	BOOL				m_bResizable;		// 是否可改变大小
	BOOL				m_bMovable;			// 是否可移动
	BOOL				m_bMoveFather;		// 是否引导父控件移动
	BOOL				m_bCanMoveOut;		// 是否能移到父控件外
	BOOL				m_bTopMost;			// 是否放在最前
	BOOL				m_bBottomMost;		// 是否放在最底
	
	tagPoint			m_ptClient;			// 逻辑坐标
	tagPoint			m_ptSize;			// 大小

	tstring				m_strAlignWnd;		// 对齐控件名
	EGUIAlign			m_eAlign;			// 对齐方式
	tagPoint			m_ptAlignOffset;	// 对齐偏移

};



}	// namespace vEngine {


