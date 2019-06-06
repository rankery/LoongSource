#include "StdAfx.h"
#include "PetSkillButton.h"
#include "PetProtoData.h"

PetSkillButton::PetSkillButton(void) :
    m_pSkillData(NULL)
{
}

PetSkillButton::~PetSkillButton(void)
{
}

BOOL PetSkillButton::Init( GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement )
{
    BOOL bRes = ButtonEx::Init(pSystem, pFather, pXmlElement);

    return bRes;
}

VOID PetSkillButton::Render()
{
    if( !IsRefresh() )
        return GUIWnd::Render();

    DrawImg();

    // 绘制冷却时间的显示
    DrawShadow(m_ptSize.x);

    // 调用基类渲染
    ButtonEx::Render();
}

VOID PetSkillButton::Update()
{
    ButtonEx::Update();
}

VOID PetSkillButton::Destroy()
{
    ButtonEx::Destroy();
}

BOOL PetSkillButton::OnInputMessage( tagGUIInputMsg* pMsg )
{
    if( m_bDisable )
        return GUIStatic::OnInputMessage(pMsg);

    switch( pMsg->dwType )
    {
        // 鼠标左右键操作为点击使用
    case WM_RBUTTONUP:
    case WM_LBUTTONUP:
        if( m_pSystem->GetMousePoint() == this )
        {
            tagGUIEvent event(this, EGUIE_Click);
            SendEvent(&event);
        }
        break;
    case WM_MOUSEMOVE:
        {
            if( m_pSystem->GetMousePoint() == this )
            { 
                if( (MK_LBUTTON & pMsg->dwParam1) 
                    && PointInWnd(pMsg->pt) == FALSE )
                {
                    tagGUIEvent event(this, EGUIE_Drag, pMsg);
                    event.dwParam1 = EDT_Skill;
                    event.dwParam2 = m_BtnData.GetData1();
                    event.dwParam3 = m_BtnData.GetData2();
                    event.dwParam4 = MAKELONG( pMsg->pt.x, pMsg->pt.y);	
                    SendEvent(&event);
                    return true;
                }	
            }
            SetRefresh();
            //return GUIButton::OnInputMessage(pMsg);
        }
        break;	
    }
    return GUIStatic::OnInputMessage(pMsg);
}

VOID PetSkillButton::SendEvent( tagGUIEvent* pEvent )
{
    GUIButton::SendEvent(pEvent);
}

VOID PetSkillButton::OnEvent( tagGUIEvent* pEvent )
{
    if(pEvent->eEvent == EGUISCB_Drop)
    {
        tagPoint pt ( LOWORD( pEvent->dwParam4 ), HIWORD( pEvent->dwParam4 ) );
        if(this->PointInWnd(pt))
        {
            tagGUIEvent event = tagGUIEvent(this, (EGUIEvent)EGUISCB_Drop);
            event.dwParam1 = pEvent->dwParam1;
            event.dwParam2 = pEvent->dwParam2;
            event.dwParam3 = pEvent->dwParam3;
            event.dwParam4 = pEvent->dwParam4;
            SendEvent(&event);
        }
    }
}

void PetSkillButton::RefrashSkill( DWORD dwPetID, DWORD dwSkillID )
{
    SetBtnData(ESBT_PetSkill, dwPetID, dwSkillID, GT_INVALID);

    m_pSkillData = PetProtoData::Inst()->FindPetSkill(dwSkillID);
    tstring path = P_VALID(m_pSkillData) ? m_pSkillData->strIconPath : _T("");

    if (GT_INVALID == dwSkillID)// 没有图标，则隐藏按钮
    {
        SetWndPic(EWPICTYPE_CURPIC, _T("data\\ui\\Bag\\L-main-icon-lock.bmp"));
        SetPicColor(0xFFFFFF);
    }
    else if(0 == dwSkillID)
    {
        SetWndPic(EWPICTYPE_CURPIC, _T("data\\ui\\Bag\\L-main-icon-lock.bmp"));
    }
    else
    {
        SetWndPic(EWPICTYPE_CURPIC, path);
    }
}
