#pragma once

class SkillButton;

class SkillItem
{
public:
	SkillItem(void);
	~SkillItem(void);

	VOID SetInvisible(BOOL b);

	VOID ReloadUI(GUIWnd* pWnd, INT nIndex);

	VOID UpdateSkill(DWORD dwID);

	DWORD GetSkillID();
	GUIWnd* GetSkillWnd();

private:
	VOID SetName(LPCTSTR szName);
	VOID SetLv(INT nLv);
	VOID SetExp(INT nExp, INT nMax);
	VOID SetType(INT nType);

private:
	SkillButton*	m_pSkill;
	GUIStatic*		m_pStcBack;
	GUIStatic*		m_pStcName;
	GUIStatic*		m_pStcLv;
	GUIStatic*		m_pStcDesc;
	GUIProgress*	m_pProExp;

};
