#pragma once

#include "..\WorldDefine\msg_mall.h"
#include "CurrencyMgr.h"

//! 元宝交易账户
struct tagRoleYBAccount
{
    DWORD       dwRoleID;       //!< 玩家ID
    INT         nYuanBao;       //!< 元宝数量
    INT64       nMoney;         //!< 金钱数量
    BOOL        bHasSellOrder;  //!< 是否有寄售订单
    BOOL        bHasBuyOrder;   //!< 是否有收购订单        

    tagRoleYBAccount() {memset(this, 0, sizeof(*this));}
};

//! 元宝同步返回类型
enum EYuanBaoSynRet
{
    EYBR_SaveYB2Account         =   0,
    EYBR_DepositAccountYB       =   1,
    EYBR_SaveSilver2Account     =   2,
    EYBR_DepositAccountSilver   =   3,
    EYBR_SubmitSellOrder        =   4,
    EYBR_SubmitBuyOrder         =   5,
    EYBR_DeleteOrder            =   6,

    EYBR_GetSellPriceList       =   7,
    EYBR_GetBuyPriceList        =   8,
    EYBR_GetYBAccount           =   9,
    EYBR_SynSellPriceList       =   10,
    EYBR_SynBuyPriceList        =   11,
    EYBR_GetYuanbaoOrder        =   12,
    EYBR_SynAccoutYB            =   13,
    EYBR_SynAccoutSilver        =   14,
    EYBR_SynAccout              =   15
};

//! 同步元宝信息的返回值
struct tagYBReturnEvnt : public tagGameEvent
{
    EYuanBaoSynRet  eType;      //!< 返回类型
    bool            bRet;       //!< 是否成功
    DWORD           dwErrCode;  //!< 错误码
    tagYBReturnEvnt(LPCTSTR szEventName, GameFrame* pSenderFrame)
        : tagGameEvent(szEventName, pSenderFrame) {}
};

//! \class YuanbaoTradeMgr
//! \brief 元宝交易管理器
//! \author hyu
//! \date 2009-3-16
class YuanbaoTradeMgr
{
public:
    YuanbaoTradeMgr(void);
    ~YuanbaoTradeMgr(void);

    VOID Init();
    VOID Destroy();

    //! Singleton
    static YuanbaoTradeMgr* Inst();

    typedef map<INT, tagYBPriceList*> _MapPrice;
    typedef map<DWORD, tagYuanBaoOrder*> _MapOrder;

    
    //------------------------------------------------------------------------
    // 对外接口
    //------------------------------------------------------------------------

    //! 获得元宝账户金钱
    INT64 GetMoneyInAccount() { return m_YBAccount.nMoney; }

    //! 获得元宝账户元宝
    INT GetYuanBaoInAccount() { return m_YBAccount.nYuanBao; }

    //! 获得寄售列表
    const _MapPrice* GetPrice2SellList() { return &m_mapSellListByPrice; }

    //! 获得收购列表
    const _MapPrice* GetPrice2BuyList() { return &m_mapBuyListByPrice; }

    //! 获得订单列表
    const _MapOrder* GetID2OrderList() { return &m_mapOrderByID; }

private:
    // 不允许复制
    YuanbaoTradeMgr(const YuanbaoTradeMgr&);
    YuanbaoTradeMgr& operator = (const YuanbaoTradeMgr&);


    //------------------------------------------------------------------------
    // 网络事件
    //------------------------------------------------------------------------
public:
    //! 玩家向交易账户存元宝
    //! \param nNum 元宝数量
    VOID BeginNC_SaveYB2Account(INT nNum);

    //! 从交易账户提取元宝
    //! \param nNum 元宝数量
    VOID BeginNC_DepositAccountYB(INT nNum);

    //! 玩家向交易账户存金钱
    //! \param nNum 金钱数
    VOID BeginNC_SaveSilver2Account(INT64 nNum);

    //! 从交易账户提取金钱
    //! \param nNum 金钱数
    VOID BeginNC_DepositAccountSilver(INT64 nNum);

    //! 获取所有元宝交易价格的列表和交易账户信息
    VOID BeginNC_SynYBTradeInfo();

    //! 提交元宝出售订单
    VOID BeginNC_SubmitSellOrder(INT nNum, INT64 nPrice);

    //! 提交元宝收购订单
    VOID BeginNC_SubmitBuyOrder(INT nNum, INT64 nPrice);

    //! 获得玩家订单
    VOID BeginNC_GetYuanbaoOrder();

    //! 删除订单
    VOID BeginNC_DeleteOrder(DWORD dwOrderID, EYBOTYPE eYBOType);
private:
    //! 同步交易账户的元宝数
    DWORD OnNS_SynAccoutYB(tagNS_SynAccoutYB* msg, DWORD);

    //! 同步交易账户的金钱数
    DWORD OnNS_SynAccoutSilver(tagNS_SynAccoutSilver* msg, DWORD);

    //! 同步交易账户
    DWORD OnNS_SynAccout(tagNS_SynAccout* msg, DWORD);

    //! 玩家向交易账户存元宝(返回)
    DWORD OnNS_SaveYB2Account(tagNS_SaveYB2Account* msg, DWORD);

    //! 从交易账户提取元宝(返回)
    DWORD OnNS_DepositAccountYB(tagNS_DepositAccountYB* msg, DWORD);

    //! 玩家向交易账户存金钱(返回)
    DWORD OnNS_SaveSilver2Account(tagNS_SaveSilver2Account* msg, DWORD);
    
    //! 从交易账户提取金钱(返回)
    DWORD OnNS_DepositAccountSilver(tagNS_DepositAccountSilver* msg, DWORD);

    //! 获取所有元宝交易价格的列表和交易账户信息(返回)
    DWORD OnNS_GetSellPriceList(tagNS_GetSellPriceList* msg, DWORD);
    DWORD OnNS_GetBuyPriceList(tagNS_GetBuyPriceList* msg, DWORD);
    DWORD OnNS_GetYBAccount(tagNS_GetYBAccount* msg, DWORD);

    //! 同步出售价格列表
    DWORD OnNS_SynSellPriceList(tagNS_SynSellPriceList* msg,DWORD);

    //! 同步购买价格列表
    DWORD OnNS_SynBuyPriceList(tagNS_SynBuyPriceList* msg, DWORD);

    //! 提交元宝出售订单(返回)
    DWORD OnNS_SubmitSellOrder(tagNS_SubmitSellOrder* msg, DWORD);
    
    //! 提交元宝收购订单(返回)
    DWORD OnNS_SubmitBuyOrder(tagNS_SubmitBuyOrder* msg, DWORD);
    
    //! 获得玩家订单(返回)
    DWORD OnNS_GetYuanbaoOrder(tagNS_GetYuanbaoOrder* msg, DWORD);

    //! 删除订单(返回)
    DWORD OnNS_DeleteOrder(tagNS_DeleteOrder* msg, DWORD);

    //------------------------------------------------------------------------
    // 缓冲容器操作
    //------------------------------------------------------------------------

    //! 更新出售价格列表缓冲,缓冲中无此则插入
    VOID UpdateSellPriceList(tagYBPriceList* pList);
    VOID ClearSellPriceList();

    //! 收购价格列表缓冲,缓冲中无此则插入
    VOID UpdateBuyPriceList(tagYBPriceList* pList);
    VOID ClearBuyPriceList();

    //! 插入玩家订单列表缓冲
    VOID InsertOrderList(tagYuanBaoOrder* pOrder);
    VOID ClearOrderList();

    //! 清空元宝交易账户
    void ClearYBAccount();

    //------------------------------------------------------------------------
    // 容器
    //------------------------------------------------------------------------

    _MapPrice                   m_mapSellListByPrice;       //!< 出售价格列表缓冲

    _MapPrice                   m_mapBuyListByPrice;        //!< 收购价格列表缓冲

    _MapOrder                   m_mapOrderByID;             //!< 玩家订单列表缓冲

    tagRoleYBAccount            m_YBAccount;                //!< 玩家元宝交易账户

    //------------------------------------------------------------------------
    // 工具
    //------------------------------------------------------------------------
    TSFPTrunk<YuanbaoTradeMgr>  m_Trunk;
    TObjRef<Util>				m_pUtil;
    TObjRef<NetCmdMgr>			m_pCmdMgr;
    TObjRef<GameFrameMgr>		m_pFrameMgr;
    TObjRef<NetSession>			m_pSession;
    CurrencyMgr*                m_pCurrency;
};
