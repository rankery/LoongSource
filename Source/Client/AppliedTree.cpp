/********************************************************************
	created:	2008/08/29
	created:	29:8:2008   17:20
	filename: 	d:\loong_client\Client\AppliedTree.cpp
	file path:	d:\loong_client\Client
	file base:	AppliedTree
	file ext:	cpp
	author:		leili
	
	purpose:	适用于任务主界面的Tree控件
*********************************************************************/
#include "StdAfx.h"
#include "AppliedTree.h"

namespace
{
	class SortList : public less<tagGUITreeItem*>
	{
	public:
		bool operator()(const tagGUITreeItem* _Left,const tagGUITreeItem* _Right) const
		{
			tstring _LeftLvl = _Left->strText;
			tstring _RightLvl = _Right->strText;

			tstring::size_type pos(0);
			if( (pos=_LeftLvl.find(_T("_"))) != tstring::npos )
			{
				_LeftLvl.replace(0,pos+1,_T(""));
			}
			if( (pos=_RightLvl.find(_T("_"))) != tstring::npos )
			{
				_RightLvl.replace(0,pos+1,_T(""));
			}
			if( _tstoi(_LeftLvl.c_str()) < _tstoi(_RightLvl.c_str()) )
				return true;

			return false;
		}
	};
}

AppliedTree::AppliedTree(void)
{
	m_eState = EAppliedTree_Operate;
}

//-----------------------------------------------------------------------------
// 渲染
//-----------------------------------------------------------------------------
VOID AppliedTree::Render()
{
	if( !IsRefresh() )
		return GUIWnd::Render();

	// 渲染底图
	m_pCanvas->rc.SetLTWH(m_ptView, m_ptSize);
	m_pRender->Draw(m_pCanvas, m_pPic, m_dwPicColor, m_eAlphaWrite);

	m_listVisibleItem.Clear();

	INT nCurRow = 0, nCurY = 0;
	if( P_VALID(m_pRootItem) )	// 是否有根控件
		Render(m_pRootItem, nCurRow, nCurY);

	// 调用基类渲染
	GUIWnd::Render();
}

//-----------------------------------------------------------------------------
// 渲染辅助函数，不渲染根节点和bag图像
//-----------------------------------------------------------------------------
VOID AppliedTree::Render( tagGUITreeItem* pItem, INT& nCurRow, INT& nCurY )
{
	if( FALSE == P_VALID(pItem) )
		return;

	if( nCurY + m_nRowHeight >= m_ptSize.y )
		return;

	if( !P_VALID(m_pPlusPic) || !P_VALID(m_pMinusPic) )
		return;

	// 不渲染根节点
	if( pItem->nLayer != 0 && nCurRow >= m_nCurrentTextRow )
	{
		INT nOffset = m_pPlusPic->ptSize.x + m_dwPicInterval;
		if( !pItem->listChildren.empty() )
		{
			m_pCanvas->rc.Set(m_ptView.x + m_rcText.left + (pItem->nLayer-1)*2, 
				m_ptView.y + nCurY + (m_nRowHeight - m_pPlusPic->ptSize.y)/2, 
				m_ptView.x + m_rcText.left + (pItem->nLayer-1)*2 + m_pPlusPic->ptSize.x, 
				m_ptView.y + nCurY + (m_nRowHeight - m_pPlusPic->ptSize.y)/2 + m_pPlusPic->ptSize.y);
			if( !pItem->bExpended )
				m_pRender->Draw(m_pCanvas, m_pPlusPic, m_dwPlusPicColor, m_eAlphaWrite);
			else
				m_pRender->Draw(m_pCanvas, m_pMinusPic, m_dwMinusPicColor, m_eAlphaWrite);
			pItem->rcPlus = m_pCanvas->rc;
		}
		else
			pItem->rcPlus.Clear();

		m_pCanvas->rc.Set(m_ptView.x + m_rcText.left + (pItem->nLayer-1)*2 + nOffset, 
			m_ptView.y + nCurY + m_rcText.top, m_ptView.x + m_rcText.right, m_ptView.y + nCurY + m_nRowHeight);
		if( m_pCanvas->rc.bottom > m_ptView.y + m_rcText.bottom )
			m_pCanvas->rc.bottom = m_ptView.y + m_rcText.bottom;

		//if( m_pCanvas->rc.left + pItem->nTextWidth + 1 < m_pCanvas->rc.right )
		//	m_pCanvas->rc.right = m_pCanvas->rc.left + pItem->nTextWidth + 1;

		if( m_pCurSelItem == pItem )	// 渲染选中
		{
			// 反色(Alpha值不变)
			DWORD dwColor = ((~pItem->dwColor)&0x00ffffff)|(pItem->dwColor&0xff000000);
			DWORD dwShadeColor = ((~m_dwShadeColor)&0x00ffffff)|(m_dwShadeColor&0xff000000); 
			m_pRender->Draw(m_pCanvas, NULL, pItem->dwColor, m_eAlphaWrite);
			m_pRender->Text(m_pCanvas, pItem->strShow, m_pFont, dwColor, dwShadeColor, m_eTextAlign, m_bTextWriteAlpha);
		}
		else
		{
			m_pRender->Text(m_pCanvas, pItem->strShow, m_pFont, pItem->dwColor, m_dwShadeColor, m_eTextAlign, m_bTextWriteAlpha);
		}

		nCurY+=m_nRowHeight;
		pItem->rcText = m_pCanvas->rc;
		m_listVisibleItem.PushBack(pItem);
	}
	nCurRow++;

	if( pItem->bExpended )
	{
		std::list<tagGUITreeItem*>::iterator it;
		for(it=pItem->listChildren.begin(); it!=pItem->listChildren.end(); it++)
			Render((*it), nCurRow, nCurY);
	}
}

//-----------------------------------------------------------------------------
// 处理输入消息，不能选中父节点，双击父节点会展开或者关闭子节点列表
//-----------------------------------------------------------------------------
BOOL AppliedTree::OnInputMessage( tagGUIInputMsg* pMsg )
{
	if(m_eState==EAppliedTree_NoOperate)
		return GUIStatic::OnInputMessage(pMsg);

	switch( pMsg->dwType )
	{
	case WM_LBUTTONDOWN:	// 选中
	case WM_LBUTTONDBLCLK:
		if( PointInWnd(pMsg->pt) )
		{
			tagGUITreeItem* pItem;
			m_listVisibleItem.ResetIterator();
			while(m_listVisibleItem.PeekNext(pItem))
			{
				if( pItem->rcText.IsContain(pMsg->pt) )
				{
					if( WM_LBUTTONDBLCLK == pMsg->dwType )
						pItem->bExpended = !pItem->bExpended;
					m_nTotalRow = CalItemTakeupRow(m_pRootItem);
					SetCurrentTextRow(m_nCurrentTextRow);	// 让滚动条刷新

					if( pItem->listChildren.empty() )
					{
						m_pCurSelItem = pItem;

						// 发送事件
						tagGUIEvent event(this, EGUIE_ItemClick, pMsg);
						if( WM_LBUTTONDBLCLK == pMsg->dwType )
							event.eEvent = EGUIE_ItemDblClick;
						event.dwParam1 = m_pUtil->Crc32(pItem->strName.c_str());
						SendEvent(&event);
						break;
					}
				}

				if( pItem->rcPlus.IsContain(pMsg->pt) )
				{
					pItem->bExpended = !pItem->bExpended;
					m_nTotalRow = CalItemTakeupRow(m_pRootItem);
					SetCurrentTextRow(m_nCurrentTextRow);	// 让滚动条刷新
					break;
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
		switch(pMsg->dwParam1)
		{
		case VK_UP:
			SetCurrentTextRow(m_nCurrentTextRow-1);
			return TRUE;
		case VK_DOWN:
			SetCurrentTextRow(m_nCurrentTextRow+1);
			return TRUE;
		case VK_PRIOR:	// 向前翻页
			if( FALSE == SetCurrentTextRow(m_nCurrentTextRow-m_ptSize.y / m_nRowHeight) )
				SetCurrentTextRow(0);
			return TRUE;
		case VK_NEXT:	// 向后翻页
			if( FALSE == SetCurrentTextRow(m_nCurrentTextRow+m_ptSize.y / m_nRowHeight) )
				SetCurrentTextRow(m_nTotalRow - m_ptSize.y / m_nRowHeight);	// 滚到最后1行
			return TRUE;
		}
		break;
	}

	return GUIStatic::OnInputMessage(pMsg);
}

//-----------------------------------------------------------------------------
// 删除某节点，根据ID删除
//-----------------------------------------------------------------------------
BOOL AppliedTree::RemoveItem( DWORD itemID )
{
	// 检查节点是否存在
	tagGUITreeItem *pItem = m_mapItem.Peek(itemID);
	if( FALSE == P_VALID(pItem) )
		return FALSE;

	GUITree::SetRefresh();
	pItem = GetEnableDelItem(pItem);	// 回溯到最上级可以删除的节点
	tagGUITreeItem *pFatherItem = pItem->pFather;
	tagGUITreeItem *pNextSelItem = NULL;
	if( P_VALID(pFatherItem) )
	{
		std::list<tagGUITreeItem*>::iterator iterItem;
		std::list<tagGUITreeItem*>::iterator nextIter;
		for( iterItem=pFatherItem->listChildren.begin(); iterItem!=pFatherItem->listChildren.end(); ++iterItem )
		{
			if( *iterItem == pItem )
			{
				nextIter = iterItem;
				++nextIter;
				if( nextIter != pFatherItem->listChildren.end() )
				{
					while( (*nextIter)->listChildren.size() != 0 )
					{
						nextIter = (*nextIter)->listChildren.begin();
					}
				}
				else
				{
					--nextIter;
					--nextIter;
					while( (*nextIter)->listChildren.size() != 0 )
					{
						nextIter = (*nextIter)->listChildren.end();
						--nextIter;
					}
				}

				pNextSelItem = *nextIter;
			}
		}
	}

	GUITree::RemoveItem(pItem->strName.c_str());

	if( pItem == m_pRootItem )
		m_pRootItem = NULL;

	if( P_VALID(m_pRootItem) )
		m_nTotalRow = GUITree::CalItemTakeupRow(m_pRootItem); // 总行数
	else
		m_nTotalRow = 0;

	if( !P_VALID(m_pCurSelItem) )
	{
		m_pCurSelItem = pNextSelItem;
	}

	// 让滚动条刷新
	SetCurrentTextRow(m_nCurrentTextRow);
	return TRUE;
}

//-----------------------------------------------------------------------------
// 向上级查找可以删除的节点
//-----------------------------------------------------------------------------
tagGUITreeItem* AppliedTree::GetEnableDelItem( tagGUITreeItem *pChild )
{
	tagGUITreeItem *pFather = pChild->pFather;
	if( P_VALID(pFather) )
	{
		if( pFather->listChildren.size() == 1 )
		{
			pFather->listChildren.remove(pChild);
			GUITree::RemoveItem(pChild);

			return GetEnableDelItem( pFather );
		}
	}

	return pChild;
}

//-----------------------------------------------------------------------------
// 插入某节点
//-----------------------------------------------------------------------------
DWORD AppliedTree::InsterItem( LPCTSTR szFatherName, LPCTSTR szText, LPCTSTR szShow, BOOL bSort, DWORD dwColor/*=0xFFFFFF*/ )
{
	DWORD itemID = GUITree::InsterItem(szFatherName, szText, szShow);

	if( bSort == TRUE )
	{
		if( P_VALID(itemID) && P_VALID(szFatherName) )
		{
			DWORD dwCrc32 = m_pUtil->Crc32(szFatherName);
			tagGUITreeItem* pFather = m_mapItem.Peek(dwCrc32);
			pFather->listChildren.sort(SortList());
		}
	}

	if( dwColor!=0xFFFFFF )
	{
		tagGUITreeItem* pItem = GetItem(itemID);

		if (P_VALID(pItem))
			pItem->dwColor = dwColor;
	}

	return itemID;
}

void AppliedTree::RefreshScrollBar()
{
	m_nTotalRow = CalItemTakeupRow(m_pRootItem);
	SetCurrentTextRow(m_nCurrentTextRow);	// 让滚动条刷新

}