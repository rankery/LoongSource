#include "stdafx.h"
#include "StaticForCast.h"
StaticForCast::StaticForCast( void )
{

}

StaticForCast::~StaticForCast( void )
{

}

BOOL StaticForCast::Init( GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement )
{
	BOOL bRet = GUIStatic::Init(pSystem, pFather, pXmlElement);
	return bRet;
}

VOID StaticForCast::Destroy()
{
	GUIStatic::Destroy();
}

BOOL StaticForCast::OnInputMessage( tagGUIInputMsg* pMsg )
{
	switch(pMsg->dwType)
	{
	case WM_RBUTTONDOWN:			// ÓÒ¼üµ¥»÷¼¤»î´°¿Ú
		{
			if( m_bUnClickable )
				break;
			tagGUIEvent event(this,(EGUIEvent)(EXUIE_SFCMouseClick));
			event.msg = *pMsg;
			SendEvent(&event);
			this->SetRefresh();	
		}
		break;
	case WM_LBUTTONDBLCLK:
		{
			if( m_bUnClickable )
				break;
			tagGUIEvent event(this,(EGUIEvent)(EXUIE_SFCMouseRClick));
			event.msg = *pMsg;
			SendEvent(&event);
			this->SetRefresh();	
		}
		break;
	}
	return GUIStatic::OnInputMessage(pMsg);
}