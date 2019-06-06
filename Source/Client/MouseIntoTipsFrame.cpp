#include "stdafx.h"
#include "MouseIntoTipsFrame.h"
#include "ToolTipFrame.h"
#include "WorldPickerFrame.h"
#include "LocalPlayer.h"
#include "NPC.h"
#include "RoleMgr.h"
#include "PlayerNameTab.h"
#include "GuildMgr.h"
#include "CreatureData.h"
#include "CombatActionChecker.h"
#include "SkillMgr.h"
#include "CombatSys.h"
#include "ToolTipStatic.h"

const TCHAR* szGreen = _T("<color=0xFF00FF00>");
const TCHAR* szRed = _T("<color=0xFFFF0000>");
const TCHAR* szOrange = _T("<color=0xFFFF6600>");
const TCHAR* szYellow = _T("<color=0xFFFFFF00>");
const TCHAR* szGuildColor = _T("<color=0xFFFFF2A4>");
TCHAR szRoleFont[X_SHORT_NAME] = _T("<font=%s,0,14,0>");
TCHAR szRoleOtherFont[X_SHORT_NAME] = _T("<font=%s,0,12,0>");

MouseIntoTipsFrame::MouseIntoTipsFrame( void ):m_Trunk(this)
{
	m_dwMouseIntoID		= GT_INVALID;
	m_pWnd				= NULL;
	m_pStcTips			= NULL;
}

MouseIntoTipsFrame::~MouseIntoTipsFrame( void )
{

}

BOOL MouseIntoTipsFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);

	GUIWnd* pDesktop = m_pGUI->GetDesktop();
	m_clientPoint.x = pDesktop->GetSize().x-200;
	m_clientPoint.y = pDesktop->GetSize().y-150;
	return bRet;
}

BOOL MouseIntoTipsFrame::Destroy()
{
	BOOL bRet = GameFrame::Destroy();

	if(P_VALID(m_pWnd))
	{
		m_pGUI->DestroyWnd(m_pWnd);
		m_pWnd = NULL;
	}

	return bRet;
}

BOOL MouseIntoTipsFrame::ReloadUI()
{
	XmlElement ele;
	tstring strPath = g_strLocalPath + _T("\\ui\\tips.xml");
	m_pGUI->LoadXml(&ele, "VFS_System", strPath.c_str());

	m_pGUI->ChangeXml(&ele, "swin\\tip", "ClassName", "ToolTipStatic");
	m_pGUI->ChangeXml(&ele, "swin", "BottomMost", "yes");

	// 创建
	m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &ele);
	m_pStcTips = (ToolTipStatic*)m_pWnd->GetChild(_T("tip"));

	m_pWnd->SetInvisible(true);

	return TRUE;
}

VOID MouseIntoTipsFrame::Update()
{
	CombatSys* pCombatSys = (CombatSys*)m_pMgr->GetFrame(_T("CombatSys"));
	if( P_VALID( pCombatSys ) )
	{
		m_dwMouseIntoID = pCombatSys->GetCurTargetID();
	}
	
	//鼠标指向高于选中
	DWORD dwMouseIntoID = GetWorldPickerFrm()->GetCurMousePointRoleID();
	if( dwMouseIntoID!=GT_INVALID )
		m_dwMouseIntoID = dwMouseIntoID;
	tstringstream strInfo;

	//玩家
	if( IS_PLAYER(m_dwMouseIntoID) )
	{
		Role* pRole = RoleMgr::Inst()->FindRole(m_dwMouseIntoID);
		if( P_VALID(pRole) )
		{
			//名字
			tstring strName = PlayerNameTab::Inst()->FindNameByID(m_dwMouseIntoID);				
			strInfo << GetMouseIntoRoleNameFont() << szGreen << strName.c_str() << _T("\\n");

			//帮派
			tstring strGuild;
			Player* pPlayer = (Player*)pRole;
			DWORD dwGuildID = pPlayer->GetRoleGuildID();
			strGuild = GuildMgr::Inst()->GetGuildName(dwGuildID);
			if( !strGuild.empty() )
				strInfo <<  GetMouseIntoRoleOtherFont() << szGuildColor << strGuild.c_str() << _T("\\n");

			//等级
			strInfo << GetMouseIntoRoleOtherFont() << szGuildColor << g_StrTable[_T("SelectRole_Lvl")] << szGreen << pPlayer->GetRoleLevel() << szGuildColor << _T("  ") << g_StrTable[_T("Mouse_Into_Info0")];
		}
	}
	else if(IS_CREATURE(m_dwMouseIntoID))
	{
		Role* pRole = RoleMgr::Inst()->FindRole(m_dwMouseIntoID);
		if( !P_VALID(pRole) )
			return;
		NPC* pNpc = (NPC*)pRole;
		const tagCreatureProto* pProto = CreatureData::Inst()->FindNpcAtt( pNpc->GetTypeID() );
		if( !P_VALID(pProto) )
			return;
		//NPC
		if( pProto->eType==ECT_NPC )
		{
			tstring strName = pProto->szName;
			strInfo << GetMouseIntoRoleNameFont() << szGreen << strName.c_str() << _T("\\n");

			//等级
			strInfo << GetMouseIntoRoleOtherFont() << szGuildColor <<g_StrTable[_T("SelectRole_Lvl")] << szGreen << pNpc->GetRoleLevel() << szGuildColor << _T(" ") << pProto->szTitle << _T(" ") << g_StrTable[_T("Mouse_Into_Info1")] << _T("\\n");

			// 是否可攻击
			BOOL bAttack = FALSE;
			const tagCreatureProto* pProto = CreatureData::Inst()->FindNpcAtt(pNpc->GetTypeID());
			if( P_VALID(pProto) )
			{
				bAttack = pProto->bCanBeAttack;
			}
			if( bAttack )
				strInfo << szGreen <<g_StrTable[_T("Mouse_Into_Info3")];
			else 
				strInfo << szRed << g_StrTable[_T("Mouse_Into_Info4")];
		}
		//怪
		else if( pProto->eType==ECT_Monster && (pProto->nType2==EMTT_Normal || pProto->nType2==EMTT_Nest || pProto->nType2==EMTT_Team ) )
		{

			tstring strName = pProto->szName;
			INT nIn = CreatureData::Inst()->FindCreatureInspiration(pNpc->GetTypeID());
			tstring szMonsterColor = GetMouseIntoMonsterColor(nIn);
			strInfo << GetMouseIntoRoleNameFont() << szMonsterColor.c_str() << strName.c_str() << _T("\\n");

			//等级
			TCHAR szBuff[X_SHORT_NAME] = {0};
			_sntprintf( szBuff, X_LONG_NAME, _T("ERank%d"), pProto->eLite );
			strInfo << GetMouseIntoRoleOtherFont() << szGuildColor << g_StrTable[_T("SelectRole_Lvl")] << szGreen << pNpc->GetRoleLevel() << szGuildColor << _T(" ") << pProto->szTitle << _T(" ") << g_StrTable[szBuff] << _T(" ") << g_StrTable[_T("Mouse_Into_Info2")] << _T("\\n");

			// 是否可攻击
			BOOL bAttack = FALSE;
			const tagCreatureProto* pProto = CreatureData::Inst()->FindNpcAtt(pNpc->GetTypeID());
			if( P_VALID(pProto) )
			{
				bAttack = pProto->bCanBeAttack;
			}
			if( bAttack )
				strInfo << szGreen <<g_StrTable[_T("Mouse_Into_Info3")];
			else 
				strInfo << szRed << g_StrTable[_T("Mouse_Into_Info4")];
		}
		//资源
		else if( pProto->eType==ECT_GameObject && pProto->nType2==EGOT_Gather )
		{
			tstring strName = pProto->szName;
			strInfo << GetMouseIntoRoleNameFont() << szGreen << strName.c_str() << _T("\\n");

			//等级
			TCHAR szBuff[X_SHORT_NAME] = {0};
			_sntprintf( szBuff, X_LONG_NAME, _T("EGatherType%d"), pProto->nType3 );
			strInfo << GetMouseIntoRoleOtherFont() << szGuildColor << g_StrTable[_T("SelectRole_Lvl")] << szGreen << pNpc->GetRoleLevel() << szGuildColor << _T(" ") << g_StrTable[szBuff] << _T(" ") << g_StrTable[_T("Mouse_Into_Info8")] << _T("\\n");

			// 是否可采集
			if( (pProto->nType3==EGT_Mine || pProto->nType3==EGT_Herb) && SkillMgr::Inst()->IsHaveProduceSkillByTypeID(9000001) )
				strInfo << szGreen <<g_StrTable[_T("Mouse_Into_Info5")];
			else if( (pProto->nType3==EGT_Food || pProto->nType3==EGT_Cloth) && SkillMgr::Inst()->IsHaveProduceSkillByTypeID(9000101) )
				strInfo << szGreen <<g_StrTable[_T("Mouse_Into_Info5")];
			else 
				strInfo << szRed << g_StrTable[_T("Mouse_Into_Info6")];
		}
		//宝箱
		else if( pProto->eType==ECT_GameObject && pProto->nType2==EGOT_Chest )
		{
			tstring strName = pProto->szName;
			strInfo << GetMouseIntoRoleNameFont() << szOrange << strName.c_str() << _T("\\n");

			strInfo << GetMouseIntoRoleOtherFont() << szGuildColor << pProto->szTitle << _T("\\n");

			strInfo << szGreen << g_StrTable[_T("Mouse_Into_Info7")];
		}
	}


	this->ShowTips( m_clientPoint, strInfo.str().c_str() );
	strInfo.clear();
}

tstring MouseIntoTipsFrame::GetMouseIntoRoleNameFont()
{
	TCHAR szBuff[X_SHORT_NAME] = {0};
	_sntprintf( szBuff, X_SHORT_NAME, szRoleFont, g_StrTable[_T("Font_Kaiti")] );
	tstring strRet = szBuff;
	return strRet;
}

tstring MouseIntoTipsFrame::GetMouseIntoRoleOtherFont()
{
	TCHAR szBuff[X_SHORT_NAME] = {0};
	_sntprintf( szBuff, X_SHORT_NAME, szRoleOtherFont, g_StrTable[_T("Font_Kaiti")] );
	tstring strRet = szBuff;
	return strRet;
}

tstring MouseIntoTipsFrame::GetMouseIntoIsAttackColor( DWORD dwType )
{
	switch( dwType )
	{
	case 0:
		return szGreen;
		break;
	case 1:
		return szRed;
		break;
	}
	return szGreen;
}

tstring MouseIntoTipsFrame::GetMouseIntoMonsterColor( INT nIn )
{
	if( nIn>0 )
		return szRed;

	return szYellow;
}

VOID MouseIntoTipsFrame::ShowTips( tagPoint ptPos, LPCTSTR szTip )
{
	if(szTip==NULL || _tcsnlen(szTip, sizeof(szTip))==0)
	{
		m_pWnd->SetInvisible(TRUE);
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
	//m_pWnd->SetTopMost(TRUE);
	//m_pWnd->FlipToTop();//显示在最上层保证能够看到
}

VOID MouseIntoTipsFrame::ResetWindowPoint( tagPoint ptSize, tagPoint ptPos )
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