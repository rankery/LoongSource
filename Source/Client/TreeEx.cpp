#include "StdAfx.h"
#include "TreeEx.h"

TreeEx::TreeEx(void)
{
}

TreeEx::~TreeEx(void)
{
}

BOOL TreeEx::SetItemColor( DWORD dwID, DWORD dwColor )
{
	tagGUITreeItem* pItem = GetItem(dwID);
	
	if (P_VALID(pItem))
		pItem->dwColor = dwColor;
	else
		return FALSE;

	return TRUE;
}

BOOL TreeEx::SetShowText( DWORD dwID, LPCTSTR szShowText )
{
	tagGUITreeItem* pItem = GetItem(dwID);

	if (P_VALID(pItem))
	{
		pItem->strShow = szShowText;
		tagPoint ptFont;
		m_pRender->GetTextSize(szShowText, m_pFont, ptFont);
		pItem->nTextWidth = ptFont.x;
	}
	else
		return FALSE;

	return TRUE;
}
