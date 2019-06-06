#pragma once
#include "gameframe.h"

struct tagGuildWarePriChange;
struct tagRoleGetNameEvent;
struct tagNS_GetGuildWarePriList;

//! \class  GuildStorePermitFrame
//! \brief  帮派仓库权限设置界面
//! \author hyu
//! \date   2009-8-3
//! \last   2009-8-3
class GuildStorePermitFrame :
    public GameFrame
{
public:
    GuildStorePermitFrame(void);
    ~GuildStorePermitFrame(void);

    virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
    virtual BOOL Destroy();
    virtual BOOL ReloadUI();
    virtual VOID Update();
    virtual BOOL EscCancel();

    //! 打开界面
    VOID ShowWnd(BOOL bVal);

    void SetNpcId(DWORD id) {m_dwNpcId = id;}
private:
    //! 清除ui
    void ClearUI();

    //! 刷新
    void Refresh();

    //! 填充列表
    void RefreshToList();

    //! 点击列表
    void ClickList(int rowIndex);

    //! 取得角色名
    tstring GetRoleName(DWORD dwRoleID);

    //! 处理GUI消息
    DWORD EventHandler(tagGUIEvent* pEvent);

    //! 设置权限
    void SetNetPrivacy(bool bVal, DWORD dwRoleId);

    //! 有玩家权限改变时
    DWORD OnChangePrivacy(tagGuildWarePriChange* pGameEvent);

    //! 取角色名
    DWORD OnGetRoleName(tagRoleGetNameEvent* pGameEvent);

    //! 获取全部帮派成员权限列表
    void BeginNC_GetGuildWarePriList();
    DWORD OnNS_GetGuildWarePriList(tagNS_GetGuildWarePriList* pMsg, DWORD);

    //! 得到权限文本
    LPCTSTR GetPrivacyText(BOOL bVal) {return bVal ? g_StrTable[_T("GuildPrivacy_yes")] : g_StrTable[_T("GuildPrivacy_no")];}
    DWORD GetPrivacyColor(BOOL bVal) {return bVal ? 0xFF00FF00 : 0xFFFF0000;}

    // 注意，以下查找函数的返回值为是否查到有此对象，结果保存在引用参数中
    bool FindPrivacyByRoleID(DWORD dwRoleID, BOOL &ret);
    bool FindRoleIDByRow(int rowIndex, DWORD &ret);
    bool FindRowByRoleID(DWORD dwRoleID, int &ret);
    bool FindPrivacyByRow(int rowIndex, DWORD &dwRoleID, BOOL &ret);
    bool SetPrivacy(DWORD roleId, BOOL bVal);

    map<DWORD, BOOL>                    m_mapRole2Permit;
    map<int, DWORD>                     m_mapRow2Role;
    map<DWORD, int>                     m_mapRole2Row;

    DWORD                               m_dwNpcId;

    TSFPTrunk<GuildStorePermitFrame>    m_Trunk;
    TObjRef<GUISystem>			        m_pGUI;
    TObjRef<Util>				        m_pUtil;
    TObjRef<NetCmdMgr>			        m_pCmdMgr;
    TObjRef<GameFrameMgr>		        m_pFrameMgr;
    TObjRef<NetSession>			        m_pSession;

private:
    //GUI控件
    GUIWnd*         m_pWnd;
    GUIButton*      m_pBtClose;     //!< 关闭窗体
    GUIButton*      m_pBtActive;    //!< 激活权限
    GUIButton*      m_pBtDiscard;   //!< 撤销权限
    GUIListBox*     m_pLBList;      //!< 权限列表

    DWORD           m_updateTimeDistance;
};
