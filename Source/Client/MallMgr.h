#pragma once
#include "..\WorldDefine\mall_define.h"
#include "..\WorldDefine\msg_mall.h"
/** class MallMgr
	brief 商城管理器
*/

class NetSession;
class MallFrame;
class YuanbaoFrame;
class GroupBuyFrame;
class MallMgr
{
public:
	MallMgr(void);
	~MallMgr(void);
	static MallMgr* Inst();

	VOID Init();
	VOID Update();
	VOID Destroy();

	//--处理网络消息
	DWORD OnNS_OpenMall(tagNS_OpenMall* pMsg, DWORD dwParam);
	DWORD OnNS_MallItem(tagNS_MallItem* pMsg, DWORD dwParam);
	DWORD OnNS_MallFreeItem(tagNS_MallFreeItem* pMsg, DWORD dwParam);
	DWORD OnNS_MallPack(tagNS_MallPack* pMsg, DWORD dwParam);
	DWORD OnNS_MallUpdateItem(tagNS_MallUpdateItem* pMsg, DWORD dwParam);
	DWORD OnNS_MallUpdatePack(tagNS_MallUpdatePack* pMsg, DWORD dwParam);
	DWORD OnNS_MallUpdate(tagNS_MallUpdate* pMsg, DWORD dwParam);
	DWORD OnNS_GetSimGPInfo(tagNS_GetSimGPInfo* pMsg, DWORD dwParam);

	//--处理游戏事件
	DWORD OnOpen_Mall(tagGameEvent* pEvent);
	DWORD OnClose_Mall(tagGameEvent* pEvent);

	//--
	TMap<DWORD, tagMallItemProto*>& GetMallProto()	{ return m_mapMallItem; }
	TMap<DWORD, tagMallPackProto*>& GetPackItem()	{ return m_mapPackItem; }
	tagMallFreeItem* GetMallFreeItem()				{ return &m_MallFreeItem; }
	tagMallItemProto* GetMallPromoteItem(int nIndex);
	map<INT64, tagGroupPurchase>& GetGroupBuyInfo() { return m_mapGgroupPur; }

	//--
	INT GetMallProtoSize() { return m_mapMallItem.Size(); }
	INT GetPackProtoSize() { return m_mapPackItem.Size(); } 
	INT GetGroupBuyInfoSize() { return m_mapGgroupPur.size(); }

	//--
	tagMallItemProto* GetMallItemByID(DWORD dwID);
	tagMallPackProto* GetMallPackByID(DWORD dwID);
	tagGroupPurchase* GetGroupBuyInfoByID(INT64 dwID);
	void DeleGroupBuyInfoByID(INT64 dwID);
	void RefreshGroupBuyInfoByID(INT64 dwID, INT nNum);


	//--免费领取的时间
	DWORD GetFreeGetTime()const { return m_dwFreeGetTime; }
	void SetFreeGetTime(DWORD dwTime) { m_dwFreeGetTime = dwTime; } 

	//--
	void ClearMallItem();
	void ClearMallPack();
	void ClearGroupBuyInfo();

	//商城frame
	MallFrame* GetMallFrame() { return m_pMallFrame; }

	//--团购
	void CreateGroupBuyFrame(LPCTSTR szFatherName);
	GroupBuyFrame* GetGroupBuyFrame() { return m_pGroupBuyFrame; }
	void SendGetGroupBuyInfo();
	INT GetGroupPurExp()const { return m_nGroupPurExp; }
	void SetGroupPurExp(INT32 exp)  { m_nGroupPurExp = exp; }

	//--赠送窗品
	void CreateMallGiftFrame();
	void SetMallGiftFrameMaxBuyNum(INT nMaxBuyNum);

    //! 元宝交易
    void CreateYuanBaoFrame(LPCTSTR szFatherName);

	// 装备栏和换装属性换算接口
	INT16 GetEquipDisplayPos(INT16 n16EquipPos);

	//--显示tips用到的时间计算
	DWORD GetDay(DWORD& dwTimeDiff)
	{
		return dwTimeDiff / dwOneDay;
	}
	DWORD GetHour(DWORD& dwTimeDiff, DWORD& dwDay)
	{
		return (dwTimeDiff - (dwDay*dwOneDay)) / dwOneHour;
	}
	DWORD GetMin(DWORD& dwTimeDiff, DWORD& dwDay, DWORD& dwHour)
	{
		return (dwTimeDiff - (dwHour*dwOneHour) - (dwDay*dwOneDay)) / 60;
	}

private:
	TSFPTrunk<MallMgr>					m_Trunk;
	TObjRef<Util>						m_pUtil;
	TObjRef<NetCmdMgr>					m_pCmdMgr;
	TObjRef<NetSession>					m_pSession;
	TObjRef<GameFrameMgr>				m_pFrameMgr;
		
	MallFrame*							m_pMallFrame;
    YuanbaoFrame*                       m_pYuanbaoFrame;
	GroupBuyFrame*						m_pGroupBuyFrame;

	map<DWORD, tagMallItemProto>		m_mapMallItemEx;			//全部非打包商品
	map<DWORD, tagMallPackProto>		m_mapPackItemEx;			//全部打包商品

	TMap<DWORD, tagMallItemProto*>		m_mapMallItem;			
	TMap<DWORD, tagMallPackProto*>		m_mapPackItem;	
	tagMallItemProto*					m_pProItem[MAX_PromoteItem];//推荐商品
	tagMallFreeItem						m_MallFreeItem;				//免费领取的商品
	DWORD								m_dwFreeGetTime;			//免费领取的时间
	DWORD								m_dwMallTime;				//商城时间	
	DWORD								m_dwGroupBuyTime;			//团购时间
	BOOL								m_bOpen;

	//--以下为团购
	map<INT64, tagGroupPurchase>		m_mapGgroupPur;				//团购的数据(以商城商品TypdID为Key)
	INT									m_nGroupPurExp;				//帮派团购指数


	static const DWORD					dwOneDay  = 24 * 60 * 60;
	static const DWORD					dwOneHour = 60 * 60;
};