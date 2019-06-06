//-----------------------------------------------------------------------------
//!\file gui_listbox.cpp
//!\author Lyp
//!
//!\date 2004-12-08
//! last 2007-04-23
//!
//!\brief 界面系统 list box 控件
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "gui_static.h"
#include "..\variable\var_container.h"
#include "gui_listbox.h"

#include "gui_event_define.h"
#include "gui_editbox.h"
#include "gui_combobox.h"
#include "gui_button.h"
#include "gui_pushbutton.h"

#include "Commdlg.h"
#include <algorithm>

namespace vEngine {
//-----------------------------------------------------------------------------
// construction
//-----------------------------------------------------------------------------
GUIListBox::GUIListBox()
{
	m_pVarMgr->Register(_T("MoveSelect"), &m_bMoveSelect, _T("no yes"));
	m_pVarMgr->Register(_T("CanMoveCol"), &m_bCanMoveCol, _T("no yes"));
	m_pVarMgr->Register(_T("OpenFileDialog"), &m_bOpenFileDialog, _T("no yes"));
	m_pVarMgr->Register(_T("OpenColorDialog"), &m_bOpenColorDialog, _T("no yes"));
	m_pVarMgr->Register(_T("RowHeight"), &m_nRowHeight);

	m_pVarMgr->Register(_T("SelectStickPicFileName"), &m_strSelectStickPic);
	m_pVarMgr->Register(_T("SelectStickPicRect"), &m_rcSelectStickPic);
	m_pVarMgr->Register(_T("SelectStickColor"), &m_dwSelectStickColor);
	m_pVarMgr->Register(_T("SelectStickActiveColor"), &m_dwSelectStickActiveColor);
	m_pVarMgr->Register(_T("RowLineColor"), &m_dwRowLineColor);
	m_pVarMgr->Register(_T("SelectTextColor"), &m_dwSelectTextColor);
	m_pVarMgr->Register(_T("SelectShadeColor"), &m_dwSelectShadeColor);
	m_pVarMgr->Register(_T("PicOffset"), &m_ptPicOffset);
	m_pVarMgr->Register(_T("PicSize"), &m_ptPicSize);

	m_nRowHeight = 16;
	m_dwSelectStickColor = 0xffA0A0A0;
	m_dwSelectStickActiveColor = 0xff808080;
	m_dwSelectTextColor = 0xff000000;
	m_dwRowLineColor = 0xffffffff;
	m_ptPicSize.Set(16, 16);
}

//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL GUIListBox::Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement)
{
	BOOL bResult = GUIStatic::Init(pSystem, pFather, pXmlElement);
	if( FALSE == bResult )
		return bResult;
	
	// 创建选择棒图片
	m_pSelectStickPic = m_pRender->CreateImage(m_strSelectStickPic, m_rcSelectStickPic);

	m_pEditBox = NULL;
	m_pComboBox = NULL;		// 组合框

	m_mapWidth.Clear();
	m_mapWidth.Add(0, 70);	// 添加两个默认列
	m_mapWidth.Add(1, 70);
	m_mapWidth.Add(2, 70);

	m_nCurrentTextRow = 0;
	m_nTotalRow = m_ptSize.y / m_nRowHeight;
	m_nTotalTextRow = 0;
	m_nCurSelectedSegment = GT_INVALID;	// 当前鼠标选中的竖分割线
	m_nCurSelectedRow = 0;	// 当前选中的行
	m_nCurEditRow = 0;	// 当前编辑的行
	m_nCurEditCol = 0;	// 当前编辑的列
	m_nCurSelectedRowWhenMouseDown = GT_INVALID;// 当鼠标左键按下的时候选择的行

	return bResult;
}

//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
VOID GUIListBox::Destroy()
{
	tagGUIListItem* pItem = 0;
	m_mapItem.ResetIterator();
	while( m_mapItem.PeekNext(pItem) )
	{
		m_pRender->DestroyImage(pItem->pPic);
		delete(pItem);
	}

	m_pRender->DestroyImage(m_pSelectStickPic);
	GUIStatic::Destroy();
}

//-----------------------------------------------------------------------------
// render
//-----------------------------------------------------------------------------
VOID GUIListBox::Render()
{
	if( !IsRefresh() )
		return GUIWnd::Render();

	// 渲染底图
	m_pCanvas->rc.SetLTWH(m_ptView, m_ptSize);
	m_pRender->Draw(m_pCanvas, m_pPic, m_dwPicColor, m_eAlphaWrite);

	// 画出选择棒
	for(INT nY=0, nRow = m_nCurrentTextRow; nY<m_ptSize.y; nY += m_nRowHeight, nRow++)
	{
		if( nRow == m_nCurSelectedRow )
		{
			m_pCanvas->rc.Set(m_ptView.x, m_ptView.y + nY, m_ptView.x + m_ptSize.x, 
				m_ptView.y + nY + m_nRowHeight);
			if( m_pCanvas->rc.bottom > m_ptView.y + m_ptSize.y )
				m_pCanvas->rc.bottom = m_ptView.y + m_ptSize.y;

			if( nRow == m_nCurSelectedRowWhenMouseDown )
				m_pRender->Draw(m_pCanvas, m_pSelectStickPic, m_dwSelectStickActiveColor, m_eAlphaWrite);
			else
				m_pRender->Draw(m_pCanvas, m_pSelectStickPic, m_dwSelectStickColor, m_eAlphaWrite);
		}
	}

	if( m_dwRowLineColor )	// 是否需要画出分界线
	{
		for(INT nCol=0, nWidth = 0; nCol<m_mapWidth.Size()-1; nCol++)	// 画竖线
		{
			nWidth += m_mapWidth.Peek(nCol);
			if( GT_INVALID != nWidth && nWidth < m_ptSize.x )
			{
				m_pCanvas->rc.Set(m_ptView.x + nWidth, m_ptView.y, m_ptView.x + nWidth, 
					m_ptView.y + m_ptSize.y);
				m_pRender->Line(m_pCanvas, m_dwRowLineColor);
			}
		}
		
		for(INT nY=m_nRowHeight; nY < m_ptSize.y; nY += m_nRowHeight)	// 横线
		{
			m_pCanvas->rc.Set(m_ptView.x, nY + m_ptView.y, m_ptView.x + m_ptSize.x, nY + m_ptView.y);
			m_pRender->Line(m_pCanvas, m_dwRowLineColor);
		}
	}

	// 显示listbox中的文本
	tagGUIListItem* pItem = NULL;
	INT nLeftX = m_rcText.left;
	for(INT nCol=0; nCol<m_mapWidth.Size(); nCol++)
	{
		INT nTextY = m_nCurrentTextRow; // 应该从哪一行文本开始显示
		for(INT nYOffset = m_rcText.top; 
			nYOffset < m_ptSize.y; nYOffset += m_nRowHeight, nTextY++)
		{
			DWORD dwPos = MAKELONG(nTextY, nCol);
			pItem = m_mapItem.Peek(dwPos);
			if( !P_VALID(pItem) )	// 当前单元格没有内容
				continue;

			INT nPicOffsetX = 0;
			if( P_VALID(pItem->pPic) )
			{
				m_pCanvas->rc.SetLTWH(m_ptView.x+nLeftX+m_ptPicOffset.x-m_rcText.left, 
					m_ptView.y+nYOffset+m_ptPicOffset.y-m_rcText.top, 
					m_ptPicSize.x, m_ptPicSize.y);
				m_pRender->Draw(m_pCanvas, pItem->pPic, pItem->dwPicColor, m_eAlphaWrite);
				nPicOffsetX = m_ptPicOffset.x + m_ptPicSize.x;
			}

			m_pCanvas->rc.left = m_ptView.x + nLeftX + nPicOffsetX;
			m_pCanvas->rc.right = m_pCanvas->rc.left + m_mapWidth.Peek(nCol) - m_rcText.left - nPicOffsetX;
			if( nCol == m_mapWidth.Size() - 1 ) // 最右边一列的列宽可以加大
				m_pCanvas->rc.right = m_ptView.x + m_ptSize.x;

			m_pCanvas->rc.top = m_ptView.y + nYOffset;
			m_pCanvas->rc.bottom = m_pCanvas->rc.top + m_nRowHeight - 2;

			if( m_pCanvas->rc.right > m_ptView.x + m_ptSize.x )
				m_pCanvas->rc.right = m_ptView.x + m_ptSize.x;

			if( m_pCanvas->rc.bottom > m_ptView.y + m_ptSize.y )
				m_pCanvas->rc.bottom = m_ptView.y + m_ptSize.y;

			if( nTextY == m_nCurSelectedRow )
				m_pRender->Text(m_pCanvas, pItem->strText.c_str(), m_pFont, 
					m_dwSelectTextColor, m_dwSelectShadeColor, m_eTextAlign, m_bTextWriteAlpha);
			else
				m_pRender->Text(m_pCanvas, pItem->strText.c_str(), m_pFont, 
					pItem->dwColor, m_dwShadeColor, m_eTextAlign, m_bTextWriteAlpha);
 		}
		
		nLeftX += m_mapWidth.Peek(nCol);
		if( GT_INVALID == nLeftX || nLeftX > m_ptSize.x )
			break;
	}

	// 调用基类渲染
	GUIWnd::Render();
}



//-----------------------------------------------------------------------------
// 给指定单元格设置文本
//-----------------------------------------------------------------------------
VOID GUIListBox::SetText(INT nRow, INT nCol, LPCTSTR szText, DWORD dwColor)
{
	ASSERT( nRow >=0 && nRow <= 0xffff );	// 最大65536
	ASSERT( nCol >=0 && nCol <= 0xffff );	// 最大65536

	DWORD dwPos = MAKELONG(nRow, nCol);
	tagGUIListItem* pItem = m_mapItem.Peek(dwPos);
	if( !P_VALID(pItem) )
	{
		pItem = new tagGUIListItem;
		if( !P_VALID(pItem) )
			return;

		m_mapItem.Add(dwPos, pItem);
	}

	pItem->strText.assign(szText);
	if( GT_VALID(dwColor) )
		pItem->dwColor = dwColor;
	else
		pItem->dwColor = m_dwTextColor;

	//计算行数相关
	if( nRow >= m_nTotalTextRow )
		m_nTotalTextRow = nRow + 1;

	if( nRow >= m_nTotalRow )
	{
		m_nTotalRow = nRow + 1;
		SetCurrentTextRow(m_nCurrentTextRow); // 让滚动条刷新
	}

	SetRefresh();
}


//-----------------------------------------------------------------------------
// 得到指定单元格的字符串内容
//-----------------------------------------------------------------------------
LPCTSTR GUIListBox::GetText(INT nRow, INT nCol)
{
	DWORD dwPos = MAKELONG(nRow, nCol);
	tagGUIListItem* pItem = m_mapItem.Peek(dwPos);
	if( P_VALID(pItem) )
		return pItem->strText.c_str();
	else
		return (LPCTSTR)GT_INVALID;	// 指定单元格无内容,返回GT_INVALID
}



//-----------------------------------------------------------------------------
// 设置单元格信息
//-----------------------------------------------------------------------------
VOID GUIListBox::SetItemPic(INT nRow, INT nCol, LPCTSTR szPic, const tagRect& rc, DWORD dwColor)
{
	DWORD dwPos = MAKELONG(nRow, nCol);
	tagGUIListItem* pItem = m_mapItem.Peek(dwPos);
	if( !P_VALID(pItem) )
	{
		this->SetText(nRow, nCol, _T(""));
		pItem = m_mapItem.Peek(dwPos);
		if( !P_VALID(pItem) )
			return;
	}

	if( P_VALID(pItem->pPic) )
		m_pRender->DestroyImage(pItem->pPic);
	
	pItem->pPic = m_pRender->CreateImage(szPic, rc);
	pItem->dwPicColor = dwColor;
	SetRefresh();
}


//-----------------------------------------------------------------------------
// 设置单元格信息
//-----------------------------------------------------------------------------
VOID GUIListBox::SetItemEditable(INT nRow, INT nCol, BOOL bEditable)
{
	DWORD dwPos = MAKELONG(nRow, nCol);
	tagGUIListItem* pItem = m_mapItem.Peek(dwPos);
	if( !P_VALID(pItem) )
		return;

	pItem->bEditable = bEditable;
}



//-----------------------------------------------------------------------------
// 设置单元格信息
//-----------------------------------------------------------------------------
VOID GUIListBox::SetItemOptionText(INT nRow, INT nCol, std::list<tstring>& listOptionText)
{
	DWORD dwPos = MAKELONG(nRow, nCol);
	tagGUIListItem* pItem = m_mapItem.Peek(dwPos);
	if( !P_VALID(pItem) )
		return;

	pItem->listOptionText = listOptionText;
}


//-----------------------------------------------------------------------------
// 添加空白行
//-----------------------------------------------------------------------------
VOID GUIListBox::InsertRow(INT nRow)
{
	ASSERT( nRow <= 0xffff );	// 最大65535行
	if( nRow >= m_nTotalTextRow )	// 向最后添加我们不需要处理
		return;

	DWORD dwPos = 0;
	tagGUIListItem* pItem = NULL;
	TMap<DWORD, tagGUIListItem*> mapTemp = m_mapItem;
	m_mapItem.Clear();

	mapTemp.ResetIterator();
	while(mapTemp.PeekNext(dwPos, pItem))
	{
		DWORD dwKey = dwPos;
		INT nThisRow = LOWORD(dwPos);
		if( nThisRow >= nRow )
			dwKey = MAKELONG(nThisRow+1, HIWORD(dwPos));

		m_mapItem.Add(dwKey, pItem);
	}

	//计算行数相关
	m_nTotalTextRow++;

	//重新计算listbox高度
	if( m_nTotalTextRow > m_nTotalRow )
	{
		m_nTotalRow = m_nTotalTextRow;
		SetCurrentTextRow(m_nCurrentTextRow); // 让滚动条刷新
	}

	SetRefresh();
}



//-----------------------------------------------------------------------------
// 设置列数
//-----------------------------------------------------------------------------
VOID GUIListBox::SetColNum(INT nColNum, INT nDefaultColWidth)
{
	m_mapWidth.Clear();
	for(INT nCol=0; nCol<nColNum; nCol++)
		m_mapWidth.Add(nCol, nDefaultColWidth);

	m_nCurSelectedSegment = GT_INVALID;	// 取消鼠标选择的分割线
	SetRefresh();
}




//-----------------------------------------------------------------------------
// 设置指定列的宽度,找不到指定列返回FALSE
//-----------------------------------------------------------------------------
BOOL GUIListBox::SetColWidth(INT nColIndex, INT nColWidth)
{
	m_nCurSelectedSegment = GT_INVALID;	// 取消鼠标选择的分割线
	SetRefresh();
	return m_mapWidth.ChangeValue(nColIndex, nColWidth);
}


//-----------------------------------------------------------------------------
// 删除所有内容
//-----------------------------------------------------------------------------
VOID GUIListBox::Clear()
{
	m_nCurrentTextRow = 0;
	m_nTotalTextRow = 0;
	m_nTotalRow = m_ptSize.y / m_nRowHeight;
	m_nCurSelectedSegment = GT_INVALID;	// 当前鼠标选中的竖分割线
	
	if( m_nCurSelectedRow >= m_nTotalRow )
		m_nCurSelectedRow = 0;	// 当前选中的行

	m_mapItem.ResetIterator();
	tagGUIListItem* pItem = 0;
	while( m_mapItem.PeekNext(pItem) )
	{
		m_pRender->DestroyImage(pItem->pPic);
		delete(pItem);
	}

	m_mapItem.Clear();

	// 发出滚动消息
	tagGUIEvent event(this, EGUIE_Scroll, NULL);
	event.dwParam1 = (DWORD)m_nCurSelectedRow;
	event.dwParam2 = (DWORD)m_nTotalRow;
	event.dwParam3 = (DWORD)m_nTotalRow;
	OnEvent(&event);
	SetRefresh();
}



//-----------------------------------------------------------------------------
// 设置当前显示的第一行是实际的哪一行
//-----------------------------------------------------------------------------
BOOL GUIListBox::SetCurrentTextRow(INT nY, BOOL bSendEvent)
{
	if( nY < 0 )
		nY = 0;

	INT nMax = m_nTotalRow - m_ptSize.y / m_nRowHeight;
	if( nMax < 0 )
		nMax = 0;

	if( nY > nMax )
		nY = nMax;

	m_nCurrentTextRow = nY;
	if( bSendEvent )	// 发出滚动消息
	{
		tagGUIEvent event(this, EGUIE_Scroll, NULL);
		event.dwParam1 = (DWORD)nY;
		event.dwParam2 = (DWORD)m_nTotalRow;
		event.dwParam3 = (DWORD)m_ptSize.y / m_nRowHeight;
		OnEvent(&event);
	}

	SetRefresh();
	return TRUE;
}



//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL GUIListBox::OnInputMessage(tagGUIInputMsg* pMsg)
{
	switch( pMsg->dwType )
	{
	case WM_LBUTTONDOWN:case WM_RBUTTONDOWN:
		if( PointInWnd(pMsg->pt, false) )
		{
			INT nX = m_ptView.x;	// 检测鼠标选中的分割线
			for(INT nCol=0; nCol<m_mapWidth.Size()-1; nCol++)
			{
				INT nCurWidth = m_mapWidth.Peek(nCol);
				nX += nCurWidth;
				if( pMsg->pt.x > nX - 5 && pMsg->pt.x < nX + 5 )
				{
					m_nCurSelectedSegment = nCol;
				}
			}

			// 选择棒的移动和选择
			for(INT nY=0, nRow = m_nCurrentTextRow; nY<m_ptSize.y; 
				nY += m_nRowHeight, nRow++)
			{
				if( pMsg->pt.y - m_ptView.y >= nY 
					&& pMsg->pt.y - m_ptView.y < nY + m_nRowHeight)
				{
					m_nCurSelectedRow = nRow;
					m_nCurSelectedRowWhenMouseDown = nRow;
				}
			}
			SetRefresh();
		}
		break;

	case WM_LBUTTONUP:case WM_RBUTTONUP:
		for(INT nY=0, nRow = m_nCurrentTextRow; nY<m_ptSize.y; 
			nY += m_nRowHeight, nRow++)
		{
			if( pMsg->pt.y - m_ptView.y >= nY 
				&& pMsg->pt.y - m_ptView.y < nY + m_nRowHeight)
			{
				// 当鼠标左键按下的时候选择的行
				if( m_nCurSelectedRowWhenMouseDown == nRow )
				{
					tagGUIEvent event(this, EGUIE_ItemClick, pMsg);
					event.dwParam1 = m_nCurSelectedRow;
					SendEvent(&event);
				}
			}
		}

		// 当鼠标左键按下的时候选择的行
		m_nCurSelectedRowWhenMouseDown = GT_INVALID;
		m_nCurSelectedSegment = GT_INVALID;	// 取消鼠标选择的分割线
		SetRefresh();
		break;

	case WM_LBUTTONDBLCLK:	// 双击编辑
		for(INT nY=0, nRow=m_nCurrentTextRow; nY<m_ptSize.y; nY+=m_nRowHeight, nRow++)
		{
			if( pMsg->pt.y - m_ptView.y >= nY && pMsg->pt.y - m_ptView.y < nY + m_nRowHeight )	// 定位行
			{
				for(INT nCol=0, nX=0; nCol<m_mapWidth.Size(); nCol++)
				{
					INT nCurWidth = m_mapWidth.Peek(nCol);
					// 最后一列的宽度可能大于设定宽度
					if( nCol == m_mapWidth.Size() - 1 && nX+nCurWidth < m_ptSize.x )
						nCurWidth += m_ptSize.x - nX + nCurWidth;	

					if( pMsg->pt.x-m_ptView.x > nX && pMsg->pt.x-m_ptView.x < nX+nCurWidth ) // 定位列
					{
						m_nCurEditRow = nRow;
						m_nCurEditCol = nCol;

						tagPoint ptClient(nX+1, nY+1), ptSize(nCurWidth-1, m_nRowHeight-1);
						if( nX+nCurWidth > m_ptSize.x )
							ptSize.x -= nX + nCurWidth - m_ptSize.x;

						// 激活内部的编辑器
						ActiveEditor(nRow, nCol, ptClient, ptSize);
					}
					nX+=nCurWidth;
				}

				tagGUIEvent event(this, EGUIE_ItemDblClick, pMsg);	// 双击
				event.dwParam1 = m_nCurEditRow;
				event.dwParam2 = m_nCurEditCol;
				SendEvent(&event);	
				break;
			}
		}
		break;

	case WM_MOUSEMOVE:
		if( FALSE == this->PointInWnd(pMsg->pt) )
		{
			if( P_VALID(m_nCurSelectedRowWhenMouseDown) )
			{
				SetRefresh();
				m_nCurSelectedRowWhenMouseDown = GT_INVALID;	
			}
		}

		// 处理鼠标拖动列宽分割线
		if( m_bCanMoveCol == TRUE && pMsg->dwParam1 == MK_LBUTTON && GT_VALID(m_nCurSelectedSegment) )
		{
			INT nX = m_ptView.x;
			for(INT nCol=0; nCol<=m_nCurSelectedSegment; nCol++)
			{
				INT nCurWidth = m_mapWidth.Peek(nCol);
				nX += nCurWidth;
			}

			INT nDiff = pMsg->pt.x - nX;
			if( m_mapWidth.Peek(m_nCurSelectedSegment) + nDiff > 0 )
			{
				if( nX + nDiff > m_ptView.x + m_ptSize.x - 2 )
					nDiff = m_ptView.x + m_ptSize.x - 2 - nX;

				m_mapWidth.ChangeValue(m_nCurSelectedSegment, m_mapWidth.Peek(m_nCurSelectedSegment) + nDiff);
			}
			else
				m_mapWidth.ChangeValue(m_nCurSelectedSegment, 1);
			SetRefresh();
		}

		// 选择棒的移动
		if( m_bMoveSelect && PointInWnd(pMsg->pt, false) )
		{
			for(INT nY=0, nRow = m_nCurrentTextRow; nY < m_ptSize.y; 
				nY += m_nRowHeight, nRow++)
			{
				if( pMsg->pt.y - m_ptView.y >= nY 
					&& pMsg->pt.y - m_ptView.y < nY + m_nRowHeight)
				{
					m_nCurSelectedRow = nRow;
					SetRefresh();
				}
			}
		}
		break;

	case WM_MOUSEWHEEL:	// 处理鼠标滚轮
		if( pMsg->dwParam1 & 0x80000000 )	// 向下
			this->SetCurrentTextRow(m_nCurrentTextRow+1);
		else
			this->SetCurrentTextRow(m_nCurrentTextRow-1);
		break;

	case WM_KEYDOWN:
		SetRefresh();
		switch(pMsg->dwParam1)
		{
		case VK_UP:
			if( m_nCurSelectedRow > m_nCurrentTextRow )
				m_nCurSelectedRow--;
			else	// 两者相等
			{
				if( m_nCurrentTextRow > 0 )	
				{
					SetCurrentTextRow(m_nCurrentTextRow-1);
					m_nCurSelectedRow--;
				}
			}
			return TRUE;
		case VK_DOWN:
			if( m_nCurSelectedRow < m_nTotalRow-1 )
			{
				m_nCurSelectedRow++;
				if( m_nCurSelectedRow > m_nCurrentTextRow + m_ptSize.y / m_nRowHeight - 1 )
					SetCurrentTextRow(m_nCurrentTextRow+1);
			}
			return TRUE;
		case VK_PRIOR:	// 向前翻页
			if( SetCurrentTextRow(m_nCurrentTextRow-m_ptSize.y / m_nRowHeight) )
				m_nCurSelectedRow = m_nCurrentTextRow;
			else
			{
				SetCurrentTextRow(0);
				m_nCurSelectedRow = 0;
			}
			return TRUE;
		case VK_NEXT:	// 向后翻页
			if( SetCurrentTextRow(m_nCurrentTextRow+m_ptSize.y / m_nRowHeight) )
				m_nCurSelectedRow = m_nCurrentTextRow;
			else
			{
				SetCurrentTextRow(m_nTotalRow - m_ptSize.y / m_nRowHeight, TRUE);	// 滚到最后1行
				m_nCurSelectedRow = m_nTotalTextRow-1;
			}
			return TRUE;
		case VK_F2:	// 激活编辑当前行
			{
				if( !P_VALID(m_nCurSelectedRow) || m_nCurSelectedRow < m_nCurrentTextRow 
					|| m_nCurSelectedRow > m_nCurrentTextRow + (m_ptSize.y/m_nRowHeight) )
					break;	// 确定当前选择行在视野内

				INT nY = (m_nCurSelectedRow - m_nCurrentTextRow) * m_nRowHeight;
				for(INT nCol=0, nX=0; nCol<m_mapWidth.Size(); nCol++)
				{
					INT nCurWidth = m_mapWidth.Peek(nCol);
					// 最后一列的宽度可能大于设定宽度
					if( nCol == m_mapWidth.Size() - 1 && nX+nCurWidth < m_ptSize.x )
						nCurWidth += m_ptSize.x - nX + nCurWidth;	

					DWORD dwPos = MAKELONG(m_nCurSelectedRow, nCol);
					tagGUIListItem* pItem = m_mapItem.Peek(dwPos);
					if( P_VALID(pItem) && pItem->bEditable )	// 单元格非法或不可编辑
					{
						m_nCurEditRow = m_nCurSelectedRow;
						m_nCurEditCol = nCol;

						tagPoint ptClient(nX+1, nY+1), ptSize(nCurWidth-1, m_nRowHeight-1);
						if( nX+nCurWidth > m_ptSize.x )
							ptSize.x -= nX + nCurWidth - m_ptSize.x;

						// 激活内部的编辑器
						ActiveEditor(m_nCurSelectedRow, nCol, ptClient, ptSize);
						return TRUE;
					}

					nX+=nCurWidth;
				}
			}
			return TRUE;
		case VK_RETURN:	// 相当于鼠标单击
			{
				if( !P_VALID(m_nCurSelectedRow) || m_nCurSelectedRow < m_nCurrentTextRow 
					|| m_nCurSelectedRow > m_nCurrentTextRow + (m_ptSize.y/m_nRowHeight) )
					break;	// 确定当前选择行在视野内

				m_nCurSelectedRow = m_nCurSelectedRow;
				m_nCurSelectedRowWhenMouseDown = m_nCurSelectedRow;

				tagGUIEvent event(this, EGUIE_ItemClick, pMsg);
				event.dwParam1 = m_nCurSelectedRow;
				SendEvent(&event);
			}
		}
		break;
	}

	return GUIStatic::OnInputMessage(pMsg);
}


//-----------------------------------------------------------------------------
// 处理子控件发的事件
//-----------------------------------------------------------------------------
VOID GUIListBox::SendEvent(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pSystem->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return;

	switch( pEvent->eEvent ) 
	{
	case EGUIE_Scroll:
		if( pWnd != this )
			SetCurrentTextRow(pEvent->dwParam1, FALSE);
		break;

	case EGUIE_EditBoxEnter:
		if( pWnd == m_pEditBox )
			m_pSystem->SetActive(this);	// 见下面的消息处理
		if( P_VALID(m_pComboBox) && pWnd == m_pComboBox )
			m_pSystem->SetActive(this);	// 见下面的消息处理
		break;

	case EGUIE_Deactive:
		if( pWnd == m_pEditBox )
		{
			SetText(m_nCurEditRow, m_nCurEditCol, m_pEditBox->GetText());
			m_pEditBox->SetInvisible(true);

			tagGUIEvent event(this, EGUIE_UserChangeListContext, NULL);	// 用户改变List内容
			event.dwParam1 = m_nCurEditRow;
			event.dwParam2 = m_nCurEditCol;
			SendEvent(&event);
		}
		if( P_VALID(m_pComboBox) && pWnd == m_pComboBox )
		{
			SetText(m_nCurEditRow, m_nCurEditCol, m_pComboBox->GetEditBox()->GetText());
			m_pComboBox->SetInvisible(true);

			tagGUIEvent event(this, EGUIE_UserChangeListContext, NULL);	// 用户改变List内容
			event.dwParam1 = m_nCurEditRow;
			event.dwParam2 = m_nCurEditCol;
			SendEvent(&event);
		}
		SetRefresh(TRUE);
		break;
	}

	GUIWnd::SendEvent(pEvent);
}


//-----------------------------------------------------------------------------
// 添加子窗口
//-----------------------------------------------------------------------------
VOID GUIListBox::AddChild(GUIWnd* pChild)
{
	GUIWnd::AddChild(pChild);

	if( pChild->GetClassName() == _T("CGUIEditBox") )	// 编辑框
	{
		m_pEditBox = (GUIEditBox*)pChild;
		m_pEditBox->SetInvisible(true);
	}

	if( pChild->GetClassName() == _T("CGUIComboBox") )	// 组合框
	{
		m_pComboBox = (GUIComboBox*)pChild;
		m_pComboBox->SetInvisible(true);
	}

	// 子控件可能是滚动条，让滚动条刷新
	SetCurrentTextRow(m_nCurrentTextRow); 
}

//-----------------------------------------------------------------------------
// 删除子窗口
//-----------------------------------------------------------------------------
VOID GUIListBox::DelChild(GUIWnd* pChild)
{
	if( m_pEditBox == pChild )
		m_pEditBox = NULL;
	if( m_pComboBox == pChild )
		m_pComboBox = NULL;

	GUIWnd::DelChild(pChild);
}


//-----------------------------------------------------------------------------
// 激活内部的编辑器
//-----------------------------------------------------------------------------
VOID GUIListBox::ActiveEditor(INT nRow, INT nCol, tagPoint& ptPos, tagPoint& ptSize)
{
	DWORD dwPos = MAKELONG(nRow, nCol);
	tagGUIListItem* pItem = m_mapItem.Peek(dwPos);
	if( !P_VALID(pItem) || !pItem->bEditable )	// 单元格非法或不可编辑
		return;

	if( !pItem->listOptionText.empty())
	{
		if( P_VALID(m_pComboBox) )
		{
			// 设置EditBox
			m_pComboBox->SetInvisible(false);
			m_pSystem->SetActive(m_pComboBox);

			m_pComboBox->SetResizable(true);
			m_pComboBox->SetMovable(true);
			tagPoint ptClient = ptPos - m_pComboBox->GetClient();
			m_pComboBox->Resize(ptSize);
			m_pComboBox->Move(ptClient);
			m_pComboBox->SetMovable(false);
			m_pComboBox->SetResizable(false);

			m_pComboBox->GetEditBox()->SetText(pItem->strText.c_str());
			GUIListBox* pListBox = m_pComboBox->GetListBox();
			if( P_VALID(pListBox) )
			{
				pListBox->SetInvisible(false);
				pListBox->SetMovable(true);
				pListBox->SetResizable(true);
				tagPoint ptListSize(pListBox->GetSize());
				ptListSize.y = 16*pItem->listOptionText.size();
				tagPoint ptListPos(0, -ptListSize.y);
				tagPoint ptClient = ptListPos - pListBox->GetClient();
				pListBox->Resize(ptListSize);
				pListBox->Move(ptClient);
				pListBox->SetMovable(false);
				pListBox->SetResizable(false);

				pListBox->Clear();
				pListBox->SetColNum(1, 100);
				std::list<tstring>::iterator it = pItem->listOptionText.begin();
				for(INT n=0;it!=pItem->listOptionText.end(); ++it, ++n)
					pListBox->SetText(n, 0, (*it).c_str());
				m_pSystem->SetActive(pListBox);
			}
		}
	}
	else
	{
		if( P_VALID(m_pEditBox) )
		{
			// 设置EditBox
			m_pEditBox->SetInvisible(false);
			m_pEditBox->SetResizable(true);
			m_pEditBox->SetMovable(true);
			tagPoint ptClient = ptPos - m_pEditBox->GetClient();
			m_pEditBox->Resize(ptSize);
			m_pEditBox->Move(ptClient);
			m_pEditBox->SetMovable(false);
			m_pEditBox->SetResizable(false);
			m_pEditBox->SetText(pItem->strText.c_str());
			m_pSystem->SetActive(m_pEditBox);

			if( m_bOpenFileDialog || m_bOpenColorDialog )
			{
				DWORD dwPos0 = MAKELONG(nRow, 0);
				tagGUIListItem* pItem0 = m_mapItem.Peek(dwPos0);
				if( pItem0->strText.find(_T("FileName")) != std::string::npos ) 
				{
					TCHAR mess[MAX_PATH];
					lstrcpy(mess, pItem->strText.c_str());
					OPENFILENAME file;
					ZeroMemory(&file, sizeof(file));
					file.lStructSize = sizeof(file);
					file.hwndOwner = (HWND)m_pGlobeVar->GetDword(_T("HWND"));

					file.Flags = OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST 
						| OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
					file.lpstrFilter = _T("image files\0*.bmp;*.jpg\0\0");
					file.lpstrFile = mess;
					file.nMaxFile = MAX_PATH;
					file.lpstrTitle = _T("SelectPicFile");
					if( ::GetOpenFileName(&file) )
					{
						SetDefaultDir();
						// 看是否能寻找出相对路径
						TCHAR szDir[MAX_PATH];
						::GetCurrentDirectory(MAX_PATH, szDir);
						tstring strSelect(mess);
						tstring strDir(szDir);
						std::transform(strSelect.begin(), strSelect.end(), strSelect.begin(), tolower);
						std::transform(strDir.begin(), strDir.end(), strDir.begin(), tolower);
						if( strSelect.find(strDir) != tstring::npos 
							&& strSelect.size() > strDir.size()+1 )
						{
							strSelect = strSelect.substr(strDir.size()+1, strSelect.size() - strDir.size()-1);
						}

						m_pEditBox->SetText(strSelect.c_str());
					}
				}

				if( pItem0->strText.find(_T("Color")) != std::string::npos ) 
				{
					DWORD dwAlpha = _tcstoul(pItem->strText.c_str(), NULL, 16) & 0xff000000;
					COLORREF temp[16]={0};
					CHOOSECOLOR color;
					ZeroMemory(&color, sizeof(color));
					color.lStructSize = sizeof(color);
					color.hwndOwner = (HWND)m_pGlobeVar->GetDword(_T("HWND"));
					color.Flags = CC_RGBINIT | CC_FULLOPEN | CC_ANYCOLOR | CC_PREVENTFULLOPEN;
					color.rgbResult = SwapColor(_tcstoul(pItem->strText.c_str(), NULL, 16));
					color.lpCustColors = temp;
					if( ::ChooseColor(&color) )
					{
						TCHAR szMess[X_LONG_STRING];
						wsprintf(szMess, _T("0x%08X"), SwapColor(color.rgbResult)|dwAlpha);
						m_pEditBox->SetText(szMess);
					}
				}
			}
		}
	}

}



}	// namespace vEngine {
