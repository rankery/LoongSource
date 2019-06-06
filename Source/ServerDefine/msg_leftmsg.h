#pragma once

#pragma pack(push)
#pragma pack(1)

//-------------------------------------------------------------------------
// 留言是一条一条保存，一起读上来的
//-------------------------------------------------------------------------
struct tagDBLeftMsg
{
	DWORD dwDateTime;
	DWORD dwRoleID;
	BYTE byData[1];
};

const INT MAX_LEFTMSG_NUM = 50;

//-------------------------------------------------------------------------
// 保存留言
//-------------------------------------------------------------------------
CMD_START(NDBC_InsertLeftMsg)
	tagDBLeftMsg data;
CMD_END;

CMD_START(NDBS_InsertLeftMsg)
CMD_END;

//-------------------------------------------------------------------------
// 更新留言
//-------------------------------------------------------------------------
// CMD_START(NDBC_UpdateLeftMsg)
// 	DWORD dwOldDateTime;
// 	tagDBLeftMsg data;
// CMD_END;
// 
// CMD_START(NDBS_UpdateLeftMsg)
// CMD_END;


//-------------------------------------------------------------------------
// 读取留言
//-------------------------------------------------------------------------
CMD_START(NDBC_LoadLeftMsg)	
	DWORD dwRoleID;
CMD_END;

CMD_START(NDBS_LoadLeftMsg)
	DWORD dwErrCode;
	DWORD dwRoleID;
	BYTE byData[1];
CMD_END;

//-------------------------------------------------------------------------
// 清空留言
//-------------------------------------------------------------------------
CMD_START(NDBC_ClearLeftMsg)	
	DWORD dwRoleID;
CMD_END;

CMD_START(NDBS_ClearLeftMsg)
	DWORD dwErrCode;
CMD_END;


#pragma pack(pop)
