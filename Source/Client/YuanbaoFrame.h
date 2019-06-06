#pragma once
#include "gameframe.h"
#include "MallFrame.h"
#include "CurrencyMgr.h"
#include "YuanbaoTradeMgr.h"

const INT YB_SELL_LIST_COL      =   3;  //!< 元宝寄售列表列数
const INT YB_BUY_LIST_COL       =   3;  //!< 元宝收购列表列数
const INT YB_ORDER_LIST_COL     =   11; //!< 元宝订单列表列数

struct tagRoleSilverChange;
struct tagRoleYuanbaoChange;

//! \class  YuanbaoFrame
//! \brief  元宝交易
//! \author hyu
//! \date   2009-3-11
//! \base   GameFrame
//! \sa     MallFrame, MallFrame
class YuanbaoFrame : public GameFrame
{
public:
    YuanbaoFrame(void);
    ~YuanbaoFrame(void);

    virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
    virtual BOOL Destroy();
    virtual BOOL ReloadUI();

    virtual BOOL EscCancel();

private:
    //! 初始化ListBox控件
    VOID InitListBox();
    //! 重置ListBox控件
    VOID ResetSellList();
    VOID ResetBuyList();
    VOID ResetOrderList();

    //! 更新元宝交易数据到界面
    VOID UpdateYuanbaoTrade();

    //! 刷新出售list
    VOID UpdateSellList();
    //! 刷新收购list
    VOID UpdateBuyList();
    //! 刷新我要出售/收购
    VOID ResetInput();
    //! 刷新交易账户
    VOID UpdateYBAccount();
    //! 刷新行囊
    VOID UpdateBag();
    //! 刷新订单列表
    VOID BeginUpdateOrder();
    VOID UpdateOrder();

    //! 删除订单
    VOID CanelOrder();

    //! 得到文本
    LPCTSTR GetTradeType(EYBOTYPE e) { return EYBOT_BUY == e ? g_StrTable[_T("YuanBaoTrade13")] : g_StrTable[_T("YuanBaoTrade14")]; }
    LPCTSTR GetOrderStus(EYBOMODE e)
    {
        switch (e)
        {
        case EYBOM_Submit:
            return g_StrTable[_T("YuanBaoTrade15")];
        	break;
        case EYBOM_Cancel:
            return g_StrTable[_T("YuanBaoTrade16")];
            break;
        case EYBOM_Close:
            return g_StrTable[_T("YuanBaoTrade17")];
            break;
        default:
            return _T("");
            break;
        }
    }

    // 游戏事件

    //! 处理元宝交易服务端返回消息
    DWORD OnYuanBaoTradeRet(tagYBReturnEvnt* pEvent);

    //! 背包中银两变化
    DWORD OnBag_SilverChange(tagRoleSilverChange* pEvent);
    DWORD OnBag_YuanbaoChange(tagRoleYuanbaoChange* pEvent);

    //! 处理消息框返回事件
    DWORD OnSaveMoney(tagMsgMoneyBoxEvent* pEvent);
    DWORD OnDepositMoney(tagMsgMoneyBoxEvent* pEvent);
    DWORD OnSaveYuanBao(tagMsgInputBoxEvent* pEvent);
    DWORD OnDepositYuanBao(tagMsgInputBoxEvent* pEvent);
    DWORD OnSellYuanBao(tagMsgBoxEvent* pEvent);
    DWORD OnBuyYuanBao(tagMsgBoxEvent* pEvent);

    // 界面事件

    //! 处理元宝界面GUI事件
    DWORD YuanbaoEventHandler(tagGUIEvent* pEvent);
    //! 处理订单查询界面GUI事件
    DWORD OrderQEventHandler(tagGUIEvent* pEvent);

    //! 计算总价
    INT64 CalSum(INT64 nGold, INT64 nSilver, INT nNum);
    //! 计算手续费
    INT64 CalCharge(INT64 nGold, INT64 nSilver, INT nNum);

    //! 打开订单查询
    VOID ShowOrderQuery(BOOL bVal);

    TSFPTrunk<YuanbaoFrame> m_Trunk;
    TObjRef<GUISystem>      m_pGUI;
    TObjRef<Util>           m_pUtil;
    TObjRef<GameFrameMgr>   m_pFrameMgr;
    TObjRef<NetSession>     m_pSession;
    CurrencyMgr*            m_pCurrencyMgr;

    INT64                   m_nInputGold;           //!< 输入框暂存数据
    INT64                   m_nInputSilver;
    INT                     m_nInputNum;

    map<INT, DWORD>         m_mapRow2OrderID;       //!< 订单id和列表行的对应

    // 元宝交易
    GUIWnd*                 m_pWnd;                 //!< 元宝交易窗体
    GUIWnd*                 m_pWndFather;           //!< 父窗体-场城主窗体mall.xml

    GUIButton*              m_pBtSalePrice;         //!< 元宝售出单价
    GUIButton*              m_pBtSaleNum;           //!< 元宝售出数量
    GUIButton*              m_pBtBuyPrice;          //!< 元宝购买单价
    GUIButton*              m_pBtBuyNum;            //!< 元宝购买数量
    GUIListBox*             m_pLBSale;              //!< 元宝出售列表
    GUIListBox*             m_pLBBuy;               //!< 元宝购买列表

    GUIPushButton*          m_pBtWannaSell;         //!< 我要出售
    GUIPushButton*          m_pBtWannaBuy;          //!< 我要收购
    GUIEditBox*             m_pEdtWannaPriceGold;   //!< 单价(金)
    GUIEditBox*             m_pEdtWannaPriceSilver; //!< 单价(银)
    GUIEditBox*             m_pEdtWannaNum;         //!< 数量
    GUIButton*              m_pBtWannaConfirm;      //!< 开始寄售/收购

    GUIStatic*              m_pStcAcctGold;         //!< 交易账户余额(金)
    GUIStatic*              m_pStcAcctSilver;       //!< 交易账户余额(银)
    GUIButton*              m_pBtMoneySave;         //!< 钱存入
    GUIButton*              m_pBtMoneyLoad;         //!< 钱取出
    GUIStatic*              m_pStcYuanbao;          //!< 交易账户内元宝
    GUIButton*              m_pBtYuanbaoSave;       //!< 元宝存入
    GUIButton*              m_pBtYuanbaoLoad;       //!< 元存取出

    GUIStatic*              m_pStcBagGold;          //!< 行囊现有(金)
    GUIStatic*              m_pStcBagSilver;        //!< 行囊现有(银)
    GUIStatic*              m_pStcBagYuanbao;       //!< 行囊元宝

    GUIButton*              m_pBtRefresh;           //!< 刷新
    GUIButton*              m_pBtOrderQuery;        //!< 查询订单

    // 订单查询
    GUIWnd*                 m_pOrderWnd;            //!< 订单查询窗体

    GUIButton*              m_pOrderClose;          //!< 订单关闭按钮

    GUIListBox*             m_pOrderTable;          //!< 订单列表

    GUIButton*              m_pOrderID;             //!< 订单编号
    GUIButton*              m_pOrderType;           //!< 订单类型
    GUIButton*              m_pOrderPrice;          //!< 单价
    GUIButton*              m_pOrderNum;            //!< 数量
    GUIButton*              m_pOrderStus;           //!< 状态
    GUIButton*              m_pOrderTimeFrom;       //!< 开始时间
    GUIButton*              m_pOrderTimeTo;         //!< 结束时间
    GUIButton*              m_pOrderAveragePrice;   //!< 成交均价
    GUIButton*              m_pOrderFinalNum;       //!< 成交数量

    GUIButton*              m_pOrderCannel;         //!< 撤销订单
    GUIButton*              m_pOrderRefresh;        //!< 刷新订单

    bool                    m_bisSell;              //!< 是否是寄售/收购状态

};
