#include "StdAfx.h"
#include "ModelSnapshoter.h"

const UINT R_SIZE_W = 128;  // 底片大小
const UINT R_SIZE_H = 128;

const UINT S_SIZE_W = 64;   // 默认照片大小
const UINT S_SIZE_H = 64;

const TCHAR strAlphaMapFileName[] = _T("data\\ui\\main\\l_player-face.bmp");

const bool g_bUseAlphaMapSize = false;  // 是否使用AlphaMap大小作为照片大小

ModelSnapshoter::ModelSnapshoter(void) : 
m_pRenderTex( NULL ),
m_pSaveTex( NULL ),
m_pAlphaMap( NULL ),
m_pSG( NULL ),
m_bInited( false )
{
}

ModelSnapshoter::~ModelSnapshoter(void)
{
	if( m_bInited )
		Destroy();
}

void ModelSnapshoter::Init()
{
	if( m_bInited )
		return;

	if( NULL == m_pAlphaMap )
		m_pAlphaMap = IDraw2D::Inst()->NewTexture( strAlphaMapFileName );

	if( NULL == m_pRenderTex )
	{
		m_pRenderTex = Device()->NewRenderTexture();
		m_pRenderTex->Create( R_SIZE_W, R_SIZE_H, EPF_A8R8G8B8, true );
	}

	if( NULL == m_pSaveTex )
	{
		m_pSaveTex = Device()->NewRenderTexture();
		UINT nSizeW = S_SIZE_W;
		UINT nSizeH = S_SIZE_H;
		if( g_bUseAlphaMapSize && NULL != m_pAlphaMap )
		{
			nSizeW = m_pAlphaMap->GetDeviceTex()->Width();
			nSizeH = m_pAlphaMap->GetDeviceTex()->Height();
		}
		m_pSaveTex->Create( nSizeW, nSizeH );
	}

	if( NULL == m_pSG )
		m_pSG = new NullSceneGraph;

	if( NULL == m_pRenderTex || NULL == m_pSaveTex )
	{
		Destroy();
		return;
	}
	m_bInited = true;
}

void ModelSnapshoter::Destroy()
{
	SAFE_DELETE( m_pSaveTex );
	SAFE_DELETE( m_pAlphaMap );
	SAFE_DELETE( m_pRenderTex );
	SAFE_DELETE( m_pSG );
	m_bInited = false;
}

void ModelSnapshoter::BltToRenderTarget( IRenderTexture* pDestTex, IRenderTexture* pSrcTex )
{
	ASSERT( NULL != pDestTex && NULL != pSrcTex );

	RECT rcDest, rcSrc;
	rcDest.left = rcDest.top = 0;
	rcDest.right = rcDest.left + pDestTex->Width();
	rcDest.bottom = rcDest.top + pDestTex->Height();

	rcSrc.left = rcSrc.top = 0;
	rcSrc.right = pSrcTex->Width();
	rcSrc.bottom = pSrcTex->Height();

	StateBlock block;
	block.SaveState( ERS_AlphaBlendEnable );  Device()->SetRenderState( ERS_AlphaBlendEnable, false );

	pDestTex->Begin();
	Device()->Clear();
	IDraw2D::Inst()->Draw( &rcDest, &rcSrc, pSrcTex, 0xffffffff, ETFilter_ANISOTROPIC );
	pDestTex->End();

	block.Apply();
}

void ModelSnapshoter::BltToBackBuffer( int x, int y, IRenderTexture* pTex, ResTexture* pAlphaMap /*= NULL*/ )
{
	ASSERT( NULL != pTex );

	RECT rcDest, rcSrc1;
	rcDest.left = rcDest.top = 0;
	rcDest.right = rcDest.left + pTex->Width();
	rcDest.bottom = rcDest.top + pTex->Height();

	rcSrc1.left = rcSrc1.top = 0;
	rcSrc1.right = pTex->Width();
	rcSrc1.bottom = pTex->Height();

	if( NULL != pAlphaMap )
	{
		RECT rcSrc2;
		rcSrc2.left = rcSrc2.top = 0;
		rcSrc2.right = pAlphaMap->GetDeviceTex()->Width();
		rcSrc2.bottom = pAlphaMap->GetDeviceTex()->Height();
		IDraw2D::Inst()->Draw( &rcDest, &rcSrc1, &rcSrc2, pTex, pAlphaMap, ETFilter_LINEAR );
	}
	else
		IDraw2D::Inst()->Draw( &rcDest, &rcSrc1, pTex, 0xffffffff, ETFilter_LINEAR );
}

void ModelSnapshoter::Render( SceneGraph* pSG, SceneNode* pNode )
{
	if( NULL == pSG || NULL == pNode )
		return;

	if( !m_bInited ) Init();

	// 渲染角色到m_pRenderTex上
	IDevice* pDev = Device();
	RenderSys* pRenderSys = Kernel::Inst()->GetRenderSys();

	CameraBase camHead = *pSG->GetCamera();
	camHead.SetPerspective( (float)m_pRenderTex->Width(), (float)m_pRenderTex->Height(), camHead.GetFOV(), camHead.GetZNear(), camHead.GetZFar() );
	m_pSG->BindCamera( &camHead );

	m_pSG->SetSunLight( pSG->GetSunLight() );

	StateBlock block;
	block.SaveState(ERS_DepthTestEnable);  pDev->SetRenderState( ERS_DepthTestEnable, true );
	block.SaveState(ERS_DepthWriteEnable); pDev->SetRenderState( ERS_DepthWriteEnable, true );

	pRenderSys->RenderSceneNode( &camHead, pNode, m_pRenderTex, true, 1, 0x005A1708, NULL, m_pSG );

	block.Apply();

	IDraw2D::Inst()->BeginDraw( IDraw2D::EBlend_Alpha );

	BltToRenderTarget( m_pSaveTex, m_pRenderTex );

	BltToBackBuffer( 10, 10, m_pSaveTex, m_pAlphaMap );

	IDraw2D::Inst()->EndDraw();
}

bool ModelSnapshoter::SaveToFile( const TCHAR* szFilename )
{
	ASSERT( NULL != szFilename );
	if( !m_bInited )
		return false;

	return m_pSaveTex->WriteToFile( szFilename );
}