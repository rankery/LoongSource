#include "StdAfx.h"
#include ".\skybox.h"
#include "RenderSys.h"
#include "..\ResSys\ResMgr.h"
#include "..\ResSys\ResTexture.h"
#include "..\Util\Exception.h"
#include "..\Kernel.h"
#include "..\Device\IDevice.h"
#include "..\Device\StateBlock.h"

namespace F3D
{
	SkyBox::SkyBox(void)
	{
		m_pTopTex=NULL;
		m_pWarpTex=NULL;
	}

	SkyBox::~SkyBox(void)
	{
		ResMgr::Inst()->ReleaseTexture(m_pTopTex);
		ResMgr::Inst()->ReleaseTexture(m_pWarpTex);
	}
	const AABBox& SkyBox::GetAABBox() const
	{
		return MathConst::BigBox;
	}

	void SkyBox::Draw(RenderSys *pSys,SceneNode *pSGNode,int nLod)
	{
		if(!m_pTopTex->IsCreated()
			|| !m_pWarpTex->IsCreated())
			return;

		IDevice *pDev=Device();
		StateBlock state;
		state.SaveState(ERS_FFogEnable);
		state.SaveState(ERS_CullMode);
		state.SaveState(ERS_FLightEnable);
		state.SaveState(ERS_DepthTestEnable);
		state.SaveState(ERS_DepthWriteEnable);

		pDev->SetCullMode(ECull_None);
		//pDev->FEnableFog(false);
		pDev->FEnableLight(false);
		pDev->EnableDepthTest(false);
		pDev->EnableDepthWrite(false);
		pDev->MatPush(EMS_View);
		pDev->MatPush(EMS_Prj);

		Matrix4 view=*(pDev->GetMatrix(EMS_View));
		view._41=view._42=view._43=0.0f;
		view._41=1.0f;
		pDev->MatLoad(EMS_View,&view);
		pDev->MatLoad(EMS_Prj,m_camera.GetPrj());
		pDev->FFlushMatrix();

		pDev->FSetTextureColorOp(0,ETOP_SelectArg1,ETA_Texture,ETA_Diffuse);
		pDev->SetSimplerFilter(0,ETFilter_LINEAR,ETFilter_LINEAR);

		pDev->SetTexture(0,m_pTopTex->GetDeviceTex());
		pDev->SetSimplerAddress(0,ETAddress_Clamp,ETAddress_Clamp,ETAddress_Clamp);
		pDev->DrawMem(EDraw_TriangleFan,2,(void*)&m_vert[0],sizeof(Vert_PNT));
		pDev->SetTexture(0,m_pWarpTex->GetDeviceTex());
		pDev->DrawMem(EDraw_TriangleFan,2,(void*)&m_vert[4],sizeof(Vert_PNT));
		pDev->DrawMem(EDraw_TriangleFan,2,(void*)&m_vert[8],sizeof(Vert_PNT));
		pDev->DrawMem(EDraw_TriangleFan,2,(void*)&m_vert[12],sizeof(Vert_PNT));
		pDev->DrawMem(EDraw_TriangleFan,2,(void*)&m_vert[16],sizeof(Vert_PNT));
		state.Apply();
		pDev->MatPop(EMS_View);
		pDev->MatPop(EMS_Prj);
		pDev->FFlushMatrix();
	}

	void SkyBox::SetDrawSize(const Vector3& size)
	{
		Vector3 halfSize=size*0.5f;
		//Ë³Ê±Õë²úÉúQUAD
		//--top
		m_vert[0].pos=Vector3(-halfSize.x,halfSize.y,halfSize.z);
		m_vert[1].pos=Vector3(halfSize.x,halfSize.y,halfSize.z);
		m_vert[2].pos=Vector3(halfSize.x,halfSize.y,-halfSize.z);
		m_vert[3].pos=Vector3(-halfSize.x,halfSize.y,-halfSize.z);
		m_vert[0].uv=Vector2(0.0f,1);
		m_vert[1].uv=Vector2(1.0f,1);
		m_vert[2].uv=Vector2(1,0);
		m_vert[3].uv=Vector2(0,0);
		//--warps
		//--front(pos z)
		float maxy=halfSize.y;
		float miny=-20;
		m_vert[4].pos=Vector3(-halfSize.x,maxy,halfSize.z);
		m_vert[5].pos=Vector3(halfSize.x,maxy,halfSize.z);
		m_vert[6].pos=Vector3(halfSize.x,miny,halfSize.z);
		m_vert[7].pos=Vector3(-halfSize.x,miny,halfSize.z);
		m_vert[4].uv=Vector2(0,0);
		m_vert[5].uv=Vector2(1.0f/4,0);
		m_vert[6].uv=Vector2(1.0f/4,1);
		m_vert[7].uv=Vector2(0,1);

		//--right(pos x)
		m_vert[8].pos=Vector3(halfSize.x,maxy,halfSize.z);
		m_vert[9].pos=Vector3(halfSize.x,maxy,-halfSize.z);
		m_vert[10].pos=Vector3(halfSize.x,miny,-halfSize.z);
		m_vert[11].pos=Vector3(halfSize.x,miny,halfSize.z);
		m_vert[8].uv=Vector2(1.0f/4,0);
		m_vert[9].uv=Vector2(1.0f/2,0);
		m_vert[10].uv=Vector2(1.0f/2,1);
		m_vert[11].uv=Vector2(1.0f/4,1);

		//--back(neg z)
		m_vert[12].pos=Vector3(halfSize.x,maxy,-halfSize.z);
		m_vert[13].pos=Vector3(-halfSize.x,maxy,-halfSize.z);
		m_vert[14].pos=Vector3(-halfSize.x,miny,-halfSize.z);
		m_vert[15].pos=Vector3(halfSize.x,miny,-halfSize.z);
		m_vert[12].uv=Vector2(1.0f/2,0);
		m_vert[13].uv=Vector2(3.0f/4,0);
		m_vert[14].uv=Vector2(3.0f/4,1);
		m_vert[15].uv=Vector2(1.0f/2,1);

		//--left(neg xs)
		m_vert[16].pos=Vector3(-halfSize.x,maxy,-halfSize.z);
		m_vert[17].pos=Vector3(-halfSize.x,maxy,halfSize.z);
		m_vert[18].pos=Vector3(-halfSize.x,miny,halfSize.z);
		m_vert[19].pos=Vector3(-halfSize.x,miny,-halfSize.z);
		m_vert[16].uv=Vector2(3.0f/4,0);
		m_vert[17].uv=Vector2(1.0f,0);
		m_vert[18].uv=Vector2(1.0f,1);
		m_vert[19].uv=Vector2(3.0f/4,1);

		m_camera.SetPerspective(Device()->GetClientWidth(),Device()->GetClientWidth(),_DegToRad(90),10,10000);
	}

	void SkyBox::Create(const char* szTopTex,const char* szWarpTex)
	{
		m_pTopTex=ResMgr::Inst()->NewTexture(szTopTex);
		m_pWarpTex=ResMgr::Inst()->NewTexture(szWarpTex);

		SetDrawSize(Vector3(100,100,100));
	}
}//namespace F3D