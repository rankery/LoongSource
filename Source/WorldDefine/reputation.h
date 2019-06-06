#pragma once
#pragma pack(push,1)
//-----------------------------------------------------------------
// 虚拟氏族种类 建议定义数量使用此
//-----------------------------------------------------------------
enum ECLanType
{
	ECLT_NULL			= -1,
	ECLT_BEGIN			= 0,

	ECLT_XuanYuan		= 0,	//轩辕
	ECLT_ShenNong		= 1,	//神农
	ECLT_FuXi			= 2,	//伏羲
	ECLT_SanMiao		= 3,	//三苗
	ECLT_JiuLi			= 4,	//九黎
	ECLT_YueZhi			= 5,	//月氏
	ECLT_NvWa			= 6,	//女娲
	ECLT_GongGong		= 7,	//共工

	ECLT_END				= 8,	
	ECLT_NUM				= ECLT_END - ECLT_BEGIN,
};

//-----------------------------------------------------------------
// 声望类型
//-----------------------------------------------------------------
enum EReputationType
{
	ERT_NULL			= -1,
	ERT_BEGIN			= 20,

	ERT_XuanYuan		= 20,	//轩辕
	ERT_ShenNong		= 21,	//神农
	ERT_FuXi			= 22,	//伏羲
	ERT_SanMiao			= 23,	//三苗
	ERT_JiuLi			= 24,	//九黎
	ERT_YueZhi			= 25,	//月氏
	ERT_NvWa			= 26,	//女娲
	ERT_GongGong		= 27,	//共工

	ERT_END				= 28,
	ERT_NUM				= ERT_END - ERT_BEGIN,
};
//-----------------------------------------------------------------
// 贡献类型
//-----------------------------------------------------------------
enum ERContributionType
{
	ERCT_NULL			= -1,
	ERCT_BEGIN			= 120,

	ERCT_XuanYuan		= 120,	//轩辕
	ERCT_ShenNong		= 121,	//神农
	ERCT_FuXi			= 122,	//伏羲
	ERCT_SanMiao		= 123,	//三苗
	ERCT_JiuLi			= 124,	//九黎
	ERCT_YueZhi			= 125,	//月氏
	ERCT_NvWa			= 126,	//女娲
	ERCT_GongGong		= 127,	//共工

	ERCT_END			= 128,
	ERCT_NUM			= ERCT_END - ERCT_BEGIN,
};

//-----------------------------------------------------------------
// 声望阶段
//-----------------------------------------------------------------
enum EReputationLevel
{
	ERL_NULL		= -1,
	ERL_BEGIN		= 0,

	ERL_Hostile		= 0,	//仇视
	ERL_Hated		= 1,	//嫌恶
	ERL_Callous		= 2,	//冷漠
	ERL_Neutrality	= 3,	//中立
	ERL_Friendly	= 4,	//友善
	ERL_Intimate	= 5,	//亲密
	ERL_Reliable	= 6,	//信赖
	ERL_Respected	= 7,	//尊敬
	ERL_Worshipped	= 8,	//崇拜
	ERL_Legend		= 9,	//传奇

	ERL_END			= 10,
	ERL_NUM			= ERL_END - ERL_BEGIN,

};

//-----------------------------------------------------------------------------
// 声望数据
//-----------------------------------------------------------------------------
struct tagReputationData
{
	INT32			nrValue[ECLT_NUM];				//声望值
	INT32			ncValue[ECLT_NUM];				//贡献值
	BYTE			nActiveCount[ECLT_NUM];			//剩余氏族珍宝激活次数
	BYTE			bisFame[ECLT_NUM];              //1是名人
	tagReputationData()	{	ZeroMemory(this, sizeof(*this));	}
};

#define MTRANS_ERT2ECLT(n)	((ECLanType)((n) - ERT_BEGIN))

#define JDG_VALID( ENUMTYPE,enumtype )	\
	((enumtype) >= ENUMTYPE##_BEGIN && (enumtype) < ENUMTYPE##_END)



#pragma pack(pop)