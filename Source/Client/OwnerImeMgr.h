#pragma once

#pragma comment ( lib, "imm32.lib" )
#include <windows.h>
#include <imm.h>

class OwnerImeMgr
{
public:
	OwnerImeMgr(void);
	~OwnerImeMgr(void);
	void Init();
	static OwnerImeMgr* Inst();

	void Destroy();

	void OnWindowsMsg(DWORD dwMsg, DWORD dwParam1, DWORD dwParam2);

	bool IfOwnerIme() { return m_bOwnerIme; }
	void EnableOwnerIme();				//允许输入法，此后玩家可以用热键呼出ime
	void DisableOwnerIme();				//关闭并禁止输入法，如ime已经打开则关闭，此后玩家不能用热键呼出ime
	void NextOwnerIme();					//切换到下一种输入法，必须EnableIme后才有效
	void SharpOwnerIme( HWND hWnd );		//切换全角/半角
	void SymbolOwnerIme( HWND hWnd );	//切换中/英文标点

	//状态函数
	TCHAR* GetOwnerImeName();				//得到输入法名字，如果当前是英文则返回NULL
	bool IfOwnerImeSharp();				//是否全角
	bool IfOwnerImeSymbol();				//是否中文标点
	void GetOwnerImeInput( TCHAR **pszCompStr, TCHAR **pszCompReadStr, TCHAR *pnImeCursor, TCHAR **pszCandList );

	//必须在消息中调用的函数，如果返回是true，则窗口函数应直接返回0，否则应传递给DefWindowProc
	bool OnWM_INPUTLANGCHANGEREQUEST();
	bool OnWM_INPUTLANGCHANGE( HWND hWnd );
	bool OnWM_IME_SETCONTEXT(){ return true; }
	bool OnWM_IME_STARTCOMPOSITION(){ return true; }
	bool OnWM_IME_ENDCOMPOSITION(){ return true; }
	bool OnWM_IME_NOTIFY( HWND hWnd, WPARAM wParam );
	bool OnWM_IME_COMPOSITION( HWND hWnd, LPARAM lParam );

private:		
	void ConvertCandList( CANDIDATELIST* pCandList, TCHAR* pszCandList );	//将选字表整理成串


private:
	TSFPTrunk<OwnerImeMgr>		m_Trunk;
	TObjRef<Util>				m_pUtil;
	TObjRef<NetCmdMgr>			m_pCmdMgr;
	TObjRef<NetSession>			m_pSession;
	TObjRef<GameFrameMgr>		m_pFrameMgr;

	bool						m_bOwnerIme;								//ime允许标志
	TCHAR						m_szCompStr[ X_LONG_NAME ];					//存储转换后的串
	TCHAR						m_szCompReadStr[ X_LONG_NAME ];				//存储输入的串
	TCHAR						m_szCandList[ X_LONG_NAME ];				//存储整理成字符串选字表
	int							m_nImeCursor;								//存储转换后的串中的光标位置
	CANDIDATELIST*				m_lpCandList;								//存储标准的选字表
	TCHAR						m_szImeName[64];							//存储输入法的名字
	bool						m_bImeSharp;								//全角标志
	bool						m_bImeSymbol;								//中文标点标志
};