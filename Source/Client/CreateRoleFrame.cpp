#include "StdAfx.h"
#include "CreateRoleFrame.h"
#include ".\Player.h"
#include ".\AvatarCfg.h"
#include "..\WorldDefine\RoleDefine.h"
#include ".\SelectRoleFrame.h"
#include ".\SeleRoleMgr.h"
#include "FilterData.h"
#include "GameSet.h"
#include "AudioSys.h"
#include "CursorMgr.h"
#include "LoadingFrame.h"

#define ROTATEBTN_YOFFSET 530
#define LROTATEBTN_XOFFSET 850
#define RROTATEBTN_XOFFSET 500
const float ROTATE_SPEED = 60;		//人物旋转的速度


CreateRoleFrame::CreateRoleFrame(void):m_Trunk(this),
m_fRoleYaw(2.14f),
m_eStatus(EPCS_OutKungSword),
m_bRotate(false),
m_bLRotate(false),
m_bRRotate(false),
m_bSentCreate(false)
{
	Clear();
	m_pPlayer	= NULL;
	ZeroMemory(&m_avatarEquip,sizeof(m_avatarEquip));
}

CreateRoleFrame::~CreateRoleFrame(void)
{
	
}

void CreateRoleFrame::Clear()
{
	m_pWnd				= NULL;
	m_pWndCaption		= NULL;
	m_pLBtnRotate		= NULL;
	m_pRBtnRotate		= NULL;
	m_pStcErrHint		= NULL;
	m_pStcSex			= NULL;
	m_pName				= NULL;
	m_pSexMutex			= NULL;
	//m_pStatusMutex		= NULL;
	m_pMalePBtn			= NULL;
	m_pFmalePBtn		= NULL;
	m_pLBtnHType		= NULL;
	m_pRBtnHType		= NULL;
	m_pStcHType			= NULL;
	m_pLBtnHColor		= NULL;
	m_pRBtnHColor		= NULL;
	m_pStcHColor		= NULL;
	m_pLBtnFace			= NULL;
	m_pRBtnFace			= NULL;
	m_pStcFace			= NULL;
	m_pLBtnFDetail		= NULL;
	m_pRBtnFDetail		= NULL;
	m_pStcFDetail		= NULL;
	m_pLBtnDress		= NULL;
	m_pRBtnDress		= NULL;
	m_pStcFDress		= NULL;
	memset( m_pStatusPBtn, 0x0, sizeof( m_pStatusPBtn ) );
	m_pBtnOK			= NULL;
	m_pBtnRandom		= NULL;
	m_pBtnCancel		= NULL;
	m_bPickRole			= false;
	m_pWnd_Control		= NULL;
	m_pLBtnRotate		= NULL;
	m_pRBtnRotate		= NULL;
	m_pPBtn_Push		= NULL;
	m_pPBtn_Pull		= NULL;
}

BOOL CreateRoleFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	BeginCreateRole();

	m_pCmdMgr->Register("NS_CreateRole", (NETMSGPROC)m_Trunk.sfp2(&CreateRoleFrame::NetRecvCreateRole), _T("NS_CreateRole"));

	m_pVarContainer = CreateObj("CreateRole", "VarContainer");
	m_pVarContainer->Load(NULL, _T("config\\CreateRole.xml"));
	m_nRoleNameLength = m_pVarContainer->GetDword(_T("Role_Name_Length"));

	TObjRef<AudioSys>()->StopMusic();
	// 播放选人音乐
	TObjRef<AudioSys>()->PlayMusic(_T("music_create_role"), TRUE, SOUND_LOUD);

	return TRUE;
}

BOOL CreateRoleFrame::Destroy()
{
	GameFrame::Destroy();

	if( P_VALID(SeleRoleMgr::Inst()->GetHugeSG()) )
	{
		m_pPlayer->DetachSceneNodeFromSG(SeleRoleMgr::Inst()->GetHugeSG());		//从场景中删除结点
	}

	if( P_VALID(m_pPlayer) )						//释放结点
		SAFE_DELETE(m_pPlayer);

	if( P_VALID(m_pWnd) )
		m_pGUI->DestroyWnd(m_pWnd);

	if( P_VALID(m_pWnd_Control) )				
		m_pGUI->DestroyWnd(m_pWnd_Control);

	
	m_pCmdMgr->UnRegister("NS_CreateRole", (NETMSGPROC)m_Trunk.sfp2(&CreateRoleFrame::NetRecvCreateRole));

	Clear();
	KillObj("CreateRole");


	TObjRef<AudioSys>()->StopMusic();

	// 播放登陆音乐
	TObjRef<AudioSys>()->PlayMusic(_T("music_login0"), FALSE, SOUND_LOUD);

	return TRUE;
}

//-----------------------------------------------------------------------------
// !加载控件
//-----------------------------------------------------------------------------
BOOL CreateRoleFrame::ReloadUI()
{
	XmlElement element;
	tstring strPath = g_strLocalPath + _T("\\ui\\creatrole.xml");
	m_pGUI->LoadXml(&element, "VFS_System", strPath.c_str());

	// 调整位置
	GUIWnd* pDesktop = m_pGUI->GetDesktop();
	std::stringstream streamX, streamY;
	streamX << (pDesktop->GetSize().x - 425);
	streamY << (pDesktop->GetSize().y - 600);
	m_pGUI->ChangeXml(&element, "cr_win", "LogicPos_x", streamX.str().c_str());
	m_pGUI->ChangeXml(&element, "cr_win", "LogicPos_y", streamY.str().c_str());
	m_pGUI->ChangeXml(&element, "cr_win", "AlignType", "NoAlign");
	m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &element);


	m_pWndCaption	= m_pWnd->GetChild(_T("cr_pic"));
	m_pName			= (GUIEditBox*)m_pWndCaption->GetChild(_T("cr_name"));	
	m_pStcErrHint	= (GUIStatic*)m_pWndCaption->GetChild(_T("reg2"));
	m_pStcSex		= (GUIStatic*)m_pWndCaption->GetChild(_T("rolesex"));
	m_pSexMutex		= (GUIFrameMutex*)m_pWndCaption->GetChild(_T("rolesex\\choose1"));
	m_pMalePBtn		= (GUIPushButton*)m_pWndCaption->GetChild(_T("rolesex\\choose1\\mbutt"));
	m_pFmalePBtn	= (GUIPushButton*)m_pWndCaption->GetChild(_T("rolesex\\choose1\\fmbutt"));

	m_pLBtnHType	= (GUIButton*)m_pWndCaption->GetChild(_T("rh_butt1"));		//发型
	m_pRBtnHType	= (GUIButton*)m_pWndCaption->GetChild(_T("rh_butt2"));
	m_pStcHType		= (GUIStatic*)m_pWndCaption->GetChild(_T("rh_type"));

	m_pLBtnHColor	= (GUIButton*)m_pWndCaption->GetChild(_T("hc_butt1"));		//发色
	m_pRBtnHColor	= (GUIButton*)m_pWndCaption->GetChild(_T("hc_butt2"));
	m_pStcHColor	= (GUIStatic*)m_pWndCaption->GetChild(_T("hc_type"));

	m_pLBtnFace		= (GUIButton*)m_pWndCaption->GetChild(_T("rf_butt1"));		//面部
	m_pRBtnFace		= (GUIButton*)m_pWndCaption->GetChild(_T("rf_butt2"));
	m_pStcFace		= (GUIStatic*)m_pWndCaption->GetChild(_T("rf_type"));

	m_pLBtnFDetail	= (GUIButton*)m_pWndCaption->GetChild(_T("rfd_butt1"));		//面部细节
	m_pRBtnFDetail	= (GUIButton*)m_pWndCaption->GetChild(_T("rfd_butt2"));
	m_pStcFDetail	= (GUIStatic*)m_pWndCaption->GetChild(_T("rfd_type"));

	m_pLBtnDress	= (GUIButton*)m_pWndCaption->GetChild(_T("rc_butt1"));		//服式
	m_pRBtnDress	= (GUIButton*)m_pWndCaption->GetChild(_T("rc_butt2"));
	m_pStcFDress	= (GUIStatic*)m_pWndCaption->GetChild(_T("rc_type"));

	m_pBtnOK		= (GUIButton*)m_pWndCaption->GetChild(_T("surebutt"));		//确定
	m_pBtnOK->SetEnable(FALSE);
	m_pBtnRandom	= (GUIButton*)m_pWndCaption->GetChild(_T("randbutt"));		//随机
	m_pBtnCancel	= (GUIButton*)m_pWndCaption->GetChild(_T("backbutt"));		//返回


	tstring strPath1 = g_strLocalPath + _T("\\ui\\camerabutt.xml");
	m_pGUI->LoadXml(&element, "VFS_System", strPath1.c_str());
	m_pWnd_Control  = m_pGUI->CreateWnd(_T("\\desktop"), &element);
	m_pLBtnRotate = (GUIButton*)m_pWnd_Control->GetChild(_T("turnright"));
	m_pRBtnRotate = (GUIButton*)m_pWnd_Control->GetChild(_T("turnleft"));
	m_pPBtn_Push = (GUIPushButton*)m_pWnd_Control->GetChild(_T("fra\\zoomin"));
	m_pPBtn_Pull = (GUIPushButton*)m_pWnd_Control->GetChild(_T("fra\\zoomout"));
	m_pPBtn_Pull->SetState(EGUIBS_PopUp);


	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(),			m_Trunk.sfp1(&CreateRoleFrame::EventHandler));
	m_pGUI->RegisterEventHandler(m_pWnd_Control->GetFullName().c_str(), m_Trunk.sfp1(&CreateRoleFrame::EventHandler));

	m_pGUI->SetActive(m_pName);

	return TRUE;
}

//-----------------------------------------------------------------------------
//! 事件处理
//! 
//-----------------------------------------------------------------------------
DWORD CreateRoleFrame::EventHandler(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;

	switch( pEvent->eEvent )
	{
	case EGUIE_EditBoxEnter:
	case EGUIE_Click:
		{
			if( pWnd==m_pBtnOK || pWnd==m_pName )		//确定
			{
				tstring szName = m_pName->GetText();
				this->FilterString(szName);
				if( _tcslen(szName.c_str())!=0 && (szName[0]>='0' && szName[0]<='9') )
				{
					m_pStcErrHint->SetText(g_StrTable[_T("CreateRole_NameBad")]);
					m_pName->SetText(_T(""));
					break;
				}

				if( FilterData::Inst()->IsValidName(szName.c_str(), m_nRoleNameLength)!=E_Success )
				{
					m_pStcErrHint->SetText(g_StrTable[_T("CreateRole_NameBad")]);
					m_pName->SetText(_T(""));
					break;
				}

				if(!m_bSentCreate)							//没有发送过创建角色消息
				{
					m_bSentCreate = true;
					//与服务器消息传递
					tagNC_CreateRole msgCreateRole;
					_tcscpy(msgCreateRole.szRoleName, szName.c_str());
					msgCreateRole.eStatus		= m_eStatus;
					msgCreateRole.AvatarAtt		= m_avatarAtt;
					m_pSession->Send(&msgCreateRole);
					SwitchToState(ECRState_WaitRepose);
				}
			}
			else if(pWnd == m_pBtnRandom)					//随机
			{
				RandAvatarAtt(m_avatarAtt, true);
				AvatarCfg::Inst()->MakeNewPlayerFahion(m_avatarAtt.bySex,m_avatarAtt.wDressMdlID,m_avatarEquip);
				m_pPlayer->UpdateAvatar(m_avatarAtt,m_avatarEquip,SeleRoleMgr::Inst()->GetHugeSG());
				UpdateAttUI(false);
			}
			else if(pWnd == m_pBtnCancel)					//返回
			{
				SwitchToState(ECRState_Close);
			}
			else if(pWnd == m_pLBtnHType)					//发型左按钮
			{
				int id = m_avatarAtt.wHairMdlID;
				id--;
				if(id<0)
					id = AvatarCfg::Inst()->GetNumHairType()-1;
				m_avatarAtt.wHairMdlID = id;
				m_pPlayer->UpdateAvatar(m_avatarAtt,m_avatarEquip,SeleRoleMgr::Inst()->GetHugeSG());
				UpdateAttUI(false);
				//RefreshCamera(true);
				m_pPBtn_Pull->SetState(EGUIBS_PushDown, FALSE);
				m_pPBtn_Push->SetState(EGUIBS_PopUp, FALSE);
			}
			else if(pWnd == m_pRBtnHType)					//发型右按钮
			{
				int id = m_avatarAtt.wHairMdlID;
				id++;
				if(id>=AvatarCfg::Inst()->GetNumHairType())
					id = 0;
				m_avatarAtt.wHairMdlID = id;
				m_pPlayer->UpdateAvatar(m_avatarAtt,m_avatarEquip,SeleRoleMgr::Inst()->GetHugeSG());
				UpdateAttUI(false);
				//RefreshCamera(true);
				m_pPBtn_Pull->SetState(EGUIBS_PushDown, FALSE);
				m_pPBtn_Push->SetState(EGUIBS_PopUp, FALSE);
			}
			else if(pWnd == m_pLBtnHColor)					//发色左按钮
			{
				int id = m_avatarAtt.wHairTexID;
				id--;
				if(id<0)
					id = AvatarCfg::Inst()->GetNumHairColor()-1;
				m_avatarAtt.wHairTexID = id;
				m_pPlayer->UpdateAvatar(m_avatarAtt,m_avatarEquip,SeleRoleMgr::Inst()->GetHugeSG());
				UpdateAttUI(false);
				//RefreshCamera(true);
				m_pPBtn_Pull->SetState(EGUIBS_PushDown, FALSE);
				m_pPBtn_Push->SetState(EGUIBS_PopUp, FALSE);
			}
			else if(pWnd == m_pRBtnHColor)					//发色右按钮
			{
				int id = m_avatarAtt.wHairTexID;
				id++;
				if(id>=AvatarCfg::Inst()->GetNumHairColor())
					id = 0;
				m_avatarAtt.wHairTexID = id;
				m_pPlayer->UpdateAvatar(m_avatarAtt,m_avatarEquip,SeleRoleMgr::Inst()->GetHugeSG());
				UpdateAttUI(false);
				//RefreshCamera(true);
				m_pPBtn_Pull->SetState(EGUIBS_PushDown, FALSE);
				m_pPBtn_Push->SetState(EGUIBS_PopUp, FALSE);
			}
			else if(pWnd == m_pLBtnFace)					//面部左按钮
			{
				int id = m_avatarAtt.wFaceMdlID;
				id--;
				if(id<0)
					id = AvatarCfg::Inst()->GetNumFace()-1;
				m_avatarAtt.wFaceMdlID = id;
				m_pPlayer->UpdateAvatar(m_avatarAtt,m_avatarEquip,SeleRoleMgr::Inst()->GetHugeSG());
				UpdateAttUI(false);
				//RefreshCamera(false);
				m_pPBtn_Push->SetState(EGUIBS_PushDown, FALSE);
				m_pPBtn_Pull->SetState(EGUIBS_PopUp, FALSE);

			}
			else if(pWnd == m_pRBtnFace)					//面部右按钮
			{
				int id = m_avatarAtt.wFaceMdlID;
				id++;
				if(id>=AvatarCfg::Inst()->GetNumFace())
					id = 0;
				m_avatarAtt.wFaceMdlID = id;
				m_pPlayer->UpdateAvatar(m_avatarAtt,m_avatarEquip,SeleRoleMgr::Inst()->GetHugeSG());
				UpdateAttUI(false);
				//RefreshCamera(false);
				m_pPBtn_Push->SetState(EGUIBS_PushDown, FALSE);
				m_pPBtn_Pull->SetState(EGUIBS_PopUp, FALSE);
			}
			else if(pWnd == m_pLBtnFDetail)					//面部细节左按钮
			{
				int id = m_avatarAtt.wFaceDetailTexID;
				id--;
				if(id<0)
					id = AvatarCfg::Inst()->GetNumFaceDetail()-1;
				m_avatarAtt.wFaceDetailTexID = id;
				m_pPlayer->UpdateAvatar(m_avatarAtt,m_avatarEquip,SeleRoleMgr::Inst()->GetHugeSG());
				UpdateAttUI(false);
				//RefreshCamera(false);
				m_pPBtn_Push->SetState(EGUIBS_PushDown, FALSE);
				m_pPBtn_Pull->SetState(EGUIBS_PopUp, FALSE);
			}
			else if(pWnd == m_pRBtnFDetail)					//面部细节右按钮
			{
				int id = m_avatarAtt.wFaceDetailTexID;
				id++;
				if(id>=AvatarCfg::Inst()->GetNumFaceDetail())
					id = 0;
				m_avatarAtt.wFaceDetailTexID = id;
				m_pPlayer->UpdateAvatar(m_avatarAtt,m_avatarEquip,SeleRoleMgr::Inst()->GetHugeSG());
				UpdateAttUI(false);
				//RefreshCamera(false);
				m_pPBtn_Push->SetState(EGUIBS_PushDown, FALSE);
				m_pPBtn_Pull->SetState(EGUIBS_PopUp, FALSE);
			}
			else if(pWnd == m_pLBtnDress)					//服式左按钮
			{
				int id = m_avatarAtt.wDressMdlID;
				id--;
				if(id<0)
					id = AvatarCfg::Inst()->GetNumDress()-1;
				m_avatarAtt.wDressMdlID = id;
				AvatarCfg::Inst()->MakeNewPlayerFahion( m_avatarAtt.bySex, id, m_avatarEquip );
				m_pPlayer->UpdateAvatar(m_avatarAtt,m_avatarEquip,SeleRoleMgr::Inst()->GetHugeSG());
				UpdateAttUI(false);
				//RefreshCamera(true);
				m_pPBtn_Pull->SetState(EGUIBS_PushDown, FALSE);
				m_pPBtn_Push->SetState(EGUIBS_PopUp, FALSE);
			}
			else if(pWnd == m_pRBtnDress)					//服式右按钮
			{
				int id = m_avatarAtt.wDressMdlID;
				id++;
				if(id>=AvatarCfg::Inst()->GetNumDress())
					id = 0;
				m_avatarAtt.wDressMdlID = id;
				AvatarCfg::Inst()->MakeNewPlayerFahion( m_avatarAtt.bySex, id, m_avatarEquip );
				m_pPlayer->UpdateAvatar(m_avatarAtt,m_avatarEquip,SeleRoleMgr::Inst()->GetHugeSG());
				UpdateAttUI(false);
				//RefreshCamera(true);
				m_pPBtn_Pull->SetState(EGUIBS_PushDown, FALSE);
				m_pPBtn_Push->SetState(EGUIBS_PopUp, FALSE);
			}
			else if(pWnd==m_pLBtnRotate || pWnd==m_pRBtnRotate)
			{
				m_bRotate = false;
				m_bRRotate = false;
				m_bLRotate = false;
			}
		}
		break;
	case EGUIE_PushButtonDown:
		{
			if(pEvent->dwParam1 != 1)
				break;
			if( pWnd == m_pMalePBtn )				//男
			{
				memset(&m_avatarAtt, 0x0, sizeof(m_avatarAtt));
				m_avatarAtt.bySex = 1;
				AvatarCfg::Inst()->MakeNewPlayerFahion( m_avatarAtt.bySex, 0, m_avatarEquip );
				m_pPlayer->UpdateAvatar(m_avatarAtt,m_avatarEquip,SeleRoleMgr::Inst()->GetHugeSG());
				m_pPlayer->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Stand,AvatarAniMixer::EAP_All,_T("std1"),true, false, 0.0f, 0.0f);
				m_pPlayer->SetYaw( 0, false );			
				m_pPlayer->Update();	
				UpdateAttUI(false);
			}
			else if( pWnd == m_pFmalePBtn )			//女
			{
				memset(&m_avatarAtt, 0x0, sizeof(m_avatarAtt));
				ZeroMemory(&m_avatarEquip,sizeof(m_avatarEquip));
				m_avatarAtt.bySex = 0;
				AvatarCfg::Inst()->MakeNewPlayerFahion( m_avatarAtt.bySex, 0, m_avatarEquip );
				m_pPlayer->UpdateAvatar(m_avatarAtt,m_avatarEquip,SeleRoleMgr::Inst()->GetHugeSG());
				m_pPlayer->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Stand,AvatarAniMixer::EAP_All,_T("std1"),true, false, 0.0f, 0.0f);
				m_pPlayer->SetYaw( 0, false );			
				m_pPlayer->Update();	
				UpdateAttUI(false);
			}
			else if( pWnd==m_pPBtn_Push && EGUIBS_PushDown==pEvent->dwParam1 )
			{
				RefreshCamera(false);
			}
			else if( pWnd==m_pPBtn_Pull && EGUIBS_PushDown==pEvent->dwParam1 )
			{
				RefreshCamera(true);
			}
		}
		break;
	case EMouse_LeftDown:
		{
			if(	pWnd == m_pLBtnRotate )
			{
				m_bRotate = true;
				m_bRRotate = false;
				m_bLRotate = true;
			}
			else if( pWnd == m_pRBtnRotate )
			{
				m_bRotate = true;
				m_bRRotate = true;
				m_bLRotate = false;
			}
		}
		break;
	}

	return FALSE;
}

VOID CreateRoleFrame::Render3D()
{
	float deltaTime=Kernel::Inst()->GetDeltaTime();

	SeleRoleMgr::Inst()->UpdateSG(deltaTime);

	if(m_bRotate == true )
	{	
		m_pPlayer->SetYaw( m_fRoleYaw, false );			//绕Y轴旋转
		m_pPlayer->Update();	
	}	

	GameFrame::Render3D();
}


//-----------------------------------------------------------------------------
// update
//-----------------------------------------------------------------------------
VOID CreateRoleFrame::Update()
{
	float deltaTime = Kernel::Inst()->GetDeltaTime();

	switch(m_curState)
	{
	case ECRState_Cartoon:
		{
			
		}
		break;
	case ECRState_Inputing:					//等待玩家输入信息
		{
			CanRolerAnimation(deltaTime);

			CheckMouseLeftDown(deltaTime);			//左右旋转鼠标按下

			if(CheckInputedName())			//当玩家姓名，安全码不为空时，且没有发送过创建消息时，确定按钮才可用
			{
				m_pBtnOK->SetEnable(TRUE);
				m_pBtnOK->SetRefresh();
			}
			else
			{
				m_pBtnOK->SetEnable(FALSE);
				m_pBtnOK->SetRefresh();
			}

			RoleRotate(deltaTime);			//角色旋转角度计算

		}
		break;
	case ECRState_WaitRepose:
		{
			
		}
		break;
	case ECRState_PlayAction:				//输入数据后,播放一段动画
		{
			if( m_pPlayer->GetAniPlayer()->IsTrackEnd(AvatarAniPlayer::EAT_Stand,_T("std3")) )
			{
				SwitchToState(ECRState_Close);
			}
		}
		break;
	}

	GameFrame::Update();
}

bool CreateRoleFrame::IsPickRole(int mouseX, int mouseY)
{
	if(m_pPlayer==NULL) return false;
	Ray ray;
	CameraEuler* camera = SeleRoleMgr::Inst()->GetCamera();
	camera->GetMouseRay(mouseX,mouseY,ray);

	TResult tr;

	AABBox box;
	m_pPlayer->GetSceneNode()->GetBox(box);

	tr=ray.Intersect(box);

	if(tr.first && tr.second<FLOAT_MAX)
		return true;
	else
		return false;
}

//角色旋转
void CreateRoleFrame::RoleRotate(float deltaTime)
{
	if(m_bLRotate==true && m_bRotate==true)			//角色左旋转
	{
		if(P_VALID(m_pLBtnRotate))
			m_fRoleYaw = m_pPlayer->GetYaw() - _DegToRad(2)*deltaTime*ROTATE_SPEED;
	}
	else if(m_bRotate==true && m_bRRotate==true)	//角色右旋转
	{
		if(P_VALID(m_pRBtnRotate))
			m_fRoleYaw = m_pPlayer->GetYaw() + _DegToRad(2)*deltaTime*ROTATE_SPEED;								
	}	
}

const Vector3 PREROLE_POS = Vector3(19050, 350, 14200);
const Vector3 Lookat = Vector3(19100,450,14600);
const Vector3 LookatNear = Vector3(19070,500,14900);
const float CamYaw=_DegToRad(0.0f);
const float CamPitch=_DegToRad(0.0f);
const float CamRoll=0.0f;
const float MAXCamDist=800.0f;

//开始创建角色
void CreateRoleFrame::BeginCreateRole()
{
	SwitchToState(ECRState_Inputing);
	
	m_avatarAtt.bySex = 0;			//0:女 1:男

	m_pPlayer			= new Player;						//创建一个Node
	if( P_VALID(m_pPlayer) )
	{
		SeleRoleMgr::Inst()->OpenMap(_T("cj-1"), SeleRoleMgr::Inst()->GetCamera(), SeleRoleMgr::ECreate_Role);
			
		AvatarCfg::Inst()->MakeNewPlayerFahion( m_avatarAtt.bySex, 0, m_avatarEquip );
		tagDisplaySet displaySet;
		displaySet.bFashionDisplay = 1;
		m_pPlayer->LoadAvatar(m_avatarAtt, m_avatarEquip, displaySet);
		m_pPlayer->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Other,AvatarAniMixer::EAP_All,_T("cret"),true, false, 0.0f, 0.0f);
		m_pPlayer->SetPos(PREROLE_POS);
		m_pPlayer->Update();
		//把Node加入到场景中
		m_pPlayer->AttachSceneNodeToSG(SeleRoleMgr::Inst()->GetHugeSG());	
		SeleRoleMgr::Inst()->SetCamera(Lookat, CamYaw, CamPitch, CamRoll, MAXCamDist);
	}

	//设置界面
	RandAvatarAtt(m_avatarAtt);	
	UpdateAttUI(false);
}


//结束创建角色
void CreateRoleFrame::EndCreateRole()
{
	m_pPlayer->DetachSceneNodeFromSG(SeleRoleMgr::Inst()->GetHugeSG());		//从场景中删除结点
	SeleRoleMgr::Inst()->CloseMap();
	SelectRoleFrame* selRoleFrame = (SelectRoleFrame *)m_pFrameMgr->CreateFrame(_T("Root"), _T("SelectRole"), _T("SelectRoleFrame"), 0);
	selRoleFrame->SetCurState(SelectRoleFrame::ESRS_Loading);

	LoadingFrame* pFrame = (LoadingFrame*)m_pFrameMgr->CreateFrame(_T("Root"), _T("Loading"), _T("LoadingFrame"), 0);
	if( P_VALID(pFrame) )
		pFrame->SetLoadingType(LoadingFrame::ELT_SelectRole);
	m_pFrameMgr->AddToDestroyList(this);		
	
}


//更新属性设置
void CreateRoleFrame::UpdateAttUI(bool bSave)
{
	if(m_avatarAtt.bySex==1)			//男
	{
		if(P_VALID(m_pSexMutex))
			m_pSexMutex->SetPush(m_pMalePBtn);
	}
	else								//女
	{
		if(P_VALID(m_pSexMutex))
			m_pSexMutex->SetPush(m_pFmalePBtn);
	}

	TCHAR szTemp[256] = {0};
	_stprintf(szTemp, _T("%d"), m_avatarAtt.wHairMdlID+1);			//设置发型
	if(P_VALID(m_pStcHType))
		m_pStcHType->SetText(szTemp);

	_stprintf(szTemp, _T("%d"), m_avatarAtt.wHairTexID+1);			//设置发色
	if(P_VALID(m_pStcHColor))
		m_pStcHColor->SetText(szTemp);

	_stprintf(szTemp, _T("%d"), m_avatarAtt.wFaceMdlID+1);			//设置面部
	if(P_VALID(m_pStcFace))
		m_pStcFace->SetText(szTemp);

	_stprintf(szTemp, _T("%d"), m_avatarAtt.wFaceDetailTexID+1);	//设置面部细节
	if(P_VALID(m_pStcFDetail))
		m_pStcFDetail->SetText(szTemp);

	_stprintf(szTemp, _T("%d"), m_avatarAtt.wDressMdlID+1);			//设置服式
	if(P_VALID(m_pStcFDress))
		m_pStcFDress->SetText(szTemp);

}


//状态转换开关
void CreateRoleFrame::SwitchToState(ECRState nextState)
{
	m_curState = nextState;
	switch(m_curState)
	{
	case ECRState_Inputing:
		{
			m_bSentCreate = false;
			if( P_VALID(m_pName) )
				m_pName->SetText(_T(""));
			if( P_VALID(m_pBtnOK) )
				m_pBtnOK->SetEnable(true);
		}
		break;
	case ECRState_WaitRepose:
		{
			if( P_VALID(m_pStcErrHint) )
				m_pStcErrHint->SetText(g_StrTable[_T("WaitAuth")]);
			m_pBtnOK->SetEnable(false);
		}
		break;
	case ECRState_PlayAction:
		{
			if( P_VALID(m_pWnd) )
				m_pWnd->SetInvisible(TRUE);
			if( P_VALID(m_pWnd_Control) )
				m_pWnd_Control->SetInvisible(TRUE);
			m_pPlayer->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Stand,AvatarAniMixer::EAP_All,_T("std3"),false/*, false, 0.0f, 0.0f*/);
		}
		break;
	case ECRState_Close:
		{	
			EndCreateRole();
		}
		break;
	}
}

//当玩家输入了姓名后并且没有发过创建消息，确定按钮才可用
BOOL CreateRoleFrame::CheckInputedName()
{
	if( !P_VALID(m_pName) )
		return FALSE;
	tstring name = m_pName->GetText();
	if( name.c_str()==NULL || _tcslen(name.c_str())==0 || m_bSentCreate)
		return FALSE;
	return TRUE;
}

//随机设置角色属性
void CreateRoleFrame::RandAvatarAtt(tagAvatarAtt& att,bool bRandAll)
{
	if(bRandAll)
	{
		att.wHairMdlID			= m_pUtil->Rand() % AvatarCfg::Inst()->GetNumHairType();
		att.wHairTexID			= m_pUtil->Rand() % AvatarCfg::Inst()->GetNumHairColor();
		att.wFaceMdlID			= m_pUtil->Rand() % AvatarCfg::Inst()->GetNumFace();
		att.wFaceDetailTexID	= m_pUtil->Rand() % AvatarCfg::Inst()->GetNumFaceDetail();
		att.wDressMdlID			= m_pUtil->Rand() % AvatarCfg::Inst()->GetNumDress();
	}
}

//鼠标左键按下旋转按钮
void CreateRoleFrame::CheckMouseLeftDown(float deltaTime)
{
	if(!P_VALID(m_pLBtnRotate))
		return;
	if(!P_VALID(m_pRBtnRotate))
		return;
	if(m_pLBtnRotate->GetState()==1 && m_pGUI->GetMousePoint()==m_pLBtnRotate)				//左旋转鼠标按下
	{
		tagGUIEvent event(m_pLBtnRotate, (EGUIEvent)EMouse_LeftDown);
		m_pLBtnRotate->SendEvent(&event);
	}
	else if(m_pRBtnRotate->GetState()==1 && m_pGUI->GetMousePoint()==m_pRBtnRotate)			//右旋转鼠标按下
	{
		tagGUIEvent event(m_pRBtnRotate, (EGUIEvent)EMouse_LeftDown);
		m_pRBtnRotate->SendEvent(&event);
	}
	//拾取角色
	else if(m_pInput->IsMouseDownOnce(0)
		&& m_pGUI->GetActive()==m_pGUI->GetDesktop())
	{
		const tagPoint& ptCursor = CursorMgr::Inst()->GetCursorPos();
		if(IsPickRole(ptCursor.x, ptCursor.y))
			m_bPickRole = true;
		else 
			m_bPickRole = false;
	}
	//通过鼠标旋转角色
	else if(m_pInput->IsMouseDown(0)
		&& m_pInput->IsMouseMove() 
		&& m_bPickRole)
	{
		FLOAT fRelativeX = m_pInput->GetRelativeX();
		m_fRoleYaw = m_pPlayer->GetYaw() + _DegToRad(2)*fRelativeX*ROTATE_SPEED*deltaTime;
		m_bRotate = true;
	}
	else
	{
		m_bRotate = false;
		m_bRRotate = false;
		m_bLRotate = false;
	}	
}

//接收服务器消息(参数待定)
DWORD CreateRoleFrame::NetRecvCreateRole(tagNS_CreateRole* pMsg, DWORD pPrama)
{
	if( pMsg->dwID != m_pUtil->Crc32("NS_CreateRole") )
		return 0;

	//创建角色成功后，播放一段动画
	if( pMsg->dwErrorCode == E_Success )
	{
		tagSimRole* pRole = (tagSimRole *)pMsg->bySimRoleInfo;
		Player* pPlayer = new Player;
		SeleRoleMgr::Inst()->CreateRole(*pRole, pPlayer);
		m_pPlayer->SetYaw( 0, false );			//绕Y轴旋转
		m_pPlayer->Update();	
		RefreshCamera(true);
		SwitchToState(ECRState_PlayAction);
	}
	else
	{
		switch(pMsg->dwErrorCode)
		{
		case E_CreateRole_RoleCreating:
			m_pStcErrHint->SetText(g_StrTable[_T("CreateRole_RoleCreating")]);
			break;
		case E_CreateRole_RoleEnumNotDone:
			m_pStcErrHint->SetText(g_StrTable[_T("CreateRole_RoleEnumNotDone")]);
			break;
		case E_CreateRole_RoleDeleting:
			m_pStcErrHint->SetText(g_StrTable[_T("CreateRole_RoleDeleting")]);
			break;
		case E_CreateRole_RoleNumFull:
			m_pStcErrHint->SetText(g_StrTable[_T("CreateRole_RoleNumFull")]);
			break;
		case E_CreateRole_NameExist:
			m_pStcErrHint->SetText(g_StrTable[_T("CreateRole_NameExist")]);
			break;
		case E_CreateRole_NameInvalid:
			m_pStcErrHint->SetText(g_StrTable[_T("CreateRole_NameInvalid")]);
			break;
		case E_CreateRole_SafeCodeInvalid:
			m_pStcErrHint->SetText(g_StrTable[_T("CreateRole_SafeCodeInvalid")]);
			break;
		case E_CreateRole_InWorld:
			m_pStcErrHint->SetText(g_StrTable[_T("CreateRole_InWorld")]);
			break;
		case E_CreateRole_RoleLoading:
			m_pStcErrHint->SetText(g_StrTable[_T("CreateRole_RoleLoading")]);
			break;
		case E_SystemError:
			m_pStcErrHint->SetText(g_StrTable[_T("SystemError")]);
			break;
		case E_Filter_Name_TooLong:
			m_pStcErrHint->SetText(g_StrTable[_T("Common_Error30")]);
			break;
		case E_Filter_Name_Null:
			m_pStcErrHint->SetText(g_StrTable[_T("Common_Error31")]);
			break;
		case E_Filter_Name_Illegal:
		case E_Filter_Name_InFilterFile:
			m_pStcErrHint->SetText(g_StrTable[_T("CreateRole_NameInvalid")]);
			break;
		default:
			m_pStcErrHint->SetText(g_StrTable[_T("SystemError")]);
			break;
		}
		m_bSentCreate = false;
		m_pBtnOK->SetEnable(true);
		m_pWnd->SetRefresh(TRUE);
		SwitchToState(ECRState_Inputing);
	}

	return 0;
}

void CreateRoleFrame::CanRolerAnimation(float time)
{
	switch(m_bAnimation)		// 检测镜头的位置，0初始值，1表示拉近，-1表示还原
	{
	case 1:		// 拉近
		{
			if(m_fRolerTimer > 5.0f)
			{	// 拉近角色后持续5秒没有动作则还原
				RefreshCamera(true);
				m_pPBtn_Push->SetState( EGUIBS_PopUp );
			}
			else
			{
				m_fRolerTimer += time;		// 累计拉近角色后持续的时间
			}
			if(m_fAnimDelay < 0.033f*4.0f)
			{
				m_fAnimDelay  += time;		// 延迟一段时间再改变镜头
			}
			else
			{
				SeleRoleMgr::Inst()->GetCamera()->Update(LookatNear);
			}
		}
		break;
	case -1:		// 拉远
		{			
			if(m_fAnimDelay < 0.033f*2.0f)
			{
				m_fAnimDelay  += time;		// 延迟一段时间再改变镜头
			}
			else 
			{
				SeleRoleMgr::Inst()->GetCamera()->Update(Lookat);
			}
		}
		break;
	case 0:		// Init
		{
			m_fRolerTimer = 0.0f;
			m_fAnimDelay  = 0.0f;
		}
		break;
	default:
		break;
	}    	
}

void CreateRoleFrame::RefreshCamera( bool bFar )
{
	if( bFar )
	{
		m_bAnimation  = -1;
		m_fRolerTimer = 0.0f;		
		m_fAnimDelay  = 0.0f;
	}
	else 
	{
		m_bAnimation  = 1;
		m_fRolerTimer = 0.0f;		
		m_fAnimDelay  = 0.0f;
	}
}

VOID CreateRoleFrame::FilterString( tstring& str )
{
	// 去掉回车
	INT nFind = 0;
	while( (nFind = str.find(_T('\r'), nFind)) != str.npos )
		str.replace(nFind, 1, _T(""));

	// 去掉换行
	nFind = 0;
	while( (nFind = str.find(_T('\n'), nFind)) != str.npos )
		str.replace(nFind, 1, _T(""));

}