#include "StdAfx.h"
#include "RoleHeadPicIcon.h"
#include "Role.h"
#include "RoleMgr.h"
#include "Player.h"
#include "LocalPlayer.h"
#include "NPC.h"
#include "AvatarEquipNode.h"

RoleHeadPicIcon::RoleHeadPicIcon() :
m_Trunk( this ),
m_pHeadImg( NULL ),
m_pSG( NULL ),
m_pCamera( NULL ),
m_pAvatarNode( NULL ),
m_bRefershAvatar( FALSE ),
m_bRefershNPC( FALSE ),
m_vCamDir( 0.3f, 0.0f, 1.0f ),
m_vCamLooatOffset( -0.2f, -0.2f, 0.0f ),
m_fCamDist( 1.7f ),
m_fRenderTargetZoom( 2.0f ),
m_dwBackColor( 0xFFFFFFFF )
{
}

RoleHeadPicIcon::~RoleHeadPicIcon()
{
}

//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL RoleHeadPicIcon::Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement)
{
	if( !ActiveStatic::Init(pSystem, pFather, pXmlElement) )
		return FALSE;

	m_dwBackColor = m_dwPicColor;
	m_dwPicColor = 0xFFFFFFFF;
	m_dwInitPicColor = m_dwPicColor;

	TObjRef<GameFrameMgr>()->RegisterEventHandle( _T("DeviceResetEvent"), (FRAMEEVENTPROC)m_Trunk.sfp1(&RoleHeadPicIcon::OnDeviceResetEvent));

	return TRUE;
}

//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
VOID RoleHeadPicIcon::Destroy()
{
	TObjRef<GameFrameMgr>()->UnRegisterEventHandler( _T("DeviceResetEvent"), (FRAMEEVENTPROC)m_Trunk.sfp1(&RoleHeadPicIcon::OnDeviceResetEvent));

	ActiveStatic::Destroy();
	FreeDynamicData();
	m_strNpcHeadPicFileName = _T("");
}

VOID RoleHeadPicIcon::OnDeviceResetEvent(tagGameEvent* )
{
	if( P_VALID( m_pAvatarNode ) )
		m_bRefershAvatar = TRUE;
	else if( !m_strNpcHeadPicFileName.empty() )
	{
		FreeDynamicData();
		NewNPCData();
		SetRefresh();
	}
}

VOID RoleHeadPicIcon::FreeDynamicData()
{
	if( P_VALID( m_pSG ) && P_VALID( m_pAvatarNode ) )
		m_pSG->DetachDynamicNode( m_pAvatarNode );

	SAFE_DEL( m_pAvatarNode );
	SAFE_DEL( m_pSG );
	SAFE_DEL( m_pCamera );

	if( P_VALID( m_pHeadImg ) )
	{
		if( m_pHeadImg->bRenderTarget )
		{
			m_pRender->ReleaseRenderTarget(m_pHeadImg->dwHandle);
			m_pHeadImg->dwHandle = GT_INVALID;
		}
		m_pRender->DestroyImage( m_pHeadImg );
		m_pHeadImg = NULL;
	}
	m_bRefershAvatar = FALSE;
	m_bRefershNPC = FALSE;
}

VOID RoleHeadPicIcon::NewPlayerData()
{
	ASSERT( !P_VALID( m_pHeadImg ) );

	// 因为需要深度缓冲，所以无法使用Render2D创建
	m_pHeadImg = new tagGUIImage;
	if( !P_VALID(m_pHeadImg) )
		return;

	tagRect rc( 0, 0, (INT)( m_ptSize.x * m_fRenderTargetZoom ), (INT)( m_ptSize.y * m_fRenderTargetZoom ) );
	m_pHeadImg->rc = rc;
	m_pHeadImg->ptSize.x = rc.right - rc.left;
	m_pHeadImg->ptSize.y = rc.bottom - rc.top;

	IRenderTexture* pRenderTexture = Device()->NewRenderTexture();
	bool bResult = pRenderTexture->Create(m_pHeadImg->ptSize.x, m_pHeadImg->ptSize.y, EPF_A8R8G8B8, true, false, 1, false);
	if( !bResult )	// 创建RenderTarget失败
	{
		SAFE_DEL(pRenderTexture);
		SAFE_DEL(m_pHeadImg);
		return;
	}
	m_pHeadImg->dwHandle = (DWORD)pRenderTexture;
	m_pHeadImg->bRenderTarget = TRUE;

	m_pCamera = new CameraBase;
	m_pCamera->SetPerspective( (float)(rc.right-rc.left), (float)(rc.bottom-rc.top), _DegToRad(45.0f), 1.0f, 20000.0f);

	m_pSG = new NullSceneGraph;
	m_pSG->BindCamera( m_pCamera );

	m_pAvatarNode = new AvatarEquipNode;
	m_pSG->AttachDynamicNode( m_pAvatarNode );
}

VOID RoleHeadPicIcon::NewNPCData()
{
	ASSERT( !P_VALID( m_pHeadImg ) );

	tagRect rcRT( 0, 0, (INT)m_ptSize.x, (INT)m_ptSize.y );                // 为了使用m_dwPicColor为背景色
	m_pHeadImg = m_pRender->CreateImage( m_strFullName, rcRT );
	m_bRefershNPC = TRUE;
}

VOID RoleHeadPicIcon::Render()
{
	if( !IsRefresh() )
		return GUIWnd::Render();

	if( !P_VALID( m_pPic ) || !P_VALID( m_pHeadImg ) )
		return;

	if( !P_VALID( m_pPic->dwHandle ) || !P_VALID( m_pHeadImg->dwHandle ) )
		return;

	if( m_bRefershAvatar )
		return;

	// 把NPC头相绘制在RenderTarget上
	if( m_bRefershNPC && !m_strNpcHeadPicFileName.empty() )
	{
		tagRect rcTex;
		tagGUIImage* pImg = m_pRender->CreateImage( m_strNpcHeadPicFileName, rcTex );
		IRenderTexture* pRT = (IRenderTexture*)m_pHeadImg->dwHandle;
		pRT->Begin();

		Device()->Clear( true, false, false, m_dwBackColor, 1.0f, 0 );
		m_pRender->Draw( m_pHeadImg, pImg, 0xFFFFFFFF, m_eAlphaWrite);

		pRT->End();
		m_pRender->DestroyImage( pImg );
		m_bRefershNPC = FALSE;
	}

	m_pCanvas->rc.SetLTWH( m_ptView, m_ptSize );
	tagRect rcDesc = m_pCanvas->rc - m_pCanvas->ptOffset;
	tagRect rcSrc;
	rcSrc = m_pHeadImg->ptSize;

	IDraw2D::Inst()->Draw( (RECT*)&rcDesc,
			               (const RECT*)&rcSrc, 
			               (const RECT*)&m_pPic->rc, 
			               (IRenderTexture*)(m_pHeadImg->dwHandle), 
			               (ResTexture*)(m_pPic->dwHandle), ETFilter_LINEAR, m_dwPicColor );

	GUIWnd::Render();
}


//-----------------------------------------------------------------------------
// 设置窗口是否隐藏
//-----------------------------------------------------------------------------
VOID RoleHeadPicIcon::SetInvisible(BOOL b)
{
	if( m_bInvisible == b )
		return;

	if( b )
		FreeDynamicData();

	ActiveStatic::SetInvisible(b);
}

VOID RoleHeadPicIcon::UpdataHeadPic( Role* pRole )
{
	if( !P_VALID(pRole) )
		return;

	SceneNode* pNode = pRole->GetSceneNode();
	ASSERT( P_VALID(pNode) );

	if( pNode->IS_STATIC_CLASS( AvatarEquipNode ) )
	{
		AvatarEquipNode* pAvataNode = (AvatarEquipNode*)pNode;
		UpdataAvatarHeadPic( pAvataNode );
	}
	else if( pNode->IS_STATIC_CLASS(EntityNode) )
	{
		EntityNode* pEntityNode = (EntityNode*)pNode;
		const tstring strMdlFileName = pEntityNode->GetResObj()->GetName();
		m_strNpcHeadPicFileName = FileNameMdlToHeadPic( strMdlFileName );

		FreeDynamicData();
		NewNPCData();
		SetRefresh();
	}
}

tstring RoleHeadPicIcon::FileNameMdlToHeadPic( tstring strMdlFileName, BOOL bFSKEL /* = FALSE */ )
{
	tstring::size_type pos = strMdlFileName.find_last_of( _T('\\') );
	if( tstring::npos == pos )
		return _T("");

	strMdlFileName.erase( pos );
	pos = strMdlFileName.find_last_of( _T('\\') );
	if( tstring::npos == pos )
		return _T("");

	tstring strName = strMdlFileName.substr( pos + 1 );
	strMdlFileName += _T("\\");
	strMdlFileName += strName;
	strMdlFileName += _T("_ico.tga");
	return strMdlFileName;
}

BOOL RoleHeadPicIcon::UpdataCamera()
{
	ASSERT( P_VALID(m_pAvatarNode) );
	ASSERT( m_pAvatarNode->IsAllResourceLoaded() );
	ASSERT( P_VALID(m_pCamera) );

	const SceneNode *pSGFaceNode = m_pAvatarNode->GetElementMainMdl( EAE_Face );
	if( !P_VALID(pSGFaceNode) )
		return FALSE;
	ASSERT( pSGFaceNode->IS_STATIC_CLASS(SGSkinNode) );
	const ResSkin *pRes = ((const SGSkinNode*)pSGFaceNode)->GetResSkin();
	AABBox boxHead;
	boxHead = *(pRes->GetAABBox());
	
	Vector3 vLookat = boxHead.GetCenter();
	vLookat.x += m_vCamLooatOffset.x * ( boxHead.max.x - boxHead.min.x );
	vLookat.y += m_vCamLooatOffset.y * ( boxHead.max.y - boxHead.min.y );
	vLookat.z += m_vCamLooatOffset.z * ( boxHead.max.z - boxHead.min.z );
	float fLen = Vec3Dist( boxHead.min, boxHead.max ) * m_fCamDist;
	Vec3Normalize( m_vCamDir );
	m_pCamera->SetLookAt( vLookat - m_vCamDir * fLen, vLookat, MathConst::AxisY );

	return TRUE;
}

VOID RoleHeadPicIcon::UpdataAvatarHeadPic( const tagAvatarAtt& att, const tagAvatarEquip& ep, const tagDisplaySet* const pDisplaySet )
{
	FreeDynamicData();
	NewPlayerData();

	if( NULL != pDisplaySet )
		m_pAvatarNode->SetDisplaySet( *pDisplaySet );
	m_pAvatarNode->LoadAvatar( att, ep );
	m_pAvatarNode->GetSkeletonAniCtrl()->OpenTrack( _T("std1"), false );
	m_pAvatarNode->GetSkeletonAniCtrl()->SetTimeFactor( 0.0f );

	m_bRefershAvatar = TRUE;
}

VOID RoleHeadPicIcon::UpdataAvatarHeadPic( AvatarEquipNode* pAvatarNode )
{
	// 获取外观属性
	const tagAvatarAtt& att = pAvatarNode->GetAtt();

	// 获取所有装备
	tagAvatarEquip ep;
	ZeroMemory( &ep, sizeof(tagAvatarEquip) );
	for( int i = EAE_Start; i <= EAE_End; i++ )
	{
		ep.AvatarEquip[i] = pAvatarNode->GetEquip( (EAvatarElement)i );
	}
	UpdataAvatarHeadPic( att, ep, &pAvatarNode->GetDisplaySet() );
}

VOID RoleHeadPicIcon::UpdataDisplaySet( const tagDisplaySet& ds )
{
	if( NULL == m_pAvatarNode )
		return;
	m_pAvatarNode->SetDisplaySet( ds );
	m_bRefershAvatar = TRUE;
}

VOID RoleHeadPicIcon::Render3D()
{
	if( m_bInvisible )
		return;
	if( !m_bRefershAvatar )
		return;
	if( !P_VALID( m_pAvatarNode ) )
		return;

	m_pAvatarNode->ForceUpdate( 0.1f, m_pSG );		// 因为是静态的，所以指定一个常量DeltaTime
	m_pAvatarNode->Update( 0.1f, m_pSG );
	if( !m_pAvatarNode->IsAllResourceLoaded() )		// 资源是否加载完成
		return;
	if( !UpdataCamera() )
		return;

	IDevice* pDev = Device();
	IRenderTexture* pRT = (IRenderTexture*)m_pHeadImg->dwHandle;
	pRT->Begin();
	pDev->Clear( true, true, false, m_dwBackColor, 1.0f, 0 );

	// 关闭后期处理
	const bool bEnablePostProcess = Kernel::Inst()->GetRenderSys()->IsEnablePostProcess();
	Kernel::Inst()->GetRenderSys()->EnablePostProcess( false );
	m_pSG->Render( 0.1f );
	Kernel::Inst()->GetRenderSys()->EnablePostProcess( bEnablePostProcess );

	pRT->End();

	SetRefresh();
	m_bRefershAvatar = FALSE;
}
