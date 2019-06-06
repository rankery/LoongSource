#pragma once

#include "ItemProtoData.h"
#include "ItemMgr.h"
#include "PlayerNameTab.h"
#include "..\WorldDefine\msg_famehall.h"

//! 本地玩家尝试激活珍宝后反馈处理
struct tagActTreasureRezult : tagGameEvent
{
    DWORD           dwTreasureid;   //!< 珍宝id
    DWORD           dwErrCode;      //!< 错误码，参见msg_famehall.h

    tagActTreasureRezult(LPCTSTR szEventName, GameFrame* pSenderFrame)
        : tagGameEvent(szEventName, pSenderFrame) {}
};

//! 服务器广播新珍宝激活成功后的事件
struct tagNewActivedTreasure : tagGameEvent
{
    DWORD        dwTreasureid;      //!< 珍宝id

    tagNewActivedTreasure(LPCTSTR szEventName, GameFrame* pSenderFrame)
        : tagGameEvent(szEventName, pSenderFrame) {}
};

//! \struct tagTreasureMgrData
//! \brief TreasureMgr使用的容器结构
struct tagTreasureMgrData
{
    ECLanType       eClanType;      //!< 所属氏族
    DWORD           dwTreasureID;   //!< 珍宝id
    DWORD           dwItemID;       //!< 对应的itemid
    DWORD           dwNameID;       //!< 名字ID
    tstring         strNamePrefix;  //!< 名字前缀，来源于NameID
    tstring         strNameSuffix;  //!< 名字后缀，Item本来名字
    INT             nConNeed;       //!< 所需消耗类型
    bool            bActived;       //!< 激活状态
    DWORD           dwShopID;       //!< 商店ID
};

//! \class TreasureMgr
//! \brief 氏族珍宝管理器
//! \detail 氏族珍宝也是物品，也可以是装备，可以通过珍宝id取item,也可以通过item查是否是珍宝
//! \author hyu
//! \date 2009-3-6
class TreasureMgr
{
public:
    typedef map<DWORD, tagTreasureMgrData*>      _MapTreasure;
    typedef multimap<DWORD, tagTreasureMgrData*> _MultiMapTrea;    

    TreasureMgr(void);
    ~TreasureMgr(void);

    static TreasureMgr* Inst();

    VOID Init();
    VOID Update();
    VOID Destroy();

    //! 激活氏族珍宝
    //! \param dwTreasureID 珍宝id
    //! \brief 给服务器发送激活珍宝请求,返回结果在OnNS_ActiveTreasure
    //! \sa OnNS_ActiveTreasure
    VOID ActiveTreasure(DWORD dwTreasureID);

    //! 通过TreasureID返回ItemID,无效返回GT_INVALID
    DWORD GetItemIDbyTreaID(DWORD dwTreaID);

    //! 通过ItemID判断是否是Treasure,是的话返回TreasureID,无效返回GT_INVALID
    DWORD GetTreaIDbyItemID(DWORD dwItemID, ECLanType eType);

    //! 判断当前此氏族珍宝是否已激活
    bool IsTreaActivedbyItemID(DWORD dwItemID, ECLanType eType);

    //! 通过ItemID判断是否是Treasure,是的话返回珍宝全名,无效返回空串
    tstring GetTreaNamebyItemID(DWORD dwItemID, ECLanType eType);

    //! 得到珍宝全名
    tstring GetTreasureName(DWORD dwTreaID);

    //! 得到珍宝消耗贡献,无效返回GT_INVALID
    INT GetContributNeed(DWORD dwTreaID);
    
    //! 直接得到容器
    const _MapTreasure GetAllTreaByTreaID() {return m_mapTreasurebyTrID;}
    const _MultiMapTrea GetAllTreaByItemID() {return m_mapTreasurebyItemID;}
    const _MapTreasure GetAllTreaByTreaID(ECLanType eVal) {return m_mapTreasurebyTrIDA[eVal];}
    const _MultiMapTrea GetAllTreaByItemID(ECLanType eVal) {return m_mapTreasurebyItemIDA[eVal];}
    const _MapTreasure GetAllActived(ECLanType eVal) {return m_mapActivedbyTrIDA[eVal];}
    const _MapTreasure GetAllUnActived(ECLanType eVal) {return m_mapUnActivedbyTrIDA[eVal];}

private:
    //! 从proto读数据
    VOID LoadData();

    //! 清除数据
    VOID ClearData();

    //! 获取名字缓存
    DWORD OnGetNameByNameID(tagGetNameByNameID* msg);

    //! 获得所有氏族珍宝,请球消息在ReputeMgr.cpp里发送，这里只顾接收
    DWORD OnNS_GetActClanTreasure(tagNS_GetActClanTreasure* pNetCmd, DWORD dwParam);

    //! 当服务器有新氏族珍宝初激活发出的广播
    DWORD OnNS_NewActivedTreasure(tagNS_NewActivedTreasure* pNetCmd, DWORD dwParam);

    //! 本地玩家激活一个珍宝后的反馈
    DWORD OnNS_ActiveTreasure(tagNS_ActiveTreasure* pNetCmd, DWORD dwParam);

    TSFPTrunk<TreasureMgr>  m_Trunk;
    TObjRef<Util>           m_pUtil;
    TObjRef<NetCmdMgr>      m_pCmdMgr;
    TObjRef<NetSession>     m_pSession;
    TObjRef<GameFrameMgr>   m_pFrameMgr;

    _MapTreasure            m_mapTreasurebyTrID;                //!< 通过珍宝id查珍宝
    _MultiMapTrea           m_mapTreasurebyItemID;              //!< 通过itemid查珍宝
    _MapTreasure            m_mapTreasurebyTrIDA[ECLT_NUM];     //!< 按氏族分类 by TreasureID
    _MultiMapTrea           m_mapTreasurebyItemIDA[ECLT_NUM];   //!< 按氏族分类 by ItemID
    _MapTreasure            m_mapActivedbyTrIDA[ECLT_NUM];      //!< 按氏族分类 全部激活的珍宝 by TreasureID
    _MapTreasure            m_mapUnActivedbyTrIDA[ECLT_NUM];    //!< 按氏族分类 全部未激活珍宝 by TreasureID

    _MultiMapTrea           m_mulmapNameIDReq;                  //!< 从nametable取珍宝名前缀，有可能同一个名字激活了多件珍宝
};
