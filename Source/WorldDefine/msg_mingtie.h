//-----------------------------------------------------------------------------
//!\file msg_mingtie.h
//!
//!\date 2008-12-29
//!
//!\brief 客户端与服务器之间关于名帖的消息
//-----------------------------------------------------------------------------

#pragma once
#include "msg_common_errorcode.h"
#include "mingtie.h"

#pragma pack(push, 1)

//错误返回码
enum
{
};

//-----------------------------------------------------------------------------
//	请求名帖需要的相关信息
//-----------------------------------------------------------------------------
CMD_START(NC_RoleMingTie)
	DWORD	dwRoleID;		// 角色ID
CMD_END

CMD_START(NS_RoleMingTie)
	BYTE	    bySex;	                                                    //性别
	EHoroscope  eHoroscope;                                                 //星座
	EZodiac     eZodiac;                                                    //生肖
	ERegion     eRegion;                                                    //区域
	EProvince   eProvince;                                                  //省份
	ECity       eCity;                                                      //城市
	TCHAR       szIndividuality_Signature[MAX_INDIVIDUALITY_SIGNATURE];     //个性签名
	DWORD       dwErrorCode;                                                //返回错误码
	CHAR        buff[1];                                                    //图片内容, 变长
CMD_END

#pragma pack(pop)
