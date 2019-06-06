#include "StdAfx.h"
#include ".\role.h"
#include "MapView.h"
#include "ClientApp.h"
#include ".\Win32App\GInput.h"
#include "FSM.h"
#include "GameEvent.h"
#include "..\Cool3D\SpecialEffect\EffectAll.h"

Role::Role(void)
{
	m_pSGNode=NULL;
	m_pFSM=new FSM;
	m_pFSM->Init();
	m_pFSM->SetRole(this);

	m_pSelEffect=NULL;
	m_fMoveSpeed = 500.0f;
	m_fJumpYSpeed = 800.0f;
}

Role::~Role(void)
{
	m_pSGNode=NULL;//自己不delete,而交给SceneGraph取delete
	SAFE_DELETE(m_pFSM);
}

void Role::Update(MapView *pMap,NavMap *pNav)
{
	if(GInput::Inst()->MouseDownFirst(0))
	{
		CameraBase *pCamera=App()->GetCamera();
		POINT coursorPt=GInput::Inst()->GetCursorPos();
		Ray ray;
		pCamera->GetMouseRay(coursorPt.x,coursorPt.y,ray);
		Vector3 pickPos;
		if(pMap->RayCollide(ray,pickPos))
		{
			tagMoveEvent event;
			event.start=m_space.GetPos();
			event.end=pickPos;
			m_pFSM->OnGameEvent(&event);
		}
	}
	else if(GInput::Inst()->KeyDownFirst(DIK_SPACE))
	{
		tagJumpEvent event;
		m_pFSM->OnGameEvent(&event);
	}
#if 0
	else if(GInput::Inst()->KeyDownFirst(DIK_BACK))//test
	{
		NavMap* pNav=App()->GetMapView()->GetNav();
		NavCarrierNode* pCarrier=pNav->GetCollider()->FindCarrier(20);


		Vector3 worldPos(11831.994141f,2118.576416f,10071.251953f);
		Vector3 localPos(503.87604f,301.27859f,413.51764f);
		pCarrier->PosToLocal(worldPos,313.219f,localPos);

		Vector3 nowPos;
		pCarrier->PosToWorld(localPos,Kernel::Inst()->GetAccumTime(),nowPos);
		SetPos(nowPos);

		tagStandOnCarrierEvent event;
		event.dwCarrierObjID=20;
		event.startTime=Kernel::Inst()->GetAccumTime();
		m_pFSM->Change2State(&event);
	}
#endif

	m_pFSM->Update();

	if(m_pSGNode)
		m_pSGNode->SetWorldMat(m_space.GetMatrix());

	if(m_pSelEffect)
	{
		/*
		Transform trans;
		Vector3 pos=m_space.GetPos();
		pos.y=0;
		trans.Translate(pos);
		//m_pSelEffect->SetWorldMat(trans.GetMatrix());
		*/
		m_pSelEffect->SetBoxCenter( m_space.GetPos() );
	}
}

void Role::AddSkin(const TCHAR *szMdlName)
{
	SGSkinNode *pNewSkin=new SGSkinNode;
	pNewSkin->LoadRes(szMdlName);
	m_pSGNode->AttachChild(pNewSkin);
}

void Role::AddCloth( const TCHAR *szMdlName )
{
	SGClothNode *pNewCloth=new SGClothNode;
	pNewCloth->LoadRes(szMdlName);
	m_pSGNode->AttachChild(pNewCloth);
}

void Role::LoadDefaultAvatar(BYTE bySex)
{
	ASSERT(m_pSGNode==NULL);

	AvatarNode *pSGNode=new AvatarNode;
	m_pSGNode=pSGNode;

	pSGNode->LoadRes(_T("data\\animation\\AvatarGirl\\std1.fskel"));
	AddSkin(_T("data\\model\\avatargirl\\hair\\Fhair01_1_hair.fskin"));
	AddSkin(_T("data\\model\\avatargirl\\face\\Fface01_face.fskin"));
	AddSkin(_T("data\\model\\avatargirl\\upper\\XSY2_upper.fskin"));
	AddSkin(_T("data\\model\\avatargirl\\glove\\XSW2_l_glove.fskin"));
	AddSkin(_T("data\\model\\avatargirl\\glove\\XSW2_r_glove.fskin"));
	AddSkin(_T("data\\model\\avatargirl\\lower\\XSK2_lower.fskin"));
	AddSkin(_T("data\\model\\avatargirl\\boot\\XSX2_boot.fskin"));

	pSGNode->GetSkeletonAniCtrl()->OpenTrack(_T("std1"),true);

	//test dyna light
#if 0
	SGLightNode *pTestLgt=new SGLightNode;
	tagLight lgt;
	lgt.ambient=Color4f(1,1.0f,1.0f,1.0f);
	lgt.attenuation=Vector3(0.0f,0.008f,0.0f);
	lgt.diffuse=Color4f(0,0,0,0);
	lgt.pos=Vector3(0,200,20);
	lgt.range=4000;
	lgt.type=Light_Point;
	pTestLgt->SetLight(&lgt);
	pSGNode->AttachChild(pTestLgt);
#endif

	//test decal effect
#if 0
	EffectSceneDecal *pDecal=new EffectSceneDecal;
	tagMtlEffectDesc decalMtl;
	_tcscpy(decalMtl.szTexName,_T("data\\System\\Effect\\GreenRing.bmp"));
	decalMtl.blendType=1;
	decalMtl.bTestZ = true;

	EffectSceneDecal::tagDecalProp prop;
	EffectSceneDecal::tagState state;
	state.size.x = 500.0f;
	state.size.y = 500.0f;
	pDecal->BeginBuild(prop,true);
	pDecal->AddKeyState(state);
	pDecal->EndBuild();
	pDecal->CreateMtl(decalMtl);

	m_pSelEffect=new SGEffectNode;
	m_pSelEffect->AddEffect(pDecal);
	m_pSelEffect->Play();
#endif

}

void Role::AttachToSceneGraph(SceneGraph *pSG)
{
	pSG->AttachDynamicNode(m_pSGNode);
	if(m_pSelEffect)
		m_pSGNode->AttachChild(m_pSelEffect);
}

void Role::SetPos( Vector3 pos )
{
	m_space.SetPos(pos);
}

Vector3 g_roleSize(50.0f,175.0f,50.0f);
const Vector3& Role::GetRoleSize()
{
	return g_roleSize;
}

void Role::PlayTrack( const TCHAR* szTrackName, bool loop )
{
	m_pSGNode->GetSkeletonAniCtrl()->OpenTrack(szTrackName,loop,0.0f,0.2f);
}

float Role::GetMoveSpeed()
{
	return m_fMoveSpeed;
}

float Role::GetDropXzSpeed()
{
	return 500.0f;
}

float Role::GetJumpXzSpeed()
{
	return 500.0f;
}

float Role::GetJumpYSpeed()
{
	return 1000.0f;
}

float Role::GetSlideSpeed()
{
	return 600.0f;
}