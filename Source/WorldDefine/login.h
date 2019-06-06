#pragma once

#include "msg_common_errorcode.h"
#define MAX_MD5_ARRAY	36
#define MAX_MIBAO		6

#pragma pack(push, 1)

//--------------------------------------------------------------------------------------------
// Login 错误代码
//--------------------------------------------------------------------------------------------
enum
{
	E_ProofResult_Account_No_Match				= 1,	//此帐号非法(帐户不存在或者密码不对,密保错误)
	E_ProofResult_Account_In_Use				= 2,	//此帐号当前已经在使用
	E_ProofResult_Account_Freezed				= 3,	//此帐号被停用
	E_ProofResult_Wrong_Build_Number			= 4,	//版本号不对
	E_ProofResult_Disabled						= 5,	//此帐号已经被封停
	E_ProofResult_Queue							= 6,	//开始排队	
	E_ProofResult_Mibao_Error					= 7,	//密保错误
	E_ProofResult_Wrong_Type					= 8,	//非法验证策略
	E_ProofResult_Proofing						= 9,	//正在验证或已经验证完成

	E_SelectWorld_Server_Full					= 11,	//该服务器当前拥挤
	E_SelectWorld_Server_Suspended				= 12,	//该服务器当前阻塞
	E_SelectWorld_Server_Maintenance			= 13,	//该服务器当前维护
	E_SelectWorld_Failed						= 14,	//出现异常，选择失败
	E_SelectWorld_Short_Time					= 15,	//间隔时间太短,请稍候重试
	E_SelectWorld_No_Select						= 16,	//你已经选择成功,不可再选
	E_SelectWorld_GameWorldName_Wrong			= 17,	//非法的游戏世界名


	E_ProofResult_Forbid_MemberCenter			= 18,	//您的会员中心账号已被封停，请联系客服
	E_ProofResult_Forbid_GMTool					= 19,	//您的游戏账号已被封停，请联系客服	
	E_ProofResult_Forbid_CellPhone				= 20,	//您的账号已被锁定，请联系客服
	E_ProofResult_Forbid_MiBao					= 21,	//您的账号已经挂失密保卡，请完成解绑后登录
};

//---------------------------------------------------------------------------------------------
// 排队时间10分钟
//---------------------------------------------------------------------------------------------
#define QUEUE_TIME	10*60


//-----------------------------------------------------------------------------
// 验证消息
//-----------------------------------------------------------------------------
CMD_START(NLC_SafeProof)
	DWORD	dwProofKey;						// 安全验证key
	CHAR	szUserName[X_SHORT_NAME];		// 用户名
	DWORD	dwType;							// 验证方式
	DWORD	dwCurVersionID;					// 当前版本ID
	DWORD	dwWorldNameCrc;					// 游戏世界名的crc
	CHAR	szPsd[MAX_MD5_ARRAY];			// 密码
	CHAR	szGUID[X_LONG_NAME];			// GUID值
CMD_END;

CMD_START(NLS_ProofResult)
	DWORD	dwErrorCode;					// 错误码
	BOOL	bGuard;							// 防沉迷
	DWORD	dwIndex;						// 等候序列号
	DWORD	dwVerifyCode;					// 登陆服务器发送给客户端的验证码，连接ZoneServer时要用到
	DWORD	dwAccountID;					// 帐号ID
	DWORD   dwIP;							// ZoneServer IP 地址
	DWORD   dwPort;							// ZoneServer 端口号
CMD_END;

//------------------------------------------------------------------------------
// 密保
//------------------------------------------------------------------------------
CMD_START(NLS_MiBao)
	CHAR	szMiBao[MAX_MIBAO];				// 密保矩阵提示码
CMD_END

CMD_START(NLC_MiBao)
	DWORD	dwMBCrc;						// 密保crc
CMD_END

//-------------------------------------------------------------------------------
// 排队
//-------------------------------------------------------------------------------
CMD_START(NLS_Queue)
	DWORD	dwPosition;						// 排队的位置
	DWORD	dwTime;							// 排队的剩余时间
CMD_END;



//-------------------------------------------------------------------------------
//	登录协议验证码			by: Jampe
//-------------------------------------------------------------------------------

#define PK_StrToID(n, s)		do{													\
									n = (DWORD)strlen(s);							\
									DWORD step = (n >> 5) + 1;						\
									for(DWORD i = n; i >= step; i -= step)			\
										n = n ^ ((n << 5) + (n >> 2) + s[i - 1]);	\
								}while(0)

#define ProofKey(k, p)			do{												\
									DWORD n[3];									\
									PK_StrToID(n[0], p->szUserName);			\
									PK_StrToID(n[1], p->szPsd);					\
									PK_StrToID(n[2], p->szGUID);				\
									k = p->dwWorldNameCrc + p->dwCurVersionID;	\
									k = (k << 2) + p->dwType;					\
									k = k + n[0] + n[1] + n[2];					\
								}while(0)


#pragma pack(pop)

