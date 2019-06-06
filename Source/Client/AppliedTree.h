/********************************************************************
	created:	2008/08/29
	created:	29:8:2008   17:13
	filename: 	d:\loong_client\Client\AppliedTree.h
	file path:	d:\loong_client\Client
	file base:	AppliedTree
	file ext:	h
	author:		leili
	
	purpose:	适用于任务主界面的Tree控件，不渲染根节点，焦点只在叶子节点上，会自动删除没有儿子的父节点
*********************************************************************/
#pragma once

class AppliedTree : public GUITree
{
public:
	enum EAppliedTree
	{
		EAppliedTree_NoOperate = 1,			//不可操作
		EAppliedTree_Operate,				//可操作
	};
	AppliedTree(void);
	virtual VOID	Render(void);
	virtual BOOL	OnInputMessage(tagGUIInputMsg* pMsg);

	DWORD			InsterItem(LPCTSTR szFatherName, LPCTSTR szText, LPCTSTR szShow, BOOL bSort=FALSE, DWORD dwColor=0xFFFFFF);
	BOOL			RemoveItem(DWORD itemID);
	VOID			SetCurAppliedTreeState(EAppliedTree eType=EAppliedTree_Operate){  m_eState=eType; }
	VOID            RefreshScrollBar();
	tagGUITreeItem*	GetRoot(void) { return m_pRootItem; }

protected:
	VOID			Render(tagGUITreeItem* pItem, INT& nCurRow, INT& nCurY);
	tagGUITreeItem*	GetEnableDelItem(tagGUITreeItem *pItem);

private:
	EAppliedTree	m_eState;
};