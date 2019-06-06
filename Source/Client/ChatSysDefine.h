#pragma once
#include "..\WorldDefine\chat_define.h"
#include "..\WorldDefine\ItemDefine.h"
//----------------------------------------------------------------
//发送给服务器的发言数据结构(发言)
//----------------------------------------------------------------
struct tagSendSpeak
{
	DWORD				dwDestID;					//发送给谁ID
	tstring				strDestName;				//发送给谁Name
	float				fSendTime;					//发送的时间
	ESendChatChannel	eChannel;					//发言频道
	tstring				strMsg;						//发言内容
	BYTE				byAutoReply;		//是否自动回复 0:不是 1:是

	tagSendSpeak()
	{	
		dwDestID	= GT_INVALID;
		eChannel	= ESCC_NULL;
		fSendTime	= 0.0f;
		byAutoReply = 0;
	}
	tagSendSpeak(const tagSendSpeak& other)
	{
		Copy(other);
	}

	const tagSendSpeak& operator = (const tagSendSpeak& other)
	{
		if(this!=&other) 
			Copy(other);
		return *this;
	}
private:
	void Copy(const tagSendSpeak&  other)
	{
		dwDestID	= other.dwDestID;
		eChannel	= other.eChannel;
		fSendTime	= other.fSendTime;
		strDestName	= other.strDestName;
		strMsg		= other.strMsg;
		byAutoReply = other.byAutoReply;
	}
};

//-----------------------------------------------------------------
//从服务器接受到的聊天信息的数据结构（接受服务器发言）
//-----------------------------------------------------------------
struct tagRecvSpeak
{
	DWORD				dwSendRoleID;		//发送者 GT_INVALID代表系统发送	
	DWORD				dwRecvRoleID;		//接收者
	ESendChatChannel	eChannel;			//发言频道
	tstring				strSendRoleName;	//发言者的名称
	tstring				strRecvRoleName;	//接受者姓名
	tstring				strText;			//发言内容
	BYTE				byAutoReply;		//是否自动回复 0:不是 1:是

	tagRecvSpeak()
	{		
		dwSendRoleID	= GT_INVALID;
		dwRecvRoleID	= GT_INVALID;
		eChannel		= ESCC_NULL;
		byAutoReply		= FALSE;
	}
	tagRecvSpeak(const tagRecvSpeak& other)
	{
		Copy(other);
	}

	const tagRecvSpeak& operator = (const tagRecvSpeak& other)
	{
		if(this!=&other)
			Copy(other);
		return *this;
	}
private:
	void Copy(const tagRecvSpeak&  other)
	{
		dwSendRoleID	= other.dwSendRoleID;
		dwRecvRoleID	= other.dwRecvRoleID;
		eChannel		= other.eChannel;
		strSendRoleName	= other.strSendRoleName;
		strRecvRoleName	= other.strRecvRoleName;
		strText			= other.strText;
		byAutoReply		= other.byAutoReply;
	}
};

//-------------------------------------------------------
//-------------------------------------------------------
struct tagChatShowEquip
{
	DWORD				dwSrcID;
	tstring				strSrcName;
	DWORD				dwDstID;
	tstring				strDstName;
	ESendChatChannel	eChannel;
	tagEquip			equip;
};
struct tagChatShowItem
{
	DWORD				dwSrcID;
	tstring				strSrcName;
	DWORD				dwDstID;
	tstring				strDstName;
	ESendChatChannel	eChannel;
	DWORD				item;
};

//-------------------------------------------------------
//常量定义
//-------------------------------------------------------
const byte NUM_SEND_CHAT = 6;				//客户端发言频道的数量

const byte NUM_CHAT_CHANNEL = 8;			//客户端接收频道的数量

const INT MAX_EMOTION = 60;					//最大表情数量

const int MIN_REPEAT_SPEAK_TIME = 30;		//当前频道重复发言的时间间隔

const int MIN_WORLD_RESPEAK_TIME = 30;		//世界频道重复发言的判定时间

const int MIN_RUNHORSE_RESPEAK_TIME = 30;	//跑马灯发言的判定时间

const int MAX_PVT_SPEAK_VALIDETIME = 120;	//私聊发言的有效时间，如果超过该时间还没有查到该玩家的ID，则从缓存中删除

const int MAX_ROW = 300;					//聊天窗口最多行数

const DWORD RICHEDITBOX_MAX_TEXT = 20480*2;	//richeditbox最大字符数

const byte NUM_CHAT_CAST_WND = 3;			//传音

const byte NUM_CHAT_ZOOM_MAX = 4;

const byte NUM_CHAT_ZOOM_MIN = 0;





//玩家聊天接收频道
enum ERecvChatChannel
{
	ERCC_NULL		= 0x00,
	ERCC_CastTalk	= 0x01,		//传音
	ERCC_World		= 0x02,		//世界
	ERCC_Pvt		= 0x04,		//私聊
	ERCC_Guild		= 0x08,		//帮派
	ERCC_Troop		= 0x10,		//队伍
	ERCC_Current	= 0x20,		//当前
	ERCC_Combat		= 0x40,		//战斗
	ERCC_Affiche	= 0x80,		//公告
	ERCC_GM			= 0x100,	//GM
	ERCC_System		= 0x200,	//系统
	ERCC_End		= 0x7FFF,	//全部
	ERCC_Num		= 10,
};

const DWORD g_dwRecvChannel[] = 
{
	ERCC_World | ERCC_Pvt | ERCC_Guild | ERCC_Troop | ERCC_Current | ERCC_Affiche | ERCC_GM | ERCC_System,
	ERCC_Troop | ERCC_System | ERCC_Affiche,
	ERCC_Guild | ERCC_System | ERCC_Affiche,
	ERCC_Pvt | ERCC_System | ERCC_Affiche,
	ERCC_World | ERCC_System | ERCC_Affiche,
	ERCC_Affiche | ERCC_System | ERCC_Affiche,
	ERCC_Combat  | ERCC_System | ERCC_Affiche,
};

//用于缓存传音
struct tagScroText					
{
	tstring szInfo;
	tstring szName;
	tstring url;
	DWORD	dwRoleID;
	float	fTime;
};

struct ShowEvent
{
	DWORD			dwParam1;	
	DWORD			dwParam2;
	DWORD			dwParam3;
	DWORD			dwParam4;
};
