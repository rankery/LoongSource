#include "..\StdAfx.h"
#include ".\gwin32app.h"
#include ".\ginput.h"
#include "..\..\Cool3D\Util\DebugMisc.h"
using namespace Cool3D;

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

class GInput::Member
{
public:
	LPDIRECTINPUT8		  m_InputObj;
	LPDIRECTINPUTDEVICE8  m_lpInputKeyboard; 
	LPDIRECTINPUTDEVICE8  m_lpInputMouse; 
	DIMOUSESTATE2		  m_MouseState;
	DIMOUSESTATE2		  m_oldMouseState;
	bool				  m_bInverted; 
	float                 m_fSensitivity; 

	DWORD				  m_dwAxes,
		m_dwButtons;
	char				  m_KeyBuffer[256];
	char				  m_oldKeyBuffer[256];
public:
	Member()
	{
		memset(this,0,sizeof(GInput::Member));
		m_fSensitivity=1.0f;
	}

	bool CreateKeyboard()
	{
		//--创建键盘设备对象
		if(FAILED(m_InputObj->CreateDevice(GUID_SysKeyboard, &m_lpInputKeyboard, NULL)))
			return false;
		//--设置数据格式
		if(FAILED(m_lpInputKeyboard->SetDataFormat(&c_dfDIKeyboard)))
			return false;
		//--设置协作级别	
		if(FAILED(m_lpInputKeyboard->SetCooperativeLevel(GWin32App::Inst()->GetHWnd(), DISCL_NONEXCLUSIVE|DISCL_FOREGROUND)))
		{
			return false;
		}	

		m_lpInputKeyboard->Acquire();
		return true;
	}

	bool CreateMouse()
	{
		DIDEVCAPS MouseCapabilities;

		if(FAILED(m_InputObj->CreateDevice(GUID_SysMouse, &m_lpInputMouse, NULL)))
			return false;
		if(FAILED(m_lpInputMouse->SetDataFormat(&c_dfDIMouse2)))
			return false;
		//--默认采用非独占，前景模式
		if(FAILED(m_lpInputMouse->SetCooperativeLevel(GWin32App::Inst()->GetHWnd(), DISCL_NONEXCLUSIVE|DISCL_FOREGROUND)))
		{	
			m_lpInputMouse->Release();
			m_lpInputMouse= NULL;
			return false;
		}


		m_lpInputMouse->Acquire();


		//--检查鼠标设备的Caps 是否有滑轮,几个Button ;鼠标是否已经与计算机建立连接

		MouseCapabilities.dwSize = sizeof(MouseCapabilities);
		m_lpInputMouse->GetCapabilities(&MouseCapabilities);
		if(!(MouseCapabilities.dwFlags & DIDC_ATTACHED))
			return false;

		m_dwAxes = MouseCapabilities.dwAxes;
		m_dwButtons = MouseCapabilities.dwButtons;
		//--检查鼠标是否与计算机失去连接
		if(m_lpInputMouse->GetDeviceState(sizeof(m_MouseState),(LPVOID)&m_MouseState) == DIERR_INPUTLOST)
		{
			m_lpInputMouse->Acquire();
		}

		SetAttributes(false, 1.0f);
		return true;

	}

	void SetAttributes(bool bInvert, float fNewSensitivity)
	{
		m_bInverted = bInvert;
		m_fSensitivity = fNewSensitivity;
	}

	void Destroy()
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

};


GInput* GInput::Inst()
{
	static GInput g_input;
	return &g_input;
}

GInput::GInput(void)
{
	m_p=new Member;
}

GInput::~GInput(void)
{
	delete m_p;
}

void GInput::Destroy()
{
	m_p->Destroy();
}

bool GInput::CreateInput()
{
	//--创建DirectInput8 对象
	if(FAILED(DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
		IID_IDirectInput8, (void**)&m_p->m_InputObj,NULL)))
		return false;

	if(!m_p->CreateKeyboard())
		return false;

	if(!m_p->CreateMouse())
		return false;

	return true;
}

void GInput::UpdateKBInput()
{
	memcpy(m_p->m_oldKeyBuffer,m_p->m_KeyBuffer,sizeof(m_p->m_KeyBuffer));

	ZeroMemory(m_p->m_KeyBuffer, sizeof(char)*256);
	if(m_p->m_lpInputKeyboard != NULL)
	{
		HRESULT hr;
		//--默认采用立即数据方式取得键盘数据
		if(FAILED( hr = m_p->m_lpInputKeyboard->GetDeviceState(sizeof(m_p->m_KeyBuffer), (LPVOID)&m_p->m_KeyBuffer)))
		{	
			hr = m_p->m_lpInputKeyboard->Acquire();

			while(hr == DIERR_INPUTLOST)
				m_p->m_lpInputKeyboard->Acquire();

			if( hr == DIERR_OTHERAPPHASPRIO || hr == DIERR_NOTACQUIRED ) 
			{
				//--可能其它应用程序的优先级别较高，或者窗口最小化，或者应用程序在做
				//其它的转换工作,稍后再试
			}
			return;			
		}
	}		
	return ;

}

void GInput::UpdateMouseInput()
{
	if(m_p->m_lpInputMouse != NULL)
	{
		m_p->m_oldMouseState=m_p->m_MouseState;

		HRESULT hr;
		ZeroMemory(&m_p->m_MouseState, sizeof(m_p->m_MouseState));
		if(FAILED(hr = m_p->m_lpInputMouse->GetDeviceState(sizeof(m_p->m_MouseState), &m_p->m_MouseState)))
		{
			hr = m_p->m_lpInputMouse->Acquire();
			while(hr == DIERR_INPUTLOST)
				hr = m_p->m_lpInputMouse->Acquire();
			return ;
		}
	}
	return ;
}

bool GInput::KeyDown(DIK key)
{
	ASSERT(key>=0 && key<256);

	bool keyDown = false;
	if(m_p->m_KeyBuffer[key] & 0x80)//键盘键
	{
		keyDown= KeyPressed(key & 0x7FFFFFFF);
	}

	return keyDown;
}

bool GInput::KeyDownFirst(DIK key)
{
	ASSERT(key>=0 && key<256);

	bool bKeyDownLast = ((m_p->m_oldKeyBuffer[key] & 0x80) != 0);
	bool bKeyDownNow = ((m_p->m_KeyBuffer[key] & 0x80) != 0);

	return !bKeyDownLast && bKeyDownNow;
}

bool GInput::KeyPressed(DIK key)
{
	ASSERT(key>=0 && key<256);
	if(m_p->m_KeyBuffer[key] & 0x80)
		return true;
	
	return false;
}

bool GInput::MouseDown(BYTE buttonIndex)
{
	ASSERT(buttonIndex<8);
	ASSERT(buttonIndex < m_p->m_dwButtons);
	if(m_p->m_MouseState.rgbButtons[buttonIndex] & 0x80)
	{
		return true;
	}
	return false;
}

bool GInput::MouseUp(BYTE buttonIndex)
{
	ASSERT(buttonIndex<8);
	ASSERT(buttonIndex < m_p->m_dwButtons);
	if( (m_p->m_oldMouseState.rgbButtons[buttonIndex] & 0x80) 
		&& !(m_p->m_MouseState.rgbButtons[buttonIndex] & 0x80) )
	{
		return true;
	}
	return false;
}

bool GInput::MouseDownFirst(BYTE buttonIndex)
{
	ASSERT(buttonIndex<8);
	ASSERT(buttonIndex < m_p->m_dwButtons);
	if( !(m_p->m_oldMouseState.rgbButtons[buttonIndex] & 0x80) 
		&& (m_p->m_MouseState.rgbButtons[buttonIndex] & 0x80) )
	{
		return true;
	}
	return false;
}

bool GInput::IsMouseMove()
{
	if(GetRelativeX() != 0.0f || GetRelativeY() != 0.0f || GetRelativeZ() != 0.0f)
		return true;
	else 
		return false;
}

void GInput::SetAttributes(bool bInvert, float fNewSensitivity)
{
	m_p->SetAttributes(bInvert,fNewSensitivity);
}


float GInput::GetAbsouluteX()
{

	POINT position;	
	::GetCursorPos(&position);
	return (float)(position.x * m_p->m_fSensitivity);

}

float GInput::GetAbsouluteY()
{
	POINT position;
	::GetCursorPos(&position);
	return (float)(position.y * m_p->m_fSensitivity);

}
//yangfei_begin
float GInput::GetRelativeX()
{
	return (float)m_p->m_MouseState.lX * m_p->m_fSensitivity;
}

float GInput::GetRelativeY()
{
	return (float)m_p->m_MouseState.lY * m_p->m_fSensitivity;
}
//yangfei_end

float GInput::GetRelativeZ()
{
	return (float)m_p->m_MouseState.lZ * m_p->m_fSensitivity;
}

POINT GInput::GetCursorPos()
{
	POINT pt;
	::GetCursorPos(&pt);
	GWin32App::Inst()->ScreenToClient(&pt);
	return pt;
}
