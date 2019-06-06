#include "StdAfx.h"
#include "RichEditBoxForChatShow.h"
#include "ChatFrame.h"
#include ".\GameClientDefine.h"
#include "CursorMgr.h"
#define TOP_SCROLLBAR_MAX_SIZE 20						
RichEditBoxForChatShow::RichEditBoxForChatShow(void)
{
	m_bFlex = FALSE;
	m_byFlexStatus = -1;
}

RichEditBoxForChatShow::~RichEditBoxForChatShow(void)
{
}



//-----------------------------------------------------------------------------
// 基本消息处理
//-----------------------------------------------------------------------------
BOOL RichEditBoxForChatShow::OnInputMessage(tagGUIInputMsg* pMsg)
{
	switch( pMsg->dwType )
	{
	case WM_LBUTTONDOWN:
		{
			m_bShowCursor = TRUE;
			// 找到鼠标点击的字符
			INT nChar = GetCharIndex(pMsg->pt.x, pMsg->pt.y, FALSE);
			if( GT_INVALID != nChar )
			{
				m_nSelectStart = m_nSelectEnd = m_nCursorPos = nChar;	//取消选择

				while( m_nCursorPos > GetLastDisplayIndex() )
					m_nStartDisplayLine++;
			}

			if( m_bFlex && pMsg->pt.y >= m_ptView.y			//鼠标在窗口的上沿
				&& pMsg->pt.y <= m_ptView.y+GUI_DEFAULT_ROW_SPACING )
			{
				m_ptLastMouseClickView.y = pMsg->pt.y;		//使拖动平滑
				m_byFlexStatus = 0;
			}
			else
				m_byFlexStatus = -1;

			//是否点击超链接
			BOOL bLink = ClickLink(pMsg);
			//改变光标
			AdptiveCursor(m_byFlexStatus==0, bLink);
			this->SetRefresh();
		}
		break;
	case WM_LBUTTONUP:
		{
			//是否点击超链接
			BOOL bLink = ClickLink(pMsg);
		}
		break;
	case WM_RBUTTONDOWN:
		{
			m_bShowCursor = TRUE;
			// 找到鼠标点击的字符
			INT nChar = GetCharIndex(pMsg->pt.x, pMsg->pt.y, FALSE);
			if( GT_INVALID != nChar )
			{
				m_nSelectStart = m_nSelectEnd = m_nCursorPos = nChar;	//取消选择

				while( m_nCursorPos > GetLastDisplayIndex() )
					m_nStartDisplayLine++;
			}

			if( m_bFlex && pMsg->pt.y >= m_ptView.y			//鼠标在窗口的上沿
				&& pMsg->pt.y <= m_ptView.y+GUI_DEFAULT_ROW_SPACING )
			{
				m_ptLastMouseClickView.y = pMsg->pt.y;		//使拖动平滑
				m_byFlexStatus = 0;
			}
			else
				m_byFlexStatus = -1;

			this->SetRefresh();
		}
		break;
	case WM_RBUTTONUP:
		{
			if( m_bCanSelect == TRUE )
			{
				CalSelectForDoubleClick();
				tagGUIEvent event(this,(EGUIEvent)(EXUIE_MouseRClick));
				event.msg = *pMsg;
				SendEvent(&event);
				this->SetRefresh();
			}
		}
		break;
	case WM_LBUTTONDBLCLK:
		{	//双击
			if( m_bCanSelect == TRUE )
			{
				//不是双击超链接,并且文字可选
				CalSelectForDoubleClick();
				tagGUIEvent event(this,(EGUIEvent)(EXUIE_MouseDoubleClick));
				SendEvent(&event);
				this->SetRefresh();
			}
		}
		break;
	case WM_MOUSEMOVE:
		{		
			// 处理鼠标形状变化 是否处于超链接的位置
			INT nPos = GetCharIndex(pMsg->pt.x, pMsg->pt.y, TRUE);
			m_nMouseLinkIndex = GetLinkIndex(nPos);

			if( GT_INVALID == m_nMouseLinkIndex )
				AdptiveCursor(FALSE, FALSE );
			else
				AdptiveCursor(FALSE, TRUE);

			if( m_pSystem->GetActive() != this )
			break;
			//-----------------------------------------------

			if( TRUE == PointInWnd(pMsg->pt) )
			{
				// 处理鼠标拖动选择
				if( FALSE == m_bCanSelect )
					break;

				if( pMsg->dwParam1 == MK_LBUTTON )
				{
					//如果是向左方向选择,则选中状态以左为原则,否则以右为原则
					INT nChar = GetCharIndex(pMsg->pt.x, pMsg->pt.y, TRUE);
					if( GT_INVALID != nChar )
					{ //
						if( nChar < m_nSelectStart )
							m_nSelectEnd = m_nCursorPos = nChar;
						else
						{
							nChar  = GetCharIndex(pMsg->pt.x, pMsg->pt.y, FALSE);
							if( GT_INVALID != nChar )
								m_nSelectEnd = m_nCursorPos = nChar;
						}
						this->SetRefresh();
					}
				}
			}
			else
			{
				if( (pMsg->dwParam1 != MK_LBUTTON ) || (GetTickCount()-m_dwLastMoveTime) < 20 )
					break;

				if( FALSE == m_bCanSelect )
					break;

				m_dwLastMoveTime = GetTickCount();

				if( pMsg->pt.y < m_ptView.y )
				{
					//上移动
					CalCursonForUpDown(TRUE);
					while( m_nCursorPos < GetRowHead(m_nStartDisplayLine) )
						m_nStartDisplayLine--;

				}
				else if( pMsg->pt.y > m_ptView.y + m_ptSize.y )
				{
					//下移动
					CalCursonForUpDown(FALSE);
					while( m_nCursorPos > GetLastDisplayIndex() )
						m_nStartDisplayLine++;
				}

				m_nSelectEnd = m_nCursorPos;
				SendScrollEvent();
				this->SetRefresh();
			}
		}
		break;
	case WM_MOUSEWHEEL:	// 处理鼠标滚轮
		{
			if( pMsg->dwParam1 & 0x80000000 )	// down
			{
				ScrollText(FALSE);
			}
			else // up
			{
				ScrollText(TRUE);
			}
			SendScrollEvent();
			this->SetRefresh();
		}
		break;
	case WM_KEYDOWN:
		this->SetRefresh();
		return OnMsgForKeyDown(pMsg->dwParam1);

	case WM_SYSCHAR:
	case WM_CHAR:
		{
			this->SetRefresh();
			//首先处理ctrl+ ()的快捷键
			if( pMsg->dwParam1 == ('a' & 0x1F) )	// ctrl+a 大小写无关
			{
				m_nSelectStart = 0;
				m_nSelectEnd = m_strText.size() - 2;
				m_nCursorPos = 0;
			}
			else if( pMsg->dwParam1 == ('c' & 0x1F) )	//ctrl+c  大小写无关
			{
				this->CopyToClipboard();
			}
			else if( pMsg->dwParam1 == ('x' & 0x1F) )	//ctrl+x  大小写无关
			{
				this->CopyToClipboard();

				if( m_bReadOnly )
					break;

				this->DelSelected();
			}
			else if( pMsg->dwParam1 == ('v' & 0x1F) )	//ctrl+v  大小写无关
			{
				if( m_bReadOnly )
					break;

				this->DelSelected();
				this->PasteFromClipboard();
			}

			if( m_bReadOnly )
				break;

			if( pMsg->dwParam1 <= 0x1F && pMsg->dwParam1 > 0 )
			{
				// 必须是有效字符,否则不予显示
				return TRUE;
			}

			// 处理基本文字输入
			// 假如有选择，先删除选择部分
			if( m_nSelectStart != m_nSelectEnd )
				this->DelSelected();

			//不能超过字节上限
			if( (INT)m_strText.size() + 1 < m_nMaxEditTextCount )
			{
				if( m_strText.size() + 1 >= m_dwMaxTextCount )
				{
					m_dwMaxTextCount = m_strText.size() + 2;
					m_VecCharIntro.resize(m_dwMaxTextCount+2);
				}

				InsertText((TCHAR*)&pMsg->dwParam1, m_nCursorPos, m_dwTextColor, m_dwShadeColor);
			}

			while( m_nCursorPos > GetLastDisplayIndex() )
				m_nStartDisplayLine++;

			SendScrollEvent();
			return TRUE;
		}
		break;
	default:
		break;
	}

	return GUIStatic::OnInputMessage(pMsg);
}

void RichEditBoxForChatShow::AdptiveCursor(bool bDrag, BOOL bLink)
{
	if( bLink )
	{
		CursorMgr::Inst()->SetIdleCursor( ECIN_Hyperlink );
	}
	else 
	{
		CursorMgr::Inst()->SetIdleCursor( ECIN_Default );
	}
}

void RichEditBoxForChatShow::ZoomWnd( INT nOffset )
{
	GUIWnd* pGFather = (GUIWnd*)m_pFather->GetFather();
	if( P_VALID(pGFather) )
	{
		tagPoint ptSize = (tagPoint&)pGFather->GetSize();

		ptSize.y += nOffset;
		pGFather->SetResizable(TRUE);
		pGFather->Resize(ptSize);
		pGFather->SetResizable(FALSE);	
		pGFather->SetMovable(TRUE);
		pGFather->Move(tagPoint(0, nOffset));
		pGFather->SetMovable(FALSE);
		pGFather->Align();

		ptSize = (tagPoint&)m_pFather->GetSize();
		ptSize.y += nOffset;
		m_pFather->SetResizable(TRUE);
		m_pFather->Resize(ptSize);
		m_pFather->SetResizable(FALSE);
		m_pFather->Align();

		//通知其它聊天窗口改变大小
		tagGUIEvent event(this, EGUIE_Drag);
		event.dwParam1 = nOffset;
		event.dwParam2 = 1;
		SendEvent(&event);
		SendScrollEvent();		

		//刷新
		//pGFather->SetRefresh(TRUE);
		//pGFather->SetInvisible(FALSE);		
	}

}