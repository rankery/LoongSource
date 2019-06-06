#include "stdafx.h"
#include "timer.h"

#pragma comment(lib,"Winmm.lib")

namespace Cool3D
{
	void Timer::Init()
	{
		m_dwLastTime=timeGetTime();
		m_dwCurrentTime=timeGetTime();
	}

	Timer::Timer(void)
	{
		m_dwLastTime=0;		
		m_dwCurrentTime=0;
		m_dwElapse=0;		
		m_dwDelta=0;		

		m_fElapse=0.0f;		
		m_fDelta=0.0f;		
	}

	Timer::~Timer(void)
	{
	}
	
}//endof namespace Snake
