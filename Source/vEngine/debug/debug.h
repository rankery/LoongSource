//-----------------------------------------------------------------------------
// File: debug
// Desc: Game Tool debug 2.0
// Auth: Lyp
// Date: 2003/12/11
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine {


//-----------------------------------------------------------------------------
// for debug
//-----------------------------------------------------------------------------
// 错误信息,由windows窗口给出提示,可以由用户决定继续或退出并生成log
#define ERR				TObjRef<Debug>()->ErrMsg

// 内部信息,显示在console
#define IMSG			TObjRef<Console>()->Print

// 重新定义ASSERT
#undef ASSERT
#ifdef _DEBUG
	// 给出windows窗口提示,可以由用户决定继续或退出并生成log
	#define ASSERT(f)	if( !(f) ) TObjRef<Debug>()->ErrMsg( \
		_T("Failure:%s\r\nFile:%s\r\nLine:%d"), _T(#f), _T(__FILE__), __LINE__)
#else
	#define ASSERT(f)	((void)0)
#endif


//-----------------------------------------------------------------------------
// class Debug 开发过程中的调试工具
//-----------------------------------------------------------------------------
class VENGINE_API Debug
{
public:
	// 显示一般调试消息，并获取系统出错信息
	INT  ErrMsg(LPCTSTR lpFormat, ...);

private:
	TObjRef<Console>	m_pConsole;

};



} // vEngine



