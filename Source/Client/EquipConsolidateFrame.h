#pragma once
#include "ComposeStatic.h"
#include "CompClidateDefine.h"
#include "AppliedTree.h"
#include "..\WorldDefine\msg_compose.h"
#include "..\WorldDefine\compose_define.h"
#include "ComColidateEvent.h"
#include "ItemEvent.h"
/*
class EquipConsolidateFrame
brief 装备强化(铭文、镌刻、淬火)
*/
class NetSession;
class EquipConsolidateFrame :
	public GameFrame
{
private:
	enum EEquipConsolidate
	{
		EEConsolidate_NULL = 0,
		EEConsolidate_SendMsg,	
		EEConsolidate_Waiting,
		EEConsolidate_RecvMsg,
	};
public:
	EquipConsolidateFrame(void);
	~EquipConsolidateFrame(void);

	//--GameFrame
	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();
	virtual VOID OnEvent(tagGameEvent* pEvent);
	virtual BOOL EscCancel();

	// 设置强化方式
	void SetConsolidateMode(EConsolidateType eType);

	// 显示/隐藏强化窗体
	void ShowWnd(BOOL bVale) { if( P_VALID(m_pWndMain) ) m_pWndMain->SetInvisible(!bVale); m_eCurState=EEConsolidate_NULL;}

	// 打开界面方式及ID
	void SetOpenUIMode(EStartCompose eType, ComposeID uID);

private:
	DWORD EventHandler(tagGUIEvent* pEvent);

	// 清空材料、主料和提高成功率按钮
	void ClearStuff();

	// 按选择的配方先清除辅料及精料的提示
	void ClearStuffSelectFormula();

	//--配方
	void SetFormula(BYTE byTimes);
	void DeleteFormula();
	void AddFormula2Tree(EConsolidateType eType, DWORD dwFormulaID);

	// 判断配方强化条件
	bool JudgeFormulaForCliDate(EConsolidateType eType, DWORD dwID);

	// 按配方设置辅料
	void SetStuffOnFormula(EConsolidateType eType,  DWORD dwID);

	//--处理网络消息
	DWORD NetRecvConsolidatePosy(tagNS_ConsolidatePosy* pMsg, DWORD pPrama);
	DWORD NetRecvConsolidateEngrave(tagNS_ConsolidateEngrave* pMsg, DWORD pPrama);
	DWORD NetRecvConsolidateQuench(tagNS_ConsolidateQuench* pMsg, DWORD pPrama);
	void SendConsolidateMsg();

	// 收到网络消息后重新设置主料，并刷新配方、清空辅料
	void ResetStuffFormular(); 

	DWORD HandleDragStuff(tagGUIEvent* pEvent);

	BOOL JudgeSpecItem(ESpecFuncComposeAdvance eSpec);

	//背包右击装备或物品添加到强化系统
	DWORD OnEventConsolidateAddItem(tagItemPosChangeExEvent* pGameEvent);

	//操作结束后清空UI,重新设置配方
	void OperatedClear();

	//
	bool RefreshItemNum2UI();

	//计算最终成功率
	void UtilCalculateSuccessRatio();

	float SetSuccessRatioAndGetStuffEffect(DWORD dwSrcSuccess);

	// 铭文成功后增加的属性
	void ShowNewPosyAtt( const tagPosyProto *pPosy, float fStuffEffect );

	// 镌刻成功后增加的属性
	void ShowNewEngraveAtt( const tagEngraveProto *pEngrave, float fStuffEffect );

	void HandleItem( bool bOperable );

private:
	TSFPTrunk<EquipConsolidateFrame>	m_Trunk;
	TObjRef<GUISystem>					m_pGUI;
	TObjRef<Util>						m_pUtil;
	TObjRef<GameFrameMgr>				m_pFrameMgr;
	TObjRef<NetSession>					m_pSession;
	TObjRef<NetCmdMgr>					m_pCmdMgr;

	GUIWnd*						m_pWndMain;
	GUIWnd*						m_pWndCaption;
	GUIStatic*					m_pWndName;				//强化窗品标题
	GUIButton*					m_pBtnExit;				//退出

	GUIStatic*					m_pStcClidateBack;
	GUIButton*					m_pBtnOk;				//确定
	GUIButton*					m_pBtnCancel;			//取消
	GUIStatic*					m_pStcNotice;			//公告

	GUIPatch*					m_pStcTreeBack;
	AppliedTree*				m_pTreeList;			//配方列表
	GUIStatic*					m_pStcListTitle;		//配方标题

	GUIStatic*					m_pStcSuccTitle;		//成功率标题
	ComposeStatic*				m_pStcSuccCtrl;			//放入提高成功率的物品按钮
	GUIStatic*					m_pStcSuccOdds;			//显示成功率
	GUIStatic*					m_pStcModi;				//修改率提示
	GUIStatic*					m_pStcModiOdds;			//修正率

	GUIStatic*					m_pStcNeedShow;			//提示

	GUIStatic*					m_pStcGolden;			//金的显示
	GUIStatic*					m_pStcSiller;			//银的显示

	GUIEditBox*					m_pEditComNum;			//打造合成的数目
	GUIStatic*					m_pStcConNum;			//强化数目提示
	GUIStatic*					m_pStcMasterHint;		//熟练度提示
	GUIStatic*					m_pStcSuccMaster;		//熟练度
	GUIStatic*					m_pStcPotValHint;		//潜力提示
	GUIStatic*					m_pStcPotVal;			//需要消耗潜力

	GUIStatic*					m_pStcSuccHint;			//成功提示
	GUIStatic*					m_pStcLoseHint;			//失败提示
	GUIStatic*					m_pStcPerfectHint;		//完美提示

	GUIStatic*					m_pStcShowAddedValue;	// 强化加值预显示

	GUIProgress*				m_pProTime;				//进度条
	GUIPushButton*				m_pPBtnFormular[FORMULA_QUANTITY];			//配方pushbutton
	ComposeStatic*				m_pStcCtrl[CTRL_QUANTITY];					//合成控件
	GUIStatic*					m_pStcMainStuffHint;						//主料提示
	GUIStatic*					m_pStcStuffHint[STUFF_QUANTITY];			//辅材料的提示
	GUIStatic*					m_pStcStuffQuan[STUFF_QUANTITY];			//辅材料总数/个数

	DWORD						m_dwRecvMsgTime;					//成功\失败\完美时提示的时间

	typedef std::map<DWORD, DWORD> TreeFormularMap;				
	TreeFormularMap				m_tree2formula;						//树ID对应的配方ID
	DWORD						m_dwFormulaID;						//当前选择的配方ID

	CompConsoContainer			m_Container;						//控件的数据容器
	EConsolidateType			m_eConType;							//强化类型

	EEquipConsolidate			m_eCurState;						//当前消息状态

	DWORD						m_dwNPCID;							//NPC的ID
	DWORD						m_escapeTime;
	DWORD						m_dwErrorCode;						//失败时可以连续锻造
	INT							m_nLoseTry;						//失败时重复次数
};