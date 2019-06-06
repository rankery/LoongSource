//-----------------------------------------------------------------------------
//!\file gui_msgbox.cpp
//!\author Lyp
//!
//!\date 2008-11-11
//! last 2008-11-11
//!
//!\brief 标准消息框
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "MsgBox.h"
#include "..\WorldDefine\currency_define.h"


//-----------------------------------------------------------------------------
// construction / destruction
//-----------------------------------------------------------------------------
MsgBox::MsgBox():m_Trunk(this)
{
	m_pWnd = NULL;
	m_pWndBack = NULL;
	m_pWndText = NULL;
	m_pWndOK = NULL;
	m_pWndCancel = NULL;

	m_bModal = FALSE;
	m_eFlag = MBF_Null;
	m_eResult = MBF_Null;
	m_dwID = GT_INVALID;
}

MsgBox::~MsgBox()
{
	if( m_pWnd )
	{
		if( m_bModal )
			m_pGUI->SetModal(NULL);
		m_pGUI->AddToDestroyList(m_pWnd);
	}
}


//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL MsgBox::Init(LPCTSTR szCaption, LPCTSTR szText, LPCTSTR szEvent, MsgBoxFlag eFlag, BOOL bModal)
{
	m_strCaption = szCaption;
	m_strText = szText;

	if( P_VALID(szEvent) )
		m_strEvent = szEvent;
	else
		m_strEvent.clear();

	m_eFlag = eFlag;
	m_bModal = bModal;

	this->LoadGUI();
	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&MsgBox::EventHandler));
	m_pWndBack = (GUIPatch*)m_pWnd->GetChild(_T("back"));
	m_pWndText = (GUIStaticEx*)m_pWndBack->GetChild(_T("textback\\text"));
	m_pWndOK = (GUIButton*)m_pWndBack->GetChild(_T("ok"));
	m_pWndCancel = (GUIButton*)m_pWndBack->GetChild(_T("cancel"));

	if( m_bModal )
		m_pGUI->SetModal(m_pWnd);

	if( !(m_eFlag & MBF_Cancel) )
		m_pWndCancel->SetEnable(false);

	m_pWndBack->SetText(m_strCaption.c_str());
	m_pWndText->SetText(m_strText.c_str());

	return TRUE;
}


//-----------------------------------------------------------------------------
// EventHandler
//-----------------------------------------------------------------------------
BOOL MsgBox::EventHandler( tagGUIEvent* pEvent )
{
	if( !P_VALID(m_pWnd) )
		return FALSE;

	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;

	switch( pEvent->eEvent )
	{
	case EGUIE_Click:
		{
			tagMsgBoxEvent event(m_strEvent.c_str(), NULL);
			if( pWnd == m_pWndOK )
			{
				m_eResult = MBF_OK;
				event.eResult = MBF_OK;
				m_pGameFrameMgr->SendEvent(&event);
			}
			if( pWnd == m_pWndCancel )
			{
				m_eResult = MBF_Cancel;
				event.eResult = MBF_Cancel;
				m_pGameFrameMgr->SendEvent(&event);
			}

			if( m_bModal )
				m_pGUI->SetModal(NULL);

			// 检查自己是否已经被删除
			GUIWnd* pTestWnd = m_pGUI->GetWnd(m_dwID);
			if( P_VALID(pTestWnd) )
				m_pGUI->AddToDestroyList(m_pWnd);	// 删除界面

			m_pWnd = NULL;
		}
		break;
	}

	return TRUE;
}



//-----------------------------------------------------------------------------
// LoadGUI
//-----------------------------------------------------------------------------
BOOL MsgBox::LoadGUI()
{
	XmlElement element;

	// 创建窗口
	tstring strPath = g_strLocalPath + _T("\\ui\\msgbox.xml");
	m_pGUI->LoadXml(&element, "VFS_System", strPath.c_str());

	// 创建
	m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &element);
	m_pGUI->SetActive(m_pWnd);

	m_dwID = m_pWnd->GetID();

	return TRUE;
}



//----------------------------------------------------------------------------------
// MsgInputBox
//----------------------------------------------------------------------------------
MsgInputBox::MsgInputBox():m_Trunk(this)
{
	m_pWnd = NULL;
	m_pWndBack = NULL;
	m_pWndText = NULL;
	m_pWndOK = NULL;
	m_pWndCancel = NULL;
	m_pWndInput = NULL;

	m_bModal = FALSE;
	m_eResult = MBF_Null;
	m_eInput = MBIT_Number;
	
}

MsgInputBox::~MsgInputBox()
{
	if( m_pWnd )
	{
		if( m_bModal )
			m_pGUI->SetModal(NULL);
		m_pGUI->AddToDestroyList(m_pWnd);
	}
}

BOOL MsgInputBox::Init( LPCTSTR szCaption, LPCTSTR szText, LPCTSTR szEvent/*=NULL*/, INT nMaxValue /*= GT_INVALID*/, MsgBoxInputType eInput/*=MBIT_Number*/, LPCTSTR szDefault/*=NULL*/, BOOL bModal/*=FALSE*/ )
{
	m_strCaption = szCaption;
	m_strText = szText;

	if( P_VALID(szEvent) )
		m_strEvent = szEvent;
	else
		m_strEvent.clear();

	m_bModal = bModal;
	m_eInput = eInput;

	this->LoadGUI();
	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&MsgInputBox::EventHandler));
	m_pWndBack = (GUIPatch*)m_pWnd->GetChild(_T("comm_pic"));
	m_pWndText = (GUIStaticEx*)m_pWndBack->GetChild(_T("comm_words"));
	m_pWndOK = (GUIButton*)m_pWndBack->GetChild(_T("surebutt"));
	m_pWndCancel = (GUIButton*)m_pWndBack->GetChild(_T("canclebutt"));
	
	if (MBIT_Text == m_eInput)
	{
		m_pWndInputEx = (GUIEditBoxEx*)m_pWndBack->GetChild(_T("putinpic\\putin"));
		m_pGUI->SetActive(m_pWndInputEx);
	}
	else
	{
		m_pWndInput = (GUIEditBox*)m_pWndBack->GetChild(_T("putinpic\\putin"));
		m_pGUI->SetActive(m_pWndInput);
	}

	if( m_bModal )
		m_pGUI->SetModal(m_pWnd);

	if ( MBIT_Number == m_eInput && nMaxValue >= 0)
	{
		m_pWndInput->SetMaxValue(nMaxValue);
	}

	m_pWndBack->SetText(m_strCaption.c_str());
	m_pWndText->SetText(m_strText.c_str());

	return TRUE;
}

VOID MsgInputBox::SetContent( LPCTSTR szContent, BOOL bSelectAll )
{
	if(MBIT_Text == m_eInput)
	{
		m_pWndInputEx->SetText(szContent);

		if( bSelectAll )
		{
			m_pWndInputEx->SelectAll();
		}
	}
}

BOOL MsgInputBox::EventHandler( tagGUIEvent* pEvent )
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;

	switch( pEvent->eEvent )
	{
	case EGUIE_Click:
		{
			tagMsgInputBoxEvent event(m_strEvent.c_str(), NULL);
			if( pWnd == m_pWndOK )
			{
				m_eResult = MBF_OK;
				event.eResult = MBF_OK;
				event.eInputType = m_eInput;
				if(MBIT_Number == m_eInput)
				{
					event.nInputNum = _tcstol(m_pWndInput->GetText(), NULL, 10);
				}
				else if(MBIT_Text == m_eInput)
				{
					event.strInputName = m_pWndInputEx->GetText(FALSE);
				}
				else
				{
					event.strInputName = m_pWndInput->GetText();
				}
				m_pGameFrameMgr->SendEvent(&event);
			}
			if( pWnd == m_pWndCancel )
			{
				m_eResult = MBF_Cancel;
				event.eResult = MBF_Cancel;
				m_pGameFrameMgr->SendEvent(&event);
			}

			if( m_bModal )
				m_pGUI->SetModal(NULL);
			m_pGUI->AddToDestroyList(m_pWnd);	// 删除界面
			m_pWnd = NULL;
		}
	}

	return TRUE;
}

BOOL MsgInputBox::LoadGUI()
{
	XmlElement element;
	tstring strPath = g_strLocalPath;
	
	// 创建窗口
	if(MBIT_Number == m_eInput)
	{
		strPath += _T("\\ui\\common4.xml");
	}
	else if(MBIT_Password == m_eInput)
	{
		strPath += _T("\\ui\\common5.xml");
	}
	else if(MBIT_Name == m_eInput)
	{
		strPath += _T("\\ui\\common3.xml");
	}
	else 
	{
		strPath += _T("\\ui\\common2.xml");
	}
	m_pGUI->LoadXml(&element, "VFS_System", strPath.c_str());
	
	// 创建
	m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &element);
	m_pGUI->SetActive(m_pWnd);

	return TRUE;
}

//--------------------------------------------------------------------------
// MsgMoneyInputBox(金钱输入框)
//--------------------------------------------------------------------------

MsgMoneyInputBox::MsgMoneyInputBox():m_Trunk(this)
{
	m_pWnd = NULL;
	m_pWndBack = NULL;
	m_pWndText = NULL;
	m_pWndOK = NULL;
	m_pWndCancel = NULL;
	m_pWndYuanBao = NULL;
	m_pWndGold = NULL;
	m_pWndSilver = NULL;

	m_bModal = FALSE;
	m_eInput = MBMT_Null;
	m_eResult = MBF_Null;
}

MsgMoneyInputBox::~MsgMoneyInputBox()
{
	if( m_pWnd )
	{
		if( m_bModal )
			m_pGUI->SetModal(NULL);
		m_pGUI->AddToDestroyList(m_pWnd);
	}
}

BOOL MsgMoneyInputBox::Init( LPCTSTR szCaption, LPCTSTR szText, LPCTSTR szEvent/*=NULL*/, INT64 n64MaxGold , INT nMaxYuanBao ,INT64 n64Default, INT nDefault, MsgBoxMoneyType eInput/*=(MsgBoxMoneyType)(MBMT_Silver|MBMT_Yuanbao)*/, BOOL bModal/*=FALSE*/ )
{
	m_strCaption = szCaption;
	m_strText = szText;

	if( P_VALID(szEvent) )
		m_strEvent = szEvent;
	else
		m_strEvent.clear();

	m_bModal = bModal;
	m_eInput = eInput;

	this->LoadGUI();
	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&MsgMoneyInputBox::EventHandler));
	m_pWndBack = (GUIPatch*)m_pWnd->GetChild(_T("comm_pic"));
	m_pWndText = (GUIStaticEx*)m_pWndBack->GetChild(_T("words"));
	m_pWndOK = (GUIButton*)m_pWndBack->GetChild(_T("surebutt"));
	m_pWndCancel = (GUIButton*)m_pWndBack->GetChild(_T("canclebutt"));
	
	if( m_bModal )
		m_pGUI->SetModal(m_pWnd);

	TCHAR szTmp[X_SHORT_NAME];

	if( m_eInput & MBMT_Silver )
	{
		m_n64MaxGold = n64MaxGold;

		m_pWndSilver = (GUIEditBox*)m_pWndBack->GetChild(_T("inputpic2\\num2"));
		
		m_pWndGold = (GUIEditBox*)m_pWndBack->GetChild(_T("inputpic1\\num1"));
		m_pGUI->SetActive(m_pWndGold);
		if( n64MaxGold >= 0 )
		{
			if(n64MaxGold >= GOLD2SILVER)
				m_pWndSilver->SetMaxValue(9999);
			else
				m_pWndSilver->SetMaxValue(MSilver2DBSilver(n64MaxGold));
			m_pWndGold->SetMaxValue(MSilver2DBGold(n64MaxGold));
		}
		
		if( n64Default > 0 )
		{
			_stprintf(szTmp, _T("%d"), MSilver2DBGold(n64Default));
			m_pWndGold->SetText(szTmp);
			_stprintf(szTmp, _T("%d"), MSilver2DBSilver(n64Default));
			m_pWndSilver->SetText(szTmp);
		}
	}

	if( m_eInput & MBMT_Yuanbao )
	{
		m_pWndYuanBao = (GUIEditBox*)m_pWndBack->GetChild(_T("inputpic3\\num3"));
		m_pGUI->SetActive(m_pWndYuanBao);
		if( nMaxYuanBao >= 0 )
		{
			m_pWndYuanBao->SetMaxValue(nMaxYuanBao);
		}

		if( nDefault > 0 )
		{
			_stprintf(szTmp, _T("%d"), nDefault);
			m_pWndYuanBao->SetText(szTmp);
		}
	}
	
	m_pWndBack->SetText(m_strCaption.c_str());
	m_pWndText->SetText(m_strText.c_str());

	return TRUE;
}

BOOL MsgMoneyInputBox::EventHandler( tagGUIEvent* pEvent )
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;

	switch( pEvent->eEvent )
	{
	case EGUIE_Click:
		{
			tagMsgMoneyBoxEvent event(m_strEvent.c_str(), NULL);
			if( pWnd == m_pWndOK )
			{
				m_eResult = MBF_OK;
				event.eResult = MBF_OK;
				event.eMonetType = m_eInput;
				if(MBMT_Silver & m_eInput)
				{
					INT nGold	= _tcstol(m_pWndGold->GetText(),   NULL, 10);
					INT nSilver = _tcstol(m_pWndSilver->GetText(), NULL, 10);
					event.n64Silver = MGold2Silver(nGold) + nSilver;
					if(m_n64MaxGold >= 0 && event.n64Silver > m_n64MaxGold)
						event.n64Silver = m_n64MaxGold;
				}
				if(MBMT_Yuanbao & m_eInput) 
				{
					event.nYuanbao = _tcstol(m_pWndYuanBao->GetText(), NULL, 10);
				}

				m_pGameFrameMgr->SendEvent(&event);
			}
			if( pWnd == m_pWndCancel )
			{
				m_eResult = MBF_Cancel;
				event.eResult = MBF_Cancel;
				m_pGameFrameMgr->SendEvent(&event);
			}

			if( m_bModal )
				m_pGUI->SetModal(NULL);
			m_pGUI->AddToDestroyList(m_pWnd);	// 删除界面
			m_pWnd = NULL;
		}
	}
	return TRUE;
}

BOOL MsgMoneyInputBox::LoadGUI()
{
	XmlElement element;
	tstring strPath = g_strLocalPath;

	// 创建窗口
	if(MBMT_Silver == m_eInput)
	{
		strPath += _T("\\ui\\common6.xml");
	}
	else if(MBMT_Yuanbao == m_eInput)
	{
		strPath += _T("\\ui\\common7.xml");
	}
	else 
	{
		strPath += _T("\\ui\\common8.xml");
	}
	m_pGUI->LoadXml(&element, "VFS_System", strPath.c_str());

	// 创建
	m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &element);
	m_pGUI->SetActive(m_pWnd);

	return TRUE;
}



//--------------------------------------------------------------------------
// 互斥3选对话框类
//--------------------------------------------------------------------------

MsgOptionBox::MsgOptionBox() : m_Trunk(this), 
							   m_pWnd(NULL),
							   m_pWndBack(NULL),
							   m_pBtnCancel(NULL),
							   m_pBtnSure(NULL),
							   m_pSeDescption(NULL),
							   m_pSeOption1(NULL),
							   m_pSeOption2(NULL),
							   m_pSeOption3(NULL),
							   m_pFrameMutex(NULL),
							   m_pBtnOption1(NULL),
							   m_pBtnOption2(NULL),
							   m_pBtnOption3(NULL),
							   m_bModal(FALSE),
							   m_eResult(EMOBF_Null),
							   m_eTemp(EMOBF_Null)
{
}

MsgOptionBox::~MsgOptionBox()
{
	if (m_pWnd)
	{
		if (m_bModal)
			m_pGUI->SetModal(NULL);
		m_pGUI->AddToDestroyList(m_pWnd);
	}
}

BOOL MsgOptionBox::Init(LPCTSTR szCaption, LPCTSTR szDescription, 
						LPCTSTR szOptionText1, LPCTSTR szOptionText2, LPCTSTR szOptionText3, 
						bool bOption1, bool bOption2, bool bOption3,
						LPCTSTR szEvent, BOOL bModal)
{

	LoadGUI();
	// 注册窗口事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&MsgOptionBox::EventHandler));

	// 加载子控件
	m_pWndBack = (GUIPatch*)(m_pWnd->GetChild(_T("comm12_pic")));
	m_pBtnCancel = (GUIButton*)(m_pWndBack->GetChild(_T("canclebutt")));
	m_pBtnSure = (GUIButton*)(m_pWndBack->GetChild(_T("surebutt")));
	m_pSeDescption = (GUIStaticEx*)(m_pWndBack->GetChild(_T("comm12_words1")));
	m_pSeOption1 = (GUIStaticEx*)(m_pWndBack->GetChild(_T("comm12_words2")));
	m_pSeOption2 = (GUIStaticEx*)(m_pWndBack->GetChild(_T("comm12_words20")));
	m_pSeOption3 = (GUIStaticEx*)(m_pWndBack->GetChild(_T("comm12_words200")));
	m_pFrameMutex = (GUIFrameMutex*)(m_pWndBack->GetChild(_T("fra")));
	m_pBtnOption1 = (GUIPushButton*)(m_pFrameMutex->GetChild(_T("opt1")));
	m_pBtnOption2 = (GUIPushButton*)(m_pFrameMutex->GetChild(_T("opt2")));
	m_pBtnOption3 = (GUIPushButton*)(m_pFrameMutex->GetChild(_T("opt3")));

	if (m_bModal)
		m_pGUI->SetModal(m_pWnd);

	if (P_VALID(szEvent))
		m_strEvent = szEvent;
	else
		m_strEvent.clear();

	// 设置控件标题
	m_pWndBack->SetText(szCaption);
	m_pSeDescption->SetText(szDescription);
	m_pSeOption1->SetText(szOptionText1);
	m_pSeOption2->SetText(szOptionText2);
	m_pSeOption3->SetText(szOptionText3);
	m_pBtnOption1->SetEnable(bOption1);
	m_pBtnOption2->SetEnable(bOption2);
	m_pBtnOption3->SetEnable(bOption3);

	m_eResult = EMOBF_Null;
	if (bOption1)
	{
		m_eTemp = EMOBF_Option1;
		m_pBtnOption1->SetState(EGUIBS_PushDown, TRUE);
	}
	else if (bOption2)
	{
		m_eTemp = EMOBF_Option2;
		m_pBtnOption2->SetState(EGUIBS_PushDown, TRUE);
	}
	else if (bOption3)
	{
		m_eTemp = EMOBF_Option3;
		m_pBtnOption3->SetState(EGUIBS_PushDown, TRUE);
	}
	return TRUE;
}

BOOL MsgOptionBox::LoadGUI()
{
	XmlElement element;

	// 读取界面元素XML文件
	tstring strPath = g_strLocalPath + _T("\\ui\\common12.xml");
	m_pGUI->LoadXml(&element, "VFS_System", strPath.c_str());

	// 让窗口居中对齐
	m_pGUI->ChangeXml(&element, "comm12_win", "AlignType", "Center");	

	// 创建窗口
	m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &element);

	m_pGUI->SetActive(m_pWnd);

	return TRUE;
}

BOOL MsgOptionBox::EventHandler(tagGUIEvent *pEvent)
{
	GUIWnd *pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if (!P_VALID(pWnd))
		return FALSE;

	switch (pEvent->eEvent)
	{
		// 处理PushDownButton按下事件
	case EGUIE_PushButtonDown:
		{
			if (pWnd == m_pBtnOption1)
			{
				m_eTemp = EMOBF_Option1;
			}
			else if (pWnd == m_pBtnOption2)
			{
				m_eTemp = EMOBF_Option2;
			}
			else if (pWnd == m_pBtnOption3)
			{
				m_eTemp = EMOBF_Option3;
			}
		} break;

		// 点下确定或取消
	case EGUIE_Click:
		{
			tagMsgOptionBoxEvent event(m_strEvent.c_str(), NULL);
			if (pWnd == m_pBtnCancel)
			{
				m_eResult = EMOBF_Null;
				event.eResult = m_eResult;
				m_pGameFrameMgr->SendEvent(&event);
			}
			if (pWnd == m_pBtnSure)
			{
				m_eResult = m_eTemp;

				event.eResult = m_eResult;
				m_pGameFrameMgr->SendEvent(&event);
			}

			if (m_bModal)
				m_pGUI->SetModal(NULL);
			m_pGUI->AddToDestroyList(m_pWnd);
			m_pWnd = NULL;
		} break;

	default: break;
	}

	return TRUE;
}

BOOL MsgBoxEx::Init( LPCTSTR szCaption, LPCTSTR szText, LPCTSTR szEvent/*=NULL*/, MsgBoxFlag eFlag/*=(MsgBoxFlag)(MBF_OK)*/, BOOL bModal/*=TRUE*/, BOOL bTop/*=FALSE*/)
{
	m_strCaption = szCaption;
	m_strText = szText;

	if( P_VALID(szEvent) )
		m_strEvent = szEvent;
	else
		m_strEvent.clear();

	m_eFlag = eFlag;
	m_bModal = bModal;

	this->LoadGUI();
	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&MsgBoxEx::EventHandler));
	m_pWndBack = (GUIPatch*)m_pWnd->GetChild(_T("comm_pic"));
	m_pWndText = (GUIStaticEx*)m_pWndBack->GetChild(_T("comm_words"));
	m_pWndOK = (GUIButton*)m_pWndBack->GetChild(_T("surebutt"));

	if( m_bModal )
		m_pGUI->SetModal(m_pWnd);

	if( bTop && P_VALID(m_pWnd) )
	{
		m_pWnd->SetTopMost(TRUE);
		m_pWnd->FlipToTop();
	}

	m_pWndBack->SetText(m_strCaption.c_str());
	m_pWndText->SetText(m_strText.c_str());

	return TRUE;
}

MsgBoxEx::MsgBoxEx():m_Trunk(this)
{
	m_pWnd = NULL;
	m_pWndBack = NULL;
	m_pWndText = NULL;
	m_pWndOK = NULL;

	m_bModal = FALSE;
	m_eFlag = MBF_Null;
	m_eResult = MBF_Null;
	m_dwID = GT_INVALID;
}

MsgBoxEx::~MsgBoxEx()
{
	if( m_pWnd )
	{
		if( m_bModal )
			m_pGUI->SetModal(NULL);
		m_pGUI->AddToDestroyList(m_pWnd);
	}
}

BOOL MsgBoxEx::EventHandler( tagGUIEvent* pEvent )
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;

	switch( pEvent->eEvent )
	{
	case EGUIE_Click:
		{
			tagMsgBoxEvent event(m_strEvent.c_str(), NULL);
			if( pWnd == m_pWndOK )
			{
				m_eResult = MBF_OK;
				event.eResult = MBF_OK;
				m_pGameFrameMgr->SendEvent(&event);
			}

			if( m_bModal )
				m_pGUI->SetModal(NULL);

			// 检查自己是否已经被删除
			GUIWnd* pTestWnd = m_pGUI->GetWnd(m_dwID);
			if( P_VALID(pTestWnd) )
				m_pGUI->AddToDestroyList(m_pWnd);	// 删除界面

			m_pWnd = NULL;
		}
		break;
	}

	return TRUE;
}

BOOL MsgBoxEx::LoadGUI()
{
	XmlElement element;

	// 创建窗口
	tstring strPath = g_strLocalPath + _T("\\ui\\common1.xml");
	m_pGUI->LoadXml(&element, "VFS_System", strPath.c_str());

	// 创建
	m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &element);
	m_pGUI->SetActive(m_pWnd);

	m_dwID = m_pWnd->GetID();

	return TRUE;
}