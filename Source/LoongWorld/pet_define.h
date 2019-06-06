//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: pet_define.h
// author: xlguo
// actor:
// data: 2009-07-13
// last:
// brief: 宠物原型定义
//-------------------------------------------------------------------------------------------------------
#pragma once

struct tagPetProto
{
	DWORD	dwTypeID;		// 宠物TypeID 
	INT		nType3;			// 宠物类型
	INT		nRoleLvlLim;	// 任务携带等级
	INT		nMountable;		// 能否骑乘
	INT		nMountSpeed;	// 骑乘速度

	Vector3	vSize;			// 碰撞尺寸
	FLOAT	fScale;			// 骑乘尺寸缩放

	BOOL	bBind;			// 是否绑定

	INT		nAptitudeMin[EIQ_End];
	INT		nAptitudeMax[EIQ_End];

};

