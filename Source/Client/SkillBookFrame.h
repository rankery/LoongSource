#pragma once
#include "SkillItem.h"
struct tagUpdateLPSkillEvent;

const INT MAX_SKILL_BOOK_NUM = 16;


class SkillBookFrame :
	public GameFrame
{
public:
	SkillBookFrame();
	~SkillBookFrame();

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();
	
	// 处理技能界面打开
	VOID OnSkillBookOpen();

private:
	// 处理GUI消息
	DWORD EventHandler(tagGUIEvent* pEvent);

	// 处理游戏事件
	DWORD OnUpdateLPSkillEvent(tagUpdateLPSkillEvent* pGameEvent);
	DWORD OnWeaponSwap(tagGameEvent* pGameEvent);
	

	// 是否是当前页
	BOOL IsCurPage(INT nIndex){return nIndex >= m_nCurPage*MAX_SKILL_BOOK_NUM 
		&& nIndex < (m_nCurPage+1)*MAX_SKILL_BOOK_NUM;}
	// 设置当前页码
	VOID SetCurPage(INT nPage);
	// 处理翻页事件
	VOID OnPageUp(BOOL bUpPage);

	// 刷新当前页
	VOID ReflashCurPage();
private:

	TSFPTrunk<SkillBookFrame>	m_Trunk;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<Util>				m_pUtil;
	TObjRef<GameFrameMgr>		m_pFrameMgr;

	GUIWnd*						m_pWnd;
	GUIStatic*					m_pStcPage;
	GUIButton*					m_pBtnPageUp;
	GUIButton*					m_pBtnPageDown;

	SkillItem					m_SkillItem[MAX_SKILL_BOOK_NUM];

	TList<DWORD>				m_skillList;//--技能列表缓存
	TList<DWORD>::TListIterator m_skillIter;//--技能列表迭代器

	INT							m_nCurPage;

};
