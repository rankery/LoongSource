#pragma once
#include "NPC.h"
#include "../WorldDefine/pet_define.h"

class   FSM_PET;
struct  tagPetProto;
struct  tagNS_GetRemotePetState;

//! \class  EPetStus
//! \brief  宠物驾驭状态
enum EPetStus
{
    EPS_Followed,   //!< 跟随
    EPS_RidingMove, //!< 骑乘走
    EPS_RidingIdle, //!< 骑乘悠闲
    EPS_Flying,     //!< 飞行
    EPS_Num
};

//! \class  Pet
//! \breif  被召唤宠物
//! \author hyu
//! \date   2009-3-31
//! \base   NPC
//! \sa     NPC,Role,RoleMgr
class Pet : public NPC
{
public:
    Pet(void);
    virtual ~Pet(void);

    // 宠物
    virtual void InitFSM();
    //virtual void AttachSceneNodeToSG(SceneGraph* pSG);
    //virtual void DetachSceneNodeFromSG(SceneGraph* pSG);
    virtual void Update();
    virtual void OnNetCmd(tagNetCmd* pNetCmd);
    virtual void OnGameEvent(tagGameEvent* pGameEvent);
    //virtual SceneNode* GetSceneNode() { return m_pEntityNode; }
    //virtual void GetHeadPos(Vector3& out);
    // 返回碰撞盒大小
    virtual Vector3 GetRoleSize();
    void SetCurrScale(const Vector3& vScale) {m_vCurrScale = vScale;}
    // 设置模型缩放
    void SetScale(float scale);
    virtual bool IsDead();
    virtual bool IsAttributeInited() const;
    //virtual void Hide( const bool bHide );
    //virtual bool IsHide() const { return m_bHide; }

    //! 因宠物有多个模型，供外部重新设置节点使用
    void DeleteSceneNode(SceneGraph* pSG);

    //! 播放宠物动画,仅供角色骑乘飞行控制
    void PlayDrivingTrack(EPetStus eStus);

    //! 获得宠物类型模型
    const tagPetProto* GetProto() const { return m_pPetProto; }

    //! 获得宠物类型ID
    DWORD GetTypeID() { return m_dwTypeID; }

    //! 获得宠物所属RoleID
    DWORD GetOwnerID() { return m_dwOwnerID; }

    //! 获得宠物速度
    float GetPetSpeed();

    //! 获得当前模型类型
    EPetModelType GetCurrMdlType();

    //! 初始化
    virtual void InitPetAttribute(tagNS_GetRemotePetState* pCmd);

    //! 设置宠物类型id
    void SetPetTypeID( DWORD dwTypeID ) { m_dwTypeID = dwTypeID; }

    //! 关闭此方法
    virtual void InitAttribute( tagNS_GetRemoteCreatureState* pCmd ) {};

    //! 开关状态机
    void SetFSMEnabled(bool bVal) { m_bFSMEnabled = bVal; }
    bool IsFSMEnabled() {return m_bFSMEnabled;}

    //! 坐骑人数计数器
    void RidersCountReset() { m_nRidersCount = 0; }
    void RidersCountInc() { ++m_nRidersCount; }
    void RidersCountDec() { --m_nRidersCount; }
    int GetRidersCount() { return m_nRidersCount; }

    //! 临时打开/关闭宠物俯仰,优先级高于tagCreatureMdlData中的bCanPitch属性
    void SetPetPitch(bool bAllowPitch) { m_bPitch = bAllowPitch; }
    bool bAllowPitch() { return m_bPitch; }

protected:

    UPetState       m_uPetState;        //!< 当前宠物状态
    FSM_PET*        m_pFSM_PET;         //!< 宠物状态机
    tagPetProto*    m_pPetProto;        //!< 宠物类型数据
    DWORD			m_dwTypeID;         //!< 宠物类型id
    DWORD           m_dwOwnerID;        //!< 宠物所属角色ID

    bool            m_bInitialized;     //!< 是否初始化完属性

    bool            m_bFSMEnabled;      //!< 是否暂停状态机

    int             m_nRidersCount;     //!< 当前坐骑上的人数

    Vector3         m_vCurrScale;       //!< 坐骑额外缩放系数

    bool            m_bPitch;           //!< 是否打开宠物俯仰

    DECL_RTTI(Pet);
};
