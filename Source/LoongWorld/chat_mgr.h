#pragma once
#include "../WorldDefine/chat_define.h"
#include "../WorldDefine/chat.h"
#include "../WorldDefine/msg_show_item.h"
#include "channel_mgr.h"


//-------------------------------------------------------------------------
// 消息命令
//-------------------------------------------------------------------------
class MsgCmd
{
public:
	//---------------------------------------------------------------------
	// 析构函数
	//---------------------------------------------------------------------
	virtual ~MsgCmd(){}

	//---------------------------------------------------------------------
	// 工厂方法
	//---------------------------------------------------------------------
	static MsgCmd*		CreateMsgCmd(PVOID pMsg, Role* pSender);
	static VOID DeleteMsgCmd(MsgCmd* &pMsgCmd);

	//---------------------------------------------------------------------
	// 接口
	//---------------------------------------------------------------------
	virtual DWORD		Parse()=0;
	
	//---------------------------------------------------------------------
	// 一些元信息
	//---------------------------------------------------------------------
	tagNetCmd*			GetSendMsg()		const	{	return m_pSend;			}
	tagNetCmd*			GetRecvMsg()		const	{	return m_pRecv;			}
	DWORD				GetReceiverID()		const	{	return m_dwDstRoleID;	}
	ESendChatChannel	GetChannelType()	const	{	return m_eChatTag;		}
	DWORD				GetContentSize()	const	{	return m_dwContentSize;	}
	BYTE*				GetContent()		const	{	return m_pByContent;	}
	Role*				GetSender()			const	{	return m_pSender;		}
	VOID				SetSender(Role* pSender)	{	m_pSender = pSender;	}

protected:
	MsgCmd(tagNetCmd* pRecv);

	DWORD			m_dwDstRoleID;			// 收件人id
	ESendChatChannel	m_eChatTag;			// 频道类型

	Role*			m_pSender;				// 发件人
	DWORD			m_dwContentSize;		// 消息内容大小
	BYTE*			m_pByContent;			// 消息内容

	tagNetCmd*		m_pRecv;				// 原始消息
	tagNetCmd*		m_pSend;				// 要发送的消息

	BYTE			m_byBuffer[1024 * 10];	// 发送消息缓冲区
};

//-------------------------------------------------------------------------
// 聊天消息命令
//-------------------------------------------------------------------------
class ChatCmd : public MsgCmd
{
	friend class MsgCmd;

public:
	virtual DWORD		Parse();

private:
	ChatCmd(tagNetCmd* pMsg)
		:MsgCmd(pMsg){}
};

//-------------------------------------------------------------------------
// 展示装备命令
//-------------------------------------------------------------------------
class EquipCmd : public MsgCmd
{	
	friend class MsgCmd;
public:
	virtual DWORD		Parse();

private:
	EquipCmd(tagNetCmd* pMsg)
		:MsgCmd(pMsg){}

	DWORD	m_byContainerType;
};

//-------------------------------------------------------------------------
// 展示物品命令
//-------------------------------------------------------------------------
class ItemCmd : public MsgCmd
{
	friend class MsgCmd;
public:
	virtual DWORD		Parse();
private:
	ItemCmd(tagNetCmd* pMsg)
		:MsgCmd(pMsg){}
};


//-------------------------------------------------------------------------
// 消息管理器
//-------------------------------------------------------------------------
class MsgMgr
{
public:
	DWORD		LoadLeftMsg(DWORD dwRoleID);

	DWORD		HandleChatMsg(PVOID pMsg, Role* pSender);

 	DWORD		HandleLoadLeftMsg(PVOID pMsg);

private:
	//---------------------------------------------------------------------
	// 留言
	//---------------------------------------------------------------------
	DWORD		SaveLeftMsg(tagNetCmd* pMsg, DWORD dwMsgSize, DWORD dwRoleID);
	
	
	ChannelMgr	m_ChannelMgr;
};


extern MsgMgr g_msgMgr;