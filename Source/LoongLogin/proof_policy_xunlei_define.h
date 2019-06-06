//-----------------------------------------------------------------------------
//!\file proof_policy_xunlei_define.h
//!\author songg
//!
//!\date 2009-06-22
//! last 2008-06-22
//!
//!\brief xunlei登陆公共接口定义
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#pragma pack(push, 1)

//char的数据 左对齐，右补空格。
//byte的数据 左对齐，右补0。

#define XL_GAMEID		"00025"

#define XL_CMD_LOGIN	"LOGIN"

#define XL_DECRYPT_REQUIRE_DATA (112) //待加密数据长度

#define XL_ENCRYPT_REQUIRE_DATA (116) //请求 加密数据长度
#define XL_MAX_LEN	(500)			// 最大长度
//-----------------------------------------------------------------------------
// 登陆整体帧结构
//-----------------------------------------------------------------------------
typedef struct tagXunLeiLoginFrame_t{
	
	//DWORD	dwFrameSize;			//size 不包括该字段本身大小
	CHAR	szGameID[5];			//标示游戏的种类
	CHAR	szProtocolVersion[5];	//协议版本
	CHAR	szEncryptData[XL_MAX_LEN];		//加密数据

	tagXunLeiLoginFrame_t()
	{
		//dwFrameSize = 0;
		memset(szGameID,' ',sizeof(szGameID));
		memset(szProtocolVersion,' ',sizeof(szProtocolVersion));
		memset(szEncryptData,' ',sizeof(szEncryptData));
	}
	
}stXunLeiLoginFrame;

//-----------------------------------------------------------------------------
// 112个byte的未加密请求数据   加密后放入stXunLeiLoginFrame.szEncryptData中
//-----------------------------------------------------------------------------
typedef struct tagXunLeiRequireData_t{

	CHAR	szCommand[10];			//命令
	USHORT	usSerialNum;				//序列号
	BYTE	byAccountData[100];		//账号密码加密包	由迅雷公司提供的登录器产生


	tagXunLeiRequireData_t()
	{
		memset(szCommand,' ',sizeof(szCommand));		
		usSerialNum = 0;
		memset(byAccountData,' ',sizeof(byAccountData));
	}

}stXunLeiRequireData;


//-----------------------------------------------------------------------------
// 未加密 变长的返回数据
//-----------------------------------------------------------------------------
typedef struct tagXunLeiReturnData_t{

	CHAR	szReturnCode[4];		//返回码
	USHORT	usSerialNum;			//序列号 对应的请求桢中的序列号相等

	CHAR	szAccountName[20];			//账号/账号ID当返回代码等于0000时才有该字段。

	CHAR	szGuard[1];				//防沉迷

	CHAR	szReverse[10];			//必须要有不然解密会崩溃

	tagXunLeiReturnData_t()
	{
		memset(szReturnCode,' ',sizeof(szReturnCode));		
		usSerialNum = 0;
		memset(szAccountName,' ',sizeof(szAccountName));
		memset(szGuard,' ',sizeof(szGuard));
	}

}stXunLeiReturnData;



#pragma pack(pop)