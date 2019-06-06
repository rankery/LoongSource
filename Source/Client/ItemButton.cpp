#include "StdAfx.h"
#include "ItemButton.h"
#include "Item.h"
#include "ItemMgr.h"
#include "Render2D.h"
#include "GameClientDefine.h"
#include "CursorMgr.h"

#include "ItemProtoData.h"
#include "AudioSys.h"
ItemButton::ItemButton()
{
	m_pQuailityPic = NULL;
}

BOOL ItemButton::Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement)
{
	BOOL bRet = ButtonEx::Init(pSystem, pFather, pXmlElement);

	m_bTextWriteAlpha = TRUE;
	
	return bRet;
}

VOID ItemButton::RefreshItem(DWORD dwTypeID, INT nCount, BYTE byQuality)
{
	// 设置图标
	if (GT_INVALID == dwTypeID)// 没有图标，则隐藏按钮
	{
		SetWndPic(EWPICTYPE_CURPIC, _T("data\\ui\\Bag\\L-main-icon-lock.bmp"));
		SetPicColor(0xFFFFFF);
	}
	else if(0 == dwTypeID)
	{
		SetWndPic(EWPICTYPE_CURPIC, _T("data\\ui\\Bag\\L-main-icon-lock.bmp"));
	}
	else
	{
		SetWndPic(EWPICTYPE_CURPIC, ItemMgr::Inst()->GetItemIconPath(dwTypeID).c_str());
	}

	// 设置物品数量
	if (0 != nCount && 1 != nCount)
	{
		TCHAR szCount[X_SHORT_NAME];
		_sntprintf(szCount, sizeof(szCount)/sizeof(TCHAR), _T("%d"), nCount);
		SetText(szCount);
	}
	else 
	{
		SetText(_T(""));
	}
	SetQuality(byQuality);

	SetBtnData(ESBT_Item, nCount, dwTypeID, byQuality);
	
	SetCDTime(0.0f);
	SetRestoreRatio(0.0f);
	// 显示按钮
	SetInvisible(FALSE);
	SetRefresh(TRUE);
}

VOID ItemButton::Destroy()
{
	m_pRender->DestroyImage(m_pQuailityPic);
	ButtonEx::Destroy();
}

VOID ItemButton::Render()
{
	if( !IsRefresh() )
		return GUIWnd::Render();
	
	// 绘制按钮图标
	DrawImg();
	// 绘制品级图标
	DrawQuality();
	// 绘制冷却时间的显示
	DrawShadow(m_ptSize.x);
	// 绘制按钮文字
	DrawText();

	// 调用基类渲染
	GUIWnd::Render();
}

VOID ItemButton::Update()
{
	ButtonEx::Update();
}

VOID ItemButton::DrawText()
{
	// 渲染文字
	if( !m_strText.empty() )
	{
		m_pCanvas->rc = m_rcText + m_ptView;

		if( m_bDisable )
			m_pRender->Text(m_pCanvas, m_strText, m_pFont, m_dwDisableTextColor, m_dwDisableShadeColor, m_eTextAlign, false);
		else
		{
			switch(m_eState)
			{
			case EGUIBS_PushDown:
				m_pCanvas->rc = m_pCanvas->rc + 1;
				m_pRender->Text(m_pCanvas, m_strText, m_pFont, m_dwPushTextColor, m_dwPushShadeColor, m_eTextAlign, false);
				break;
			case EGUIBS_HighLight:
				m_pRender->Text(m_pCanvas, m_strText, m_pFont, m_dwMoveTextColor, m_dwMoveShadeColor, m_eTextAlign, false);
				break;
			default:
				m_pRender->Text(m_pCanvas, m_strText, m_pFont, m_dwTextColor, m_dwShadeColor, m_eTextAlign, false);
				break;
			}
		}
	}
}

BOOL ItemButton::OnInputMessage(tagGUIInputMsg* pMsg)
{
	if( m_bDisable )
		return ButtonEx::OnInputMessage(pMsg);

	switch( pMsg->dwType )
	{
	case WM_LBUTTONDOWN:// 鼠标左键操作为拖动
		if( m_pSystem->GetMousePoint() == this ) 
		{
			if(CursorMgr::Inst()->GetCursorState() == ECS_Idle)
			{
				tagGUIEvent event(this, EGUIE_Drag, pMsg);
				POINT pt;
				::GetCursorPos(&pt);
				event.dwParam4 = MAKELONG(pt.x, pt.y);
				SendEvent(&event);

				// 播放物品拿起声音
				this->PlaySound(m_BtnData.GetData2());
			}
			else
			{
				tagGUIEvent event(this, (EGUIEvent)EGUISCB_UseSpecFunc, pMsg);
				event.dwParam1 = CursorMgr::Inst()->GetCursorActive();
				event.dwParam2 = CursorMgr::Inst()->GetCursorActiveParam();
				SendEvent(&event);
				CursorMgr::Inst()->Change2IdleState();
			}
			//SetEnable(FALSE);
		}
		break;
	case WM_RBUTTONUP:// 鼠标右键操作为点击使用
		if( m_pSystem->GetMousePoint() == this )
		{
			if(CursorMgr::Inst()->GetCursorState() == ECS_Idle)
			{
				tagGUIEvent event(this, EGUIE_Click);
				SendEvent(&event);
			}
			else
			{
				CursorMgr::Inst()->Change2IdleState();
			}
		}
		break;
	case WM_MOUSEMOVE:
		{
			return GUIStatic::OnInputMessage(pMsg);
		}
		break;
	
	}

	return GUIStatic::OnInputMessage(pMsg);
}

VOID ItemButton::OnEvent(tagGUIEvent* pEvent)
{
	if(pEvent->eEvent == EGUISCB_Drop)
	{
		tagPoint pt ( LOWORD( pEvent->dwParam4 ), HIWORD( pEvent->dwParam4 ) );
        GUIWnd* pWnd = m_pSystem->GetDesktop()->GetWnd(pt);
		if(this->PointInWnd(pt) && P_VALID(pWnd) && this == pWnd)
		{
			tagGUIEvent event = tagGUIEvent(this, (EGUIEvent)EGUISCB_Drop);
			event.dwParam1 = pEvent->dwParam1;
			event.dwParam2 = pEvent->dwParam2;
			event.dwParam3 = pEvent->dwParam3;
			event.dwParam4 = pEvent->dwParam4;
			SendEvent(&event);

			// 播放物品放下声音
			this->PlaySound(pEvent->dwParam3);
		}
	}
}

VOID ItemButton::DrawQuality()
{
	if(P_VALID(m_pQuailityPic))
		m_pRender->Draw(m_pCanvas, m_pQuailityPic, m_dwPicColor, m_eAlphaWrite);
}

VOID ItemButton::SetQualityPic( tstring strPath )
{
	if(P_VALID(m_pQuailityPic))
	{
		m_pRender->DestroyImage(m_pQuailityPic);				// 删除背景图片
		m_pQuailityPic = NULL;
	}
	m_pQuailityPic = m_pRender->CreateImage(strPath, m_rcPic);		// 读入图片
}

VOID ItemButton::SetQuality( BYTE byQuality )
{
	// 设置物品品级
	if (1 == byQuality)
	{
		SetQualityPic(_T("data\\ui\\bag\\quality_yellow.bmp"));
	}
	else if (2 == byQuality)
	{
		SetQualityPic(_T("data\\ui\\bag\\quality_green.bmp"));
	}
	else if (3 == byQuality)
	{
		SetQualityPic(_T("data\\ui\\bag\\quality_blue.bmp"));
	}
	else if (4 == byQuality)
	{
		SetQualityPic(_T("data\\ui\\bag\\quality_orange.bmp"));
	}
	else if( 5 == byQuality )
	{
		SetQualityPic(_T("data\\ui\\bag\\quality_purple.dds"));
	}
	else
	{
		if(P_VALID(m_pQuailityPic))
		{
			m_pRender->DestroyImage(m_pQuailityPic);
			m_pQuailityPic = NULL;
		}
	}

	SetRefresh(TRUE);
}


// 播放物品拿起声音
VOID ItemButton::PlaySound(DWORD dwItemTypeID)
{
	const tagItemProto* pProto = ItemProtoData::Inst()->FindItemProto(dwItemTypeID);
	if( P_VALID(pProto) )
	{
		switch(pProto->eType)
		{
		case EIT_Weapon: // 武器
			TObjRef<AudioSys>()->Play2DSound(_T("takeup_weapon"), SOUND_WISPER);
			return;
				
		case EIT_ClothArmor: // 布甲 
			TObjRef<AudioSys>()->Play2DSound(_T("takeup_cloth"), SOUND_WISPER);
			return;

		case EIT_Armor: // 铠甲 
			TObjRef<AudioSys>()->Play2DSound(_T("takeup_armor"), SOUND_WISPER);
			return;
		}

		switch( pProto->eTypeReserved )
		{
		case 1:	// 药品
			TObjRef<AudioSys>()->Play2DSound(_T("takeup_drug"), SOUND_WISPER);
			return;
		}

		// 否则播放普通物品音效
		TObjRef<AudioSys>()->Play2DSound(_T("takeup_item"), SOUND_WISPER);
	}
}


GoodsButton::GoodsButton()
{
	m_pShangjiaPic	= NULL;
	
}


BOOL GoodsButton::Init( GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement )
{
	BOOL bRet = ItemButton::Init(pSystem, pFather, pXmlElement);

	return bRet;
}

VOID GoodsButton::Render()
{
	if( !IsRefresh() )
		return GUIWnd::Render();

	// 绘制按钮图标
	DrawImg();
	// 绘制品级图标
	DrawQuality();
	// 绘制冷却时间的显示
	DrawShadow(m_ptSize.x);
	// 绘制按钮文字
	DrawText();
	// 绘制是否上架
	DrawShangjia();

	// 调用基类渲染
	GUIWnd::Render();
}

VOID GoodsButton::Destroy()
{
	m_pRender->DestroyImage(m_pShangjiaPic);

	ItemButton::Destroy();
}

VOID GoodsButton::SetShangjia( bool bShangjia )
{
	if (bShangjia)
	{
		SetShangjiaPic();
	}
	else
	{
		m_pRender->DestroyImage(m_pShangjiaPic);
		m_pShangjiaPic = NULL;
	}
	SetRefresh(TRUE);
}

VOID GoodsButton::DrawShangjia()
{
	if (P_VALID(m_pShangjiaPic))
	{
		m_pRender->Draw(m_pCanvas, m_pShangjiaPic, m_dwPicColor, m_eAlphaWrite);
	}
}

VOID GoodsButton::SetShangjiaPic()
{
	if(P_VALID(m_pShangjiaPic))
	{
		m_pRender->DestroyImage(m_pShangjiaPic);				// 删除背景图片
		m_pShangjiaPic = NULL;
	}
	m_pShangjiaPic = m_pRender->CreateImage(_T("data\\ui\\Bag\\L_baitan-shangjia.bmp"), m_rcPic);		// 读入图片
}



MallButton::MallButton()
{
	m_pHotPic	= NULL;
	m_pNewPic	= NULL;
}

BOOL MallButton::Init( GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement )
{
	BOOL bRet = ItemButton::Init(pSystem, pFather, pXmlElement);

	return bRet;
}

VOID MallButton::Render()
{
	if( !IsRefresh() )
		return GUIWnd::Render();

	// 绘制按钮图标
	DrawImg();
	// 绘制品级图标
	DrawQuality();
	// 绘制冷却时间的显示
	DrawShadow(m_ptSize.x);
	// 绘制按钮文字
	DrawText();
	// 绘制是否是新品和新品
	DrawMallItemImg();

	// 调用基类渲染
	GUIWnd::Render();
}

VOID MallButton::Destroy()
{
	m_pRender->DestroyImage(m_pHotPic);
	m_pRender->DestroyImage(m_pNewPic);
	ItemButton::Destroy();
}

VOID MallButton::SetHotMallItem( bool bHotItem )
{
	if (bHotItem)
	{
		SetHotPic();
	}
	else
	{
		m_pRender->DestroyImage(m_pHotPic);
		m_pHotPic = NULL;
	}
	SetRefresh(TRUE);
}

VOID MallButton::SetNewMallItem( bool bNewItem )
{
	if (bNewItem)
	{
		SetNewPic();
	}
	else
	{
		m_pRender->DestroyImage(m_pNewPic);
		m_pNewPic = NULL;
	}
	SetRefresh(TRUE);
}

VOID MallButton::SetHotPic()
{
	if(P_VALID(m_pHotPic))
	{
		m_pRender->DestroyImage(m_pHotPic);				// 删除背景图片
		m_pHotPic = NULL;
	}
	m_pHotPic = m_pRender->CreateImage(_T("data\\ui\\mall\\L_re.bmp"), m_rcPic);		// 读入图片
}

VOID MallButton::SetNewPic()
{
	if(P_VALID(m_pNewPic))
	{
		m_pRender->DestroyImage(m_pNewPic);				// 删除背景图片
		m_pNewPic = NULL;
	}
	m_pNewPic = m_pRender->CreateImage(_T("data\\ui\\mall\\L_xin.bmp"), m_rcPic);		// 读入图片
}

VOID MallButton::DrawMallItemImg()
{
	if (P_VALID(m_pHotPic))
	{
		m_pRender->Draw(m_pCanvas, m_pHotPic, m_dwPicColor, m_eAlphaWrite);
	}

	if (P_VALID(m_pNewPic))
	{
		m_pRender->Draw(m_pCanvas, m_pNewPic, m_dwPicColor, m_eAlphaWrite);
	}
}

ChestButton::ChestButton() 
: m_pCursorPic(NULL)
, m_bDrawCursor(false)
{
}

ChestButton::~ChestButton()
{

}

BOOL ChestButton::Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement)
{
	BOOL bResult = ItemButton::Init(pSystem, pFather, pXmlElement);

	tstring strPath(_T("data\\ui\\Sys\\L-wabao-l.bmp"));
	m_pCursorPic = m_pRender->CreateImage(strPath, m_rcPic);

	return bResult;
}

VOID ChestButton::Destroy()
{	
	m_pRender->DestroyImage(m_pCursorPic);

	ItemButton::Destroy();
}

VOID ChestButton::Render()
{
	if(!IsRefresh())
		return GUIWnd::Render();
	// 绘制按钮图标
	DrawImg();
	// 绘制品级图标
	DrawQuality();
	// 绘制旋转光标
	DrawCursor();

	GUIWnd::Render();
}


void ChestButton::EnableCursor(bool bCursor)
{
	m_bDrawCursor = bCursor;
	SetRefresh();
}

void ChestButton::DrawCursor()
{
	if (m_bDrawCursor)
	{
		if (P_VALID(m_pCursorPic))
			m_pRender->Draw(m_pCanvas, m_pCursorPic, m_dwPicColor, m_eAlphaWrite);
	}
}


