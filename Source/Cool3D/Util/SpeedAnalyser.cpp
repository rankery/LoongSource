#include "StdAfx.h"
#include ".\speedanalyser.h"
#include "..\Device\DrawX.h"
#include <Mmsystem.h>

//#define TEST_CLIENT_SPEED		//测试客户端速度

namespace Cool3D
{
	SpeedAnalyser::SpeedAnalyser()
	{
		QueryPerformanceFrequency(&m_freq);
		m_maxTimeCount = 0;

		for(int i=0; i<20; i++)
			m_drawItem[i] = true;
	}

	SpeedAnalyser::~SpeedAnalyser(void)
	{
	}

	SpeedAnalyser* SpeedAnalyser::Inst()
	{
		static SpeedAnalyser g_speedAnalyser;
		return &g_speedAnalyser;
	}

	void SpeedAnalyser::AddItem(const TCHAR * szTitle)
	{
		tstring key = szTitle;
		Item item;
		m_itemMap.insert( make_pair(key,item) );
	}

	void SpeedAnalyser::Begin(const TCHAR * szTitle)
	{
		tstring key = szTitle;
		map<tstring,Item>::iterator it = m_itemMap.find(key);
		if( it != m_itemMap.end() )
		{
			QueryPerformanceCounter( &it->second.start );
		}
		else
		{
			AddItem(szTitle);
			Begin(szTitle);
		}
	}

	double SpeedAnalyser::End(const TCHAR * szTitle)
	{
		double currTime=0;

		tstring key = szTitle;
		map<tstring,Item>::iterator it = m_itemMap.find(key);
		if( it != m_itemMap.end() )
		{
			Item& item = it->second;

			QueryPerformanceCounter(&item.end);

			//-- 计算当前帧运行时间
			currTime = ((double)item.end.QuadPart - (double)item.start.QuadPart) 
				/ (double)m_freq.QuadPart;
			currTime*=1000.0f;

			//-- 累计总运行时间
			item.totalTime += currTime;

			//-- 计算平均时间
			item.count++;
			if( item.count >= 10 )
			{
				item.avgTime = item.totalTime / 10.0f;
				item.totalTime = 0;
				item.count=0;
			}

			//-- 计算峰值
			if(currTime>item.maxTime)
				item.maxTime=currTime;
		}

		return currTime;
	}

	void SpeedAnalyser::Draw(int x, int y)
	{
		static TCHAR buf[2048];
		static TCHAR buf1[512];

		buf[0] = '\0';

		for( map<tstring,Item>::iterator it = m_itemMap.begin();
				it != m_itemMap.end(); ++it )
		{
			_stprintf_s( buf1, _T("%s: %.2f/%.2f; \n"), 
				it->first.c_str(), it->second.avgTime, it->second.maxTime );
			_tcscat_s( buf, buf1 );
		}

		DrawX::DrawDebugString( buf, x, y, 0xFFFFFF00 );

		//-- 每400帧峰值清0
		m_maxTimeCount++;
		if( m_maxTimeCount >= 400 )
		{
			for( map<tstring,Item>::iterator it = m_itemMap.begin();
				it != m_itemMap.end(); ++it )
			{
				it->second.maxTime = 0;
			}
			m_maxTimeCount = 0;
		}
	}

	
	ScreenPrinter* ScreenPrinter::Inst()
	{
		static ScreenPrinter g_sp;
		return &g_sp;
	}

	void ScreenPrinter::Print(const TCHAR *format, ...)
	{
		va_list arglist;
		va_start( arglist, format );

		static TCHAR buf[1024];
		_vsntprintf_s( buf, 1023, format, arglist ); 

		buf[1023] = '\0';
		va_end( arglist );

		tstring str = buf;
		m_strArray.push_back(str);
	}

	void ScreenPrinter::Draw(int x, int y)
	{
		tstring str;
		for( vector<tstring>::iterator it = m_strArray.begin();
			it != m_strArray.end(); ++it )
		{
			str += *it;
			str += _T("\n");
		}

		DrawX::DrawDebugString( str.c_str(), x, y, 0xFFFFFF00 );

		m_strArray.clear();
	}

	void BeginSpeedTest(const TCHAR* szText)
	{
#ifdef TEST_CLIENT_SPEED
		SpeedAnalyser::Inst()->Begin(szText);
#endif
	}
	void EndSpeedTest(const TCHAR* szText)
	{
#ifdef TEST_CLIENT_SPEED
		SpeedAnalyser::Inst()->End(szText);
#endif
	}
	void DrawSpeedTest(int x, int y)
	{
#ifdef TEST_CLIENT_SPEED
		SpeedAnalyser::Inst()->Draw(x, y);
#endif
	}

}//end namespace Cool3D
