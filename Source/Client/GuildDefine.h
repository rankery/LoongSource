#pragma once

struct tagGuildAffairInfo
{
	DWORD		dwID;			// 事务ID
	INT32		nGuildLv;		// 帮派等级
	INT32		nCity;			// 城市ID
	INT32		nFund;			// 需要资金
	INT32		nMaterial;		// 需要资财
	tstring		strName;		// 事务名称
	tstring		strDesc;		// 事务描述
};

struct tagGuildSkillInfo
{
	DWORD dwSkillID;			// 技能ID
	tstring strName;			// 技能名
	INT32 n16Fulfill;			// 这个技能的进度
	INT32 nResearchFund;		// 研究所需资金
	INT32 nResearchMaterial;	// 研究所需资材
	INT32 nLearnSilver;			// 学习所需银子
	INT32 nLearnContribution;	// 学习所需帮派贡献
	INT32 nLearnFund;			// 学习所需帮派资金
	INT32 nLearnMaterial;		// 学习所需帮派资材
};

struct tagCommerceInfo
{
	INT32	nLevelRange;	// 等级范围(最高等级*1000+最低等级)
	INT32	nDeposit;		// 保证金
	INT32	nBeginningTael;	// 初始商银
	INT32	nPurposeTael;	// 指标商银
	INT32	nMaxTael;		// 商银上限
};

struct tagGoodInfo
{
	DWORD dwID;				// 商货ID
	tstring strName;		// 商货名称
	tstring strIconPath;	// 图标名
	tstring strDisplayType;	// 商货显示类型
	tstring strDesc;		// 商货描述
};

struct tagCOCInfo
{
	DWORD dwID;				// 商会ID
	tstring strName;		// 商会名称
	tstring strDesc;		// 商会描述
};