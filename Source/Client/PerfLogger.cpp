#include "stdafx.h"
#include "PerfLogger.h"
#include <fstream>
#include <iomanip>
#include "psapi.h"
#include "Role.h"
#include "Player.h"
#include "LocalPlayer.h"
#include "RoleMgr.h"
#include "EffectMgr.h"
#include "MapMgr.h"
#include "..\WorldDefine\svn_revision.h"

#pragma comment(lib, "psapi.lib")

PerfLogger::PerfLogger() :
m_Trunk( this ),
m_bSampleStart( false ),
m_fSampleInterval( 10.0f ),
m_fLastSampleTime( 0 ),
m_dwNumProcessors( 0 ),
m_nLastCPUTime( 0 ),
m_nLastCPUSystemTime( 0 )
{
	ZeroMemory( m_szBuff, sizeof(m_szBuff) );
}

PerfLogger::~PerfLogger()
{
}

static PerfLogger g_PerfLogger;

PerfLogger* PerfLogger::Inst()
{
	return &g_PerfLogger;
}

void PerfLogger::Init()
{
	TObjRef<Console>()->Register(_T("pflog"), m_Trunk.sfp2(&PerfLogger::OnCommand), _T("Log performance"), 2);

	// 保存CUP数量
	SYSTEM_INFO info;   
	GetSystemInfo(&info);   
	m_dwNumProcessors = info.dwNumberOfProcessors;

	m_hProcess = GetCurrentProcess();
}

DWORD PerfLogger::OnCommand( LPCTSTR szCommand, LPCTSTR szParam1 )
{
	tstring strCommand = szCommand;
	if( strCommand == _T("start") )
	{
		float fSampleInterval = 10.0f;
		if( 0 != _tcslen( szParam1 ) )
		{
			fSampleInterval = (float)_tstof( szParam1 );
			if( fSampleInterval <= 0.0f )
			{
				IMSG( _T("Error param(SampleInterval).\n") );
				return -1;
			}
		}
		else
		{
			IMSG( _T("Use default sampleInterval(10s).\n") );
		}
		LogStart( fSampleInterval );
	}
	else if( strCommand == _T("end") )
	{
		LogEnd();
	}
	return 1;
}

void PerfLogger::Updata()
{
	if( m_bSampleStart && Kernel::Inst()->GetAccumTime() - m_fLastSampleTime > m_fSampleInterval )
	{
		LogRunTimeData( m_strCurFileName.c_str() );
		m_fLastSampleTime = Kernel::Inst()->GetAccumTime();
	}
}

void PerfLogger::LogStart( float fSampleInterval )
{
	if( m_bSampleStart )
		LogEnd();

	m_strCurFileName = MakeFileName();
	if( m_strCurFileName.empty() )
	{
		IMSG( _T("Performance log start Failed.(MakeFileName)") );
		return;
	}

	WriteFileHead( m_strCurFileName.c_str() );

	m_fSampleInterval = fSampleInterval;
	m_bSampleStart = true;
	m_fLastSampleTime = 0.0f;

	IMSG( _T("Start performance log.[%s]\n"), m_strCurFileName.c_str() );
}

void PerfLogger::LogEnd()
{
	if( !m_bSampleStart )
		return;

	std::locale old_loc = std::locale::global(std::locale(""));

	tofstream ofs;
	ofs.open( m_strCurFileName.c_str(), ios::app );
	ofs<<endl<<_T("--------------------------------------End---------------------------------")<<endl;
	ofs.close();

	std::locale::global(std::locale(old_loc));

	IMSG( _T("End performance log.[%s]\n"), m_strCurFileName.c_str() );
	m_strCurFileName = _T("");
	m_bSampleStart = false;
}

tstring PerfLogger::MakeFileName()
{
	FILETIME CurrentTime;
	GetSystemTimeAsFileTime(&CurrentTime);

	WORD wDate = 0, wTime = 0;
	if ( !FileTimeToLocalFileTime(&CurrentTime, &CurrentTime) ||
		 !FileTimeToDosDateTime(&CurrentTime, &wDate, &wTime) )
		 return _T("");

	_sntprintf( m_szBuff, TEXT_BUFF_SIZE, _T("log\\PerformanceReport[%d-%d-%d %02d%02d%02d].log"),
			    (wDate / 32) & 15, wDate & 31, (wDate / 512) + 1980,
			    (wTime >> 11), (wTime >> 5) & 0x3F, (wTime & 0x1F) * 2 );

	return m_szBuff;
}

bool PerfLogger::WriteFileHead( LPCTSTR szFileName )
{
	std::locale old_loc = std::locale::global(std::locale(""));

	tofstream ofs;
	ofs.open( szFileName );

	const int w = 30;
	ofs<<_T("--------------------------------------System Info---------------------------------")<<endl<<endl;

	// 计算机名
	DWORD dwLen = TEXT_BUFF_SIZE;
	GetComputerName( m_szBuff, &dwLen ); 
	ofs<<setw(w)<<_T("Computer Name : ")<<m_szBuff<<endl;

	// 操作系统
	tstring strOSInfo;
	if( m_pUtil->ReadReg( _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"), _T("ProductName"), m_szBuff ) )
	{
		strOSInfo += m_szBuff;
	}
	if( m_pUtil->ReadReg( _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"), _T("CSDVersion"), m_szBuff ) )
	{
		strOSInfo += _T(" -");
		strOSInfo += m_szBuff;
	}
	if( m_pUtil->ReadReg( _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"), _T("CurrentVersion"), m_szBuff ) )
	{
		strOSInfo += _T(" (");
		strOSInfo += m_szBuff;
		strOSInfo += _T(")");
	}
	if( !strOSInfo.empty() )
		ofs<<setw(w)<<_T("OS : ")<<strOSInfo<<endl;
	else
		ofs<<setw(w)<<_T("OS : Unknow")<<endl;

	// CPU
	if( m_pUtil->ReadReg( _T("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"), _T("ProcessorNameString"), m_szBuff ) )
	{
		ofs<<setw(w)<<_T("CPU : ")<<m_szBuff<<endl;
	}

	// DirectX版本
	if( m_pUtil->ReadReg( _T("SOFTWARE\\Microsoft\\DirectX"), _T("Version"), m_szBuff ) )
	{
		ofs<<setw(w)<<_T("DirectX Version : ")<<m_szBuff<<endl;
	}

	// 内存
	MEMORYSTATUSEX        Meminfo;
	memset(&Meminfo, 0, sizeof(Meminfo));
	Meminfo.dwLength = sizeof(Meminfo);
	if( GlobalMemoryStatusEx( &Meminfo ) )
	{
		double dSize = (double)Meminfo.ullTotalPhys / ( 1024.0 * 1024.0 * 1024.0 );
		ofs<<setw(w)<<_T("Total Physical Memory : ")<<std::showpoint<<std::setprecision(3)<<dSize<<_T(" GB")<<endl;
	}

	// 显卡
	IDevice* pDev = Device();
	if( NULL != pDev )
	{
		UINT nAdapterCount = pDev->GetAdapterCount();
		ofs<<setw(w)<<_T("Num Video Card : ")<<nAdapterCount<<endl;
		for( UINT i = 0; i < nAdapterCount; i++ )
		{
			D3DADAPTER_IDENTIFIER9 di;
			if( !pDev->GetAdapterIdentifier( i, &di ) )
				return false;
			ofs<<setw(w)<<_T("Video Card : ")<<di.Description<<endl;
			ofs<<setw(w)<<_T("Main Driver : ")<<di.Driver<<endl;
			ofs<<setw(w)<<_T("Driver Version : ")<<HIWORD(di.DriverVersion.HighPart)<<_T('.')
												 <<LOWORD(di.DriverVersion.HighPart)<<_T('.')
												 <<HIWORD(di.DriverVersion.LowPart)<<_T('.')
												 <<LOWORD(di.DriverVersion.LowPart)<<endl;
		}
		UINT nSize = pDev->GetAvailableTextureMem();
		double dSize = (double)nSize / ( 1024.0 * 1024.0 );
		ofs<<setw(w)<<_T("Available Texture Memory : ")<<std::showpoint<<std::setprecision(5)<<dSize<<_T(" MB")<<endl;
		DWORD dwShaderVersion = pDev->CapsVertexShaderVersion();
		ofs<<setw(w)<<_T("Vertex Shader Version : ")<<D3DSHADER_VERSION_MAJOR(dwShaderVersion)<<_T(".")<<D3DSHADER_VERSION_MINOR(dwShaderVersion)<<endl;
		dwShaderVersion = pDev->CapsPixelShaderVersion();
		ofs<<setw(w)<<_T("Pixel Shader Version : ")<<D3DSHADER_VERSION_MAJOR(dwShaderVersion)<<_T(".")<<D3DSHADER_VERSION_MINOR(dwShaderVersion)<<endl;
		int nTex = pDev->FCapsMaxSimultaneousTextures();
		ofs<<setw(w)<<_T("Max Simultaneous Textures : ")<<nTex<<endl;
		nTex = pDev->CapsMaxTextureStages();
		ofs<<setw(w)<<_T("Max Texture Stages : ")<<nTex<<endl;
	}

	// 应用程序路径
	GetModuleFileName( NULL, m_szBuff, TEXT_BUFF_SIZE );
	ofs<<setw(w)<<_T("App Path : ")<<m_szBuff<<endl;

	// 进程ID
	ofs<<setw(w)<<_T("Process ID : ")<<GetCurrentProcessId()<<endl;

	// 版本号
	ofs<<setw(w)<<_T("Client Revision : ")<<CLIENT_BUILD_REVISION<<endl;
	ofs<<setw(w)<<_T("vEngine Revision : ")<<VENGINE_BUILD_REVISION<<endl;
	ofs<<setw(w)<<_T("Cool3D Revision : ")<<COOL3D_BUILD_REVISION<<endl;
	ofs<<setw(w)<<_T("WorldBase Revision : ")<<WORLDBASE_BUILD_REVISION<<endl;
	ofs<<setw(w)<<_T("WorldDefine Revision : ")<<WORLDDEFINE_BUILD_REVISION<<endl;

	ofs<<endl<<_T("--------------------------------------RunTime Data---------------------------------")<<endl<<endl;

	ofs.close();

	std::locale::global(std::locale(old_loc));
	return true;
}

bool PerfLogger::LogRunTimeData( LPCTSTR szFileName )
{
	std::locale old_loc = std::locale::global(std::locale(""));

	tofstream ofs;
	ofs.open( szFileName, ios::app );

	const TCHAR szSpace[] = _T(" ");
	const double fInvMB = 1.0 / ( 1024.0 * 1024.0 );
	const int w = 8;

	// 时间点
	FILETIME CurrentTime;
	GetSystemTimeAsFileTime(&CurrentTime);
	WORD wDate = 0, wTime = 0;
	if ( !FileTimeToLocalFileTime(&CurrentTime, &CurrentTime) ||
		 !FileTimeToDosDateTime(&CurrentTime, &wDate, &wTime) )
		return false;
	_sntprintf( m_szBuff, TEXT_BUFF_SIZE, _T("[%02d:%02d:%02d]"),
				(wTime >> 11), (wTime >> 5) & 0x3F, (wTime & 0x1F) * 2 );

	ofs<<m_szBuff<<szSpace;

	// CPU使用率
	int nCPUUsage = GetCPUUsage();
	ofs<<_T("CPUUsage(%)=")<<setw(w/2)<<std::left<<nCPUUsage<<szSpace;

	// 内存使用
	PROCESS_MEMORY_COUNTERS pmc;   
	if( ::GetProcessMemoryInfo( m_hProcess, &pmc, sizeof(pmc) ) )   
	{   
		ofs<<_T("MemoryUsage(mb)=")	    << setw(w)<<std::left<<std::showpoint<<std::setprecision(5)<<pmc.WorkingSetSize * fInvMB<<szSpace;
		ofs<<_T("PagefileUsage(mb)=")	<< setw(w)<<std::left<<std::showpoint<<std::setprecision(5)<<pmc.PagefileUsage * fInvMB<<szSpace;
	}

	// 内部信息
	IDevice* pDev = Device();
	if( NULL != pDev )
	{
		ofs<<_T("FPS=")					<< setw(w)<<std::left<<pDev->GetRenderStatus()->FPS<<szSpace;
		ofs<<_T("Primative=")			<< setw(w)<<std::left<<pDev->GetRenderStatus()->numPrim<<szSpace;
		ofs<<_T("DrawCall=")			<< setw(w)<<std::left<<pDev->GetRenderStatus()->numDrawCall<<szSpace;
		ofs<<_T("Texture(mb)=")			<< setw(w)<<std::left<<std::showpoint<<std::setprecision(5)<<(double)pDev->GetRenderStatus()->texSize * fInvMB<<szSpace;
		ofs<<_T("RenderTarget(mb)=")	<< setw(w)<<std::left<<std::showpoint<<std::setprecision(5)<<(double)pDev->GetRenderStatus()->renderTexSize * fInvMB<<szSpace;
		ofs<<_T("StaticVB(mb)=")		<< setw(w)<<std::left<<std::showpoint<<std::setprecision(5)<<(double)pDev->GetRenderStatus()->staticVBSize * fInvMB<<szSpace;
		ofs<<_T("DynamicVB(mb)=")		<< setw(w)<<std::left<<std::showpoint<<std::setprecision(5)<<(double)pDev->GetRenderStatus()->dynamicVBSize * fInvMB<<szSpace;
		ofs<<_T("StaticIB(mb)=")		<< setw(w)<<std::left<<std::showpoint<<std::setprecision(5)<<(double)pDev->GetRenderStatus()->staticIBSize * fInvMB<<szSpace;
		ofs<<_T("DynamicIB(mb)=")		<< setw(w)<<std::left<<std::showpoint<<std::setprecision(5)<<(double)pDev->GetRenderStatus()->dynamicIBSize * fInvMB<<szSpace;
		ofs<<_T("RoleNum=")				<< setw(w/2)<<std::left<<RoleMgr::Inst()->GetRoleMap().size()<<szSpace;
		ofs<<_T("EffectNum=")			<< setw(w/2)<<std::left<<EffectMgr::Inst()->GetNumEffect()<<szSpace;
		if( !MapMgr::Inst()->GetCurMapName().empty() )
		{
			ofs<<_T("MapName=")				<< setw(w)<<std::left<<MapMgr::Inst()->GetCurMapName()<<szSpace;
		}
		else
		{
			ofs<<_T("MapName=")				<< setw(w)<<std::left<<_T("Null")<<szSpace;
		}
		Role* pLP = RoleMgr::Inst()->GetLocalPlayer();
		if( P_VALID( pLP ) )
		{
			_sntprintf( m_szBuff, TEXT_BUFF_SIZE, _T("(%d,%d)"), (int)( pLP->GetPos().x / TILE_SCALE ), (int)( pLP->GetPos().z / TILE_SCALE ) );
			ofs<<_T("LPTile=") << setw(w*2)<<std::left<<m_szBuff<<szSpace;
		}
		else 
		{
			ofs<<_T("LPTile=") << setw(w*2)<<std::left<<_T("Null")<<szSpace;
		}
		TObjRef<NetSession> pSession;
		if( P_VALID( pSession ) )
		{
			ofs<<_T("SendQueueSize=")			<< setw(w/2)<<std::left<<pSession->GetSendQueueSize()<<szSpace;
			ofs<<_T("SendQueueActionSize=")     << setw(w/2)<<std::left<<pSession->GetSendQueueActionSize()<<szSpace;
		}
		else
		{
			ofs<<_T("SendQueueSize=")			<< setw(w/2)<<std::left<<_T("Null")<<szSpace;
			ofs<<_T("SendQueueActionSize=")     << setw(w/2)<<std::left<<_T("Null")<<szSpace;
		}
	}
	
	ofs<<endl;
	ofs.close();

	std::locale::global(std::locale(old_loc));
	return true;
}

INT64 PerfLogger::FileTime2INT64(const FILETIME* ftTime)
{
	ASSERT(ftTime); 

	LARGE_INTEGER li;   
	li.LowPart = ftTime->dwLowDateTime;   
	li.HighPart = ftTime->dwHighDateTime;   
	return li.QuadPart;   
}

int PerfLogger::GetCPUUsage()   
{
	FILETIME ftCurTime;   
	FILETIME ftCreationTime;   
	FILETIME ftExitTime;   
	FILETIME ftKernelTime;   
	FILETIME ftUserTime;   

	GetSystemTimeAsFileTime(&ftCurTime);
	if (!GetProcessTimes( m_hProcess, &ftCreationTime, &ftExitTime, &ftKernelTime, &ftUserTime ) )   
	{   
		return -1;
	}   
	INT64 nCurSystemTime = ( FileTime2INT64(&ftKernelTime) + FileTime2INT64(&ftUserTime) ) / m_dwNumProcessors;   
	INT64 nCurTime = FileTime2INT64(&ftCurTime);

	if ( ( 0 == m_nLastCPUSystemTime ) || ( 0 == m_nLastCPUTime ) )   
	{   
		// First call, just set the last values.   
		m_nLastCPUSystemTime = nCurSystemTime;   
		m_nLastCPUTime = nCurTime;
		return -1;
	}   

	const INT64 nSystemTimeDelta = nCurSystemTime - m_nLastCPUSystemTime;   
	const INT64 nTimeDelta = nCurTime - m_nLastCPUTime;
 
	if ( nTimeDelta == 0 )   
		return -1;   

	// We add time_delta / 2 so the result is rounded.   
	int nRet = (int)((nSystemTimeDelta * 100 + nTimeDelta / 2) / nTimeDelta );   
	m_nLastCPUSystemTime = nCurSystemTime;   
	m_nLastCPUTime = nCurTime;
	return nRet;   
}   
