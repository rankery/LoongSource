#include "StdAfx.h"
#include ".\drawx.h"
#include "..\Kernel.h"
#include "IDevice.h"
#include "StateBlock.h"
#include "IStreamDefine.h"
#include "RenderStatus.h"
#include "IFont.h"
#include "..\Util\Exception.h"
#include "..\Math\Color.h"
#include "..\ResSys\VertStream.h"
#include "..\RenderMtl\IMaterial.h"
#include "..\RenderMtl\MConstColor.h"
#include "..\RenderSys\RenderSys.h"
#include "..\RenderSys\MtlRenderCache.h"

namespace Cool3D
{
	IStreamDefine *DrawX::m_pVertDef=NULL;
	IStreamDefine *DrawX::m_pVertDef2D=NULL;
	VertStream	*DrawX::m_pSphereVerts=NULL;
	IMaterial	*DrawX::m_pMtl3D=NULL;

	DrawX	g_drawX;
	DrawX::DrawX(void)
	{
	}

	DrawX::~DrawX(void)
	{
		SAFE_DELETE(m_pVertDef);
		SAFE_DELETE(m_pVertDef2D);
		SAFE_DELETE(m_pSphereVerts);
		SAFE_DELETE(m_pMtl3D);
	}

	namespace Detail
	{
		void BuildSphereVerts(VertStream* pVerts,float radius, int slices )
		{	
			int numVerts=2*(slices+1)*slices;
			
			pVerts->Create(EVType_PD,numVerts);

			Vert_PD *pV=(Vert_PD *)pVerts->GetRawBuffer();

			// Fill VB (build triangle stripes)
			// I know we could use fans at the poles to reduce
			// vertex count but I don't care for this sample.
			//--
			DWORD i=0, j=0;
			float Phi=0, Theta=2*3.1415926f;
			float PhiInc   = 3.1415926f/slices;
			float ThetaInc = -2*3.1415926f/slices;
			float Pos[3]; // Position of current point
			float UV[2];
			float Norm;   // Norm of position vector (used to compute normal)


			for ( i=0; i<(DWORD)slices; i++ )
			{
				Theta = 0;

				for ( j=0; j<=(DWORD)slices; j++ )
				{			
					// Position and normal computation for 1st point of strip
					//--
					Pos[0] = radius*cosf(Theta)*sinf(Phi);
					Pos[1] = radius*cosf(Phi);
					Pos[2] = radius*sinf(Theta)*sinf(Phi);
					Norm   = sqrtf( Pos[0]*Pos[0] + Pos[1]*Pos[1] + Pos[2]*Pos[2] );
					UV[0] = Theta/(2*3.1415926f);
					UV[1] = Phi/3.1415926f;

					//Normal( Pos[0]/Norm, Pos[1]/Norm, Pos[2]/Norm  );
					pV->diffuse=0xFFFFFFFF;
					pV->pos=Vector3(Pos[0],Pos[1],Pos[2]);
					pV++;

					// Position and normal computation for 2nd point of strip
					//--
					Pos[0] = radius*cosf(Theta)*sinf(Phi+PhiInc);
					Pos[1] = radius*cosf(Phi+PhiInc);
					Pos[2] = radius*sinf(Theta)*sinf(Phi+PhiInc);
					Norm   = sqrtf( Pos[0]*Pos[0] + Pos[1]*Pos[1] + Pos[2]*Pos[2] );
					UV[0] = Theta/(2*3.1415926f);
					UV[1] = (Phi+PhiInc)/3.1415926f;

					pV->diffuse=0xFFFFFFFF;
					pV->pos=Vector3(Pos[0],Pos[1],Pos[2]);
					pV++;

					Theta += ThetaInc;
				}

				Phi += PhiInc;
			}//endof for
		}
	}
	void DrawX::DrawSphere()
	{
		const int Slices=6;
		if(m_pSphereVerts==NULL)
		{
			m_pSphereVerts=new VertStream;
			Detail::BuildSphereVerts(m_pSphereVerts,1.0f,Slices);
		}

		
		IDevice *pDev=Device();
		SetStreamDefine();
		BeginMtl();
		for ( int i=0; i<Slices; i++ )
		{
			Vert_PD *pV=(Vert_PD *)m_pSphereVerts->GetRawBuffer();
			pV+=i*2*(Slices+1);
			pDev->DrawMem(EDraw_TriangleStrip,2*Slices,pV,sizeof(Vert_PD));
		}
		EndMtl();
	}

	void DrawX::DrawDebugString(const TCHAR* szString,int x,int y,DWORD color)
	{
		IFont	*pFont=Device()->GetRenderStatus()->pFont;
		if(pFont!=NULL)
			pFont->DrawText(szString,x,y,color);
	}

	void DrawX::CreateDefaultMtl()
	{
		if(m_pMtl3D==NULL)
		{
			MConstColor *pColor=new MConstColor;
			pColor->m_bUsingVertColor=true;
			m_pMtl3D=pColor;
		}
	}

	void DrawX::BeginMtl()
	{
		CreateDefaultMtl();
		Kernel::Inst()->GetRenderSys()->PrepMtl(m_pMtl3D);
		MtlRenderCache *pMtlCache=m_pMtl3D->GetRenderCache();

		pMtlCache->Begin();
		pMtlCache->BeginPass(0);
	}
	
	void DrawX::EndMtl()
	{
		MtlRenderCache *pMtlCache=m_pMtl3D->GetRenderCache();
		
		pMtlCache->EndPass();
		pMtlCache->End();
	}

	void DrawX::Draw3DLine(const Vector3& p1,const Vector3& p2,DWORD color)
	{
		BeginMtl();
		Vert_PD vert[2];
		vert[0].diffuse=color;
		vert[0].pos=p1;
		vert[1].diffuse=color;
		vert[1].pos=p2;

		IDevice *pDev=Device();
		StateBlock block;
		block.SaveState(ERS_FFogEnable);
		pDev->FEnableFog(false);
		SetStreamDefine();
		pDev->DrawMem(EDraw_LineList,1,vert,sizeof(Vert_PD));
		block.Apply();
		EndMtl();
	}

	void DrawX::Draw2DLine(int sx,int sy,int ex,int ey,DWORD color)
	{
		BeginMtl();
		struct Vert_PwD
		{
			Vector4 pos;
			DWORD	diffuse;
		};
		Vert_PwD vert[2];
		vert[0].diffuse=color;
		vert[0].pos=Vector4((float)sx,(float)sy,0.0f,1.0f);
		vert[1].diffuse=color;
		vert[1].pos=Vector4((float)ex,(float)ey,0.0f,1.0f);

		IDevice *pDev=Device();
		StateBlock block;
		block.SaveState(ERS_DepthTestEnable);
		block.SaveState(ERS_DepthWriteEnable);
		block.SaveState(ERS_AlphaBlendEnable);
		block.SaveState(ERS_SrcBlend);
		block.SaveState(ERS_DstBlend);
		block.SaveState(ERS_CullMode);
		block.SaveState(ERS_FLightEnable);
		block.SaveState(ERS_FFogEnable);
		

		pDev->EnableAlphaBlend(true);
		pDev->SetAlphaBlend(EBlend_SrcAlpha,EBlend_InvSrcAlpha);
		pDev->EnableDepthTest(false);
		pDev->EnableDepthWrite(false);
		pDev->SetCullMode(ECull_None);
		pDev->FEnableLight(false);
		pDev->FEnableFog(false);

		pDev->SetVertShader(NULL);
		pDev->SetPixelShader(NULL);

		SetStreamDefine(1);
		pDev->DrawMem(EDraw_LineList,1,vert,sizeof(Vert_PwD));
		block.Apply();
		EndMtl();
	}

	void DrawX::Draw2DQuad(int x,int y,int width,int height,bool bLine,DWORD color)
	{
		IDevice *pDev=Device();
		int clientW=pDev->GetClientWidth();
		int clientH=pDev->GetClientHeight();
		if(x>=clientW || y>=clientH)
			return;

		RECT destRect;
		destRect.left=x;
		destRect.right=x+width;
		destRect.top=y;
		destRect.bottom=y+height;
		if(destRect.left<0)
			destRect.left=0;
		if(destRect.right>clientW)
			destRect.right=clientW;
		if(destRect.top<0)
			destRect.top=0;
		if(destRect.bottom>clientH)
			destRect.bottom=clientH;

		if(destRect.right-destRect.left<=0
			|| destRect.bottom-destRect.top<=0)
			return;

		
		pDev->MatPush(EMS_Prj);
		pDev->MatPush(EMS_View);
		pDev->MatPush(EMS_World);
		Matrix4 id;
		D3DXMatrixIdentity(&id);
		pDev->MatLoad(EMS_Prj,&id);
		pDev->MatLoad(EMS_View,&id);
		pDev->MatLoad(EMS_World,&id);
		pDev->FFlushMatrix();

		Vert_PD quad[5];
#pragma warning( disable : 4244 )
		quad[0].pos.x=destRect.left;
		quad[0].pos.y=destRect.top;
		quad[1].pos.x=destRect.right;
		quad[1].pos.y=destRect.top;
		quad[2].pos.x=destRect.right;
		quad[2].pos.y=destRect.bottom;
		quad[3].pos.x=destRect.left;
		quad[3].pos.y=destRect.bottom;
		for(int i=0;i<4;i++)
		{
			quad[i].diffuse=color;
			quad[i].pos.x=pDev->NormalizeScreenX(quad[i].pos.x);
			quad[i].pos.y=pDev->NormalizeScreenY(quad[i].pos.y);
			quad[i].pos.z=0;
		}
#pragma warning( default : 4244 )
		quad[4].diffuse=color;
		quad[4].pos=quad[0].pos;

		StateBlock block;
		block.SaveState(ERS_DepthTestEnable);
		block.SaveState(ERS_DepthWriteEnable);
		block.SaveState(ERS_AlphaBlendEnable);
		block.SaveState(ERS_SrcBlend);
		block.SaveState(ERS_DstBlend);
		block.SaveState(ERS_CullMode);
		block.SaveState(ERS_FLightEnable);
		block.SaveState(ERS_FFogEnable);
		

		pDev->EnableAlphaBlend(true);
		pDev->SetAlphaBlend(EBlend_SrcAlpha,EBlend_InvSrcAlpha);
		pDev->EnableDepthTest(false);
		pDev->EnableDepthWrite(false);
		pDev->SetCullMode(ECull_None);
		pDev->FEnableLight(false);
		pDev->FEnableFog(false);

		pDev->SetVertShader(NULL);
		pDev->SetPixelShader(NULL);
		
		pDev->FSetTextureColorOp(0,ETOP_SelectArg1,ETA_Diffuse,ETA_Diffuse);
		pDev->FSetTextureAlphaOp(0,ETOP_SelectArg1,ETA_Diffuse,ETA_Diffuse);
		pDev->FSetTextureColorOp(1,ETOP_Disable,ETA_Diffuse,ETA_Diffuse);
		pDev->FSetTextureAlphaOp(1,ETOP_Disable,ETA_Diffuse,ETA_Diffuse);

		
		SetStreamDefine();
		if(bLine)
			pDev->DrawMem(EDraw_LineStrip,4,(void*)quad,sizeof(Vert_PD));
		else
			Draw3DQuad(quad);

		pDev->MatPop(EMS_Prj);
		pDev->MatPop(EMS_View);
		pDev->MatPop(EMS_World);
		pDev->FFlushMatrix();
		block.Apply();
	}

	void DrawX::Draw2DPrimitiveUP( EDrawType eType, LPPOINT lpPoints, int nVerCount, DWORD color)
	{
		IDevice *pDev=Device();
		
		pDev->MatPush(EMS_Prj);
		pDev->MatPush(EMS_View);
		pDev->MatPush(EMS_World);
		Matrix4 id;
		D3DXMatrixIdentity(&id);
		pDev->MatLoad(EMS_Prj,&id);
		pDev->MatLoad(EMS_View,&id);
		pDev->MatLoad(EMS_World,&id);
		pDev->FFlushMatrix();

		Vert_PD* quad = new Vert_PD[nVerCount];
#pragma warning( disable : 4244 )
		for( int i=0; i<nVerCount; i++ ) 
		{
			quad[i].diffuse=color;			
			quad[i].pos.x=pDev->NormalizeScreenX(lpPoints[i].x);
			quad[i].pos.y=pDev->NormalizeScreenY(lpPoints[i].y);
			quad[i].pos.z=0;
		}
#pragma warning( default : 4244 )

		StateBlock block;
		block.SaveState(ERS_DepthTestEnable);
		block.SaveState(ERS_DepthWriteEnable);
		block.SaveState(ERS_AlphaBlendEnable);
		block.SaveState(ERS_SrcBlend);
		block.SaveState(ERS_DstBlend);
		block.SaveState(ERS_CullMode);
		block.SaveState(ERS_FLightEnable);
		block.SaveState(ERS_FFogEnable);
		

		pDev->EnableAlphaBlend(true);
		pDev->SetAlphaBlend(EBlend_SrcAlpha,EBlend_InvSrcAlpha);
		pDev->EnableDepthTest(false);
		pDev->EnableDepthWrite(false);
		pDev->SetCullMode(ECull_None);
		pDev->FEnableLight(false);
		pDev->FEnableFog(false);

		pDev->SetVertShader(NULL);
		pDev->SetPixelShader(NULL);
		
		pDev->FSetTextureColorOp(0,ETOP_SelectArg1,ETA_Diffuse,ETA_Diffuse);
		pDev->FSetTextureAlphaOp(0,ETOP_SelectArg1,ETA_Diffuse,ETA_Diffuse);
		pDev->FSetTextureColorOp(1,ETOP_Disable,ETA_Diffuse,ETA_Diffuse);
		pDev->FSetTextureAlphaOp(1,ETOP_Disable,ETA_Diffuse,ETA_Diffuse);

		
		SetStreamDefine();
		UINT PrimitiveCount = 0;
		// 根据顶点数，计算图元数量
		switch( eType )
		{
		case	EDraw_PointList:	PrimitiveCount = nVerCount;			break;
		case	EDraw_LineList:		PrimitiveCount = nVerCount/2;		break;
		case	EDraw_LineStrip:	PrimitiveCount = nVerCount-1;		break;
		case	EDraw_TriangleList:	PrimitiveCount = nVerCount/3;		break;
		case	EDraw_TriangleStrip:PrimitiveCount = nVerCount-2;		break;
		case	EDraw_TriangleFan:	PrimitiveCount = nVerCount-2;		break;
		}
		ASSERT( PrimitiveCount > 0 );

		pDev->DrawMem(eType,PrimitiveCount,(void*)quad,sizeof(Vert_PD));
		
		pDev->MatPop(EMS_Prj);
		pDev->MatPop(EMS_View);
		pDev->MatPop(EMS_World);
		pDev->FFlushMatrix();
		block.Apply();

		delete [] quad;
	}

	void DrawX::SetStreamDefine(int type)
	{
		switch(type)
		{
		case 0:
			if(m_pVertDef==NULL)
			{
				m_pVertDef=Device()->NewStreamDefine();
				m_pVertDef->Begin();
				m_pVertDef->AddElement(0,		0,		VET_FLOAT3,		VES_POSITION,	0);
				m_pVertDef->AddElement(0,		4*3,	VET_COLOUR,		VES_DIFFUSE,	0);
				m_pVertDef->End();
			}
			Device()->SetStreamDefine(m_pVertDef);
			break;
		case 1:
			if(m_pVertDef2D==NULL)
			{
				m_pVertDef2D=Device()->NewStreamDefine();
				m_pVertDef2D->Begin();
				m_pVertDef2D->AddElement(0,		0,		VET_FLOAT4,		VES_POSITION_TRANSED,	0);
				m_pVertDef2D->AddElement(0,		4*4,	VET_COLOUR,		VES_DIFFUSE,			0);
				m_pVertDef2D->End();
			}
			Device()->SetStreamDefine(m_pVertDef2D);
			break;
		}
	}
	void DrawX::Draw3DQuad(const Vert_PD* pVert)
	{
		SetStreamDefine();
		Device()->DrawMem(EDraw_TriangleFan,2,(void*)pVert,sizeof(Vert_PD));
	}


	void DrawX::DrawAABBox(const AABBox& box,DWORD color)
	{
		BeginMtl();

		IDevice *pDevice=Device();
		StateBlock sb;
		sb.SaveState(ERS_FillMode);
		sb.SaveState(ERS_CullMode);
		pDevice->SetFillMode(EFill_Wireframe);
		pDevice->SetCullMode(ECull_None);
		pDevice->FSetTextureColorOp(0,ETOP_SelectArg1,ETA_Diffuse,ETA_Diffuse);
		pDevice->FSetTextureColorOp(1,ETOP_Disable,ETA_Diffuse,ETA_Diffuse);
		
		Vert_PD quad[4];
		int i;
		for(i=0;i<4;i++)
			quad[i].diffuse=color;
		quad[0].pos=Vector3(box.min.x,box.max.y,box.max.z);
		quad[1].pos=Vector3(box.max.x,box.max.y,box.max.z);
		quad[2].pos=Vector3(box.max.x,box.min.y,box.max.z);
		quad[3].pos=Vector3(box.min.x,box.min.y,box.max.z);
		Draw3DQuad(quad);

		quad[0].pos=Vector3(box.min.x,box.max.y,box.min.z);
		quad[1].pos=Vector3(box.max.x,box.max.y,box.min.z);
		quad[2].pos=Vector3(box.max.x,box.min.y,box.min.z);
		quad[3].pos=Vector3(box.min.x,box.min.y,box.min.z);
		Draw3DQuad(quad);
		//left
		quad[0].pos=Vector3(box.min.x,box.max.y,box.max.z);
		quad[1].pos=Vector3(box.min.x,box.max.y,box.min.z);
		quad[2].pos=Vector3(box.min.x,box.min.y,box.min.z);
		quad[3].pos=Vector3(box.min.x,box.min.y,box.max.z);
		Draw3DQuad(quad);
		//right
		quad[0].pos=Vector3(box.max.x,box.max.y,box.max.z);
		quad[1].pos=Vector3(box.max.x,box.max.y,box.min.z);
		quad[2].pos=Vector3(box.max.x,box.min.y,box.min.z);
		quad[3].pos=Vector3(box.max.x,box.min.y,box.max.z);
		Draw3DQuad(quad);
		//top
		quad[0].pos=Vector3(box.min.x,box.max.y,box.min.z);
		quad[1].pos=Vector3(box.max.x,box.max.y,box.min.z);
		quad[2].pos=Vector3(box.max.x,box.max.y,box.max.z);
		quad[3].pos=Vector3(box.min.x,box.max.y,box.max.z);
		Draw3DQuad(quad);
		//bottom
		quad[0].pos=Vector3(box.min.x,box.min.y,box.min.z);
		quad[1].pos=Vector3(box.max.x,box.min.y,box.min.z);
		quad[2].pos=Vector3(box.max.x,box.min.y,box.max.z);
		quad[3].pos=Vector3(box.min.x,box.min.y,box.max.z);
		Draw3DQuad(quad);

		sb.Apply();

		EndMtl();
	}
}//namespace Cool3D