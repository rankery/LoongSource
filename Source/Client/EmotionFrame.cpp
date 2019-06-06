#include "StdAfx.h"
#include "EmotionFrame.h"
#include "ToolTipFrame.h"
#include "ToolTipCreator.h"

EmotionFrame::EmotionFrame(void):m_Trunk(this)
{
	m_pWnd = NULL;
	m_pWndCaption = NULL;
	memset(m_pBtnEmotion, 0x0, sizeof(m_pBtnEmotion));
}

EmotionFrame::~EmotionFrame(void)
{
	
}

BOOL EmotionFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	return TRUE;
}

BOOL EmotionFrame::Destroy()
{
	GameFrame::Destroy();

	if( P_VALID(m_pWnd) )
		m_pGUI->AddToDestroyList(m_pWnd);

	return TRUE;
}

BOOL EmotionFrame::ReloadUI()
{
	XmlElement element;
	tstring strPath = g_strLocalPath + _T("\\ui\\emo.xml");

	m_pGUI->LoadXml(&element, "VFS_System", strPath.c_str());

	char szBuff[X_LONG_NAME];
	char szPic[X_LONG_NAME];
	//修改属性
	for(int i=0; i<MAX_EMOTION; i++)
	{
		sprintf( szBuff, "emowin\\emoback\\emopic\\emo%d", i+1 );
		sprintf( szPic, "data\\ui\\chat\\emotion\\emo%02d.bmp", i+1 );
		m_pGUI->ChangeXml(&element, szBuff, "PicFileName", szPic); 
		m_pGUI->ChangeXml(&element, szBuff, "PushPicFileName", szPic); 
		m_pGUI->ChangeXml(&element, szBuff, "MovePicFileName", szPic); 
		m_pGUI->ChangeXml(&element, szBuff, "PicColor", "0xFFFFFFFF"); 
		m_pGUI->ChangeXml(&element, szBuff, "PushPicColor", "0xFFFFFFFF"); 
		m_pGUI->ChangeXml(&element, szBuff, "MovePicColor", "0xFFFFFFFF"); 
	}


	m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &element);

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&EmotionFrame::EventHandler));

	m_pWndCaption = m_pWnd->GetChild( _T("emoback") );
	
	for(int i=0; i<MAX_EMOTION; i++)
	{
		tstringstream szBtnName;
		szBtnName << _T("emopic\\emo") << i+1;
		m_pBtnEmotion[i] = (GUIButton*)m_pWndCaption->GetChild(szBtnName.str().c_str());
	}
	
	m_pWnd->SetInvisible(TRUE);
	return TRUE;
}

BOOL EmotionFrame::EventHandler(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;
	switch(pEvent->eEvent)
	{
	case EGUIE_Click:
		for(int i=0; i<MAX_EMOTION; i++)
		{
			if( pWnd == m_pBtnEmotion[i] )
			{
				const TCHAR* szEmnTran = NULL;
				szEmnTran = GetEmtionTran(i);
				if( !P_VALID(szEmnTran) || _tcslen(szEmnTran)<=0 )
					break;
				m_pFrameMgr->SendEvent( &tagGameEventForChat(_T("tagGameEventForChat"), this, szEmnTran) );
			}
		}
		break;
	case EGUIE_MoveIntoCtrl:
		{
			if( _tcsnccmp(pWnd->GetName().c_str(), _T("emo"), 3) == 0 )
			{
				for( int i=0; i<MAX_EMOTION; i++ )
				{
					if( pWnd == m_pBtnEmotion[i])
					{
						tstring strTip = g_StrTable[_T("TipsDefaultFont")];
						strTip += GetEmotionMean(i);
						ShowTip( m_pBtnEmotion[i], strTip.c_str() );
					}
				}
			}
			
		}
		break;
	case EGUIE_MoveOutOfCtrl:
		{
			ShowTip(pWnd, NULL);
		}
		break;
	}
	return FALSE;
}

VOID EmotionFrame::LoadEmotionFile(const tstring& szFile)
{
	TObjRef<VarContainer> VarContainer = CreateObj("Emotion", "VarContainer");
	VarContainer->Load("VFS_System", szFile.c_str());
	TCHAR szBuff[X_LONG_NAME];
	for(int i=0; i<MAX_EMOTION; i++)
	{
		_stprintf( szBuff, _T("emo%02d"), i+1 );
		m_vecEmotionTran.push_back( VarContainer->GetString(_T("charater"), szBuff) );
		m_vecEmotionMean.push_back( VarContainer->GetString(_T("tran"), szBuff) );
	}
	KillObj("Emotion");
}

const TCHAR* EmotionFrame::GetEmtionTran(int index)
{
	if( m_vecEmotionTran.empty() )
		return NULL;
	if( index<0 || index>=(int)m_vecEmotionTran.size() )
		return NULL;

	ASSERT(m_vecEmotionTran[index].size()>1);
	return m_vecEmotionTran[index].c_str();
}

VOID EmotionFrame::SetInVisibleFrame(bool bVal)
{
	m_pWnd->SetInvisible(bVal); 
	if( !bVal && P_VALID(m_pWnd) )
		m_pWnd->Align();
}

const TCHAR* EmotionFrame::GetEmotionMean( int index )
{
	if( m_vecEmotionMean.empty() )
		return NULL;
	if( index<0 || index>=(int)m_vecEmotionMean.size() )
		return NULL;

	return m_vecEmotionMean[index].c_str();
}

BOOL EmotionFrame::EscCancel()
{
	if( m_pWnd->IsInvisible() == FALSE )
	{
		SetInVisibleFrame(true);
		return TRUE;
	}
	return FALSE;
}