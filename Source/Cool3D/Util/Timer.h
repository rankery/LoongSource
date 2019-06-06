
#include "..\Cool3D.h"
#include <windows.h>
#include <Mmsystem.h>
#pragma once

namespace Cool3D
{
	/**	\class Timer
		\brief 时间管理工具
		\par
			<br>.游戏中所有用到时间的地方都需统一从此取得
			<br>.提供整体运行时间和两帧间隔时间
			<br>.内部保存原始数据(毫秒)和浮点数据,前者为了减少浮点误差,后者提供给游戏使用
	*/
	class Cool3D_API Timer
	{
	protected:
		//--内部状态量
		DWORD	m_dwLastTime;	//上一次Update得到的系统时间
		DWORD	m_dwCurrentTime;	//本次Update得到的系统时间
		//--------------------------
		DWORD	m_dwElapse;	//程序从启动到现在的总时间,毫秒
		DWORD	m_dwDelta;	//两次Update之间的间隔时间,毫秒

		float	m_fElapse;	//秒
		float	m_fDelta;	//秒
	public:

		void Init();
		
		void Update()
		{
			m_dwCurrentTime=timeGetTime();

			m_dwDelta=m_dwCurrentTime-m_dwLastTime;
			m_dwElapse+=m_dwDelta;

			m_fElapse=m_dwElapse*0.001f;
			m_fDelta=m_dwDelta*0.001f;

			//--
			m_dwLastTime=m_dwCurrentTime;
		}

		void SetElapseTime(DWORD time)	
		{	m_dwElapse=time;
			m_fElapse=m_dwElapse*0.001f;
		}

		DWORD GetDeltaDW()	{ return m_dwDelta;}
		float GetDelta()	{ return m_fDelta; }

		DWORD GetElapseDW()	{ return m_dwElapse; }
		float GetElapse()	{ return m_fElapse; }
				
		Timer(void);
		~Timer(void);
	};
}//endof namespace Snake
