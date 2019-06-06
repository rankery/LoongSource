#include "stdafx.h"
#include "AmbientOcclusion.h"
#include "IUICallBack.h"
#include "SGVisitor.h"
#include "..\ResSys\ResMgr.h"
#include "..\Device\IVertShader.h"
#include "..\Device\IPixelShader.h"
#include "..\Device\IDevice.h"
#include "..\Kernel.h"
#include "..\Util\DebugMisc.h"
#include "..\Terrain\TerrainPatch.h"
#include "LightMapTempGen.h"
#include "..\Device\IDeviceTex.h"
#include "..\Device\IRenderTexture.h"
#include "..\SceneGraph\HugeQuadTreeSG.h"
#include "..\Device\StateBlock.h"
#include "..\RenderSys\RenderSys.h"
#include "..\Public\FileUtil.h"
#include "..\Util\FMemory.h"
#include "..\RenderSys\RenderNode.h"
#include "..\Util\FileUtil.h"
#include "..\SceneGraph\EntityNode.h"

#include <process.h>	// for _beginthreadex
#include "..\Device\DX9Device\DX9RenderTex.h"
#include "..\Device\DX9Device\DX9device.h"
using namespace Detail;

namespace Cool3D
{

	BOOL f_bTerminateThreadAmbientOcclusionWriteFile = FALSE;

	class AmbientOcclusion::Member
	{
	public:
		static int LightCount;
		static const int MaxLightCount=1000;

		Member();
		~Member();

		IDeviceTex	*LoadPosMap(SceneNode *pNode);
		IDeviceTex	*LoadNormalMap(SceneNode *pNode);
		IDeviceTex	*LoadAOMap(SceneNode *pNode);

		void		CalcLightDir(void);
		bool		MakeDepthMap(HugeQuadTreeSG* pSG,SceneNode *pNode,int number);
		void		ImageEnhancement(const TCHAR *szSrcPath,const TCHAR *szDstPath,float lum,float constract);

	public:
		IVertShader		*m_pDepthVS;
		IPixelShader	*m_pDepthPS;
		IVertShader		*m_pAOVS;
		IPixelShader	*m_pAOPS;

		IRenderTexture	*m_pDepthRT;
		Matrix4			m_matViewPrj;

		BOOL			m_bSmallAO;

	private:
		Vector3			m_lightDir[MaxLightCount];
	};

	int AmbientOcclusion::Member::LightCount = 0;
	AmbientOcclusion::Member::Member()
		: m_bSmallAO(TRUE)
	{
		IDevice* pDev=Device();

		m_pDepthVS=pDev->NewVS();
		m_pDepthVS->CreateFromFile(_T("data\\system\\radiosity\\depthmap_direct.vs"),"vs_2_0","main");
		m_pDepthPS=pDev->NewPS();
		m_pDepthPS->CreateFromFile(_T("data\\system\\radiosity\\depthmap_direct.ps"),"ps_2_0","main");

		m_pAOVS=pDev->NewVS();
		m_pAOVS->CreateFromFile(_T("data\\system\\radiosity\\ambientocclusion.vs"),"vs_2_0","main");
		m_pAOPS=pDev->NewPS();
		m_pAOPS->CreateFromFile(_T("data\\system\\radiosity\\ambientocclusion.ps"),"ps_2_0","main");

		m_pDepthRT=pDev->NewRenderTexture();
		m_pDepthRT->Create(1024,1024,EPF_R32F,true,false,1);

		CalcLightDir();
	}

	AmbientOcclusion::Member::~Member()
	{
		SAFE_DELETE(m_pAOVS);
		SAFE_DELETE(m_pAOPS);
		SAFE_DELETE(m_pDepthVS);
		SAFE_DELETE(m_pDepthPS);

		SAFE_DELETE(m_pDepthRT);
	}

	IDeviceTex* AmbientOcclusion::Member::LoadPosMap(SceneNode *pNode)
	{
		IDeviceTex *pTex=Device()->NewTexture();
		IFS *pFS=Kernel::Inst()->DefaultFS();
		tstring szPath=LightMapTempGen::MakePosMapName(pNode->GetLightmap());
		pTex->LoadFromFile(pFS,szPath.c_str(),1);
		return pTex;
	}

	IDeviceTex* AmbientOcclusion::Member::LoadNormalMap(SceneNode *pNode)
	{
		IDeviceTex *pTex=Device()->NewTexture();
		IFS *pFS=Kernel::Inst()->DefaultFS();
		tstring szPath=LightMapTempGen::MakeNormalMapName(pNode->GetLightmap());
		pTex->LoadFromFile(pFS,szPath.c_str(),1);
		return pTex;
	}

	IDeviceTex * AmbientOcclusion::Member::LoadAOMap( SceneNode *pNode )
	{
		IDeviceTex* pTex=Device()->NewTexture();
		IFS* pFS=Kernel::Inst()->DefaultFS();
		tstring szPath=AmbientOcclusion::MakeAOMapName(pNode->GetLightmap());
		pTex->LoadFromFile(pFS,szPath.c_str(),1);
		return pTex;
	}

	bool AmbientOcclusion::Member::MakeDepthMap( HugeQuadTreeSG* pSG,SceneNode *pNode,int number )
	{
		ASSERT(number>=0 && number<LightCount);
		ASSERT(pNode!=NULL);

		AABBox nodeBox;
		pNode->GetBox(nodeBox);

		CameraBase camera;

		RenderNode::m_pSpecialVS=m_pDepthVS;
		RenderNode::m_pSpecialPS=m_pDepthPS;

		float radius=D3DXVec3Length(&(nodeBox.max-nodeBox.min))/2.0f;
		Vector3 dir=m_lightDir[number];
		Vector3 lookAt=nodeBox.GetCenter();		lookAt.y=nodeBox.min.y;
		Vector3 lookFrom=lookAt+dir*(radius+90000.0f);
		camera.SetLookAt(lookFrom,lookAt);
		camera.SetOrtho((float)m_pDepthRT->Width(),(float)m_pDepthRT->Height(),0,radius*2.0f+200000.0f);
		camera.AdjustAdaptive(nodeBox);

		D3DXMatrixMultiply(&m_matViewPrj,camera.GetView(),camera.GetPrj());

		Vector3 nodeViewZone((nodeBox.max.x-nodeBox.min.x)*0.5f+300.0f,
			(nodeBox.max.y-nodeBox.min.y)*0.5f+90000.0f,(nodeBox.max.z-nodeBox.min.z)*0.5f+300.0f);

		if( pNode->IS_STATIC_CLASS(TerrainPatch) )	//拍地块时扩大范围 地块是不是应该更大些
		{
			nodeViewZone.x = (nodeBox.max.x-nodeBox.min.x) * 3.0f;//1.5f;
			nodeViewZone.z = (nodeBox.max.z-nodeBox.min.z) * 3.0f;//1.5f;

			if( m_bSmallAO == FALSE )
			{
				Vector3 litViewZone(-1.0f,-1.0f,-1.0f);
				pSG->SetViewZoneSize(HugeQuadTreeSG::EVZT_LitNode,litViewZone);
			}
		}
		else if( m_bSmallAO==FALSE && Vec3DistSq(nodeBox.min,nodeBox.max) >= 1000000.0f )//中大型地物
		{
			Vector3 litViewZone(-1.0f,-1.0f,-1.0f);
			pSG->SetViewZoneSize(HugeQuadTreeSG::EVZT_LitNode,litViewZone);
		}
		else
			pSG->SetViewZoneSize(HugeQuadTreeSG::EVZT_LitNode,nodeViewZone);

		pSG->SetViewZoneSize(HugeQuadTreeSG::EVZT_MidNode,nodeViewZone);
		pSG->SetViewZoneSize(HugeQuadTreeSG::EVZT_BigNode,nodeViewZone);
		pSG->SetViewZoneSize(HugeQuadTreeSG::EVZT_SimpleTerrain,nodeViewZone);
		pSG->InvalidViewZone();
		pSG->SyncInit();

		pSG->RenderShadowMap(camera,m_pDepthRT);

		RenderNode::m_pSpecialVS=NULL;
		RenderNode::m_pSpecialPS=NULL;

		//m_pDepthRT->WriteToFile(_T("c:\\depth.tga"));

		return true;
	}

	void AmbientOcclusion::Member::CalcLightDir( void )
	{
		int dirCount=0;
		Vector3 dir;

		for( float zi=5; zi<90; zi+=5 )
		{
			dir.y=sinf(_DegToRad(zi));
			float step = 360 / (36.0f * cosf(_DegToRad(zi)));
			for( float xyi=0+zi*4; xyi<360+zi*4; xyi+=step )
			{
				dir.x=cosf(_DegToRad(xyi));
				dir.z=sinf(_DegToRad(xyi));

				m_lightDir[dirCount]=dir;
				D3DXVec3Normalize(&m_lightDir[dirCount],&m_lightDir[dirCount]);
				dirCount++;
			}
		}

		m_lightDir[dirCount]=Vector3(0.0f,1.0f,0.0f);
		LightCount=dirCount;
	}

	void AmbientOcclusion::Member::ImageEnhancement( const TCHAR *szSrcPath,const TCHAR *szDstPath, float lum, float constract )
	{
		ASSERT(szSrcPath!=NULL && szDstPath!=NULL);

		Image occlusionMap;
		occlusionMap.LoadFromFile(szSrcPath);

		occlusionMap.AdjuctLumContrast(lum,constract);

		occlusionMap.WriteToFile(szDstPath);
	}

	AmbientOcclusion::AmbientOcclusion( IUICallBack* pUI )
	{
		m_pUI=pUI;
		m_p=new Member;
	}

	AmbientOcclusion::~AmbientOcclusion( void )
	{
		delete m_p;
	}

	bool AmbientOcclusion::OnVisitSceneNode( HugeQuadTreeSG *pSG,SceneNode *pNode,int index,int totalNum,DWORD param )
	{
		DWORD dwTime = timeGetTime();
		if(_tcslen(pNode->GetLightmap())==0)
			return true;

		IDevice* pDev=Device();

		/*
		0____1
		|    |
		3____2
		*/
		Vert_P vert[4];
		vert[0].pos=Vector3(0,		0,		0);
		vert[1].pos=Vector3(1.0f,	0,		0);
		vert[2].pos=Vector3(1.0f,	1.0f,	0);
		vert[3].pos=Vector3(0,		1.0f,	0);

		RenderSys* pSys=Kernel::Inst()->GetRenderSys();

		IDeviceTex *pLightPosMap=m_p->LoadPosMap(pNode);

		IRenderTexture* pAOTex=pDev->NewRenderTexture();
		pAOTex->Create(pLightPosMap->Width(),pLightPosMap->Height(),EPF_R32F,false,false,1);

		pAOTex->Begin();
		pDev->Clear(true,false,false,0,1,0);
		pAOTex->End();

		//--计算遮挡系数
		for(int i=0;i<Member::LightCount;i++)
		{
			m_p->MakeDepthMap(pSG,pNode,i);

			pAOTex->Begin();

			StateBlock block;
			block.SaveState(ERS_AlphaBlendEnable);	pDev->EnableAlphaBlend(true);
			block.SaveState(ERS_SrcBlend);
			block.SaveState(ERS_DstBlend);			pDev->SetAlphaBlend(EBlend_One,EBlend_One);
			block.SaveState(ERS_DepthTestEnable);	pDev->SetRenderState(ERS_DepthTestEnable,false);
			block.SaveState(ERS_DepthWriteEnable);	pDev->SetRenderState(ERS_DepthWriteEnable,false);
			block.SaveState(ERS_CullMode);			pDev->SetCullMode(ECull_None);

			pDev->SetStreamDefine(pSys->GetStreamDefine(EVType_P));

			pDev->SetVertShader(m_p->m_pAOVS);
			pDev->SetPixelShader(m_p->m_pAOPS);

			pDev->SetTexture(0,pLightPosMap);
			pDev->SetSimplerAddress(0,ETAddress_Clamp,ETAddress_Clamp,ETAddress_Clamp);
			pDev->SetSimplerFilter(0,ETFilter_POINT,ETFilter_POINT);
			pDev->SetSimplerMipFilter(0,ETFilter_NONE);

			m_p->m_pAOPS->SetConstValue("g_matViewPrj",(void*)&(m_p->m_matViewPrj),sizeof(Matrix4));
			m_p->m_pAOPS->SetConstValue("g_iLightCount",(void*)&(Member::LightCount),sizeof(int));

			pDev->SetTexture(1,m_p->m_pDepthRT);
			pDev->SetSimplerAddress(1,ETAddress_Clamp,ETAddress_Clamp,ETAddress_Clamp);
			pDev->SetSimplerFilter(1,ETFilter_POINT,ETFilter_POINT);
			pDev->SetSimplerMipFilter(1,ETFilter_NONE);

			pDev->DrawMem(EDraw_TriangleFan,2,&vert[0],sizeof(Vert_P));

			block.Apply();

			pAOTex->End();
		}

		// 将显存纹理拷到内存纹理，然后交给线程写盘
		IDirect3DSurface9* pSurface = ((DX9RenderTex*)pAOTex)->GetDX9Surface();
		IDirect3DSurface9* pDestSurface = NULL;
		IDirect3DTexture9* pTempTex = NULL;
		while( TRUE )
		{
			HRESULT hr = D3DXCreateTexture(DX9Dev(), pLightPosMap->Width(), pLightPosMap->Height(), 1, 0, D3DFMT_R32F, D3DPOOL_SYSTEMMEM, &pTempTex);
			if( D3D_OK == hr)
				break;

			Sleep(1000);
		}
		pTempTex->GetSurfaceLevel(0, &pDestSurface);
		DX9Dev()->GetRenderTargetData(pSurface, pDestSurface);
		SAFE_RELEASE(pDestSurface);
		tagThreadData* pData = new tagThreadData;
		pData->pTexture = pTempTex;
		pData->strName = MakeAOMapName(pNode->GetLightmap());
		m_listRT.PushBack(pData);
		//pAOTex->WriteToFile(MakeAOMapName(pNode->GetLightmap()).c_str());


		SAFE_DELETE(pLightPosMap);
		SAFE_DELETE(pAOTex);

		//--
		pDev->SetTexture(0,(IDeviceTex*)NULL);
		pDev->SetTexture(1,(IDeviceTex*)NULL);
		pDev->SetVertShader(NULL);
		pDev->SetPixelShader(NULL);

		//--界面进度条
		m_pUI->SetProgress(index+1,totalNum);

		while(m_listRT.Size()>50)
			Sleep(1000);	// 避免内存上涨过快

		// 显示调试信息
		// m_pUI->AddMsg(_T("index=%d, time=%d, list:%d\r\n"), index, timeGetTime() - dwTime, m_listRT.Size());
		return true;
	}

	UINT WINAPI ThreadAmbientOcclusionWriteFile(LPVOID pParam);
	bool AmbientOcclusion::Start( const TCHAR *szSGPath,BOOL bSmallAO )
	{
		m_pUI->SetProgress(0,1);

		ResMgr::Inst()->EnableIOAsync(false);

		m_p->m_bSmallAO = bSmallAO;

		f_bTerminateThreadAmbientOcclusionWriteFile = FALSE;
		HANDLE hHandle = (HANDLE)_beginthreadex(NULL, 0, ThreadAmbientOcclusionWriteFile, &m_listRT, 0, NULL);

		SGVisitor visitor;
		bool bResult = visitor.Start(szSGPath,128.0f*50.0f,512.0f*50.0f,this,0);

		// 等待写盘完成
		f_bTerminateThreadAmbientOcclusionWriteFile = TRUE;
		::WaitForSingleObject( hHandle, INFINITE );
		return bResult;
	}

	tstring AmbientOcclusion::MakeAOMapName( const TCHAR *szMapName )
	{
		TCHAR szPath[512];
		TCHAR szName[512];
		_tsplitpath(szMapName,NULL,szPath,szName,NULL);

		TCHAR szOut[512];
		_sntprintf(szOut,512,_T("%s%s_ao.tga"),szPath,szName);

		return tstring(szOut);
	}

	tstring AmbientOcclusion::MakeAOTempMapName( const TCHAR *szMapName )
	{
		TCHAR szPath[512];
		TCHAR szName[512];
		_tsplitpath(szMapName,NULL,szPath,szName,NULL);

		TCHAR szOut[512];
		_sntprintf(szOut,512,_T("%s%s_temp.aotmp"),szPath,szName);

		return tstring(szOut);
	}

	void AmbientOcclusion::DeleteTempFiles( const TCHAR *szPath )
	{
		TCHAR szFind[512];
		_sntprintf(szFind,512,_T("%s\\*.aotmp"),szPath);
		DeleteFiles(szFind);

		_sntprintf(szFind,512,_T("%s\\*_ao.tga"),szPath);
		DeleteFiles(szFind);
	}


	UINT WINAPI ThreadAmbientOcclusionWriteFile(LPVOID pParam)
	{
		TSafeList<AmbientOcclusion::tagThreadData*>* pList = (TSafeList<AmbientOcclusion::tagThreadData*>*)pParam;

		while( !f_bTerminateThreadAmbientOcclusionWriteFile || pList->Size() > 0 )
		{
			while( pList->Size() > 0 )
			{
				AmbientOcclusion::tagThreadData* pData = pList->PopFront();
				IDirect3DTexture9* pRenderTexture = pData->pTexture;
				IDirect3DSurface9* pSurface = NULL;
				pRenderTexture->GetSurfaceLevel(0, &pSurface);


				while(TRUE)
				{
					if( D3D_OK == D3DXSaveSurfaceToFile(pData->strName.c_str(), D3DXIFF_TGA, pSurface, NULL, NULL) )
						break;
				}

				SAFE_RELEASE(pSurface);
				SAFE_RELEASE(pRenderTexture);
				SAFE_DELETE(pData);
			}

			::WaitForSingleObject(pList->GetEvent(), 100);
		}
		return 0;
	}

}//namespace Cool3D