#include "stdafx.h"
#include "RoleHeadTipsFrame.h"
#include "WorldFrame.h"
#include "Role.h"
#include "NPC.h"
#include "LocalPlayer.h"
#include "RoleMgr.h"
#include "Pet.h"
#include "PetManager.h"
#include "PlayerNameTab.h"
#include "CreatureData.h"
#include "PlayerStateFrame.h"
#include "WorldPickerFrame.h"
#include "QuestMgr.h"
#include "StallMgr.h"
#include "GameSet.h"
#include "GuildMgr.h"
#include "VipNetBarMgr.h"
#include "CombatSys.h"
#include "TeamSys.h"

const Color4ub NPCNameColor = Color4ub(255,138,248,255);			//NPC名称,

//PK状态
const Color4ub PlayerPeaceColor = Color4ub(255,255,255,255);		//和平
const Color4ub PlayerWarnningColor = Color4ub(255,255,228,0);		//警告
const Color4ub PlayerWickedColor = Color4ub(255,255,150,0);			//邪恶
const Color4ub PlayerWantedColor = Color4ub(255,197,0,254);			//通缉

//角色状态
const Color4ub PlayerSafeguardColor = Color4ub(255,154,234,133);	//被保护
const Color4ub PlayerPKColor = Color4ub(255,255,54,0);				//行凶

//队友提示贴图
const TCHAR* szTeam = _T("data\\ui\\main\\L_main-leader-c.bmp");

const float scalePerUnit = 2.0e-4f;//每个世界单位产生的缩放比例

class NpcNameColorTable
{
	enum MNCTConst
	{
		NumColor=5,
	};
private:
	Color4ub m_colorTab[NumColor];

public:
	NpcNameColorTable()
	{
		m_colorTab[0]=Color4ub(255,255,255,255);	//同级（等级+1，等级-1）怪物名
		m_colorTab[1]=Color4ub(255,255,188,0);		//稍高级（等级+2，等级+3）怪物名
		m_colorTab[2]=Color4ub(255,255,66,0);		//高级（等级高3级以上，不包括高3级）怪物名
		m_colorTab[3]=Color4ub(255,81,220,62);		//稍低级（等级-2，等级-3）怪物名，
		m_colorTab[4]=Color4ub(255,174,174,174);	//低级（等级低于3级，不包括低3级）怪物名
	}
	~NpcNameColorTable()	{}

	Color4ub GetColor(int monsterLv)
	{
		LocalPlayer* pLocalPlayer = RoleMgr::Inst()->GetLocalPlayer();
		int lpLv = pLocalPlayer->GetRoleLevel();		//玩家等级

		int diff=monsterLv-lpLv;
		if(diff==0
			|| diff==1
			|| diff==-1)
		{
			return m_colorTab[0];
		}
		if(diff>1)
		{
				if(diff>3)
					return m_colorTab[2];
				else 
					return m_colorTab[1];
		}
		if(diff<-1)
		{
			if(diff<-3)
				return m_colorTab[4];
			else 
				return m_colorTab[3];
		}

		return m_colorTab[0];
	}
};

NpcNameColorTable g_npcColorTab;

class RoleHeadTipSortFunc
{
public:
	bool operator() (const tagHeadTip* tip1,const tagHeadTip* tip2)
	{
		return tip1->scrPos.z > tip2->scrPos.z;
	}
};


RoleHeadTipsFrame::RoleHeadTipsFrame( void ):m_Trunk(this)
{
	m_pFont				= NULL;
	m_bShowTips			= TRUE;
	m_bShowHPorMP		= TRUE;
}

RoleHeadTipsFrame::~RoleHeadTipsFrame( void )
{
	
}

BOOL RoleHeadTipsFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{

	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	Create();

	m_pKeyMap->Register(0, g_StrTable[_T("HotKey_HPorMP")],	
		(INPUTMAPEVENTHANDLE)m_Trunk.sfp2(&RoleHeadTipsFrame::OnKey), DIK_HOME, TRUE, FALSE, 0);

	return TRUE;
}

BOOL RoleHeadTipsFrame::Destroy()
{
	SAFE_DELETE(m_pFont);
	SAFE_DELETE(m_pFontTitle);
	IDraw2D::Inst()->ReleaseTexture(m_pResHP);
	IDraw2D::Inst()->ReleaseTexture(m_pResMP);	
	ClearAllTips();
	return TRUE;
}

VOID RoleHeadTipsFrame::Render3D()
{
	if( m_bShowTips )
	{
		UpdateRoleHeadTips();
		DrawAllRoleHeadTips();
		m_questFlag.Update();
	}
}

VOID RoleHeadTipsFrame::OnEvent( tagGameEvent* pEvent )
{
	
}

VOID RoleHeadTipsFrame::Update()
{
	
}

void RoleHeadTipsFrame::Create()
{
	m_pFont = Device()->NewDepthFont();
	m_pFont->Create(8, 20, 0, FALSE, g_StrTable[_T("Font_MSYH")]);

	m_pFontTitle = Device()->NewDepthFont();
	m_pFontTitle->Create(6, 18, 0, FALSE, g_StrTable[_T("Font_MSYH")] );

	m_questFlag.Create();

	IFS* pIFS = (IFS*)(VOID*)TObjRef<VirtualFileSys>("VFS_System");
	m_pResHP = IDraw2D::Inst()->NewTexture(_T("data\\ui\\common\\L_jindu-glass-e-j.bmp"),pIFS);
	m_pResMP = IDraw2D::Inst()->NewTexture(_T("data\\ui\\common\\L_jindu-glass-d-j.bmp"),pIFS);
}

void RoleHeadTipsFrame::UpdateRoleHeadTips()
{
	ClearAllTips();
	ClientCamera* pCamera = GetWorldFrm()->GetCamera();
	const Vector3& lookFrom = pCamera->GetPos();
	Vector3 scrPos;
	Vector3	worldPos;
	const map<DWORD,Role*>& roleMap = RoleMgr::Inst()->GetRoleMap();
	for(map<DWORD,Role*>::const_iterator it=roleMap.begin();
		it!=roleMap.end(); ++it)
	{
		Role* pRole = const_cast<Role*>(it->second);

		// 视野之外的角色不显示头顶信息
		if( !RoleMgr::Inst()->IsInViewDist(pRole) )
			continue;

		pRole->GetHeadPos(worldPos);
		scrPos = pCamera->WorldToScreenF(worldPos);
		scrPos.z /= 2;
		if( scrPos.z<0.0f || scrPos.z>0.5f )
			continue;

		float dist = D3DXVec3Length(&(lookFrom-worldPos));
		float scale = 1.0f;
		if( dist > 6000.0f )
			continue;
	

		DWORD dwRoleID = pRole->GetID();
		tstring strRoleName ;
		tstring strRoleTitle;
		tstring strGuildTip;
		tstring strVipNetBarTip;
        tstring strSpouse;
		DWORD dwHostility = 0;

		EQuestFlag qf = EQF_Null;
		if( IS_PLAYER(dwRoleID) )
		{
			//开启显示玩家姓名,称号等
			if( GameSet::Inst()->Get(EGS_ShowOtherPlayerName) )
			{
				strRoleName = PlayerNameTab::Inst()->FindNameByID(dwRoleID);
				strRoleTitle = GetRoleTitleTips(pRole);
				strGuildTip =  GetRoleGuildTips(pRole);
				strVipNetBarTip = GetRoleVipNetBarTips(pRole);
				dwHostility = GetRoleHostilityTips(pRole);
                strSpouse = GetRoleSpouse(pRole);
			}
			//显示鼠标所指玩家姓名,称号等
			else if(GetWorldPickerFrm()->GetCurMousePointRoleID()==dwRoleID)
			{
				strRoleName = PlayerNameTab::Inst()->FindNameByID(dwRoleID);
				strRoleTitle = GetRoleTitleTips(pRole);
				strGuildTip =  GetRoleGuildTips(pRole);
				strVipNetBarTip = GetRoleVipNetBarTips(pRole);
				dwHostility = GetRoleHostilityTips(pRole);
                strSpouse = GetRoleSpouse(pRole);
			}
			//显示当前选中玩家姓名,称号等
			else 
			{
				CombatSys* pCombatSys = (CombatSys*)m_pMgr->GetFrame(_T("CombatSys"));
				if( !P_VALID( pCombatSys ) )
					continue;
				if( pCombatSys->GetCurTargetID() == dwRoleID )
				{
					strRoleName = PlayerNameTab::Inst()->FindNameByID(dwRoleID);
					strRoleTitle = GetRoleTitleTips(pRole);
					strGuildTip =  GetRoleGuildTips(pRole);
					strVipNetBarTip = GetRoleVipNetBarTips(pRole);
					dwHostility = GetRoleHostilityTips(pRole);
                    strSpouse = GetRoleSpouse(pRole);
				}
			}
		}
		else if( IS_CREATURE(dwRoleID) )
		{
			NPC* pNpc = (NPC*)pRole;
			const tagCreatureProto* pProto = CreatureData::Inst()->FindNpcAtt( pNpc->GetTypeID() );
			if( !P_VALID(pProto) )
				continue;
				
			if( pProto->eType==ECT_NPC )
			{	
				strRoleName = pProto->szName;
				strRoleTitle = GetRoleTitleTips(pRole);

				//任务提示
				if( QuestMgr::Inst()->GetQuery()->IsAcceptNPC(((NPC*)pRole)->GetTypeID()) )
				{
					qf = EQF_Accept;
					
					// 查看该NPC可接任务是否被过滤掉
					if (QuestMgr::Inst()->GetData()->IsNPCHasQuestByCurrentFilter(((NPC*)pRole)->GetTypeID()) == FALSE)
						qf = EQF_Null;
					// 是否根据等级过滤
					if (QuestMgr::Inst()->GetData()->IsFilterByPlayerLevel() == TRUE)
					{
						if (QuestMgr::Inst()->GetData()->IsNPCHasQuestByCurrentFilterByPlayerLevel(((NPC*)pRole)->GetTypeID()) == FALSE)
							qf = EQF_Null;
					}
				}
				if( QuestMgr::Inst()->GetQuery()->IsCompleteNPC(((NPC*)pRole)->GetTypeID()) 
					|| QuestMgr::Inst()->GetQuery()->IsUpdateNPC(((NPC*)pRole)->GetTypeID()) )
					qf = EQF_Finish;
			}
			//任务中采集地物在鼠标移上后应显示名称
			if( (pNpc->IsQuestInvesGO() || pNpc->IsQuestTalkGameObject() || pNpc->IsCommonInvesGO())
				&& QuestMgr::Inst()->GetQuery()->IsInvesObject(pProto->dwTypeID)
				&& GetWorldPickerFrm()->GetCurMousePointRoleID()==dwRoleID )
			{
				strRoleName = pProto->szName;
				strRoleTitle = GetRoleTitleTips(pRole);

				//任务提示
				if( QuestMgr::Inst()->GetQuery()->IsAcceptNPC(((NPC*)pRole)->GetTypeID()) )
					qf = EQF_Accept;
				if( QuestMgr::Inst()->GetQuery()->IsCompleteNPC(((NPC*)pRole)->GetTypeID()) 
					|| QuestMgr::Inst()->GetQuery()->IsUpdateNPC(((NPC*)pRole)->GetTypeID()) )
					qf = EQF_Finish;
			}
			else if( pProto->eType==ECT_Monster )
			{	
				//巢穴不显示,队长显示
				if( pProto->nType3==6 )
					continue;
				else if(GameSet::Inst()->Get(EGS_ShowMonsterName))
				{
					strRoleName = pProto->szName;
					strRoleTitle = GetRoleTitleTips(pRole);
				}
				else if( GetWorldPickerFrm()->GetCurMousePointRoleID()==dwRoleID )
				{
					strRoleName = pProto->szName;
					strRoleTitle = GetRoleTitleTips(pRole);
				}
				else 
				{
					CombatSys* pCombatSys = (CombatSys*)m_pMgr->GetFrame(_T("CombatSys"));
					if( !P_VALID( pCombatSys ) )
						continue;
					if( pCombatSys->GetCurTargetID() == dwRoleID )
					{
						strRoleName = pProto->szName;
						strRoleTitle = GetRoleTitleTips(pRole);
					}
				}
			}
			else if( pProto->eType==ECT_GameObject
				&& pProto->nType2==EGOT_Gather )
			{
				strRoleName = pProto->szName;
			}
		}
        else if (IS_PET(dwRoleID))
        {
            Pet* pPet = static_cast<Pet*>(pRole);

            // 只显示普通跟随态的模型名字，骑乘时不显示
            if (pPet->IsFSMEnabled())
            {
                const tagPetDispInfo* pDispInfo = NULL;
                if (pPet->IsAttributeInited())
                    pDispInfo = PetManager::Inst()->FindPetDispInfo(pPet->GetOwnerID(), dwRoleID);

                if (P_VALID(pDispInfo))
                {
                    strRoleName = pDispInfo->szName;
                }
            }
        }

		tagHeadTip *pTip = new tagHeadTip; 
		pRole->GetHeadPos(pTip->worldPos);
		pTip->roleID = dwRoleID;
		pTip->strNameTip = strRoleName;
		pTip->qeustFlag = qf;
		pTip->txtNameColor = GetRoleTipsColor(pRole);
		//added by hyu
		pTip->strTitleTip = strRoleTitle;
		pTip->txtTitleColor = GetRoleTitleColor(pRole);
		//end added by hyu
		pTip->strResTex = GetRoleTitleIcon(pRole);
		//added by hqzhang
		pTip->strGuildTip = strGuildTip;
		//end added by hqzhang
        //added by hyu
        pTip->strSpouse = strSpouse;
        pTip->spouseColor = GetRoleSpouseColor(pRole);

		pTip->strVipNetBarTip = strVipNetBarTip;
		pTip->dwHostility = dwHostility;

		m_vecTipsList.push_back(pTip);
	}

	if( GameSet::Inst()->Get(EGS_ShowSelfName) )
	{
		DWORD dwLocalID = RoleMgr::Inst()->GetLocalPlayerID();
		tstring strRoleName = PlayerNameTab::Inst()->FindNameByID(dwLocalID);

		LocalPlayer* pLocalPlayer = RoleMgr::Inst()->GetLocalPlayer();
		if( !P_VALID(pLocalPlayer) )
			return;

		tstring strRoleTitle = GetRoleTitleTips(pLocalPlayer);

		if(strRoleName==_T("") && strRoleTitle == _T(""))	//altered by hyu [if(strRoleName == _T(""))]
			return;

		tagHeadTip *pTip = new tagHeadTip;

		pLocalPlayer->GetHeadPos(pTip->worldPos);
		pTip->roleID = dwLocalID;
		pTip->strNameTip = strRoleName;
		pTip->qeustFlag = EQF_Null;
		pTip->txtNameColor = GetRoleTipsColor(pLocalPlayer);
		//added by hyu
		pTip->strTitleTip = strRoleTitle;
		pTip->txtTitleColor = GetRoleTitleColor(pLocalPlayer);
		//end added by hyu

		//added by hqzhang
		pTip->strGuildTip = GetRoleGuildTips(pLocalPlayer);
		//end added by hqzhang

		pTip->strVipNetBarTip = GetRoleVipNetBarTips(pLocalPlayer);
		pTip->dwHostility = GetRoleHostilityTips(pLocalPlayer);

        pTip->strSpouse = GetRoleSpouse(pLocalPlayer);
        pTip->spouseColor = GetRoleSpouseColor(pLocalPlayer);

		m_vecTipsList.push_back(pTip);
	}
}

void RoleHeadTipsFrame::DrawAllRoleHeadTips()
{
	float nowTime = Kernel::Inst()->GetAccumTime();
	ClientCamera* pCamera = GetWorldFrm()->GetCamera();
	const Vector3& lookFrom = pCamera->GetPos();
	
	//计算出屏幕坐标
	vector<tagHeadTip*>::iterator iter;
	for(iter=m_vecTipsList.begin(); iter!=m_vecTipsList.end(); ++iter)
	{
		tagHeadTip* tip = (*iter);
		tip->scrPos = pCamera->WorldToScreenF(tip->worldPos);
		tip->scrPos.z /= 2;

		tagPoint ptSize;
		m_pRender->GetTextSize(tip->strNameTip.c_str(), NULL, ptSize);

		float dist = D3DXVec3Length(&(lookFrom-tip->worldPos));
		float scale = 1.0f;
		if( dist > 2000.0f )
		{
			scale = 1.0f - (dist-2000.0f)*scalePerUnit;
			if( scale < 0.1f )
				scale = 0.1f;
		}
		tip->scale = scale;
	}

	//按照Z值排序
	RoleHeadTipSortFunc sortFunc;
	std::sort(m_vecTipsList.begin(), m_vecTipsList.end(), sortFunc);

	//依次画出
	int clientW=Device()->GetClientWidth();
	int clientH=Device()->GetClientHeight();
	
	m_pFont->Begin();
	m_pFontTitle->Begin();
	for(iter=m_vecTipsList.begin(); iter!=m_vecTipsList.end(); ++iter)
	{
		tagHeadTip& tip=(*(*iter));
		if(tip.scrPos.z<0.0f || tip.scrPos.z>0.5f)			//视锥剪裁
			continue;
		POINT pt = pCamera->WorldToScreen(tip.worldPos);

		tagPoint ptSize;
		m_pRender->GetTextSize(tip.strNameTip.c_str(), NULL, ptSize);

		//名字位置修正
		if( IS_PLAYER(tip.roleID) || IS_CREATURE(tip.roleID) || IS_PET(tip.roleID) )
		{
			pt.y -= (LONG)(25.0f * tip.scale);
		}
        ////宠物名字修正
        //if ( IS_PET(tip.roleID) )
        //{
        //    pt.y -= (LONG)(25.0f * tip.scale);
        //}

		RECT rcName;
		rcName.left = pt.x - ptSize.x/2;
		rcName.right = pt.x + ptSize.x/2;
		rcName.top = pt.y - ptSize.y;
		rcName.bottom = pt.y;
		tip.rectName = rcName;
		
		//added by hqzhang
		//帮派位置再修正
		if( IS_PLAYER(tip.roleID) && tip.strGuildTip != _T("") )
		{
			pt.y -= (LONG)(18.0f * tip.scale);
		}

		m_pRender->GetTextSize(tip.strGuildTip.c_str(), NULL, ptSize);

		RECT rcGuild;
		rcGuild.left = pt.x - ptSize.x/2;
		rcGuild.right = pt.x + ptSize.x/2;
		rcGuild.top = pt.y - ptSize.y;
		rcGuild.bottom = pt.y;
		//end added by hqzhang

		//added by hyu
		//称号位置再修正
		if( ( IS_PLAYER(tip.roleID) || IS_CREATURE(tip.roleID) ) && !tip.strTitleTip.empty() )
		{
			pt.y -= (LONG)(18.0f * tip.scale);
		}

		m_pRender->GetTextSize(tip.strTitleTip.c_str(), NULL, ptSize);

		RECT rcTitle;
		rcTitle.left = pt.x - ptSize.x/2;
		rcTitle.right = pt.x + ptSize.x/2;
		rcTitle.top = pt.y - ptSize.y;
		rcTitle.bottom = pt.y;
		//end added by hyu

        //added by hyu
        //夫妻称谓位置再修正
        if( IS_PLAYER(tip.roleID) && !tip.strSpouse.empty() )
        {
            pt.y -= (LONG)(18.0f * tip.scale);
        }

        m_pRender->GetTextSize(tip.strSpouse.c_str(), NULL, ptSize);

        RECT rcSpouse;
        rcSpouse.left = pt.x - ptSize.x/2;
        rcSpouse.right = pt.x + ptSize.x/2;
        rcSpouse.top = pt.y - ptSize.y;
        rcSpouse.bottom = pt.y;
        //end added by hyu

		//金牌网吧位置再修正
		if( IS_PLAYER(tip.roleID) && !tip.strVipNetBarTip.empty() )
		{
			pt.y -= (LONG)(18.0f * tip.scale);
		}

		m_pRender->GetTextSize(tip.strVipNetBarTip.c_str(), NULL, ptSize);

		RECT rcVipNetBar;
		rcVipNetBar.left = pt.x - ptSize.x/2;
		rcVipNetBar.right = pt.x + ptSize.x/2;
		rcVipNetBar.top = pt.y - ptSize.y;
		rcVipNetBar.bottom = pt.y;


		//是否在屏幕范围内
		if(rcName.left >=0 && rcName.top >= 0
			&& rcName.right < clientW && rcName.bottom < clientH
			&& tip.strNameTip!=_T(""))
		{
			RECT rect1;
			rect1.top = rcName.top-1;
			rect1.bottom = rcName.bottom-1;
			rect1.left = rcName.left;
			rect1.right = rcName.right;
			m_pFont->DrawText(tip.strNameTip.c_str(), &rect1, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, tip.scrPos.z, tip.scale);

			rect1.top = rcName.top+1;
			rect1.bottom = rcName.bottom+1;
			rect1.left = rcName.left;
			rect1.right = rcName.right;
			m_pFont->DrawText(tip.strNameTip.c_str(), &rect1, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, tip.scrPos.z, tip.scale);

			rect1.top = rcName.top;
			rect1.bottom = rcName.bottom;
			rect1.left = rcName.left-1;
			rect1.right = rcName.right-1;
			m_pFont->DrawText(tip.strNameTip.c_str(), &rect1, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, tip.scrPos.z, tip.scale);

			rect1.top = rcName.top;
			rect1.bottom = rcName.bottom;
			rect1.left = rcName.left+1;
			rect1.right = rcName.right+1;
			m_pFont->DrawText(tip.strNameTip.c_str(), &rect1, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, tip.scrPos.z, tip.scale);

			//绘制姓名
			m_pFont->DrawText(tip.strNameTip.c_str(), &rcName, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, tip.txtNameColor, tip.scrPos.z, tip.scale);

		}
		//added by hyu
		if (rcTitle.left >=0 && rcTitle.top >= 0
			&& rcTitle.right < clientW && rcTitle.bottom < clientH
			&& tip.strTitleTip!=_T("") )
		{
			RECT rect1;
			rect1.top = rcTitle.top-1;
			rect1.bottom = rcTitle.bottom-1;
			rect1.left = rcTitle.left;
			rect1.right = rcTitle.right;
			if( IS_PLAYER(tip.roleID) )
				m_pFont->DrawText(tip.strTitleTip.c_str(), &rect1, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, tip.scrPos.z, tip.scale);
			else
				m_pFontTitle->DrawText(tip.strTitleTip.c_str(), &rect1, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, tip.scrPos.z, tip.scale);

			rect1.top = rcTitle.top+1;
			rect1.bottom = rcTitle.bottom+1;
			rect1.left = rcTitle.left;
			rect1.right = rcTitle.right;
			if( IS_PLAYER(tip.roleID) )
				m_pFont->DrawText(tip.strTitleTip.c_str(), &rect1, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, tip.scrPos.z, tip.scale);
			else 
				m_pFontTitle->DrawText(tip.strTitleTip.c_str(), &rect1, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, tip.scrPos.z, tip.scale);

			rect1.top = rcTitle.top;
			rect1.bottom = rcTitle.bottom;
			rect1.left = rcTitle.left-1;
			rect1.right = rcTitle.right-1;
			if( IS_PLAYER(tip.roleID) )
				m_pFont->DrawText(tip.strTitleTip.c_str(), &rect1, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, tip.scrPos.z, tip.scale);
			else
				m_pFontTitle->DrawText(tip.strTitleTip.c_str(), &rect1, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, tip.scrPos.z, tip.scale);

			rect1.top = rcTitle.top;
			rect1.bottom = rcTitle.bottom;
			rect1.left = rcTitle.left+1;
			rect1.right = rcTitle.right+1;
			if( IS_PLAYER(tip.roleID) )
				m_pFont->DrawText(tip.strTitleTip.c_str(), &rect1, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, tip.scrPos.z, tip.scale);
			else
				m_pFontTitle->DrawText(tip.strTitleTip.c_str(), &rect1, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, tip.scrPos.z, tip.scale);

			//绘制称号
			if( IS_PLAYER(tip.roleID) )
				m_pFont->DrawText(tip.strTitleTip.c_str(), &rcTitle, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, tip.txtTitleColor, tip.scrPos.z, tip.scale);
			else
				m_pFontTitle->DrawText(tip.strTitleTip.c_str(), &rcTitle, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, tip.txtTitleColor, tip.scrPos.z, tip.scale);
		}
		//end added by hyu
        if (rcSpouse.left >=0 && rcSpouse.top >= 0
            && rcSpouse.right < clientW && rcSpouse.bottom < clientH
            && tip.strSpouse!=_T("") )
        {
            RECT rect1;
            rect1.top = rcSpouse.top-1;
            rect1.bottom = rcSpouse.bottom-1;
            rect1.left = rcSpouse.left;
            rect1.right = rcSpouse.right;
            m_pFont->DrawText(tip.strSpouse.c_str(), &rect1, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, tip.scrPos.z, tip.scale);

            rect1.top = rcSpouse.top+1;
            rect1.bottom = rcSpouse.bottom+1;
            rect1.left = rcSpouse.left;
            rect1.right = rcSpouse.right;
            m_pFont->DrawText(tip.strSpouse.c_str(), &rect1, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, tip.scrPos.z, tip.scale);

            rect1.top = rcSpouse.top;
            rect1.bottom = rcSpouse.bottom;
            rect1.left = rcSpouse.left-1;
            rect1.right = rcSpouse.right-1;
            m_pFont->DrawText(tip.strSpouse.c_str(), &rect1, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, tip.scrPos.z, tip.scale);

            rect1.top = rcSpouse.top;
            rect1.bottom = rcSpouse.bottom;
            rect1.left = rcSpouse.left+1;
            rect1.right = rcSpouse.right+1;
            m_pFont->DrawText(tip.strSpouse.c_str(), &rect1, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, tip.scrPos.z, tip.scale);

            //绘制夫妻称谓
            m_pFont->DrawText(tip.strSpouse.c_str(), &rcSpouse, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, tip.spouseColor, tip.scrPos.z, tip.scale);
        }
		//added by hqzhang
		if (rcGuild.left >=0 && rcGuild.top >= 0
			&& rcGuild.right < clientW && rcGuild.bottom < clientH
			&& tip.strGuildTip!=_T("") )
		{
			RECT rect1;
			rect1.top = rcGuild.top-1;
			rect1.bottom = rcGuild.bottom-1;
			rect1.left = rcGuild.left;
			rect1.right = rcGuild.right;
			m_pFont->DrawText(tip.strGuildTip.c_str(), &rect1, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, tip.scrPos.z, tip.scale);

			rect1.top = rcGuild.top+1;
			rect1.bottom = rcGuild.bottom+1;
			rect1.left = rcGuild.left;
			rect1.right = rcGuild.right;
			m_pFont->DrawText(tip.strGuildTip.c_str(), &rect1, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, tip.scrPos.z, tip.scale);

			rect1.top = rcGuild.top;
			rect1.bottom = rcGuild.bottom;
			rect1.left = rcGuild.left-1;
			rect1.right = rcGuild.right-1;
			m_pFont->DrawText(tip.strGuildTip.c_str(), &rect1, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, tip.scrPos.z, tip.scale);

			rect1.top = rcGuild.top;
			rect1.bottom = rcGuild.bottom;
			rect1.left = rcGuild.left+1;
			rect1.right = rcGuild.right+1;
			m_pFont->DrawText(tip.strGuildTip.c_str(), &rect1, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, tip.scrPos.z, tip.scale);

			//绘制帮派
			m_pFont->DrawText(tip.strGuildTip.c_str(), &rcGuild, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, tip.txtNameColor, tip.scrPos.z, tip.scale);
		}
		//end added by hqzhang

		if(rcVipNetBar.left >=0 && rcVipNetBar.top >= 0
			&& rcVipNetBar.right < clientW && rcVipNetBar.bottom < clientH
			&& tip.strVipNetBarTip!=_T(""))
		{
			RECT rect1;
			rect1.top = rcVipNetBar.top-1;
			rect1.bottom = rcVipNetBar.bottom-1;
			rect1.left = rcVipNetBar.left;
			rect1.right = rcVipNetBar.right;
			m_pFont->DrawText(tip.strVipNetBarTip.c_str(), &rect1, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, tip.scrPos.z, tip.scale);

			rect1.top = rcVipNetBar.top+1;
			rect1.bottom = rcVipNetBar.bottom+1;
			rect1.left = rcVipNetBar.left;
			rect1.right = rcVipNetBar.right;
			m_pFont->DrawText(tip.strVipNetBarTip.c_str(), &rect1, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, tip.scrPos.z, tip.scale);

			rect1.top = rcVipNetBar.top;
			rect1.bottom = rcVipNetBar.bottom;
			rect1.left = rcVipNetBar.left-1;
			rect1.right = rcVipNetBar.right-1;
			m_pFont->DrawText(tip.strVipNetBarTip.c_str(), &rect1, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, tip.scrPos.z, tip.scale);

			rect1.top = rcVipNetBar.top;
			rect1.bottom = rcVipNetBar.bottom;
			rect1.left = rcVipNetBar.left+1;
			rect1.right = rcVipNetBar.right+1;
			m_pFont->DrawText(tip.strVipNetBarTip.c_str(), &rect1, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, tip.scrPos.z, tip.scale);

			//绘制金牌网吧名称
			m_pFont->DrawText(tip.strVipNetBarTip.c_str(), &rcVipNetBar, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, tip.txtNameColor, tip.scrPos.z, tip.scale);
		}
	}
	m_pFontTitle->End();
	m_pFont->End();

	IDraw2D::Inst()->BeginDraw(IDraw2D::EBlend_Alpha, true);
	for(iter=m_vecTipsList.begin(); iter!=m_vecTipsList.end(); iter++)
	{
		tagHeadTip& tip=(*(*iter));

		if(tip.scrPos.z<0.0f || tip.scrPos.z>0.5f)			//视锥剪裁
			continue;
		POINT pt = pCamera->WorldToScreen(tip.worldPos);

		RECT rc;
		rc.left = pt.x - 35;
		rc.right = pt.x + 35;
		rc.top = pt.y - 35;
		rc.bottom = pt.y + 35;

		if(rc.left>=0 && rc.top>=0
			&& rc.right<clientW && rc.bottom<clientH)
		{
			//绘制任务提示
			m_questFlag.Draw(pt, tip.scrPos.z, tip.scale, tip.qeustFlag);
		}

		if( !tip.strNameTip.empty() )
		{
			// 戾气图标
			TCHAR* szHostility = NULL;
			DWORD dwHostility = tip.dwHostility;
			if( dwHostility > EHostility_Zero && dwHostility < EHostility_End )
			{
				switch( dwHostility )
				{
				case EHostility_One:
				case EHostility_Two:
				case EHostility_Three:
					szHostility = _T("data\\ui\\pk\\l_pk_yi.dds");
					break;
				case EHostility_Four:
				case EHostility_Five:
				case EHostility_Six:
					szHostility = _T("data\\ui\\pk\\l_pk_er.dds");
					break;
				case EHostility_Seven:
				case EHostility_Eight:
				case EHostility_Nine:
					szHostility = _T("data\\ui\\pk\\l_pk_san.dds");
					break;
				}
			}

			//组队提示
			if( (tip.roleID == RoleMgr::Inst()->GetLocalPlayerID() && TeamSys::Inst()->IsInTeam() )
				|| TeamSys::Inst()->IsTeammate(tip.roleID) )
			{
				tagPoint ptSize;
				m_pRender->GetTextSize(tip.strNameTip.c_str(), NULL, ptSize);

				float teamSize	= 15.0f*tip.scale;
				pt.y -= (LONG)(23 * tip.scale);
				float centerY	= (float)pt.y - 7.5f;
				RECT rcTeam;
				rcTeam.left		= LONG(pt.x + ptSize.x*tip.scale/2+5);
				rcTeam.right	= LONG(rcTeam.left + teamSize);
				rcTeam.top		= (LONG)(centerY - teamSize/2.0f);
				rcTeam.bottom	= (LONG)(centerY + teamSize/2.0f);


				if(  rcTeam.left>=0 && rcTeam.top>=0
					&& rcTeam.right<clientW && rcTeam.bottom<clientH )
				{
					m_titleFlag.DrawTeamFlag(rcTeam, tip.scrPos.z, szTeam);
				}

				if( dwHostility > 0 )
				{
					RECT rcHostility;
					rcHostility.left	= LONG(rcTeam.right + 1.0f);
					rcHostility.right	= LONG(rcTeam.right + rcTeam.right - rcTeam.left + 1.0f);
					rcHostility.top		= rcTeam.top;
					rcHostility.bottom	= rcTeam.bottom;

					if(  rcHostility.left>=0 && rcHostility.top>=0
						&& rcHostility.right<clientW && rcHostility.bottom<clientH )
					{
						m_titleFlag.DrawTeamFlag(rcHostility, tip.scrPos.z, szHostility);
					}
				}
			}
			else if( dwHostility > 0 )
			{
				tagPoint ptSize;
				m_pRender->GetTextSize(tip.strNameTip.c_str(), NULL, ptSize);

				float teamSize	= 15.0f*tip.scale;
				pt.y -= (LONG)(23 * tip.scale);
				float centerY	= (float)pt.y - 7.5f;
				RECT rcTeam;
				rcTeam.left		= LONG(pt.x + ptSize.x*tip.scale/2+5);
				rcTeam.right	= LONG(rcTeam.left + teamSize);
				rcTeam.top		= (LONG)(centerY - teamSize/2.0f);
				rcTeam.bottom	= (LONG)(centerY + teamSize/2.0f);

				if(  rcTeam.left>=0 && rcTeam.top>=0
					&& rcTeam.right<clientW && rcTeam.bottom<clientH )
				{
					m_titleFlag.DrawTeamFlag(rcTeam, tip.scrPos.z, szHostility);
				}
			}
		}

		if(tip.strTitleTip == _T(""))
			continue;
		tagPoint ptSize;
		m_pRender->GetTextSize(tip.strTitleTip.c_str(), NULL, ptSize);
		if( IS_CREATURE(tip.roleID) )
		{
			pt.y -= (LONG)(43 * tip.scale);
		}
		else
		{
			if( tip.strGuildTip==_T("") )
			{
				pt.y -= (LONG)(43 * tip.scale);
			}
			else
			{
				pt.y -= (LONG)(61 * tip.scale);
			}
		}
		float titleSize = 15.0f*tip.scale;
		RECT rcTitleIcon;
		rcTitleIcon.right = pt.x - (LONG)(ptSize.x*tip.scale)/2;
		rcTitleIcon.left = rcTitleIcon.right - (LONG)titleSize;
		float centerY = (float)pt.y - 7.5f;
		rcTitleIcon.bottom = (LONG)(centerY + titleSize/2.0f);
		rcTitleIcon.top = (LONG)(centerY - titleSize/2.0f);
		if( rcTitleIcon.left>=0 && rcTitleIcon.top>=0
			&& rcTitleIcon.right<clientW && rcTitleIcon.bottom<clientH
			&& tip.strResTex!=_T("") )
		{
			//称号贴图
			m_titleFlag.DrawRoleTitleFlag(rcTitleIcon, tip.scrPos.z, tip.strResTex.c_str());
		}
	}
	if( m_bShowHPorMP )
	{
		UpdateLocalHPorMPProgress();
		UpdateOtherHPorMPProgress();
	}
	IDraw2D::Inst()->EndDraw();
}

void RoleHeadTipsFrame::ClearAllTips()
{
	vector<tagHeadTip*>::iterator iter;

	for(iter=m_vecTipsList.begin(); iter!=m_vecTipsList.end(); ++iter)
	{
		tagHeadTip* pTip = (*iter);
		delete pTip;
	}

	m_vecTipsList.clear();
}

Cool3D::Color4ub RoleHeadTipsFrame::GetRoleTipsColor( Role* pRole )
{
	Color4ub color = Color4ub(255,255,255,255);		//默认白色

	if( IS_PLAYER(pRole->GetID()) )
	{
		Player* pPlayer = (Player*)pRole;
		if( ((ERoleState)pPlayer->GetRoleState()&ERS_Safeguard) != 0x0 )
		{
			color = PlayerSafeguardColor;
		}
		else if( ((ERoleState)pPlayer->GetRoleState()&ERS_PK) != 0x0 || ((ERoleState)pPlayer->GetRoleState()&ERS_PKEX) != 0x0 )
		{
			color = PlayerPKColor;
		}
		else 
		{
			switch(pPlayer->GetPKState())
			{
			case ERPKS_Peace:
				color = PlayerPeaceColor;
				break;
			case ERPKS_Warnning:
				color = PlayerWarnningColor;
				break;
			case ERPKS_Wicked:
				color = PlayerWickedColor;
				break;
			case ERPKS_Wanted:
				color = PlayerWantedColor;
				break;
			}
		}
	}
	else if (IS_CREATURE(pRole->GetID()))
	{
		NPC* pNpc = (NPC*)pRole;
		if(pNpc->IsNPC())
			color = NPCNameColor;
		else if(pNpc->IsMonster())
			color = g_npcColorTab.GetColor( pNpc->GetRoleLevel() );
	}
    else if (IS_PET(pRole->GetID()))
    {

    }
    else
    {

    }

	return color;
}

const tstring RoleHeadTipsFrame::GetRoleTitleTips( Role* pRole )
{
    if( IS_PLAYER(pRole->GetID()) )
    {
        Player * pPlayer = static_cast<Player*>(pRole);
		DWORD dwTitleID = pPlayer->GetTitleID();

		if( pPlayer->GetID()==RoleMgr::Inst()->GetLocalPlayerID() 
			&&  GameSet::Inst()->Get(EGS_ShowSelfName) )
		{
			return pPlayer->GetTitleName();
		}
		else if( GameSet::Inst()->Get(EGS_ShowOtherPalyerTitle) )
		{
			return pPlayer->GetTitleName();
		}
		else
		{
			CombatSys* pCombatSys = (CombatSys*)m_pMgr->GetFrame(_T("CombatSys"));
			if( !P_VALID( pCombatSys ) )
				return _T("");
			if( pCombatSys->GetCurTargetID() == pPlayer->GetID() )
			{	
				return  pPlayer->GetTitleName();
			}
		}
    }
    else if( IS_CREATURE( pRole->GetID() ) )
    {
		NPC* pNpc = (NPC*)pRole;
		const tagCreatureProto* pProto = CreatureData::Inst()->FindNpcAtt( pNpc->GetTypeID() );
		if( !P_VALID(pProto) )
			return _T("");

		if( pProto->eType==ECT_NPC )
		{
			return pProto->szTitle;
		}
		else if( pProto->eType==ECT_Monster )
		{	
			//不可见的巢穴不显示名字
			if( pProto->nType2==EMTT_Nest && !pProto->bVisble)
				return _T("");
			else if(GameSet::Inst()->Get(EGS_ShowMonsterName))
				return pProto->szTitle;
			else if( GetWorldPickerFrm()->GetCurMousePointRoleID()==pRole->GetID() )
				return pProto->szTitle;
			else 
			{
				CombatSys* pCombatSys = (CombatSys*)m_pMgr->GetFrame(_T("CombatSys"));
				if( !P_VALID( pCombatSys ) )
					return _T("");
				if( pCombatSys->GetCurTargetID() == pNpc->GetID() )
				{	
					return pProto->szTitle;
				}
			}
		}
    }
    
	return _T("");
}

const tstring RoleHeadTipsFrame::GetRoleTitleIcon(Role* pRole)
{
	if( IS_CREATURE( pRole->GetID() ) )
	{
		NPC* pNpc = (NPC*)pRole;
		const tagCreatureProto* pProto = CreatureData::Inst()->FindNpcAtt( pNpc->GetTypeID() );
		if( !P_VALID(pProto) )
			return _T("");
		if( pProto->eType==ECT_NPC )
		{
			return pProto->szTitleIcon;
		}
		else if( pProto->eType==ECT_Monster )
		{
			if( GameSet::Inst()->Get(EGS_ShowMonsterName) )
				return pProto->szTitleIcon;
			else if( GetWorldPickerFrm()->GetCurMousePointRoleID()==pRole->GetID() )
				return pProto->szTitleIcon;
		}
	}
	return _T("");
}

Cool3D::Color4ub RoleHeadTipsFrame::GetRoleTitleColor( Role* pRole )
{
	Color4ub color = Color4ub(255,255,255,255);		//默认白色

    if( IS_PLAYER(pRole->GetID()) )
    {
        Player* pPlayer = static_cast<Player*>(pRole);

        DWORD dwTitleID = pPlayer->GetTitleID();
        DWORD dwTitleColor = RoleTitleProtoData::Inst()->GetTitleColorbyID(dwTitleID);

        return Color4ub(dwTitleColor);
    }
    else if (IS_CREATURE(pRole->GetID()))
    {
		NPC* pNpc = (NPC*)pRole;
		if(pNpc->IsNPC())
			color = NPCNameColor;
		else if(pNpc->IsMonster())
			color = g_npcColorTab.GetColor( pNpc->GetRoleLevel() );
    }
    else if (IS_PET(pRole->GetID()))
    {

    }
    else
    {

    }

	return color;
}

const tstring RoleHeadTipsFrame::GetRoleGuildTips( Role* pRole )
{
	if( IS_PLAYER(pRole->GetID()) )
	{
		Player * pPlayer = static_cast<Player*>(pRole);
		DWORD dwGuildID = pPlayer->GetRoleGuildID();
		// 如果ID无效则直接返回
		if(!GT_VALID(dwGuildID))
			return _T("");

		tstring strName = GuildMgr::Inst()->GetGuildName(dwGuildID);
		if(strName.size() > 0)
		{
			TCHAR szTmp[X_SHORT_NAME];
			_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR),
				g_StrTable[_T("GuildHead_NameNPos")], strName.c_str(),
				GuildMgr::Inst()->GetMemberPos(pPlayer->GetRoleGuildPos()).c_str());
			return szTmp;
		}
	}
	return _T("");
}

const tstring RoleHeadTipsFrame::GetRoleVipNetBarTips( Role* pRole )
{
	if( IS_PLAYER(pRole->GetID()) )
	{
		if( pRole->GetID()==RoleMgr::Inst()->GetLocalPlayerID() )
		{
			if( GameSet::Inst()->Get(EGS_ShowSelfName) )
				return VipNetBarMgr::Inst()->GetVipNetBarName(pRole->GetID());
		}
		else if( GameSet::Inst()->Get(EGS_ShowOtherPalyerTitle) )
		{
			return VipNetBarMgr::Inst()->GetVipNetBarName(pRole->GetID());
		}
		else
		{
			CombatSys* pCombatSys = (CombatSys*)m_pMgr->GetFrame(_T("CombatSys"));
			if( !P_VALID( pCombatSys ) )
				return _T("");
			if( pCombatSys->GetCurTargetID() == pRole->GetID() )
			{	
				return  VipNetBarMgr::Inst()->GetVipNetBarName(pRole->GetID());
			}
		}
	}
	return _T("");
}

void RoleHeadTipsFrame::UpdateLocalHPorMPProgress()
{
	static int nWidth = 75;
	static int nHeight = 8;

	LocalPlayer* pLP = RoleMgr::Inst()->GetLocalPlayer();
	if( !P_VALID(pLP) )
		return;

	ClientCamera* pCamera = GetWorldFrm()->GetCamera();
	if(!P_VALID(pCamera))
		return;
	const Vector3& lookFrom = pCamera->GetPos();

	Vector3 worldPos,srcPos;
	pLP->GetHeadPos(worldPos);

	srcPos = pCamera->WorldToScreenF(worldPos);
	srcPos.z *= 0.5f;

	if(srcPos.z<=0 || srcPos.z>=0.5f)
		return;

	POINT pt = pCamera->WorldToScreen(worldPos);
	pt.y -= 15;

	//血
	const int nMaxHp	= pLP->GetAttribute( ERA_MaxHP );
	const int nHp		= pLP->GetAttribute( ERA_HP );
	const int nPerHp	= (int)(((float)nHp)/((float)nMaxHp)*((float)nWidth));

	RECT destRC,srcRC;
	
	srcRC.left = srcRC.top = 0;
	srcRC.right = srcRC.left+nWidth;
	srcRC.bottom = nHeight;

	destRC.left = pt.x - nWidth/2;
	destRC.right = destRC.left + nPerHp;
	destRC.top = pt.y-nHeight/2;
	destRC.bottom = destRC.top+nHeight/2;

	IDraw2D::Inst()->Draw(&destRC, &srcRC, m_pResHP, 0xFFFFFFFF, ETFilter_POINT, 0, IDraw2D::EAWT_Write, srcPos.z);

	//真气
	pt.y += 8;
	const int nMaxMp	= pLP->GetAttribute( ERA_MaxMP );
	const int nMp		= pLP->GetAttribute( ERA_MP );
	const int nPerMp	= (int)(((float)nMp)/((float)nMaxMp)*((float)nWidth));

	destRC.left = pt.x - nWidth/2;
	destRC.right = destRC.left + nPerMp;
	destRC.top = pt.y-nHeight/2;
	destRC.bottom = destRC.top+nHeight/2;

	IDraw2D::Inst()->Draw(&destRC, &srcRC, m_pResMP, 0xFFFFFFFF, ETFilter_POINT, 0, IDraw2D::EAWT_Write, srcPos.z);

}

void RoleHeadTipsFrame::UpdateOtherHPorMPProgress()
{
	static float nWidth = 75.0f;
	static float nHeight = 8.0f;

	static INT nClientWidth = Device()->GetClientWidth();
	static INT nClientHeight = Device()->GetClientHeight();


	DWORD dwMouseIntoID = GT_INVALID;
	CombatSys* pCombatSys = (CombatSys*)m_pMgr->GetFrame(_T("CombatSys"));
	if( P_VALID( pCombatSys ) )
		dwMouseIntoID = pCombatSys->GetCurTargetID();

	ClientCamera* pCamera = GetWorldFrm()->GetCamera();
	if(!P_VALID(pCamera))
		return;
	const Vector3& lookFrom = pCamera->GetPos();

	Role* pRole = RoleMgr::Inst()->FindRole(dwMouseIntoID);
	if( !P_VALID(pRole) )
		return;

	if( IS_PLAYER(dwMouseIntoID)  )
	{
		Vector3 worldPos,srcPos;
		pRole->GetHeadPos(worldPos);

		srcPos = pCamera->WorldToScreenF(worldPos);
		srcPos.z *= 0.5f;

		if(srcPos.z<=0 || srcPos.z>=0.5f)
			return;

		float dist = D3DXVec3Length(&(lookFrom-worldPos));
		float scale = 1.0f;
		if( dist > 2000.0f )
		{
			scale = 1.0f - (dist-2000.0f)*scalePerUnit;
			if( scale < 0.1f )
				scale = 0.1f;
		}

		POINT pt = pCamera->WorldToScreen(worldPos);
		pt.y = pt.y-(LONG)(15.0f*scale);

		//血
		const int nMaxHp	= pRole->GetAttribute( ERA_MaxHP );
		const int nHp		= pRole->GetAttribute( ERA_HP );
		const float nPerHp	= (((float)nHp)/((float)nMaxHp)*(nWidth));

		RECT destRC,srcRC;

		srcRC.left = srcRC.top = 0;
		srcRC.right = srcRC.left+(LONG)nWidth;
		srcRC.bottom = (LONG)nHeight;

		destRC.left = pt.x - (LONG)(nWidth*scale/2);
		destRC.right = destRC.left + (LONG)(nPerHp*scale);
		destRC.top = pt.y-(LONG)(nHeight*scale/2);
		destRC.bottom = destRC.top+(LONG)(nHeight*scale/2);

		if( destRC.left>=0 && destRC.top>=0 && destRC.right<=nClientWidth && destRC.bottom<=nClientHeight )
			IDraw2D::Inst()->Draw(&destRC, &srcRC, m_pResHP, 0xFFFFFFFF, ETFilter_POINT, 0, IDraw2D::EAWT_Write, srcPos.z);

		//真气
		pt.y = pt.y + (LONG)(8.0f*scale);
		const int nMaxMp	= pRole->GetAttribute( ERA_MaxMP );
		const int nMp		= pRole->GetAttribute( ERA_MP );
		const float nPerMp	= (((float)nMp)/((float)nMaxMp)*(nWidth));

		destRC.left = pt.x - (LONG)(nWidth*scale/2);
		destRC.right = destRC.left + (LONG)(nPerMp*scale);
		destRC.top = pt.y-(LONG)(nHeight*scale/2);
		destRC.bottom = destRC.top+(LONG)(nHeight*scale/2);

		if( destRC.left>=0 && destRC.top>=0 && destRC.right<=nClientWidth && destRC.bottom<=nClientHeight )
			IDraw2D::Inst()->Draw(&destRC, &srcRC, m_pResMP, 0xFFFFFFFF, ETFilter_POINT, 0, IDraw2D::EAWT_Write, srcPos.z);
	}
	else if( IS_CREATURE(dwMouseIntoID) )
	{
		NPC* pNpc = (NPC*)pRole;
		const tagCreatureProto* pProto = CreatureData::Inst()->FindNpcAtt( pNpc->GetTypeID() );
		if( !P_VALID(pProto) )
			return;

		 if(pProto->eType==ECT_NPC || pProto->eType==ECT_Monster)
		 {
			 Vector3 worldPos,srcPos;
			 pRole->GetHeadPos(worldPos);

			 srcPos = pCamera->WorldToScreenF(worldPos);
			 srcPos.z *= 0.5f;

			 if(srcPos.z<=0 || srcPos.z>=0.5f)
				 return;

			 float dist = D3DXVec3Length(&(lookFrom-worldPos));
			 float scale = 1.0f;
			 if( dist > 2000.0f )
			 {
				 scale = 1.0f - (dist-2000.0f)*scalePerUnit;
				 if( scale < 0.1f )
					 scale = 0.1f;
			 }

			 POINT pt = pCamera->WorldToScreen(worldPos);
			 pt.y = pt.y-(LONG)(15.0f*scale);

			 //血
			 const int nMaxHp	= pRole->GetAttribute( ERA_MaxHP );
			 const int nHp		= pRole->GetAttribute( ERA_HP );
			 const float nPerHp	= (((float)nHp)/((float)nMaxHp)*(nWidth));

			 RECT destRC,srcRC;

			 srcRC.left = srcRC.top = 0;
			 srcRC.right = srcRC.left+(LONG)nWidth;
			 srcRC.bottom = (LONG)nHeight;

			 destRC.left = pt.x - (LONG)(nWidth*scale/2);
			 destRC.right = destRC.left + (LONG)(nPerHp*scale);
			 destRC.top = pt.y-(LONG)(nHeight*scale/2);
			 destRC.bottom = destRC.top+(LONG)(nHeight*scale/2);

			 if( destRC.left>=0 && destRC.top>=0 && destRC.right<=nClientWidth && destRC.bottom<=nClientHeight )
				 IDraw2D::Inst()->Draw(&destRC, &srcRC, m_pResHP, 0xFFFFFFFF, ETFilter_POINT, 0, IDraw2D::EAWT_Write, srcPos.z);

			 //真气
			 pt.y = pt.y + (LONG)(8.0f*scale);
			 const int nMaxMp	= pRole->GetAttribute( ERA_MaxMP );
			 const int nMp		= pRole->GetAttribute( ERA_MP );
			 const float nPerMp	= (((float)nMp)/((float)nMaxMp)*(nWidth));

			 destRC.left = pt.x - (LONG)(nWidth*scale/2);
			 destRC.right = destRC.left + (LONG)(nPerMp*scale);
			 destRC.top = pt.y-(LONG)(nHeight*scale/2);
			 destRC.bottom = destRC.top+(LONG)(nHeight*scale/2);

			 if( destRC.left>=0 && destRC.top>=0 && destRC.right<=nClientWidth && destRC.bottom<=nClientHeight )
				 IDraw2D::Inst()->Draw(&destRC, &srcRC, m_pResMP, 0xFFFFFFFF, ETFilter_POINT, 0, IDraw2D::EAWT_Write, srcPos.z);
		 }
	}
}

DWORD RoleHeadTipsFrame::OnKey( DWORD dwID, BOOL bDown )
{
	if( bDown )
	{
		m_bShowHPorMP = !m_bShowHPorMP;
	}

	return 0;
}

const DWORD RoleHeadTipsFrame::GetRoleHostilityTips( Role* pRole )
{
	if( IS_PLAYER(pRole->GetID()) )
	{
		Player * pPlayer = static_cast<Player*>(pRole);
		if( P_VALID(pPlayer) )
			return pPlayer->GetAttribute( ERA_Hostility );
	}
	return 0;
}

const tstring RoleHeadTipsFrame::GetRoleSpouse( Role* pRole )
{
    DWORD spouseId = GT_INVALID;

    if (IS_PLAYER(pRole->GetID()))
    {
        Player* pPlayer = static_cast<Player*>(pRole);
        if (P_VALID(pPlayer) && pPlayer->IsAttributeInited())
        {
            spouseId = pPlayer->GetPlayerLoverID();
            if (GT_INVALID == spouseId)
                return _T("");

            
            tstring spouseName = PlayerNameTab::Inst()->FindNameByID(spouseId);
            if (spouseName.empty())
                return _T("");

            tstringstream tss;
            tss << _T("<") << spouseName << _T(">") << (0 == pPlayer->GetAvatarAtt().bySex ? g_StrTable[_T("MarriageTitleFemale")] : g_StrTable[_T("MarriageTitleMale")]);

            return tss.str();
            
        }
    }

    return _T("");
}

Cool3D::Color4ub RoleHeadTipsFrame::GetRoleSpouseColor( Role* pRole )
{
    Color4ub color = Color4ub(255,255,255,255);		//默认白色

    if( IS_PLAYER(pRole->GetID()) )
    {
    }

    return color;
}