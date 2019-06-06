#pragma once

#include "..\WorldDefine\msg_talent.h"
#include "..\WorldDefine\compose_define.h"
class NetSession;
class EquipConsolidateFrame;
class ItemProduceFrame;
class DeComposeFrame;
struct tagStartNPCTalkEvent;
struct tagOnTheRoleOfItemEvent;
struct tagNS_Inlay;
struct tagNS_Brand;
struct tagNS_AddPot;
struct tagNS_LoongSoul;
struct tagUseSkillEvent;
struct tagNS_Chisel;
#define	MTransProSkillType(id) (DWORD)(id%9000/100)
#define MTransProduceSkillEx(id) (DWORD)(id/1000)
class ComposeMgr
{
public:
	ComposeMgr(void);
	~ComposeMgr(void);

	static ComposeMgr* Inst();

	BOOL Init(); 

	VOID Destroy();

	//--
	const EquipConsolidateFrame* GetEquipConsolidateFrame() { return m_pEquipCon; }
	const ItemProduceFrame* GetItemProduceFrame() { return m_pItemPro; }

	//--学习/遗忘生产技能
	void LearnProduceSkill(DWORD dwSkillID, DWORD dwNpcID);
	void LeaveProduceSkill(DWORD dwSkillID, DWORD dwNpcID);

	DWORD GetColorByQuality(BYTE byQuality);
	
private:
	//--
	BOOL CreateEquipConsolidateFrame();
	BOOL CreateItemProduceFrame();
	BOOL CreateDeComposeFrame();

	//--
	DWORD OnStartNPCTalkEvent(tagStartNPCTalkEvent* pEvt);
	DWORD OnConslidateItemEvent(tagOnTheRoleOfItemEvent* pEvt);
	DWORD OnChiselItemEvent(tagOnTheRoleOfItemEvent* pEvt);
	DWORD OnLearnProduceSkillEvent(tagMsgBoxEvent* pEvent);
	DWORD OnLeaveProSkillEvent(tagMsgBoxEvent* pEvent);
	DWORD OnUseSkillEvent(tagUseSkillEvent* pEvent);
	DWORD OnCloseComposeFrameEvent(tagGameEvent* pEvent);
	DWORD OnCloseInsuranceFrameEvent(tagGameEvent* pEvent);

	//--
	DWORD OnNS_Inlay(tagNS_Inlay* pMsg, DWORD dwParam);
	DWORD OnNS_Brand(tagNS_Brand* pMsg, DWORD dwParam);
	DWORD OnNS_AddPot(tagNS_AddPot* pMsg, DWORD dwParam);
	DWORD OnNS_LoongSoul(tagNS_LoongSoul* pMsg, DWORD dwParam);
	DWORD OnNS_Chisel(tagNS_Chisel* pMsg, DWORD dwParam);
	DWORD OnNS_LearnSkill(tagNS_LearnSkill* pMsg, DWORD dwParam);
	DWORD OnNS_ForgetSkill(tagNS_ForgetSkill* pMsg, DWORD dwParam);

	//
	EProduceType GetProduceType(DWORD dwSkillID);
	BOOL IsProduceSKill(DWORD dwSkillID);

	
private:
	TSFPTrunk<ComposeMgr>		m_Trunk;
	EquipConsolidateFrame*		m_pEquipCon;			//强化
	ItemProduceFrame*			m_pItemPro;				//合成
	DeComposeFrame*				m_pDeCompose;			//点化

	DWORD						m_dwSkillID;
	DWORD						m_dwNPCID;
};