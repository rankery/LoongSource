#pragma once
#include "ComposeStatic.h"
#include "CompClidateDefine.h"
#include "AppliedTree.h"
#include "..\WorldDefine\compose_define.h"
#include "ComColidateEvent.h"
#include "..\WorldDefine\msg_item.h"
#include "..\WorldDefine\msg_compose.h"
#include "ItemEvent.h"
/*
	class ItemSmithComposeFrame
	brief 物品生产合成
*/

class NetSession;
struct tagUpdateLPSkillEvent;
class ItemProduceFrame :
	public GameFrame
{
private:
	enum EItemProduceMsg
	{
		EEItemProduce_NULL = 0,
		EEItemProduce_SendMsg,		
		EEItemProduce_Waiting,
		EEItemProduce_RecvMsg,
	};

public:
	ItemProduceFrame(void);
	~ItemProduceFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();
	virtual VOID OnEvent(tagGameEvent* pEvent);
	virtual BOOL EscCancel();

	// 设置生产方式
	void SetProduceMode(EProduceType eType);

	// 显示/隐藏生产合成窗体
	void ShowWnd(BOOL bVale) { if( P_VALID(m_pWndMain) ) m_pWndMain->SetInvisible(!bVale); }

	// 打开界面方式及ID
	void SetOpenUIMode(EStartCompose eType, ComposeID uID);

private:
	BOOL EventHandler(tagGUIEvent* pEvent);

	//--设置配方
	void SetFormula(EProduceType eType);
	void AddFormula2Tree(EProduceType eProType, EComposeType eComType);
	void DeleteFormula();
	//点化配方特殊处理
	void SetFormulaForPointUp();

	// 根据配方设置材料
	void SetStuffOnFormula(EProduceType eType, DWORD dwID);

	// 清除材料数据
	void ClearStuffSelectFormula();

	// 清空控件及缓存的数据
	void ClearStuff();

	// 开始生成,判断材料是否满足配方
	void JudgeStuffIsContentForm(EProduceType eType, DWORD dwID);

	// 多次合成时,当合成完一次时,重新判断材料的个数是否满足配方要求
	bool ReCountStuff();

	// 发送合成消息
	void SendProduceMsg();
	void SendDeComposeMsg();
	DWORD NetRecvItemUpdate(tagNS_ItemRemove* pMsg, DWORD pPrama);
	DWORD NetRecvNS_Produce(tagNS_Produce* pMsg, DWORD pPrama);
	DWORD NetRecvNS_Decomposition(tagNS_Decomposition* pMsg, DWORD pPrama);

	BOOL HandleDragStuff(tagGUIEvent* pEvent);

	//判断特殊物品是否用于生成
	BOOL JudgeSpecItem(ESpecFuncComposeAdvance eSpec);

	void SetStcCtrlState(EProduceType eType);

	void ResetStartState();

	//技能升级,刷新配方
	DWORD OnUpdateLPSkillEvent(tagUpdateLPSkillEvent* pEvent);
	
	//背包右击装备或物品添加到生产系统
	DWORD OnEventProduceAddItem(tagItemPosChangeExEvent* pGameEvent);

	void PushButtonState(bool bEnable);

	// 
	void UtilCalculateSuccessRatio();

	void HandleItem( bool bOperable );


private:
	TSFPTrunk<ItemProduceFrame>			m_Trunk;
	TObjRef<GUISystem>					m_pGUI;
	TObjRef<Util>						m_pUtil;
	TObjRef<GameFrameMgr>				m_pFrameMgr;
	TObjRef<NetSession>					m_pSession;
	TObjRef<NetCmdMgr>					m_pCmdMgr;

	GUIWnd*						m_pWndMain;
	GUIWnd*						m_pWndCaption;
	GUIStatic*					m_pWndName;				//合成窗口标题
	GUIButton*					m_pBtnExit;				//退出

	GUIStatic*					m_pStcComBack;
	GUIButton*					m_pBtnOk;				//确定
	GUIButton*					m_pBtnCancel;			//取消
	GUIStatic*					m_pStcNotice;			//公告

	GUIPatch*					m_pStcTreeBack;
	AppliedTree*				m_pTreeList;			//配方列表
	GUIStatic*					m_pStcListTitle;		//配方标题

	GUIStatic*					m_pStcSuccTitle;		//成功率标题
	ComposeStatic*				m_pStcSuccCtrl;			//放入提高成功率的物品按钮
	GUIStatic*					m_pStcSucc;				//成功率提示
	GUIStatic*					m_pStcSuccOdds;			//显示成功率
	GUIStatic*					m_pStcModi;				//修改率提示
	GUIStatic*					m_pStcModiOdds;			//修正率

	GUIStaticEx*				m_pStcNeedShow;			//提示

	GUIStatic*					m_pStcGolden;			//金的显示
	GUIStatic*					m_pStcSiller;			//银的显示

	GUIEditBox*					m_pEditComNum;			//打造合成的数目
	GUIStatic*					m_pStcConNum;			//强化数目提示
	GUIStatic*					m_pStcMasterHint;		//熟练度提示
	GUIStatic*					m_pStcSuccMaster;		//熟练度
	GUIStatic*					m_pStcPotValHint;		//活力提示
	GUIStatic*					m_pStcPotVal;			//需要消耗活力

	GUIStatic*					m_pStcSuccHint;			//成功提示
	GUIStatic*					m_pStcLoseHint;			//失败提示
	GUIStatic*					m_pStcPerfectHint;		//完美提示

	GUIProgress*				m_pProTime;				//进度条
	GUIPushButton*				m_pPBtnFormular[FORMULA_QUANTITY];			//配方pushbutton
	ComposeStatic*				m_pStcCtrl[CTRL_QUANTITY];					//合成控件
	GUIStatic*					m_pStcMainStuffHint;						//主料提示
	GUIStatic*					m_pStcStuffHint[STUFF_QUANTITY];			//材料的提示
	GUIStatic*					m_pStcStuffQuan[STUFF_QUANTITY];			//材料总数/个数

	DWORD						m_dwItemSuccID;						//提高成功率的物品ID
	DWORD						m_dwStartTime;						//成功\失败\完美时提示的时间
	DWORD						m_dwRecvMsgTime;					//成功\失败\完美时提示的时间
	INT							m_nProduceNum;						//合成次数

	typedef std::map<DWORD, DWORD> TreeFormularMap;				
	TreeFormularMap				m_tree2formula;						//树ID对应的配方ID
	DWORD						m_dwFormulaID;						//当前选择的配方ID

	EProduceType				m_eProType;							//生成类型
	EComposeType				m_eComType;							//合成类型(生成类型的子类)
	CompConsoContainer			m_Container;						//材料容器
	
	EItemProduceMsg				m_eCurState;						//当前消息状态

	EStartCompose				m_eComposeType;						//是什么方式打开的UI
	ComposeID					m_uComposeID;
	DWORD						m_escapeTime;
};
