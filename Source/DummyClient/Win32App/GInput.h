#pragma once

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#include <dinput.h>	//For DIK define

typedef unsigned short DIK;

/**	\class GInput
	\brief 游戏输入设备
	\remarks 
	\par DInput设备对象 默认采用立即数据,前景模式，只能产生一个实例.
	\par key值见<dinput.h>中的DIK_XXX定义
*/

class GInput
{
public:
	GInput(void);
	virtual ~GInput(void);
	static GInput* Inst(void);

	void UpdateKBInput(void);
	void UpdateMouseInput(void);

	/** 检查key键是否被按下
	*/
	bool KeyPressed(DIK key);
	bool KeyDown(DIK key);
	bool KeyDownFirst(DIK key);

	/** 检查哪个鼠标键按下
		\param 左键是0,右键是1
	*/
	bool MouseDown(BYTE buttonIndex);
	bool MouseUp(BYTE buttonIndex);
	bool IsMouseMove();
	bool MousePressed(BYTE buttonIndex);
	bool MouseDownFirst(BYTE buttonIndex);

	/** Destroy所有DirectInput设备对象
		必须手动调用,确保所有DirectInput对象被成功销毁
	*/
    void Destroy(void);
	
	void SetAttributes(bool bInvert, float fNewSensitivity);

	POINT GetCursorPos();
	
	/** 取得三维空间中鼠标的x坐标
	*/
	float GetRelativeX(void);
	/** 取得三维空间中鼠标的y坐标
	*/
	float GetRelativeY(void);
	/** 取得三维空间中鼠标的z坐标
	*/
	float GetRelativeZ(void);
	/** 取得二维平面坐标中鼠标x坐标
	*/
	float GetAbsouluteX(void);
	/** 取得二维平面坐标中鼠标y坐标
	*/
	float GetAbsouluteY(void);
private:
	friend class GWin32App;
	/** 创建DirectInput8 对象, 并创建默认的 Keyboard , Mouse 设备
	*/
	bool CreateInput(void);
private:
	class Member;
	Member	*m_p;
};
