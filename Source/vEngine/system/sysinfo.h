//-----------------------------------------------------------------------------
// File: sysinfo.h
// Auth: Lyp
// Date: 2004/02/03
// Last: 2004/06/22
// Desc: 得到系统信息
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine{


class VENGINE_API SysInfo
{
public:

	// 得到Windows版本描述字符串
	BOOL GetWinVer(LPSTR pszVersion, LPSTR pszBuild);
	// 检查是否NT操作系统
	BOOL IsNT();

	// 得到DX版本
	DWORD GetDXVersion();

	// 根据异常码得到异常描述字符串
	CONST CHAR *GetExceptionDescription(DWORD ExceptionCode);

	// FormatTime 将时间输出成人可以阅读的格式
	VOID FormatTime(LPSTR pDest, FILETIME TimeToPrint);

	// GetFilePart
	CHAR* GetFilePart(LPCSTR pSource);

	// 关闭系统
	BOOL SystemShutdown();

	// 得到CPU指令集能力
	DWORD GetCpuCaps();
	// 得到CPU个数
	INT GetProcessorNum();

};




}