#pragma once
#include "ComposeStatic.h"
#include "CompClidateDefine.h"
#include "AppliedTree.h"
#include "..\WorldDefine\compose_define.h"
#include "ComColidateEvent.h"
#include "..\WorldDefine\msg_item.h"
#include "..\WorldDefine\msg_compose.h"
#include "ItemEvent.h"

struct tagUpdateLPSkillEvent;
class DeComposeFrame : public GameFrame
{
private:
	enum EDeComposeMsg
	{
		EDeCompose_NULL = 0,
		EDeCompose_SendMsg,		
		EDeCompose_Waiting,
		EDeCompose_RecvMsg,
	};
public:
	DeComposeFrame(void);
	~DeComposeFrame(void);

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
	DWORD EventHandler(tagGUIEvent* pEvent);

	DWORD NetRecvNS_Decomposition(tagNS_Decomposition* pMsg, LPVOID pPrama);

	//背包右击装备或物品添加到生产系统
	DWORD OnEventProduceAddItem(tagItemPosChangeExEvent* pGameEvent);

	//拖动释放
	DWORD HandleDragStuff(tagGUIEvent* pEvent);

	//判断特殊物品是否用于生成
	BOOL JudgeSpecItem(ESpecFuncComposeAdvance eSpec);

	//发送网络消息
	void SendDecomposeMsg();

	//重新设置界面
	void ResetStartUIState();

	//清除界面及保存数据
	void ClearUIandStuff();

	//判断是否能点化或分解
	void JudgeDecompositonFormula(DWORD dwItemID=GT_INVALID, INT64 n64ID=GT_INVALID, EEquipPos ePos=EEP_Null);

	//判断是否可以发送点化消息
	void JudgeSendDecompose();

	void PushButtonState(bool bEnable);

	//技能升级,刷新技能
	DWORD OnUpdateLPSkillEvent(tagUpdateLPSkillEvent* pEvent);


private:
	TSFPTrunk<DeComposeFrame>			m_Trunk;
	TObjRef<GUISystem>					m_pGUI;
	TObjRef<Util>						m_pUtil;
	TObjRef<GameFrameMgr>				m_pFrameMgr;
	TObjRef<NetSession>					m_pSession;
	TObjRef<NetCmdMgr>					m_pCmdMgr;

	GUIWnd*								m_pWndMain;
	GUIWnd*								m_pWndCaption;
	GUIWnd*								m_pWndBack;
	GUIButton*							m_pBtnExit;
	GUIButton*							m_pBtnCancel;
	GUIButton*							m_pBtnOk;
	GUIListBox*							m_pDeStuffList;			//成分出材料
	GUIStatic*							m_pStcTitle;			//标题
	ComposeStatic*						m_pDeUnit;				//分解物品
	ComposeStatic*						m_pDeSucc;				//提高成功率
	GUIStatic*							m_pStcPotVal;			//活力
	GUIStatic*							m_pStcGold;				//金
	GUIStatic*							m_pStcSilver;			//银
	GUIProgress*						m_pProTime;				//进度条
	GUIPushButton*						m_pPBtnType[PointUpSubTypeQuan];	//点化类

	DWORD								m_dwFormulaID;						//当前选择的配方ID
	DWORD								m_dwRecvMsgTime;					//成功\失败\完美时提示的时间
	EProduceType						m_eProType;							//生成类型
	EComposeType						m_eComType;							//合成类型(生成类型的子类)
	CompConsoContainer					m_Container;						//材料容器
	EDeComposeMsg						m_eCurState;						//当前消息状态
	EStartCompose						m_eComposeType;						//是什么方式打开的UI
	ComposeID							m_uComposeID;
	DWORD								m_escapeTime;
};