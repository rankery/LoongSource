#include "StdAfx.h"
#include ".\Radiosity.h"
#include "..\Device\IRenderToEnvMap.h"
#include "..\Device\IRenderTexture.h"
#include "..\Device\IDeviceTex.h"
#include "..\Device\IVertShader.h"
#include "..\Device\IPixelShader.h"
#include "..\Device\StateBlock.h"
#include "..\Device\IVertBuffer.h"
#include "..\Device\IIndexBuffer.h"
#include ".\LightMapTempGen.h"
#include ".\SGVisitor.h"
#include ".\IUICallBack.h"
#include "..\ResSys\ResMgr.h"
#include "..\Kernel.h"
#include "..\Cool3DOption.h"
#include "..\Util\DebugMisc.h"
#include "..\SceneGraph\HugeQuadTreeSG.h"
#include "..\SceneGraph\SceneNode.h"
#include "..\SceneGraph\EntityNode.h"
#include "..\RenderSys\RenderSys.h"
#include "..\ResSys\VertStream.h"
#include "..\Terrain\TerrainPatch.h"
#include ".\LightMapBlur.h"

#include <process.h>	// for _beginthreadex
#include "..\Device\DX9Device\DX9RenderTex.h"
#include "..\Device\DX9Device\DX9device.h"
using namespace Detail;
namespace Cool3D
{
	BOOL f_bTerminateThreadRadiosityWriteFile = FALSE;
	const float ViewDist=24.0f*50.0f;//鱼眼图拍摄视野距离

	//--class Radiosity::Member----------------------------
	class Radiosity::Member
	{
	public:
		struct tagLightMapName
		{
			tstring szLightMap;
			tstring szTempFile;
		};
	public:
		IRenderTexture*		m_pSphereMap;
		IRenderTexture*		m_pMulRT;
		IRenderToEnvMap*	m_pRenderToEnvMap;
		IDeviceTex*			m_pAttenTex;
		IVertShader*		m_pMulVS;
		IPixelShader*		m_pMulPS;
		IVertShader*		m_pTexelLerpVS;
		IPixelShader*		m_pTexelLerpPS;
		IVertShader*		m_pAddVS;
		IPixelShader*		m_pAddPS;
		IRenderTexture*		m_pDiffuseRT;
		LightMapBlur*		m_pBlur;
		vector<tagLightMapName> m_lightMapName;
		TSafeList<tagThreadData*>	m_listRT;
	public:
		Member()
		{
			IDevice* pDev=Device();
			m_pMulVS=pDev->NewVS();
			m_pMulVS->CreateFromFile(_T("data\\system\\radiosity\\mul.vs"),"vs_2_0","main");
			m_pMulPS=pDev->NewPS();
			m_pMulPS->CreateFromFile(_T("data\\system\\radiosity\\mul.ps"),"ps_2_0","main");

			m_pTexelLerpVS=pDev->NewVS();
			m_pTexelLerpVS->CreateFromFile(_T("data\\system\\radiosity\\texellerp.vs"),"vs_2_0","main");
			m_pTexelLerpPS=pDev->NewPS();
			m_pTexelLerpPS->CreateFromFile(_T("data\\system\\radiosity\\texellerp.ps"),"ps_2_0","main");

			m_pAddVS=pDev->NewVS();
			m_pAddVS->CreateFromFile(_T("data\\system\\radiosity\\add.vs"),"vs_2_0","main");
			m_pAddPS=pDev->NewPS();
			m_pAddPS->CreateFromFile(_T("data\\system\\radiosity\\add.ps"),"ps_2_0","main");

			m_pAttenTex=pDev->NewTexture();
			m_pAttenTex->LoadFromFile(Kernel::Inst()->DefaultFS(),
				_T("data\\system\\radiosity\\atten.tga"),1);

			m_pSphereMap=pDev->NewRenderTexture();
			m_pSphereMap->Create(64,64,EPF_A8R8G8B8,true,true,1);

			m_pRenderToEnvMap=pDev->NewRenderToEnvMap();
			m_pRenderToEnvMap->Create(64,EPF_A8R8G8B8,true);

			m_pMulRT=pDev->NewRenderTexture();
			m_pMulRT->Create(16,16,EPF_A8R8G8B8,false,true,1);

			m_pDiffuseRT=pDev->NewRenderTexture();
			m_pDiffuseRT->Create(1024,1024,EPF_A8R8G8B8,false,false,1);

			m_pBlur=new LightMapBlur(NULL);
		}

		~Member()
		{
			SAFE_DELETE(m_pMulVS);
			SAFE_DELETE(m_pMulPS);
			SAFE_DELETE(m_pTexelLerpVS);
			SAFE_DELETE(m_pTexelLerpPS);
			SAFE_DELETE(m_pAddVS);
			SAFE_DELETE(m_pAddPS);
			SAFE_DELETE(m_pAttenTex);
			SAFE_DELETE(m_pSphereMap);
			SAFE_DELETE(m_pRenderToEnvMap);
			SAFE_DELETE(m_pMulRT);
			SAFE_DELETE(m_pDiffuseRT);
			SAFE_DELETE(m_pBlur);
		}

		Color4ub CalcRadiosity(HugeQuadTreeSG* pSG,Vector3& eye,Vector3& dir)
		{
			//--拍摄鱼眼图
			pSG->RenderHemisphere(eye,dir,ViewDist,m_pSphereMap,m_pRenderToEnvMap);
			//m_pSphereMap->WriteToFile(_T("d:\\spheremap.tga"));

			/*
			//--鱼眼图和衰减图相乘
			IDevice* pDev=Device();

			StateBlock block;
			block.SaveState(ERS_DepthWriteEnable);	pDev->EnableDepthWrite(false);
			block.SaveState(ERS_DepthTestEnable);	pDev->EnableDepthTest(false);
			block.SaveState(ERS_CullMode);			pDev->SetCullMode(ECull_None);

			pDev->SetTexture(0,m_pSphereMap);
			pDev->SetSimplerAddress(0,ETAddress_Clamp,ETAddress_Clamp,ETAddress_Clamp);
			pDev->SetSimplerFilter(0,ETFilter_POINT,ETFilter_POINT);
			pDev->SetSimplerMipFilter(0,ETFilter_NONE);

			pDev->SetTexture(1,m_pAttenTex);
			pDev->SetSimplerAddress(1,ETAddress_Clamp,ETAddress_Clamp,ETAddress_Clamp);
			pDev->SetSimplerFilter(1,ETFilter_POINT,ETFilter_POINT);
			pDev->SetSimplerMipFilter(1,ETFilter_NONE);

			pDev->SetVertShader(m_pMulVS);
			pDev->SetPixelShader(m_pMulPS);

			RenderSys* pSys=Kernel::Inst()->GetRenderSys();
			pDev->SetStreamDefine(pSys->GetStreamDefine(EVType_P));

			m_pMulRT->Begin();

			/*
			0____1
			|    |
			3____2
			*/
			/*Vert_P vert[4];
			vert[0].pos=Vector3(0,		0,		0);
			vert[1].pos=Vector3(1.0f,	0,		0);
			vert[2].pos=Vector3(1.0f,	1.0f,	0);
			vert[3].pos=Vector3(0,		1.0f,	0);
			pDev->DrawMem(EDraw_TriangleFan,2,&vert[0],sizeof(Vert_P));

			m_pMulRT->End();
			block.Apply();
			pDev->SetVertShader(NULL);
			pDev->SetPixelShader(NULL);
			pDev->SetTexture(0,(IDeviceTex*)NULL);
			pDev->SetTexture(1,(IDeviceTex*)NULL);

			//m_pMulRT->WriteToFile(_T("d:\\mul.tga"));
			*/

			//--统计相乘纹理平均像素值
			UINT width = m_pSphereMap->Width(), height = m_pSphereMap->Height();
			DWORD dwR =0, dwG = 0, dwB = 0;
			tagLockedRect lock;
			m_pSphereMap->LockRect(&lock,NULL);
			
			BYTE* pBytes=(BYTE*)lock.pBits;
			DWORD dwColor = 0;

			for(UINT y=0;y<height;y++)
			{
				for(UINT x=0;x<width;x++)
				{
					dwColor=*((DWORD*)&pBytes[y*lock.pitch+x*4]);
					dwR += (dwColor>>16)&0x000000FF;
					dwG += (dwColor>>8)&0x000000FF;
					dwB += (dwColor)&0x000000FF;
				}
			}
			m_pSphereMap->UnlockRect();
			dwR /= m_pSphereMap->Width()*m_pSphereMap->Height();
			dwG /= m_pSphereMap->Width()*m_pSphereMap->Height();
			dwB /= m_pSphereMap->Width()*m_pSphereMap->Height();

			// 衰减
			dwR /= 4;
			dwG /= 4;
			dwB /= 4;

			return Color4ub(255, (BYTE)dwR, (BYTE)dwG, (BYTE)dwB);
		}

		tstring MakeTempLightMapName(const TCHAR* szLightMap)
		{
			TCHAR szPath[512];
			TCHAR szName[512];
			_tsplitpath(szLightMap,NULL,szPath,szName,NULL);

			TCHAR szOut[512];
			_sntprintf(szOut,512,_T("%s%s_temp.tga"),szPath,szName);

			return tstring(szOut);
		}

		bool ReplaceLightMap()
		{
			for(int i=0;i<(int)m_lightMapName.size();i++)
			{
				tagLightMapName& name=m_lightMapName[i];
				if(DeleteFile(name.szLightMap.c_str())==FALSE)
					return false;
				if(MoveFile(name.szTempFile.c_str(),name.szLightMap.c_str())==FALSE)
					return false;
			}
			return true;
		}

		void GetVB(SceneNode* pNode,VertStream& vb)
		{
			if(pNode->IS_STATIC_CLASS(TerrainPatch))
			{
				TerrainPatch* pPatch=(TerrainPatch*)pNode;
				pPatch->GetVertexBuffer(&vb);
			}
			else if(pNode->IS_STATIC_CLASS(EntityNode))
			{
				EntityNode* pEntity=(EntityNode*)pNode;
				pEntity->GetVertexBuffer(&vb);
			}
			else
				ASSERT(0);
		}

		void GetIB(SceneNode* pNode,vector<WORD>& ib)
		{
			if(pNode->IS_STATIC_CLASS(TerrainPatch))
			{
				TerrainPatch* pPatch=(TerrainPatch*)pNode;
				pPatch->GetIndexBuffer(ib);
			}
			else if(pNode->IS_STATIC_CLASS(EntityNode))
			{
				EntityNode* pEntity=(EntityNode*)pNode;
				pEntity->GetIndexBuffer(ib);
			}
			else
				ASSERT(0);
		}

		IDeviceTex* LoadLightMap(const TCHAR* szLightMap)
		{
			IDeviceTex* pTex=Device()->NewTexture();
			IFS* pFS=Kernel::Inst()->DefaultFS();
			pTex->LoadFromFile(pFS,szLightMap,1,false,false);
			return pTex;
		}

		bool RadiosityPerVertex(HugeQuadTreeSG* pSG,SceneNode* pNode,const TCHAR* szTempLightMap)
		{
			VertStream vb;
			GetVB(pNode,vb);
			vector<WORD> ib;
			GetIB(pNode,ib);

			//--计算顶点radiosity
			ASSERT(vb.GetType()==EVType_PNDT);
			Vert_PNDT* pVert=(Vert_PNDT*)vb.GetRawBuffer();
			for(UINT i=0;i<vb.GetNumVert();i++)
			{
				pVert->diffuse=CalcRadiosity(pSG,pVert->pos,pVert->normal);
				pVert++;
			}

			//--插值每个texel的radiosity
			IDevice* pDev=Device();

			StateBlock block;
			block.SaveState(ERS_DepthTestEnable);	pDev->SetRenderState(ERS_DepthTestEnable,false);
			block.SaveState(ERS_DepthWriteEnable);	pDev->SetRenderState(ERS_DepthWriteEnable,false);
			block.SaveState(ERS_CullMode);			pDev->SetCullMode(ECull_None);

			RenderSys* pSys=Kernel::Inst()->GetRenderSys();
			IVertBuffer* pVB=pSys->AllocVB(vb.GetBufferSize());
			pVB->CopyFromVertStream(&vb);
			
			IIndexBuffer* pIB=pSys->AllocIB(ib.size()*sizeof(WORD));
			pIB->CopyFromMem(&ib[0],ib.size()*sizeof(WORD));

			pDev->SetStreamDefine(pSys->GetStreamDefine(EVType_PNDT));
			pDev->SetVertShader(m_pTexelLerpVS);
			pDev->SetPixelShader(m_pTexelLerpPS);
			
			pDev->SetVB(0,pVB);
			pDev->SetIB(pIB);

			m_pDiffuseRT->Begin();
			pDev->Clear(true,false,false,0,1.0f,0);
			pDev->DrawIndexed(EDraw_TriangleList,0,vb.GetNumVert(),ib.size()/3);
			m_pDiffuseRT->End();

			block.Apply();
			pSys->FreeVB(pVB);
			pSys->FreeIB(pIB);
			pDev->SetVertShader(NULL);
			pDev->SetPixelShader(NULL);

			//--扩散
			if(pNode->IS_STATIC_CLASS(EntityNode))
				m_pBlur->Diffuse(m_pDiffuseRT,1);
            
			//--叠加到Lightmap
			AddToLightMap(pNode->GetLightmap(),szTempLightMap,pNode);
			
			return true;
		}

		void AddToLightMap(const TCHAR* szLightMap,const TCHAR* szTempFile,SceneNode* pNode)
		{
			IDevice* pDev=Device();
			IDeviceTex* pLightMap=LoadLightMap(szLightMap);

			EPixelFormat fmt;
			if(pNode->IS_STATIC_CLASS(TerrainPatch))
				fmt=EPF_A8R8G8B8;
			else
				fmt=EPF_X8R8G8B8;

			IRenderTexture* pRenderTex=pDev->NewRenderTexture();
			pRenderTex->Create(pLightMap->Width(),pLightMap->Height(),fmt,false,false,1);

			IRenderTexture* pRadiosityTex=pDev->NewRenderTexture();
			pRadiosityTex->Create(pLightMap->Width(),pLightMap->Height(),fmt,false,false,1);
			pRadiosityTex->StrechRect(m_pDiffuseRT,NULL,NULL,ETFilter_LINEAR);

			StateBlock block;
			block.SaveState(ERS_DepthWriteEnable);	pDev->EnableDepthWrite(false);
			block.SaveState(ERS_DepthTestEnable);	pDev->EnableDepthTest(false);
			block.SaveState(ERS_CullMode);			pDev->SetCullMode(ECull_None);

			pDev->SetTexture(0,pLightMap);
			pDev->SetSimplerAddress(0,ETAddress_Clamp,ETAddress_Clamp,ETAddress_Clamp);
			pDev->SetSimplerFilter(0,ETFilter_POINT,ETFilter_POINT);
			pDev->SetSimplerMipFilter(0,ETFilter_NONE);

			pDev->SetTexture(1,pRadiosityTex);
			pDev->SetSimplerAddress(1,ETAddress_Clamp,ETAddress_Clamp,ETAddress_Clamp);
			pDev->SetSimplerFilter(1,ETFilter_POINT,ETFilter_POINT);
			pDev->SetSimplerMipFilter(1,ETFilter_NONE);

			pDev->SetVertShader(m_pAddVS);
			pDev->SetPixelShader(m_pAddPS);

			RenderSys* pSys=Kernel::Inst()->GetRenderSys();
			pDev->SetStreamDefine(pSys->GetStreamDefine(EVType_P));

			pRenderTex->Begin();

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
			pDev->DrawMem(EDraw_TriangleFan,2,&vert[0],sizeof(Vert_P));

			pRenderTex->End();

			block.Apply();
			pDev->SetVertShader(NULL);
			pDev->SetPixelShader(NULL);
			pDev->SetTexture(0,(IDeviceTex*)NULL);
			pDev->SetTexture(1,(IDeviceTex*)NULL);

			// 将显存纹理拷到内存纹理，然后交给线程写盘
			IDirect3DSurface9* pSurface = ((DX9RenderTex*)pRenderTex)->GetDX9Surface();
			IDirect3DSurface9* pDestSurface = NULL;
			IDirect3DTexture9* pTempTex = NULL;
			HRESULT hr = 0;
			while( TRUE )
			{
				if(pNode->IS_STATIC_CLASS(TerrainPatch))
					hr = D3DXCreateTexture(DX9Dev(), pLightMap->Width(),pLightMap->Height(), 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pTempTex);
				else
					hr = D3DXCreateTexture(DX9Dev(), pLightMap->Width(),pLightMap->Height(), 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &pTempTex);

				if( D3D_OK == hr)
					break;

				Sleep(1000);
			}
			pTempTex->GetSurfaceLevel(0, &pDestSurface);
			DX9Dev()->GetRenderTargetData(pSurface, pDestSurface);
			SAFE_RELEASE(pDestSurface);
			tagThreadData* pData = new tagThreadData;
			pData->pTexture = pTempTex;
			pData->strName = szTempFile;
			m_listRT.PushBack(pData);
			// pRenderTex->WriteToFile(szTempFile);

			//--
			SAFE_DELETE(pRenderTex);
			SAFE_DELETE(pLightMap);
			SAFE_DELETE(pRadiosityTex);
		}

		bool RadiosityPerTexel(HugeQuadTreeSG* pSG,SceneNode* pNode,const TCHAR* szTempLightMap)
		{
			Image posMap;
			tstring szPosMap=LightMapTempGen::MakePosMapName(pNode->GetLightmap());
			posMap.LoadFromFile(szPosMap.c_str());

			Image normalMap;
			tstring szNormalMap=LightMapTempGen::MakeNormalMapName(pNode->GetLightmap());
			normalMap.LoadFromFile(szNormalMap.c_str());

			Image lightMap;
			lightMap.LoadFromFile(pNode->GetLightmap());
			if(!lightMap.IsReversed())
				lightMap.VFlip();

			ASSERT(posMap.Width()==normalMap.Width());
			ASSERT(posMap.Height()==normalMap.Height());
			ASSERT(lightMap.Width()==posMap.Width());
			ASSERT(lightMap.Height()==posMap.Height());

			UINT uByte = lightMap.GetBpp() / 8;
			UINT width = posMap.Width();
			UINT height = posMap.Height();
			UINT step = 4;

			Image radiosityMap;
			radiosityMap.Create(width/step, height/step, EPF_R8G8B8);

			Vector4* pPosMap=(Vector4*)posMap.GetRawBuffer();
			Vector4* pNormalMap=(Vector4*)normalMap.GetRawBuffer();
			tagBGR* pLightMap=(tagBGR*)lightMap.GetRawBuffer();
			tagBGR* pRadiosityMap=(tagBGR*)radiosityMap.GetRawBuffer();
			Vector4* p=0;
			Vector4* n=0;
			tagBGR* l=0;
			tagBGR* r=0;

			// 按照指定倍数减少拍摄鱼眼图次数
			for(UINT y=0;y<height;y+=step)
			{
				for(UINT x=0;x<width;x+=step)
				{
					n = pNormalMap + x + y*width;
					//l = (tagBGR*)(((LPBYTE)pLightMap)+(uByte*x + uByte*y*width));
					//INT u = (INT)l->B + (INT)l->G + (INT)l->R;
					if( n->w != 0 )
					{
						r = pRadiosityMap + x/step + y/step*width/step;
						p = pPosMap + x + y*width;
						Vector3 eye(p->x,p->y,p->z);
						Vector3 dir(n->x,n->y,n->z);
						Color4ub color = CalcRadiosity(pSG,eye,dir);

						r->R=color.R;
						r->G=color.G;
						r->B=color.B;
					}
					else
					{
						r = pRadiosityMap + x/step + y/step*width/step;
						r->R=0;
						r->G=0;
						r->B=0;
					}
				}
			}

			// 放大一倍
			UINT nNewStep = step/2;
			for(nNewStep = step/2; nNewStep>=1; nNewStep/=2)
			{
				radiosityMap.Scale(width/nNewStep, height/nNewStep,EIS_BestQuality);
				pRadiosityMap=(tagBGR*)radiosityMap.GetRawBuffer();

				// 中心
				for(UINT y=1;y<height/nNewStep;y+=2)
				{
					for(UINT x=1;x<width/nNewStep;x+=2)
					{
						n = pNormalMap + x*nNewStep + y*nNewStep*width;
						if( n->w != 0 )
						{
							tagBGR* r_left = pRadiosityMap + (x-1) + y*(width/nNewStep);
							tagBGR* r_right = pRadiosityMap + (x+1) + y*(width/nNewStep);
							tagBGR* r_top = pRadiosityMap + x + (y-1)*(width/nNewStep);
							tagBGR* r_bottom = pRadiosityMap + x + (y+1)*(width/nNewStep);

							INT ul = (INT)r_left->B + (INT)r_left->G + (INT)r_left->R;
							INT ur = (INT)r_right->B + (INT)r_right->G + (INT)r_right->R;
							INT ut = (INT)r_top->B + (INT)r_top->G + (INT)r_top->R;
							INT ub = (INT)r_bottom->B + (INT)r_bottom->G + (INT)r_bottom->R;
							if( abs(ul - ur) > 100 || abs(ut - ub) > 100 )
							{
								r = pRadiosityMap + x + y*(width/nNewStep);
								p = pPosMap + x*nNewStep + y*nNewStep*width;
								Vector3 eye(p->x,p->y,p->z);
								Vector3 dir(n->x,n->y,n->z);
								Color4ub color = CalcRadiosity(pSG,eye,dir);

								r->R=color.R;
								r->G=color.G;
								r->B=color.B;
							}
						}
						else
						{
							r = pRadiosityMap + x + y*(width/nNewStep);
							r->R=0;
							r->G=0;
							r->B=0;
						}
					}
				}
			}

			// 放大到lightmap相同大小
			//radiosityMap.Scale(width, height,EIS_BestQuality);
			
			pRadiosityMap=(tagBGR*)radiosityMap.GetRawBuffer();
			// 合成到lightmap
			for(UINT y=0;y<height;y++)
			{
				for(UINT x=0;x<width;x++)
				{
					l = (tagBGR*)(((LPBYTE)pLightMap)+(uByte*x + uByte*y*width));
					r = pRadiosityMap + x + y*width;

					l->R = r->R + l->R > 255 ? 255 : r->R + l->R;
					l->G = r->G + l->G > 255 ? 255 : r->G + l->G;
					l->B = r->B + l->B > 255 ? 255 : r->B + l->B;
				}
			}

			if(!lightMap.IsReversed())
				lightMap.VFlip();
			lightMap.WriteToFile(szTempLightMap);

			return true;
		}
	};

	//--class Radiosity------------------------------------
	Radiosity::Radiosity(IUICallBack* pUI):m_pUI(pUI)
	{
		m_p=new Member;
	}

	Radiosity::~Radiosity(void)
	{
		delete m_p;
	}

	bool Radiosity::OnVisitSceneNode(HugeQuadTreeSG* pSG,SceneNode* pNode,int index,int totalNum,DWORD param)
	{
		if(_tcslen(pNode->GetLightmap())==0)
			return true;

		DWORD dwTime = timeGetTime();
		Member::tagLightMapName name;
		name.szLightMap=pNode->GetLightmap();
		name.szTempFile=m_p->MakeTempLightMapName(pNode->GetLightmap());
		m_p->m_lightMapName.push_back(name);

		
		if(!m_p->RadiosityPerTexel(pSG,pNode,name.szTempFile.c_str()))
			return false;

		m_pUI->SetProgress(index+1,totalNum);

		// 显示调试信息
		m_pUI->AddMsg(_T("index=%d, time=%d, list:%d\r\n"), index, timeGetTime() - dwTime, m_p->m_listRT.Size());
		return true;
	}

	void Radiosity::OnSceneGraphLoad(HugeQuadTreeSG* pSG)
	{
	}

	UINT WINAPI ThreadRadiosityWriteFile(LPVOID pParam);
	bool Radiosity::Start(const TCHAR* szSGPath)
	{
		m_pUI->SetProgress(0,1);

		ResMgr::Inst()->EnableIOAsync(false);
		ResMgr::Inst()->DoGC();

		f_bTerminateThreadRadiosityWriteFile = FALSE;
		HANDLE hHandle = (HANDLE)_beginthreadex(NULL, 0, ThreadRadiosityWriteFile, &m_p->m_listRT, 0, NULL);

		m_p->m_lightMapName.clear();

		Cool3DOption::Inst()->SetOption(Cool3DOption::EO_LightMap,Cool3DOption::ELM_Open);
		Cool3DOption::Inst()->SetOption(Cool3DOption::EO_Highlight,Cool3DOption::EHL_Open);

		SGVisitor visitor;
		bool ok=visitor.Start(szSGPath,128.0f*50.0f,ViewDist,this,0);

		Cool3DOption::Inst()->SetOption(Cool3DOption::EO_LightMap,Cool3DOption::ELM_None);
		Cool3DOption::Inst()->SetOption(Cool3DOption::EO_Highlight,Cool3DOption::EHL_None);

		// 等待写盘完成
		f_bTerminateThreadRadiosityWriteFile = TRUE;
		::WaitForSingleObject( hHandle, INFINITE );

		if(!ok)
			return false;

		//--将临时lightmap替换lightmap
		if(!m_p->ReplaceLightMap())
			return false;

		return true;
	}

	UINT WINAPI ThreadRadiosityWriteFile(LPVOID pParam)
	{
		TSafeList<Radiosity::tagThreadData*>* pList = (TSafeList<Radiosity::tagThreadData*>*)pParam;

		while( !f_bTerminateThreadRadiosityWriteFile || pList->Size() > 0 )
		{
			while( pList->Size() > 0 )
			{
				Radiosity::tagThreadData* pData = pList->PopFront();
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