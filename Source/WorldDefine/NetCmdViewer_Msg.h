#pragma once


#pragma pack(push, 1)
struct NetCmdViewer_Msg
{
	DWORD			dwMsgSize;
	tagDWORDTime	liTime;
	DWORD			accountID;
	DWORD			roleID;
	tagNetCmd		netCmd;
};
#pragma pack(pop)