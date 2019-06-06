#pragma once
#include "ProgressEx.h"
#include "..\WorldDefine\reputation.h"
#include "..\WorldDefine\role_att.h"

struct tagNS_GetRoleInitState_Reputation;
struct tagNS_RoleReputeChange;

//! 声望阶段颜色
enum EPrgrssColor
{
	EPC_RED			=	0,
	EPC_YELLOW		=	1,
	EPC_GREEN		=	2,
	EPC_BLUE		=	3,
	EPC_ORANGE		=	4,

	EPC_END			=	5
};

//! \struct tagReputeLevel
//! \brief 声望等级数据
struct tagReputeLevel
{
	EReputationLevel		eLevel;			//!< 声望阶段
	EPrgrssColor			eColor;			//!< 声望阶段颜色
	LPCTSTR					szLevel;		//!< 声望阶段文本
	INT						nFrom;			//!< 声望值 [nFrom, nTo)
	INT						nTo;
	INT						nDiffValue;		//!< 声望阶段值 = nTo - nFrom
	INT						nMaxContribute;	//!< 贡献值上限
};

//! 声望值有变化
struct tagReputeChange : public tagGameEvent
{
	ECLanType				eClanType;		//!< 氏族类型
	INT						nValue;			//!< 声望值
	INT						nChange;		//!< 变化值

	tagReputeChange(LPCTSTR szEventName, GameFrame* pSenderFrame)
		: tagGameEvent(szEventName, pSenderFrame){}
};

//! 成为名人
struct tagBecomeEvent : public tagGameEvent
{
    ECLanType               eClanType;      //!< 氏族类型

    tagBecomeEvent(LPCTSTR szEventName, GameFrame* pSenderFrame)
        : tagGameEvent(szEventName, pSenderFrame) {}
};

//! 氏族珍宝激活次数改变
struct tagTreasureActCountChange : public tagGameEvent
{
    ECLanType               eClanType;      //!< 氏族类型
    INT                     nCount;         //!< 剩余激活次数
    tagTreasureActCountChange(LPCTSTR szEventName, GameFrame* pSenderFrame)
        : tagGameEvent(szEventName, pSenderFrame) {}
};

//! 氏族声望等级变化 by add xtian 2009-6-15
struct tagReputeLvlChange : public tagGameEvent
{
	 ECLanType					eClanType;      //!< 氏族类型
	 EReputationLevel			eLevel;			//!< 声望阶段
	 tagReputeLvlChange(LPCTSTR szEventName, GameFrame* pSenderFrame)
		 : tagGameEvent(szEventName, pSenderFrame) {}
};

//! \class ReputeMgr
//! \brief 声望值管理器
//! \author hyu
class ReputeMgr
{
public:
	ReputeMgr(void);
	~ReputeMgr(void);

	VOID Init();
	VOID Destroy();

	static ReputeMgr* Inst();

	//! 获取当前声望值
	INT GetReputeValue(ECLanType eCtype) {return JDG_VALID(ECLT, eCtype) ? m_nReputeValue[eCtype] : 0;}
    //! 与GetReputeValue本质一样，传入参数不同，因策划中任务相关的声望消耗是从20开始，故应使用此函数
    INT GetReputeValueEx(EReputationType eRtype) {return JDG_VALID(ERT, eRtype) ? m_nReputeValue[eRtype - ERT_BEGIN] : 0;}

	//! 获取当前声望等级
	const tagReputeLevel* GetReputeLevel(ECLanType eCtype) {return JDG_VALID(ECLT, eCtype) ? &m_ReputeLevel[eCtype] : NULL;}

    //! 获得当前是否是名人
    BOOL IsFame(ECLanType eCtype) {return JDG_VALID(ECLT, eCtype) ? m_bisFame[eCtype] : FALSE;}

    //! 获得当前氏族珍宝激活次数
    INT GetActCount(ECLanType eCtype) {return JDG_VALID(ECLT, eCtype) ? m_nActCount[eCtype] : 0;}

	//! 根据声望值计算声望等级
	BOOL UlityCalReputeLevel(tagReputeLevel& ReputeSrc, INT nReputeValue);

	//! 设置声望条颜色
	VOID UlitySetPrgrssColor( ProgressEx* pProgress, EPrgrssColor eColor );

private:

	//! 设置声望值
	VOID SetReputeValue(ECLanType eCtype, INT nValue, INT nChange);

	//------------------------------------------------------------------------
	//! 网络消息
	//------------------------------------------------------------------------
	
	//! 初始化
	DWORD OnInitReputation(tagNS_GetRoleInitState_Reputation* pNetCmd, DWORD);

	//! 初始化计算各氏族的声望等级 by add xtian 2009-6-15
	VOID OnInitRepuLvl(ECLanType eType, INT nReputeValue);

	//! 当前角色氏族声望等级发生变化时发送游戏事件 by add xtian 2009-6-15
	void UlityLocalPlayerRepuLvl( ECLanType eType, INT nReputeValue, INT nChange );

	//! 声望值改变 
	DWORD OnReputeChange(tagNS_RoleReputeChange* pNetCmd, DWORD);

    //! 名人状态改变
    DWORD OnBecomeFame(tagNS_BecomeFame* pNetCmd, DWORD);

    //! 氏族珍宝激活次数改变
    DWORD OnActCountChange(tagNS_TreasureActCountChange* pNetCmd, DWORD);

    //! 氏族贡献达上限时的提示
    DWORD OnNS_ClanConUpperLimit(tagNS_ClanConUpperLimit* pNetCmd, DWORD);

	TSFPTrunk<ReputeMgr>		m_Trunk;
	TObjRef<Util>				m_pUtil;
	TObjRef<NetCmdMgr>			m_pCmdMgr;
	TObjRef<GameFrameMgr>		m_pFrameMgr;
	TObjRef<NetSession>			m_pSession;

	INT							m_nReputeValue[ECLT_NUM];		//!< 存放当前角色声望值
	tagReputeLevel				m_ReputeLevel[ECLT_NUM];		//!< 存放当前角色声望阶段
	EReputationLevel			m_eRepuLvl[ECLT_NUM];			//!< 存放当前角色各氏族的声望等级 by add xtian 2009-6-15
    BOOL                        m_bisFame[ECLT_NUM];            //!< 是否是名人
    INT                         m_nActCount[ECLT_NUM];          //!< 珍宝激活次数

	//! 进度条前景颜色
	tstring						m_strPrgssForePic_Red;
	tstring						m_strPrgssForePic_Yellow;
	tstring						m_strPrgssForePic_Green;
	tstring						m_strPrgssForePic_Blue;
	tstring						m_strPrgssForePic_Orange;
};
