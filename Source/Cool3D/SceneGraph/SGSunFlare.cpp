#include "StdAfx.h"
#include ".\SGSunFlare.h"
#include "..\RenderMtl\MBitmap.h"
#include "..\Math\Camerabase.h"
#include "..\Math\Frustum.h"
#include "SceneGraph.h"
#include "..\RenderMtl\MMFrameBlend.h"
#include "..\RenderMtl\MMDepthFrame.h"
#include "..\RenderMtl\MtlCombiner.h"
#include "SGSunNode.h"
#include "..\RenderSys\DynamicMesh.h"
#include "..\Device\IDevice.h"
#include "..\Device\DX9Device\DX9Device.h"
#include "..\RenderSys\RenderSys.h"
#include "..\RenderSys\RenderSysFixed.h"
#include "..\ResSys\ResMgr.h"
#include "..\Device\IDeviceTex.h"
#include "..\vfs\IFS.h"
#include "..\RenderMtl\MMAlphaMask.h"
#include "..\Device\IDeviceVizQuery.h"
#include "..\Device\DrawX.h"

namespace Cool3D
{
	IMP_RTTI(SGSunFlare,SceneNode);

	const int MAX_FLARE_NUM = 12;
	const float g_fDistFactor[MAX_FLARE_NUM] = { 0.0f, 0.4f, 0.5f, 0.7f, 0.9f, 1.1f, 1.2f, 1.25f, 1.3f, 1.45f, 1.7f, 1.9f };

	class SGSunFlare::Member
	{
	public:
		vector<DynamicMesh*>	m_vecMesh;
		vector<MBitmap*>		m_vecMtl;
		IDeviceVizQuery*		m_pVizQuery;
		float					m_fSunVisible;			// 太阳的可见度
		float                   m_fCurVisible;
		Member() :
		m_pVizQuery( NULL ),
		m_fSunVisible( 1.0f )
		{
		}

		~Member()	
		{
			SAFE_DELETE(m_pVizQuery);
			for( size_t i = 0; i < m_vecMesh.size(); i++ )
			{
				DynamicMesh* pMesh = m_vecMesh[i];
				SAFE_DELETE( pMesh );
			}
			m_vecMesh.clear();
			for( size_t i = 0; i < m_vecMtl.size(); i++ )
			{
				MBitmap* pMtl = m_vecMtl[i];
				SAFE_DELETE( pMtl );
			}
			m_vecMtl.clear();
		}

	       
		//更新mesh
		void UpdateMesh(const int nIndex,const Vector2& vScreenPos)
		{
			ASSERT( nIndex < (int)m_vecMesh.size() );
			ASSERT( nIndex < (int)m_vecMtl.size() );

			DynamicMesh* pMesh = m_vecMesh[nIndex];
			MBitmap* pMtl = m_vecMtl[nIndex];
			if( NULL == pMesh || NULL == pMtl )
				return;
			
			if( NULL == pMtl->m_pBitmap || NULL == pMtl->m_pBitmap->GetDeviceTex() )
			{
				pMesh->SetDrawLimit( 0 );
				return;
			}

			VertStream *pVStream = pMesh->GetVertStream();
			ASSERT( pVStream );
			Vert_PtDT *pVert = (Vert_PtDT *)pVStream->GetRawBuffer();
			ASSERT( pVert );

			Vector2 vHalfSize = Vector2( pMtl->m_pBitmap->GetDeviceTex()->Width() / 2.0f, pMtl->m_pBitmap->GetDeviceTex()->Height() / 2.0f );

			pVert[0].pos.x = (float)vScreenPos.x - vHalfSize.x;
			pVert[0].pos.y = (float)vScreenPos.y - vHalfSize.y;
			pVert[1].pos.x = (float)vScreenPos.x - vHalfSize.x;
			pVert[1].pos.y = (float)vScreenPos.y + vHalfSize.y;
			pVert[2].pos.x = (float)vScreenPos.x + vHalfSize.x;
			pVert[2].pos.y = (float)vScreenPos.y - vHalfSize.y;
			pVert[3].pos.x = (float)vScreenPos.x + vHalfSize.x;
			pVert[3].pos.y = (float)vScreenPos.y + vHalfSize.y;

			Color4f color = Color4f( m_fSunVisible, 1.0f, 1.0f, 1.0f );
			for( int i = 0; i < 4; i++ )
			{
				pVert[i].diffuse=D3DXCOLOR( color.R, color.G, color.B, color.A );
				pVert[i].pos.z = 1.0f;
				pVert[i].pos.w = 1.0f;
			}
			pMesh->SetDrawLimit( 2 );
		}
	};

	SGSunFlare::SGSunFlare(void)
	{
		m_p=new Member;
	}

	SGSunFlare::~SGSunFlare(void)
	{
		delete m_p;
	}

	bool SGSunFlare::IsInFrustum(const Frustum *pFrumstum) const
	{
		ASSERT( 0 && _T("not support") );
		return true;
	}

	void SGSunFlare::LoadRes(const TCHAR* szName)
	{
		ASSERT( 0 && _T("not support") );
	}

	UINT SGSunFlare::GetNumRenderNode() const
	{
		return m_p->m_vecMesh.size();
	}

	RenderNode * SGSunFlare::GetRenderNode(UINT i) const
	{
	    if(i>=0 && i<m_p->m_vecMesh.size())	
		   return m_p->m_vecMesh[i];
		else
		   return NULL;
	}

	const Matrix4& SGSunFlare::GetWorldMat() const
	{
		return MathConst::IdentityMat;
	}

	void SGSunFlare::SetWorldMat(const Matrix4 *pMat)
	{
		ASSERT( 0 && _T("not support") );
	}

	void SGSunFlare::GetBox(AABBox& out) const
	{
		ASSERT( 0 && _T("not support") );
	}

	IMaterial* SGSunFlare::GetMtl(int mtlID)
	{
		if(mtlID>=0 && m_p->m_vecMtl.size())
			return m_p->m_vecMtl[mtlID];
		else
			return NULL;
	}

	void SGSunFlare::Update(float deltaTime,SceneGraph *pSG)
	{
		if(pSG==NULL)
			return;

		SGSunNode* pSunNode = pSG->GetSunNode();
		if( NULL == pSunNode || !pSunNode->IsEnableFlare() )
			return;

		if( m_p->m_vecMtl.size() == 0 )
			return;

		// 遮挡查询
		if( NULL != m_p->m_pVizQuery )
		{
			DWORD dwNumPixelsDrawn = 0;
			if( m_p->m_pVizQuery->GetResultAsync( dwNumPixelsDrawn ) )
			{
				float fPixelsCount = pSunNode->GetNumPixels();
				m_p->m_fSunVisible = dwNumPixelsDrawn / fPixelsCount;
			}
		}

		// 在太阳与屏幕中心的连线上绘制光晕
		const POINT ptSun = pSunNode->GetScreenPos();
		Vector2 vStart = Vector2( (float)ptSun.x, (float)ptSun.y );
		Vector2 vScreenCenter = Vector2( (float)( Device()->GetClientWidth() / 2 ), (float)( Device()->GetClientHeight() / 2 ) );
		Vector2 vDir = vScreenCenter - vStart;
		float fDist = D3DXVec2Length( &vDir );
		D3DXVec2Normalize( &vDir, &vDir );


		// 更新每个光晕片
		for( size_t i = 0; i < m_p->m_vecMesh.size(); i++ )
		{
			const Vector2 vPos = vStart + vDir * fDist * g_fDistFactor[i];
			m_p->UpdateMesh( i, vPos );
		}
	}

	void SGSunFlare::ForceUpdate(float deltaTime,SceneGraph *pSG)
	{
		//todo
	}

	bool SGSunFlare::Create(const TCHAR* szTexBaseName, const TCHAR* szExt)
	{
		if( !IsHWSupport() )
			return false;

		m_p->m_vecMesh.clear();
		m_p->m_vecMtl.clear();

		TCHAR szTexPath[MAX_PATH];
		IFS* pIFS = ResMgr::Inst()->SafeGetFS( ResMgr::EFST_System );
		for( int i = 0; i < MAX_FLARE_NUM; i++ )
		{
			_stprintf( szTexPath, _T("%s_%d.%s"), szTexBaseName, i + 1, _T("dds") );
			DWORD dwFileSize = pIFS->GetSize( szTexPath );
			if( 0 == dwFileSize || -1 == dwFileSize )
			{
				_stprintf( szTexPath, _T("%s_%d.%s"), szTexBaseName, i + 1, szExt );
				dwFileSize = pIFS->GetSize( szTexPath );
				if( 0 == dwFileSize || -1 == dwFileSize )
					break;
			}

			// mtl
			MMFrameBlend *pBlend=new MMFrameBlend;
			pBlend->m_blendEnable=true;
			pBlend->m_src=EBlend_SrcAlpha;
			pBlend->m_dst=EBlend_One;

			MMDepthFrame *pDepth=new MMDepthFrame;
			pDepth->m_depthTestEnable=false;
			pDepth->m_depthWriteEnable=false;

			MBitmap* pMtl = new MBitmap;
			pMtl->SetTexture(0,szTexPath,1);
			pMtl->m_bEnableLight=false;
			pMtl->m_bTwoSide=true;
			pMtl->AttachModifier(pBlend);
			pMtl->AttachModifier(pDepth);	
			m_p->m_vecMtl.push_back( pMtl );

			// mesh
			DynamicMesh* pMesh = new DynamicMesh;
			pMesh->Create( i, EDraw_TriangleStrip, EVType_PtDT, 4 );
			VertStream *pVStream = pMesh->GetVertStream();
			Vert_PtDT *pVert = (Vert_PtDT *)pVStream->GetRawBuffer();
			pVert[0].uv=Vector2(0.0f,0.0f);
			pVert[1].uv=Vector2(0.0f,1.0f);
			pVert[2].uv=Vector2(1.0f,0.0f);
			pVert[3].uv=Vector2(1.0f,1.0f);
			pMesh->SetDrawLimit( 0 );
			m_p->m_vecMesh.push_back( pMesh );
		}

		if( m_p->m_vecMesh.size() == 0 )
			return false;

		// VizQuery
		m_p->m_pVizQuery = Device()->NewVizQuery();
		m_p->m_pVizQuery->Create();

		return true;
	}
	bool SGSunFlare::BeginVizQuery()
	{
		if( NULL == m_p->m_pVizQuery )
			return false;

		return m_p->m_pVizQuery->Begin();
	}

	bool SGSunFlare::EndVizQuery()
	{
		if( NULL == m_p->m_pVizQuery )
			return false;

		return m_p->m_pVizQuery->End();
	}

	float SGSunFlare::GetVisble()
	{
		return m_p->m_fSunVisible;
	}

	bool SGSunFlare::IsHWSupport()
	{
		return Device()->CapsVizQuery() && Device()->CapsColorWriteEnable();
	}
}//namespace Cool3D