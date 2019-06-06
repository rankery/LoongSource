//-----------------------------------------------------------------------------
//!\file msg_script.h
//!
//!\date 2009-4-16
//!
//!\brief 客户端与服务器之间脚本消息
//-----------------------------------------------------------------------------
#pragma once

#pragma pack(push, 1)

enum EMsgUnitType;
enum EDlgOption;

//-----------------------------------------------------------------------------
// NS_ScriptBroad中dwMisc字段的位类型定义
//-----------------------------------------------------------------------------
CMD_START(NS_MsgBlob)
	BYTE byData[1];
CMD_END

//-----------------------------------------------------------------------------
// 客户端对话框发给服务的缺省消息
//-----------------------------------------------------------------------------
CMD_START(NC_DlgDefaultMsg)
	EMsgUnitType	eDlgTarget;				// 客户端对话框操作所对应的目标（任务，物品， 玩家）
	DWORD			dwTargetID;				// 目标ID
	EDlgOption		eDlgOption;				// 玩家返回的对话框选项
CMD_END

//只有单RoleID
CMD_START(NS_MsgBroadCastBlob)
	BYTE byData[1];
CMD_END

//-----------------------------------------------------------------------------
// 客户端触发服务器脚本的缺省消息
//-----------------------------------------------------------------------------
CMD_START(NC_DefaultRequest)
	EMsgUnitType	eDlgTarget;				// 触发服务器脚本的对应目标（固定活动）
	DWORD			dwTargetID;				// 目标ID
	DWORD			dwEventType;			// 触发的事件类型
CMD_END

#pragma pack(pop)