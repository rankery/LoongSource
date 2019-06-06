/*******************************

服务器客户端关于实力的消息

*******************************/

#pragma once

#include "msg_common_errorcode.h"
#include "role_info.h"

#pragma pack(push, 1)

const INT MAX_RANKINGS_LEN = 500;
const INT RANKINGS_PAGE_LEN = 20;

enum ERankingType
{
	ERT_Strength,	// 实力值
	ERT_Level,		// 角色等级
	//ERT_Money,		// 金钱
	ERT_Fame,		// 声望
	ERT_Pet,		// 宝宝
	ERT_Equip,		// 武器
	ERT_All
};

struct tagRankingInfo
{	
	INT				nCurRanking;		// 当前排名
	INT				nRankingChange;		// 排名变化,如果玩家上次排名在500名开外，本次排进了前500，那么无法得到具体的变化值
										// 例如：玩家本次排名为450，上次排名为500以外，那么该值为 500 + （500 - 450）= 550
										// 客户端的处理为： 550 - 500 = 50，显示排名变化为：上升 >50
	INT64			n64Value;			// 值
	tagRoleIDName	sIDName;			// 角色ID和姓名
};

struct tagPetRankingInfo
{
	INT				nCurRanking;		// 当前排名
	INT				nRankingChange;		// 排名变化
	TCHAR			szPetName[X_SHORT_NAME];		// 宠物的名字
	tagRoleIDName	sIDName;			// 角色ID和姓名
};

//-----------------------------------------------------------
// 获取服务器实力排名
//-----------------------------------------------------------
CMD_START(NC_GetRankingInfo)
	INT				nPage;				// 申请排名的页数，由服务器自己计算，nPage从1开始
	ERankingType	eType;				// 要申请排行的类型
CMD_END;

//-----------------------------------------------------------
// 服务器反馈回来的信息
//-----------------------------------------------------------
CMD_START(NS_GetRankingInfo)
	INT				nPage;								// 申请排名的页数
	INT				nLocalPlayerRank;					// 本地玩家排名
	ERankingType	eType;								// 排名类型
	tagRankingInfo	sData[RANKINGS_PAGE_LEN];			// 获取的排名信息
CMD_END;

CMD_START(NS_GetPetRankingInfo)
	INT				nPage;								// 申请排名的页数
	INT				nLocalPlayerRank;					// 本地玩家排名
	//ERankingType	eType;								// 排名类型
	tagPetRankingInfo	sData[RANKINGS_PAGE_LEN];			// 获取的排名信息
CMD_END;

#pragma pack(pop)
