#pragma once
#pragma pack(push,1)

enum EMotionType1
{
	EMT1_Null			= -1,
	EMT1_Self			= 0,
	EMT1_Interactive,
};


//-------------------------------------------------------------------------
// 个性动作
//-------------------------------------------------------------------------
struct tagMotionProto
{
	DWORD			dwTypeID;		// id
	EMotionType1	eMotionType;	// 动作类型
	DWORD			dwFriendVal;		// 友好度要求
};

#pragma pack(pop)