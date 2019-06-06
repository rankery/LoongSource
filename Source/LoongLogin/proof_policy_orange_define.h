//-----------------------------------------------------------------------------
//!\file proof_policy_xunlei_define.h
//!\author songg
//!
//!\date 2009-06-22
//! last 2008-06-22
//!
//!\brief 台湾游戏橘子登录
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#pragma pack(push, 1)

//所有參數都是以單號分隔 ('). 每一個訊息最後都會加上 “\r\n” 作結尾.

//橘子的账号密码统一走guid 采用空格分隔 因为账号名要求可以采用邮箱消息中原有的账号名不够长

#define X_OG_SC_LEN 7 //ServiceCode
#define X_OG_REGION_LEN 3 //Region

#define X_OG_SEND_DIVIDE_SIGN '\'' //发送橘子消息的分隔符

#define X_OG_RECEIVE_DIVIDE_SIGN ';' //接收橘子消息的分隔符

#define X_CLIENT_RECEIVE_DIVIDE_SIGN ' ' //接收客户端消息的分隔符


#define X_FROBID_MEMBER_CENTER "ServiceAccount_Locked" //账号被锁

#pragma pack(pop)