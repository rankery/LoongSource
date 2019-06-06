#include "StdAfx.h"
#include "GroundItemTipsFrame.h"
#include "GroundItemMgr.h"
#include "MapMgr.h"
#include "ItemProtoData.h"
#include "WorldPickerFrame.h"

GroundItemTipsFrame::GroundItemTipsFrame(void) : 
m_pFont( NULL )
{
}

GroundItemTipsFrame::~GroundItemTipsFrame(void)
{
}

BOOL GroundItemTipsFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	if( !GameFrame::Init(pMgr,pFather,dwParam) )
		return FALSE;

	m_pFont = Device()->NewDepthFont();
	m_pFont->Create( 8, 16, 0, FALSE, g_StrTable[_T("Font_Kaiti")]);

	return TRUE;
}

BOOL GroundItemTipsFrame::Destroy()
{
	SAFE_DELETE(m_pFont);

	return GameFrame::Destroy();
}

VOID GroundItemTipsFrame::Update()
{
}

VOID GroundItemTipsFrame::Render3D()
{
	if( !m_pInput->IsKeyDown( DIK_LALT ) )
	{
		WorldPickerFrame* pWp = GetWorldPickerFrm();
		if( !P_VALID( pWp ) )
			return;

		tagGroundItem* pMousPointItem = pWp->GetCurMousePointGroundItem();
		if( !P_VALID(pMousPointItem) )
			return;

		m_pFont->Begin();
		DrawItemTip( pMousPointItem );
		m_pFont->End();
	}
	else
	{
		SceneGraph* pSG = MapMgr::Inst()->GetSceneGraph();
		if( NULL == pSG )
			return;
		CameraBase* pCam = pSG->GetCamera();
		if( NULL == pCam )
			return;
		Frustum frustum;
		frustum.Extract( pCam->GetView(), pCam->GetPrj() );

		TMap<INT64, tagGroundItem*>& mapItem = GroundItemMgr::Inst()->GetItemMap();
		tagGroundItem* pItem = NULL;
		mapItem.ResetIterator();

		m_pFont->Begin();
		while(mapItem.PeekNext(pItem))
		{
			if( !P_VALID(pItem) || !P_VALID(pItem->pNode) )
				continue;
			if( !pItem->pNode->IsInFrustum( &frustum ) )
				continue;

			DrawItemTip( pItem );
		}
		m_pFont->End();
	}
}

VOID GroundItemTipsFrame::DrawItemTip( const tagGroundItem* pItem )
{
	if( !P_VALID(pItem) || !P_VALID(pItem->pNode) )
		return;

	tstring strText;
	const tagItemProto* pProto = NULL;
	DWORD dwTextColor = 0xFFffff20;
	if( TYPE_ID_MONEY == pItem->dwTypeID )	// 金钱只用显示数量
	{
		tstringstream stream;
		stream << pItem->nNum << g_StrTable[_T("Dak_Silver")];
		strText = stream.str();
	}
	else
	{
		pProto = ItemProtoData::Inst()->FindItemProto( pItem->dwTypeID );
		if( !P_VALID( pProto ) )
			return;
		if( _T('\0') == pProto->pDisplayInfo->szName[0] )
			return;
		strText = pProto->pDisplayInfo->szName;
		switch( pProto->byQuality )
		{
		case EIQ_White:		dwTextColor = 0xFFFFFFFF; break;
		case EIQ_Yellow:	dwTextColor = 0xFFFFFF00; break;
		case EIQ_Green:		dwTextColor = 0xFF00FF00; break;
		case EIQ_Blue:		dwTextColor = 0xFF0000FF; break;
		case EIQ_Orange:	dwTextColor = 0xFFFF8000; break;
		case EIQ_Purple:	dwTextColor = 0xFF9932CD; break;
		}
	}

	AABBox box;
	pItem->pNode->GetBox( box );
	Vector3 vTipPos = pItem->space.GetPos();
	vTipPos.y += box.max.y - box.min.y;

	DrawTip( vTipPos, strText.c_str(), dwTextColor );
}

VOID GroundItemTipsFrame::DrawTip( const Vector3& vPos, const TCHAR* szText, const DWORD dwColor  )
{
	SIZE sizeText;
	m_pFont->MeasureText( szText, sizeText );

	CameraBase* pCam = MapMgr::Inst()->GetSceneGraph()->GetCamera();
	Vector3 vScreenPos = pCam->WorldToScreenF( vPos );
	POINT ptPos = pCam->ScreenFToScreen( vScreenPos );

	RECT rcText;
	rcText.left = ptPos.x - sizeText.cx / 2;
	rcText.top = ptPos.y - sizeText.cy * 2;
	rcText.right = rcText.left + sizeText.cx;
	rcText.bottom = rcText.top + sizeText.cy;
	
	// z scale!!
	const float z = vScreenPos.z * 0.5f;
	// 增加字体阴影
	RECT rcShade;
	rcShade = rcText;
	rcShade.left-=2;
	m_pFont->DrawText(szText, (LPRECT)&rcShade, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, z);
	rcShade = rcText;
	rcShade.right+=2;
	m_pFont->DrawText(szText, (LPRECT)&rcShade, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, z);
	rcShade = rcText;
	rcShade.top-=1;
	m_pFont->DrawText(szText, (LPRECT)&rcShade, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, z);
	rcShade = rcText;
	rcShade.bottom+=1;
	m_pFont->DrawText(szText, (LPRECT)&rcShade, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, z);
	// 显示物品名称
	m_pFont->DrawText( szText, &rcText, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, dwColor, z );

}