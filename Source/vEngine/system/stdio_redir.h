//-----------------------------------------------------------------------------
// File: stdio_redir
// Auth: Lyp
// Date: 2007/11/14
// Last: 2007/11/14
//-----------------------------------------------------------------------------
#pragma once

namespace vEngine{
//-----------------------------------------------------------------------------
//!\brief stdio 重定向
//!
//!
//-----------------------------------------------------------------------------
class VENGINE_API StdioRedir
{
public:
	BOOL Init();
	VOID Destroy();

	LPBYTE	Recv(DWORD& dwSize);
	BOOL	Send(LPVOID pMsg, DWORD dwMsgSize);

	//! 对应于Recv(),因为内存是内部分配的，所以应该交还内部来释放
	VOID FreeRecved(LPBYTE pMsg);

private:

	HANDLE		m_hProcess;
	HANDLE		m_hStdoutRdDup;
	HANDLE		m_hStdinWrDup;
};

} // namespace vEngine {