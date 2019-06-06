#include "StdAfx.h"
#include "LockStatic.h"
#include "..\WorldDefine\exchange_define.h"

LockStatic::LockStatic(void)
{
	m_pLastWnd = NULL;
}

LockStatic::~LockStatic(void)
{
}

BOOL LockStatic::OnInputMessage( tagGUIInputMsg* pMsg )
{
	switch(pMsg->dwType)
	{
	case WM_MOUSEMOVE:	// 处理鼠标移动
		{
			if( m_pSystem->GetMousePoint() == this )
			{
				//如果是鼠标移动，发送消息到该控件下面的物品按钮控件
				for (INT i=0; i<MAX_EXCHANGE_ITEM; ++i)
				{
					TCHAR szTmp[X_SHORT_NAME] = {0};
					_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("itempic%d\\item%d"), i+1, i+1);
					GUIWnd* pWnd = m_pFather->GetChild(szTmp);
					if (P_VALID(pWnd) && pWnd->PointInWnd(pMsg->pt))
					{
						if (pWnd != m_pLastWnd)
						{
							//如果上一个控件存在，则先发出移出事件
							if(P_VALID(m_pLastWnd))
							{
								tagGUIEvent e1(m_pLastWnd, EGUIE_MoveOutOfCtrl, pMsg);
								m_pLastWnd->SendEvent(&e1);
							}
							
							//发送移入控件并记录
							tagGUIEvent e2(pWnd, EGUIE_MoveIntoCtrl, pMsg);
							pWnd->SendEvent(&e2);
							m_pLastWnd = pWnd;
							
							break;
						}
					}
				}
			}
			else
			{
				m_pLastWnd = NULL;
			}
		}
		break;
	}
	return GUIStatic::OnInputMessage(pMsg);
}
