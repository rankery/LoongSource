#pragma once
#include "gameframe.h"
#include "../WorldDefine/ItemDefine.h"
#include "../WorldDefine/msg_compose.h"

struct tagNS_GetGemRemovalInfo;

class Item;
class ComposeStatic;
class IconStatic;
class IconStaticHL;

class GemRemovalFrame : public GameFrame
{

public:
	GemRemovalFrame(void);
	~GemRemovalFrame(void);

	virtual BOOL Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam );
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();
	virtual VOID OnEvent( tagGameEvent* pEvent );

	void SetNPCID( DWORD dwNPCID ){ m_dwNPCId = dwNPCID; }
	void ShowWnd( BOOL bShow )
	{
		if( P_VALID( m_pWnd ) )
		{
			m_pWnd->SetInvisible( !bShow );
			m_pWnd->SetRefresh( TRUE );
		}
	}

private:
	BOOL EventHandler( tagGUIEvent* pEvent );
	
	/** 显示确认界面
	*/
	VOID ShowSureWnd();
	
	/** 得到最终想要的宝石，和拆除符的信息
	*/
	INT16 GetWantGemNum( tagNC_GemRemoval& evt );

	/** 发送宝石拆除请求之前-检查
	*/
	bool SubmitCheck();

	/** 清空所有控件，和数据
	*/
	void ClearAll();

	void DisplayGoldCost( INT nGoldCost,INT	nSilver );

	struct tagGemRmSymbolInfo
	{
		INT64	n64ItemID;		//宝石拆除符的64位ID
		INT		nLevel;			//宝石拆除符的级别
		INT		nQuantity;		//宝石拆除符的数量

		tagGemRmSymbolInfo()
		{
			n64ItemID = GT_INVALID;
			nLevel = GT_INVALID;
			nQuantity = 0;
		}
	};

	/** \根据背包中的宝石拆除符信息，统计出我们需要的格式
	*/
	VOID CensusGemRmSignal( std::list<Item*>& lst );

	//-------网络事件处理器-------------------
	DWORD OnNS_GetRemovalInfo( tagNS_GetGemRemovalInfo* pMsg, LPVOID pPrama );
	DWORD OnNS_GetRemoval( tagNS_GemRemoval* pMsg, LPVOID pParam );

private:
	TSFPTrunk<GemRemovalFrame>			m_Trunk;
	TObjRef<GUISystem>					m_pGUI;
	TObjRef<Util>						m_pUtil;
	TObjRef<GameFrameMgr>				m_pFrameMgr;
	TObjRef<NetSession>					m_pSession;
	TObjRef<NetCmdMgr>					m_pCmdMgr;

	GUIWnd*				m_pWnd;
	GUIButton*			m_pBtnClose;							//关闭按钮
	GUIButton*			m_pBtnOk;								//确定按钮
	GUIButton*			m_pBtnCancel;							//取消按钮
	ComposeStatic*			m_pIconEquip;						//将要被拆除宝石的装备
	IconStaticHL*		m_pIconGems[MAX_EQUIPHOLE_NUM];			//将要得到的宝石的图片
	IconStatic*			m_pIconSymbols[MAX_EQUIPHOLE_NUM];		//将要消耗的宝石拆除符
	GUIStatic*			m_pStcGold;								//金
	GUIStatic*			m_pStcSilver;							//银

	GUIWnd*				m_pWndSure;								//二次确认界面
	GUIButton*			m_pBtnSureClose;						//确认界面的关闭按钮
	GUIButton*			m_pBtnSureOk;							//确认界面的OK
	GUIButton*			m_pBtnSureCancel;						//确认界面的取消按钮
	GUIListBox*			m_plbGems;								//确认界面里的宝石列表框
	GUIListBox*			m_plbSymbols;							//确认界面里的消耗列表框
	GUIStatic*			m_pStcSureGold;							//确认界面-金
	GUIStatic*			m_pStcSureSilver;						//确认界面-银


	DWORD				m_Gems[MAX_EQUIPHOLE_NUM];				//拆除的宝石的typeid
	std::list<tagGemRmSymbolInfo>		m_lstGmRmSymbolInfo;

	INT64				m_Symbols[MAX_EQUIPHOLE_NUM];			//消耗的宝石拆除符的64位ID
	DWORD				m_n64GoldCost;							//将要消耗的金
	INT					m_nSilverCost;							//将要消耗的银


	enum GemRemovalState
	{
		EGRS_Operate,
		EGRS_SendMsg,
		EGRS_ReceiveMsg
	};

	GemRemovalState		m_CurState;								//当前的操作状态
	INT64				m_n64Src;								//要拆除的物品
	DWORD				m_dwNPCId;								//npc id
	DWORD				m_dwElapseTime;							//计时器，检查到NPC的距离
};
