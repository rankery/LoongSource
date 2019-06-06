#pragma once
#include "..\Cool3D\Util\RTTIObj.h"
#include "RoleRotater.h"
#include "UpperRotater.h"
#include "HeadRotater.h"
#include "RoleScaler.h"
#include "..\WorldDefine\RoleDefine.h"
#include "..\WorldDefine\buff_define.h"
#include "..\WorldDefine\dword_flag.h"

struct tagRoleBuff
{
	DWORD				dwBuffTypeID;				// Buff属性ID
	INT					nPersistTimeLeft;			// Buff消失时间倒计时（毫秒）
	INT					nMaxPersistTime;			// Buff总持续时间（毫秒）
	DWORD				dwStartTime;				// 倒计时开始时间（tagDWORDTime） 
	INT					nWarpTimes;					// 当前叠加次数
	tagBuffProtoClient*	pBuffProto;					// 指向Buff模板指针
};

class SGDynamicProjector;

/** \class Role
	\brief 游戏中各种角色的基类
*/
class Role : public RTTIObj
{
public:
	Role(void);
	virtual ~Role(void);

	//--基础功能
	//初始化状态机
	virtual void InitFSM() =0;
	//将SceneNode添加到SceneGraph，由RoleMgr调用
	virtual void AttachSceneNodeToSG(SceneGraph* pSG) =0;
	//将SceneNode从SceneGraph中移除，由RoleMgr调用
	virtual void DetachSceneNodeFromSG(SceneGraph* pSG) =0;
	//更新角色行为
	virtual void Update() =0;
	//处理网络消息
	virtual void OnNetCmd(tagNetCmd* pNetCmd){}
	//处理游戏事件
	virtual void OnGameEvent(tagGameEvent* pGameEvent){}
	//得到SceneNode
	virtual SceneNode* GetSceneNode() =0;
	//阴影
	virtual void AttachShadow( SGDynamicProjector* pShadow );
	virtual void DetachShadow();
	//隐藏/显示
	virtual void Hide( const bool bHide ) =0;
	virtual bool IsHide() const =0;
	//是否需要释放
	virtual bool IsNeedToFree() const { return m_bNeedToFree; }
	virtual void Free() { m_bNeedToFree=true; }
	
	//--属性相关
	//设置角色ID
	void SetID(DWORD id)	{ m_id=id;		}
	//返回角色ID
	DWORD GetID()			{ return m_id;	}
	//设置角色坐标
	void SetPos(const Vector3& pos);
	//返回角色坐标
	const Vector3& GetPos();
	//设置角色偏转角
	void SetYaw(float angle,bool bShade=true);
	void SetPitch(float angle,bool bShade=true);
	//返回角色偏转角
	float GetYaw() const;
	//返回最终偏转角
	float GetDestYaw();
	//设置角色缩放
	void SetScale(const Vector3& scale,bool bShade=true);
	//返回角色最终缩放
	const Vector3& GetDesScale() const;
	//返回角色俯仰角
	float GetPitch() const;
	//得到角色尺寸
	virtual Vector3 GetRoleSize() =0;
	//得到移动速度
	float GetMoveSpeed(BOOL bIsMount=FALSE);
	//得到掉落平移速度
	float GetDropXzSpeed();
	//得到跳跃上升初始速度
	float GetJumpYSpeed();
	//得到击飞平移速度
	float GetHitFlyXzSpeed();
	//得到击飞上升初始速度
	float GetHitFlyYSpeed();
	//得到游泳速度
	float GetSwimSpeed();
	//得到头顶的位置
	virtual void GetHeadPos(Vector3& out)=0;
	//得到对象空间
	const ObjectSpace& GetObjectSpace() const { return m_space; }

	//读取角色属性
	INT GetAttribute(ERoleAttribute eType){return m_roledata[eType];}
	//写入角色属性
	void SetAttribute(ERoleAttribute eType, INT nValue, bool bInit);
	//写入远程角色属性
	void SetRemoteRoleAtt(ERemoteRoleAtt eType, INT nValue, bool bInit);
	//属性是否已经初始化
	virtual bool IsAttributeInited() const =0;

	//读取角色等级
	INT GetRoleLevel(){return m_nRoleLevel;}
	//写入角色等级
	void SetRoleLevel(INT nLevel, BOOL bLevelUp = FALSE);
	
	//读取角色名称
	tstring GetRoleName(){return m_strRoleName;}
	//写入角色名称
	void SetRoleName(tstring strName);
	
	//获得角色状态
	const tagDWORDFlagArray<ES_End>& GetState(){ return m_state;}
	//设置状态
	void SetState(EState state);
	//清除状态
	void UnSetState(EState state);
	//判断状态
	bool IsInState(EState eState){ return m_state.IsSet(eState) != FALSE;}
	//返回角色是否死亡
	virtual bool IsDead() =0;
	
	//--战斗相关
	//增加buff
	VOID AddRoleBuff(tagBuffMsgInfo data);
	//删除buff
	VOID RemoveRoleBuff(DWORD dwBuffID);
	//更新buff
	VOID UpdateRoleBuff(tagBuffMsgInfo data);
	//清空buff
	VOID FreeRoleBuff();
	//发送buff改变的游戏事件
	VOID SendBuffChangeEvent();
	//根据大ID查找BUFF
	tagRoleBuff* FindBuff(DWORD dwBuffID);
	//获取bufflist
	TList<tagRoleBuff*>& GetBuffList(){return m_listRoleBuff;}

	//--动作相关
	//面向某个位置
	const vector<DWORD>& GetMsgCodes() const { return m_vecMsgCode; }
	virtual VOID FaceTo(const Vector3& pos,BOOL bFaceRound=TRUE) =0;
	virtual VOID FaceBack(void) =0;
	BOOL GetBoneSpace( const TCHAR* szBoneName, Matrix4& matOut );

	//--效果相关
	//透明效果
	virtual void SetTransparent(float alpha) =0;
	virtual float GetTransparent() const { return m_fTransparent; }
	//颜色效果
	virtual void SetMulColor(const float r, const float g, const float b);
protected:
	/** \处理消息点
	*/
    void HandleMsgCode(AnimationCtrl* pAniCtrl);
	virtual void OnMsgCode( const DWORD dwMsgCode ) =0;
protected:
	SGDynamicProjector*		m_pShadow;				// 记录一个阴影指针,以便Detach
	ObjectSpace				m_space;				// 角色空间定义

	RoleRotater				m_rotaterYaw;			// 旋转控制器
	RoleRotater				m_rotaterPitch;			// 旋转控制器

	RoleScaler				m_scaler;				// 缩放控制器
	vector<DWORD>			m_vecMsgCode;			// 动作消息代码
	UpperRotater			m_upperRotater;
	HeadRotater				m_headRotater;
	DWORD					m_id;					// 角色ID
	
	
	tstring					m_strRoleName;			// 角色名称
	INT						m_nRoleLevel;			// 角色等级
	INT						m_roledata[ERA_End];	// 角色信息

	tagDWORDFlagArray<ES_End>	m_state;			// 服务器传过来的角色状态

	TList<tagRoleBuff*>		m_listRoleBuff;			// 角色buff列表

	bool					m_bNeedToFree;			// 是否需要释放
	float					m_fTransparent;			// 当前透明度
	Vector3					m_vMulColor;            // 相乘的颜色
	DECL_RTTI(Role);
};
