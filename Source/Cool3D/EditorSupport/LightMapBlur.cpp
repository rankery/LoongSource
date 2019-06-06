#include "StdAfx.h"
#include ".\LightMapBlur.h"
#include ".\LightMapTempGen.h"
#include "..\Image\Image.h"
#include ".\IUICallBack.h"
#include "..\Device\RenderTexChain.h"
#include "..\Device\IRenderTexture.h"
#include "..\Device\IVertShader.h"
#include "..\Device\IPixelShader.h"
#include "..\Device\IDeviceTex.h"
#include "..\Device\StateBlock.h"
#include ".\IUICallBack.h"
#include "..\Kernel.h"
#include "..\Util\DebugMisc.h"
#include "..\Util\FileUtil.h"
#include "..\RenderSys\RenderSys.h"
#include "..\Util\Filename.h"
#include "..\EditorSupport\TerrainEditor.h"


#include <process.h>	// for _beginthreadex
#include "..\Device\DX9Device\DX9RenderTex.h"
#include "..\Device\DX9Device\DX9device.h"
using namespace Detail;
namespace Cool3D
{
	BOOL f_bTerminateThreadLightMapBlurWriteFile = FALSE;

	//--class LightMapBlur::Member----------------------------
	class LightMapBlur::Member
	{
	public:
		IVertShader*	m_pBlurVS;
		IPixelShader*	m_pBlurPS;
		IVertShader*	m_pDiffuseVS;
		IPixelShader*	m_pDiffusePS;
		TSafeList<tagThreadData*>	m_listRT;
		
	public:
		Member()
		{
			IDevice* pDev=Device();
			m_pBlurVS=pDev->NewVS();
			m_pBlurVS->CreateFromFile(_T("data\\system\\radiosity\\blur.vs"),"vs_2_0","main");
			m_pBlurPS=pDev->NewPS();
			m_pBlurPS->CreateFromFile(_T("data\\system\\radiosity\\blur.ps"),"ps_2_0","main");
			m_pDiffuseVS=pDev->NewVS();
			m_pDiffuseVS->CreateFromFile(_T("data\\system\\radiosity\\diffuse.vs"),"vs_2_0","main");
			m_pDiffusePS=pDev->NewPS();
			m_pDiffusePS->CreateFromFile(_T("data\\system\\radiosity\\diffuse.ps"),"ps_2_0","main");
		}

		~Member()
		{
			SAFE_DELETE(m_pBlurVS);
			SAFE_DELETE(m_pBlurPS);
			SAFE_DELETE(m_pDiffuseVS);
			SAFE_DELETE(m_pDiffusePS);
		}

		IDeviceTex* LoadLightMap(const TCHAR* szLightMap)
		{
			IDeviceTex* pTex=Device()->NewTexture();
			IFS* pFS=Kernel::Inst()->DefaultFS();
			pTex->LoadFromFile(pFS,szLightMap,1,false,false);
			return pTex;
		}

		IDeviceTex* LoadUVMap(const TCHAR* szUVMap)
		{
			IDeviceTex* pTex=Device()->NewTexture();
			IFS* pFS=Kernel::Inst()->DefaultFS();
			pTex->LoadFromFile(pFS,szUVMap,1,false,false);
			return pTex;
		}

		void SearchLightMap(vector<tstring>& out,const TCHAR* szPath)
		{
			TCHAR szFind[512];
			_sntprintf(szFind,512,_T("%s\\*.tga"),szPath);

			SearchFiles(szFind,out);
		}

		void BlurLightMap(const TCHAR* szLightMap,int blurTimes)
		{
			IDeviceTex* pLightMap=LoadLightMap(szLightMap);

			//--把normalmap作为uvmap
			Filename normalFile(szLightMap);
			tstring strUVMap=normalFile.NoExtension() + _T("_normal.tmp");
			IDeviceTex* pUVMap=LoadUVMap(strUVMap.c_str());
			
			EPixelFormat fmt;
			if(pLightMap->Format()==EPF_A8R8G8B8)
				fmt=EPF_A8R8G8B8;
			else
				fmt=EPF_X8R8G8B8;

			IDevice* pDev=Device();
			RenderTexChain rtChain;
			rtChain.Create(pLightMap->Width(),pLightMap->Height(),fmt,false);
			rtChain.Src()->StrechRect(pLightMap,NULL,NULL,ETFilter_POINT);

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

			StateBlock block;
			block.SaveState(ERS_DepthWriteEnable);	pDev->EnableDepthWrite(false);
			block.SaveState(ERS_DepthTestEnable);	pDev->EnableDepthTest(false);
			block.SaveState(ERS_CullMode);		pDev->SetCullMode(ECull_None);

			Vector2 lightMapSize((float)pLightMap->Width(),(float)pLightMap->Height());
			m_pBlurPS->SetConstValue("g_lightMapSize",&lightMapSize,sizeof(lightMapSize));

			pDev->SetVertShader(m_pBlurVS);
			pDev->SetPixelShader(m_pBlurPS);

			RenderSys* pSys=Kernel::Inst()->GetRenderSys();
			pDev->SetStreamDefine(pSys->GetStreamDefine(EVType_P));

			for(int i=0;i<blurTimes;i++)
			{
				rtChain.Dest()->Begin();
				pDev->Clear(true,false,false,0,1.0f,0);

				pDev->SetTexture(0,rtChain.Src());
				pDev->SetSimplerAddress(0,ETAddress_Clamp,ETAddress_Clamp,ETAddress_Clamp);
				pDev->SetSimplerFilter(0,ETFilter_POINT,ETFilter_POINT);
				pDev->SetSimplerMipFilter(0,ETFilter_NONE);

				pDev->SetTexture(1,pUVMap);
				pDev->SetSimplerAddress(1,ETAddress_Clamp,ETAddress_Clamp,ETAddress_Clamp);
				pDev->SetSimplerFilter(1,ETFilter_POINT,ETFilter_POINT);
				pDev->SetSimplerMipFilter(1,ETFilter_NONE);

				pDev->DrawMem(EDraw_TriangleFan,2,&vert[0],sizeof(Vert_P));
				rtChain.Dest()->End();

				rtChain.Flip();
			}

			// 将显存纹理拷到内存纹理，然后交给线程写盘
			IDirect3DSurface9* pSurface = ((DX9RenderTex*)rtChain.Src())->GetDX9Surface();
			IDirect3DSurface9* pDestSurface = NULL;
			IDirect3DTexture9* pTempTex = NULL;
			HRESULT hr = 0;
			while( TRUE )
			{
				if(pLightMap->Format()==EPF_A8R8G8B8)
					hr = D3DXCreateTexture(DX9Dev(), pLightMap->Width(), pLightMap->Height(), 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pTempTex);
				else
					hr = D3DXCreateTexture(DX9Dev(), pLightMap->Width(), pLightMap->Height(), 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &pTempTex);

				if( D3D_OK == hr)
					break;

				Sleep(1000);
			}
			pTempTex->GetSurfaceLevel(0, &pDestSurface);
			DX9Dev()->GetRenderTargetData(pSurface, pDestSurface);
			SAFE_RELEASE(pDestSurface);
			tagThreadData* pData = new tagThreadData;
			pData->pTexture = pTempTex;
			pData->strName = szLightMap;
			m_listRT.PushBack(pData);
			//rtChain.Src()->WriteToFile(szLightMap);

			//--
			block.Apply();
			SAFE_DELETE(pLightMap);
			SAFE_DELETE(pUVMap);
			pDev->SetTexture(0,(IDeviceTex*)NULL);
			pDev->SetTexture(1,(IDeviceTex*)NULL);
			pDev->SetVertShader(NULL);
			pDev->SetPixelShader(NULL);

			while(m_listRT.Size()>50)
				Sleep(1000);	// 避免内存上涨过快
		}

		void Diffuse(IRenderTexture* pTex,int diffuseTimes)
		{
			IDevice* pDev=Device();
			RenderTexChain rtChain;
			rtChain.Create(pTex->Width(),pTex->Height(),pTex->Format(),false);

			rtChain.Src()->StrechRect(pTex,NULL,NULL,ETFilter_POINT);

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

			StateBlock block;
			block.SaveState(ERS_DepthWriteEnable);	pDev->EnableDepthWrite(false);
			block.SaveState(ERS_DepthTestEnable);	pDev->EnableDepthTest(false);
			block.SaveState(ERS_CullMode);		pDev->SetCullMode(ECull_None);

			Vector2 texSize((float)pTex->Width(),(float)pTex->Height());
			m_pDiffusePS->SetConstValue("g_texSize",&texSize,sizeof(texSize));
			pDev->SetVertShader(m_pDiffuseVS);
			pDev->SetPixelShader(m_pDiffusePS);

			RenderSys* pSys=Kernel::Inst()->GetRenderSys();
			pDev->SetStreamDefine(pSys->GetStreamDefine(EVType_P));

			for(int i=0;i<diffuseTimes;i++)
			{
				rtChain.Dest()->Begin();
				pDev->Clear(true,false,false,0,1.0f,0);
				pDev->SetTexture(0,rtChain.Src());
				pDev->SetSimplerAddress(0,ETAddress_Clamp,ETAddress_Clamp,ETAddress_Clamp);
				pDev->SetSimplerFilter(0,ETFilter_POINT,ETFilter_POINT);
				pDev->SetSimplerMipFilter(0,ETFilter_NONE);
				pDev->DrawMem(EDraw_TriangleFan,2,&vert[0],sizeof(Vert_P));
				rtChain.Dest()->End();

				rtChain.Flip();
			}

			//--
			pTex->StrechRect(rtChain.Src(),NULL,NULL,ETFilter_POINT);

			//--
			block.Apply();
			pDev->SetTexture(0,(IDeviceTex*)NULL);
			pDev->SetVertShader(NULL);
			pDev->SetPixelShader(NULL);
		}
	};
	
	//--class LightMapBlur------------------------------------
	LightMapBlur::LightMapBlur(IUICallBack* pUI):m_pUI(pUI)
	{
		m_p=new Member;
	}

	LightMapBlur::~LightMapBlur(void)
	{
		delete m_p;
	}

	UINT WINAPI ThreadLightMapBlurWriteFile(LPVOID pParam);
	bool LightMapBlur::Start(const TCHAR* szPath,int blurTimes,UINT mapWidth,UINT mapHeight)
	{
		vector<tstring> lightMaps;
		m_p->SearchLightMap(lightMaps,szPath);
		m_pUI->SetProgress(0,(int)lightMaps.size());

		f_bTerminateThreadLightMapBlurWriteFile = FALSE;
		HANDLE hHandle = (HANDLE)_beginthreadex(NULL, 0, ThreadLightMapBlurWriteFile, &m_p->m_listRT, 0, NULL);

		UINT width = mapWidth / TerrainEditor::EPatchSize;
		ASSERT(width*TerrainEditor::EPatchSize==mapWidth);
		UINT height = mapHeight / TerrainEditor::EPatchSize;
		ASSERT(height*TerrainEditor::EPatchSize==mapHeight);

		bool bIsTrn = false;
		UINT index = 0;//总序号;trn_XXX
		Image blurImg;
		Image childImg;
		TCHAR szDrive[256]={0};
		_tcscpy(szDrive,Kernel::Inst()->GetWorkPath());
		TCHAR szDir[256]={0};
		TCHAR szFileName[256]={0};
		TCHAR szExt[256]={0};
		TCHAR szChild[256]={0};
		for(int i=0;i<(int)lightMaps.size();i++)
		{
			tstring& szLightMap=lightMaps[i];
			bIsTrn = false;

			_tsplitpath(szLightMap.c_str(),NULL,szDir,szFileName,szExt);
			if( _tcslen(szFileName)>=7 )
			{
				TCHAR szTrn[5]={0};
				_tcsncpy(szTrn,szFileName,4);
				if( _tcscmp(szTrn,_T("trn_"))==0 )
				{
					bIsTrn = true;
					TCHAR *szIndex = &(szFileName[4]);
					index = _tstoi(szIndex);
				}
			}

			if( bIsTrn )//地形lightmap trn_
			{
				UINT row = index / width;
				UINT col = index - row*width;

				//顺序按照从左到右，从下到上
				//每张贴图256*256大小
				//模糊后取row,col处的贴图，保存成trn_XXX.blur格式
				if( row==0 && col==0 )//左下角	贴图空间的左上角
				{
					//总共四张贴图
					//row+1,col
					//row+1,col+1
					//row,col
					//row,col+1
					blurImg.Create(2*256,2*256,EPF_A8R8G8B8);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,(row+1)*width+col,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,0,256,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,(row+1)*width+col+1,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,256,256,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,row*width+col,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,0,0,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,row*width+col+1,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,256,0,0,0,256,256);

					blurImg.Blur(blurTimes);
					blurImg.DiffuseRow(255,0,254,1);
					blurImg.DiffuseCol(255,0,254,1);

					childImg.Blit(&blurImg,0,0,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d_.tga"),szDrive,szDir,row*width+col);
					childImg.WriteToFile(szChild);
					TCHAR szNew[256]={0};
					_stprintf(szNew,_T("%s%strn_%03d.blur"),szDrive,szDir,row*width+col);
					::_trename(szChild,szNew);
				}
				else if( row==0 && col==width-1 )//右下角	贴图空间的右上角
				{
					//总共四张贴图
					//row+1,col-1
					//row+1,col
					//row,col-1
					//row,col
					blurImg.Create(2*256,2*256,EPF_A8R8G8B8);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,(row+1)*width+col-1,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,0,256,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,(row+1)*width+col,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,256,256,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,row*width+col-1,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,0,0,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,row*width+col,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,256,0,0,0,256,256);

					blurImg.Blur(blurTimes);
					blurImg.DiffuseRow(255,257,511,1);
					blurImg.DiffuseCol(255,0,254,-1);

					childImg.Blit(&blurImg,0,0,256,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d_.tga"),szDrive,szDir,row*width+col);
					childImg.WriteToFile(szChild);
					TCHAR szNew[256]={0};
					_stprintf(szNew,_T("%s%strn_%03d.blur"),szDrive,szDir,row*width+col);
					::_trename(szChild,szNew);
				}
				else if( row==height-1 && col==0 )//左上角	贴图空间的左下角
				{
					//总共四张贴图
					//row,col
					//row,col+1
					//row-1,col
					//row-1,col+1
					blurImg.Create(2*256,2*256,EPF_A8R8G8B8);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,row*width+col,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,0,256,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,row*width+col+1,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,256,256,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,(row-1)*width+col,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,0,0,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,(row-1)*width+col+1,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,256,0,0,0,256,256);

					blurImg.Blur(blurTimes);
					blurImg.DiffuseRow(255,0,254,-1);
					blurImg.DiffuseCol(255,257,511,1);

					childImg.Blit(&blurImg,0,0,0,256,256,256);
					_stprintf(szChild,_T("%s%strn_%03d_.tga"),szDrive,szDir,row*width+col);
					childImg.WriteToFile(szChild);
					TCHAR szNew[256]={0};
					_stprintf(szNew,_T("%s%strn_%03d.blur"),szDrive,szDir,row*width+col);
					::_trename(szChild,szNew);
				}
				else if( row==height-1 && col==width-1 )//右上角	贴图空间的右下角
				{
					//总共四张贴图
					//row,col-1
					//row,col
					//row-1,col-1
					//row-1,col
					blurImg.Create(2*256,2*256,EPF_A8R8G8B8);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,row*width+col-1,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,0,256,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,row*width+col,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,256,256,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,(row-1)*width+col-1,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,0,0,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,(row-1)*width+col,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,256,0,0,0,256,256);

					blurImg.Blur(blurTimes);
					blurImg.DiffuseRow(255,257,511,-1);
					blurImg.DiffuseCol(255,257,511,-1);

					childImg.Blit(&blurImg,0,0,256,256,256,256);
					_stprintf(szChild,_T("%s%strn_%03d_.tga"),szDrive,szDir,row*width+col);
					childImg.WriteToFile(szChild);
					TCHAR szNew[256]={0};
					_stprintf(szNew,_T("%s%strn_%03d.blur"),szDrive,szDir,row*width+col);
					::_trename(szChild,szNew);
				}
				else if( index>0 && index<width-1 )//下边	贴图空间的上边
				{
					//总共六张贴图
					//row+1,col-1
					//row+1,col
					//row+1,col+1
					//row,col-1
					//row,col
					//row,col+1
					blurImg.Create(3*256,2*256,EPF_A8R8G8B8);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,(row+1)*width+col-1,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,0,256,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,(row+1)*width+col,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,256,256,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,(row+1)*width+col+1,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,512,256,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,row*width+col-1,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,0,0,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,row*width+col,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,256,0,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,row*width+col+1,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,512,0,0,0,256,256);

					blurImg.Blur(blurTimes);
					blurImg.DiffuseRow(255,257,510,1);
					blurImg.DiffuseCol(256,0,254,-1);
					blurImg.DiffuseCol(511,0,254,1);

					childImg.Blit(&blurImg,0,0,256,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d_.tga"),szDrive,szDir,row*width+col);
					childImg.WriteToFile(szChild);
					TCHAR szNew[256]={0};
					_stprintf(szNew,_T("%s%strn_%03d.blur"),szDrive,szDir,row*width+col);
					::_trename(szChild,szNew);
				}
				else if( col==0 && row>0 && row<height-1 )//左边	贴图空间的左边
				{
					//总共六张贴图
					//row+1,col
					//row+1,col+1
					//row,col
					//row,col+1
					//row-1,col
					//row-1,col+1
					blurImg.Create(2*256,3*256,EPF_A8R8G8B8);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,(row+1)*width+col,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,0,512,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,(row+1)*width+col+1,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,256,512,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,row*width+col,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,0,256,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,row*width+col+1,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,256,256,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,(row-1)*width+col,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,0,0,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,(row-1)*width+col+1,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,256,0,0,0,256,256);

					blurImg.Blur(blurTimes);
					blurImg.DiffuseRow(256,0,254,-1);
					blurImg.DiffuseRow(511,0,254,1);
					blurImg.DiffuseCol(255,257,510,1);

					childImg.Blit(&blurImg,0,0,0,256,256,256);
					_stprintf(szChild,_T("%s%strn_%03d_.tga"),szDrive,szDir,row*width+col);
					childImg.WriteToFile(szChild);
					TCHAR szNew[256]={0};
					_stprintf(szNew,_T("%s%strn_%03d.blur"),szDrive,szDir,row*width+col);
					::_trename(szChild,szNew);
				}
				else if( col==width-1 && row>0 && row<height )//右边	贴图空间的右边
				{
					//总共六张贴图
					//row+1,col-1
					//row+1,col
					//row,col-1
					//row,col
					//row-1,col-1
					//row-1,col
					blurImg.Create(2*256,3*256,EPF_A8R8G8B8);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,(row+1)*width+col-1,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,0,512,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,(row+1)*width+col,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,256,512,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,row*width+col-1,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,0,256,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,row*width+col,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,256,256,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,(row-1)*width+col-1,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,0,0,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,(row-1)*width+col,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,256,0,0,0,256,256);

					blurImg.Blur(blurTimes);
					blurImg.DiffuseRow(256,254,511,-1);
					blurImg.DiffuseRow(511,254,511,1);
					blurImg.DiffuseCol(256,257,510,-1);

					childImg.Blit(&blurImg,0,0,256,256,256,256);
					_stprintf(szChild,_T("%s%strn_%03d_.tga"),szDrive,szDir,row*width+col);
					childImg.WriteToFile(szChild);
					TCHAR szNew[256]={0};
					_stprintf(szNew,_T("%s%strn_%03d.blur"),szDrive,szDir,row*width+col);
					::_trename(szChild,szNew);
				}
				else if( row==height-1 && col>0 && col<width-1 )//上边	贴图空间的下边
				{
					//总共六张贴图
					//row,col-1
					//row,col
					//row,col+1
					//row-1,col-1
					//row-1,col
					//row-1,col+1
					blurImg.Create(3*256,2*256,EPF_A8R8G8B8);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,row*width+col-1,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,0,256,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,row*width+col,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,256,256,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,row*width+col+1,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,512,256,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,(row-1)*width+col-1,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,0,0,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,(row-1)*width+col,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,256,0,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,(row-1)*width+col+1,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,512,0,0,0,256,256);

					blurImg.Blur(blurTimes);
					blurImg.DiffuseRow(256,257,510,-1);
					blurImg.DiffuseCol(256,257,511,-1);
					blurImg.DiffuseCol(511,257,511,1);

					childImg.Blit(&blurImg,0,0,256,256,256,256);
					_stprintf(szChild,_T("%s%strn_%03d_.tga"),szDrive,szDir,row*width+col);
					childImg.WriteToFile(szChild);
					TCHAR szNew[256]={0};
					_stprintf(szNew,_T("%s%strn_%03d.blur"),szDrive,szDir,row*width+col);
					::_trename(szChild,szNew);
				}
				else//普通点
				{
					//总共九张贴图 
					//row+1,col-1
					//row+1,col
					//row+1,col+1
					//row,col-1
					//row,col
					//row,col+1
					//row-1,col-1
					//row-1,col
					//row-1,col+1
					blurImg.Create(3*256,3*256,EPF_A8R8G8B8);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,(row+1)*width+col-1,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,0,512,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,(row+1)*width+col,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,256,512,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,(row+1)*width+col+1,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,512,512,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,row*width+col-1,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,0,256,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,row*width+col,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,256,256,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,row*width+col+1,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,512,256,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,(row-1)*width+col-1,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,0,0,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,(row-1)*width+col,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,256,0,0,0,256,256);
					_stprintf(szChild,_T("%s%strn_%03d%s"),szDrive,szDir,(row-1)*width+col+1,szExt);
					childImg.Destroy();
					childImg.LoadFromFile( szChild );
					blurImg.Blit(&childImg,512,0,0,0,256,256);

					blurImg.Blur(blurTimes);
					blurImg.DiffuseRow(256,257,510,-1);
					blurImg.DiffuseRow(511,257,510,1);
					blurImg.DiffuseCol(256,257,510,-1);
					blurImg.DiffuseCol(511,257,510,1);

					childImg.Blit(&blurImg,0,0,256,256,256,256);
					_stprintf(szChild,_T("%s%strn_%03d_.tga"),szDrive,szDir,row*width+col);
					childImg.WriteToFile(szChild);
					TCHAR szNew[256]={0};
					_stprintf(szNew,_T("%s%strn_%03d.blur"),szDrive,szDir,row*width+col);
					::_trename(szChild,szNew);
				}
			}
			else//非地形贴图
			{
				m_p->BlurLightMap(szLightMap.c_str(),blurTimes);
			}

			m_pUI->SetProgress(i+1,(int)lightMaps.size());
		}

		//删除所有trn_XXX.tga
		TCHAR szFind[512];
		_sntprintf(szFind,512,_T("%s%strn_*.tga"),szDrive,szDir);
		vector<tstring> finds;
		SearchFiles(szFind,finds);
		for( UINT findIndex=0; findIndex<finds.size(); findIndex++ )
		{
			tstring& file = finds[findIndex];
			::DeleteFile(file.c_str());
		}
		finds.clear();
		//改扩展名blur为tga
		_sntprintf(szFind,512,_T("%s%s*.blur"),szDrive,szDir);
		SearchFiles(szFind,finds);
		TCHAR szNew[512];
		for( UINT findIndex=0; findIndex<finds.size(); findIndex++ )
		{
			_tsplitpath(finds[findIndex].c_str(),szDrive,szDir,szFileName,szExt);
			_stprintf(szNew,_T("%s%s%s.tga"),szDrive,szDir,szFileName);
			::_trename(finds[findIndex].c_str(),szNew);
		}

		// 等待写盘完成
		f_bTerminateThreadLightMapBlurWriteFile = TRUE;
		::WaitForSingleObject( hHandle, INFINITE );

		return true;
	}

	void LightMapBlur::Diffuse(IRenderTexture* pTex,int diffuseTimes)
	{
		m_p->Diffuse(pTex,diffuseTimes);
	}

	UINT WINAPI ThreadLightMapBlurWriteFile(LPVOID pParam)
	{
		TSafeList<LightMapBlur::tagThreadData*>* pList = (TSafeList<LightMapBlur::tagThreadData*>*)pParam;

		while( !f_bTerminateThreadLightMapBlurWriteFile || pList->Size() > 0 )
		{
			while( pList->Size() > 0 )
			{
				LightMapBlur::tagThreadData* pData = pList->PopFront();
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