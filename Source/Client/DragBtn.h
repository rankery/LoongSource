#pragma once
#include ".\GameClientDefine.h"

class DragBtn :
	public GUIStatic
{
public:
	DragBtn(void);
	~DragBtn(void);
	virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
	virtual BOOL OnInputMessage(tagGUIInputMsg* pMsg);
	virtual VOID OnEvent(tagGUIEvent* pEvent);					
	virtual VOID Render();
	
	//初使化一个实例
	static DragBtn* NewInstance(GUISystem *pGUISys);
	static void DestroyDragBtn();

	//移动拖动按钮
	VOID MoveToPos(INT x,INT y);

	//开始拖动
	VOID BeginDrage();

	//结束拖动
	VOID EndDrag();
	
	//是否在游戏窗口
	bool PointInScene(tagPoint& pt);

	//清除拖动按钮记录的数据
	void Clear(){ m_pSrcWnd = NULL; m_eDragType = EDT_Null; m_dwParam1 = m_dwParam2 = m_dwParam3 = m_dwParam4 = 0; }

private:	

	EDragType			m_eDragType;							//拖动类型
	GUIWnd*				m_pSrcWnd;								//原控件
	DWORD				m_dwSrcWndID;							//原控件的ID

	DWORD				m_dwParam1;								//参数1
	DWORD				m_dwParam2;								//参数2
	DWORD				m_dwParam3;								//参数3
	DWORD				m_dwParam4;								//参数4

	int					m_nClickUp; //	MouseLeftKeyUp状态，用于控制拖动操作
									// 0:转入显示，刚被激活;  
									// 1:收到第一个clickup事件;
									// 2:收到第二次clickup事件;
};
