//-----------------------------------------------------------------------------
//!\file gui_editbox.cpp
//!\author Lyp
//!
//!\date 2004-12-08
//! last 2007-12-31
//!
//!\brief 界面系统 edit box 控件
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "gui_static.h"
#include "gui_editbox.h"
#include "gui_event_define.h"
#include "..\resource\resource.h"

namespace vEngine {
//-----------------------------------------------------------------------------
// construction
//-----------------------------------------------------------------------------
GUIEditBox::GUIEditBox()
{
	m_pVarMgr->Register(_T("ReadOnly"), &m_bReadOnly, _T("no yes"));
	m_pVarMgr->Register(_T("Password"), &m_bPassword, _T("no yes"));
	m_pVarMgr->Register(_T("Numerical"), &m_bNumerical, _T("no yes"));
	m_pVarMgr->Register(_T("CursorColor"), &m_dwCursorColor);
	m_pVarMgr->Register(_T("CursorTimer"), &m_dwCursorTimer);
	m_pVarMgr->Register(_T("MaxTextCount"), &m_nMaxTextCount);

	m_dwTextColor = 0xFF000000;
	m_dwCursorColor = 0xFF000000;
	m_dwCursorTimer = 200;
	m_nMaxTextCount = 65535;
	m_nMaxNumberValue = 2147483647;
	m_bEditable = TRUE;
}


//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL GUIEditBox::Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement)
{
	BOOL bResult = GUIStatic::Init(pSystem, pFather, pXmlElement);

	m_bShowCursor = false;		// 不显示光标
	m_nSelStart = 0;			// 选择开始处
	m_nSelEnd = 0;				// 选择结束处
	m_nCursorPos = 0;			// 光标位置
	m_nDispStart = 0;			// 显示的第一个字符，是实际的第几个字符
	m_nDispEnd = 0;				// 显示的最后一个字符，是实际的第几个字符

	this->CalCharWidth();
	return bResult;
}


//-----------------------------------------------------------------------------
// update
//-----------------------------------------------------------------------------
VOID GUIEditBox::Update()
{
	static DWORD dwLastTime = timeGetTime();
	// 处理光标的显示与闪烁
	if( m_pSystem->GetActive() == this )
	{
		if( timeGetTime() - dwLastTime > m_dwCursorTimer )
		{
			m_bShowCursor = !m_bShowCursor;
			dwLastTime = timeGetTime();
			this->SetRefresh();
		}
	}

	GUIStatic::Update();
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
VOID GUIEditBox::Render()
{
	if( !IsRefresh() )
		return GUIWnd::Render();

	// 渲染底图
	m_pCanvas->rc.SetLTWH(m_ptView, m_ptSize);
	m_pRender->Draw(m_pCanvas, m_pPic, m_dwPicColor, m_eAlphaWrite);

	// 渲染基本字符字
	tstring strStar;
	m_pCanvas->rc = m_rcText + m_ptView;
	if( m_bPassword )
	{
		strStar.assign(m_strText.size(), _T('*'));
		m_pRender->Text(m_pCanvas, strStar.substr(m_nDispStart, m_nDispEnd - m_nDispStart), 
			m_pFont, m_dwTextColor, m_dwShadeColor, m_eTextAlign, m_bTextWriteAlpha);
	}
	else
	{
		m_pRender->Text(m_pCanvas, m_strText.substr(m_nDispStart, m_nDispEnd - m_nDispStart), 
			m_pFont, m_dwTextColor, m_dwShadeColor, m_eTextAlign, m_bTextWriteAlpha);
	}

	// 渲染选定字符
	if( m_nSelStart != m_nSelEnd )
	{
		INT nStart = min(m_nSelStart, m_nSelEnd);
		INT nLen = abs(m_nSelEnd - m_nSelStart);
		if( nStart < m_nDispStart )
		{
			nLen -= m_nDispStart - nStart;
			nStart = m_nDispStart;
		}
		if( nStart + nLen >= m_nDispEnd )
			nLen = m_nDispEnd - nStart;

		// 计算反色起始和结束坐标
		m_pCanvas->rc.left = m_ptView.x + m_rcText.left;
		for(INT n=m_nDispStart; n<nStart; n++)
			m_pCanvas->rc.left += m_vecCharSize[n];
		m_pCanvas->rc.right = m_pCanvas->rc.left;
		for(INT n=nStart; n<nStart+nLen; n++)
			m_pCanvas->rc.right += m_vecCharSize[n];

		// 反色(Alpha值不变)
		DWORD dwColor = ((~m_dwTextColor)&0x00ffffff)|(m_dwTextColor&0xff000000); 
		DWORD dwShadeColor = ((~m_dwShadeColor)&0x00ffffff)|(m_dwShadeColor&0xff000000); 
		m_pRender->Draw(m_pCanvas, NULL, m_dwTextColor, m_eAlphaWrite);
		if( m_bPassword )
		{
			m_pRender->Text(m_pCanvas, strStar.substr(nStart, nLen).c_str(), m_pFont, 
				dwColor, m_dwShadeColor, m_eTextAlign, m_bTextWriteAlpha);
		}
		else
		{
			m_pRender->Text(m_pCanvas, m_strText.substr(nStart, nLen).c_str(), m_pFont, 
				dwColor, m_dwShadeColor, m_eTextAlign, m_bTextWriteAlpha);
		}
	}

	// 渲染光标 
	if( m_bShowCursor && m_nCursorPos >=m_nDispStart && m_nCursorPos <= m_nDispEnd )
	{
		m_pCanvas->rc.left = m_ptView.x + m_rcText.left;
		for(INT n=m_nDispStart; n<m_nCursorPos; n++)
			m_pCanvas->rc.left += m_vecCharSize[n];
		m_pCanvas->rc.right = m_pCanvas->rc.left+2;
		m_pRender->Draw(m_pCanvas, NULL, m_dwCursorColor, m_eAlphaWrite);
	}

	// 调用基类渲染
	GUIWnd::Render();
}


//-----------------------------------------------------------------------------
// 基本消息处理
//-----------------------------------------------------------------------------
BOOL GUIEditBox::OnInputMessage(tagGUIInputMsg* pMsg)
{
	INT nPos = m_nCursorPos;
	switch( pMsg->dwType )
	{
	case WM_LBUTTONDOWN: // 找到鼠标点击的字符，并移动光标
		nPos = GetCharIndex(pMsg->pt.x);
		if( GT_VALID(nPos) )
		{
			m_nSelStart = m_nSelEnd = m_nCursorPos = nPos;
			this->SetRefresh();
		}
		break;

	case WM_LBUTTONDBLCLK:	// 计算双击的选择区域
		m_nSelStart = m_strText.find_last_of(' ', m_nCursorPos) + 1;
		m_nSelEnd = m_strText.find_first_of(' ', m_nCursorPos);
		if( m_nSelEnd == std::string::npos )
			m_nSelEnd = m_strText.size();
		m_nCursorPos = m_nSelEnd;
		this->SetRefresh();
		break;

	case WM_MOUSEMOVE:
		if( pMsg->dwParam1 == MK_LBUTTON && m_pSystem->GetActive() == this )	// 处理鼠标拖动选择
		{
			if( pMsg->pt.x < m_ptView.x+m_rcText.left )
				this->ChangeDisplayStart(-1);
			if( pMsg->pt.x > m_ptView.x+m_rcText.right )
				this->ChangeDisplayStart(1);

			INT n = GetCharIndex(pMsg->pt.x);
			if( GT_VALID(n) )
				m_nSelEnd = m_nCursorPos = n;
			this->SetRefresh();
		}
		break;

	case WM_KEYDOWN:
		if( VK_LEFT == pMsg->dwParam1 && m_nCursorPos > 0 )	// 处理左方向键
		{
			m_nCursorPos--;
			if( m_nCursorPos < m_nDispStart )	// 是否需要往右卷轴
				this->ChangeDisplayStart(-1);
		}
		if( VK_RIGHT == pMsg->dwParam1 && m_nCursorPos < (INT)m_strText.size() ) // 处理右方向键
		{
			m_nCursorPos++;
			while( m_nCursorPos > m_nDispEnd ) // 需要卷动
				this->ChangeDisplayStart(1);
		}
		if( VK_HOME == pMsg->dwParam1 )	// 光标移动到行首前
		{
			m_nCursorPos = 0;
			this->ChangeDisplayStart(-m_nDispStart);
		}
		if( VK_END == pMsg->dwParam1 )	// 光标移动到行首尾
		{
			m_nCursorPos = m_strText.size();
			while( m_nCursorPos > m_nDispEnd )
				this->ChangeDisplayStart(1);
		}
		if( VK_DELETE == pMsg->dwParam1 && !m_bReadOnly )
		{
			if( m_nSelStart != m_nSelEnd )
				this->DelSelected();
			else
			{
				// 删除字符
				m_strText.erase(m_nCursorPos, 1);
				this->CalCharWidth();
			}
			this->SetRefresh();
			return TRUE;
		}
		if( VK_LEFT == pMsg->dwParam1 || VK_RIGHT == pMsg->dwParam1	// 处理SHIFT选择
			 || VK_HOME == pMsg->dwParam1 || VK_END == pMsg->dwParam1 )
		{
			if( GetKeyState(VK_SHIFT) < 0 )
				m_nSelStart = (m_nSelStart == m_nSelEnd) ? nPos : m_nSelStart;
			else
				m_nSelStart = m_nCursorPos;
			m_nSelEnd = m_nCursorPos;
			this->SetRefresh();
			return TRUE;
		}
		if( VK_RETURN == pMsg->dwParam1 && !m_bReadOnly )	// 处理回车
		{
			tagGUIEvent event(this, EGUIE_EditBoxEnter, pMsg);	// 产生事件
			SendEvent(&event);
			return TRUE;
		}
		break;

	case WM_SYSCHAR:
	case WM_CHAR:
		this->SetRefresh();
		switch( pMsg->dwParam1 )
		{
		case ('c' & 0x1F):	// Ctrl + C
			this->CopyToClipboard();
			return TRUE;
		case ('v' & 0x1F):	// Ctrl + V
			if( !m_bReadOnly )
			{
				this->DelSelected();
				this->PasteFromClipboard();
				this->SetRefresh();
			}
			return TRUE;
		case ('a' & 0x1F):	// Ctrl+A 选中所有元素
			m_nSelStart = m_nDispStart = m_nCursorPos = 0;
			m_nSelEnd = m_strText.size();
			ChangeDisplayStart(0);	// 计算m_nDispEnd
			this->SetRefresh();
			return TRUE;
		case ('x' & 0x1F):	// Ctrl+X
			this->CopyToClipboard();
			if( !m_bReadOnly )
				this->DelSelected();
			this->SetRefresh();
			return TRUE;
		case VK_BACK:	// 退格 相当于 左箭头+del
			if( !m_bReadOnly )
			{
				if( m_nSelStart != m_nSelEnd ) // 有选中，删选中
					this->DelSelected();
				else if( m_nCursorPos > 0 )
				{
					m_nCursorPos--;
					if( m_nCursorPos < m_nDispStart )	// 是否需要往右卷轴 
						this->ChangeDisplayStart(-1);

					if( GetKeyState(VK_SHIFT) < 0 )	// 处理选择
					{
						m_nSelStart = m_nSelStart == m_nSelEnd ? nPos : m_nSelStart;
						m_nSelEnd = m_nCursorPos;
					}

					// 删除字符
					m_strText.erase(m_nCursorPos, 1);
					this->CalCharWidth();
				}
				return TRUE;
			}
			break;

		case VK_TAB:
			break;

		case VK_RETURN:	// 处理回车
			{
				tagGUIEvent event(this, EGUIE_EditBoxEnter, pMsg);	// 产生事件
				SendEvent(&event);
				return TRUE;
			}
			break;

		default:
			if( m_bReadOnly || GetKeyState(VK_ESCAPE) < 0 
				||(m_bNumerical && (pMsg->dwParam1<0x30||pMsg->dwParam1> 0x39))) //只允许数字
				break;
			if( (INT)m_strText.size() + 1 >= m_nMaxTextCount )	//先计算字节数上的限制
				break;

			if( m_nSelStart != m_nSelEnd )	// 假如有选择，先删除选择部分
				this->DelSelected();

			m_strText.insert(m_nCursorPos++, (TCHAR*)&pMsg->dwParam1);
			this->CalCharWidth();
			while( m_nCursorPos > m_nDispEnd ) // 是否需要往右卷轴
				this->ChangeDisplayStart(1);
			return TRUE;
			break; // case default
		}	
		break;	// case WM_CHAR:
	}	// switch( pMsg->dwType )


	return GUIStatic::OnInputMessage(pMsg);
}


//-----------------------------------------------------------------------------
// 被设置为焦点
//-----------------------------------------------------------------------------
GUIWnd* GUIEditBox::OnActive()
{
	// 光标移动到字符串末尾
	m_nCursorPos = m_strText.size();
	while( m_nCursorPos > m_nDispEnd )
		this->ChangeDisplayStart(1);

	// 全部选择
	m_nSelStart = 0;
	m_nSelEnd = m_nCursorPos;

	this->SetRefresh();
	return GUIStatic::OnActive();
}


//-----------------------------------------------------------------------------
// 丢失焦点
//-----------------------------------------------------------------------------
VOID GUIEditBox::OnDeactive(GUIWnd* pWnd)
{
	m_nSelStart = m_nSelEnd = m_nCursorPos;	// 取消选择
	m_bShowCursor = false;	// 光标消失

	this->SetRefresh();
	GUIStatic::OnDeactive(pWnd);
}


//-----------------------------------------------------------------------------
// 移动控件,并可改变控件大小
//-----------------------------------------------------------------------------
BOOL GUIEditBox::Resize(tagPoint& ptSize)
{
	tagPoint pt = ptSize;
	BOOL bResult = GUIStatic::Resize(ptSize);
	if( !(pt == ptSize) )
		ChangeDisplayStart(0);	// 控件大小改变,需要重新计算显示参数

	return bResult;
}


//-----------------------------------------------------------------------------
// 如果字符串被修改，需要调用此函数计算每个字符宽度
//-----------------------------------------------------------------------------
VOID GUIEditBox::CalCharWidth()
{
	TCHAR szTemp[2] = {0};
	tagPoint ptSize;

	// 数值框先判断是否超出最大值
	if(m_bNumerical)
	{
		INT nValue = _tcstol(m_strText.c_str(), NULL, 10);
		if(nValue < 0 || nValue > m_nMaxNumberValue)
		{
			tstringstream ss;
			ss << m_nMaxNumberValue;
			m_strText = ss.str();
			m_nCursorPos = m_strText.size();
		}
	}
	
	m_vecCharSize.assign(m_strText.size()+1, 0); // +1留出循环空间
	for(INT n=0; n<(INT)m_strText.size(); n++)
	{
		szTemp[0] = m_bPassword ? _T('*') : m_strText[n];
		m_pRender->GetTextSize(szTemp, m_pFont, ptSize);
		m_vecCharSize[n] = ptSize.x;
	}

	ChangeDisplayStart(0);	// 重新计算m_nDispStart/m_nDispEnd
}


//-----------------------------------------------------------------------------
// 修改从那个字符开始显示(nIndex为相对值)
//-----------------------------------------------------------------------------
VOID GUIEditBox::ChangeDisplayStart(INT nIndex)
{
	if( m_nDispStart <= 0 && nIndex < 0 )
		return;
	if( m_nDispEnd >= (INT)m_strText.size() && nIndex > 0 )
		return;

	m_nDispStart += nIndex;

	// 计算EditBox当前渲染文字字数
	INT nLeft = 0;
	for(INT n=m_nDispStart; n<(INT)m_strText.size(); n++)
	{
		if( nLeft + m_vecCharSize[n] >= m_rcText.right - m_rcText.left )
			break;	// 显示不下了

		nLeft += m_vecCharSize[n];
		m_nDispEnd = n + 1;
	}
}


//-----------------------------------------------------------------------------
// 由view坐标得到最接近字符的序号
//-----------------------------------------------------------------------------
INT GUIEditBox::GetCharIndex(INT nViewX)
{
	// 找到鼠标点击的字符
	INT nX = nViewX - m_ptView.x - m_rcText.left;
	INT nBestPos = GT_INVALID;
	INT nLeastDistance = 0x7fffffff;
	INT nCharPos = 0;	// 记录当前字符位置

	if( m_strText.size() == 0 || nX < 0 )
		return 0;

	for(INT n=m_nDispStart; n<=m_nDispEnd; n++)
	{
		if( abs(nCharPos - nX) < nLeastDistance )
		{
			nLeastDistance = abs(nCharPos - nX);
			nBestPos = n;
		}

		if( n < (INT)m_vecCharSize.size() )
			nCharPos += m_vecCharSize[n];
	}

	return nBestPos;
}


//-----------------------------------------------------------------------------
// 直接设置文本框内容
//-----------------------------------------------------------------------------
VOID GUIEditBox::SetText(LPCTSTR szText)
{
	//注意字节限制
	INT nTextCount = (INT)lstrlen(szText);
	nTextCount = (nTextCount > m_nMaxTextCount) ? m_nMaxTextCount : nTextCount;

	m_strText.assign(szText, nTextCount);
	m_nCursorPos = m_nSelEnd = m_nSelStart = 0;
	m_nDispStart = m_nDispEnd = 0;
	this->CalCharWidth();
	this->SetRefresh();
}


//-----------------------------------------------------------------------------
// 删除选择的字符
//-----------------------------------------------------------------------------
VOID GUIEditBox::DelSelected()
{
	if( m_nSelStart == m_nSelEnd ) //　没有文本被选择
		return;

	if( m_nSelStart > m_nSelEnd )
		std::swap(m_nSelStart, m_nSelEnd);

	m_strText.erase(m_nSelStart, m_nSelEnd - m_nSelStart);
	m_nCursorPos = m_nSelEnd = m_nSelStart;
	if( m_nSelStart < m_nDispStart )
		m_nDispStart = m_nCursorPos;

	this->CalCharWidth();
}


//-----------------------------------------------------------------------------
// 将选定内容拷到剪贴板 
//-----------------------------------------------------------------------------
VOID GUIEditBox::CopyToClipboard()
{
	// 密码模式/无选择 不拷贝
	if( m_bPassword || m_nSelStart == m_nSelEnd )
		return;	
	
	if( !OpenClipboard(NULL) )
		return;

	EmptyClipboard(); 

	INT nLen = abs(m_nSelEnd - m_nSelStart);
	HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (nLen+1) * sizeof(TCHAR)); 
	if( NULL == hglbCopy ) 
	{ 
		CloseClipboard();
		return;
	} 

	// Lock the handle and copy the text to the buffer. 
	LPTSTR lptstrCopy = (LPTSTR)GlobalLock( hglbCopy ); 
	INT nStart = m_nSelStart < m_nSelEnd ? m_nSelStart : m_nSelEnd;
	memcpy(lptstrCopy, &m_strText[nStart], nLen * sizeof(TCHAR)); 
	lptstrCopy[nLen] = (TCHAR)0;    // null character 
	GlobalUnlock(hglbCopy); 

	// Place the handle on the clipboard. 
	SetClipboardData(CF_UNICODETEXT, hglbCopy); 
	CloseClipboard();
}


//-----------------------------------------------------------------------------
// 从剪贴板拷贝文本到当前位置
//-----------------------------------------------------------------------------
VOID GUIEditBox::PasteFromClipboard()
{
	if( FALSE == IsClipboardFormatAvailable(CF_UNICODETEXT) ) 
		return; 

	if( FALSE == OpenClipboard(NULL) )
		return; 

	HGLOBAL hglb = GetClipboardData(CF_UNICODETEXT); 
	if( hglb != NULL ) 
	{ 
		LPTSTR lpstr = (LPTSTR)GlobalLock(hglb); 
		if( NULL != lpstr ) 
		{ 
			tstring str(lpstr);
			DWORD dwInsertSize = str.size();

			if( m_bNumerical )	// 判断该字符串是不是含有非数字
			{
				tstring strNum(_T("0123456789"));
				if( tstring::npos != str.find_first_not_of(strNum) )
				{
					GlobalUnlock(hglb); 
					CloseClipboard(); 
					return;
				}
			}

			if( m_strText.size() + dwInsertSize > (DWORD)m_nMaxTextCount ) // 节数限制
				dwInsertSize = m_nMaxTextCount - m_strText.size();

			m_strText.insert(m_nCursorPos, str.substr(0, dwInsertSize));
			m_nCursorPos += dwInsertSize;
			this->CalCharWidth();

			// 计算是否需要往左卷轴
			while( m_nCursorPos > m_nDispEnd )
				this->ChangeDisplayStart(1);

			GlobalUnlock(hglb); 
		} 
	} 
	
	CloseClipboard(); 
	return; 
}


}	// namespace vEngine {
