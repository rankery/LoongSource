#pragma once

#include "login_define.h"

enum EnumFatigueState
{
	EFS_Null		= -1,
	EFS_BEGIN		= 0, 

	EFS_HEALTHY		= 0,
	EFS_WEARY		= 1,
	EFS_SICK		= 2,

	EFS_END			,
	EFS_NUM			 = EFS_END,
};

#define EFS_VALID( state ) ((state) >= EFS_BEGIN && (state) < EFS_END)

// 一分钟有多少秒，以方便调试
#define SECOND_PER_MINITE 60

// 多长时间保存一次
#define TIME_PER_SAVE_MINITE 5

struct tagFatigueNotify
{
	DWORD				dwAccountID;
	DWORD				dwState;
	DWORD				dwAccOnTimeMin;
};


struct tagFatigueInfo
{
	DWORD				dwAccountNameCrc;
	INT					nAccOnLineTimeMin;
	INT					nAccOffLineTimeMin;
	BOOL				bOnLine;
	tagFatigueInfo()
	{
		dwAccountNameCrc = 0;
		nAccOnLineTimeMin = 0;
		nAccOffLineTimeMin = 0;
		bOnLine = FALSE;
		bLastOnLine = FALSE;
		bNeedSave = FALSE;
		eLastState = EFS_Null;
	}

	VOID Update()
	{
		// 在线
		if (bOnLine)
		{
			nAccOnLineTimeMin++;

			if (0 == (nAccOnLineTimeMin % TIME_PER_SAVE_MINITE))
				bNeedSave = TRUE;
		}
		// 不在线
		else
		{
			if (nAccOnLineTimeMin != 0)
			{
				nAccOffLineTimeMin++;
				// 累计离线时间超过５小时
				if (nAccOffLineTimeMin >= 60 * 5)
				{
					nAccOffLineTimeMin = 0;
					nAccOnLineTimeMin = 0;
				}

				if (0 == (nAccOffLineTimeMin % TIME_PER_SAVE_MINITE))
					bNeedSave = TRUE;
			}
		}
	}

	BOOL				NeedNotify();
	BOOL				NeedSave()	{	return bNeedSave;	}
	VOID				SetSaved()	{	bNeedSave = FALSE;	}
	EnumFatigueState	Status();

	EnumFatigueState	eLastState;
	BOOL				bLastOnLine;
	BOOL				bNeedSave;
};

inline BOOL tagFatigueInfo::NeedNotify()
{
	if (!bOnLine)
		return FALSE;

	// 第一次登陆通知
	if(!bLastOnLine && bOnLine)
		return TRUE;

	// 状态变化时通知
	EnumFatigueState eNewState = Status();
	if (eNewState != eLastState)
	{
		eLastState = eNewState;
		return TRUE;
	}

	BOOL bNotify = FALSE;

	// 固定时间点通知
	switch(eLastState)
	{
	case EFS_HEALTHY:
		bNotify = !(nAccOnLineTimeMin % 60);
		break;
	case EFS_WEARY:
		bNotify = !(nAccOnLineTimeMin % 30);
		break;
	case EFS_SICK:
		bNotify = !(nAccOnLineTimeMin % 15);
		break;
	}

	return bNotify;
}

inline EnumFatigueState tagFatigueInfo::Status()
{
	if (nAccOnLineTimeMin >= 0 && nAccOnLineTimeMin < 180)
	{
		return EFS_HEALTHY;
	}
	else if (nAccOnLineTimeMin >= 180 && nAccOnLineTimeMin < 300)
	{
		return EFS_WEARY;
	}
	else
	{
		return EFS_SICK;
	}
}

