//-----------------------------------------------------------------------------
//!\file gui_system.h
//!\author Lyp
//!
//!\date 2004-12-08
//! last 2007-04-12
//!
//!\brief 界面系统
//-----------------------------------------------------------------------------
#pragma once

namespace vEngine {
//-----------------------------------------------------------------------------
// GUI界面系统, (默认会创建一个叫做 desktop 的根控件)
//-----------------------------------------------------------------------------
class VENGINE_API GUISystem
{
public:
	BOOL Init(INT nWidth, INT nHeight, IGUIRender*, LPCSTR szVFS=NULL);
	VOID Destroy();
	VOID Update();
	VOID Render();

	// 外部通过此函数向XUI系统传窗口消息，返回是否处理了键盘消息
	BOOL OnWindowsMsg(DWORD dwMsg, DWORD dwParam1, DWORD dwParam2);

	// 从文件直接创建控件集, szNewClass可以转换根控件类型
	GUIWnd* CreateFromFile(LPCSTR szVFS, LPCTSTR szFileName, LPCSTR szNewClass=NULL);	
	// 从文件读取控件集, 但不马上创建
	BOOL LoadXml(XmlElement* pElement, LPCSTR szVFS, LPCTSTR szFileName);
	// 改变XML中某个控件的指定属性
	BOOL ChangeXml(XmlElement* pXmlElement, LPCSTR szControl, LPCSTR szKey, LPCSTR szValue);
	GUIWnd* CreateWnd(LPCTSTR szFatherFullName, XmlElement* pXmlElement);// 创建控件

	// 销毁控件
	BOOL AddToDestroyList(GUIWnd* pWnd); // 外部调用，用来删除控件
	VOID DestroyWnd(GUIWnd* pWnd);

	// 焦点处理
	BOOL SetActive(GUIWnd* pControl);	// 设置焦点控件
	VOID SetModal(GUIWnd* pControl);	// 设置强制焦点控件，设置NULL来取消

	// 事件处理
	BOOL PushEvent(tagGUIEvent* pEvent) { return m_ListEvent.AddMsg(pEvent, sizeof(tagGUIEvent)); }
	BOOL RegisterEventHandler(tstring strWndName, VUIEVENTHANDLE fp); // 注册事件处理函数
	BOOL UnRegisterEventHandler(tstring strWndName, VUIEVENTHANDLE fp); // 注销事件处理函数

	// 读取字体
	BOOL LoadFont(LPCTSTR szFontFile);

	// 各种工具接口
	GUIWnd* GetWnd(DWORD dwID) {return m_mapAllWnd.Peek(dwID);} // 获得控件指针,如果不存在返回GT_INVALID
	GUIWnd* GetWnd(LPCTSTR szFullName) {return m_mapAllWnd.Peek(m_pUtil->Crc32(szFullName));}
	GUIWnd* GetMousePoint() {return m_pPoint;}	// 得到鼠标当前指向的控件
	GUIWnd* GetActive() {return m_pActive;}	// 得到当前激活的控件
	GUIWnd* GetDesktop() {return m_pDesktop;}	// 得到桌面
	IGUIRender*	GetRender() {return m_pRender;}	// 获得渲染接口

	BOOL IsChildOf(GUIWnd* pFather, GUIWnd* pChild); // 检查是否是指定控件的子孙控件
	VOID Record(XmlElement* pXmlElement, GUIWnd* pWnd);	// 记录指定控件及其子控件到XmlElement
	VOID RedrawAllWnd(); // 重画所有控件

	// 注册控件类
	template<typename T>VOID Register(TWrap<T> wrap, LPCTSTR szName, LPCTSTR szDesc);

	// 调试和编辑接口
	std::list<tstring>& EnumControlType() {return m_listWndType;}	// 得到所有控件类型
	std::list<tstring>& EnumControlDesc() {return m_listWndDesc;}	// 得到所有控件类型描述
	INT GetControlNum() { return m_mapAllWnd.Size(); } // 得到控件总数

	GUISystem():m_pDesktop(0),m_pActive(0),m_pModal(0),m_pPoint(0){}
	~GUISystem() {this->Destroy();}

private:
	TObjRef<Util>					m_pUtil;
	TObjRef<VirtualFileSys>			m_pVFS;
	TAutoFactory<GUIWnd>			m_AF;
	IGUIRender*						m_pRender;

	std::list<tstring>				m_listWndType;	// 所有控件类型
	std::list<tstring>				m_listWndDesc;	// 所有控件描述
	TMap<DWORD, GUIWnd*>			m_mapAllWnd;	// 指向所有控件
	TList<GUIWnd*>					m_listDestroy;	// 删除列表
	GUIWnd*							m_pDesktop;		// 根控件(桌面)
	GUIWnd*							m_pActive;		// 当前激活的控件
	GUIWnd*							m_pModal;		// 当前强制激活的控件
	GUIWnd*							m_pPoint;		// 当前指向的控件
	tagGUIInputMsg					m_Msg;


	struct tagEventHandle	// 事件处理结构定义
	{
		tstring						strWnd;
		std::list<VUIEVENTHANDLE>	listFP;
	};
	TMap<DWORD, tagEventHandle*>	m_mapHandle;	// 事件处理者
	MsgQueue						m_ListEvent;	// 事件列表

	VOID RegisterControls();	// 注册所有内部控件
};




//-----------------------------------------------------------------------------
// 注册控件
//-----------------------------------------------------------------------------
template<typename T>VOID GUISystem::Register(TWrap<T> wrap, LPCTSTR szName, LPCTSTR szDesc)
{ 
	if( m_AF.Register(wrap, m_pUtil->Crc32(szName)) )
	{
		m_listWndType.push_back(tstring(szName));
		m_listWndDesc.push_back(tstring(szDesc));
	}
}

}	// namespace vEngine {
