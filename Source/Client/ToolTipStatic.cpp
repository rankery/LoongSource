#include "stdafx.h"
#include "ToolTipStatic.h"


ToolTipStatic::ToolTipStatic()
{
}

VOID ToolTipStatic::SetText(LPCTSTR szText)
{
	m_strText = szText;
	
	PreParseText();
}

BOOL ToolTipStatic::Resize(tagPoint& ptSize)
{
	BOOL bResult = RecalculateSize(ptSize);
	
	SetRefresh();
	
	return bResult;
}

VOID ToolTipStatic::PreParseText()
{
	this->ClearResource();

	if( m_strText.empty() )
		return;

	tagStaticExItem* pItem = new tagStaticExItem;
	pItem->dwColor = m_dwTextColor;
	pItem->pFont = m_pFont;
	pItem->pImage = NULL;
	pItem->ptPos.x = m_rcText.left;
	pItem->ptPos.y = m_rcText.top;
	m_listItems.push_back(pItem);

	UINT nIndex = 0;
	INT nLargeHeight = 0;
	while( nIndex < m_strText.size() )
	{
		if( m_strText[nIndex] == _T('<') )
		{
			INT n =  m_strText.find_first_of(_T('>'), nIndex);
			if( n == tstring::npos || n - nIndex <= 1 )
				break;	// Óï·¨´íÎó

			tstring str = m_strText.substr(nIndex+1, n-nIndex-1);
			n =  str.find_first_of(_T('='));
			if( n == tstring::npos || n <= 0 || n>= (INT)str.size() )
				break;	// Óï·¨´íÎó

			tstring strName = str.substr(0, n);
			tstring strValue = str.substr(n+1, str.size() - n);

			tagStaticExItem* pNewItem = new tagStaticExItem;
			pNewItem->dwColor = pItem->dwColor;
			pNewItem->pFont = pItem->pFont;
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
					if( pNewItem->pImage->ptSize.y > pNewItem->ptSize.y )
						pNewItem->ptSize = pNewItem->pImage->ptSize;
					else
						pNewItem->ptSize.x = pNewItem->pImage->ptSize.x;
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
			pItem->ptSize.x += ptSize.x;
			if( ptSize.y > pItem->ptSize.y )
				pItem->ptSize.y = ptSize.y;
			
			pItem->strText += m_strText[nIndex];
			nIndex++;
		}
	}
}

BOOL ToolTipStatic::RecalculateSize(tagPoint& ptSize)
{
	BOOL bResult;

	INT nWidth,nHeight,nCurWidth;
	nWidth = MIN_TIPS_WIDTH;
	nCurWidth = 0;
	
	std::list<tagStaticExItem*>::iterator it = m_listItems.begin();
	tagStaticExItem* pItem = *it;
	nHeight = pItem->ptPos.y;
	do
	{
		pItem = *it;
		if(nHeight == pItem->ptPos.y)
		{
			nCurWidth += pItem->ptSize.x;
		}
		else
		{
			nHeight = pItem->ptPos.y;
			nCurWidth = pItem->ptSize.x;
		}
	
		nWidth = nWidth >= nCurWidth ? nWidth : nCurWidth;
		nWidth = nWidth > MAX_TIPS_WIDTH ? MAX_TIPS_WIDTH : nWidth;
		++it;
	} while( it!=m_listItems.end() );
	
	ptSize.x = nWidth;
	ptSize.y = m_ptSize.y;
	
	bResult = GUIStaticEx::Resize(ptSize);

	for(it=m_listItems.begin(); it!=m_listItems.end(); ++it)
	{
		pItem = *it;
		nHeight = pItem->ptPos.y + pItem->ptSize.y;
	}
	
	ptSize.x = nWidth;
	ptSize.y = nHeight + m_nRowHeight;
	bResult = GUIStatic::Resize(ptSize);

	return bResult;
}

