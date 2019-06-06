#pragma once

#include "buttonex.h"
/** class SkillButton
	brief 技能显示按钮
*/
class SkillButton :
	public ButtonEx
{
public:
	SkillButton(void);
	~SkillButton(void);

	virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
	virtual VOID Render();
	virtual VOID Update();
	virtual VOID Destroy();
	virtual BOOL OnInputMessage(tagGUIInputMsg* pMsg);
	virtual VOID SendEvent(tagGUIEvent* pEvent);
	virtual VOID OnEvent(tagGUIEvent* pEvent); // 接受事件
	// 刷新技能按钮
	void ReflashSkill(DWORD dwID);

	//测试，冷却时间
	void SetCDTime(float time) { m_fCDCount = time; }

	//测试，旋转率
	void SetRestoreRatio(float time) { m_fRestoreRatio = time; }

private:

private:

	tagGUIImage*				m_pBtnPic;					//按下时增加的一个透明图片


	TObjRef<GameFrameMgr>		m_pFrameMgr;		//frame管理器

};
