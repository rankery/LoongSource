//-----------------------------------------------------------------------------
//!\file gui_editboxex.cpp
//!\author Lyp
//!
//!\date 2004-12-08
//! last 2008-01-21
//!
//!\brief 界面系统 edit box ex 控件
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "gui_static.h"
#include "gui_editboxex.h"
#include "gui_event_define.h"
#include "..\resource\resource.h"

namespace vEngine {
//-----------------------------------------------------------------------------
// construction
//-----------------------------------------------------------------------------
GUIEditBoxEx::GUIEditBoxEx()
{
	m_pVarMgr->Register(_T("ReadOnly"), &m_bReadOnly, _T("no yes"));
	m_pVarMgr->Register(_T("LineHeight"), &m_nLineHeight);
	m_pVarMgr->Register(_T("CursorColor"), &m_dwCursorColor);
	m_pVarMgr->Register(_T("CursorTimer"), &m_dwCursorTimer);
    m_pVarMgr->Register(_T("MaxTextCount"), &m_nMaxTextCount);
	
	m_dwTextColor = 0xFF000000;
	m_dwShadeColor = 0;
	m_dwCursorColor = 0xFF000000;
    m_dwCursorTimer = 200;
    m_nMaxTextCount = 0xFFFF;
	m_nLineHeight = 16;	// 行高
	m_bShowCursor = false;	// 不显示光标
	m_nDispStart = m_nCurFirstLine = m_nCursorPos = 0;
	m_bEditable = TRUE;
}


//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL GUIEditBoxEx::Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement)
{
	InitializeCriticalSection(&m_Lock);
	BOOL bResult = GUIStatic::Init(pSystem, pFather, pXmlElement);
	m_nVisibleLine = (m_rcText.bottom - m_rcText.top) / m_nLineHeight;	// 一共可显示的行数
	
	tstring strText = m_strText;
	if( m_strText.size() > 0 )
		strText.assign(m_strText.begin(), m_strText.end()-1);
	this->Clear();

	// 判断是否是等宽字体
	m_bFixWidth = FALSE;	// 是否等宽
	tagPoint pt1, pt2;
	m_pRender->GetTextSize(_T("M"), m_pFont, pt1);
	m_pRender->GetTextSize(_T("i"), m_pFont, pt2);
	if( pt1.x == pt2.x )
	{
		m_bFixWidth = TRUE;		// 是否等宽
		m_nEngWidth = pt1.x;	// 英文宽度
	}

	m_pRender->GetTextSize(_T("你"), m_pFont, pt1);
	m_nChnWidth = pt1.x;	// 中文宽度

	this->AddString(0, strText);
	return bResult;
}


//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
VOID GUIEditBoxEx::Destroy()
{
	DeleteCriticalSection(&m_Lock);
	GUIStatic::Destroy();
}


//-----------------------------------------------------------------------------
// update
//-----------------------------------------------------------------------------
VOID GUIEditBoxEx::Update()
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
VOID GUIEditBoxEx::Render()
{
	if( !IsRefresh() )
		return GUIWnd::Render();

	// 渲染底图
	m_pCanvas->rc.SetLTWH(m_ptView, m_ptSize);
	m_pRender->Draw(m_pCanvas, m_pPic, m_dwPicColor, m_eAlphaWrite);

	// 渲染书签
	if( m_mapBookmark.Size() > 0 )
	{
		for(INT n=m_nCurFirstLine; n<m_nCurFirstLine+m_nVisibleLine; n++)
		{
			if( n >= m_nLine )
				break;

			DWORD dwColor = m_mapBookmark.Peek(n);
			if( GT_VALID(dwColor) )
			{
				m_pCanvas->rc.left = m_ptView.x + m_rcText.left;
				m_pCanvas->rc.right = m_ptView.x + m_rcText.right;
				m_pCanvas->rc.top = m_ptView.y + m_rcText.top + m_nLineHeight*(n-m_nCurFirstLine);
				m_pCanvas->rc.bottom = m_pCanvas->rc.top + m_nLineHeight;
				m_pRender->Draw(m_pCanvas, NULL, dwColor, m_eAlphaWrite);
			}
		}
	}

	// 在此进入锁定区
	EnterCriticalSection(&m_Lock);

	// 计算需要渲染得字符数
	INT nLine=0, nLast = 0;
	for(INT n=m_nDispStart; n<(INT)m_vecChar.size(); n++)
	{
		if( m_vecChar[n].eType > EGEC_Normal )
			nLine++, nLast = n;
		if( nLine > m_nVisibleLine )
			break;
	}

	// 渲染基本字符字
	m_pCanvas->rc = m_rcText + m_ptView;
	m_pRender->Text(m_pCanvas, m_strText.substr(m_nDispStart, nLast - m_nDispStart), 
		m_pFont, m_dwTextColor, m_dwShadeColor, m_eTextAlign, m_bTextWriteAlpha);

	// 渲染选定字符
	if( m_nSelStart != m_nSelEnd )
	{
		INT nSelStart = m_nSelStart;
		INT nSelEnd = m_nSelEnd;

		if( nSelStart > nSelEnd )
			std::swap(nSelStart, nSelEnd);

		DWORD dwTextColor = ((~m_dwTextColor)&0x00ffffff)|(m_dwTextColor&0xff000000); 

		m_pCanvas->rc = m_rcText + m_ptView;
		m_pCanvas->rc.right = m_pCanvas->rc.left;
		m_pCanvas->rc.bottom = m_pCanvas->rc.top + m_nLineHeight;
		for(INT n=m_nDispStart, nPointer=m_nDispStart; n<=nLast; n++)
		{
			if( m_vecChar[n].eType > EGEC_Normal || n >= nSelEnd )
			{
				m_pRender->Draw(m_pCanvas, NULL, m_dwTextColor, m_eAlphaWrite);
				m_pRender->Text(m_pCanvas, m_strText.substr(nPointer, n - nPointer), 
					m_pFont, dwTextColor, m_dwShadeColor, m_eTextAlign, m_bTextWriteAlpha);
				nPointer = n+1;

				m_pCanvas->rc.left = m_pCanvas->rc.right = m_ptView.x + m_rcText.left;
				m_pCanvas->rc.top += m_nLineHeight;
				m_pCanvas->rc.bottom = m_pCanvas->rc.top + m_nLineHeight;
				if( m_pCanvas->rc.bottom > m_ptView.y + m_rcText.bottom )
					m_pCanvas->rc.bottom = m_ptView.y + m_rcText.bottom;

				if( n >= nSelEnd )
					break;
			}

			m_pCanvas->rc.right += m_vecChar[n].nWidth;
			if( n<nSelStart )
			{
				m_pCanvas->rc.left = m_pCanvas->rc.right;
				nPointer=n+1;
			}
		}
	}

	// 渲染光标 
	if( m_bShowCursor && m_nCursorPos >= m_nDispStart && m_nCursorPos <= nLast )
	{
		m_pCanvas->rc.left = m_ptView.x + m_rcText.left;
		for(INT n = this->FindFirstCharThisLine(m_nCursorPos);n<m_nCursorPos;n++)
			m_pCanvas->rc.left += m_vecChar[n].nWidth;
		m_pCanvas->rc.right = m_pCanvas->rc.left+2;
		
		m_pCanvas->rc.top = m_ptView.y + m_rcText.top;
		m_pCanvas->rc.top += (this->GetLine(m_nCursorPos) - m_nCurFirstLine)*m_nLineHeight; 
		m_pCanvas->rc.bottom = m_pCanvas->rc.top + m_nLineHeight; 
		if( m_pCanvas->rc.bottom > m_ptView.y + m_rcText.bottom )
			m_pCanvas->rc.bottom = m_ptView.y + m_rcText.bottom;
		m_pRender->Draw(m_pCanvas, NULL, m_dwCursorColor, m_eAlphaWrite);
	}

	// 在此离开锁定区
	LeaveCriticalSection(&m_Lock);

	// 调用基类渲染
	GUIWnd::Render();
}


//-----------------------------------------------------------------------------
// 基本消息处理
//-----------------------------------------------------------------------------
BOOL GUIEditBoxEx::OnInputMessage(tagGUIInputMsg* pMsg)
{
	INT nLastCursorPos = m_nCursorPos;
	switch( pMsg->dwType )
	{
	case WM_LBUTTONDOWN: // 找到鼠标点击的字符，并移动光标
		if( m_pSystem->GetMousePoint() == this )
		{
			INT nPos = GetCharIndex(pMsg->pt);
			if( GT_VALID(nPos) )
			{
				m_nSelStart = m_nSelEnd = nPos;
				this->ChangeCursorPos(nPos - m_nCursorPos);
				this->SetRefresh();
			}
		}
		break;
	case WM_LBUTTONDBLCLK:	// 计算双击的选择区域
		if( m_pSystem->GetMousePoint() == this )
		{
			m_nSelStart = m_strText.find_last_of(' ', m_nCursorPos) + 1;
			m_nSelEnd = m_strText.find_first_of(' ', m_nCursorPos);
			if( m_nSelEnd == std::string::npos )
				m_nSelEnd = m_strText.size();
			this->ChangeCursorPos(m_nSelEnd - m_nCursorPos);
			this->SetRefresh();
		}
		break;
	case WM_MOUSEMOVE:
		if( pMsg->dwParam1 == MK_LBUTTON && m_pSystem->GetActive() == this )	// 处理鼠标拖动选择
		{
			if( pMsg->pt.y < m_ptView.y+m_rcText.top )	// 处理上下滚
				this->ChangeDisplayLineStart(-1);
			if( pMsg->pt.y > m_ptView.y+m_rcText.bottom )
				this->ChangeDisplayLineStart(1);

			INT n = GetCharIndex(pMsg->pt);
			if( GT_VALID(n) )
			{
				m_nSelEnd = n;
				ChangeCursorPos(n-m_nCursorPos);
			}
			this->SetRefresh();
		}
		break;
	case WM_MOUSEWHEEL:	// 处理鼠标滚轮
		if( pMsg->dwParam1 & 0x80000000 )	// 向下
			this->ChangeDisplayLineStart(3);
		else
			this->ChangeDisplayLineStart(-3);
	case WM_KEYDOWN:
		this->SetRefresh();
		if( VK_LEFT == pMsg->dwParam1 && m_nCursorPos > 0 )	// 处理左方向键
			ChangeCursorPos(-1);
		if( VK_RIGHT == pMsg->dwParam1 && m_nCursorPos < (INT)m_strText.size()-1 ) // 处理右方向键
			ChangeCursorPos(1);
		if( VK_UP == pMsg->dwParam1 && this->GetLine(m_nCursorPos) > 0 )	// 处理上方向键
		{
			INT nThisLeft = m_nCursorPos - this->FindFirstCharThisLine(m_nCursorPos);
			INT nUpEnd = this->FindFirstCharThisLine(m_nCursorPos)-1;
			INT nUpFirst = this->FindFirstCharThisLine(nUpEnd);
			INT nPos = nUpFirst + nThisLeft;
			if( nUpEnd - nUpFirst < nThisLeft )	// 上一行没有本行这么长
				nPos = nUpEnd;
			ChangeCursorPos(nPos-m_nCursorPos);
		}
		if( VK_DOWN == pMsg->dwParam1 && this->GetLine(m_nCursorPos) < m_nLine - 1 )	// 处理下方向键
		{
			INT nThisLeft = m_nCursorPos - this->FindFirstCharThisLine(m_nCursorPos);
			INT nDownFirst = this->FindReturnThisLine(m_nCursorPos)+1;
			INT nDownEnd = this->FindReturnThisLine(nDownFirst);
			INT nPos = nDownFirst + nThisLeft;
			if( nDownEnd - nDownFirst < nThisLeft )	// 下一行没有本行这么长
				nPos = nDownEnd;
			ChangeCursorPos(nPos-m_nCursorPos);
		}
		if( VK_HOME == pMsg->dwParam1 )	// 光标移动到行首前
		{
			INT nPos = this->FindFirstCharThisLine(m_nCursorPos) - m_nCursorPos;
			ChangeCursorPos(nPos);
			if( GetKeyState(VK_CONTROL) < 0 )	// 滚动到文档首
			{
				this->ChangeDisplayLineStart(-m_nCurFirstLine);
				ChangeCursorPos(0-m_nCursorPos);
			}
		}
		if( VK_END == pMsg->dwParam1 )	// 光标移动到行首尾
		{
			INT nPos = this->FindReturnThisLine(m_nCursorPos) - m_nCursorPos;
			ChangeCursorPos(nPos);
			if( GetKeyState(VK_CONTROL) < 0 )	// 滚动到文档尾
			{
				this->ChangeDisplayLineStart(0xffffff);
				ChangeCursorPos(m_vecChar.size()-1-m_nCursorPos);
			}
		}
		if( VK_PRIOR == pMsg->dwParam1 )
		{
			this->ChangeDisplayLineStart(-m_nVisibleLine);
			ChangeCursorPos(m_nDispStart-m_nCursorPos);
		}
		if( VK_NEXT == pMsg->dwParam1 )
		{
			this->ChangeDisplayLineStart(m_nVisibleLine);
			ChangeCursorPos(m_nDispStart-m_nCursorPos);
		}
		if( VK_LEFT == pMsg->dwParam1 || VK_RIGHT == pMsg->dwParam1	// 处理SHIFT选择
			 || VK_HOME == pMsg->dwParam1 || VK_END == pMsg->dwParam1
			 || VK_UP == pMsg->dwParam1 || VK_DOWN == pMsg->dwParam1
			 ||  VK_PRIOR == pMsg->dwParam1 ||  VK_PRIOR == pMsg->dwParam1 )
		{
			if( GetKeyState(VK_SHIFT) < 0 )
				m_nSelStart = (m_nSelStart == m_nSelEnd) ? nLastCursorPos : m_nSelStart;
			else
				m_nSelStart = m_nCursorPos;
			m_nSelEnd = m_nCursorPos;
			return TRUE;
		}
		if( VK_DELETE == pMsg->dwParam1 && !m_bReadOnly ) // 处理del删除
		{
			if( m_nSelStart != m_nSelEnd )
				this->DelString(m_nSelStart, m_nSelEnd);	// 有选中，删除选中
			else
				if( m_nCursorPos < (INT)m_strText.size() - 1 )	// 删除1个字符
					DelString(m_nCursorPos, m_nCursorPos+1);
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
			if( m_bReadOnly )
				break;
			this->DelString(m_nSelStart, m_nSelEnd);
			this->PasteFromClipboard();
			return TRUE;
		case ('a' & 0x1F):	// Ctrl+A 选中所有元素
			m_nSelStart = 0;
			m_nSelEnd = m_strText.size()-1;
			this->ChangeCursorPos(m_vecChar.size()-1-m_nCursorPos);
			this->ChangeDisplayLineStart(0xffffff);	// 挪到最后行末尾
			return TRUE;
		case ('x' & 0x1F):	// Ctrl+X
			this->CopyToClipboard();
			if( !m_bReadOnly )
				this->DelString(m_nSelStart, m_nSelEnd);
			return TRUE;
		case ('l' & 0x1F):	// 选中本行复制到剪贴板，并删除
			m_nSelStart = this->FindFirstCharThisLine(m_nCursorPos);
			m_nSelEnd = this->FindReturnThisLine(m_nCursorPos)+1;
			this->CopyToClipboard();
			if( !m_bReadOnly )
			{
				this->DelString(m_nSelStart, m_nSelEnd);
				this->ChangeCursorPos(m_nSelStart-m_nCursorPos);
			}
			return TRUE;
		case VK_BACK:	// 退格 相当于 左箭头+del
			if( m_bReadOnly )
				break;
			if( m_nSelStart != m_nSelEnd ) // 有选中，删选中
				this->DelString(m_nSelStart, m_nSelEnd);
			else if( m_nCursorPos > 0 && m_vecChar.size() > 1 )
			{
				ChangeCursorPos(-1);
				DelString(m_nCursorPos, m_nCursorPos+1);	// 删除字符
			}
			return TRUE;
		default:
			if( m_bReadOnly || GetKeyState(VK_ESCAPE) < 0 )
				break;

            if( (INT)m_strText.size() + 1 >= m_nMaxTextCount )	//先计算字节数上的限制
                break;

			this->DelString(m_nSelStart, m_nSelEnd); // 先删除选择部分
			ChangeCursorPos(AddString(m_nCursorPos, (TCHAR*)&pMsg->dwParam1));
			return TRUE;
		}	
		break;	// case WM_CHAR:
	}	// switch( pMsg->dwType )

	return GUIStatic::OnInputMessage(pMsg);
}



//-----------------------------------------------------------------------------
// 处理子控件发的事件
//-----------------------------------------------------------------------------
VOID GUIEditBoxEx::SendEvent(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pSystem->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return;

	switch( pEvent->eEvent ) 
	{
	case EGUIE_Scroll:
		if( pWnd != this )
			this->SetCurLine(pEvent->dwParam1, FALSE);
		break;

	default:
		break;
	}

	GUIStatic::SendEvent(pEvent);
}


//-----------------------------------------------------------------------------
// 添加子窗口
//-----------------------------------------------------------------------------
VOID GUIEditBoxEx::AddChild(GUIWnd* pChild)
{
	GUIStatic::AddChild(pChild);

	// 子控件可能是滚动条，让滚动条刷新
	SetCurLine(m_nCurFirstLine); 
}


//-----------------------------------------------------------------------------
// 被设置为焦点
//-----------------------------------------------------------------------------
GUIWnd* GUIEditBoxEx::OnActive()
{
	this->SetRefresh();
	return GUIStatic::OnActive();
}


//-----------------------------------------------------------------------------
// 丢失焦点
//-----------------------------------------------------------------------------
VOID GUIEditBoxEx::OnDeactive(GUIWnd* pWnd)
{
	m_nSelStart = m_nSelEnd = m_nCursorPos;	// 取消选择
	m_bShowCursor = false;	// 光标消失

	this->SetRefresh();
	GUIStatic::OnDeactive(pWnd);
}


//-----------------------------------------------------------------------------
// 移动控件,并可改变控件大小
//-----------------------------------------------------------------------------
BOOL GUIEditBoxEx::Resize(tagPoint& ptSize)
{
	tagPoint pt = m_ptSize;
	BOOL bResult = GUIStatic::Resize(ptSize);
	if( !(pt == m_ptSize) )
	{
		this->ChangeCursorPos(0-m_nCursorPos);
		ReMakeup(0, m_vecChar.size()-1);	// 控件大小改变,需要重新计算显示参数

		// 发出滚动消息
		tagGUIEvent event(this, EGUIE_Scroll, NULL);
		event.dwParam1 = (DWORD)m_nCurFirstLine;
		event.dwParam2 = (DWORD)m_nLine;
		event.dwParam3 = (DWORD)m_nVisibleLine;
		OnEvent(&event);

		this->SetRefresh();
	}

	return bResult;
}


//-----------------------------------------------------------------------------
// 向指定位置，指定行添加字符串，返回增加字符量
//-----------------------------------------------------------------------------
INT GUIEditBoxEx::AddString(INT nPos, tstring str)
{
	if( str.empty() || nPos > (INT)m_vecChar.size() - 1 )
		return 0;

	INT nFind = 0;
	while( (nFind = str.find(_T('\t'), nFind)) != str.npos )	// 替换tab
		str.replace(nFind, 1, _T("    "));
	nFind = 0;
	while( (nFind = str.find(_T("\r\n"), nFind)) != str.npos )	// 去掉/r/n前的/r
		str.replace(nFind, 2, _T("\n"));
	nFind = 0;
	while( (nFind = str.find(_T("\r"), nFind)) != str.npos )	// 替换/r为/n
		str.replace(nFind, 1, _T("\n"));

	tagPoint ptSize;
	tagGUIEditCharInfo info;
	TCHAR szTemp[2] = {0};
	m_strText.insert(nPos, str);
	m_vecChar.insert(m_vecChar.begin()+nPos, str.size(), info);

	if( m_bFixWidth )	// 是否等宽字
	{
		for(INT n=0; n<(INT)str.size(); n++)
		{
			szTemp[0] = str[n];
			if( ((CHAR*)szTemp)[1] != 0 )
				m_vecChar[n+nPos].nWidth = m_nChnWidth;
			else
				m_vecChar[n+nPos].nWidth = m_nEngWidth;
			if( str[n] == _T('\n') )
			{
				m_vecChar[n+nPos].eType = EGEC_Return;
				m_vecChar[n+nPos].nWidth = 0;
				m_nLine++;
			}
		}
	}
	else
	{
		for(INT n=0; n<(INT)str.size(); n++)
		{
			szTemp[0] = str[n];
			m_pRender->GetTextSize(szTemp, m_pFont, ptSize);
			m_vecChar[n+nPos].nWidth = ptSize.x;
			if( str[n] == _T('\n') )
			{
				m_vecChar[n+nPos].eType = EGEC_Return;
				m_nLine++;
			}
		}
	}

	this->SetRefresh();

	// 需要重新排版，以及统计实际增加字符数
	INT nIncrease = str.size() + ReMakeup(nPos, nPos +str.size());

	// 发出滚动消息
	tagGUIEvent event(this, EGUIE_Scroll);
	event.dwParam1 = (DWORD)m_nCurFirstLine;
	event.dwParam2 = (DWORD)m_nLine;
	event.dwParam3 = (DWORD)m_nVisibleLine;
	OnEvent(&event);

	return nIncrease;
}


//-----------------------------------------------------------------------------
// 得到本行的第一个字
//-----------------------------------------------------------------------------
INT	GUIEditBoxEx::FindFirstCharThisLine(INT nPos)
{
	if( nPos >= (INT)m_vecChar.size() )
		return nPos = m_vecChar.size() - 1;
	if( nPos <= 0 )
		return nPos;
	
	if( m_vecChar[nPos].eType > EGEC_Normal )	// 指定位置就是回车
	{
		// 如果前面也是回车，就返回自己
		if( m_vecChar[nPos-1].eType > EGEC_Normal )
			return nPos;
		nPos--;
	}

	while( nPos >= 0 )
	{
		if( m_vecChar[nPos].eType > EGEC_Normal )
			return nPos+1;	// 上一行回车之后的一个字是本行的第一个字

		nPos--;
	}

	return 0;
}


//-----------------------------------------------------------------------------
// 得到指定行的第一个字
//-----------------------------------------------------------------------------
INT GUIEditBoxEx::FindFirstChar(INT nLine)
{
	INT nPos = 0;
	INT nLineStep = 0;

	while( nPos < (INT)m_vecChar.size() )
	{
		if( m_vecChar[nPos].eType > EGEC_Normal )
		{
			
			if( nLineStep >= nLine )
			{
				if( nPos > 0 )
					return FindFirstCharThisLine(nPos);
				else
					return nPos;
			}

			nLineStep++;
		}

		nPos++;
	}

	return 0;
}



//-----------------------------------------------------------------------------
// 得到本行的换行符位置
//-----------------------------------------------------------------------------
INT	GUIEditBoxEx::FindReturnThisLine(INT nPos)
{
	if( nPos >= (INT)m_vecChar.size() )
		return nPos = m_vecChar.size() - 1;

	while( nPos < (INT)m_vecChar.size() )
	{
		if( m_vecChar[nPos].eType > EGEC_Normal )
			break;

		nPos++;
	}

	return nPos;
}


//-----------------------------------------------------------------------------
// 由指定位置得到行号,必须确保m_nDispStart以及m_nCurFirstLine正确
//-----------------------------------------------------------------------------
INT	GUIEditBoxEx::GetLine(INT nPos)
{
	INT nPointer = 0;
	INT nCounter = 0;
	if( nPos == 0 )
		return 0;
	if( nPos >= (INT)m_vecChar.size() )
		return m_nLine-1;
	if( nPos == m_nDispStart )
		return m_nCurFirstLine;

	if( nPos > m_nDispStart )	// 从m_nDispStart前后开始统计比较优化
	{
		nPointer=m_nDispStart;
		nCounter = m_nCurFirstLine;
		for(;nPointer<nPos; nPointer++)
		{
			if( m_vecChar[nPointer].eType > EGEC_Normal )
				nCounter++;
		}
	}
	else
	{
		nCounter = m_nCurFirstLine;
		for(nPointer=m_nDispStart-1;nPointer>=nPos; nPointer--)
		{
			if( m_vecChar[nPointer].eType > EGEC_Normal )
				nCounter--;
		}
	}

	return nCounter;
}


//-----------------------------------------------------------------------------
// 重新排版,返回增加字符量
//-----------------------------------------------------------------------------
INT GUIEditBoxEx::ReMakeup(INT nPos, INT nEndPos)
{
	INT nWidth = 0;	// 用来统计宽度
	INT nIncrease = 0;	// 用来统计增减的换行符数量
	tagGUIEditCharInfo info;
	info.eType = EGEC_VReturn;	// 软回车

	// 从指定字符所属行的第一个字开始排版
	for(INT n=FindFirstCharThisLine(nPos); n<(INT)m_vecChar.size()-1; n++)
	{
		nWidth += m_vecChar[n].nWidth;
		if( nWidth > m_rcText.right - m_rcText.left ) // 宽度到了
		{
			m_strText.insert(n, _T("\n"), 1); // 加一个软回车
			m_vecChar.insert(m_vecChar.begin()+n, info);
			nWidth = 0;		// 宽度重新统计
			nIncrease++;	// 字符数改变
			nEndPos++;
			m_nLine++;	// 相当于增加了一行
			continue;	// 此行结束
		}

		if( m_vecChar[n].eType == EGEC_Return ) // 碰到硬回车
		{
			if( n >= nEndPos ) // 超过需要统计的范围
				break;

			nWidth = 0;	// 宽度重新统计
			continue;	// 此行结束
		}

		if( m_vecChar[n].eType == EGEC_VReturn ) // 碰到软回车就删除
		{
			m_strText.erase(n, 1);
			m_vecChar.erase(m_vecChar.begin()+n);
			nIncrease--;	// 字符数改变
			m_nLine--;
			n--;
			nEndPos--;
			continue;
		}
	}

	return nIncrease;
}


//-----------------------------------------------------------------------------
// 修改从那一行开始显示(nLine为相对值)
//-----------------------------------------------------------------------------
VOID GUIEditBoxEx::ChangeDisplayLineStart(INT nLine, BOOL bSendEvent)
{
	INT nPointer = m_nDispStart;
	if( nLine < 0 )
	{
		if( nLine < -m_nCurFirstLine )
			nLine = -m_nCurFirstLine;
		if( nLine >= 0 )	// 无法上滚
			return;

		INT nIncLine = 0;
		while( nIncLine > nLine )
		{
			nPointer--;
			if( nPointer <= 0 )
			{
				nPointer = 0;
				break;
			}

			if( m_vecChar[nPointer].eType > EGEC_Normal )
			{
				nIncLine--;
				m_nCurFirstLine--;
			}
		}
	}
	else
	{
		if( m_nCurFirstLine + nLine + m_nVisibleLine >= m_nLine )	// 尽量下滚
			nLine = m_nLine - m_nCurFirstLine - m_nVisibleLine;
		if( nLine <= 0 )	// 无法下滚
			return;

		INT nIncLine = 0;
		while( nIncLine < nLine )
		{
			if( m_vecChar[nPointer].eType > EGEC_Normal )
			{
				nIncLine++;
				m_nCurFirstLine++;
			}
			nPointer++;
			if( nPointer >= (INT)m_vecChar.size() )
			{
				nPointer = m_vecChar.size() - 1;
				break;
			}
		}
	}

	m_nDispStart = this->FindFirstCharThisLine(nPointer);
	SetRefresh();
	
	if( bSendEvent )	// 发出滚动消息
	{
		tagGUIEvent event(this, EGUIE_Scroll);
		event.dwParam1 = (DWORD)m_nCurFirstLine;
		event.dwParam2 = (DWORD)m_nLine;
		event.dwParam3 = (DWORD)m_nVisibleLine;
		OnEvent(&event);
	}
}


//-----------------------------------------------------------------------------
// 修改鼠标光标位置(nPos为相对值)
//-----------------------------------------------------------------------------
VOID GUIEditBoxEx::ChangeCursorPos(INT nPos)
{
	m_nCursorPos += nPos;
	if( m_nCursorPos > (INT)m_vecChar.size() - 1 )
		m_nCursorPos = m_vecChar.size() - 1;
	if( m_nCursorPos < 0  )
		m_nCursorPos = 0;

	INT nLine = GetLine(m_nCursorPos);
	if( nLine < m_nCurFirstLine )	// 是否要往上卷
		ChangeDisplayLineStart(nLine - m_nCurFirstLine);
	if( nLine >= m_nCurFirstLine + m_nVisibleLine)	// 是否要往下卷
		ChangeDisplayLineStart(nLine - m_nCurFirstLine - m_nVisibleLine+1);

	// 发出光标移动消息
	tagGUIEvent event(this, EGUIE_CursorMove);
	event.dwParam1 = (DWORD)nLine;
	event.dwParam2 = (DWORD)m_nLine;
	event.dwParam3 = (DWORD)m_nVisibleLine;
	SendEvent(&event);

	this->SetRefresh();
}


//-----------------------------------------------------------------------------
// 由view坐标得到最接近字符的序号
//-----------------------------------------------------------------------------
INT GUIEditBoxEx::GetCharIndex(tagPoint pt)
{
	// 找到鼠标点击的字符
	INT nX = pt.x - m_ptView.x - m_rcText.left;
	INT nY = pt.y - m_ptView.y - m_rcText.top;
	INT nBestPos = GT_INVALID;
	INT nLeastDistanceX = 0x7fffffff;
	INT nLeastDistanceY = 0x7fffffff;
	tagPoint ptCharPos(0, m_nLineHeight/2);	// 记录当前字符位置

	if( m_strText.size() <= 1 )
		return 0;

	for(INT n=m_nDispStart; n<(INT)m_vecChar.size(); n++)
	{
		INT nDistX = (ptCharPos.x - nX)*(ptCharPos.x - nX);
		INT nDistY = (ptCharPos.y - nY)*(ptCharPos.y - nY);
		
		if( nDistY < nLeastDistanceY )	// 优先按照行来匹配
		{
			nLeastDistanceX = nDistX;
			nLeastDistanceY = nDistY;
			nBestPos = n;
		}

		if( nDistY <= nLeastDistanceY && nDistX <= nLeastDistanceX )
		{
			nLeastDistanceX = nDistX;
			nLeastDistanceY = nDistY;
			nBestPos = n;
		}

		ptCharPos.x += m_vecChar[n].nWidth;
		if( m_vecChar[n].eType > EGEC_Normal )
		{
			ptCharPos.x = 0;
			ptCharPos.y += m_nLineHeight;
			if( ptCharPos.y > m_rcText.bottom-m_rcText.top )
				break;
		}
	}

	return nBestPos;
}


//-----------------------------------------------------------------------------
// 删除所有信息
//-----------------------------------------------------------------------------
VOID GUIEditBoxEx::Clear()
{
	m_nSelStart = m_nSelEnd = 0;	// 选择位置
	this->ChangeCursorPos(0-m_nCursorPos);		// 光标位置

	// 加入一个软回车
	tagGUIEditCharInfo info;
	info.eType = EGEC_VReturn;
	m_vecChar.clear();
	m_vecChar.push_back(info);
	m_strText.assign(_T("\n"));
	m_nLine = 1;	// 一共多少行

	// 发出滚动消息
	tagGUIEvent event(this, EGUIE_Scroll, NULL);
	event.dwParam1 = (DWORD)m_nCurFirstLine;
	event.dwParam2 = (DWORD)m_nLine;
	event.dwParam3 = (DWORD)m_nVisibleLine;
	OnEvent(&event);
	
	this->SetRefresh();
}


//-----------------------------------------------------------------------------
// 设置当前行
//-----------------------------------------------------------------------------
VOID GUIEditBoxEx::SetCurLine(INT n, BOOL bSendEvent)
{ 
	this->ChangeDisplayLineStart(n-m_nCurFirstLine, bSendEvent); 
}


//-----------------------------------------------------------------------------
// bVirtualReturn:是否需要虚拟回车
//-----------------------------------------------------------------------------
tstring& GUIEditBoxEx::GetText(BOOL bVirtualReturn)
{
	// 为了还原文本原貌，将所有的\n都变成\r\n,所有软回车都删除
	m_strOut = m_strText;
	for(INT n=0, nP=0; nP<(INT)m_vecChar.size(); n++,nP++)
	{
		if( m_vecChar[nP].eType == EGEC_Return )
		{
			m_strOut.insert(n, 1, _T('\r'));
			n++;
		}
		if( m_vecChar[nP].eType == EGEC_VReturn && !bVirtualReturn )
		{
			m_strOut.erase(n, 1);
			n--;
		}
	}

	return m_strOut;
}



//-----------------------------------------------------------------------------
// 得到文本框实际内容, nLen:[in]需要取出长度，[out]实际取出长度
//-----------------------------------------------------------------------------
LPCTSTR	GUIEditBoxEx::GetText(INT nStart, INT& nLen)
{
	if( nLen == 0 )
		nLen = m_vecChar.size() - 1 - nStart;

	m_strOut.clear();
	if( nLen <= 0 || nStart < 0 || nStart > (INT)m_vecChar.size()-1 )
	{
		nLen = 0;
		return m_strOut.c_str();
	}

	// 为了还原文本原貌，将所有的\n都变成\r\n,所有软回车都删除
	m_strOut = m_strText.substr(nStart, nLen);
	for(INT n=0, nP=nStart; n<nLen; n++,nP++)
	{
		if( m_vecChar[nP].eType == EGEC_Return )
		{
			m_strOut.insert(n, 1, _T('\r'));
			n++;
			nLen++;
		}
		if( m_vecChar[nP].eType == EGEC_VReturn )
		{
			m_strOut.erase(n, 1);
			n--;
			nLen--;
		}
	}
	
	return m_strOut.c_str();
}


//-----------------------------------------------------------------------------
// 直接设置文本框内容
//-----------------------------------------------------------------------------
VOID GUIEditBoxEx::SetText(LPCTSTR szText)
{
    //注意字节限制
    INT nTextCount = (INT)lstrlen(szText);
    nTextCount = (nTextCount > m_nMaxTextCount) ? m_nMaxTextCount : nTextCount;

	this->Clear();
	this->AddString(0, szText);

    m_nCursorPos = m_nSelEnd = m_nSelStart = 0;
    m_nDispStart = 0;
	this->SetRefresh();
}


//-----------------------------------------------------------------------------
// 删除前面多少行
//-----------------------------------------------------------------------------
VOID GUIEditBoxEx::DelFrontLine(INT nLine)
{
	INT nPointer = 0;
	INT nCounter = 0;
	for(nPointer=0;nPointer<(INT)m_vecChar.size()-1; nPointer++)
	{
		if( m_vecChar[nPointer].eType > EGEC_Normal )
		{
			nCounter++;
			if( nCounter >= nLine )
				break;
		}
	} 

	this->DelString(0, nPointer);
}




//-----------------------------------------------------------------------------
// 删除指定字符
//-----------------------------------------------------------------------------
VOID GUIEditBoxEx::DelString(INT nPos, INT nEndPos)
{
	if( nPos == nEndPos )
		return;

	if( nPos > nEndPos )
		std::swap(nPos, nEndPos);

	m_nCursorPos = m_nSelEnd = m_nSelStart = nPos;	// 取消选择

	INT nStartLine = this->GetLine(nPos);
	INT nEndLine = this->GetLine(nEndPos);
	INT nOriginLines = m_nLine;

	// 计算删除多少行
	for(INT n=nPos; n<nEndPos; n++)
	{
		if( m_vecChar[n].eType > EGEC_Normal )
			m_nLine--;
	}

	m_strText.erase(nPos, nEndPos - nPos);
	m_vecChar.erase(m_vecChar.begin()+nPos, m_vecChar.begin() + nEndPos);

	INT nCharDec = nEndPos - nPos - this->ReMakeup(nPos, 0);	// 实际减少的字符
	INT	nLineDec = nOriginLines - m_nLine;	// 实际减少的行数

	if( nLineDec <= 0 )
		return;	// 没有行数的减少，不需要再计算

	// 如果删除位置完全处于显示范围之前
	if( nPos < m_nDispStart && nEndPos <= m_nDispStart )
	{
		m_nCurFirstLine -= nLineDec;
		m_nDispStart = FindFirstChar(m_nCurFirstLine);
	}
	else if( nPos < m_nDispStart && nEndPos > m_nDispStart )
	{
		// 如果删除位置与显示位置重合
		m_nCurFirstLine -= (m_nCurFirstLine - nStartLine);
		m_nDispStart = FindFirstChar(m_nCurFirstLine);
	}

	// 如果最后没有足够的文本显示
	if( m_nCurFirstLine + m_nVisibleLine > m_nLine )
	{
		// 需要将显示起始位置挪到合适的位置
		if( m_nVisibleLine >= m_nLine )
		{
			m_nCurFirstLine = m_nDispStart = m_nCursorPos = 0;
		}
		else
		{
			m_nCurFirstLine = m_nLine - m_nVisibleLine;
			m_nDispStart = FindFirstChar(m_nCurFirstLine);
		}
	}

	if( m_vecChar.size() == 0 )
		this->Clear();

	// 发出滚动消息
	tagGUIEvent event(this, EGUIE_Scroll, NULL);
	event.dwParam1 = (DWORD)m_nCurFirstLine;
	event.dwParam2 = (DWORD)m_nLine;
	event.dwParam3 = (DWORD)m_nVisibleLine;
	OnEvent(&event);

}


//-----------------------------------------------------------------------------
// 将选定内容拷到剪贴板 
//-----------------------------------------------------------------------------
VOID GUIEditBoxEx::CopyToClipboard()
{
	// 无选择 不拷贝
	if( m_nSelStart == m_nSelEnd )
		return;	
	
	if( !OpenClipboard(NULL) )
		return;

	EmptyClipboard(); 

	// 为了还原文本原貌，将所有的\n都变成\r\n,所有软回车都删除
	INT nLen = abs(m_nSelEnd - m_nSelStart);
 	INT nStart = m_nSelStart < m_nSelEnd ? m_nSelStart : m_nSelEnd;
	LPCTSTR szText = this->GetText(nStart, nLen);
	
	HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (nLen+1) * sizeof(TCHAR)); 
	if( NULL == hglbCopy ) 
	{ 
		CloseClipboard();
		return;
	} 

	// Lock the handle and copy the text to the buffer. 
	LPTSTR lptstrCopy = (LPTSTR)GlobalLock( hglbCopy );
	memcpy(lptstrCopy, szText, nLen * sizeof(TCHAR)); 
	lptstrCopy[nLen] = (TCHAR)0;    // null character 
	GlobalUnlock(hglbCopy); 

	// Place the handle on the clipboard. 
	SetClipboardData(CF_UNICODETEXT, hglbCopy); 
	CloseClipboard();
}


//-----------------------------------------------------------------------------
// 从剪贴板拷贝文本到当前位置
//-----------------------------------------------------------------------------
VOID GUIEditBoxEx::PasteFromClipboard()
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
            if( m_strText.size() + dwInsertSize > (DWORD)m_nMaxTextCount ) // 字数限制
                str = str.substr(0, m_nMaxTextCount - m_strText.size() - 1);

		    INT nAdd = AddString(m_nCursorPos, str.c_str());
		    ChangeCursorPos(nAdd);
		    GlobalUnlock(hglb);
		}
	} 
	
	CloseClipboard(); 
	return; 
}


//-----------------------------------------------------------------------------
// 设置书签
//-----------------------------------------------------------------------------
VOID GUIEditBoxEx::SetBookmark( INT nLine, DWORD dwColor )
{
	if( !GT_VALID(dwColor) )
		m_mapBookmark.Erase(nLine);
	else
		m_mapBookmark.Add(nLine, dwColor);
}


//-----------------------------------------------------------------------------
// 全部选中
//-----------------------------------------------------------------------------
VOID GUIEditBoxEx::SelectAll()
{
	m_nSelStart = 0;
	m_nSelEnd = m_strText.size()-1;
	this->ChangeCursorPos(m_vecChar.size()-1-m_nCursorPos);
	this->ChangeDisplayLineStart(0xffffff);	// 挪到最后行末尾
	this->SetRefresh();
}

}	// namespace vEngine {