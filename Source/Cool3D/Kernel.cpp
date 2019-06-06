#include "StdAfx.h"
#include ".\kernel.h"

#include "Util\DebugMisc.h"
#include "Util\PlugIn.h"
#include "Util\RTTIObj.h"
#include "Util\ConfigFile.h"
#include "VFS\IFS.h"
#include "VFS\StdFS.h"

#include "Image\Image.h"
#include "Image\ImageGDI.h"
#include "Device\DX9Device\DX9Device.h"
#include "RenderSys\RenderSysFixed.h"
#include "ResSys\ResMgr.h"
#include "Draw2D\IDraw2D.h"
#include "RenderSys\WaterReflectMapRender.h"
#include "RenderSys\WaterRefractMapRender.h"
#include "Terrain\TerrainMultiLayersRender.h"
#include "Terrain\TPatchTriangulationDynamic.h"
#include "Terrain\TPatchTriangulationStatic.h"
#include "Terrain\TRiverPatchTriangulationDynamic.h"
#include "Terrain\TPatchLodTable.h"
#include "RenderSys\WaterRender.h"
#include "Audio\AudioSystem.h"

#include ".\Cool3DOption.h"

#include <ddraw.h>
#pragma comment(lib,"ddraw.lib")

namespace Cool3D
{
	bool Kernel::s_bSSE=false;
	namespace Detail
	{

		void GetCPUID( int i, DWORD *A, DWORD *B, DWORD *C, DWORD *D )
		{

			__asm
			{			
				mov eax,[i];
				_emit 0x0f;
				_emit 0xa2;

				mov edi,[A];
				mov [edi],eax;

				mov edi,[B];
				mov [edi],ebx;

				mov edi,[C];
				mov [edi],ecx;

				mov edi,[D];
				mov [edi],edx;

				mov eax,0;
				mov ebx,0;
				mov ecx,0;
				mov edx,0;
				mov esi,0;
				mov edi,0;
			}
		}
	};
	using namespace Detail;
	class Kernel::Member
	{
	public:
		vector<PlugIn*>		m_pPlugIns;
		UINT				m_texPoolSize;
		UINT				m_renderPoolSize;
		UINT				m_draw2DPoolSize;
		UINT				m_skeletonPoolSize;
		UINT				m_specialEffectPoolSize;
		UINT				m_vsPoolSize;
		UINT				m_psPoolSize;
		TCHAR				m_szAppPath[512];
		TCHAR				m_szWorkPath[512];
		tstring				m_szLastError;
		
		Member()
		{	
			m_texPoolSize=16*1024*1024;
			m_renderPoolSize=8*1024*1024;
			m_draw2DPoolSize=4*1024*1024;
			m_skeletonPoolSize=50*1024*1024;
			m_specialEffectPoolSize=2*1024*1024;
			m_vsPoolSize=40;
			m_psPoolSize=40;
			
			//--
			GetModuleFileName(NULL,m_szAppPath,sizeof(m_szAppPath));
			TCHAR *find=::_tcsrchr(m_szAppPath,_T('\\'));
			if(find!=NULL)
				*(find+1)=0;
			_tcslwr_s(m_szAppPath,sizeof(m_szAppPath)/sizeof(TCHAR));

			//--
			GetModuleFileName(NULL,m_szWorkPath,sizeof(m_szWorkPath)/sizeof(TCHAR));
			find=::_tcsrchr(m_szWorkPath,_T('\\'));
			if(find!=NULL)
			{
				*find=0;
				find=::_tcsrchr(m_szWorkPath,_T('\\'));
				if(find!=NULL)
				{
					*(find+1)=0;
					_tcslwr_s(m_szWorkPath,sizeof(m_szWorkPath)/sizeof(TCHAR));
				}
			}
		}
		~Member()
		{}
	};
	
	Kernel::Kernel(void)
	{
		m_bEngineCreated=false;
		m_bDeviceCreated=false;

		m_pDevice=NULL;
		m_pRenderSys=NULL;
		m_pDefaultFS=new StdFS;

		m_p=new Member;

		//--
		_InitLogFile();

		m_timer.Init();

		//--检测CPU
		DWORD A,B,C,D;
		GetCPUID( 1, &A, &B, &C, &D );
		if(D & 0x02000000)
		{
			s_bSSE=true;
			TRACE(_T("CPU: SSE detected.\r\n"));
		}
	}

	Kernel::~Kernel(void)
	{
		delete m_p;
		delete m_pDefaultFS;
	}

	static Kernel g_kernelInst;
	Kernel *Kernel::Inst()
	{
		return &g_kernelInst;
	}

	void Kernel::LoadPlugIns()
	{
	}

	void Kernel::CreateEngine(const tagEngineCfg* pCfg)
	{
		/**配置
		DeviceClass="";
		RenderSysClass="RenderSysFixed";
		TexPoolSize=16*1024;		--KBytes
		RenderablePoolSize=8*1024; 	--KBytes
		Draw2DPoolSize=4*1024;		--KBytes
		*/
		//--先初始化某些部件
		TRACE(_T("Number of Runtime class = %d\n"),RTTFactory::Inst()->GetNumClass());
		Image::InitSys();
		ImageGDI::InitSys();
		AudioSystem::Inst()->Init();

		//--创建Device
		ASSERT(m_pDevice==NULL);
		m_pDevice=new DX9Device;

		//--创建RenderSys
		ASSERT(m_pRenderSys==NULL);
		if(pCfg==NULL)
			m_pRenderSys=new RenderSysFixed;
		else
			m_pRenderSys=(RenderSys*)RTTFactory::Inst()->CreateObj(pCfg->renderSysClass);


		if(pCfg!=NULL)
		{
			m_p->m_texPoolSize=1024*pCfg->texPoolSize;
			m_p->m_renderPoolSize=1024*pCfg->renderablePoolSize;
			m_p->m_draw2DPoolSize=1024*pCfg->draw2DPoolSize;
		}

		SetCurrentDirectory(GetWorkPath());

		m_bEngineCreated=true;
	}

	void Kernel::CreateEngine(const ConfigFile* pCfg)
	{
		if(pCfg!=NULL)
		{
			tagEngineCfg cfg;
			_tcscpy_s(cfg.deviceClass,sizeof(cfg.deviceClass)/sizeof(TCHAR),pCfg->GetString(_T("DeviceClass")).c_str());
			_tcscpy_s(cfg.renderSysClass,sizeof(cfg.renderSysClass)/sizeof(TCHAR),pCfg->GetString(_T("RenderSysClass")).c_str());
			cfg.texPoolSize=pCfg->GetInt(_T("TexPoolSize"));
			cfg.renderablePoolSize=pCfg->GetInt(_T("RenderablePoolSize"));
			cfg.draw2DPoolSize=pCfg->GetInt(_T("Draw2DPoolSize"));

			CreateEngine(&cfg);
		}
		else
			CreateEngine((const tagEngineCfg*)NULL);
	}

	void Kernel::CreateRenderDevice(tagDeviceCfg *pCfg)
	{
		ASSERT(m_pDevice!=NULL);
		m_pDevice->Create(pCfg);
		m_bDeviceCreated=true;

		//--检查画面选项
		Cool3DOption::Inst()->CheckCreateOptions();

		//--
		ASSERT(m_pRenderSys!=NULL);
		if(m_pRenderSys)
			m_pRenderSys->InitDevice();
		
		//--创建其他
		ResMgr::Inst()->Init(m_p->m_texPoolSize,m_p->m_renderPoolSize,
			m_p->m_skeletonPoolSize,m_p->m_specialEffectPoolSize,
			m_p->m_vsPoolSize,m_p->m_psPoolSize);
		IDraw2D::Inst()->Create(m_p->m_draw2DPoolSize);		
		
		//--初始化水面反射/折射效果
		WaterReflectMapRender::Inst()->Init();
		WaterRefractMapRender::Inst()->Init();

		//--创建地形三角形生成器
		TPatchTriangulationStatic::Inst()->Create();
		TPatchTriangulationDynamic::Inst()->Create();

		//--创建河流三角形生成器
		TRiverPatchTriangulationDynamic::Inst()->Create();
	
		//--创建地形块LOD查询Table
		TPatchLodTable::Inst()->Create(32,32);
		TPatchLodTable::Inst_SimpleTerrain()->Create(8,8);

		//--创建地形多纹理渲染器
		if(Cool3DOption::Inst()->GetOption(Cool3DOption::EO_TerrainDetail)==Cool3DOption::ETD_MultiLayers)
			TerrainMultiLayersRender::Inst()->Create();

		//--创建河流渲染器
		WaterRender::Inst()->Create();
	}

	void Kernel::FinalDestroy()
	{
		WaterReflectMapRender::Inst()->Destroy();
		WaterRefractMapRender::Inst()->Destroy();

        TerrainMultiLayersRender::Inst()->Destroy();

		TPatchTriangulationDynamic::Inst()->Destroy();
		TRiverPatchTriangulationDynamic::Inst()->Destroy();
		
		TPatchLodTable::Inst()->Destroy();
		TPatchLodTable::Inst_SimpleTerrain()->Destroy();

		WaterRender::Inst()->Destroy();
		
		ResMgr::Inst()->Destroy();
		SAFE_DELETE(m_pRenderSys);
		IDraw2D::Inst()->Destroy();
		SAFE_DELETE(m_pDevice);

		AudioSystem::Inst()->Destroy();
		Image::ShutDownSys();

		m_bEngineCreated=false;
		m_bDeviceCreated=false;
	}

	void Kernel::SetLastError(const TCHAR *lpszFormat, ...)
	{
		va_list args;
		va_start(args, lpszFormat);

		int nBuf;
		TCHAR szBuffer[512];

		nBuf = _vstprintf_s(szBuffer, 512, lpszFormat, args);
		ASSERT(nBuf < sizeof(szBuffer)); //Output truncated as it was > sizeof(szBuffer)

		
		m_p->m_szLastError=szBuffer;
		va_end(args);

		//--写入Log
		_WriteLogString(_T("Cool3D Error:%s\r\n"),szBuffer);
	}
	const TCHAR* Kernel::GetLastError()
	{
		return m_p->m_szLastError.c_str();
	}

	const TCHAR* Kernel::GetAppPath() const
	{
		return m_p->m_szAppPath;
	}

	const TCHAR* Kernel::GetWorkPath() const
	{
		return m_p->m_szWorkPath;
	}

	DWORD Kernel::GetPixelShaderVersion()
	{
		IDirect3D9* pD3D = ::Direct3DCreate9(D3D_SDK_VERSION);
		THROW_NULL(pD3D,_T("D3D9 object create failed."));

		D3DCAPS9 caps;
		HRESULT hr=pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&caps);
		SAFE_RELEASE( pD3D );
		return caps.PixelShaderVersion;
	}
}//namespace Cool3D