//-----------------------------------------------------------------------------
//!\file gui_staticex.cpp
//!\author Lyp
//!
//!\date 2004-12-08
//! last 2008-06-17
//!
//!\brief 界面系统static ex控件
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "gui_static.h"
#include "gui_staticex.h"
#include "gui_event_define.h"

#include "..\console\console_gui.h"
namespace vEngine {
//-----------------------------------------------------------------------------
// constructor / destructor
//-----------------------------------------------------------------------------
GUIStaticEx::GUIStaticEx()
{
	// 注册所有可编辑属性
	m_pVarMgr->Register(_T("RowHeight"), &m_nRowHeight);

	// 设置初始值
	m_nRowHeight = 2;
}

//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL GUIStaticEx::Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement)
{
	GUIStatic::Init(pSystem, pFather, pXmlElement);

	m_nTotalHeight = 0;
	this->ParseText();
	return TRUE;
}


//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
VOID GUIStaticEx::Destroy()
{
	ClearResource();
	GUIStatic::Destroy();
}


//-----------------------------------------------------------------------------
// render
//-----------------------------------------------------------------------------
VOID GUIStaticEx::Render()
{
	if( !IsRefresh() )
		return GUIWnd::Render();

	// 渲染底图
	m_pCanvas->rc.SetLTWH(m_ptView, m_ptSize);
	m_pRender->Draw(m_pCanvas, m_pPic, m_dwPicColor, m_eAlphaWrite);

	// 渲染文字
	std::list<tagStaticExItem*>::iterator it;
	for(it=m_itCurrent; it!=m_listItems.end(); ++it)
	{
		tagStaticExItem* pItem = (*it);
		if( pItem->strText.size() > 0 )
		{
			m_pCanvas->rc = m_rcText + m_ptView;
			m_pCanvas->rc.left = m_ptView.x + pItem->ptPos.x;
			m_pCanvas->rc.top = m_ptView.y + pItem->ptPos.y;
			m_pRender->Text(m_pCanvas, pItem->strText, pItem->pFont, pItem->dwColor, m_dwShadeColor, m_eTextAlign, m_bTextWriteAlpha);
		}

		if( P_VALID(pItem->dwLinkID) )
		{
			m_pCanvas->rc = m_rcText + m_ptView;
			m_pCanvas->rc.left = m_ptView.x + pItem->ptPos.x;
			m_pCanvas->rc.top = m_ptView.y + pItem->ptPos.y + pItem->ptSize.y - 1;
			m_pCanvas->rc.right = m_ptView.x + pItem->ptPos.x + pItem->ptSize.x;
			m_pCanvas->rc.bottom = m_pCanvas->rc.top;
			m_pRender->Line(m_pCanvas, pItem->dwLinkColor);
		}

		if( P_VALID(pItem->pImage) )
		{
			m_pCanvas->rc.SetLTWH(m_ptView + pItem->ptPos, pItem->pImage->ptSize);
			m_pRender->Draw(m_pCanvas, pItem->pImage, pItem->dwColor, m_eAlphaWrite);
		}
	}

	// 调用基类渲染
	GUIWnd::Render();
}


//-----------------------------------------------------------------------------
// 消息处理
//-----------------------------------------------------------------------------
BOOL GUIStaticEx::OnInputMessage(tagGUIInputMsg* pMsg)
{
	switch( pMsg->dwType )
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		if( m_pSystem->GetMousePoint() == this )
		{
			std::list<tagStaticExItem*>::iterator it;
			for(it=m_itCurrent; it!= m_listItems.end(); ++it)
			{
				tagStaticExItem* pItem = *it;
				if( P_VALID(pItem->dwLinkID) )
				{
					tagRect rc(pItem->ptPos, pItem->ptSize);
					rc = rc + m_ptView;
					if( rc.IsContain(pMsg->pt) )
					{
						tagGUIEvent event(this, EGUIE_StaticExLinkClick, pMsg, pItem->dwLinkID, (DWORD)pItem->strText.c_str());
						SendEvent(&event);
					}
				}
			}
		}
		break;

	case WM_MOUSEMOVE:
		if( m_pSystem->GetMousePoint() == this )
		{ 
			std::list<tagStaticExItem*>::iterator it;
			for(it=m_itCurrent; it!= m_listItems.end(); ++it)
			{
				tagStaticExItem* pItem = *it;
				if( P_VALID(pItem->dwLinkID) )
				{
					tagRect rc(pItem->ptPos, pItem->ptSize);
					rc = rc + m_ptView;
					if( rc.IsContain(pMsg->pt) )
					{
						// 改变鼠标光标
					}
				}
			}
		}
		break;

	case WM_MOUSEWHEEL:	// 处理鼠标滚轮
		if( P_VALID(m_pFather) )
			m_pFather->OnInputMessage(pMsg);
		break;
	case WM_KEYDOWN:
		if( P_VALID(m_pFather) && (pMsg->dwParam1==VK_UP||pMsg->dwParam1==VK_DOWN) )
			m_pFather->OnInputMessage(pMsg);
		break;
	}

	return GUIStatic::OnInputMessage(pMsg);
}


//-----------------------------------------------------------------------------
// 把字符串解析成items
//-----------------------------------------------------------------------------
VOID GUIStaticEx::ParseText()
{
	this->ClearResource();
	m_itCurrent = m_listItems.end();
	m_nTotalHeight = 0;
	if( m_strText.empty() )
		return;

	tagStaticExItem* pItem = new tagStaticExItem;
	pItem->dwColor = m_dwTextColor;
	pItem->pFont = m_pFont;
	pItem->dwLinkID = GT_INVALID;
	pItem->dwLinkColor = 0;
	pItem->pImage = NULL;
	pItem->ptPos.x = m_rcText.left;
	pItem->ptPos.y = m_rcText.top;
	m_listItems.push_back(pItem);
	m_itCurrent = m_listItems.begin();

	UINT nIndex = 0;
	INT nLargeHeight = 0;
	while( nIndex < m_strText.size() )
	{
		if( m_strText[nIndex] == _T('<') )
		{
			INT n =  m_strText.find_first_of(_T('>'), nIndex);
			if( n == tstring::npos || n - nIndex <= 1 )
				break;	// 语法错误
			
			tstring str = m_strText.substr(nIndex+1, n-nIndex-1);
			n =  str.find_first_of(_T('='));
			if( n == tstring::npos || n <= 0 || n>= (INT)str.size() )
				break;	// 语法错误

			tstring strName = str.substr(0, n);
			tstring strValue = str.substr(n+1, str.size() - n);

			tagStaticExItem* pNewItem = new tagStaticExItem;
			pNewItem->dwColor = pItem->dwColor;
			pNewItem->pFont = pItem->pFont;
			pNewItem->dwLinkID = pItem->dwLinkID;
			pNewItem->dwLinkColor = pItem->dwLinkColor;
			pNewItem->pImage = NULL;
			pNewItem->ptPos.x = pItem->ptPos.x + pItem->ptSize.x;
			pNewItem->ptPos.y = pItem->ptPos.y;
			pNewItem->ptSize.y = pItem->ptSize.y;

			if( !ParseKey(strName, strValue, pNewItem) )
			{
				delete pNewItem;
			}
			else
			{
				m_listItems.push_back(pNewItem);
				pItem = pNewItem;
				if( P_VALID(pNewItem->pImage) )
				{
					tagStaticExItem* pNewItem = new tagStaticExItem;
					pNewItem->dwColor = pItem->dwColor;
					pNewItem->pFont = pItem->pFont;
					pNewItem->dwLinkID = pItem->dwLinkID;
					pNewItem->dwLinkColor = pItem->dwLinkColor;
					pNewItem->pImage = NULL;
					pNewItem->ptPos.x = pItem->ptPos.x + pItem->ptSize.x;
					pNewItem->ptPos.y = pItem->ptPos.y;
					pNewItem->ptSize.y = pItem->ptSize.y;
					m_listItems.push_back(pNewItem);
					pItem = pNewItem;

				}
			}
			nIndex += str.size() + 2;
		}
		else if( m_strText[nIndex] == _T('\\') )
		{
			if( m_strText[nIndex+1] == _T('n') )
			{
				tagStaticExItem* pNewItem = new tagStaticExItem;
				pNewItem->dwColor = pItem->dwColor;
				pNewItem->pFont = pItem->pFont;
				pNewItem->dwLinkID = pItem->dwLinkID;
				pNewItem->dwLinkColor = pItem->dwLinkColor;
				pNewItem->pImage = NULL;
				pNewItem->ptPos.x = m_rcText.left;
				pNewItem->ptPos.y = pItem->ptPos.y + pItem->ptSize.y + m_nRowHeight;
				m_listItems.push_back(pNewItem);
				pItem = pNewItem;
				nIndex+=2;
			}
		}
		else
		{
			tagPoint ptSize;
			tstring strText;
			strText += m_strText[nIndex];
			m_pRender->GetTextSize(strText.c_str(), pItem->pFont, ptSize);
			if( pItem->ptPos.x + pItem->ptSize.x + ptSize.x > m_rcText.right 
				||  m_strText[nIndex] == _T('\n') )
			{
				tagStaticExItem* pNewItem = new tagStaticExItem;
				pNewItem->dwColor = pItem->dwColor;
				pNewItem->pFont = pItem->pFont;
				pNewItem->dwLinkID = pItem->dwLinkID;
				pNewItem->dwLinkColor = pItem->dwLinkColor;
				pNewItem->pImage = NULL;
				pNewItem->ptPos.x = m_rcText.left;
				pNewItem->ptPos.y = pItem->ptPos.y + pItem->ptSize.y + m_nRowHeight;
				pNewItem->ptSize = ptSize;
				m_listItems.push_back(pNewItem);
				pItem = pNewItem;

			}
			else
			{
				pItem->ptSize.x += ptSize.x;
				if( ptSize.y > pItem->ptSize.y )
					pItem->ptSize.y = ptSize.y;
			}

			if( m_strText[nIndex] != _T('\n') )
			{
				pItem->strText += m_strText[nIndex];
			}
			nIndex++;
		}
	}

	// 计算总体高度
	std::list<tagStaticExItem*>::iterator it;
	for(it=m_listItems.begin(); it!= m_listItems.end(); ++it)
	{
		tagStaticExItem* pItem = *it;
		INT nH = pItem->ptPos.y + pItem->ptSize.y;
		if( nH > m_nTotalHeight )
				m_nTotalHeight = nH;
	}
}


//-----------------------------------------------------------------------------
// 把字符串解析成关键字
//-----------------------------------------------------------------------------
BOOL GUIStaticEx::ParseKey(tstring& strKey, tstring& strValue, tagStaticExItem* pItem)
{
	if( strKey == _T("font") )	// <font=type,width,height,weight>:设置字体，例如<font=Arial,0,16,400>
	{
		std::vector<tstring> vec;
		m_pUtil->StringToToken(vec, strValue.c_str(), _T(','));
		if( vec.size() != 4 )
			return FALSE;
		
		INT nWidth = _tcstoul(vec[1].c_str(), NULL, 10);
		INT nHeight = _tcstoul(vec[2].c_str(), NULL, 10);
		INT nWeight = _tcstoul(vec[3].c_str(), NULL, 10);
		tagGUIFont* pFont = m_pRender->CreateFont(vec[0].c_str(), nWidth, nHeight, nWeight);
		if( !P_VALID(pFont) )
			return FALSE;

		m_listFont.PushBack(pFont);
		pItem->pFont = pFont;
	}
	else if( strKey == _T("color") )	// <color=0xFFFFFFFF>:设置文字或者图片颜色
	{
		pItem->dwColor = _tcstoul(strValue.c_str(), NULL, 16);
	}
	else if( strKey == _T("pic") )	// <pic=filename,left,top,right,bottom>:图片文件名，使用图片的哪个区域来显示
	{
		std::vector<tstring> vec;
		m_pUtil->StringToToken(vec, strValue.c_str(), _T(','));
		if( vec.size() != 5 )
			return FALSE;
		INT nLeft = _tcstoul(vec[1].c_str(), NULL, 10);
		INT nTop = _tcstoul(vec[2].c_str(), NULL, 10);
		INT nRight = _tcstoul(vec[3].c_str(), NULL, 10);
		INT nBottom = _tcstoul(vec[4].c_str(), NULL, 10);
		tagRect rc(nLeft, nTop, nRight, nBottom);

		tagGUIImage* pImage = m_pRender->CreateImage(vec[0], rc);
		if( !P_VALID(pImage) )
			return FALSE;

		m_listImage.PushBack(pImage);
		pItem->pImage = pImage;
		pItem->ptSize = pImage->ptSize;
	}
	else if( strKey == _T("link") )	// <link=id,LineColor>:显示一个超链接，用户点击后会发送带id信息的事件
	{
		std::vector<tstring> vec;
		m_pUtil->StringToToken(vec, strValue.c_str(), _T(','));
		if( vec.size() != 2 )
			return FALSE;

		pItem->dwLinkID = _tcstoul(vec[0].c_str(), NULL, 16);
		pItem->dwLinkColor = _tcstoul(vec[1].c_str(), NULL, 16);
	}


	return TRUE;
}


//-----------------------------------------------------------------------------
// 改变控件大小时，对内容进行重新排版
//-----------------------------------------------------------------------------
BOOL GUIStaticEx::Resize(tagPoint& ptSize)
{
	BOOL bResult = GUIStatic::Resize(ptSize);

	this->ParseText();
	return bResult;
}



//-----------------------------------------------------------------------------
// 清空所有资源
//-----------------------------------------------------------------------------
VOID GUIStaticEx::ClearResource()
{
	// 先清空item表
	std::list<tagStaticExItem*>::iterator it;
	for(it=m_listItems.begin(); it!= m_listItems.end(); ++it)
	{
		tagStaticExItem* pItem = *it;
		SAFE_DEL(pItem);
	}
	m_listItems.clear();

	// 清空font表
	tagGUIFont* pFont = NULL;
	m_listFont.ResetIterator();
	while( m_listFont.PeekNext(pFont) )
		m_pRender->DestroyFont(pFont);
	m_listFont.Clear();

	// 清空Image表
	tagGUIImage* pImage = NULL;
	m_listImage.ResetIterator();
	while( m_listImage.PeekNext(pImage) )
		m_pRender->DestroyImage(pImage);
	m_listImage.Clear();
}


}	// namespace vEngine {
