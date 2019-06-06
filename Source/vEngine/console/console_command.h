//-----------------------------------------------------------------------------
//!\file console_command.h
//!\author Lyp
//!
//!\date 2004-06-21
//! last 2004-06-21
//!
//!\brief 控制台命令集
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine {


typedef DWORD (WINAPI* OutputFunction)(DWORD szText);
//-----------------------------------------------------------------------------
//!\brief 控制台命令集
//!
//!
//-----------------------------------------------------------------------------
class VENGINE_API ConsoleCommand
{
public:
	DWORD Excute(LPCWSTR szCommand);
	VOID Register(LPCWSTR szName, LPVOID fpFun, LPCWSTR szDesc=NULL, INT nParamNum=1);
	VOID SetOutput(OutputFunction fp) { m_fpOutputFun=fp; }
	VOID SetUnhandle(OutputFunction fp) { m_fpUnhandleFun=fp; }

	ConsoleCommand();
	~ConsoleCommand();

private:
	TSFPTrunk<ConsoleCommand>			m_Trunk;
	TObjRef<Util>						m_pUtil;

	OutputFunction						m_fpOutputFun;
	OutputFunction						m_fpUnhandleFun;
	std::list<tstring>					m_listCommand;
	std::map<DWORD, LPVOID>				m_mapCommandTable;
	std::map<DWORD, INT>				m_mapCommandParam;
	std::list<tstring>::iterator		m_it;
	
	DWORD Help(DWORD);
	DWORD ExitApplication(DWORD);
	DWORD QuitApplication(DWORD);
	DWORD ShowLastError(DWORD);

	DWORD DefaultOutput(LPCTSTR szText);
};






} // namespace vEngine {
