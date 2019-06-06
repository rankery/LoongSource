#include "StdAfx.h"
#include "ItemMenuButton.h"
#include "GameClientDefine.h"

ItemMenuButton::ItemMenuButton(void)
{
}

ItemMenuButton::~ItemMenuButton(void)
{
}

VOID ItemMenuButton::OnEvent( tagGUIEvent* pEvent )
{
	if(pEvent->eEvent == EGUISCB_Drop)
	{
		tagPoint pt ( LOWORD( pEvent->dwParam4 ), HIWORD( pEvent->dwParam4 ) );
		if(this->PointInWnd(pt))
		{
			tagGUIEvent event = tagGUIEvent(this, (EGUIEvent)EGUISCB_Drop);
			event.dwParam1 = pEvent->dwParam1;
			event.dwParam2 = pEvent->dwParam2;
			event.dwParam3 = pEvent->dwParam3;
			event.dwParam4 = pEvent->dwParam4;
			SendEvent(&event);
		}
	}
	GUIPushButton::OnEvent(pEvent);
}