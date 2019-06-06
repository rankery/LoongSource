#include "StdAfx.h"
#include ".\rskybox.h"
#include "RStaticMesh.h"
#include "..\Device\IDevice.h"
#include "..\Kernel.h"
#include "..\SceneGraph\SceneGraph.h"
#include "..\SceneGraph\SceneNode.h"
#include "..\Draw2D\IDraw2D.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(RSkyBox,RenderNode);

	class RSkyBox::Member
	{
	public:
		RStaticMesh	m_mesh[3];
		bool        m_bRenderShade;		// 是否渲染天空遮罩
		DWORD       m_dwShadeColor;		// 天空遮罩颜色
		DWORD       m_dwShadeEndColor;	// 天空遮罩结束颜色
		Member() : m_bRenderShade(false),m_dwShadeColor(0xffffffff),m_dwShadeEndColor(0x00ffffff){}
	};

	RSkyBox::RSkyBox(void)
	{
		m_p=new Member;
	}

	RSkyBox::~RSkyBox(void)
	{
		delete m_p;
	}

	EMtlTrait RSkyBox::GetMtlTrait(SceneNode *pSGNode) const
	{
		ASSERT( 0 && _T("not support") );
		return EMT_Unknow;
	}

	DWORD RSkyBox::GetMtlTrait1(SceneNode *pSGNode) const
	{
		ASSERT( 0 && _T("not support") );
		return 0;
	}

	DWORD RSkyBox::GetMtlTrait2(SceneNode *pSGNode) const
	{
		ASSERT( 0 && _T("not support") );
		return 0;
	}

	int RSkyBox::GetMtlID() const
	{
		ASSERT( 0 && _T("not support") );
		return 0;
	}

	const AABBox& RSkyBox::GetAABBox() const
	{
		return MathConst::BigBox;
	}

	float CalcScreenY(CameraBase& camera, float dist)
	{
		Matrix4 matViewPrj;
		D3DXMatrixMultiply(&matViewPrj,camera.GetView(),camera.GetPrj());

		Vector3 dir = camera.GetDirection();
		dir.y = 0.0f;

		Vector3 lookat = camera.GetPos() + dir*dist;

		Vector4 tmp;
		D3DXVec3Transform(&tmp,&lookat,&matViewPrj);
		float screenY = tmp.y / tmp.w;
		screenY = screenY*0.5f + 0.5f;
		screenY = 1.0f - screenY;

		return screenY;
	}

	void RSkyBox::Draw(RenderSys *pSys,SceneNode *pSGNode,int nLod,RenderItem* pItem,SceneGraph* pSG)
	{
		IDevice *pDev=Device();

		StateBlock state;
		state.SaveState(ERS_FFogEnable);
		state.SaveState(ERS_FLightEnable);
		state.SaveState(ERS_CullMode);
		state.SaveState(ERS_DepthTestEnable);
		state.SaveState(ERS_DepthWriteEnable);

		pDev->FEnableFog(false);
		pDev->FEnableLight(false);
		pDev->SetCullMode(ECull_None);
		pDev->EnableDepthTest(false);
		pDev->EnableDepthWrite(false);

		pDev->MatPush(EMS_World);
		pDev->MatLoad(EMS_World,&pSGNode->GetWorldMat());
		pDev->FFlushMatrix(true,false,false,false);

		for( int i = 0; i < 3; i++ )
			m_p->m_mesh[i].Draw(pSys,pSGNode);

		pDev->MatPop(EMS_World);
		pDev->FFlushMatrix(true,false,false,false);

		state.Apply();

		if( m_p->m_bRenderShade )
		{
			// 天空遮罩
			IDraw2D::Inst()->BeginDraw(IDraw2D::EBlend_Alpha);

			//--set pixel stage
			pDev->FSetTextureColorOp(0,ETOP_SelectArg1,ETA_Diffuse,ETA_Diffuse);
			pDev->FSetTextureAlphaOp(0,ETOP_SelectArg1,ETA_Diffuse,ETA_Diffuse);

			pDev->FSetTextureColorOp(1,ETOP_Disable);
			pDev->FSetTextureAlphaOp(1,ETOP_Disable);

			CameraBase* pCamera = pSG->GetCamera();
			RECT rc;
			rc.left = 0;
			rc.right = pDev->GetClientWidth();
			rc.bottom = (LONG)(CalcScreenY(*pCamera, 100*50) * pDev->GetClientHeight())+100;
			rc.top = rc.bottom - 550;

			//--make vert
			Vert_PtDT vert[4];

			vert[0].pos.x = (FLOAT)rc.left;
			vert[0].pos.y = (FLOAT)rc.top;
			vert[1].pos.x = (FLOAT)rc.right;
			vert[2].pos.y = (FLOAT)rc.bottom;
			vert[3].pos.x = vert[0].pos.x;
			vert[1].pos.y = vert[0].pos.y;
			vert[2].pos.x = vert[1].pos.x;
			vert[3].pos.y = vert[2].pos.y;

			vert[0].uv.x = 0;
			vert[0].uv.y = 0;
			vert[1].uv.x = 0;
			vert[2].uv.y = 0;
			vert[3].uv.x = vert[0].uv.x;
			vert[1].uv.y = vert[0].uv.y;
			vert[2].uv.x = vert[1].uv.x;
			vert[3].uv.y = vert[2].uv.y;

			vert[0].diffuse = m_p->m_dwShadeEndColor;
			vert[0].pos.z = 1;
			vert[0].pos.w = 1;
			vert[1].diffuse = m_p->m_dwShadeEndColor;
			vert[1].pos.z = 1;
			vert[1].pos.w = 1;
			vert[2].diffuse = m_p->m_dwShadeColor;
			vert[2].pos.z = 1;
			vert[2].pos.w = 1;
			vert[3].diffuse = m_p->m_dwShadeColor;
			vert[3].pos.z = 1;
			vert[3].pos.w = 1;

			pDev->DrawMem(EDraw_TriangleFan, 2, &vert, sizeof(Vert_PtDT));

			IDraw2D::Inst()->EndDraw();
		}
	}

	void RSkyBox::CreateMeshs( const Vector3& drawSize )
	{
		VertStream *pVStream=NULL;
		Vert_PT *pVert=NULL;
		Vector3 halfSize=drawSize*0.5f;
		//--建立顶部mesh
		pVStream=new VertStream();
		pVStream->Create(EVType_PT,4);
		pVert=(Vert_PT *)pVStream->GetRawBuffer();
		//--top
		pVert[0].pos=Vector3(-halfSize.x,halfSize.y,halfSize.z);
		pVert[1].pos=Vector3(halfSize.x,halfSize.y,halfSize.z);
		pVert[2].pos=Vector3(halfSize.x,halfSize.y,-halfSize.z);
		pVert[3].pos=Vector3(-halfSize.x,halfSize.y,-halfSize.z);
		pVert[0].uv=Vector2(0.0f,1);
		pVert[1].uv=Vector2(1.0f,1);
		pVert[2].uv=Vector2(1,0);
		pVert[3].uv=Vector2(0,0);

		m_p->m_mesh[0].Create(0,EDraw_TriangleFan,pVStream);
		SAFE_DELETE(pVStream);

		float maxy=halfSize.y;
		float miny=-120;
		Vert_PT tmpVert[8];
		//--建立前右的Mesh,顺时针产生QUAD
		pVStream=new VertStream();
		pVStream->Create(EVType_PT,2*2*3);
		pVert=(Vert_PT *)pVStream->GetRawBuffer();

		//--front(pos z)
		tmpVert[0].pos=Vector3(-halfSize.x,maxy,halfSize.z);
		tmpVert[1].pos=Vector3(halfSize.x,maxy,halfSize.z);
		tmpVert[2].pos=Vector3(halfSize.x,miny,halfSize.z);
		tmpVert[3].pos=Vector3(-halfSize.x,miny,halfSize.z);
		tmpVert[0].uv=Vector2(0.0f,0.0f);
		tmpVert[1].uv=Vector2(1.0f,0.0f);
		tmpVert[2].uv=Vector2(1.0f,1.0f/2);
		tmpVert[3].uv=Vector2(0.0f,1.0f/2);

		//--right(pos x)
		tmpVert[4].pos=Vector3(halfSize.x,maxy,halfSize.z);
		tmpVert[5].pos=Vector3(halfSize.x,maxy,-halfSize.z);
		tmpVert[6].pos=Vector3(halfSize.x,miny,-halfSize.z);
		tmpVert[7].pos=Vector3(halfSize.x,miny,halfSize.z);
		tmpVert[4].uv=Vector2(0.0f,1.0f);
		tmpVert[5].uv=Vector2(1.0f,1.0f);
		tmpVert[6].uv=Vector2(1.0f,1.0f/2);
		tmpVert[7].uv=Vector2(0.0f,1.0f/2);

		int vi=0;
		for(int i=0;i<8;i+=4)
		{
			pVert[vi]=tmpVert[i];	vi++;
			pVert[vi]=tmpVert[i+1];	vi++;
			pVert[vi]=tmpVert[i+2];	vi++;

			pVert[vi]=tmpVert[i+2];	vi++;
			pVert[vi]=tmpVert[i+3];	vi++;
			pVert[vi]=tmpVert[i];	vi++;
		}
		m_p->m_mesh[1].Create(1,EDraw_TriangleList,pVStream);
		SAFE_DELETE(pVStream);

		//建立后左的Mesh，顺时针产生QUAD
		pVStream=new VertStream();
		pVStream->Create(EVType_PT,2*2*3);
		pVert=(Vert_PT *)pVStream->GetRawBuffer();

		//--back(neg z)
		tmpVert[0].pos=Vector3(halfSize.x,maxy,-halfSize.z);
		tmpVert[1].pos=Vector3(-halfSize.x,maxy,-halfSize.z);
		tmpVert[2].pos=Vector3(-halfSize.x,miny,-halfSize.z);
		tmpVert[3].pos=Vector3(halfSize.x,miny,-halfSize.z);
		tmpVert[0].uv=Vector2(0.0f,0.0f);
		tmpVert[1].uv=Vector2(1.0f,0.0f);
		tmpVert[2].uv=Vector2(1.0f,1.0f/2);
		tmpVert[3].uv=Vector2(0.0f,1.0f/2);

		//--left(neg x)
		tmpVert[4].pos=Vector3(-halfSize.x,maxy,-halfSize.z);
		tmpVert[5].pos=Vector3(-halfSize.x,maxy,halfSize.z);
		tmpVert[6].pos=Vector3(-halfSize.x,miny,halfSize.z);
		tmpVert[7].pos=Vector3(-halfSize.x,miny,-halfSize.z);
		tmpVert[4].uv=Vector2(0.0f,1.0f);
		tmpVert[5].uv=Vector2(1.0f,1.0f);
		tmpVert[6].uv=Vector2(1.0f,1.0f/2);
		tmpVert[7].uv=Vector2(0.0f,1.0f/2);

		vi=0;
		for(int i=0;i<8;i+=4)
		{
			pVert[vi]=tmpVert[i];	vi++;
			pVert[vi]=tmpVert[i+1];	vi++;
			pVert[vi]=tmpVert[i+2];	vi++;

			pVert[vi]=tmpVert[i+2];	vi++;
			pVert[vi]=tmpVert[i+3];	vi++;
			pVert[vi]=tmpVert[i];	vi++;
		}
		m_p->m_mesh[2].Create(2,EDraw_TriangleList,pVStream);
		SAFE_DELETE(pVStream);
	}

	void RSkyBox::EnableRenderShade( const bool bEnable )
	{
		m_p->m_bRenderShade = bEnable;
	}

	void RSkyBox::SetShadeColor( const DWORD dwColor )
	{
		m_p->m_dwShadeColor = dwColor;
		m_p->m_dwShadeEndColor = ( dwColor & 0x00ffffff );
	}

}//namespace Cool3D