#include "stdafx.h"
#include "MallGiftFrame.h"

MallGiftFrame::MallGiftFrame( void ):m_Trunk(this)
{
	m_pWnd					= NULL;
	m_pWndCaption			= NULL;	
	m_pBtnOK				= NULL;
	m_pBtnCancel			= NULL;
	m_pEdtName				= NULL;
	m_pEdtNum				= NULL;
	m_pEditWord				= NULL;
}

MallGiftFrame::~MallGiftFrame( void )
{

}

BOOL MallGiftFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam );

	return bRet;
}

BOOL MallGiftFrame::Destroy()
{
	BOOL bRet = GameFrame::Destroy();

	if(P_VALID(m_pWnd))
	{
		m_pGUI->AddToDestroyList(m_pWnd);
		m_pWnd = NULL;
	}

	return bRet;
}

BOOL MallGiftFrame::ReloadUI()
{
	XmlElement ele;
	tstring strPath = g_strLocalPath + _T("\\ui\\mall_gift.xml");
	m_pGUI->LoadXml(&ele, "VFS_System", strPath.c_str());

	m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &ele);

	m_pWndCaption	= m_pWnd->GetChild(_T("gift_pic"));
	m_pBtnOK		= (GUIButton*)m_pWndCaption->GetChild(_T("surebutt"));
	m_pBtnCancel	= (GUIButton*)m_pWndCaption->GetChild(_T("canclebutt"));
	m_pEdtName		= (GUIEditBox*)m_pWndCaption->GetChild(_T("putinpic1\\putin"));
	m_pEdtNum		= (GUIEditBox*)m_pWndCaption->GetChild(_T("putinpic0\\putin"));
	m_pEditWord		= (GUIEditBox*)m_pWndCaption->GetChild(_T("putinpic\\putin"));

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&MallGiftFrame::EventHandler));

	return TRUE;
}

VOID MallGiftFrame::EventHandler( tagGUIEvent* pEvent )
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	switch (pEvent->eEvent)
	{
	case EGUIE_Click:
		{
			tagMsgMallGiftEvent event(_T("tagMsgMallGiftEvent"), NULL);
			if( pWnd == m_pBtnOK )
			{
				event.eRet = MGF_OK;
				event.strName = m_pEdtName->GetText();
				TCHAR szBuff[X_SHORT_NAME] = {0};
				_tcsncpy( szBuff, m_pEdtNum->GetText(), sizeof(szBuff)/sizeof(TCHAR) );
				event.n16Num = _ttoi(szBuff);
				event.strWord = m_pEditWord->GetText();
				event.dwTime = timeGetTime();
				m_pFrameMgr->SendEvent(&event);
			}
			if( pWnd == m_pBtnCancel )
			{
				event.eRet = MGF_Cancel;
				m_pFrameMgr->SendEvent(&event);
			}
			if( P_VALID(m_pWnd) )
				m_pWnd->SetInvisible(TRUE);
			if( P_VALID(m_pEdtName) )
				m_pEdtName->SetText(_T(""));
			if( P_VALID(m_pEdtNum) )
				m_pEdtNum->SetText(_T("1"));
			if( P_VALID(m_pEditWord) )
				m_pEditWord->SetText(_T(""));
		}
		break;
	}
}

void MallGiftFrame::SetBuyNum()
{
	if( P_VALID(m_pEdtNum) )
		m_pEdtNum->SetText(_T("1"));
}

void MallGiftFrame::SetMaxBuyNum(INT nMaxNum)
{
	if( P_VALID(m_pEdtNum) )
		m_pEdtNum->SetMaxValue(nMaxNum);
}

void MallGiftFrame::ShowGiftFrame()
{
	if( P_VALID(m_pWnd) )
	{
		m_pWnd->SetInvisible(FALSE);
		m_pWnd->FlipToTop();
	}
}

BOOL MallGiftFrame::EscCancel()
{
	if( P_VALID(m_pWnd) && m_pWnd->IsInvisible() )
		return FALSE;

	tagMsgMallGiftEvent event(_T("tagMsgMallGiftEvent"), NULL);
	event.eRet = MGF_Cancel;
	m_pFrameMgr->SendEvent(&event);
	if( P_VALID(m_pWnd) )
		m_pWnd->SetInvisible(TRUE);
	if( P_VALID(m_pEdtName) )
		m_pEdtName->SetText(_T(""));
	if( P_VALID(m_pEdtNum) )
		m_pEdtNum->SetText(_T("1"));
	if( P_VALID(m_pEditWord) )
		m_pEditWord->SetText(_T(""));
	return TRUE;
}