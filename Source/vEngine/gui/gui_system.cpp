//-----------------------------------------------------------------------------
//!\file gui_system.cpp
//!\author Lyp
//!
//!\date 2004-12-08
//! last 2007-04-13
//!
//!\brief 界面系统
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "gui_define.h"
#include "gui_system.h"
#include "..\console\console_gui.h"

#include "gui_static.h"
#include "gui_staticex.h"
#include "gui_button.h"
#include "gui_pushbutton.h"
#include "gui_editbox.h"
#include "gui_editboxex.h"
#include "gui_listbox.h"
#include "gui_scrollbar.h"
#include "gui_progress.h"
#include "gui_combobox.h"
#include "gui_patch.h"
#include "gui_richeditbox.h"
#include "gui_framemutex.h"
#include "gui_frame.h"
#include "gui_virtual.h"
#include "gui_tab.h"
#include "gui_tree.h"
#include "gui_window.h"
#include "gui_httppic.h"

#include "..\vfs\vfs.h"
namespace vEngine {
//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL GUISystem::Init(INT nDesktopWidth, INT nDesktopHeight, IGUIRender* pRender,
					 LPCSTR szVFS)
{
	m_pRender = pRender;

	// 创建桌面对象
	m_pPoint = m_pActive = m_pDesktop = new GUIWnd;
	if( NULL == m_pDesktop )
		return FALSE;

	XmlElement ele;
	ele.SetAttribute("Name", "desktop");
	ele.SetAttribute("ClassName", "GUIWnd");
	ele.SetAttribute("Movable", FALSE);
	ele.SetAttribute("Size_x", nDesktopWidth);
	ele.SetAttribute("Size_y", nDesktopHeight);
	if( FALSE == m_pDesktop->Init(this, NULL, &ele) )	// pFather = NULL
	{
		SAFE_DEL(m_pDesktop);
		return FALSE;
	}

		// 初始化画布
	tagGUIImage* pNewTarget = new tagGUIImage;
	pNewTarget->dwHandle = 0;
	pNewTarget->ptSize = m_pDesktop->GetSize();
	m_pDesktop->SetCanvas(pNewTarget);

	// 添加到全局表中
	m_mapAllWnd.Add(m_pUtil->Crc32(_T("\\desktop")), m_pDesktop);	

	m_listWndType.clear();
	m_listWndDesc.clear();
	m_listDestroy.Clear();

	// 注册所有内部控件
	this->RegisterControls();

	if( szVFS )
		m_pVFS = szVFS;

	return TRUE;
}


//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
VOID GUISystem::Destroy()
{
	ASSERT(0 != m_listWndType.size()); // 外部必须先调用此函数，再注销所有控件类型
	if( m_pDesktop )
	{
		m_pDesktop->Destroy();
		tagGUIImage* pTarget = m_pDesktop->GetCanvas();
		SAFE_DEL(pTarget);
	}
	SAFE_DEL(m_pDesktop);

	m_mapHandle.ResetIterator();
	tagEventHandle* pEventHandle = NULL;
	while( m_mapHandle.PeekNext(pEventHandle) )
		SAFE_DEL(pEventHandle);
	m_mapHandle.Clear();
}


//-----------------------------------------------------------------------------
// 创建控件
//-----------------------------------------------------------------------------
VOID GUISystem::Update()
{
	// 处理destroy列表
	if( m_listDestroy.Size() > 0 )
	{
		GUIWnd* pWnd;
		m_listDestroy.ResetIterator();
		while( m_listDestroy.PeekNext(pWnd) )
			this->DestroyWnd(pWnd);	// 真正删除控件

		m_listDestroy.Clear();
	}

	// 事件分派
	DWORD dwSize= 0;
	tagGUIEvent* pEvent = NULL;
	while( pEvent = (tagGUIEvent*)m_ListEvent.GetMsg(dwSize) )
	{
		tagEventHandle* pHandle = m_mapHandle.Peek(pEvent->dwSendWndID);
		if( !P_VALID(pHandle) )
		{
			m_ListEvent.FreeMsg(pEvent);
			continue;
		}

		std::list<VUIEVENTHANDLE>::iterator it;
		for(it=pHandle->listFP.begin(); it!=pHandle->listFP.end(); ++it)
		{
			if( (*it)((DWORD)pEvent) )	// 有一个处理函数返回真就不再调用其它处理函数了
				break;
		}

		m_ListEvent.FreeMsg(pEvent);
	}

	// 所有控件Update
	m_pDesktop->Update();
}


//-----------------------------------------------------------------------------
// 创建控件
//-----------------------------------------------------------------------------
VOID GUISystem::Render()
{ 
	m_pDesktop->Render(); 
}


//-----------------------------------------------------------------------------
// 从文件直接创建控件集, szNewClass可以转换根控件类型
//-----------------------------------------------------------------------------
GUIWnd* GUISystem::CreateFromFile(LPCSTR szVFS, LPCTSTR szFileName, LPCSTR szNewClass)
{
	GUIWnd* pWndRoot = (GUIWnd*)GT_INVALID;

	XmlDocument doc;
	if( !doc.LoadFile(szVFS, szFileName) )
		return pWndRoot;

	XmlHandle docHandle( &doc );
	XmlHandle eleHandle = docHandle.FirstChildElement("desktop").FirstChildElement();
	XmlElement* pElement = eleHandle.Element();
	if( !pElement )	// 是否符合CGUI存盘格式
		return pWndRoot;

	while( pElement )
	{
		if( !P_VALID(pWndRoot) )
		{
			if( P_VALID(szNewClass) )
				pElement->SetAttribute("ClassName", szNewClass);
			pWndRoot = CreateWnd(_T("\\desktop"), pElement);
		}
		else
		{
			CreateWnd(_T("\\desktop"), pElement);
		}

		pElement = pElement->NextSiblingElement();
	}

	return pWndRoot;
}


//-----------------------------------------------------------------------------
// 从文件读取控件集
//-----------------------------------------------------------------------------
BOOL GUISystem::LoadXml(XmlElement* pElement, LPCSTR szVFS, LPCTSTR szFileName)
{
	XmlDocument doc;
	if( !doc.LoadFile(szVFS, szFileName) )
		return FALSE;

	XmlHandle docHandle( &doc );
	XmlHandle eleHandle = docHandle.FirstChildElement("desktop").FirstChildElement();
	if( !eleHandle.Element() )	// 是否符合CGUI存盘格式
		return FALSE;

	*pElement = *eleHandle.Element();
	return TRUE;
}


//-----------------------------------------------------------------------------
// 修改XML数据中的控件属性
//-----------------------------------------------------------------------------
BOOL GUISystem::ChangeXml(XmlElement* pXmlElement, LPCSTR szControl, LPCSTR szKey, LPCSTR szValue)
{
	std::string strName, strFind(szControl);
	BOOL bFinally = (strFind.find_first_of('\\') == std::string::npos);
	strFind.assign(strFind, 0, strFind.find_first_of('\\'));

	while( pXmlElement )
	{
		strName = pXmlElement->Attribute("Name");
		if( strName == strFind )
		{
			if( bFinally )
			{
				pXmlElement->SetAttribute(szKey, szValue);
				return TRUE;
			}
			else
			{
				pXmlElement = pXmlElement->FirstChildElement();
				szControl = strstr(szControl, "\\") + 1;	// 跳过"\"
				return ChangeXml(pXmlElement, szControl, szKey, szValue);
			}
		}
		pXmlElement = pXmlElement->NextSiblingElement();
	}
	return FALSE;
}


//-----------------------------------------------------------------------------
// 创建控件
//-----------------------------------------------------------------------------
GUIWnd* GUISystem::CreateWnd(LPCTSTR szFatherFullName, XmlElement* pXmlElement)
{
	// 寻找父控件
	DWORD dwFatherID = m_pUtil->Crc32(szFatherFullName);
	GUIWnd* pFather = m_mapAllWnd.Peek(dwFatherID);
	if( !P_VALID(pFather) )
	{
		IMSG(_T("could not found father control:%s\r\n"), szFatherFullName);
		pFather = m_pDesktop;	// 找不到父控件,以桌面为父控件
	}

	// 创建对象
	tstring strClassName(m_pUtil->Unicode8ToUnicode(pXmlElement->Attribute("ClassName")));
	DWORD dwClassID = m_pUtil->Crc32(strClassName.c_str());
	GUIWnd* pWnd = m_AF.Create(dwClassID);
	if( !P_VALID(pWnd) )
	{
		IMSG(_T("could not find control class:%s\r\n"), strClassName.c_str());
		DWORD dwClassID = m_pUtil->Crc32(_T("CGUIStatic"));	// 默认创建Static控件
		pWnd = m_AF.Create(dwClassID);
	}

	// 控件初始化
	if( FALSE == pWnd->Init(this, pFather, pXmlElement) )	
		IMSG(_T("could not init control:%s\r\n"), pWnd->GetFullName());
	
	m_mapAllWnd.Add(m_pUtil->Crc32(pWnd->GetFullName().c_str()), pWnd);	// 添加ID
	pFather->AddChild(pWnd); // 通知父亲

	XmlElement* pChild = pXmlElement->FirstChildElement();
	while( pChild )
	{
		this->CreateWnd(pWnd->GetFullName().c_str(), pChild);
		pChild = pChild->NextSiblingElement();
	}

	// 自动对齐,只到返回FALSE,说明所有控件都移到合适位置
	while( pWnd->Align() );

	pWnd->FlipToTop();
	return pWnd;
}


//-----------------------------------------------------------------------------
// 真正删除控件
//-----------------------------------------------------------------------------
VOID GUISystem::DestroyWnd(GUIWnd* pWnd)
{
	// 从全局表中删除
	DWORD dwID = m_pUtil->Crc32(pWnd->GetFullName().c_str());
	m_mapAllWnd.Erase(dwID);

	// 如果是active控件，则重置active控件 
	if( m_pActive == pWnd )	
		m_pActive = m_pDesktop;
	if( m_pPoint == pWnd )
		m_pPoint = m_pDesktop;

	// 调用自删除函数
	pWnd->Destroy();

	// 从内存中删除
	DWORD dwClassID = m_pUtil->Crc32(pWnd->GetClassName().c_str());
	m_AF.Destroy(pWnd, dwClassID);

	// 删除注册的事件处理器
	tagEventHandle* pHandle = m_mapHandle.Peek(dwID);
	if( P_VALID(pHandle) )
	{
		m_mapHandle.Erase(dwID);
		SAFE_DEL(pHandle);
	}
}



//-----------------------------------------------------------------------------
// 删除控件
//-----------------------------------------------------------------------------
BOOL GUISystem::AddToDestroyList(GUIWnd* pWnd)	
{
	DWORD dwID = m_pUtil->Crc32(pWnd->GetFullName().c_str());
	if( !GT_VALID(m_mapAllWnd.Peek(dwID)) )
		return FALSE;	// 不存在指定控件

	m_listDestroy.PushBack(pWnd);
	return TRUE;
}


//-----------------------------------------------------------------------------
// 记录指定控件及其子控件
//-----------------------------------------------------------------------------
VOID GUISystem::Record(XmlElement* pXmlElement, GUIWnd* pWnd)
{
	if( pWnd == m_pDesktop )
	{
		pXmlElement->SetValue("desktop");
	}
	else
	{
		pXmlElement->SetValue("UIControl");
		pWnd->GetVar()->Save(pXmlElement, TRUE);
	}

	std::list<GUIWnd*>& listWnd = pWnd->GetAllChild();
	std::list<GUIWnd*>::iterator it;
	for(it=listWnd.begin(); it!=listWnd.end(); ++it)
	{
		XmlElement* pNewXmlElement = new XmlElement;
		if( P_VALID(pNewXmlElement) )
		{
			Record(pNewXmlElement, (*it));
			pXmlElement->LinkEndChild(pNewXmlElement);
		}
		else
		{
			delete(pNewXmlElement);
		}
	}
}


//-----------------------------------------------------------------------------
// 重画所有控件
//-----------------------------------------------------------------------------
VOID GUISystem::RedrawAllWnd()
{
	GUIWnd* pWnd = NULL;
	m_mapAllWnd.ResetIterator();
	while(m_mapAllWnd.PeekNext(pWnd))
		pWnd->SetRefresh();
}


//-----------------------------------------------------------------------------
// 接收windows消息
//-----------------------------------------------------------------------------
BOOL GUISystem::OnWindowsMsg(DWORD dwMsg, DWORD dw1, DWORD dw2)
{
	BOOL bHandle = FALSE;
	m_Msg.Set(dwMsg, dw1, dw2);

	if( dwMsg >= WM_MOUSEFIRST && dwMsg <= WM_MOUSELAST )
		m_pRender->ClientToView(m_Msg.pt);

	// 先将消息直接传给焦点控件
	bHandle = m_pActive->OnInputMessage(&m_Msg);
	
	if( dwMsg == WM_MOUSEMOVE )	// 鼠标移动消息同时传给鼠标所经过的控件
	{
		GUIWnd* pLastPointWindow = m_pPoint;
		GUIWnd* pWnd = m_pDesktop->GetWnd(m_Msg.pt);
		if( P_VALID(pWnd) && pWnd != m_pPoint )
			m_pPoint = pWnd;	// 改变鼠标指向控件

		if( pLastPointWindow != m_pActive )
			pLastPointWindow->OnInputMessage(&m_Msg);
	}
		
	return bHandle;
}


//-----------------------------------------------------------------------------
//! 设置焦点控件
//-----------------------------------------------------------------------------
BOOL GUISystem::SetActive(GUIWnd* pControl) 
{ 
	if( m_pActive == pControl )	// 当前的控件即是焦点
		return TRUE;

	if( P_VALID(m_pModal) && pControl != m_pModal && !IsChildOf(m_pModal, pControl) )
		return FALSE;

	DWORD dwID = pControl->GetID();
	GUIWnd* pLostActive = m_pActive;
	pLostActive->OnDeactive(pControl);	// 这里可能造成控件重构

	pControl = GetWnd(dwID);
	if( P_VALID(pControl) )
	{
		m_pActive = pControl->OnActive();
		return TRUE;
	}
	else
	{
		m_pActive = m_pDesktop->OnActive();
		return FALSE;
	}
}


//-----------------------------------------------------------------------------
// 设置强制焦点控件，设置NULL来取消
//-----------------------------------------------------------------------------
VOID GUISystem::SetModal(GUIWnd* pControl)
{
	m_pModal = pControl;
}


//-----------------------------------------------------------------------------
// 注册事件处理函数
//-----------------------------------------------------------------------------
BOOL GUISystem::RegisterEventHandler( tstring strWndName, VUIEVENTHANDLE fp )
{
	DWORD dwID = m_pUtil->Crc32(strWndName.c_str());
	tagEventHandle* pHandle = m_mapHandle.Peek(dwID);
	if( !P_VALID(pHandle) )
	{
		pHandle = new tagEventHandle;
		pHandle->strWnd = strWndName;
		m_mapHandle.Add(dwID, pHandle);
	}

	pHandle->listFP.push_back(fp);
	return TRUE;
}


//-----------------------------------------------------------------------------
// 注销事件处理函数
//-----------------------------------------------------------------------------
BOOL GUISystem::UnRegisterEventHandler( tstring strWndName, VUIEVENTHANDLE fp )
{
	DWORD dwID = m_pUtil->Crc32(strWndName.c_str());
	tagEventHandle* pHandle = m_mapHandle.Peek(dwID);
	if( !P_VALID(pHandle) )
		return FALSE;
	
	pHandle->listFP.remove(fp);
	if( pHandle->listFP.empty() )
	{
		m_mapHandle.Erase(dwID);
		SAFE_DEL(pHandle);
	}

	return TRUE;
}


//-----------------------------------------------------------------------------
// 读取字体
//-----------------------------------------------------------------------------
BOOL GUISystem::LoadFont(LPCTSTR szFontFile)
{
	tstring strName = g_strLocalPath + _T("\\") +  szFontFile;
	DWORD dwFileSize = m_pVFS->GetSize(strName.c_str());
	DWORD dwFonts = 1;
	if( !P_VALID(dwFileSize) )
		return FALSE;

	LPBYTE pMem = new BYTE[dwFileSize];
	if( !P_VALID(pMem) )
		return FALSE;

	m_pVFS->LoadFile(pMem, strName.c_str());
	::AddFontMemResourceEx(pMem, dwFileSize, 0, &dwFonts);
	SAFE_DEL_ARRAY(pMem);
	return TRUE;
}



//-----------------------------------------------------------------------------
// 检查是否是指定控件的子孙控件
//-----------------------------------------------------------------------------
BOOL GUISystem::IsChildOf(GUIWnd* pFather, GUIWnd* pChild)
{
	if( pFather == pChild->GetFather() )
		return TRUE;

	pChild = pChild->GetFather();
	if( P_VALID(pChild) )
		return IsChildOf(pFather, pChild);

	return FALSE;
}


//-----------------------------------------------------------------------------
// 注册所有内部控件
//-----------------------------------------------------------------------------
VOID GUISystem::RegisterControls()
{
	Register(TWrap<GUIWnd>(), _T("CGUIWnd"), _T("BaseClass"));
	Register(TWrap<GUIStatic>(), _T("CGUIStatic"), _T("Static"));
	Register(TWrap<GUIStaticEx>(), _T("CGUIStaticEx"), _T("StaticEx"));
	Register(TWrap<GUIButton>(), _T("CGUIButton"), _T("Button"));
	Register(TWrap<GUIPushButton>(), _T("CGUIPushButton"), _T("PushButton"));
	Register(TWrap<GUIEditBox>(), _T("CGUIEditBox"), _T("EditBox"));
	Register(TWrap<GUIEditBoxEx>(), _T("CGUIEditBoxEx"), _T("EditBoxEx"));
	Register(TWrap<GUIRichEditBox>(), _T("CGUIRichEditBox"), _T("RichEditBox"));
	Register(TWrap<GUIListBox>(), _T("CGUIListBox"), _T("ListBox"));
	Register(TWrap<GUIScrollBar>(), _T("CGUIScrollBar"), _T("ScrollBar"));
	Register(TWrap<GUIProgress>(), _T("CGUIProgress"), _T("ProgressBar"));
	Register(TWrap<GUIComboBox>(), _T("CGUIComboBox"), _T("ComboBox"));
	Register(TWrap<GUIPatch>(), _T("CGUIPatch"), _T("PatchWindow"));
	Register(TWrap<GUIFrame>(), _T("CGUIFrame"), _T("FrameBox"));
	Register(TWrap<GUIFrameMutex>(), _T("CGUIFrameMutex"), _T("FrameMutex"));
	Register(TWrap<GUIVirtual>(), _T("CGUIVirtual"), _T("VirtualFrame"));
	Register(TWrap<GUITab>(), _T("CGUITab"), _T("Tab"));
	Register(TWrap<GUITree>(), _T("CGUITree"), _T("Tree"));
	Register(TWrap<GUIWindow>(), _T("CGUIWindow"), _T("WindowWrap"));
	Register(TWrap<GUIHttpPic>(), _T("CGUIHttpPic"), _T("HttpPicture"));
}



}	// namespace vEngine {

