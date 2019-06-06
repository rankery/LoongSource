#include "stdafx.h"
#include "OwnerImeMgr.h"
#include "OwnerImeEvent.h"
#include "OwnerImeFrame.h"

OwnerImeMgr::OwnerImeMgr( void ):m_Trunk(this)
{
	m_lpCandList	= NULL;
}

OwnerImeMgr::~OwnerImeMgr( void )
{

}

OwnerImeMgr g_OwnerImeMgr;
OwnerImeMgr* OwnerImeMgr::Inst()
{
	return &g_OwnerImeMgr;
}

void OwnerImeMgr::Init()
{
	m_pUtil			= TObjRef<Util>();
	m_pCmdMgr		= TObjRef<NetCmdMgr>();
	m_pSession		= TObjRef<NetSession>();
	m_pFrameMgr		= TObjRef<GameFrameMgr>();
	
	DisableOwnerIme();
}

void OwnerImeMgr::Destroy()
{
	DisableOwnerIme();
	if( m_lpCandList ){
		GlobalFree( (HANDLE)m_lpCandList );
		m_lpCandList = NULL;
	}
}

void OwnerImeMgr::EnableOwnerIme()
{
	m_bOwnerIme = true;
}

void OwnerImeMgr::DisableOwnerIme()
{
	while( ImmIsIME( GetKeyboardLayout( 0 )) && m_bOwnerIme)
		ActivateKeyboardLayout(( HKL )HKL_NEXT, 0 );//如果ime打开通过循环切换到下一个关闭
	m_bOwnerIme = false;
	m_szImeName[0] = 0;
	m_szCompStr[0] = 0;
	m_szCompReadStr[0] = 0;
	m_szCandList[0] = 0;
	m_nImeCursor = 0;
}

void OwnerImeMgr::NextOwnerIme()
{
	if( m_bOwnerIme )
		ActivateKeyboardLayout(( HKL )HKL_NEXT, 0 );
}

void OwnerImeMgr::SharpOwnerIme( HWND hWnd )
{
	ImmSimulateHotKey( hWnd, IME_CHOTKEY_SHAPE_TOGGLE );
}

void OwnerImeMgr::SymbolOwnerIme( HWND hWnd )
{
	ImmSimulateHotKey( hWnd, IME_CHOTKEY_SYMBOL_TOGGLE );
}

bool OwnerImeMgr::OnWM_INPUTLANGCHANGEREQUEST()
{
	return !m_bOwnerIme;								//如果禁止ime则返回false，此时窗口函数应返回0，否则DefWindowProc会打开输入法
}

bool OwnerImeMgr::OnWM_INPUTLANGCHANGE( HWND hWnd )
{
	if(!m_bOwnerIme)
		return false;
	//ime改变
	HKL hKL = GetKeyboardLayout( 0 );
	if( ImmIsIME( hKL ))
	{
		HIMC hIMC = ImmGetContext( hWnd );
		ImmEscape( hKL, hIMC, IME_ESC_IME_NAME, m_szImeName );						//取得新输入法名字
		DWORD dwConversion, dwSentence;
		ImmGetConversionStatus( hIMC, &dwConversion, &dwSentence );
		m_bImeSharp = ( dwConversion & IME_CMODE_FULLSHAPE )? true : false;			//取得全角标志
		m_bImeSymbol = ( dwConversion & IME_CMODE_SYMBOL )? true : false;			//取得中文标点标志
		ImmReleaseContext( hWnd, hIMC );
		tagOwnerImeShow evt;
		evt.bShow = TRUE;
		m_pFrameMgr->SendEvent(&evt);
	}
	else		//英文输入
	{
		m_szImeName[ 0 ] = 0;
		tagOwnerImeShow evt;
		evt.bShow = FALSE;
		m_pFrameMgr->SendEvent(&evt);
	}
	return false;								//总是返回false，因为需要窗口函数调用DefWindowProc继续处理
}

bool OwnerImeMgr::OnWM_IME_NOTIFY( HWND hWnd, WPARAM wParam )
{
	if(!m_bOwnerIme)
		return false;

	HIMC hIMC;
	DWORD dwSize;
	DWORD dwConversion, dwSentence;
	switch( wParam ){
		case IMN_SETCONVERSIONMODE:		//全角/半角，中/英文标点改变
			{
				hIMC = ImmGetContext( hWnd );
				ImmGetConversionStatus( hIMC, &dwConversion, &dwSentence );
				m_bImeSharp = ( dwConversion & IME_CMODE_FULLSHAPE )? true : false;
				m_bImeSymbol = ( dwConversion & IME_CMODE_SYMBOL )? true : false;
				ImmReleaseContext( hWnd, hIMC );
			}
			break;
		case IMN_OPENCANDIDATE:			//进入选字状态
		case IMN_CHANGECANDIDATE:		//选字表翻页
			{
				hIMC = ImmGetContext( hWnd );
				if( m_lpCandList )
				{
					GlobalFree( (HANDLE)m_lpCandList );
					m_lpCandList = NULL;
				}
				//释放以前的选字表
				if( dwSize = ImmGetCandidateList( hIMC, 0, NULL, 0 ))
				{
					m_lpCandList = (LPCANDIDATELIST)GlobalAlloc( GPTR, dwSize );
					if( m_lpCandList )
						ImmGetCandidateList( hIMC, 0, m_lpCandList, dwSize );
				}
				
				//得到新的选字表
				ImmReleaseContext( hWnd, hIMC );
				if( m_lpCandList )
					ConvertCandList( m_lpCandList, m_szCandList );				//选字表整理成串
				
				tagOwnerImeWordEvent evt;
				evt.strText = m_szCandList;
				m_pFrameMgr->SendEvent(&evt);
			}
			break;
		case IMN_CLOSECANDIDATE:		//关闭选字表
			{
				if( m_lpCandList )
				{
					GlobalFree( (HANDLE)m_lpCandList );
					m_lpCandList = NULL;
				}
				m_szCandList[0] = 0;
			}
			break;
	}
	return true;						//总是返回true，防止ime窗口打开
}

bool OwnerImeMgr::OnWM_IME_COMPOSITION( HWND hWnd, LPARAM lParam )
{
	if(!m_bOwnerIme)
		return false;

	HIMC hIMC;
	DWORD dwSize;
	hIMC = ImmGetContext( hWnd );
	if( lParam & GCS_COMPSTR )
	{
		dwSize = ImmGetCompositionString( hIMC, GCS_COMPSTR, (LPVOID)m_szCompStr, sizeof( m_szCompStr ));
		dwSize /= sizeof(TCHAR);
		m_szCompStr[ dwSize ] = 0;
	}
	
	//取得szCompStr
	if( lParam & GCS_COMPREADSTR )
	{
		dwSize = ImmGetCompositionString( hIMC, GCS_COMPREADSTR, (LPVOID)m_szCompReadStr, sizeof( m_szCompReadStr ));
		dwSize /= sizeof(TCHAR);
		m_szCompReadStr[ dwSize ] = 0;
	}
	
	//取得szCompReadStr
	if( lParam & GCS_CURSORPOS )
	{
		m_nImeCursor = 0xffff & ImmGetCompositionString( hIMC, GCS_CURSORPOS, NULL, 0 );
	}

	//设置自定义输入法输入界面
	tagOwnerInputWordEvent evt;
	evt.strText = m_szCompStr;
	m_pFrameMgr->SendEvent(&evt);
	
	//取得nImeCursor
	if( lParam & GCS_RESULTSTR )
	{
		TBYTE str[ X_LONG_NAME ];
		dwSize = ImmGetCompositionString( hIMC, GCS_RESULTSTR, (LPVOID)str, sizeof( str ));//取得汉字输入串
		dwSize /= sizeof(TCHAR);
		str[ dwSize ] = 0;
		TBYTE *p = str;
		while( *p )
			PostMessage( hWnd, WM_CHAR, (WPARAM)(*p++), 1 );//转成WM_CHAR消息

		//清空自定义输入法输入界面
		tagOwnerInputWordClear evt;
		m_pFrameMgr->SendEvent(&evt);
	}
	ImmReleaseContext( hWnd, hIMC );
	return true;							//总是返回true，防止ime窗口打开
}

TCHAR* OwnerImeMgr::GetOwnerImeName()
{
	return m_szImeName[0]? m_szImeName : NULL;
}

//是否全角
bool OwnerImeMgr::IfOwnerImeSharp()
{
	return m_bImeSharp;
}

//是否中文标点
bool OwnerImeMgr::IfOwnerImeSymbol()
{
	return m_bImeSymbol;
}

void OwnerImeMgr::GetOwnerImeInput( TCHAR **pszCompStr, TCHAR **pszCompReadStr, TCHAR *pnImeCursor, TCHAR **pszCandList )
{
	if( pszCompStr )
		*pszCompStr = m_szCompStr;
	if( pszCompReadStr )
		*pszCompReadStr = m_szCompReadStr;
	if( pnImeCursor )
		*pnImeCursor = m_nImeCursor;
	if( pszCandList )
		*pszCandList = m_szCandList;
}

void OwnerImeMgr::ConvertCandList( CANDIDATELIST* pCandList, TCHAR* pszCandList )
{
	unsigned int i;
	if( pCandList->dwCount < pCandList->dwSelection )
	{
		pszCandList[ 0 ] = 0;
		return;
	}
	char* a;
	TCHAR* b;

	for( i = 0; ( i < pCandList->dwCount - pCandList->dwSelection )&&( i < pCandList->dwPageSize ); i++ )
	{
		*pszCandList++ = ( i % 10 != 9 )? i % 10 + _T('1') : _T('0');				//每项对应的数字键
		*pszCandList++ = _T(' ');													//用'.'分隔
		a = (char*)pCandList + pCandList->dwOffset[ pCandList->dwSelection + i ];	//根据偏移找到字符串的地址
		b = (TCHAR*)a;																//将指针转换成宽字
		_tcscpy( pszCandList, b );													//每项实际的内容
		pszCandList += _tcslen( pszCandList );
		*pszCandList++ = _T('\t');	
	}
	*( pszCandList )= _T('\0');	
}

void OwnerImeMgr::OnWindowsMsg( DWORD dwMsg, DWORD dwParam1, DWORD dwParam2 )
{
	if( !m_bOwnerIme )
		return;

	switch(dwMsg)
	{
	case WM_IME_SETCONTEXT:
		{
			OnWM_IME_SETCONTEXT();
		}
		break;
	case WM_INPUTLANGCHANGEREQUEST:
		{
			OnWM_INPUTLANGCHANGEREQUEST();
		}
		break;
	case WM_INPUTLANGCHANGE:
		{
			HWND hWnd = (HWND)TObjRef<VarContainer>()->GetDword(_T("HWND"));
			OnWM_INPUTLANGCHANGE( hWnd );
		}
		break;
	case WM_IME_STARTCOMPOSITION:
		{
			OnWM_IME_STARTCOMPOSITION();
		}
		break;
	case WM_IME_ENDCOMPOSITION:
		{
			OnWM_IME_ENDCOMPOSITION();
		}
		break;
	case WM_IME_NOTIFY:
		{
			HWND hWnd = (HWND)TObjRef<VarContainer>()->GetDword(_T("HWND"));
			OnWM_IME_NOTIFY( hWnd, dwParam1 );
		}
		break;
	case WM_IME_COMPOSITION:
		{
			HWND hWnd = (HWND)TObjRef<VarContainer>()->GetDword(_T("HWND"));
			OnWM_IME_COMPOSITION( hWnd, dwParam2 );
		}
		break;
	}
}

