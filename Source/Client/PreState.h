#pragma once
#include "..\WorldDefine\base_define.h"
#include "..\WorldDefine\move_define.h"
#include "CombatEvent.h"

class FSM_LP;
class LocalPlayer;
/** 前置状态
*/
struct tagPreState
{
	enum EState					//状态
	{
		EState_None=-1,			//无
		EState_MouseMove,		//鼠标移动/游泳
		EState_KeyboardMove,	//键盘移动/游泳
		EState_Pursue,			//追踪
	};

	EState					state;
	tagMouseMoveEvent		mouseMove;
	tagKeyboardMoveEvent	keyboardMove;
	tagPursueEvent			pursue;

	/** 构造函数
	*/
	tagPreState();
	/** 切换到前置状态
	*/
	void Change2LowerState(FSM_LP* pFSM,LocalPlayer* pRole,bool bSwim,bool bOnWater);
	/** 前置状态是否正在移动中
	*/
	bool IsMoving();
};