#include "stdafx.h"
#include "OwnerImeFrame.h"
#include "OwnerImeEvent.h"
#include "CursorMgr.h"
#include "OwnerImeMgr.h"

OwnerImeFrame::OwnerImeFrame( void ):m_Trunk(this)
{
	m_pWndIme			= NULL;
	m_pWndInput			= NULL;
	m_pLWord			= NULL;
	m_pBtnUp			= NULL;
	m_pWndActive		= NULL;
	m_pBtnDown			= NULL;
	m_nPageMax			= GT_INVALID;
	m_nCurPage			= 0;
}

OwnerImeFrame::~OwnerImeFrame( void )
{

}

BOOL OwnerImeFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	//--注册游戏事件
	m_pMgr->RegisterEventHandle(_T("tagOwnerInputWordEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&OwnerImeFrame::OnOwnerInputWordEvent));
	m_pMgr->RegisterEventHandle(_T("tagOwnerImeWordEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&OwnerImeFrame::OnOwnerImeWordEvent));
	m_pMgr->RegisterEventHandle(_T("tagOwnerInputWordClear"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&OwnerImeFrame::OnOwnerInputWordClear));
	m_pMgr->RegisterEventHandle(_T("tagOwnerImeShow"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&OwnerImeFrame::OntOwnerImeShow));



	return TRUE;
}

BOOL OwnerImeFrame::Destroy()
{
	if( P_VALID(m_pWndInput) )
		m_pGUI->AddToDestroyList(m_pWndInput);

	if( P_VALID(m_pWndIme) )
		m_pGUI->AddToDestroyList(m_pWndIme);

	//--注销游戏事件
	m_pMgr->UnRegisterEventHandler(_T("tagOwnerInputWordEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&OwnerImeFrame::OnOwnerInputWordEvent));
	m_pMgr->UnRegisterEventHandler(_T("tagOwnerImeWordEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&OwnerImeFrame::OnOwnerImeWordEvent));
	m_pMgr->UnRegisterEventHandler(_T("tagOwnerInputWordClear"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&OwnerImeFrame::OnOwnerInputWordClear));
	m_pMgr->UnRegisterEventHandler(_T("tagOwnerImeShow"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&OwnerImeFrame::OntOwnerImeShow));


	return GameFrame::Destroy();
}

BOOL OwnerImeFrame::ReloadUI()
{
	XmlElement ele;
	tstring strPath = g_strLocalPath + _T("\\ui\\inputword2.xml");
	m_pGUI->LoadXml(&ele, "VFS_System", strPath.c_str());
	m_pWndInput = (GUIStatic*)m_pGUI->CreateWnd(_T("\\desktop"), &ele);
	m_pWndInput->SetInvisible(TRUE);

	tstring strPath1 = g_strLocalPath + _T("\\ui\\inputword.xml");
	m_pGUI->LoadXml(&ele, "VFS_System", strPath1.c_str());
	m_pWndIme = m_pGUI->CreateWnd(_T("\\desktop"), &ele);
	m_pWndIme->SetInvisible(TRUE);
	m_pLWord = (GUIListBox*)m_pWndIme->GetChild(_T("wordlist"));
	m_pLWord->SetColNum(1, 200);
	m_pBtnUp = 	(GUIButton*)m_pWndIme->GetChild(_T("back"));
	m_pBtnDown = (GUIButton*)m_pWndIme->GetChild(_T("next"));

	//注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWndIme->GetFullName().c_str(), m_Trunk.sfp1(&OwnerImeFrame::EventHandler));


	return TRUE;
}

VOID OwnerImeFrame::Update()
{

}

DWORD OwnerImeFrame::EventHandler( tagGUIEvent* pEvent )
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;
	switch(pEvent->eEvent)
	{
	case EGUIE_Click:
		{

		}
		break;
	case EGUIE_ItemClick:
		{

		}
		break;
	default:
		break;
	}

	return TRUE;
}

DWORD OwnerImeFrame::OnOwnerInputWordEvent( tagOwnerInputWordEvent* pGameEvent )
{
	if( P_VALID(m_pWndInput) )
	{
		tagPoint txtSize;	
		m_pRender->GetTextSize(pGameEvent->strText.c_str(), NULL, txtSize);
		txtSize.x += 50;
		txtSize.y = 20;
		m_pWndInput->SetResizable(TRUE);
		m_pWndInput->Resize(txtSize);
		m_pWndInput->SetResizable(FALSE);

		GUIStatic* pWnd = (GUIStatic*)m_pGUI->GetActive();
		tstring strText;
		if( P_VALID(pWnd) )
		{
			strText = pWnd->GetText();
		}
		tagPoint txtSizeAct;

		//输入框与备选字界面一起移动
		if( !strText.empty() )
		{
			m_pRender->GetTextSize(strText.c_str(), NULL, txtSizeAct);

			tagPoint ptActive = (tagPoint&)(pWnd->GetView());
			const tagPoint& ptSize = pWnd->GetSize();

			tagPoint pointOff;
			if( txtSizeAct.x < ptSize.x )
			{
				ptActive.x += txtSizeAct.x;
				tagPoint pointOff(ptActive - (tagPoint&)(m_pWndInput->GetView()));
				pointOff.y = 0;

				m_pWndInput->SetMovable(TRUE);
				m_pWndInput->Move(pointOff);
				m_pWndInput->SetMovable(FALSE);
				m_pWndInput->FlipToTop();

				INT nClientW = (INT)Device()->GetClientWidth()/2;
				INT nClientH = (INT)Device()->GetClientHeight()/2;
				//屏幕左边
				if( pointOff.x < nClientW )
				{
					tagPoint pointIme = (tagPoint&)(m_pWndInput->GetView());
					const tagPoint& ptSizeInput = m_pWndInput->GetSize();
					pointIme.x = pointIme.x+ptSizeInput.x;
					tagPoint pointOffIme(pointIme-(tagPoint&)(m_pWndIme->GetView()));
					m_pWndIme->Move(pointOffIme);
					m_pWndIme->FlipToTop();
				}
				else
					//屏幕右边
				{
					tagPoint pointIme = (tagPoint&)(m_pWndInput->GetView());
					const tagPoint& ptSizeInput = m_pWndInput->GetSize();
					pointIme.x = pointIme.x-ptSizeInput.x;
					tagPoint pointOffIme(pointIme-(tagPoint&)(m_pWndIme->GetView()));
					m_pWndIme->Move(pointOffIme);
					m_pWndIme->FlipToTop();
				}
			}	
			//只有备选字界面移动
			else 
			{
				tagPoint ptCandidate = m_pWndIme->GetView();
				tagPoint ptCandiSize = m_pWndIme->GetSize();
				ptCandidate.x += ptCandiSize.x;
				tagPoint pointOff(ptCandidate - (tagPoint&)(m_pWndInput->GetView()));

				INT nClientW = (INT)Device()->GetClientWidth()/2;
				INT nClientH = (INT)Device()->GetClientHeight()/2;
				//屏幕左边
				if( pointOff.x < nClientW )
				{
					tagPoint pointIme = (tagPoint&)(m_pWndInput->GetView());
					const tagPoint& ptSizeInput = m_pWndInput->GetSize();
					pointIme.x = pointIme.x+ptSizeInput.x;
					tagPoint pointOffIme(pointIme-(tagPoint&)(m_pWndIme->GetView()));
					m_pWndIme->Move(pointOffIme);
					m_pWndIme->FlipToTop();
				}
				else
					//屏幕右边
				{
					tagPoint pointIme = (tagPoint&)(m_pWndInput->GetView());
					const tagPoint& ptSizeInput = m_pWndInput->GetSize();
					pointIme.x = pointIme.x-ptSizeInput.x;
					tagPoint pointOffIme(pointIme-(tagPoint&)(m_pWndIme->GetView()));
					m_pWndIme->Move(pointOffIme);
					m_pWndIme->FlipToTop();
				}
			}
		}
		

		m_pWndInput->SetText(pGameEvent->strText.c_str());
		m_pWndInput->SetRefresh(TRUE);
		if( pGameEvent->strText.empty() )
		{
			m_pWndIme->SetInvisible(TRUE);
			m_pWndInput->SetInvisible(TRUE);

		}
		else if( !pGameEvent->strText.empty() && m_pWndIme->IsInvisible())
		{
			m_pWndInput->SetInvisible(FALSE);
			m_pWndIme->SetInvisible(FALSE);
			m_pLWord->Clear();
		}
	}
	return 0;
}

DWORD OwnerImeFrame::OnOwnerImeWordEvent( tagOwnerImeWordEvent* pGameEvent )
{
	if( P_VALID(m_pLWord) )
	{
		m_pLWord->Clear();
		int nPos = 0;
		int nIndex = 0;
		INT nRow = 0;
		TCHAR szBuff[X_LONG_NAME] = {0};
		INT nLen = pGameEvent->strText.length();
		_tcsncpy( szBuff, pGameEvent->strText.c_str(), nLen );
		
		for( ; nPos<nLen; ++nPos )
		{
			if( szBuff[nPos] == _T('\t') )
			{
				tstring temp;
				temp.assign(szBuff, nIndex, nPos-nIndex);
				m_pLWord->SetText(nRow, 0, temp.c_str());
				nIndex = nPos+1;
				++nRow;
			}
		}
	}
	return 0;
}

DWORD OwnerImeFrame::OntOwnerImeShow( tagOwnerImeShow* pGameEvent )
{
	if( P_VALID(m_pWndIme) && P_VALID(m_pWndInput) && P_VALID(m_pLWord) )
	{
		if( !pGameEvent->bShow )
		{
			m_pWndInput->SetInvisible(TRUE);
			m_pWndIme->SetInvisible(TRUE);
			return 0;
		}
		m_pWndInput->SetText(_T(""));

		GUIWnd* pWnd = m_pGUI->GetActive();
		m_pWndActive = pWnd;
		if( P_VALID(pWnd) )
		{
			tagPoint txtSize;	
			txtSize.x = 50;
			txtSize.y = 20;
			m_pWndInput->SetResizable(TRUE);
			m_pWndInput->Resize(txtSize);
			m_pWndInput->SetResizable(FALSE);

			tagPoint ptActive = (tagPoint&)(pWnd->GetView());
			const tagPoint& ptSize = pWnd->GetSize();
			ptActive.x += txtSize.x;
			ptActive.y = ptActive.y + ptSize.y + 20;
			tagPoint pointOff(ptActive - (tagPoint&)(m_pWndInput->GetView()));

			m_pWndInput->SetMovable(TRUE);
			m_pWndInput->Move(pointOff);
			m_pWndInput->SetMovable(FALSE);
			m_pWndInput->FlipToTop();
			
			INT nClientW = (INT)Device()->GetClientWidth()/2;
			INT nClientH = (INT)Device()->GetClientHeight()/2;
			//屏幕左边
			if( pointOff.x < nClientW )
			{
					tagPoint pointIme = (tagPoint&)(m_pWndInput->GetView());
					const tagPoint& ptSizeInput = m_pWndInput->GetSize();
					pointIme.x = pointIme.x+ptSizeInput.x;
					tagPoint pointOffIme(pointIme-(tagPoint&)(m_pWndIme->GetView()));
					m_pWndIme->Move(pointOffIme);
					m_pWndIme->FlipToTop();
			}
			else
			//屏幕右边
			{
				tagPoint pointIme = (tagPoint&)(m_pWndInput->GetView());
				const tagPoint& ptSizeInput = m_pWndInput->GetSize();
				pointIme.x = pointIme.x-ptSizeInput.x;
				tagPoint pointOffIme(pointIme-(tagPoint&)(m_pWndIme->GetView()));
				m_pWndIme->Move(pointOffIme);
				m_pWndIme->FlipToTop();
			}
		}
	}
	return 0;
}

DWORD OwnerImeFrame::OnOwnerInputWordClear( tagOwnerInputWordClear* pGameEvent )
{
	if( P_VALID(m_pWndInput) )
	{
		m_pWndInput->SetText(_T(""));
		m_pWndInput->SetRefresh(TRUE);
		m_pWndInput->SetInvisible(TRUE);
		m_pWndIme->SetInvisible(TRUE);
	}
	return 0;
}