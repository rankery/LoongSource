#pragma once
#include "../ServerDefine/gm_tool_define.h"
enum EDoubleAct
{
	EDA_Null		= -1,
	EDA_ExpStart	= 0,
	EDA_ExpStop		= 1,
	EDA_LootStart	= 10,
	EDA_LootStop	= 11,
};

inline EDoubleAct TransEDT2EDA(EDoubleType	eEdt, BOOL bStart)
{
	if(eEdt == EDoubleType_Exp)
	{
		if(bStart)	return EDA_ExpStart;
		else		return EDA_ExpStop;
	}
	else if (eEdt == EDoubleType_Item)
	{
		if(bStart)	return EDA_LootStart;
		else		return EDA_LootStop;
	}
	return EDA_Null;
}

inline EDoubleType TransEDA2EDT(EDoubleAct eEda)
{
	if (eEda == EDA_ExpStart ||	eEda == EDA_ExpStop)
	{
		return EDoubleType_Exp;
	}
	else if (eEda == EDA_LootStart || eEda == EDA_LootStop)
	{
		return EDoubleType_Item;
	}
	return EDoubleType_NULL;
}

inline BOOL TransEDA2BStart(EDoubleAct eEda)
{
	if (eEda == EDA_ExpStart ||	eEda == EDA_LootStart)
	{
		return TRUE;
	}
	else// if (eEda == EDA_ExpStop || eEda == EDA_LootStop)
	{
		return FALSE;
	}
}
