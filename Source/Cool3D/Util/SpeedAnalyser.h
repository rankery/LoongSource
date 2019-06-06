#pragma once
#include "..\Cool3D.h"
#include <string>
#include <map>
using namespace std;
#pragma warning( disable : 4251 )

namespace Cool3D
{
	/**	\class SpeedAnalyser
	\biref 客户速度分析工具.
	\remraks 用来实时统计并显示某一段代码每帧的执行时间
	*/
	class Cool3D_API SpeedAnalyser
	{
		struct Item
		{
			DWORD			count;
			double			totalTime;
			double			avgTime;
			double			maxTime;
			LARGE_INTEGER	start;
			LARGE_INTEGER	end;
			Item()
			{
				memset(this,0,sizeof(Item));
			}
		};

	public:
		SpeedAnalyser();
		~SpeedAnalyser(void);

		static SpeedAnalyser* Inst();
		void AddItem(const TCHAR * szTitle);

		void Begin(const TCHAR * szTitle);
		double End(const TCHAR * szTitle);
		void Draw(int x, int y);

		//test
		bool				m_drawItem[20];
		float				m_testData[20];

	private:
		LARGE_INTEGER		m_freq;
		map<tstring,Item>	m_itemMap;
		DWORD				m_maxTimeCount;
	};

	class Cool3D_API ScreenPrinter
	{
	public:
		ScreenPrinter(){}
		~ScreenPrinter(){}

		static ScreenPrinter* Inst();
		void Print(const TCHAR *format, ...);
		void Draw(int x, int y);

	private:
		vector<tstring> m_strArray;
	};

	Cool3D_API void BeginSpeedTest(const TCHAR* szText);
	Cool3D_API void EndSpeedTest(const TCHAR* szText);
	Cool3D_API void DrawSpeedTest(int x, int y);

}//end namespace Cool3D

