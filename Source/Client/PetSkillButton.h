#pragma once
#include "buttonex.h"

struct tagPetSkillData;

//! \class  PetSkillButton
//! \brief  宠物技能按钮
//! \author hyu
//! \date   2009-7-22
class PetSkillButton :
    public ButtonEx
{
public:
    PetSkillButton(void);
    ~PetSkillButton(void);

    virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
    virtual VOID Render();
    virtual VOID Update();
    virtual VOID Destroy();
    virtual BOOL OnInputMessage(tagGUIInputMsg* pMsg);
    virtual VOID SendEvent(tagGUIEvent* pEvent);
    virtual VOID OnEvent(tagGUIEvent* pEvent); // 接受事件

    // 刷新技能按钮
    void RefrashSkill(DWORD dwPetID, DWORD dwSkillID);

    // 取得按钮所拥有的技能原型数据
    const tagPetSkillData* GetSkillData()
    {
        return m_pSkillData;
    }

private:

    //设置当前冷却时间
    void SetCDTime(float time) { m_fCDCount = time; }

    //设置当前旋转率
    void SetRestoreRatio(float time) { m_fRestoreRatio = time; }

    const tagPetSkillData* m_pSkillData;    //!< 宠物技能数据
};
