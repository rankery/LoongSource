#include "StdAfx.h"
#include "PlayerViewWindow.h"
#include "Role.h"
#include "RoleMgr.h"
#include "Player.h"
#include "LocalPlayer.h"
#include "AvatarEquipNode.h"

const float g_fRotateSpeed = 2.0f;
const Vector3 g_vCamLookAt( 0.0f, 85.0f, 0.0f );
const Vector3 g_vCamFrom( 0.0f, 120.0f, -230.0f );

PlayerViewWindow::PlayerViewWindow() :
m_pAvatarNode( NULL ),
m_pSG( NULL ),
m_fAvatarYaw( 0.0f ),
m_pBnRotaLeft( NULL ),
m_pBnRotaRight( NULL )
{
	m_pCanvas = NULL;
	m_pNewCanvas = NULL;
}

PlayerViewWindow::~PlayerViewWindow()
{
}

//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL PlayerViewWindow::Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement)
{
	if( !GUIStatic::Init(pSystem, pFather, pXmlElement) )
		return FALSE;

	// 因为需要深度缓冲，所以无法使用Render2D创建
	m_pNewCanvas = new tagGUIImage;
	if( !P_VALID(m_pNewCanvas) )
		return FALSE;

	m_pNewCanvas->rc = tagRect( 0, 0, m_ptSize.x, m_ptSize.y );
	m_pNewCanvas->ptSize = m_ptSize;

	IRenderTexture* pRenderTexture = Device()->NewRenderTexture();
	bool bResult = pRenderTexture->Create(m_pNewCanvas->ptSize.x, m_pNewCanvas->ptSize.y, EPF_A8R8G8B8, true, false, 1, false);
	if( !bResult )	// 创建RenderTarget失败
	{
		SAFE_DEL(pRenderTexture);
		SAFE_DEL(m_pNewCanvas);
		return FALSE;
	}
	m_pNewCanvas->dwHandle = (DWORD)pRenderTexture;
	m_pNewCanvas->bRenderTarget = TRUE;

	m_pNewCanvas->ptOffset.x = m_ptView.x;
	m_pNewCanvas->ptOffset.y = m_ptView.y;
	m_pNewCanvas->bRefresh = TRUE;

	m_Camera.SetLookAt( g_vCamFrom, g_vCamLookAt, MathConst::AxisY );
	m_Camera.SetPerspective( (float)m_ptSize.x, (float)m_ptSize.y, _DegToRad(45.0f), 1.0f, 20000.0f);
	
	m_pSG = new NullSceneGraph;
	m_pSG->BindCamera( &m_Camera );
	m_pAvatarNode = new AvatarEquipNode;
	
	return TRUE;
}


//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
VOID PlayerViewWindow::Destroy()
{
	GUIStatic::Destroy();
	
	m_pBnRotaLeft = NULL;
	m_pBnRotaRight = NULL;

	m_pSG->DetachDynamicNode( m_pAvatarNode );
	SAFE_DEL( m_pAvatarNode );
	SAFE_DEL( m_pSG );
	m_pRender->ReleaseRenderTarget(m_pNewCanvas->dwHandle);
	m_pRender->DestroyImage(m_pNewCanvas);
	m_pNewCanvas = NULL;
}

VOID PlayerViewWindow::UpdataCamera()
{
	if( !P_VALID(m_pAvatarNode ) )
		return;

	// 计算AvatarNode的包围盒
	AABBox boxAvatar;
	const int nNumChild = m_pAvatarNode->GetNumChildren();
	for( int i = 0; i < nNumChild; i++ )
	{
		const SceneNode *pChild = m_pAvatarNode->GetChild( i );
		if(pChild->IS_STATIC_CLASS(SGSkinNode))
		{
			const ResSkin *pRes=((const SGSkinNode*)pChild)->GetResSkin();
			boxAvatar.Merge(*(pRes->GetAABBox()));
		}
	}
	boxAvatar.Transform( m_pAvatarNode->GetWorldMat() );

	Vector3 vLookAt = boxAvatar.GetCenter();
	Vector3 vFrom = vLookAt;
	vFrom.z -= 220.0f;
	m_Camera.SetLookAt( vFrom, vLookAt, MathConst::AxisY );
}

VOID PlayerViewWindow::Update()
{
	if( !m_bInvisible )
	{
		// 处理旋转按钮
		float fRotateAngle = g_fRotateSpeed * Kernel::Inst()->GetDeltaTime();
		if( P_VALID( m_pBnRotaLeft ) && 1 == m_pBnRotaLeft->GetState() )
			RotateYaw( fRotateAngle );
		if( P_VALID( m_pBnRotaRight ) && 1 == m_pBnRotaRight->GetState() )
			RotateYaw( -fRotateAngle );
	}
	GUIStatic::Update();
}

VOID PlayerViewWindow::Render3D()
{
	if( m_bInvisible )
		return GUIStatic::Render();

	if( !P_VALID( m_pAvatarNode ) )
		return;

	IDevice* pDev = Device();
	IRenderTexture* pRT = (IRenderTexture*)m_pNewCanvas->dwHandle;
	float fTimeDelta = Kernel::Inst()->GetDeltaTime();

	m_pNewCanvas->bRefresh = TRUE;
	m_bRefresh = TRUE;

	pRT->Begin();
	pDev->Clear( true, true, false, 0xff000000, 1.0f, 0 );
	StateBlock block;
	block.SaveState(ERS_ColorWriteEnable);
	pDev->SetRenderState(ERS_ColorWriteEnable, D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_RED);

	// 关闭后期处理
	const bool bEnablePostProcess = Kernel::Inst()->GetRenderSys()->IsEnablePostProcess();
	Kernel::Inst()->GetRenderSys()->EnablePostProcess( false );
	m_pSG->Render( fTimeDelta );
	Kernel::Inst()->GetRenderSys()->EnablePostProcess( bEnablePostProcess );

	block.Apply();
	pRT->End();
}

VOID PlayerViewWindow::Render()
{
	if( m_bInvisible )
		return GUIStatic::Render();
	
	if( !P_VALID( m_pAvatarNode ) )
		return;

	// 渲染文字及子控件
	m_pRender->PushRenderTarget(m_pNewCanvas);
	m_pNewCanvas->rc = m_rcText + m_ptView;
	m_pRender->Text(m_pNewCanvas, m_strText, m_pFont, m_dwTextColor, m_dwShadeColor, m_eTextAlign, m_bTextWriteAlpha);
	GUIWnd::Render();
	m_pRender->PopRenderTarget(m_pNewCanvas);

	// 渲染到父窗口
	m_pCanvas->rc.SetLTWH(m_ptView, m_ptSize);
	m_pNewCanvas->rc.SetLTWH(0, 0, m_ptSize.x, m_ptSize.y);
	m_pRender->Draw(m_pCanvas, m_pNewCanvas, 0xFFFFFFFF, m_eAlphaWrite);
}

VOID PlayerViewWindow::RefreshViewCoord()
{
	GUIStatic::RefreshViewCoord();
	m_pNewCanvas->ptOffset.x = m_ptView.x;
	m_pNewCanvas->ptOffset.y = m_ptView.y;
}

//-----------------------------------------------------------------------------
// 添加子窗口
//-----------------------------------------------------------------------------
VOID PlayerViewWindow::AddChild(GUIWnd* pChild)
{
	GUIStatic::AddChild(pChild);

	pChild->SetCanvas(m_pNewCanvas);
	m_pNewCanvas->bRefresh = TRUE;

	// 保存旋转按钮指针
	if( pChild->GetClassName() == _T("CGUIButton") )
	{
		if( pChild->GetName() == _T("roletl") )
			m_pBnRotaLeft = (GUIButton*)pChild;
		else if( pChild->GetName() == _T("roletr") )
			m_pBnRotaRight = (GUIButton*)pChild;
	}
}

//-----------------------------------------------------------------------------
// 设置窗口是否隐藏
//-----------------------------------------------------------------------------
VOID PlayerViewWindow::SetInvisible(BOOL b)
{
	if( m_bInvisible == b )	// 防止重复创建删除
		return;

	GUIStatic::SetInvisible(b);

	if( b )
	{
		SAFE_DEL( m_pAvatarNode );                                       // 删除AvatarNode
		m_pRender->ReleaseRenderTarget(m_pNewCanvas->dwHandle);          // 删除画布,释放render target
		m_pNewCanvas->dwHandle = GT_INVALID;
		
	}
	else
	{
		tagRect rc;
		rc = m_ptSize;
		m_pNewCanvas->dwHandle = m_pRender->CreateRenderTarget(rc);      // 创建render target
		if( !GT_VALID(m_pNewCanvas->dwHandle) )
		{
			// 创建RenderTarget失败
			GUIStatic::SetInvisible( TRUE );
		}
		if( !P_VALID( m_pAvatarNode ) )                                   // 创建AvatarNode
			m_pAvatarNode = new AvatarEquipNode;
	}

}

VOID PlayerViewWindow::RotateYaw( const float fAngle )
{
	if( !P_VALID( m_pAvatarNode ) )
		return;

	m_fAvatarYaw += fAngle;

	Transform tran;
	tran.LoadIdentity();
	tran.Rotate( m_fAvatarYaw, 0.0f, 0.0f );
	m_pAvatarNode->SetWorldMat( tran.GetMatrix() );
}

VOID PlayerViewWindow::UpdataAvatar( Player* pPlayer )
{
	ASSERT( P_VALID( m_pAvatarNode ) );

	if( !P_VALID( pPlayer ) || !P_VALID( pPlayer->GetSceneNode() ) )
		return;

	m_pSG->DetachDynamicNode( m_pAvatarNode );

	const tagAvatarAtt& att = pPlayer->GetAvatarAtt();							// 获取外观属性
	tagAvatarEquip ep;
	for( int i = EAE_Start; i <= EAE_End; i++ )									// 获取所有装备
	{
		ep.AvatarEquip[i] = pPlayer->GetAvatarEquip( (EAvatarElement)i );
	}

	if( NULL == m_pAvatarNode->GetResource() )									// 没有加载骨骼，全部加载
	{
		m_pAvatarNode->SetDisplaySet( pPlayer->GetDisplaySet() );
		m_pAvatarNode->LoadAvatar( att, ep );
		if( EITE_Blade == m_pAvatarNode->GetRWeaponType() )
			m_pAvatarNode->GetSkeletonAniCtrl()->OpenTrack( _T("std2"), true );
		else
			m_pAvatarNode->GetSkeletonAniCtrl()->OpenTrack( _T("std1"), true );
	}
	else if( m_pAvatarNode->GetAtt().bySex != att.bySex )	                     //	性别变了,重新加载骨骼			
	{
		SAFE_DEL(m_pAvatarNode);
		m_pAvatarNode = new AvatarEquipNode;
		m_pAvatarNode->SetDisplaySet( pPlayer->GetDisplaySet() );
		m_pAvatarNode->LoadAvatar( att, ep );
		if( EITE_Blade == m_pAvatarNode->GetRWeaponType() )
			m_pAvatarNode->GetSkeletonAniCtrl()->OpenTrack( _T("std2"), true );
		else
			m_pAvatarNode->GetSkeletonAniCtrl()->OpenTrack( _T("std1"), true );
	}
	else                                                                         // 更新角色可视属性及装备
	{
		m_pAvatarNode->SetDisplaySet( pPlayer->GetDisplaySet() );
		m_pAvatarNode->UpdateAllAtt( att );
		m_pAvatarNode->UpdateAllEquip( ep );
	}
	m_pSG->AttachDynamicNode( m_pAvatarNode );
	//UpdataCamera();
}

void PlayerViewWindow::SetEquip( const EAvatarElement eAvaEnt, const tagEquipDisplay& ed ) 
{ 
	ASSERT(m_pAvatarNode!=NULL);
	m_pAvatarNode->SetEquip( eAvaEnt, ed ); 
}

VOID PlayerViewWindow::UpdataDisplaySet( const tagDisplaySet& ds )
{
	if( NULL == m_pAvatarNode )
		return;
	m_pAvatarNode->SetDisplaySet( ds );
}



