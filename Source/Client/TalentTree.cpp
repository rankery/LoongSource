#include "stdafx.h"
#include "TalentTree.h"
#include "SkillProtoData.h"
#include "SkillMgr.h"
#include "RoleMgr.h"
#include "LocalPlayer.h"
#include "TalentButton.h"
#include "ToolTipCreator.h"
#include "ToolTipFrame.h"

const SIZE TALENT_FRAME_SIZE	=	{28, 18};
const SIZE TALENT_SPACE_SIZE	=	{14, 14};
const SIZE TALENT_LEAF_SIZE		=	{37, 37};
const SIZE TALENT_BIDDEN_SIZE	=	{17, 7};
const SIZE TALENT_BIDDEN_OFFSET =	{0,  39};
const SIZE TALENT_BRANCH_SIZE	=	{3,  9};
const SIZE TALENT_BRANCH_OFFSET =	{17, 42};
const SIZE TALENT_WINDOW_SIZE   =	{481,475};


TalentTree::TalentTree(void)
{
	for (INT i=0; i<ETT_End; ++i)
	{
		m_pWndTalent[i]=NULL;
	}

	m_nCurPage = GT_INVALID;
}

TalentTree::~TalentTree(void)
{
	
}

VOID TalentTree::Create(GUIWnd* pWnd, GUISystem* pGUI)
{
	// 创建前先清空数据
	FreeMap(m_mapTalent);
	FreeMap(m_mapTalentSkillEffect);
	FreeMap(m_mapRoleLevelEffect);
	FreeMap(m_mapRoleClassEffect);
	m_mapIndex.Clear();
	for (int i=0; i<ETT_End; ++i)
	{
		FreeMap(m_mapTalentPointEffect[i]);
		m_listActiveTalent[i].Clear();
	}
	tstring strPath = g_strLocalPath + _T("\\attdata\\talent_tree.xml");
	LoadTalentConfig(strPath.c_str());
	
	for(INT i=0; i<ETT_End; ++i)
	{
		//首先创建底部控件
		CreateWnd((ETalentType)i, pWnd, pGUI);
	}
	
	// 创建天资树
	DWORD dwID;
	tagTalentLeaf* pLeaf = NULL;
	m_mapTalent.ResetIterator();
	while (m_mapTalent.PeekNext(dwID, pLeaf))
	{
		ASSERT(pLeaf->eTalentType > ETT_Null && pLeaf->eTalentType < ETT_End);

		CreateLeaf(pLeaf, m_pWndTalent[pLeaf->eTalentType], pGUI);
		
		if (P_VALID(pLeaf->dwChildLeaf))
		{
			CreateBranch(pLeaf, m_mapTalent.Peek(pLeaf->dwChildLeaf)->Pos, m_pWndTalent[pLeaf->eTalentType], pGUI);
		}

		/*m_mapTalent.ChangeValue(dwID, pLeaf);*/

		// 创建dwID
		ASSERT(P_VALID(pLeaf->pBtnTalentLeaf));
		m_mapIndex.Add(pLeaf->pBtnTalentLeaf->GetID(), dwID);
	}		
	
}

VOID TalentTree::Destroy()
{
	// 清空数据
	FreeMap(m_mapTalent);
	FreeMap(m_mapTalentSkillEffect);
	FreeMap(m_mapRoleLevelEffect);
	FreeMap(m_mapRoleClassEffect);
	m_mapIndex.Clear();
	for (int i=0; i<ETT_End; ++i)
	{
		FreeMap(m_mapTalentPointEffect[i]);
		m_listActiveTalent[i].Clear();
	}

	m_nCurPage = GT_INVALID;
}

VOID TalentTree::LoadTalentConfig(LPCTSTR szFile)
{
	XmlDocument doc;
	if( !doc.LoadFile("VFS_System", szFile) )
		return;

	XmlHandle docHandle( &doc );
	XmlHandle eleHandle = docHandle.FirstChildElement("desktop").FirstChildElement("skill");
	XmlElement* pElement = eleHandle.Element();
	if( !pElement )	
		return;

	while( pElement )
	{
		LoadTalentLeaf(pElement);
		pElement = pElement->NextSiblingElement("skill");
	}
}

VOID TalentTree::LoadTalentLeaf(XmlElement* pElement)
{
	XmlAttribute* pAttrib = pElement->FirstAttribute();
	tstring strValue;
	tagTalentLeaf* pLeaf = new tagTalentLeaf;

	while(pAttrib)
	{
		const std::string& strName = pAttrib->NameStr();
		strValue = m_pUtil->Unicode8ToUnicode(pAttrib->Value());

		if ( "id" == strName )
		{
			pLeaf->dwTalentLeaf = (DWORD)_ttoi(strValue.c_str());
			const tagSkillProtoClient* pSkill = SkillProtoData::Inst()->FindSkillProto(MTransSkillTypeID(pLeaf->dwTalentLeaf, 1));
			if(P_VALID(pSkill))
			{
				pLeaf->eTalentType = pSkill->eTalentType;
				pLeaf->eUseType = pSkill->eUseType;
				pLeaf->nMaxLevel = pSkill->nMaxLearnLevel;
				pLeaf->eLevelUpType = pSkill->eLevelUpType;
				
				//更新该技能所有等级的开启影响表
				for (INT i=1; i<=pSkill->nMaxLearnLevel; ++i)
				{
					AddTalentActiveEffect(MTransSkillTypeID(pLeaf->dwTalentLeaf, i));
				}

			}
			else
			{
				SAFE_DEL(pLeaf);
				return;
			}
		}
		else if( "pic" == strName )
		{
			_tcsncpy(pLeaf->szIconPath, strValue.c_str(), X_SHORT_NAME/sizeof(TCHAR));
		}
		else if( "follow_id" == strName )
		{
			pLeaf->dwChildLeaf = (DWORD)_ttoi(strValue.c_str());
		}
		else if( "col" == strName )
		{
			pLeaf->Pos.y = _ttoi(strValue.c_str()) - 1;
		}
		else if( "row" == strName )
		{
			pLeaf->Pos.x = _ttoi(strValue.c_str()) - 1;
		}

		pAttrib = pAttrib->Next();
	}

	m_mapTalent.Add(pLeaf->dwTalentLeaf, pLeaf);
}

VOID TalentTree::CreateWnd(ETalentType eType, GUIWnd* pFather, GUISystem* pGUI)
{
	XmlElement ele;
	CHAR szTmp[X_SHORT_NAME]={0};

	{
		sprintf(szTmp, "talentwindow%d", eType);
		ele.SetAttribute("Name",		szTmp);
		ele.SetAttribute("ClassName",	"CGUIStatic");
		ele.SetAttribute("Size_x",		TALENT_WINDOW_SIZE.cx);
		ele.SetAttribute("Size_y",		TALENT_WINDOW_SIZE.cy);
		ele.SetAttribute("LogicPos_x",	0);
		ele.SetAttribute("LogicPos_y",	0);
		ele.SetAttribute("PicColor",	0xFFFFFF);
	}
	m_pWndTalent[eType] = pGUI->CreateWnd(pFather->GetFullName().c_str(), &ele);
	ASSERT(P_VALID(m_pWndTalent[eType]));
	m_pWndTalent[eType]->SetInvisible(TRUE);
	
}

VOID TalentTree::CreateLeaf(tagTalentLeaf* pLeaf, GUIWnd* pFather, GUISystem* pGUI)
{
	XmlElement ele;
	CHAR szTemp[X_SHORT_NAME];
	TCHAR szTmp[X_LONG_NAME];

	SIZE LeafSize;
	GetCurLeafPos(LeafSize, pLeaf->Pos);

	// 创建天资按钮
	{
		sprintf(szTemp, "talentleaf%d%d", pLeaf->Pos.x, pLeaf->Pos.y);
		ele.SetAttribute("Name", szTemp);
		ele.SetAttribute("ClassName", "TalentButton");
		ele.SetAttribute("PicColor", 0xFF9A9A9A);
		ele.SetAttribute("PushPicColor", 0xFF9A9A9A);
		ele.SetAttribute("MovePicColor", 0xFF9A9A9A);
		ele.SetAttribute("Size_x", TALENT_LEAF_SIZE.cx);
		ele.SetAttribute("Size_y", TALENT_LEAF_SIZE.cy);
		if (pLeaf->eUseType == ESUT_Active)
		{
			ele.SetAttribute("LogicPos_x", LeafSize.cx - 1);
			ele.SetAttribute("LogicPos_y", LeafSize.cy - 1);
			ele.SetAttribute("PicFileName", "data\\ui\\Skill\\L-skill-icon-a.bmp");			
		}
		else if(pLeaf->eUseType == ESUT_Passive)
		{
			ele.SetAttribute("LogicPos_x", LeafSize.cx);
			ele.SetAttribute("LogicPos_y", LeafSize.cy);
			ele.SetAttribute("PicFileName", "data\\ui\\Skill\\L-skill-icon-b.bmp");
		}
		else 
		{
			return;
		}
	}
	pLeaf->pBtnTalentLeaf = (TalentButton*)pGUI->CreateWnd(pFather->GetFullName().c_str(), &ele);
	ASSERT(P_VALID(pLeaf->pBtnTalentLeaf));
	// 设置天资图标
	_stprintf(szTmp, _T("data\\ui\\Icon\\%s.tga"), pLeaf->szIconPath);
	pLeaf->pBtnTalentLeaf->SetIcon(szTmp);
	if (pLeaf->eUseType == ESUT_Active)
	{
		pLeaf->pBtnTalentLeaf->SetAlphaTex(_T("data\\ui\\Skill\\L-skill-icon-c.bmp"));			
	}
	else
	{
		pLeaf->pBtnTalentLeaf->SetAlphaTex(_T("data\\ui\\Skill\\L-skill-icon-d.bmp"));
	}

	ele.SetAttribute("PicFileName", "");
	
	// 创建天资已投点数
	{
		sprintf(szTemp, "talentbidden%d%d", pLeaf->Pos.x, pLeaf->Pos.y);
		ele.SetAttribute("Name", szTemp);
		ele.SetAttribute("ClassName", "CGUIStatic");
		ele.SetAttribute("Size_x", TALENT_BIDDEN_SIZE.cx);
		ele.SetAttribute("Size_y", TALENT_BIDDEN_SIZE.cy);
		ele.SetAttribute("LogicPos_x", LeafSize.cx + TALENT_BIDDEN_OFFSET.cx);
		ele.SetAttribute("LogicPos_y", LeafSize.cy + TALENT_BIDDEN_OFFSET.cy);
		ele.SetAttribute("PicColor", 0xFFFFFF);
		ele.SetAttribute("Font", m_pUtil->UnicodeToUnicode8(g_StrTable[_T("Font_Kaiti")]));
		ele.SetAttribute("FontHeight", 12);
		ele.SetAttribute("TextRect_left", 0);
		ele.SetAttribute("TextRect_top", 0);
		ele.SetAttribute("TextRect_right", TALENT_BIDDEN_SIZE.cx);
		ele.SetAttribute("TextRect_bottom", TALENT_BIDDEN_SIZE.cy);
		ele.SetAttribute("TextColor", 0xFF9A9A9A);
		ele.SetAttribute("TextAlign", "RightCenter");
	}	
	pLeaf->pStcBidden = (GUIStatic*)pGUI->CreateWnd(pFather->GetFullName().c_str(), &ele);
	ASSERT(P_VALID(pLeaf->pStcBidden));

	// 创建天资最大可投点数
	{
		_snprintf(szTemp, sizeof(szTemp), "talentmaxbid%d%d", pLeaf->Pos.x, pLeaf->Pos.y);
		ele.SetAttribute("Name", szTemp);
		ele.SetAttribute("LogicPos_x", LeafSize.cx + TALENT_BIDDEN_OFFSET.cx + TALENT_BIDDEN_SIZE.cx);
		ele.SetAttribute("LogicPos_y", LeafSize.cy + TALENT_BIDDEN_OFFSET.cy);
		ele.SetAttribute("TextAlign", "LeftCenter");
	}
	pLeaf->pStcMaxPoint = (GUIStatic*)pGUI->CreateWnd(pFather->GetFullName().c_str(), &ele);
	ASSERT(P_VALID(pLeaf->pStcMaxPoint));
	
	//--填写最大可投点
	_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("/%d"), pLeaf->nMaxLevel);
	pLeaf->pStcMaxPoint->SetText(szTmp);
	
	//--刷新当前可投点数
	pLeaf->bActive = IsTalentLeafActive(pLeaf);
	ReflashActiveTalentList(pLeaf);
	SetBidden(pLeaf);
}	

VOID TalentTree::CreateBranch(tagTalentLeaf* pLeaf, tagPoint ChildPos, GUIWnd* pFather, GUISystem* pGUI)
{
	XmlElement ele;
	CHAR szTemp[X_SHORT_NAME];

    SIZE LeafSize = {0, 0};
	GetCurLeafPos(LeafSize, pLeaf->Pos);
    SIZE BranchSize = {0, 0};
	GetCurBranchSize(BranchSize, pLeaf->Pos, ChildPos);
	

	// 创建天资树枝
	{
		sprintf(szTemp, "talentbranch%d%d", pLeaf->Pos.x, pLeaf->Pos.y);
		ele.SetAttribute("Name", szTemp);
		ele.SetAttribute("ClassName", "CGUIStatic");
		ele.SetAttribute("Size_x", BranchSize.cx);
		ele.SetAttribute("Size_y", BranchSize.cy);
		ele.SetAttribute("LogicPos_x", LeafSize.cx + TALENT_BRANCH_OFFSET.cx);
		ele.SetAttribute("LogicPos_y", LeafSize.cy + TALENT_BRANCH_OFFSET.cy);
		ele.SetAttribute("PicColor", 0xFFFFFFFF);
		ele.SetAttribute("PicFileName", "data\\ui\\Skill\\L-skill-tiao.bmp");
	}
	pLeaf->pStcTalentBranch = (GUIStatic*)pGUI->CreateWnd(pFather->GetFullName().c_str(), &ele);
	ASSERT(P_VALID(pLeaf->pStcTalentBranch));
}

VOID TalentTree::GetCurLeafPos(SIZE& size, tagPoint pos)
{
	size.cx = TALENT_FRAME_SIZE.cx 
		+ pos.x * TALENT_LEAF_SIZE.cx 
		+ pos.x * TALENT_SPACE_SIZE.cx;
	size.cy = TALENT_FRAME_SIZE.cy 
		+ pos.y * TALENT_LEAF_SIZE.cy 
		+ pos.y * TALENT_SPACE_SIZE.cy;
}

VOID TalentTree::GetCurBranchSize(SIZE& size, tagPoint pos, tagPoint child)
{
	INT col,row;
	col = child.x - pos.x;
	row = child.y - pos.y;
	if (0 == col)
	{
		if (0 < row)
		{
			size.cx = TALENT_BRANCH_SIZE.cx;
			size.cy = TALENT_BRANCH_SIZE.cy * row + (TALENT_LEAF_SIZE.cy + TALENT_SPACE_SIZE.cy) * (row - 1);
		}
	}
}

VOID TalentTree::SetCurPage( INT nPage )
{
	ASSERT(nPage > ETT_Null && nPage < ETT_End);
	// 如果原有当前页为有效值则隐藏原有当前页
	if(ETT_Null < m_nCurPage && ETT_End > m_nCurPage)
	{
		m_pWndTalent[m_nCurPage]->SetInvisible(TRUE);
	}
	
	// 显示当前页
	m_nCurPage = nPage;
	m_pWndTalent[m_nCurPage]->SetInvisible(FALSE);
	m_pWndTalent[m_nCurPage]->SetRefresh(TRUE);
	m_pWndTalent[m_nCurPage]->FlipToTop();

}

VOID TalentTree::UpdateTalentLeaf( DWORD dwID, BOOL bAdd )
{
	const tagSkillData* pSkill = SkillMgr::Inst()->GetSkillData(dwID);
	// 非天资技能不更新
	if(P_VALID(pSkill))
	{
		if(ETT_Null != pSkill->pProto->eTalentType && ETT_End != pSkill->pProto->eTalentType )
		{
			tagTalentLeaf* pLeaf = m_mapTalent.Peek(dwID);
			if(P_VALID(pLeaf))
			{
				pLeaf->nCurLevel = pSkill->nLevel;
				pLeaf->nLearnLevel = pSkill->nLearnLevel;
				pLeaf->eLevelUpType = pSkill->pProto->eLevelUpType;
				
				pLeaf->bActive = IsTalentLeafActive(pLeaf);
				ReflashActiveTalentList(pLeaf);
				SetBidden(pLeaf);
				
				// 更新他的附属技能
				if(bAdd && 1 != pSkill->nLearnLevel)
				{
					//如果是新加，且技能不是新学会，则需要更新一下该技能的第一级
					OnTalentLevelUp(MTransSkillTypeID(pSkill->dwSkillID, 1));
				}
				OnTalentLevelUp(MTransSkillTypeID(pSkill->dwSkillID, pSkill->nLearnLevel));
				
				//刷新天资Tips
				ReflashTalentTips(pLeaf);
			}
		}
	}
	else
	{
		//如果技能不存在且技能为
		tagTalentLeaf* pLeaf = m_mapTalent.Peek(dwID);
		if(P_VALID(pLeaf))
		{
			pLeaf->nCurLevel = 0;
			pLeaf->nLearnLevel = 0;

			pLeaf->bActive = IsTalentLeafActive(pLeaf);
			ReflashActiveTalentList(pLeaf);
			SetBidden(pLeaf);

			//刷新天资Tips
			ReflashTalentTips(pLeaf);
		}
	}
}

tagTalentLeaf* TalentTree::GetCurLeaf( DWORD dwWndID )
{
	DWORD dwSkillID = m_mapIndex.Peek(dwWndID);
	return m_mapTalent.Peek(dwSkillID);
	
}

VOID TalentTree::SetBidden(tagTalentLeaf* pLeaf)
{
	//--根据是否投点来设置天资图标的颜色
	if(0 < pLeaf->nLearnLevel)
	{
		pLeaf->pBtnTalentLeaf->SetPicColor(0xFFFFFFFF);
	}
	else
	{
		pLeaf->pBtnTalentLeaf->SetPicColor(0xFF9A9A9A);
	}

	//--根据是否可投点来设置已投点数和可投点数的颜色
	if(pLeaf->bActive)
	{
		pLeaf->pStcBidden->SetTextColor(0xFF00FF00);
		pLeaf->pStcMaxPoint->SetTextColor(0xFF00FF00);
	}
	else
	{
		//--根据习得等级来设置不可投点情况下的已投点数和可投点数的颜色
		if(0 == pLeaf->nLearnLevel)
		{
			pLeaf->pStcBidden->SetTextColor(0xFF9A9A9A);
			pLeaf->pStcMaxPoint->SetTextColor(0xFF9A9A9A);
		}
		else if(pLeaf->nMaxLevel == pLeaf->nLearnLevel)
		{
			pLeaf->pStcBidden->SetTextColor(0xFFFFFF00);
			pLeaf->pStcMaxPoint->SetTextColor(0xFFFFFF00);
		}
		else
		{
			pLeaf->pStcBidden->SetTextColor(0xFF00FF00);
			pLeaf->pStcMaxPoint->SetTextColor(0xFF00FF00);
		}
	}
		
	//--根据当前等级和习得等级重新设置已投点数的颜色
	if(pLeaf->nCurLevel > pLeaf->nLearnLevel)
	{
		pLeaf->pStcBidden->SetTextColor(0xFF00FFFF);
	}
	else if(pLeaf->nCurLevel < pLeaf->nLearnLevel)
	{
		pLeaf->pStcBidden->SetTextColor(0xFFFF0000);	
	}
	
	//--设置可投点数的数值
	TCHAR szTmp[X_SHORT_NAME]={0};
	_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("%d"), pLeaf->nCurLevel);
	pLeaf->pStcBidden->SetText(szTmp);

	pLeaf->pBtnTalentLeaf->SetRefresh();
	pLeaf->pStcBidden->SetRefresh(TRUE);
	pLeaf->pStcMaxPoint->SetRefresh();

}

template<typename K, typename V>
VOID TalentTree::FreeMap( TMap<K, V*> &mapList )
{
	V* pNode = NULL;

	mapList.ResetIterator();
	while (mapList.PeekNext(pNode))
	{
		SAFE_DEL(pNode);
	}

	mapList.Clear();
}

template<typename A, typename B>
VOID TalentTree::AddNode( TMap<A, TList<B>*> &mapList, A key, B value )
{
	TList<B>* pList = mapList.Peek(key);
	if(P_VALID(pList))
	{
		pList->PushBack(value);
	}
	else
	{
		pList = new TList<B>;
		pList->PushBack(value);
		mapList.Add(key, pList);
	}
}

VOID TalentTree::AddTalentActiveEffect( DWORD dwTypeID )
{
	const tagSkillProtoClient* pSkill = SkillProtoData::Inst()->FindSkillProto(dwTypeID);
	if(!P_VALID(pSkill))
		return;

	// 写入技能等级对后续技能开启的影响
	if(GT_INVALID != pSkill->dwPreLevelSkillID)
	{
		AddNode(m_mapTalentSkillEffect, pSkill->dwPreLevelSkillID, pSkill->dwID);
	}
	// 写入当前天资点数对技能开启的影响
	if(0 != pSkill->nNeedTalentPoint)
	{
		ASSERT(pSkill->eTalentType > ETT_Null && pSkill->eTalentType < ETT_End);
		AddNode(m_mapTalentPointEffect[pSkill->eTalentType], pSkill->nNeedTalentPoint, pSkill->dwID);
	}
	// 写入玩家等级对技能开启的影响
	if(0 != pSkill->nNeedRoleLevel && 1 != pSkill->nNeedRoleLevel)
	{
		AddNode(m_mapRoleLevelEffect, pSkill->nNeedRoleLevel, pSkill->dwID);
	}
	//// 写入玩家职业对技能开启的影响
	//if(EV_Base != pSkill->eNeedClassType)
	//{
	//	AddNode(m_mapRoleClassEffect, pSkill->eNeedClassType, pSkill->dwID);
	//}
}

BOOL TalentTree::IsTalentLeafActive(tagTalentLeaf* pLeaf)
{
	//--是否有可投点数
	//if(0 >= pLp->GetAttribute(ERA_TalentPoint))
	//	return FALSE;

	//--是否为三个天资范畴
	//if(!SkillMgr::Inst()->IsInsideOfMaxTalent(pLeaf->eTalentType))
	//	return FALSE;

	//--技能升级条件类型不是固定,则不可投点
	if(ESLUT_Fixed != pLeaf->eLevelUpType)
		return FALSE;
	
	//--如果技能已到顶级，则不可再投
	if(pLeaf->nMaxLevel <= pLeaf->nLearnLevel)
		return FALSE;
	
	//--判断下一级技能是否可学
	if(E_Success != SkillMgr::Inst()->IsSkillCanLearn(MTransSkillTypeID(pLeaf->dwTalentLeaf, pLeaf->nLearnLevel+1)))
		return FALSE;
	
	return TRUE;
}

VOID TalentTree::OnRoleLevelUp( INT nLevel, INT nInc )
{
	//只处理升级，不处理降级
	TList<DWORD>* pList;
	for(INT i=nLevel-nInc; i<nLevel; ++i)
	{
		pList = m_mapRoleLevelEffect.Peek(i+1);
		if(P_VALID(pList))
		{
			DWORD dwID;
			pList->ResetIterator();
			while (pList->PeekNext(dwID))
			{
				tagTalentLeaf* pLeaf = m_mapTalent.Peek(MTransSkillID(dwID));
				if(P_VALID(pLeaf) && dwID == GetNextLevelTalentTypeID(pLeaf))
				{
					pLeaf->bActive = IsTalentLeafActive(pLeaf);
					ReflashActiveTalentList(pLeaf);
					SetBidden(pLeaf);
				}
			}
		}
	}
}

VOID TalentTree::OnRoleClassChange( EClassType eCur, EClassType ePast )
{
	TList<DWORD>* pList = m_mapRoleClassEffect.Peek(eCur);
	DWORD dwID;
	if(P_VALID(pList))
	{
		pList->ResetIterator();
		while (pList->PeekNext(dwID))
		{
			tagTalentLeaf* pLeaf = m_mapTalent.Peek(MTransSkillID(dwID));
			if(P_VALID(pLeaf) && dwID == GetNextLevelTalentTypeID(pLeaf))
			{
				pLeaf->bActive = IsTalentLeafActive(pLeaf);
				ReflashActiveTalentList(pLeaf);
				SetBidden(pLeaf);
			}
		}
	}
	
	pList = m_mapRoleClassEffect.Peek(ePast);
	if(P_VALID(pList))
	{
		pList->ResetIterator();
		while (pList->PeekNext(dwID))
		{
			tagTalentLeaf* pLeaf = m_mapTalent.Peek(MTransSkillID(dwID));
			if(P_VALID(pLeaf) && dwID == GetNextLevelTalentTypeID(pLeaf))
			{
				pLeaf->bActive = IsTalentLeafActive(pLeaf);
				ReflashActiveTalentList(pLeaf);
				SetBidden(pLeaf);
			}
		}
	}
}

VOID TalentTree::OnTalentPointChange( ETalentType eTalent, INT nCurPoint )
{
	if(0 != nCurPoint)// 天资点数变为零需要集中处理，这里不处理
	{
		TList<DWORD>* pList = m_mapTalentPointEffect[eTalent].Peek(nCurPoint);
		if(P_VALID(pList))
		{
			DWORD dwID;
			pList->ResetIterator();
			while (pList->PeekNext(dwID))
			{
				tagTalentLeaf* pLeaf = m_mapTalent.Peek(MTransSkillID(dwID));
				if(P_VALID(pLeaf) && dwID == GetNextLevelTalentTypeID(pLeaf))
				{
					pLeaf->bActive = IsTalentLeafActive(pLeaf);
					ReflashActiveTalentList(pLeaf);
					SetBidden(pLeaf);
				}
			}
		}
	}
}

VOID TalentTree::OnTalentLevelUp( DWORD dwPreTalentID )
{
	TList<DWORD>* pList = m_mapTalentSkillEffect.Peek(dwPreTalentID);
	if(P_VALID(pList))
	{
		DWORD dwID;
		pList->ResetIterator();
		while (pList->PeekNext(dwID))
		{
			tagTalentLeaf* pLeaf = m_mapTalent.Peek(MTransSkillID(dwID));
			if(P_VALID(pLeaf) && dwID == GetNextLevelTalentTypeID(pLeaf))
			{
				pLeaf->bActive = IsTalentLeafActive(pLeaf);
				ReflashActiveTalentList(pLeaf);
				SetBidden(pLeaf);
			}
		}
	}
}

VOID TalentTree::OnNoTalentPoint()
{
	DWORD dwID;
	for(INT i=0; i<ETT_End; ++i)
	{
		// 将
		m_listActiveTalent[i].ResetIterator();
		while(m_listActiveTalent[i].PeekNext(dwID))
		{
			tagTalentLeaf* pLeaf = m_mapTalent.Peek(dwID);
			if(P_VALID(pLeaf))
			{
				pLeaf->bActive = FALSE;
				SetBidden(pLeaf);
			}
		}
	}
	
}

VOID TalentTree::OnNoTalentTypeCanChoose()
{
	DWORD dwID;
	for(INT i=0; i<ETT_End; ++i)
	{
		if(!SkillMgr::Inst()->IsInsideOfMaxTalent((ETalentType)i))
		{
			m_listActiveTalent[i].ResetIterator();
			while(m_listActiveTalent[i].PeekNext(dwID))
			{
				tagTalentLeaf* pLeaf = m_mapTalent.Peek(dwID);
				if(P_VALID(pLeaf))
				{
					pLeaf->bActive = FALSE;
					SetBidden(pLeaf);
				}
			}
		}
	}
}


VOID TalentTree::OnHaveTalentPoint()
{
	DWORD dwID;
	for(INT i=0; i<ETT_End; ++i)
	{
		if(SkillMgr::Inst()->IsInsideOfMaxTalent((ETalentType)i))
		{
			m_listActiveTalent[i].ResetIterator();
			while(m_listActiveTalent[i].PeekNext(dwID))
			{
				tagTalentLeaf* pLeaf = m_mapTalent.Peek(dwID);
				if(P_VALID(pLeaf))
				{
					pLeaf->bActive = TRUE;
					SetBidden(pLeaf);
				}
			}
		}
	}
}

VOID TalentTree::ReflashActiveTalentList( tagTalentLeaf* pLeaf )
{
	BOOL bRet = m_listActiveTalent[pLeaf->eTalentType].IsExist(pLeaf->dwTalentLeaf);
	if(bRet)
	{
		if(!pLeaf->bActive)
		{
			m_listActiveTalent[pLeaf->eTalentType].Erase(pLeaf->dwTalentLeaf);
		}
	}
	else
	{
		if(pLeaf->bActive)
		{
			m_listActiveTalent[pLeaf->eTalentType].PushBack(pLeaf->dwTalentLeaf);
		}
	}
}

DWORD TalentTree::GetNextLevelTalentTypeID( tagTalentLeaf* pLeaf )
{
	if(pLeaf->nCurLevel <= pLeaf->nLearnLevel)
	{
		return MTransSkillTypeID(pLeaf->dwTalentLeaf, pLeaf->nCurLevel + 1);
	}
	return GT_INVALID;
}

VOID TalentTree::ReflashTalentTips( tagTalentLeaf* pLeaf )
{
	//如果鼠标在天资技能按钮上，则重新显示Tips
	if( P_VALID(pLeaf) 
		&& P_VALID(pLeaf->pBtnTalentLeaf)
		&& pLeaf->pBtnTalentLeaf == TObjRef<GUISystem>()->GetMousePoint() )
	{
		tstring strTips;
		if(0 == pLeaf->nCurLevel)
		{
			strTips = ToolTipCreator::Inst()->GetTalentTips(MTransSkillTypeID(pLeaf->dwTalentLeaf, 1));
		}
		else
		{
			strTips = ToolTipCreator::Inst()->GetTalentTips(MTransSkillTypeID(pLeaf->dwTalentLeaf, pLeaf->nCurLevel));
		}
		ShowTip(pLeaf->pBtnTalentLeaf, strTips.c_str());	
	}
}

VOID TalentTree::OnClearAllTalent()
{
	tagTalentLeaf* pLeaf = NULL;
	m_mapTalent.ResetIterator();
	while (m_mapTalent.PeekNext(pLeaf))
	{
		//--刷新当前可投点数
		pLeaf->bActive = IsTalentLeafActive(pLeaf);
		ReflashActiveTalentList(pLeaf);
		SetBidden(pLeaf);
	}
}