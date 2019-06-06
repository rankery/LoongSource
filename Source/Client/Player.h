#pragma once
#include "role.h"
#include "..\WorldDefine\ItemDefine.h"
#include "AvatarEquipNode.h"
#include "AvatarAniPlayer.h"
#include "RoleTitleProtoData.h"
#include "ActionState.h"

class FSM_RP;
class Pet;
struct tagNS_GetRemoteRoleState;

/** \class Player
	\brief 玩家角色
*/
class Player :	public Role
{
public:
	Player(void);
	virtual ~Player(void);

	//--Role-------------------------------------------------------------------
	virtual void InitFSM();
	virtual void AttachSceneNodeToSG(SceneGraph* pSG);
	virtual void DetachSceneNodeFromSG(SceneGraph* pSG);
	virtual void Update();
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pGameEvent);
	virtual SceneNode* GetSceneNode() { return m_pAvatarNode; }
	virtual void GetHeadPos(Vector3& out);
	virtual Vector3 GetRoleSize();
	virtual bool IsDead();
	virtual bool IsAttributeInited() const;
	virtual void Hide( const bool bHide );
	virtual bool IsHide() const { return m_bHide; }
	virtual VOID FaceTo(const Vector3& pos,BOOL bFaceRound=TRUE);
	virtual VOID FaceBack(void);
	virtual void ReplayLowerAni();
	virtual void SetTransparent(float alpha);

	//--动作播放----------------------------------------------------------------
	AvatarAniPlayer* GetAniPlayer()	{ return &m_aniPlayer;}
	NodeAnimationCtrl* GetAniCtrl();
	void  SetMount(Pet *pPet);
	void  SetPos(const Vector3& pos);
	void  SetYaw(float angle,bool bShade=true);
	float GetMoveSpeed();
	float GetMoveYaw(){ return m_moveYaw;}
	void SetMoveYaw(float yaw){ m_moveYaw=yaw;}

	//--角色可视属性及装备----------------------------------------------------
	/** \加载默认的avatar
	*/
	void LoadDefaultAvatar();
	/** \加载指定的avatar，pEpuip=NULL则只加载att
	*/
	void LoadAvatar(const tagAvatarAtt& att, const tagAvatarEquip& ep, const tagDisplaySet& displaySet);
	/** \更新avtatar，pEpuip=NULL则只更新att
	*/
	void UpdateAvatar(const tagAvatarAtt& att, const tagAvatarEquip& ep, SceneGraph* pSG);
	/** \换单个装备
	*/
	void SetAvatarEquip( const EAvatarElement eAvaEnt, const tagEquipDisplay& ed );
	/** \获得单个装备
	*/
	const tagEquipDisplay& GetAvatarEquip( const EAvatarElement eAvaEnt ) const;
	/** \获得角色可视属性数据
	*/
	const tagAvatarAtt& GetAvatarAtt() const;
	/** \拿出/收回武器
	*/
	void HandUpWeapon( const bool bHandUp );
	/** \获取主手武器类型
	*/
	EItemTypeEx GetRWeaponType() const;
	/** \获取附手武器类型
	*/
	EItemTypeEx GetLWeaponType() const;
	/** \展开/关闭所有装备(播放变形动画)
	*/
	void OpenAllEquip( const bool bOpen );
	/** \所有装备是否已打开
	*/
	bool IsAllEquipOpened();
	/** \所有装备是否已关闭
	*/
	bool IsAllEquipClosed();
	/** \更新龙魂特效
	*/
	void UpdataLongHunEffect( const EAvatarElement eAvaEnt, const BYTE byEquipEffect );
	/** \更新套装特效
	*/
	void UpdataSuitEffect( const DWORD dwSuitID );

	//--玩家属性相关-----------------------------------------------------------
	/** \初始化玩家属性
	*/
	void InitAttribute(tagNS_GetRemoteRoleState* pCmd);
	/** \设置玩家姓名
	*/
	void SetPlayerName(LPCTSTR szName, DWORD dwNameCrc = GT_INVALID);

    //! 设置玩家称号
    void SetPlayerTitle(DWORD dwTitleID);

	// 设置角色摊位等级
	void SetStallLevel(BYTE byLevel);

    //! 设置玩家绑定的复活点
    void SetRebornPoint(DWORD dwMapID, Vector3& dwRebornPoint);

    //! 设置玩家可记录传送点
    void SetTransportPoint(DWORD dwMapID, Vector3& dwPoint);
    //! 得到当前玩家设置的可记录传送点的地图名
    tstring GetTransportPointName();
    //! 得到当前玩家设置的可记录传送点的坐标
    Vector3 GetTransportPoint();

	/** \设置玩家职业
	*/
	void SetPlayerClass(EClassType eClass);
    void SetPlayerClassEx(EClassTypeEx eClassEx);
    /** \获得玩家职业
    */
    EClassType GetPlayerClass() { return m_eClass; }
    EClassTypeEx GetPlayerClassEx() { return m_eClassEx; }

	// 获得摊位等级
	BYTE GetPlayerStallLevel() { return m_byStallLevel; }

    /** \设置玩家配偶
    */
    void SetPlayerLoverID(DWORD dwLoverID);
    void SetPlayerIsHasWedding(BOOL bWedding) { m_bHaveWedding = bWedding; }
    /** \获得玩家配偶,无则返回GT_INVALID
    */
    DWORD GetPlayerLoverID() { return m_dwLoverID; }
    BOOL IsHaveWedding() { return m_bHaveWedding; }

	/** \获取玩家性别
		\玩家性别未去到则返回0xFF表示未知
	*/
	BYTE GetPlayerSex(){if(P_VALID(m_pAvatarNode))return m_pAvatarNode->GetAtt().bySex; return 0xFF;}
	/** \设置角色状态
	*/
	void SetRoleState(DWORD dwValue);
	/** \设置角色状态
	*/
	void SetRoleState(ERoleState eType, bool bFlag);
	/** \获取角色状态
	*/
	DWORD GetRoleState(){ return m_roleState;}
	/** \获取角色是否进入某状态
	*/
	bool GetRoleState(ERoleState eType){return (m_roleState&eType) != 0;}

    //! 获取称号
    DWORD GetTitleID() { return m_dwTitleID; }
    tstring GetTitleName() {return m_strTitleName; }
    
    //! 获取当前绑定的复活点名称
    tstring GetRebornPoinName();

	/** \获取PK状态
	*/
	ERolePKState GetPKState(){ return m_ePKState;}
	/** \是否可以水上行走
	*/
	bool CanWaterWalk() { return 0!=(m_roleState&ERS_WaterWalk); }
	/** \是否在帮
	*/
	bool IsInGuild() const {return P_VALID(m_dwGuildID);}

    /** \显示设置
    */
	const tagDisplaySet& GetDisplaySet();
	void SetDisplaySet( const tagDisplaySet& ds );

	/** \设置玩家帮派ID
	*/
	void SetRoleGuildID( DWORD dwID ){m_dwGuildID = dwID;}
	/** \设置玩家帮派职位
	*/
	void SetRoleGuildPos( INT8 n8Pos ){m_n8GuildPos = n8Pos;}
	/** \获取玩家帮派ID
	*/
	DWORD GetRoleGuildID(){return m_dwGuildID;}
	/** \获取玩家帮派职位
	*/
	INT8 GetRoleGuildPos(){return m_n8GuildPos;}

	//--效果相关-----------------------------------------------------------
	/** \显示/隐藏武器
	*/
	void HideWeapon(bool bHide);

	// 显示/隐藏武器附属模型
	void HideWeaponPertain(bool bHide);

	// 播放/停止播放套装特效
	void PlaySuitEffect(bool bPlay);

	//--
	virtual bool IsReadyState();
	virtual bool IsStandState();
	virtual bool IsStyleActionState();
	virtual bool IsIdleState();
	virtual EActionState GetActionState();

	Pet*	GetMount(void)	{ return m_pMount; }
    void SwitchMountPitch(bool bPitchEnabled);

	/**	\计算骑乘时屁股距离地面的高度
	*/
	float GetBunsHeight(void);

	/** \发送显示/隐藏摊位
	*/
	void SendEventStallAppear(bool bAppear, BYTE byStallLevel = 1);

protected:
	/** \发送状态改变游戏事件
	*/
	void SendEventRoleStateChange(const DWORD dwOldState);
	/**	\计算骑乘时的世界矩阵
	*/
	bool CalRideMat(Pet *pMount,Matrix4& outRide,UINT index);
	/** \处理消息点
	*/
	virtual void OnMsgCode( const DWORD dwMsgCode );
protected:
	AvatarEquipNode*	m_pAvatarNode;		//布娃娃结点
	AvatarAniPlayer		m_aniPlayer;		//动画播放器

	float				m_moveYaw;			//角色移动方向

	DWORD				m_dwRoleNameCrc;	//玩家名称Crc值
	EClassType			m_eClass;			//专精职业
	EClassTypeEx		m_eClassEx;			//英雄职业
	DWORD				m_roleState;		//服务器传过来的玩家状态
	ERolePKState		m_ePKState;			//PK状态

	bool                m_bAttributeInited;	//是否已从服务器获得属性
	bool                m_bHide;

	Pet*				m_pMount;			//坐骑
	int					m_bunsBoneIndex;	//臀部骨骼索引

	DWORD				m_dwGuildID;		//所属帮派ID
	INT8				m_n8GuildPos;		//所属帮派职位

    DWORD               m_dwTitleID;        //当前称号id(无效为GT_INVALID)
    tstring             m_strTitleName;     //当前称号名,无效为空字符串

    DWORD               m_dwRebornMapID;    //当前绑定的复活点mapId(无效为GT_INVALID)
    Vector3             m_vRebornPoint;     //复活点坐标

    DWORD               m_dwTransportID;    // 当前记录传送符绑定地图ID
    Vector3             m_vTransportPoint;  // 当前记录传送符绑定地图坐标

    DWORD			    m_dwLoverID;        // 爱人对应的roleid 没结婚则为GT_INVALID
    BOOL			    m_bHaveWedding;     // 是否举行过婚礼
	
	BYTE				m_byStallLevel;		// 摊位等级
private:
	FSM_RP*				m_pFSM;

	float				m_lastYaw;
	float				m_lastPitch;

	DECL_RTTI(Player);
};
