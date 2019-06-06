#pragma once
#include "Cool3D.h"
#include "Util\Timer.h"
#include "Device\IDevice.h"
#include "ResSys\ResMgr.h"

using namespace Cool3D;

	class IFS;//不在Cool3D namespace内
namespace Cool3D
{
	class IDevice;
	class RenderSys;
	class SceneGraph;
	class ConfigFile;

	struct tagEngineCfg 
	{
		TCHAR deviceClass[8];
		TCHAR renderSysClass[24];
		DWORD texPoolSize;
		DWORD renderablePoolSize;
		DWORD draw2DPoolSize;

		tagEngineCfg(void)
		{
			ZeroMemory(this,sizeof(tagEngineCfg));
		}
	};
	
	class Cool3D_API Kernel
	{
	public:
		static bool s_bSSE;
		Kernel(void);
		~Kernel(void);

		static Kernel *Inst();
	
		void CreateEngine(const ConfigFile* pCfg);
		void CreateEngine(const tagEngineCfg* pCfg);
		void CreateRenderDevice(tagDeviceCfg *pCfg);

		bool IsEngineCreated()
		{ return m_bEngineCreated;}
		bool IsDeviceCreated()
		{ return m_bDeviceCreated;}
		
		IDevice *GetDevice()
		{ return m_pDevice;}
		RenderSys *GetRenderSys()
		{ return m_pRenderSys; }
		

		IFS* DefaultFS()
		{	return m_pDefaultFS; }

		void LoadPlugIns();

		void FinalDestroy();
		void SetLastError(const TCHAR *lpszFormat, ...);
		const TCHAR* GetLastError();
		const TCHAR* GetAppPath() const;
		const TCHAR* GetWorkPath() const;

		//--时间处理
		/** 每帧执行一次
		*/
		void Tick()				
		{	
			m_timer.Update(); 
			ResMgr::Inst()->Update();
		}

		float GetAccumTime()	{	return m_timer.GetElapse(); }
		float GetDeltaTime()	{	return m_timer.GetDelta(); }
		DWORD GetAccumTimeDW()	{	return m_timer.GetElapseDW(); }
		DWORD GetDeltaTimeDW()	{	return m_timer.GetDeltaDW(); }
		void SetAccumTime(DWORD time)	{	m_timer.SetElapseTime(time); }

		DWORD GetPixelShaderVersion();
	private:
		Timer		m_timer;

		IDevice		*m_pDevice;
		RenderSys	*m_pRenderSys;
		IFS			*m_pDefaultFS;

		bool		m_bEngineCreated;
		bool		m_bDeviceCreated;

		class Member;
		Member *m_p;
	};

	inline IDevice* Device()
	{
		return Kernel::Inst()->GetDevice();
	}
}//namespace Cool3D