//-----------------------------------------------------------------------------
// File: input
// Desc: interface for directx input
// Auth: Lyp
// Date: 2008-02-21
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "Input_dx8.h"

// 使用其它部件
#include "..\variable\var_container.h"
#pragma comment(lib, "dinput8.lib")

namespace vEngine {
//-----------------------------------------------------------------------------
// Construction/Destruction
//-----------------------------------------------------------------------------
InputDX8::InputDX8()
{
	ZeroMemory(m_KeyBuffer, sizeof(m_KeyBuffer));
	ZeroMemory(m_OldKeyBuffer, sizeof(m_OldKeyBuffer));
	m_InputObj = NULL;
	m_lpInputKeyboard = NULL;
	m_lpInputMouse = NULL;
}


InputDX8::~InputDX8()
{
	Destroy();
}


//-----------------------------------------------------------------------------
// init 
//-----------------------------------------------------------------------------
BOOL InputDX8::Init()
{
	//--创建DirectInput8 对象
	if( FAILED(DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
		IID_IDirectInput8, (void**)&m_InputObj, NULL)) )
		return FALSE;

	//--创建键盘设备对象
	if( FAILED(m_InputObj->CreateDevice(GUID_SysKeyboard, &m_lpInputKeyboard, NULL)) )
		return FALSE;

	//--设置数据格式
	if( FAILED(m_lpInputKeyboard->SetDataFormat(&c_dfDIKeyboard)) )
		return FALSE;

	//--设置协作级别
	HWND hWnd = (HWND)TObjRef<VarContainer>()->GetDword(_T("HWND"));
	if( FAILED(m_lpInputKeyboard->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE|DISCL_FOREGROUND)) )
		return FALSE;

	m_lpInputKeyboard->Acquire();

	// 鼠标
	DIDEVCAPS MouseCapabilities;
	if( FAILED(m_InputObj->CreateDevice(GUID_SysMouse, &m_lpInputMouse, NULL)) )
		return FALSE;

	if(FAILED(m_lpInputMouse->SetDataFormat(&c_dfDIMouse2)))
		return FALSE;
	
	//--默认采用非独占，前景模式
	if( FAILED(m_lpInputMouse->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE|DISCL_FOREGROUND|DISCL_NOWINKEY)) )
		return FALSE;

	m_lpInputMouse->Acquire();

	//--检查鼠标设备的Caps 是否有滑轮,几个Button ;鼠标是否已经与计算机建立连接
	MouseCapabilities.dwSize = sizeof(MouseCapabilities);
	m_lpInputMouse->GetCapabilities(&MouseCapabilities);
	if( !(MouseCapabilities.dwFlags & DIDC_ATTACHED) )
		return FALSE;

	m_dwAxes = MouseCapabilities.dwAxes;
	m_nButtons = (INT)MouseCapabilities.dwButtons;
	//--检查鼠标是否与计算机失去连接
	if( m_lpInputMouse->GetDeviceState(sizeof(m_MouseState),(LPVOID)&m_MouseState) == DIERR_INPUTLOST )
		m_lpInputMouse->Acquire();

	SetAttributes(FALSE, 1.0f);
	return TRUE;
}


//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
VOID InputDX8::Destroy()
{
	if(m_InputObj)
	{
		if(m_lpInputKeyboard)
		{
			m_lpInputKeyboard->Unacquire();
			m_lpInputKeyboard->Release();
			m_lpInputKeyboard = NULL;
		}
		if(m_lpInputMouse)
		{
			m_lpInputMouse->Unacquire();
			m_lpInputMouse->Release();
			m_lpInputMouse = NULL;
		}
		m_InputObj->Release();
	}
	m_InputObj = NULL;
}


//-----------------------------------------------------------------------------
// SetAttributes
//-----------------------------------------------------------------------------
VOID InputDX8::SetAttributes(BOOL bInvert, FLOAT fNewSensitivity)
{
	m_bInverted = bInvert;
	m_fSensitivity = fNewSensitivity;
}


//-----------------------------------------------------------------------------
// Update
//-----------------------------------------------------------------------------
VOID InputDX8::Update()
{
	// 键盘
	memcpy(m_OldKeyBuffer, m_KeyBuffer, sizeof(m_KeyBuffer));
	ZeroMemory(m_KeyBuffer, sizeof(m_KeyBuffer));

	//--默认采用立即数据方式取得键盘数据
	if( FAILED(m_lpInputKeyboard->GetDeviceState(sizeof(m_KeyBuffer), (LPVOID)&m_KeyBuffer)) )
		while( DIERR_INPUTLOST == m_lpInputKeyboard->Acquire() );

	// 鼠标
	m_OldMouseState = m_MouseState;
	ZeroMemory(&m_MouseState, sizeof(m_MouseState));
	if( FAILED(m_lpInputMouse->GetDeviceState(sizeof(m_MouseState), &m_MouseState)) )
		while( DIERR_INPUTLOST == m_lpInputMouse->Acquire() );
}






} // namespace vEngine {
