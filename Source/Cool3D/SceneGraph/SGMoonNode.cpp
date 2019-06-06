#include "StdAfx.h"
#include ".\sgmoonnode.h"
#include "..\RenderSys\DynamicMesh.h"
#include "..\RenderMtl\MBitmap.h"
#include "..\RenderMtl\MMFrameBlend.h"
#include "..\RenderMtl\MMDepthFrame.h"
#include "..\Math\Camerabase.h"
#include "SceneGraph.h"

namespace F3D
{
	IMP_RTTI(SGMoonNode,SceneNode);

	class SGMoonNode::Member
	{
	public:
		DynamicMesh	m_mesh;//一个月亮面片
		MBitmap		m_mtl[15];//15种纹理

		Vector3     m_MoonPosition;       //月亮位置=月亮与照相机的相对位置 + 照相机位置
		Vector3     m_MoonRotateAxis;     //月亮旋转轴
		Vector3     m_MoonRelationPos;    //月亮与照相机的相对位置
		Vector3		m_MoonDirection;	  //月光照射方向
		Color4f     m_MoonColor;          //月亮颜色
		float       m_fSize;              //月亮尺寸
		Vector3     m_fInitMoonPos;       //初始月亮位置
		SGSunNode*  m_pSunNode;           //太阳结点
		SGSkyDome*  m_pSkyDome;           //天空结点
		int         m_nDate;              //日期

		Member()
		{
			m_nDate=1;
			m_MoonColor=Color4f(1.0f,1.0f,1.0f,1.0f);
			m_MoonDirection=Vector3(1,-0.5f,1);
		}

		~Member()	
		{
		}

		//更新mesh
		void UpdateMesh(CameraBase* pCamera,Vector3 Pos,Color4f Color,float fScale)
		{
			Vector3 CameraLookAt=pCamera->GetLookat()-pCamera->GetPos();
			D3DXVec3Normalize(&CameraLookAt,&CameraLookAt);

			Vector3 horizontal,vertical;

			D3DXVec3Cross(&horizontal,&pCamera->GetUp(),&CameraLookAt);
			D3DXVec3Normalize(&horizontal,&horizontal);
			D3DXVec3Cross(&vertical,&horizontal,&CameraLookAt);
			D3DXVec3Normalize(&vertical,&vertical);

			horizontal *= 1.5f * fScale;
			vertical *= 1.5f * fScale;

			VertStream *pVStream=m_mesh.GetVertStream();
			Vert_PTD *pVert=(Vert_PTD *)pVStream->GetRawBuffer();

			pVert[0].pos=Pos-horizontal-vertical;
			pVert[0].diffuse=D3DXCOLOR(Color.R,Color.G,Color.B,Color.A);

			pVert[1].pos=Pos-horizontal+vertical;
			pVert[1].diffuse=D3DXCOLOR(Color.R,Color.G,Color.B,Color.A);

			pVert[2].pos=Pos+horizontal-vertical;
			pVert[2].diffuse=D3DXCOLOR(Color.R,Color.G,Color.B,Color.A);

			pVert[3].pos=Pos+horizontal+vertical;
			pVert[3].diffuse=D3DXCOLOR(Color.R,Color.G,Color.B,Color.A);
		}

		//设置材质
		void SetMtl(int nIndex,TCHAR* szTexFileName)
		{
			MMFrameBlend *pBlend=new MMFrameBlend;
			pBlend->m_blendEnable=true;
			pBlend->m_src=EBlend_SrcAlpha;
			pBlend->m_dst=EBlend_One;

			m_mtl[nIndex].SetTexture(0,szTexFileName,true,1);
			m_mtl[nIndex].m_bEnableLight=false;
			m_mtl[nIndex].m_bTwoSide=false;
			m_mtl[nIndex].AttachModifier(pBlend);
		}

	};

	SGMoonNode::SGMoonNode(void)
	{
		m_p=new Member;
	}

	SGMoonNode::~SGMoonNode(void)
	{
		delete m_p;
	}

	bool SGMoonNode::IsInFrustum(const Frustum *pFrumstum) const
	{
		return true;
	}

	void SGMoonNode::LoadRes(const TCHAR* szName)
	{
	}

	const Matrix4& SGMoonNode::GetWorldMat() const
	{
		return MathConst::IdentityMat;
	}

	void SGMoonNode::SetWorldMat(const Matrix4 *pMat)
	{
	}

	void SGMoonNode::GetBox(AABBox& out) const
	{
	}

	UINT SGMoonNode::GetNumRenderNode() const
	{
		return 1;
	}

	RenderNode * SGMoonNode::GetRenderNode(UINT i) const
	{
		return &m_p->m_mesh;
	}

	IMaterial* SGMoonNode::GetMtl(int mtlID)
	{
		if(mtlID >= 0 && mtlID < 15)
		    return &m_p->m_mtl[mtlID];
		else
			return NULL;
	}

	void SGMoonNode::Update(float deltaTime,SceneGraph *pSG)
	{
		if(pSG==NULL)
			return;

		//得到照相机信息
		CameraBase *pCamera=pSG->GetCamera();
		//得到月亮位置
		m_p->m_MoonPosition=m_p->m_MoonRelationPos+pCamera->GetPos();

		//画月亮
		float fTexAlpha=0.0f;
		float fSunAngle;
		m_p->m_pSunNode->GetSunPositionInfo(fSunAngle);

		// 03:40-04:00 : 夜 -> 晨
		if( fSunAngle>=3.7*15 && fSunAngle<=4*15 )
		{
			fTexAlpha=1.0f-(fSunAngle-3.7f*15)/(0.3f*15);
		}
		// 04-08: 晨 - > 昏
		else if( fSunAngle>=4*15 && fSunAngle<=22*15 )
		{
			fTexAlpha=0.0f;
		}
		// 22:00-22:20: 昏 -> 夜
		else if( fSunAngle>=22*15 && fSunAngle<=22.3*15 )
		{
			fTexAlpha=(fSunAngle-22.0f*15)/(0.3f*15);
		}
		// 22:20-03:40 夜
		else
		{
			fTexAlpha=1.0f;
		}

		fTexAlpha*=m_p->m_pSkyDome->GetSkyTextureAlpha();

		m_p->m_MoonColor.A=fTexAlpha;

		m_p->UpdateMesh(pCamera,m_p->m_MoonPosition,m_p->m_MoonColor,m_p->m_fSize);
	}

	void SGMoonNode::ForceUpdate(float deltaTime,SceneGraph *pSG)
	{
		//todo?
	}

	//功能
	void SGMoonNode::Create(Vector3 InitMoonPos, Vector3 InitMoonRotateAxis,float fSize,SGSunNode* pSunNode,SGSkyDome* pSkyDome)
	{
		m_p->m_MoonRelationPos=InitMoonPos;
		m_p->m_fInitMoonPos=InitMoonPos;
		m_p->m_MoonRotateAxis=InitMoonRotateAxis;
		m_p->m_fSize=fSize;
		m_p->m_pSunNode=pSunNode;
		m_p->m_pSkyDome=pSkyDome;
		
		//初始化纹理 15
		m_p->SetMtl(0,_T("System/SkyBox/moon/MoonTex_14.tga"));
		m_p->SetMtl(1,_T("System/SkyBox/moon/MoonTex_13.tga"));
		m_p->SetMtl(2,_T("System/SkyBox/moon/MoonTex_12.tga"));
		m_p->SetMtl(3,_T("System/SkyBox/moon/MoonTex_11.tga"));
		m_p->SetMtl(4,_T("System/SkyBox/moon/MoonTex_10.tga"));
		m_p->SetMtl(5,_T("System/SkyBox/moon/MoonTex_9.tga"));
		m_p->SetMtl(6,_T("System/SkyBox/moon/MoonTex_8.tga"));
		m_p->SetMtl(7,_T("System/SkyBox/moon/MoonTex_7.tga"));
		m_p->SetMtl(8,_T("System/SkyBox/moon/MoonTex_6.tga"));
		m_p->SetMtl(9,_T("System/SkyBox/moon/MoonTex_5.tga"));
		m_p->SetMtl(10,_T("System/SkyBox/moon/MoonTex_4.tga"));
		m_p->SetMtl(11,_T("System/SkyBox/moon/MoonTex_3.tga"));
		m_p->SetMtl(12,_T("System/SkyBox/moon/MoonTex_2.tga"));
		m_p->SetMtl(13,_T("System/SkyBox/moon/MoonTex_1.tga"));
		m_p->SetMtl(14,_T("System/SkyBox/moon/MoonTex_15.tga"));
		
		//初始化mesh
		m_p->m_mesh.Create(0,EDraw_TriangleStrip,EVtype_PTD,4);

		VertStream *pVStream=m_p->m_mesh.GetVertStream();
		Vert_PTD *pVert=(Vert_PTD *)pVStream->GetRawBuffer();
		pVert[0].uv=Vector2(0.0f,0.0f);
		pVert[1].uv=Vector2(0.0f,1.0f);
		pVert[2].uv=Vector2(1.0f,0.0f);
		pVert[3].uv=Vector2(1.0f,1.0f);

	}

	//--设置日期信息
	void SGMoonNode::SetDateInfo(int nDate)
	{
		m_p->m_nDate=nDate;
		switch(nDate)
		{
		case 1:	case 2:	case 30:	m_p->m_mesh.SetMtlID(0);	break;
		case 3:	case 4:				m_p->m_mesh.SetMtlID(1);	break;
		case 5:	case 6:				m_p->m_mesh.SetMtlID(2);	break;
		case 7:	case 8:				m_p->m_mesh.SetMtlID(3);	break;
		case 9:	case 10:			m_p->m_mesh.SetMtlID(4);	break;
		case 11: case 12:			m_p->m_mesh.SetMtlID(5);	break;
		case 13: case 14:			m_p->m_mesh.SetMtlID(6);	break;
		case 15: case 16:
			m_p->m_mesh.SetMtlID(7);
			break;
		case 17:	case 18:
			m_p->m_mesh.SetMtlID(8);
			break;
		case 19:	case 20:
			m_p->m_mesh.SetMtlID(9);
			break;
		case 21:	case 22:
			m_p->m_mesh.SetMtlID(10);
			break;
		case 23:	case 24:
			m_p->m_mesh.SetMtlID(11);
			break;
		case 25:	case 26:
			m_p->m_mesh.SetMtlID(12);
			break;
		case 27:	case 28:
			m_p->m_mesh.SetMtlID(13);
			break;
		case 29:
			m_p->m_mesh.SetMtlID(14);
			break;
		}
	}

	//更新月亮的位置
	void SGMoonNode::UpdateMoonPos(float deltaTime,Vector3 CamPos)
	{
		//得到太阳的相对位置,要求月亮与太阳有相同的旋转轴
		Vector3 SunPos=m_p->m_pSunNode->GetSunRelationPos();
		float fAngle =90.0f + m_p->m_nDate * 5.6f;

		D3DXVec3Normalize(&SunPos,&SunPos);
		SunPos=SunPos*D3DXVec3Length(&m_p->m_fInitMoonPos);

		D3DXMATRIX mat;
		D3DXMatrixRotationAxis(&mat,&m_p->m_MoonRotateAxis,-1*_DegToRad(fAngle));
		D3DXVec3TransformCoord(&m_p->m_MoonRelationPos,&SunPos,&mat);

		m_p->m_MoonPosition=m_p->m_MoonRelationPos + CamPos;

		m_p->m_MoonDirection=m_p->m_MoonRelationPos*-1;
	}

	Vector3 SGMoonNode::GetLightDir()
	{
		return m_p->m_MoonDirection;
	}


}//namespace F3D