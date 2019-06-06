#include "stdafx.h"
#include "IconStatic.h"
#include "GameClientDefine.h"

IconStatic::IconStatic( void )
{

}

IconStatic::~IconStatic( void )
{

}

BOOL IconStatic::Init( GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement )
{
	BOOL bRet = GUIStatic::Init(pSystem, pFather, pXmlElement);
	return bRet;
}

VOID IconStatic::Destroy()
{
	GUIStatic::Destroy();
}

BOOL IconStatic::OnInputMessage( tagGUIInputMsg* pMsg )
{
	switch( pMsg->dwType )
	{
	case WM_LBUTTONDOWN:
		{
			tagGUIEvent event(this, EGUIE_Drag, pMsg);
			SendEvent(&event);
		}
		break;
	case WM_RBUTTONUP:
		{
			tagGUIEvent event(this, EGUIE_Click);
			SendEvent(&event);
		}
		break;
	case WM_MOUSEMOVE:
		{
			
		}
		break;

	default:
		break;
	}

	return GUIStatic::OnInputMessage(pMsg);
}