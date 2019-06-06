#pragma once

//--碰撞返回值定义---------------------------------------------------------
enum ENavResult
{
	ENR_Failed=0,				//调用失败，可能是上层逻辑有问题
	ENR_Arrive,					//到达终点
	ENR_Blocking,				//被挡住
	ENR_WillDrop,				//将掉落
	ENR_WillVDrop,				//将垂直掉落
	ENR_WillSlide,				//将滑落
	ENR_WillSwim,				//将游泳
	ENR_WillCarry,				//将着陆到载具上
	ENR_WillOnWater,			//将水上站立
	ENR_Landed,					//跳跃着陆
	ENR_SwimLanded,				//游泳搁浅
	ENR_CarryLanded,			//乘载着陆
	ENR_ToBeContinued,			//可以继续碰撞
};
