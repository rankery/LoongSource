#include "stdafx.h"
#include "ToolTipFrame.h"
#include "ToolTipStatic.h"
#include "RoleMgr.h"
#include "LocalPlayer.h"

ToolTipFrame::ToolTipFrame():m_Trunk(this)
{
	m_pWnd			= NULL;
	m_pWndEx		= NULL;
	m_pWndCompare	= NULL;
	m_pWndCompareEx = NULL;
	m_pStcTips		= NULL;
	m_pStcTipsEx	= NULL;
	m_pStcCompare	= NULL;
	m_pStcCompareEx = NULL;
	m_pSrcWnd		= NULL;
}

ToolTipFrame::~ToolTipFrame()
{

}

BOOL ToolTipFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);

	m_pGUI->RegisterEventHandler(m_pGUI->GetDesktop()->GetFullName().c_str(), (VUIEVENTHANDLE)m_Trunk.sfp1(&ToolTipFrame::EventHandler));

	return bRet;
}

BOOL ToolTipFrame::Destroy()
{
	BOOL bRet = GameFrame::Destroy();

	bRet = DestroyUI() && bRet;

	return bRet;
}

BOOL ToolTipFrame::ReloadUI()
{
	XmlElement ele;
	tstring strPath = g_strLocalPath + _T("\\ui\\tips.xml");
	m_pGUI->LoadXml(&ele, "VFS_System", strPath.c_str());
	
	m_pGUI->ChangeXml(&ele, "swin\\tip", "ClassName", "ToolTipStatic");
	
	// 创建
	m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &ele);
	m_pStcTips = (ToolTipStatic*)m_pWnd->GetChild(_T("tip"));
	
	m_pWndEx = m_pGUI->CreateWnd(_T("\\desktop"), &ele);
	m_pStcTipsEx = (ToolTipStatic*)m_pWndEx->GetChild(_T("tip"));

	m_pWndCompare = m_pGUI->CreateWnd(_T("\\desktop"), &ele);
	m_pStcCompare = (ToolTipStatic*)m_pWndCompare->GetChild(_T("tip"));

	m_pWndCompareEx = m_pGUI->CreateWnd(_T("\\desktop"), &ele);
	m_pStcCompareEx = (ToolTipStatic*)m_pWndCompareEx->GetChild(_T("tip"));

	m_pWnd->SetInvisible(true);
	m_pWndEx->SetInvisible(true);
	m_pWndCompare->SetInvisible(true);
	m_pWndCompareEx->SetInvisible(true);

	return TRUE;
}

BOOL ToolTipFrame::DestroyUI()
{
	if(P_VALID(m_pWnd))
	{
		m_pGUI->DestroyWnd(m_pWnd);
	}
	if(P_VALID(m_pWndEx))
	{
		m_pGUI->DestroyWnd(m_pWndEx);
	}
	if(P_VALID(m_pWndCompare))
	{
		m_pGUI->DestroyWnd(m_pWndCompare);
	}
	if(P_VALID(m_pWndCompareEx))
	{
		m_pGUI->DestroyWnd(m_pWndCompareEx);
	}
	return TRUE;
}

VOID ToolTipFrame::ShowTips(GUIWnd* pWnd, LPCTSTR szTip, const RECT& rc)
{
	if(szTip==NULL || _tcsnlen(szTip, sizeof(szTip))==0)
	{
		m_pWnd->SetInvisible(TRUE);
		m_pWndEx->SetInvisible(TRUE);
		m_pWndCompare->SetInvisible(TRUE);
		m_pWndCompareEx->SetInvisible(TRUE);
		return;
	}
	//记录为哪个窗口启动的
	m_pSrcWnd=pWnd;
	
	m_pStcTips->SetText(szTip);
	tagPoint ptSize = tagPoint(0,0);
	m_pStcTips->Resize(ptSize);

	m_pWnd->Resize(tagPoint(ptSize.x+16, ptSize.y+16));
	
	//最后在显示出来
	m_pWnd->SetInvisible(FALSE);
	m_pWnd->SetTopMost(TRUE);
	m_pWnd->FlipToTop();//显示在最上层保证能够看到
}

VOID ToolTipFrame::ShowTips(GUIWnd* pWnd, LPCTSTR szTip, LPCTSTR szTipEx/* =NULL */)
{
	if(szTip==NULL || _tcsnlen(szTip, sizeof(szTip))==0)
	{
		m_pWnd->SetInvisible(TRUE);
		m_pWndEx->SetInvisible(TRUE);
		m_pWndCompare->SetInvisible(TRUE);
		m_pWndCompareEx->SetInvisible(TRUE);
		return;
	}
	//记录为哪个窗口启动的
	m_pSrcWnd=pWnd;
	
	m_pStcTips->SetText(szTip);
	tagPoint ptSize = tagPoint(0,0);
	m_pStcTips->Resize(ptSize);
	
	//重新设置窗体的位置
	ResetWindowPoint(tagPoint(ptSize.x+16, ptSize.y+16));

	BOOL bAddons = szTipEx != NULL && _tcsnlen(szTipEx, sizeof(szTipEx))!=0;
	if(bAddons)
	{
		m_pStcTipsEx->SetText(szTipEx);
		ptSize = tagPoint(0,0);
		m_pStcTipsEx->Resize(ptSize);
		//附属窗口和主窗口等宽
		INT nMaxSize = max(ptSize.x+16, m_pWnd->GetSize().x);
		ResetWindowPoint(tagPoint(nMaxSize, m_pWnd->GetSize().y));
		m_pWndEx->Resize(tagPoint(nMaxSize, ptSize.y+16));
		
		AlignWnd(m_pWndEx, m_pWnd, EGUIA_OuterBottomLeft);
		m_pWndEx->SetInvisible(FALSE);
		m_pWndEx->SetTopMost(TRUE);
		m_pWndEx->FlipToTop();//显示在最上层保证能够看到
	}

	//最后在显示出来
	m_pWnd->SetInvisible(FALSE);
	m_pWnd->SetTopMost(TRUE);
	m_pWnd->FlipToTop();//显示在最上层保证能够看到
}

VOID ToolTipFrame::ShowTips(tagPoint ptPos, LPCTSTR szTip)
{
	if(szTip==NULL || _tcsnlen(szTip, sizeof(szTip))==0)
	{
		m_pWnd->SetInvisible(TRUE);
		m_pWndEx->SetInvisible(TRUE);
		m_pWndCompare->SetInvisible(TRUE);
		m_pWndCompareEx->SetInvisible(TRUE);
		return;
	}
	
	m_pStcTips->SetText(szTip);
	tagPoint ptSize = tagPoint(0,0);
	m_pStcTips->Resize(ptSize);

	//重新设置窗体的位置
	ResetWindowPoint(tagPoint(ptSize.x+16, ptSize.y+16), ptPos);

	//最后在显示出来
	m_pWnd->SetInvisible(FALSE);
	m_pWnd->SetRefresh();
	m_pWnd->SetTopMost(TRUE);
	m_pWnd->FlipToTop();//显示在最上层保证能够看到

}

VOID ToolTipFrame::ShowTipsCompare( LPCTSTR szTip, LPCTSTR szTipEx/*=NULL*/ )
{
	if(szTip==NULL || _tcsnlen(szTip, sizeof(szTip))==0)
	{
		m_pWndCompare->SetInvisible(TRUE);
		m_pWndCompareEx->SetInvisible(TRUE);
		return;
	}
	
	// 增加“当前装备”提示
	tstring strTmp = g_StrTable[_T("EquipCompare_Text")];
	strTmp += szTip;
	m_pStcCompare->SetText(strTmp.c_str());
	tagPoint ptSize = tagPoint(0,0);
	m_pStcCompare->Resize(ptSize);

	//重新设置窗体的位置
	m_pWndCompare->Resize(tagPoint(ptSize.x+16, ptSize.y+16));
	EGUIAlign eAlign = EGUIA_OuterRightTop;
	if(P_VALID(m_pSrcWnd))
		eAlign = m_pSrcWnd->GetView().x < (LONG)(Device()->GetClientWidth()/2) ? EGUIA_OuterRightTop : EGUIA_OuterLeftTop;
	AlignWnd(m_pWndCompare, m_pWnd, eAlign);

	BOOL bAddons = szTipEx != NULL && _tcsnlen(szTipEx, sizeof(szTipEx))!=0;
	if(bAddons)
	{
		// 增加“当前装备”提示
		strTmp = g_StrTable[_T("EquipCompare_Text")];
		strTmp += szTipEx;
		m_pStcCompareEx->SetText(strTmp.c_str());
		ptSize = tagPoint(0,0);
		m_pStcCompareEx->Resize(ptSize);
		
		m_pWndCompareEx->Resize(tagPoint(ptSize.x+16, ptSize.y+16));
		AlignWnd(m_pWndCompareEx, m_pWndCompare, eAlign);

		m_pWndCompareEx->SetInvisible(FALSE);
		m_pWndCompareEx->SetTopMost(TRUE);
		m_pWndCompareEx->FlipToTop();//显示在最上层保证能够看到
	}

	//最后在显示出来
	m_pWndCompare->SetInvisible(FALSE);
	m_pWndCompare->SetTopMost(TRUE);
	m_pWndCompare->FlipToTop();//显示在最上层保证能够看到
}

VOID ToolTipFrame::ResetWindowPoint(tagPoint ptSize)
{
	//提示信息的管理是根据m_pSrcWnd的位置在决定的
	//获得客户端的界面大小
	tagPoint ptClientSize = tagPoint((LONG)Device()->GetClientWidth(), (LONG)Device()->GetClientHeight());
	//获得源控件的大小和位置
	const tagPoint ptWndSize = m_pSrcWnd->GetSize();
	const tagPoint ptWndView = m_pSrcWnd->GetView();
	
	//获取理想显示位置
	tagPoint pt = tagPoint(0, 0);
	if(ptWndView.x > ptClientSize.x/2)
	{
		pt.x = ptWndView.x - ptSize.x;
	}
	else
	{
		pt.x = ptWndView.x + ptWndSize.x;
	}

	if(ptWndView.y > ptClientSize.y/2)
	{
		pt.y = ptWndView.y - ptSize.y;
	}
	else
	{
		pt.y = ptWndView.y + ptWndSize.y;
	}
	
	//计算需要移动的距离
	tagPoint ptMove;
	ptMove.x = pt.x - m_pWnd->GetView().x;
	ptMove.y = pt.y - m_pWnd->GetView().y;
	
	tagRect rc(pt.x, pt.y, pt.x+ptSize.x, pt.y+ptSize.y);
	
	//调整窗体在屏幕中的位置
	if(rc.left < 0)
		ptMove.x -= rc.left;
	if(rc.right > ptClientSize.x)
		ptMove.x -= rc.right - ptClientSize.x;
	if(rc.top < 0)
		ptMove.y -= rc.top;		
	if(rc.bottom > ptClientSize.y)
		ptMove.y -= rc.bottom - ptClientSize.y;

	m_pWnd->Move(ptMove);
	m_pWnd->Resize(ptSize);
}

VOID ToolTipFrame::ResetWindowPoint( tagPoint ptSize, tagPoint ptPos )
{
	//获得客户端的界面大小
	tagPoint ptClientSize = tagPoint((LONG)Device()->GetClientWidth(), (LONG)Device()->GetClientHeight());
	
	//计算需要移动的距离
	tagPoint ptMove;
	ptMove.x = ptPos.x - m_pWnd->GetView().x;
	ptMove.y = ptPos.y - m_pWnd->GetView().y;

	tagRect rc(ptPos.x, ptPos.y, ptPos.x+ptSize.x, ptPos.y+ptSize.y);

	//调整窗体在屏幕中的位置
	if(rc.left < 0)
		ptMove.x -= rc.left;
	if(rc.right > ptClientSize.x)
		ptMove.x -= rc.right - ptClientSize.x;
	if(rc.top < 0)
		ptMove.y -= rc.top;		
	if(rc.bottom > ptClientSize.y)
		ptMove.y -= rc.bottom - ptClientSize.y;

	m_pWnd->Move(ptMove);
	m_pWnd->Resize(ptSize);
}
BOOL ToolTipFrame::EventHandler( tagGUIEvent* pEvent )
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;

	switch (pEvent->eEvent)
	{
	case EGUIE_MoveIntoCtrl:
		{
			ShowStaticTip((GUIStatic*)pWnd);
		}
		break;
	case EGUIE_MoveOutOfCtrl:
		{
			m_pWnd->SetInvisible(true);
			m_pWndEx->SetInvisible(true);
			m_pWndCompare->SetInvisible(true);
			m_pWndCompareEx->SetInvisible(true);
		}
		break;
	}
	return TRUE;
}

VOID ToolTipFrame::ShowStaticTip( GUIStatic* pStatic )
{
	if (pStatic->GetTips().size() > 0)
	{
		tstring strTip;
		strTip = g_StrTable[_T("TipsDefaultFont")]; 
		strTip += pStatic->GetTips();
		// 玩家原始属性增加tips
		if(_tcsnccmp(pStatic->GetFullName().c_str(), 
			_T("\\desktop\\ra_win\\ra_pic\\rstatepic\\equipback\\role\\rs_add\\word"), 59) == 0)
		{
			TCHAR szTmp[X_SHORT_NAME] = {0};
			LocalPlayer* pLp = RoleMgr::Inst()->GetLocalPlayer();
			if(P_VALID(pLp))
			{
				if(_tcscmp(pStatic->GetName().c_str(), _T("word2")) == 0)
				{
					_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), g_StrTable[_T("Tooltip_OriginAtt1")], pLp->GetOriginAtt(ERA_Physique));
				}
				else if(_tcscmp(pStatic->GetName().c_str(), _T("word21")) == 0)
				{
					_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), g_StrTable[_T("Tooltip_OriginAtt2")], pLp->GetOriginAtt(ERA_Strength));
				}
				else if(_tcscmp(pStatic->GetName().c_str(), _T("word22")) == 0)
				{
					_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), g_StrTable[_T("Tooltip_OriginAtt3")], pLp->GetOriginAtt(ERA_Pneuma));
				}
				else if(_tcscmp(pStatic->GetName().c_str(), _T("word23")) == 0)
				{
					_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), g_StrTable[_T("Tooltip_OriginAtt4")], pLp->GetOriginAtt(ERA_InnerForce));
				}
				else if(_tcscmp(pStatic->GetName().c_str(), _T("word24")) == 0)
				{
					_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), g_StrTable[_T("Tooltip_OriginAtt5")], pLp->GetOriginAtt(ERA_Agility));
				}
				else if(_tcscmp(pStatic->GetName().c_str(), _T("word25")) == 0)
				{
					_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), g_StrTable[_T("Tooltip_OriginAtt6")], pLp->GetOriginAtt(ERA_Technique));
				}
				strTip += szTmp;
			}
		}
		::ShowTip(pStatic, strTip.c_str());
	}
	
}

VOID ToolTipFrame::AlignWnd( GUIWnd* pSrcWnd, GUIWnd* pDstWnd, EGUIAlign eAlign )
{
	tagPoint ptClient = tagPoint(0, 0);
	tagPoint ptSrcSize = pSrcWnd->GetSize();
	tagPoint ptSrcView = pSrcWnd->GetView(); 
	tagPoint ptDstSize = pDstWnd->GetSize();
	tagPoint ptDstView = pDstWnd->GetView();
	tagPoint ptOffset = ptDstView - ptSrcView;

	switch(eAlign)
	{
	case EGUIA_Center:				// 内部居中
		ptClient.x = (ptDstSize.x - ptSrcSize.x) / 2; ptClient.y = (ptDstSize.y - ptSrcSize.y) / 2; break;
	case EGUIA_InnerLeftTop:		// 内部左上
		ptClient.x = 0; ptClient.y = 0;	break;
	case EGUIA_InnerLeft:			// 内部左中
		ptClient.x = 0; ptClient.y = (ptDstSize.y - ptSrcSize.y) / 2; break;
	case EGUIA_InnerLeftBottom:		// 内部左下
		ptClient.x = 0; ptClient.y = ptDstSize.y - ptSrcSize.y; break;
	case EGUIA_InnerRightTop:		// 内部右上
		ptClient.x = ptDstSize.x - ptSrcSize.x; ptClient.y = 0; break;
	case EGUIA_InnerRight:			// 内部右中
		ptClient.x = ptDstSize.x - ptSrcSize.x; ptClient.y = (ptDstSize.y - ptSrcSize.y) / 2; break;
	case EGUIA_InnerRightBottom:	// 内部右下
		ptClient.x = ptDstSize.x - ptSrcSize.x; ptClient.y = ptDstSize.y - ptSrcSize.y; break;
	case EGUIA_InnerTop:			// 内部上中
		ptClient.x = (ptDstSize.x - ptSrcSize.x) / 2; ptClient.y = 0; break;
	case EGUIA_InnerBottom:			// 内部下中
		ptClient.x = (ptDstSize.x - ptSrcSize.x) / 2; ptClient.y = ptDstSize.y - ptSrcSize.y; break;
	case EGUIA_OuterLeftTop:		// 外部左上
		ptClient.x = -ptSrcSize.x; ptClient.y = 0; break;
	case EGUIA_OuterLeft:			// 外部左中
		ptClient.x = -ptSrcSize.x; ptClient.y = (ptDstSize.y - ptSrcSize.y) / 2; break;
	case EGUIA_OuterLeftBottom:		// 外部左下
		ptClient.x = -ptSrcSize.x; ptClient.y = ptDstSize.y - ptSrcSize.y; break;
	case EGUIA_OuterRightTop:		// 外部右上
		ptClient.x = ptDstSize.x; ptClient.y = 0; break;
	case EGUIA_OuterRight:			// 外部右中
		ptClient.x = ptDstSize.x; ptClient.y = (ptDstSize.y - ptSrcSize.y) / 2; break;
	case EGUIA_OuterRightBottom:	// 外部右下
		ptClient.x = ptDstSize.x; ptClient.y = ptDstSize.y - ptSrcSize.y; break;
	case EGUIA_OuterTopLeft:		// 外部上左
		ptClient.x = 0; ptClient.y = -ptSrcSize.y; break;
	case EGUIA_OuterTop:			// 外部上中
		ptClient.x = (ptDstSize.x - ptSrcSize.x) / 2; ptClient.y = -ptSrcSize.y; break;
	case EGUIA_OuterTopRight:		// 外部上右
		ptClient.x = ptDstSize.x - ptSrcSize.x; ptClient.y = -ptSrcSize.y; break;
	case EGUIA_OuterBottomLeft:		// 外部下左
		ptClient.x = 0; ptClient.y = ptDstSize.y; break;
	case EGUIA_OuterBottom:			// 外部下中
		ptClient.x = (ptDstSize.x - ptSrcSize.x) / 2; ptClient.y = ptDstSize.y; break;
	case EGUIA_OuterBottomRight:	// 外部下右
		ptClient.x = ptDstSize.x - ptSrcSize.x; ptClient.y = ptDstSize.y; break;
	}

	ptClient = ptClient + ptOffset;
	
	pSrcWnd->Move(ptClient);
}

BOOL ToolTipFrame::EscCancel()
{
	if(m_pWnd->IsInvisible())
		return FALSE;

	m_pWnd->SetInvisible(true);
	m_pWndEx->SetInvisible(true);
	m_pWndCompare->SetInvisible(true);
	m_pWndCompareEx->SetInvisible(true);
	return TRUE;
}