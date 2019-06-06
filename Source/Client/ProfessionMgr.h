#pragma once
struct tagNS_SetClass;
struct tagNS_ChangeClass;
struct tagNS_RoleClassChangeBroadcast;

//! \class  ProfessionMgr
//! \brief  职业管理器
//! \author hyu
//! \date   2009-8-28
class ProfessionMgr
{
public:
    ProfessionMgr(void);
    ~ProfessionMgr(void);

    void Init();
    void Destroy();
    void Update();

    //! Singleton
    static ProfessionMgr& Inst();

    //! 得到专精职业名称
    LPCTSTR GetClassName(EClassType eClass);

    //! 得到英雄职业名称
    LPCTSTR GetClassExName(EClassTypeEx eClassEx);

    //! 打开"我想进行职业专精"界面
    DWORD OnSetClass(tagGameEvent* pEvent);

    //! 打开"我想更换专精职业"界面
    DWORD OnChangeClass(tagGameEvent* pEvent);

    //! 打开"我想更换英雄职业"界面
    DWORD OnChangeClassEx(tagGameEvent* pEvent);

    //! 向服务器请求职业专精
    void BeginNC_SetClass(DWORD dwNpcId, EClassType classType);
    DWORD OnNS_SetClass(tagNS_SetClass* pMsg, DWORD);

    //! 向服务器更换专精职业
    void BeginNC_ChangeClass(DWORD dwNpcId, EClassType classType);
    DWORD OnNS_ChangeClass(tagNS_ChangeClass* pMsg, DWORD);

    //! 远程玩家专精职业发生改变，给周围玩家广播
    DWORD OnNS_RoleClassChangeBroadcast(tagNS_RoleClassChangeBroadcast* pMsg, DWORD);

    void ShowErrorMsg( DWORD dwErrCode );

private:
    //------------------------------------------------------------------------
    // 工具
    //------------------------------------------------------------------------
    TSFPTrunk<ProfessionMgr>    m_Trunk;
    TObjRef<Util>				m_pUtil;
    TObjRef<NetCmdMgr>			m_pCmdMgr;
    TObjRef<GameFrameMgr>		m_pFrameMgr;
    TObjRef<NetSession>			m_pSession;
};
