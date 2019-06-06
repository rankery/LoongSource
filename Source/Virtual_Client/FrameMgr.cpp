#include "stdafx.h"
#include "FrameMgr.h"
#include "Frame.h"
#include "Player.h"


FrameMgr::FrameMgr( Player* pPlayer ) :
m_pPlayer(pPlayer)
{
}

FrameMgr::~FrameMgr(void)
{
	Destroy();
}

VOID FrameMgr::Init()
{
	for (int i=0; i<EF_END; i++)
	{
		m_Frame[i] = NULL;
	}
	m_byCurFrame = GT_INVALID;
	m_listDestroy.Clear();
}

VOID FrameMgr::Update()
{
	// 验证当前Frame的合法性
	if (!GT_VALID(m_byCurFrame))
		return;

	if (m_byCurFrame < 0 || m_byCurFrame >= EF_END)
		return;

	if (!P_VALID(m_Frame[m_byCurFrame]))
		return;

	// 更新当前Frame
	m_Frame[m_byCurFrame]->Update();

	// 清除已经废弃的Frame
	if (!m_listDestroy.Empty())
	{
		FrameBase* pFrameBase = NULL;
		m_listDestroy.ResetIterator();
		while (m_listDestroy.PeekNext(pFrameBase))
		{
			m_listDestroy.Erase(pFrameBase);

			SAFE_DEL(pFrameBase);
		}
	}
}

VOID FrameMgr::Destroy()
{
	// 清除当前Frame
	for (int i=0; i<EF_END; i++)
	{
		if (P_VALID(m_Frame[i]))
		{
			// 更新watch信息
			tagLoginInfo* pLoginInfo = m_pPlayer->m_pLoginInfo;
			switch (m_Frame[i]->m_eFrameType)
			{
			case EF_LOGIN:
				pLoginInfo->nInLogin--;
				break;

			case EF_SELECT:
				pLoginInfo->nInSelect--;
				break;

			case EF_WORLD:
				pLoginInfo->nInWorld--;
				break;
			}
			
			// 销毁
			m_listDestroy.PushFront(m_Frame[i]);
			m_Frame[i] = NULL;
		}
	}

	// 清除废弃Frame
	FrameBase* pFrameBase = NULL;
	m_listDestroy.ResetIterator();
	while (m_listDestroy.PeekNext(pFrameBase))
	{
		SAFE_DEL(pFrameBase);
	}
	m_listDestroy.Clear();
}

BOOL FrameMgr::ChangeFrame( EFrame eFrame )
{
	if (m_byCurFrame == eFrame)
		return FALSE;

	if (eFrame < 0 || eFrame >= EF_END)
		return FALSE;

	if (!P_VALID(m_pPlayer))
		return FALSE;

	FrameBase* pFrame = NULL;
	switch (eFrame)
	{
	case EF_LOGIN:
		pFrame = new LoginFrame(m_pPlayer);
		break;

	case EF_SELECT:
		pFrame = new SelectRoleFrame(m_pPlayer);
		break;

	case EF_WORLD:
		pFrame = new WorldFrame(m_pPlayer);
		break;
	}

	if (!P_VALID(pFrame))
		return FALSE;

	if (m_byCurFrame >= 0 && m_byCurFrame < EF_END)
	{
		m_listDestroy.PushFront(m_Frame[m_byCurFrame]);
		m_Frame[m_byCurFrame] = NULL;
	}

	ChangeLoginInfo(m_pPlayer->m_pLoginInfo, m_byCurFrame, eFrame);
	m_byCurFrame = eFrame;
	m_Frame[m_byCurFrame] = pFrame; 

	m_Frame[m_byCurFrame]->Init();
	//IMSG(_T("Change to Frame %d\n"), m_byCurFrame);

	return TRUE;
}

VOID FrameMgr::ChangeLoginInfo( tagLoginInfo* pLoginInfo, BYTE srcFrame, BYTE dstFrame )
{
	if (!P_VALID(pLoginInfo))
		return;

	if (!GT_VALID(dstFrame))
		return;
	
	switch (dstFrame)
	{
	case EF_LOGIN:
		pLoginInfo->nInLogin++;
		break;

	case EF_SELECT:
		pLoginInfo->nInSelect++;
		break;

	case EF_WORLD:
		pLoginInfo->nInWorld++;
		break;
	}

	if (!GT_VALID(srcFrame))
		return;

	switch (srcFrame)
	{
	case EF_LOGIN:
		if (pLoginInfo->nInLogin > 0)
		{
			pLoginInfo->nInLogin--;
		}	
		break;

	case EF_SELECT:
		if (pLoginInfo->nInSelect > 0)
		{
			pLoginInfo->nInSelect--;
		}	
		break;

	case EF_WORLD:
		if (pLoginInfo->nInWorld > 0)
		{
			pLoginInfo->nInWorld--;
		}	
		break;
	}
}