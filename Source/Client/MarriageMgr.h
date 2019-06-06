#pragma once

struct tagNS_RemoteChangeSpouse;

//! \class  MarriageMgr
//! \brief  婚姻管理器
//! \author hyu
//! \date   2009-9-9
class MarriageMgr
{
public:
    MarriageMgr(void);
    ~MarriageMgr(void);

    void Init();
    void Destroy();
    void Update();

    //! Singleton
    static MarriageMgr& Inst();

    //! 婚姻相关错误码
    bool ShowErrMsg(DWORD dwErrCode);

    //! 显示结婚说明
    DWORD ShowMarriageManule(tagGameEvent *pEvent);

private:
    //! 处理网络消息
    DWORD OnNS_RemoteChangeSpouse   (tagNS_RemoteChangeSpouse*  pMsg, DWORD);

    //------------------------------------------------------------------------
    // 工具
    //------------------------------------------------------------------------
    TSFPTrunk<MarriageMgr>      m_Trunk;
    TObjRef<Util>				m_pUtil;
    TObjRef<NetCmdMgr>			m_pCmdMgr;
    TObjRef<GameFrameMgr>		m_pFrameMgr;
    TObjRef<NetSession>			m_pSession;
};
