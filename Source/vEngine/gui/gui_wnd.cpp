//-----------------------------------------------------------------------------
//!\file gui_wnd.cpp
//!\author Lyp
//!
//!\date 2004-12-08
//! last 2005-03-31
//!
//!\brief 界面系统框架
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "gui_wnd.h"
#include "gui_event_define.h"


namespace vEngine {
//-----------------------------------------------------------------------------
// constructor / destructor
//-----------------------------------------------------------------------------
GUIWnd::GUIWnd()
{
	m_pVarMgr = new VarMgr;
	m_fpMsgHandle = NULL;
	m_bUnClickable = FALSE;

	// 注册所有可编辑属性
	m_pVarMgr->Register(_T("Name"), &m_strName);
	m_pVarMgr->Register(_T("ClassName"), &m_strClassName);
	m_pVarMgr->Register(_T("Invisible"), &m_bInvisible, _T("no yes"));
	m_pVarMgr->Register(_T("Resizable"), &m_bResizable, _T("no yes"));
	m_pVarMgr->Register(_T("Movable"), &m_bMovable, _T("no yes"));
	m_pVarMgr->Register(_T("MoveFather"), &m_bMoveFather, _T("no yes"));
	m_pVarMgr->Register(_T("CanMoveOutOfFather"), &m_bCanMoveOut, _T("no yes"));
	m_pVarMgr->Register(_T("TopMost"), &m_bTopMost, _T("no yes"));
	m_pVarMgr->Register(_T("BottomMost"), &m_bBottomMost, _T("no yes"));
	m_pVarMgr->Register(_T("LogicPos"), &m_ptClient);
	m_pVarMgr->Register(_T("Size"), &m_ptSize);
	m_pVarMgr->Register(_T("AlignControl"), &m_strAlignWnd);
	m_pVarMgr->Register(_T("AlignType"), (INT*)&m_eAlign, 
		_T("NoAlign Center InnerLeftTop InnerLeft InnerLeftBottom InnerRightTop InnerRight ")
		_T("InnerRightBottom InnerTop InnerBottom OuterLeftTop OuterLeft OuterLeftBottom ")
		_T("OuterRightTop OuterRight OuterRightBottom OuterTopLeft OuterTop OuterTopRight ")
		_T("OuterBottomLeft OuterBottom OuterBottomRight"));
	m_pVarMgr->Register(_T("AlignOffset"), &m_ptAlignOffset);

	// 设置初始值
	m_ptSize.Set(128, 128);
	m_bRefresh = TRUE;
	m_bEditable = FALSE;
	m_pFont = NULL;
}


GUIWnd::~GUIWnd()
{
	SAFE_DEL(m_pVarMgr);
}

//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL GUIWnd::Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement)
{
	m_pSystem = pSystem;
	m_pFather = pFather;
	m_pRender = pSystem->GetRender();

	// 读入属性
	m_pVarMgr->Load(pXmlElement);

	// 得到全名
	m_strFullName = P_VALID(pFather) ? pFather->GetFullName() : _T("");
	m_strFullName.append(_T("\\"));
	m_strFullName.append(m_strName);

	// 确保不重名
	while( P_VALID(m_pSystem->GetWnd(m_strFullName.c_str())) )
	{
		m_strName += _T("0");
		m_strFullName += _T("0");
	}

	m_dwID = m_pUtil->Crc32(m_strFullName.c_str());

	// 算出实际座标
	m_ptView = m_ptClient;
	ClientToView(m_ptView);
	m_bDrag = FALSE;
	m_bResize = FALSE;	// 是否处于改变大小状态

	// 是否需要继承一些属性
	if( pFather )
	{
		m_pCanvas = pFather->GetCanvas();	// 画布
		m_pCanvas->bRefresh = TRUE;
	}

	return TRUE;
}



//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
VOID GUIWnd::Destroy()
{
	// 删除所有子窗口
	std::list<GUIWnd*> temp = m_listChild;
	std::list<GUIWnd*>::iterator it;
	for(it=temp.begin(); it!=temp.end(); ++it)
		m_pSystem->DestroyWnd(*it);

	// 通知父窗口删除自己
	if( m_pFather )
		m_pFather->DelChild(this);

	SetRefresh(TRUE);
}


//-----------------------------------------------------------------------------
// update
//-----------------------------------------------------------------------------
VOID GUIWnd::Update()
{
	std::list<GUIWnd*>::iterator it;
	for(it=m_listChild.begin(); it!=m_listChild.end(); ++it)
		(*it)->Update();
}


//-----------------------------------------------------------------------------
// render
//-----------------------------------------------------------------------------
VOID GUIWnd::Render()
{
	if( m_bInvisible )
		return;

	if( P_VALID(m_pCanvas->dwHandle) )	// 如果有离屏缓冲
		m_bRefresh = FALSE;	// 本次渲染后就不需要再次渲染

	std::list<GUIWnd*>::reverse_iterator it;
	for(it=m_listChild.rbegin(); it!=m_listChild.rend(); ++it)
		(*it)->Render();
}


//-----------------------------------------------------------------------------
// 移动指定偏移量
//-----------------------------------------------------------------------------
BOOL GUIWnd::Move(tagPoint& ptClient)
{
	if( FALSE == P_VALID(m_pFather) )
		return FALSE;

	if( ptClient.IsEmpty() ) 
		return FALSE;

	if( !m_bMovable )
	{
		if( m_bMoveFather )
			return m_pFather->Move(ptClient);
		else
			return FALSE;
	}

	// 参数是偏移量，我们要计算出实际值
	tagPoint ptNewClient = m_ptClient + ptClient;

	// 本控件是否能够移动到父窗口之外的位置
	if( !m_bCanMoveOut )
	{
		// 把控件控制在父窗口内
		const tagPoint& ptFatherSize = m_pFather->GetSize();
		if( ptNewClient.x + m_ptSize.x > ptFatherSize.x )
			ptNewClient.x = ptFatherSize.x - m_ptSize.x;
		if( ptNewClient.x < 0 )
			ptNewClient.x = 0;
		if( ptNewClient.y + m_ptSize.y > ptFatherSize.y )
			ptNewClient.y = ptFatherSize.y - m_ptSize.y;
		if( ptNewClient.y < 0 )
			ptNewClient.y = 0;
	}

	SetRefresh(TRUE);
	
	m_ptClient = ptNewClient;
	this->RefreshViewCoord();

	tagGUIEvent event(this, EGUIE_Drag, NULL);
	SendEvent(&event);
	return TRUE;
}

//-----------------------------------------------------------------------------
// 改变控件大小
//-----------------------------------------------------------------------------
BOOL GUIWnd::Resize(tagPoint& ptSize)
{
	if( FALSE == P_VALID(m_pFather) )	// 桌面不能改动大小
		return FALSE;

	if( !m_bResizable )
		return FALSE;

	m_ptSize = ptSize;

	// 本控件是否能够移动到父窗口之外的位置
	if( !m_bCanMoveOut )
	{
		// 把控件控制在父窗口内
		const tagPoint& ptFatherSize = m_pFather->GetSize();
		if( m_ptSize.x + m_ptClient.x > ptFatherSize.x )
			m_ptSize.x = ptFatherSize.x - m_ptClient.x;
		if( m_ptSize.y + m_ptClient.y > ptFatherSize.y )
			m_ptSize.y = ptFatherSize.y - m_ptClient.y;
	}

	SetRefresh(TRUE);

	tagGUIEvent event(this, EGUIE_Resize, NULL);
	SendEvent(&event);
	return TRUE;
}


//-----------------------------------------------------------------------------
// 将控件client坐标转化成游戏窗口的逻辑坐标(view)
//-----------------------------------------------------------------------------
VOID GUIWnd::RefreshViewCoord()
{
	m_ptView = m_ptClient;
	ClientToView(m_ptView);

	// 更新所有子控件的座标
	std::list<GUIWnd*>::iterator it;
	for(it=m_listChild.begin(); it!=m_listChild.end(); ++it)
		(*it)->RefreshViewCoord();
}



//-----------------------------------------------------------------------------
// 将windows的client坐标转化成我们游戏窗口的逻辑坐标(view)
//-----------------------------------------------------------------------------
VOID GUIWnd::ClientToView(tagPoint& ptClient)
{
	if( FALSE == P_VALID(m_pFather) )
		return;

	ptClient = ptClient + m_pFather->GetClient();
	m_pFather->ClientToView(ptClient);
}



//-----------------------------------------------------------------------------
// 将指定子窗口移动到最前面
//-----------------------------------------------------------------------------
GUIWnd* GUIWnd::FlipToTop()
{
	if( FALSE == P_VALID(m_pFather) )
		return this;

	std::list<GUIWnd*>& list = m_pFather->GetAllChild();
	list.remove(this);

	// 如果我们是top most控件就放到最上
	if( m_bTopMost )
	{
		list.push_front(this);
	}
	else if( m_bBottomMost )
	{
		// 否则放到所有bottom most类型的控件的上面
		std::list<GUIWnd*>::iterator it = list.begin();
		while( it!=list.end() && !(*it)->IsBottomMost() )
			++it;

		list.insert(it, this);
	}
	else
	{
		// 否则放到所有top most类型的控件的下面
		std::list<GUIWnd*>::iterator it = list.begin();
		while( it!=list.end() && (*it)->IsTopMost() )
			++it;

		list.insert(it, this);
	}

	SetRefresh();
	m_pFather->FlipToTop();
	return this;
}



//-----------------------------------------------------------------------------
// 指定点是否在我们窗口内
//-----------------------------------------------------------------------------
BOOL GUIWnd::PointInWnd(tagPoint& pt, bool bCheckChild)
{
	if( m_bInvisible )
		return FALSE;

	tagRect rc;
	rc.SetLTWH(m_ptView, m_ptSize);
	if( !m_bInvisible && rc.IsContain(pt) )
		return TRUE;

	// 再检查点是否在我们的所有子窗口内
	if( bCheckChild )
	{
		std::list<GUIWnd*>::iterator it;
		for(it=m_listChild.begin(); it!=m_listChild.end(); ++it)
		{
			if( TRUE == (*it)->PointInWnd(pt, bCheckChild) )
				return TRUE;
		}
	}

	return FALSE;
}



//-----------------------------------------------------------------------------
// 根据给定点坐标给出最顶端控件(除非不可见)
//-----------------------------------------------------------------------------
GUIWnd* GUIWnd::GetWnd(tagPoint& pt) 
{
	if( m_bInvisible )
		return (GUIWnd*)GT_INVALID;

	if( !PointInWnd(pt) )
		return (GUIWnd*)GT_INVALID;

	if( m_bUnClickable )
		return (GUIWnd*)GT_INVALID;

	GUIWnd *pWnd = this;   //满足基本条件,但未必是顶端窗体

	// 从顶端窗体开始搜索
	std::list<GUIWnd*>::iterator it;
	for(it=m_listChild.begin(); it!=m_listChild.end(); ++it)
	{
		GUIWnd *pTmp = (*it)->GetWnd(pt);
		if( GT_VALID(pTmp) )
		{
			pWnd = pTmp;
			break;
		}
	}
	
	return pWnd;
}


//-----------------------------------------------------------------------------
// handle input message
//-----------------------------------------------------------------------------
BOOL GUIWnd::OnInputMessage(tagGUIInputMsg* pMsg)
{
	switch(pMsg->dwType)
	{
	case WM_LBUTTONDOWN:	// 左键单击激活窗口
		{
			GUIWnd* pWnd = m_pSystem->GetDesktop()->GetWnd(pMsg->pt);
			if( P_VALID(pWnd) && pWnd != this )
			{
				if( m_pSystem->SetActive(pWnd) )
					return pWnd->OnInputMessage(pMsg);
			}
			
			m_ptLastMouseClickView = pMsg->pt;
			if( !m_bDrag )
			{
				m_bDrag = TRUE;
				tagGUIEvent event(this, EGUIE_DragStart, pMsg);
				SendEvent(&event);
			}
		}
		break;

	case WM_RBUTTONDOWN:	// 右键单击激活窗口
		{
			GUIWnd* pWnd = m_pSystem->GetDesktop()->GetWnd(pMsg->pt);
			if( P_VALID(pWnd) && pWnd != this )
				return pWnd->OnInputMessage(pMsg);

			m_pSystem->SetActive(this);
		}
		break;

	case WM_MOUSEMOVE:	// 处理窗口拖动和改变大小
		if( m_bDrag )
		{
			if( m_bResize && m_bResizable )	// 是否可改变大小
			{
				tagPoint pt;
				pt = m_ptSize + pMsg->pt - m_ptLastMouseClickView;
				if( pt.x >= 0 && pt.y >= 0 )
				Resize(pt);
			}
			else
				Move(pMsg->pt - m_ptLastMouseClickView);

			m_ptLastMouseClickView = pMsg->pt;
		}
		else
		{
			// 是否处于Resize区间
			if( m_pSystem->GetMousePoint() == this 
				&& (pMsg->pt.x >= m_ptView.x + m_ptSize.x - 5 
				&& pMsg->pt.x <= m_ptView.x + m_ptSize.x
				&& pMsg->pt.y >= m_ptView.y + m_ptSize.y - 5 
				&& pMsg->pt.y <= m_ptView.y + m_ptSize.y) )
				m_bResize = TRUE;
			else
				m_bResize = FALSE;
		}
		break;

	case WM_LBUTTONUP:	// 处理拖动事件完成
		if( m_bDrag )
		{
			m_bDrag = FALSE;
			tagGUIEvent event(this, EGUIE_DragEnd, pMsg, 1);
			SendEvent(&event);
		}
		break;
	case WM_KEYDOWN:	// 发送虚拟键事件
		{
			tagGUIEvent event(this, EGUIE_KeyDown, pMsg);
			SendEvent(&event);
		}
		break;
	}

	if( m_fpMsgHandle )
		return (*m_fpMsgHandle)(pMsg);
	else
		return FALSE;
}



//-----------------------------------------------------------------------------
// 将事件反馈给回调函数或者向上传递,返回TRUE表示消息被外部回调函数接受
//-----------------------------------------------------------------------------
VOID GUIWnd::SendEvent(tagGUIEvent* pEvent)
{
	pEvent->dwSendWndID = m_dwID;
	m_pSystem->PushEvent(pEvent);
	if( m_pFather )
		m_pFather->SendEvent(pEvent);
}



//-----------------------------------------------------------------------------
// 接受事件
//-----------------------------------------------------------------------------
VOID GUIWnd::OnEvent(tagGUIEvent* pEvent)
{
	std::list<GUIWnd*>::iterator it;
	for(it=m_listChild.begin(); it!=m_listChild.end(); ++it)
		(*it)->OnEvent(pEvent);
}


//-----------------------------------------------------------------------------
// 取消激活通知
//-----------------------------------------------------------------------------
VOID GUIWnd::OnDeactive(GUIWnd* pWnd)
{
	m_bDrag = FALSE;
	tagGUIEvent event(this, EGUIE_Deactive, NULL, (DWORD)pWnd);
	SendEvent(&event);
}		


//-----------------------------------------------------------------------------
// 被激活通知
//-----------------------------------------------------------------------------
GUIWnd* GUIWnd::OnActive()
{ 
	tagGUIEvent event(this, EGUIE_Active, NULL);
	SendEvent(&event);
	return FlipToTop();
}


//-----------------------------------------------------------------------------
// 自动对齐,如果发生控件的移动,则返回TRUE
//-----------------------------------------------------------------------------
BOOL GUIWnd::Align()
{ 
	BOOL bResult = FALSE;

	std::list<GUIWnd*>::iterator it;
	for(it=m_listChild.begin(); it!=m_listChild.end(); ++it)
	{
		if( (*it)->Align() )
			bResult = TRUE;
	}

	if( EGUIA_Null == m_eAlign || !P_VALID(m_pFather) )	// 不自动对齐
		return bResult;

	if(  m_strAlignWnd.empty() && P_VALID(m_pFather) )
		m_strAlignWnd = m_pFather->GetFullName();

	GUIWnd* pWnd = m_pSystem->GetWnd(m_strAlignWnd.c_str());
	if( !P_VALID(pWnd) )
	{
		if( m_strAlignWnd == m_pFather->GetName() )
			pWnd = m_pFather;
		else
			return bResult;
	}

	// 初始化座标
	tagPoint ptClientOrigin = m_ptClient;
	tagPoint ptViewOrigin = m_ptView;

	m_ptView.Clear();
	m_ptClient.Clear();
	ClientToView(m_ptView);

	tagPoint ptSize = pWnd->GetSize();
	tagPoint ptView = pWnd->GetView();
	tagPoint ptOffset = ptView - m_ptView;

	switch(m_eAlign)
	{
	case EGUIA_Center:				// 内部居中
		m_ptClient.x = (ptSize.x - m_ptSize.x) / 2; m_ptClient.y = (ptSize.y - m_ptSize.y) / 2; break;
	case EGUIA_InnerLeftTop:		// 内部左上
		m_ptClient.x = 0; m_ptClient.y = 0;	break;
	case EGUIA_InnerLeft:			// 内部左中
		m_ptClient.x = 0; m_ptClient.y = (ptSize.y - m_ptSize.y) / 2; break;
	case EGUIA_InnerLeftBottom:		// 内部左下
		m_ptClient.x = 0; m_ptClient.y = ptSize.y - m_ptSize.y; break;
	case EGUIA_InnerRightTop:		// 内部右上
		m_ptClient.x = ptSize.x - m_ptSize.x; m_ptClient.y = 0; break;
	case EGUIA_InnerRight:			// 内部右中
		m_ptClient.x = ptSize.x - m_ptSize.x; m_ptClient.y = (ptSize.y - m_ptSize.y) / 2; break;
	case EGUIA_InnerRightBottom:	// 内部右下
		m_ptClient.x = ptSize.x - m_ptSize.x; m_ptClient.y = ptSize.y - m_ptSize.y; break;
	case EGUIA_InnerTop:			// 内部上中
		m_ptClient.x = (ptSize.x - m_ptSize.x) / 2; m_ptClient.y = 0; break;
	case EGUIA_InnerBottom:			// 内部下中
		m_ptClient.x = (ptSize.x - m_ptSize.x) / 2; m_ptClient.y = ptSize.y - m_ptSize.y; break;
	case EGUIA_OuterLeftTop:		// 外部左上
		m_ptClient.x = -m_ptSize.x; m_ptClient.y = 0; break;
	case EGUIA_OuterLeft:			// 外部左中
		m_ptClient.x = -m_ptSize.x; m_ptClient.y = (ptSize.y - m_ptSize.y) / 2; break;
	case EGUIA_OuterLeftBottom:		// 外部左下
		m_ptClient.x = -m_ptSize.x; m_ptClient.y = ptSize.y - m_ptSize.y; break;
	case EGUIA_OuterRightTop:		// 外部右上
		m_ptClient.x = ptSize.x; m_ptClient.y = 0; break;
	case EGUIA_OuterRight:			// 外部右中
		m_ptClient.x = ptSize.x; m_ptClient.y = (ptSize.y - m_ptSize.y) / 2; break;
	case EGUIA_OuterRightBottom:	// 外部右下
		m_ptClient.x = ptSize.x; m_ptClient.y = ptSize.y - m_ptSize.y; break;
	case EGUIA_OuterTopLeft:		// 外部上左
		m_ptClient.x = 0; m_ptClient.y = -m_ptSize.y; break;
	case EGUIA_OuterTop:			// 外部上中
		m_ptClient.x = (ptSize.x - m_ptSize.x) / 2; m_ptClient.y = -m_ptSize.y; break;
	case EGUIA_OuterTopRight:		// 外部上右
		m_ptClient.x = ptSize.x - m_ptSize.x; m_ptClient.y = -m_ptSize.y; break;
	case EGUIA_OuterBottomLeft:		// 外部下左
		m_ptClient.x = 0; m_ptClient.y = ptSize.y; break;
	case EGUIA_OuterBottom:			// 外部下中
		m_ptClient.x = (ptSize.x - m_ptSize.x) / 2; m_ptClient.y = ptSize.y; break;
	case EGUIA_OuterBottomRight:	// 外部下右
		m_ptClient.x = ptSize.x - m_ptSize.x; m_ptClient.y = ptSize.y; break;
	}

	m_ptClient = m_ptClient + ptOffset;
	m_ptClient = m_ptClient + m_ptAlignOffset;
	if( !(ptClientOrigin == m_ptClient) )
	{
		this->RefreshViewCoord();
		this->SetRefresh(TRUE);	// 重画整个画布
		return TRUE;
	}
	else
	{
		m_ptView = ptViewOrigin;
		return bResult;
	}

	
}


//-----------------------------------------------------------------------------
// 得到子控件名，不需要传全名
//-----------------------------------------------------------------------------
GUIWnd* GUIWnd::GetChild(LPCTSTR szChildName)
{
	tstring str = m_strFullName + _T("\\") + szChildName;
	return m_pSystem->GetWnd(str.c_str());
}


//-----------------------------------------------------------------------------
// 设置用户数据
//-----------------------------------------------------------------------------
VOID GUIWnd::SetUserData( INT nIndex, DWORD dwValue )
{
	if( (INT)m_vecUserData.size() < nIndex+1 )
		m_vecUserData.resize(nIndex+1);
	m_vecUserData[nIndex] = dwValue;
}


//-----------------------------------------------------------------------------
// 得到用户数据
//-----------------------------------------------------------------------------
DWORD GUIWnd::GetUserData( INT nIndex )
{
	if( (INT)m_vecUserData.size() < nIndex+1 )
		m_vecUserData.resize(nIndex+1, 0);
	return m_vecUserData[nIndex];
}

}	// namespace vEngine {