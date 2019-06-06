#pragma once
#include "gameframe.h"
#include "ProfessionFrame.h"

const int MAX_CLASS_BUTTON_COUNT = 8;

enum ESetClassType
{
    ESClassType_NEW,    //!< 新就职
    ESClassType_Change  //!< 变更
};

//! \class  ProfessionFrame
//! \brief  转职管理界面
//! \author hyu
//! \base   GameFrame
//! \sa     ProfessionMgr
class ProfessionFrame :
    public GameFrame
{
public:
    ProfessionFrame(void);
    ~ProfessionFrame(void);

    virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
    virtual BOOL Destroy();
    virtual BOOL ReloadUI();
    virtual VOID Update();
    virtual VOID Render3D();
    virtual BOOL EscCancel();

    //! 打开界面
    VOID ShowWnd(BOOL bVal);

    // 设置当前操作类型
    void SetOperaType(ESetClassType eType) { m_eType = eType; }

    // 设置当前npcId
    void SetNPCID(DWORD dwNpcId) { m_dwNpcId = dwNpcId; }

private:
    //! 处理GUI消息
    DWORD EventHandler(tagGUIEvent* pEvent);

    //! 处理对话框返回消息
    DWORD EventSetClass(tagMsgBoxEvent* pEvent);
    DWORD EventChangeClass(tagMsgBoxEvent* pEvent);


    // 控件
    GUIWnd*						m_pWnd;
    GUIButton*                  m_pBtClose;     //!< 关闭
    GUIButton*                  m_pBtChange;    //!< 转职
    GUIButton*                  m_pBtCancel;    //!< 取消
    GUIPushButton*              m_pPbClass[MAX_CLASS_BUTTON_COUNT]; //!< 职业选择按钮

    // 工具
    TSFPTrunk<ProfessionFrame>  m_Trunk;
    TObjRef<GUISystem>			m_pGUI;
    TObjRef<Util>				m_pUtil;
    TObjRef<NetCmdMgr>			m_pCmdMgr;
    TObjRef<GameFrameMgr>		m_pFrameMgr;
    TObjRef<NetSession>			m_pSession;

    // 当前操作类型
    ESetClassType               m_eType;

    // 当前npcId
    DWORD                       m_dwNpcId;

    // 当前按下的职业
    EClassType                  m_eCurrClassType;
};
