#pragma once
#include "role.h"

class FSM_NPC;
struct tagNS_GetRemoteCreatureState;
struct tagCreatureMdlData;

/** \class NPC
	\brief 非玩家角色
*/
class NPC :	public Role
{
public:
	NPC(void);
	virtual ~NPC(void);

	//--Role
	virtual void InitFSM();
	virtual void AttachSceneNodeToSG(SceneGraph* pSG);
	virtual void DetachSceneNodeFromSG(SceneGraph* pSG);
	virtual void Update();
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pGameEvent);
	virtual SceneNode* GetSceneNode() { return m_pEntityNode; }
	virtual void GetHeadPos(Vector3& out);
	virtual Vector3 GetRoleSize();
	virtual bool IsDead();
	virtual bool IsAttributeInited() const;
	virtual void Hide( const bool bHide );
	virtual bool IsHide() const { return m_bHide; }
	virtual VOID FaceTo(const Vector3& pos,BOOL bFaceRound=TRUE);
	virtual VOID FaceBack(void);
	virtual void SetTransparent(float alpha);
	virtual	AnimationCtrl* GetAniCtrl();

	//--
	/** \加载SceneNode
	*/
	void LoadEntity( const TCHAR* szFileName );
	/**	\加载蒙皮
	*/
	void LoadSkin( const TCHAR* szFileName );
	void UpdateBoundingBox(void);
	/** \生物属性初始化
	*/
	virtual void InitAttribute( tagNS_GetRemoteCreatureState* pCmd );
	/** \获得属性结构指针
	*/
	const tagCreatureProto* GetProto() const { return m_pProto; }
	/** \获得模型相关数据
	*/
	const tagCreatureMdlData* GetMdlData() const { return m_pMdlData; }
	/** \设置所属ID
	*/
	void SetTaggedOwnerID( const DWORD dwID ) { m_dwTaggedOwnerID = dwID; }
	/** \是否属于本地玩家
	*/
	bool IsBelongToLP() const;
	/** \类型判断
	*/
	bool IsNPC();
	bool IsGather();
	bool IsMonster();
	bool IsChest();
	bool IsNormalGameObject();
	bool IsCommonInvesGO();
	bool IsQuestInvesGO();
	bool IsQuestTalkGameObject();
    bool IsPet();
	/** \获得TypeID
	*/
	DWORD GetTypeID() { return m_dwTypeID; }

	//--动画播放
	void PlayTrack(const TCHAR* szTrackName,bool loop,bool replay=false,float timePos=0.0f);
	bool IsTrackEnd(const TCHAR* szTrackName);

	//--效果相关
	/** \任务相关特效
	*/
	void UpdateGameObjectQuestEffect();
protected:
	void UpdataPitch();
	virtual void OnMsgCode( const DWORD dwMsgCode );
protected:
	EntityNode*				m_pEntityNode; 
	tstring					m_curTrack;			// 当前播放的track
	FSM_NPC*				m_pFSM;

	tagCreatureProto*		m_pProto;			// 静态属性结构指针
	tagCreatureMdlData*		m_pMdlData;			// 模型相关数据

	DWORD					m_dwTypeID;
	bool					m_bGetAttribute;	// 是否已从服务器获得属性

	bool					m_bHide;			// 是否隐藏
	DWORD					m_dwQueryEffectID;	// 任务相关特效ID

	float					m_originYaw;

	float					m_lastYaw;
	float					m_lastPitch;

	DWORD                   m_dwTaggedOwnerID;  // 掉落所属ID

	DECL_RTTI(NPC);
};
