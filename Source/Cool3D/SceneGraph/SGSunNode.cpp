#include "StdAfx.h"
#include ".\sgsunnode.h"
#include "..\RenderSys\DynamicMesh.h"
#include "..\RenderMtl\MBitmap.h"
#include "..\Math\Camerabase.h"
#include "..\Math\Frustum.h"
#include "SceneGraph.h"
#include "..\Device\IDeviceTex.h"
#include "..\RenderMtl\MMFrameBlend.h"
#include "..\RenderMtl\MMDepthFrame.h"
#include "..\RenderMtl\MtlCombiner.h"
#include "..\Kernel.h"
#include "..\RenderMtl\MMAlphaMask.h"
#include ".\SGSunFlare.h"
#include "..\Util\Filename.h"

namespace Cool3D
{
	IMP_RTTI(SGSunNode,SceneNode);

	class SGSunNode::Member
	{
	public:
		DynamicMesh			m_mesh;
		MBitmap				m_mtl;
		Vector3				m_vPos;				// 世界坐标（会跟随摄像机移动）
		POINT				m_ptPos;            // 屏幕坐标
		POINT				m_ptSize;           // 屏幕空间的大小
		const float			m_fDistToCam;		// 到照相机的距离
		bool                m_bInFrustum;
		Member() :
		m_vPos( 0.0f, 0.0f, 0.0f ),
		m_fDistToCam( 5000.0f ),
		m_bInFrustum( false )
		{
			m_ptPos.x = 0;
			m_ptPos.y = 0;
			m_ptSize.x = 0;
			m_ptSize.y = 0;
		}

		~Member()	
		{
		}
		bool IsInScreen( const POINT& ptCenter, const POINT& ptHalfSize )
		{
			return ( ptCenter.x + ptHalfSize.x >= 0 &&
					 ptCenter.y + ptHalfSize.y >= 0 &&
				     ptCenter.x - ptHalfSize.x <= (LONG)Device()->GetClientWidth() &&
				     ptCenter.y - ptHalfSize.y <= (LONG)Device()->GetClientHeight() );
		}
	};

	SGSunNode::SGSunNode(void) : m_bEnableFlare( false )
	{
		m_p=new Member;
	}

	SGSunNode::~SGSunNode(void)
	{
		delete m_p;
	}

	bool SGSunNode::IsInFrustum(const Frustum *pFrumstum) const
	{
		ASSERT( 0 && _T("not support") );
		return true;
	}

	void SGSunNode::LoadRes(const TCHAR* szName)
	{
	}

	UINT SGSunNode::GetNumRenderNode() const
	{
		return 1;
	}

	RenderNode * SGSunNode::GetRenderNode(UINT i) const
	{
		ASSERT( 0 == i );
		return &m_p->m_mesh;
	}

	const Matrix4& SGSunNode::GetWorldMat() const
	{
		return MathConst::IdentityMat;
	}

	void SGSunNode::SetWorldMat(const Matrix4 *pMat)
	{
		ASSERT( 0 && _T("not support") );
	}

	void SGSunNode::GetBox(AABBox& out) const
	{
		ASSERT( 0 && _T("not support") );
	}

	IMaterial* SGSunNode::GetMtl(int mtlID)
	{
		ASSERT( 0 == mtlID );
		return &m_p->m_mtl;
	}

	void SGSunNode::Update(float deltaTime,SceneGraph *pSG)
	{
		m_p->m_bInFrustum = false;

		if( NULL == pSG )
			return;

		CameraBase* pCam = pSG->GetCamera();
		if( NULL == pCam )
			return;

		if( NULL == m_p->m_mtl.m_pBitmap || NULL == m_p->m_mtl.m_pBitmap->GetDeviceTex() )
			return;

		// 在屏幕空间画太阳
		const tagLight& sunLight = pSG->GetSunLight();
		m_p->m_vPos = pCam->GetPos();
		m_p->m_vPos -= sunLight.dir * m_p->m_fDistToCam;
		const Vector3 vPosInCamSpace = m_p->m_vPos * (*pCam->GetView());
		if( vPosInCamSpace.z < 0.0f )
		{
			m_p->m_mesh.SetDrawLimit( 0 );
			return;
		}
		m_p->m_ptPos = pCam->WorldToScreen( m_p->m_vPos );

		m_p->m_ptSize.x = m_p->m_mtl.m_pBitmap->GetDeviceTex()->Width();
		m_p->m_ptSize.y = m_p->m_mtl.m_pBitmap->GetDeviceTex()->Height();
		POINT ptHalfSize;
		ptHalfSize.x = m_p->m_ptSize.x / 2;
		ptHalfSize.y = m_p->m_ptSize.y / 2;
		if( !m_p->IsInScreen( m_p->m_ptPos, ptHalfSize ) )
		{
			m_p->m_mesh.SetDrawLimit( 0 );
			return;
		}

		VertStream *pVStream = m_p->m_mesh.GetVertStream();
		ASSERT( pVStream );
		Vert_PtT *pVert=(Vert_PtT *)pVStream->GetRawBuffer();
		ASSERT( pVert );

		pVert[0].pos.x = (float)m_p->m_ptPos.x - ptHalfSize.x;
		pVert[0].pos.y = (float)m_p->m_ptPos.y - ptHalfSize.y;
		pVert[1].pos.x = (float)m_p->m_ptPos.x - ptHalfSize.x;
		pVert[1].pos.y = (float)m_p->m_ptPos.y + ptHalfSize.y;
		pVert[2].pos.x = (float)m_p->m_ptPos.x + ptHalfSize.x;
		pVert[2].pos.y = (float)m_p->m_ptPos.y - ptHalfSize.y;
		pVert[3].pos.x = (float)m_p->m_ptPos.x + ptHalfSize.x;
		pVert[3].pos.y = (float)m_p->m_ptPos.y + ptHalfSize.y;
		for( int i = 0; i < 4; i++ )
		{
			pVert[i].pos.z = 1.0f;
			pVert[i].pos.w = 1.0f;
		}
		m_p->m_mesh.SetDrawLimit( 2 );
		m_p->m_bInFrustum = true;

		SceneNode::Update(deltaTime,pSG);
	}

	void SGSunNode::ForceUpdate(float deltaTime,SceneGraph *pSG)
	{

	}

	bool SGSunNode::Create( const TCHAR* szTexBasePath )
	{
		// 检查纹理是否存在
		TCHAR szTex[MAX_PATH];
		_stprintf( szTex, _T("%ssun.%s"), szTexBasePath, _T("dds") );
		IFS* pIFS = ResMgr::Inst()->SafeGetFS( ResMgr::EFST_System );
		DWORD dwFileSize = pIFS->GetSize( szTex );
		if( 0 == dwFileSize || -1 == dwFileSize )
		{
			_stprintf( szTex, _T("%ssun.tga"), szTexBasePath );
			dwFileSize = pIFS->GetSize( szTex );
			if( 0 == dwFileSize || -1 == dwFileSize )
				return false;
		}

		// mtl
		MMFrameBlend *pBlend=new MMFrameBlend;
		pBlend->m_blendEnable=true;
		pBlend->m_src=EBlend_SrcAlpha;
		pBlend->m_dst=EBlend_One;

		MMDepthFrame *pDepth=new MMDepthFrame;
		pDepth->m_depthTestEnable=true;
		pDepth->m_depthWriteEnable=true;

		m_p->m_mtl.SetTexture(0,szTex,1);
		m_p->m_mtl.m_bEnableLight=false;
		m_p->m_mtl.m_bTwoSide=false;
		m_p->m_mtl.AttachModifier(pBlend);
		m_p->m_mtl.AttachModifier(pDepth);

		// mesh
		m_p->m_mesh.Create(0,EDraw_TriangleStrip,EVType_PtT,4);
		VertStream *pVStream=m_p->m_mesh.GetVertStream();
		Vert_PtT *pVert=(Vert_PtT *)pVStream->GetRawBuffer();
		pVert[0].uv=Vector2(0.0f,0.0f);
		pVert[1].uv=Vector2(0.0f,1.0f);
		pVert[2].uv=Vector2(1.0f,0.0f);
		pVert[3].uv=Vector2(1.0f,1.0f);
		m_p->m_mesh.SetDrawLimit( 0 );
		m_p->m_mesh.SetIdentityWorldMat( true );

		// flare
		if( SGSunFlare::IsHWSupport() )
		{
			SGSunFlare* pSunFlare = new SGSunFlare;
			tstring strFlareTexBaseName = szTexBasePath;
			strFlareTexBaseName += _T("sun");
			if( pSunFlare->Create( strFlareTexBaseName.c_str(), _T("tga") ) )
				this->AttachChild( pSunFlare );
			else
				SAFE_DELETE( pSunFlare );
		}

		return true;
	}

	Vector3 SGSunNode::GetPos() const
	{
		return m_p->m_vPos;
	}

	POINT SGSunNode::GetScreenPos() const
	{
		return m_p->m_ptPos;
	}

	bool SGSunNode::IsInFrustum() const
	{
		return m_p->m_bInFrustum;
	}

	float SGSunNode::GetNumPixels() const
	{
		return (float)m_p->m_ptSize.x * m_p->m_ptSize.y;
	}
}//namespace Cool3D