//-----------------------------------------------------------------------------
//!\file gui_virtual.cpp
//!\author Lyp
//!
//!\date 2005-05-26
//! last 2005-05-27
//!
//!\brief 界面系统虚拟控件
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "gui_virtual.h"


namespace vEngine {
//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL GUIVirtual::Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement)
{
	BOOL bResult = GUIWnd::Init(pSystem, pFather, pXmlElement);
	if( FALSE == bResult )
		return FALSE;

	m_ptClient.Clear();
	m_ptSize = pFather->GetSize();

	// 算出实际座标
	m_ptView = m_ptClient;
	ClientToView(m_ptView);

	m_bMovable = false;		// 我们不能移动
	return bResult;
}




//-----------------------------------------------------------------------------
// 指定点是否在我们窗口内
//-----------------------------------------------------------------------------
BOOL GUIVirtual::PointInWnd(tagPoint& pt)
{
	if( m_bInvisible )
		return FALSE;

	// 检查点是否在我们的所有子窗口内
	std::list<GUIWnd*>::iterator it;
	for(it=m_listChild.begin(); it!=m_listChild.end(); ++it)
	{
		if( TRUE == (*it)->PointInWnd(pt) )
			return TRUE;
	}

	return FALSE;
}

}	// namespace vEngine {
