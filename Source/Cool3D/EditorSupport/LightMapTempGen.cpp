#include "StdAfx.h"
#include "..\Device\IRenderTexture.h"
#include "..\Device\IVertShader.h"
#include "..\Device\IPixelShader.h"
#include "..\Device\StateBlock.h"
#include "..\SceneGraph\HugeQuadTreeSG.h"
#include "..\SceneGraph\SceneNode.h"
#include "..\SceneGraph\EntityNode.h"
#include "..\RenderSys\RenderSys.h"
#include "..\Image\Image.h"
#include "..\ResSys\ResMgr.h"
#include "..\Kernel.h"
#include "..\Terrain\TerrainPatch.h"
#include "..\Terrain\TerrainPatchRender.h"
#include "..\Terrain\TPatchLodTable.h"
#include ".\SGVisitor.h"
#include ".\LightMapTempGen.h"
#include ".\LightMapBlur.h"
#include ".\IUICallBack.h"
#include "..\Util\FileUtil.h"
#include "..\RenderMtl\MGeneral.h"
#include "..\RenderMtl\MConstColor.h"
#include "..\ResSys\ResStaticMesh.h"
#include "..\ResSys\ResSceneMesh.h"
#include "..\ResSys\ResKeyFrame.h"

#include <process.h>	// for _beginthreadex
#include "..\Device\DX9Device\DX9RenderTex.h"
#include "..\Device\DX9Device\DX9device.h"
using namespace Detail;
namespace Cool3D
{

	BOOL f_bTerminateThreadLightMapTempGenWriteFile = FALSE;

	//--class LightMapTempGen::Member-------------------------
	class LightMapTempGen::Member
	{
	public:
		IVertShader*	m_pMdlPosVS;
		IPixelShader*	m_pMdlPosPS;
		IVertShader*	m_pMdlNormalVS;
		IPixelShader*	m_pMdlNormalPS;
		IVertShader*	m_pTrnPosVS;
		IPixelShader*	m_pTrnPosPS;
		IVertShader*	m_pTrnNormalVS;
		IPixelShader*	m_pTrnNormalPS;
		IRenderTexture* m_pDiffuseRT;
		LightMapBlur*	m_pBlur;

		bool			m_isPos;
	public:

		Member()
		{
			IDevice* pDev=Device();
			m_pMdlPosVS=pDev->NewVS();
			m_pMdlPosVS->CreateFromFile(_T("data\\system\\radiosity\\mdl_pos.vs"),"vs_2_0","main");
			m_pMdlPosPS=pDev->NewPS();
			m_pMdlPosPS->CreateFromFile(_T("data\\system\\radiosity\\mdl_pos.ps"),"ps_2_0","main");
			m_pMdlNormalVS=pDev->NewVS();
			m_pMdlNormalVS->CreateFromFile(_T("data\\system\\radiosity\\mdl_normal.vs"),"vs_2_0","main");
			m_pMdlNormalPS=pDev->NewPS();
			m_pMdlNormalPS->CreateFromFile(_T("data\\system\\radiosity\\mdl_normal.ps"),"ps_2_0","main");

			m_pTrnPosVS=pDev->NewVS();
			m_pTrnPosVS->CreateFromFile(_T("data\\system\\radiosity\\trn_pos.vs"),"vs_2_0","main");
			m_pTrnPosPS=pDev->NewPS();
			m_pTrnPosPS->CreateFromFile(_T("data\\system\\radiosity\\trn_pos.ps"),"ps_2_0","main");
			m_pTrnNormalVS=pDev->NewVS();
			m_pTrnNormalVS->CreateFromFile(_T("data\\system\\radiosity\\trn_normal.vs"),"vs_2_0","main");
			m_pTrnNormalPS=pDev->NewPS();
			m_pTrnNormalPS->CreateFromFile(_T("data\\system\\radiosity\\trn_normal.ps"),"ps_2_0","main");

			m_pDiffuseRT=pDev->NewRenderTexture();
			m_pDiffuseRT->Create(1024,1024,EPF_A32R32G32B32F,false,false,1);

			m_pBlur=new LightMapBlur(NULL);
			m_isPos=false;
		}

		~Member()
		{
			SAFE_DELETE(m_pMdlPosVS);
			SAFE_DELETE(m_pMdlPosPS);
			SAFE_DELETE(m_pMdlNormalVS);
			SAFE_DELETE(m_pMdlNormalPS);

			SAFE_DELETE(m_pTrnPosVS);
			SAFE_DELETE(m_pTrnPosPS);
			SAFE_DELETE(m_pTrnNormalVS);
			SAFE_DELETE(m_pTrnNormalPS);

			SAFE_DELETE(m_pDiffuseRT);

			SAFE_DELETE(m_pBlur);
		}

		int CalcLightMapSize(SceneNode* pNode)
		{
			//return 32;//test
			const float		m_boxSize[4]={500.0f,1000.0f,2000.0f,3000.0f};
			const int		m_imgSize[5]={32,64,128,256,512};

			if(pNode->IS_STATIC_CLASS(TerrainPatch))
				return 256;

			int lightmapSize=0;
			if(pNode->IS_STATIC_CLASS(EntityNode))
			{
				EntityNode *pEntity=(EntityNode*)pNode;
				ResRenderable *pRes=pEntity->GetResObj();
				ASSERT( pRes!=NULL );

				if( pRes->IS_STATIC_CLASS(ResStaticMesh) )
				{
					ResStaticMesh *pResStatic=(ResStaticMesh*)pRes;
					lightmapSize=pResStatic->GetLightMapSize();
				}
				else if( pRes->IS_STATIC_CLASS(ResSceneMesh) )
				{
					ResSceneMesh *pResScene=(ResSceneMesh*)pRes;
					lightmapSize=pResScene->GetLightMapSize();
				}
				else if( pRes->IS_STATIC_CLASS(ResKeyFrame) )
				{
					ResKeyFrame *pResKF=(ResKeyFrame*)pRes;
					lightmapSize=pResKF->GetLightMapSize();
				}
			}

			if( lightmapSize>0 )
			{
				//m_pUI->AddMsg(_T(""));
				return lightmapSize;
			}

			AABBox box;
			pNode->GetBox(box);
			float sz=Vec3Dist(box.min,box.max);
			int j;
			for(j=0;j<4;j++)
			{
				if(sz<m_boxSize[j])
					break;
			}
			return m_imgSize[j];
		}
	};

	//--class LightMapTempGen----------------------------------
	LightMapTempGen::LightMapTempGen(IUICallBack* pUI):m_pUI(pUI)
	{
		m_p=new Member;
	}

	LightMapTempGen::~LightMapTempGen(void)
	{
		delete m_p;
	}

	bool LightMapTempGen::OnVisitSceneNode(HugeQuadTreeSG* pSG,SceneNode* pNode,int index,int totalNum,DWORD param)
	{
		DWORD dwTime = timeGetTime();
		if(_tcslen(pNode->GetLightmap())==0)
			return true;

		//--
		IDevice* pDev=Device();
		int lightMapSize=m_p->CalcLightMapSize(pNode);
		ASSERT(lightMapSize<=512);
		IRenderTexture* pRenderTex=pDev->NewRenderTexture();
		pRenderTex->Create(lightMapSize,lightMapSize,EPF_A32R32G32B32F,false,false);

		//--
		CameraBase camera;

		AABBox box;
		pNode->GetBox(box);
		Vector3 lookAt=box.GetCenter();
		Vector3 dir(1,-1,1);
		Vec3Normalize(dir);
		Vector3 from=lookAt-dir*Vec3Dist(box.min,box.max);
		camera.SetLookAt(from,lookAt);

		camera.SetOrtho((float)lightMapSize,(float)lightMapSize,0,100000.0f);
		camera.AdjustAdaptive(box);

		//--
		TerrainPatchRender::m_renderMode=TerrainPatchRender::ERM_Editor;
		TPatchLodTable::Inst()->Clear(0);

		m_p->m_isPos=true;

		RenderSys* pRenderSys=Kernel::Inst()->GetRenderSys();

		StateBlock block;
		block.SaveState(ERS_DepthTestEnable);	pDev->SetRenderState(ERS_DepthTestEnable,false);
		block.SaveState(ERS_DepthWriteEnable);	pDev->SetRenderState(ERS_DepthWriteEnable,false);
		block.SaveState(ERS_CullMode);			pDev->SetCullMode(ECull_None);
		//--
		if(pNode->IS_STATIC_CLASS(EntityNode))
		{
			m_p->m_pMdlPosVS->SetConstValue("g_matWorld",(void*)&pNode->GetWorldMat(),sizeof(Matrix4));
			RenderNode::m_pSpecialVS=m_p->m_pMdlPosVS;
			RenderNode::m_pSpecialPS=m_p->m_pMdlPosPS;
		}
		else
		{
			RenderNode::m_pSpecialVS=m_p->m_pTrnPosVS;
			RenderNode::m_pSpecialPS=m_p->m_pTrnPosPS;
		}

		//--需要利用alpha通道作为mtlID，暂时屏蔽alphatest
		RenderNode::m_pSpecialMtl = new MConstColor;

		pRenderSys->RenderSceneNode(&camera,pNode,m_p->m_pDiffuseRT,true,0,0,this);

		//--
		pRenderTex->StrechRect(m_p->m_pDiffuseRT,NULL,NULL,ETFilter_NONE);
		m_p->m_pBlur->Diffuse(pRenderTex,1);

		// 将显存纹理拷到内存纹理，然后交给线程写盘
		IDirect3DSurface9* pSurface = ((DX9RenderTex*)pRenderTex)->GetDX9Surface();
		IDirect3DSurface9* pDestSurface = NULL;
		IDirect3DTexture9* pTempTex = NULL;

		while( TRUE )
		{
			HRESULT hr = D3DXCreateTexture(DX9Dev(), lightMapSize, lightMapSize, 1, 0, D3DFMT_A32B32G32R32F, D3DPOOL_SYSTEMMEM, &pTempTex);
			if( D3D_OK == hr)
				break;

			Sleep(1000);
		}
		
		pTempTex->GetSurfaceLevel(0, &pDestSurface);
		DX9Dev()->GetRenderTargetData(pSurface, pDestSurface);
		SAFE_RELEASE(pDestSurface);
		tagThreadData* pData = new tagThreadData;
		pData->pTexture = pTempTex;
		pData->strName = MakePosMapName(pNode->GetLightmap());
		m_listRT.PushBack(pData);
		// pRenderTex->WriteToFile(szPosMap.c_str());

		SAFE_DELETE(RenderNode::m_pSpecialMtl);
		m_p->m_isPos=false;
		//--
		if(pNode->IS_STATIC_CLASS(EntityNode))
		{
			Matrix4 matInv,matInvTrans;
			D3DXMatrixInverse(&matInv,NULL,&pNode->GetWorldMat());
			D3DXMatrixTranspose(&matInvTrans,&matInv);

			m_p->m_pMdlNormalVS->SetConstValue("g_matWorldInvTrans",&matInvTrans,sizeof(Matrix4));
			RenderNode::m_pSpecialVS=m_p->m_pMdlNormalVS;
			RenderNode::m_pSpecialPS=m_p->m_pMdlNormalPS;
			pRenderSys->RenderSceneNode(&camera,pNode,m_p->m_pDiffuseRT,true,0,0,this);
		}
		else
		{
			RenderNode::m_pSpecialVS=m_p->m_pTrnNormalVS;
			RenderNode::m_pSpecialPS=m_p->m_pTrnNormalPS;
			pRenderSys->RenderSceneNode(&camera,pNode,m_p->m_pDiffuseRT,true,0,0,NULL);
		}

		//--
		pRenderTex->StrechRect(m_p->m_pDiffuseRT,NULL,NULL,ETFilter_NONE);
		m_p->m_pBlur->Diffuse(pRenderTex,1);

		// 将显存纹理拷到内存纹理，然后交给线程写盘
		pSurface = ((DX9RenderTex*)pRenderTex)->GetDX9Surface();
		pDestSurface = NULL;
		pTempTex = NULL;
		while( TRUE )
		{
			HRESULT hr = D3DXCreateTexture(DX9Dev(), lightMapSize, lightMapSize, 1, 0, D3DFMT_A32B32G32R32F, D3DPOOL_SYSTEMMEM, &pTempTex);
			if( D3D_OK == hr)
				break;

			Sleep(1000);
		}
		pTempTex->GetSurfaceLevel(0, &pDestSurface);
		DX9Dev()->GetRenderTargetData(pSurface, pDestSurface);
		SAFE_RELEASE(pDestSurface);
		pData = new tagThreadData;
		pData->pTexture = pTempTex;
		pData->strName = MakeNormalMapName(pNode->GetLightmap());
		m_listRT.PushBack(pData);
//		pRenderTex->WriteToFile(szNormalMap.c_str());

		//--
		block.Apply();
		RenderNode::m_pSpecialVS=NULL;
		RenderNode::m_pSpecialPS=NULL;
		pDev->SetVertShader(NULL);
		pDev->SetPixelShader(NULL);
		SAFE_DELETE(pRenderTex);

		//--
		m_pUI->SetProgress(index+1,totalNum);

		while(m_listRT.Size()>50)
			Sleep(1000);	// 避免内存上涨过快

		// 显示调试信息
		// m_pUI->AddMsg(_T("index=%d, time=%d, list:%d\r\n"), index, timeGetTime() - dwTime, m_listRT.Size());
		return true;
	}

	void LightMapTempGen::OnRenderBefore(SceneNode* pSGNode,RenderNode* pRenderNode)
	{
		if( m_p->m_isPos == true )
		{
			if( pSGNode->IS_STATIC_CLASS(EntityNode) )
			{
				float mtlID=((float)pRenderNode->GetMtlID()+1.0f)/(float)pSGNode->GetNumMtl();
				m_p->m_pMdlPosPS->SetFloat("g_fMtlID",mtlID);
			}
			else
				m_p->m_pTrnPosPS->SetFloat("g_fMtlID",1.0f);
		}
		else
		{
			IMaterial* pMtl=pSGNode->GetMtl(pRenderNode->GetMtlID());
			float alpha=1.0f;
			if(pMtl!=NULL
				&&pMtl->IS_KIND_OF(MGeneral))
			{
				MGeneral* pMG=(MGeneral*)pMtl;
				if(pMG->m_pDiffuse->m_bTwoSide)
					alpha=2.0f;
			}
			m_p->m_pMdlNormalPS->SetFloat("g_normalMapAlpha",alpha);
		}
	}

	UINT WINAPI ThreadLightMapTempGenWriteFile(LPVOID pParam);
	bool LightMapTempGen::Start(const TCHAR* szSGPath)
	{
		m_pUI->SetProgress(0,1);

		ResMgr::Inst()->EnableIOAsync(false);

		f_bTerminateThreadLightMapTempGenWriteFile = FALSE;
		HANDLE hHandle = (HANDLE)_beginthreadex(NULL, 0, ThreadLightMapTempGenWriteFile, &m_listRT, 0, NULL);

		SGVisitor visitor;
		bool bResult = visitor.Start(szSGPath,128.0f*50.0f,512.0f*50.0f,this,0);

		// 等待写盘完成
		f_bTerminateThreadLightMapTempGenWriteFile = TRUE;
		::WaitForSingleObject( hHandle, INFINITE );
		return bResult;
	}

	void LightMapTempGen::DeleteTempFiles(const TCHAR* szPath)
	{
		TCHAR szFind[512];
		_sntprintf(szFind,512,_T("%s\\*.tmp"),szPath);

		DeleteFiles(szFind);
	}

	tstring LightMapTempGen::MakePosMapName(const TCHAR* szLightMapName)
	{
		TCHAR szPath[512];
		TCHAR szName[512];
		_tsplitpath(szLightMapName,NULL,szPath,szName,NULL);
		
		TCHAR szOut[512];
		_sntprintf(szOut,512,_T("%s%s_pos.tmp"),szPath,szName);

		return tstring(szOut);
	}

	tstring LightMapTempGen::MakeNormalMapName(const TCHAR* szLightMapName)
	{
		TCHAR szPath[512];
		TCHAR szName[512];
		_tsplitpath(szLightMapName,NULL,szPath,szName,NULL);

		TCHAR szOut[512];
		_sntprintf(szOut,512,_T("%s%s_normal.tmp"),szPath,szName);

		return tstring(szOut);
	}

	UINT WINAPI ThreadLightMapTempGenWriteFile(LPVOID pParam)
	{
		TSafeList<LightMapTempGen::tagThreadData*>* pList = (TSafeList<LightMapTempGen::tagThreadData*>*)pParam;

		while( !f_bTerminateThreadLightMapTempGenWriteFile || pList->Size() > 0 )
		{
			while( pList->Size() > 0 )
			{
				LightMapTempGen::tagThreadData* pData = pList->PopFront();
				IDirect3DTexture9* pRenderTexture = pData->pTexture;
				IDirect3DSurface9* pSurface = NULL;
				pRenderTexture->GetSurfaceLevel(0, &pSurface);


				while(TRUE)
				{
					if( D3D_OK == D3DXSaveSurfaceToFile(pData->strName.c_str(), D3DXIFF_DDS, pSurface, NULL, NULL) )
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