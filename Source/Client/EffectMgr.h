#pragma once
#include "..\WorldDefine\msg_combat.h"
#include "CombatEvent.h"

namespace Cool3D
{
	class EffectTarget;
};
class Role;

/** \class EffectMgr
	\brief 特效管理器
*/
class EffectMgr
{
public:
	/** \特效类型
	*/
	enum EEffectType
	{
		EET_Common,         // 普通特效
		EET_Role,           // 角色特效
		EET_Launch,         // 发射特效
		EET_Buff,           // Buff特效
		EET_BeAttack,       // 被攻击特效
	};
	/** \特效的缩放类型
	*/
	enum EEffectScaleType
	{
		EEST_Null,           // 不缩放
		EEST_Y,				 // 根据高度缩放
		EEST_RoleScale,      // 使用角色的缩放来缩放特效
		EEST_XYZ,			 // 使用X,Y,Z的平均值缩放
		EEST_YOutOnly,       // 根据高度放大，不缩小
		EEST_XZ,			 // 使用X,Z的平均值缩放
	};
private:
	/** \特效基本数据
	*/
	struct tagBaseEffectData
	{
		const EEffectType       eType;                // 特效类型
		SGAttachableEffectNode*	pSGNode;              // 特效Scene node
		DWORD				    dwBindRoleID;         // 特效绑定的角色ID（如果没有绑定角色，ID==GT_INVALID）
		bool                    bBindTagNode;		  // 是否绑定了挂载点或骨骼
		EEffectScaleType		eScaleType;           // 缩放类型
		bool                    bPullOnWater;         // 如果在水下，拉到水面以上
		tagBaseEffectData( EEffectType type ) : eType( type ), pSGNode( NULL ), dwBindRoleID( GT_INVALID ), bBindTagNode( false ), eScaleType( EEST_Null ), bPullOnWater( false ) {}
	};
	typedef map<DWORD, tagBaseEffectData*> EffecMap;

	/** \角色特效
	*/
	struct tagRoleEffectData : public tagBaseEffectData
	{
		tagRoleEffectData() : tagBaseEffectData( EET_Role ) {}
	};

	/** \发射特效
	*/
	struct tagLaunchEffectData : public tagBaseEffectData
	{
		tagHitTargetEvent evtHitTarget;
		bool              bEventSended;
		tagLaunchEffectData() : tagBaseEffectData( EET_Launch ), bEventSended( false ) {}
	};

	/** \Buff特效
	*/
	struct tagBuffEffectData : public tagBaseEffectData
	{
		DWORD dwBuffID;								// BUFF ID
		tagBuffEffectData() : tagBaseEffectData( EET_Buff ), dwBuffID( GT_INVALID ) {}
	};

	/** \被攻击特效
	*/
	struct tagBeAttackEffectData : public tagBaseEffectData
	{
		tagBeAttackEffectData() : tagBaseEffectData( EET_BeAttack ) {}
	};

	enum { EFFECT_START_ID = 1 };
	enum { MIN_EFFECT_NUM = 30, MAX_EFFECT_NUM = 150 };  // 根据特效细节以及当前特效数量限制某些特效播放
public:
	EffectMgr(void);
	~EffectMgr(void);

	/** \初始化，由战斗系统调用
	*/
	VOID Init();
	/** \释放数据，由战斗系统调用
	*/
	VOID Destroy();
	/** \更新特效，由战斗系统调用
	*/
	VOID Update();

	/** \播放特效，指定位置、缩放、旋转
	*/
	DWORD PlayEffect(const TCHAR* szPath, const Vector3& vPos, const Vector3& vScale, const Vector3& vRot );
	/** \在指定位置播放一个特效
	    \szFolderName-特效所在文件夹名称，如果为NULL，则认为szEffectName就是特效的路径
	*/
	DWORD PlayEffect(const Vector3& vPos,float yaw,const TCHAR* szEffectName, const Vector3* pSize=NULL, EEffectScaleType eScaleType = EEST_Null, const TCHAR* szFolderName = _T("combat") );
	/** \播放一个角色特效,返回特效ID,播放失败返回GT_INVALID
	*/
	DWORD PlayRoleEffect(DWORD dwRoleID,const TCHAR* szEffectName,const TCHAR* szTagNodeName=NULL, EEffectScaleType eScaleType = EEST_Y, const TCHAR* szFolderName = _T("combat"), const bool bPullOnWater = false );
	/** \播放发射特效,返回特效ID
	*/
	DWORD PlayLaunchEffect(const tagHitTargetEvent& evt,const TCHAR* szEffectName,const TCHAR* szTagNodeName=NULL);
	/** \根据nNumEffect和szEffectBaseName随机播放一个被攻击特效，返回特效ID
	*/
	DWORD PlayBeAttackEffect(DWORD dwRoleID,const TCHAR* szEffectBaseName,const int nNumEffect, EEffectScaleType eScaleType = EEST_YOutOnly );
	/** \播放一个Buff特效,返回特效ID
	*/
	DWORD PlayBuffEffect(DWORD dwRoleID,DWORD dwBuffID,const TCHAR* szEffectName, EEffectScaleType eScaleType = EEST_Y );
	/** \停止一个Buff特效
	*/
	VOID StopBuffEffect(DWORD dwRoleID,DWORD dwBuffID);
	/** \停止一个特效
	*/
	VOID StopEffect( const DWORD dwEffectID );
	/** \显示\隐藏特效
	*/
	VOID ShowEffect( const DWORD dwEffectID, const bool bShow );
	/** \显示\隐藏绑定在角色身上的所有特效
	*/
	VOID ShowBindRoleEffect( const DWORD dwBindRoleID, const bool bShow );
	/** \获取容器中特效的数量
	*/
	size_t GetNumEffect() const { return m_mapEffect.size(); }
	/** \特效数量是否已经达到上限（有些不重要的特效就可以不播放）
	*/
	bool                 IsEffectNumFull();
	static EffectMgr* Inst();
private:
	SGAttachableEffectNode*		LoadSFXFile( const TCHAR* szEffectName, const TCHAR* szFolderName = _T("combat") );
	DWORD						AddEffect( tagBaseEffectData* pData, Role* pBindRole, const TCHAR* szTagNodeName );
	VOID						DeleteEffect( tagBaseEffectData* pData );    // 不会删除map中的节点
	VOID						OnCloseMap( tagGameEvent* );
	BOOL                        UpdataRoleEffect( tagBaseEffectData* pData );
	VOID                        UpdataTargetPath( tagLaunchEffectData* pData, const float fTimeDelta );
	VOID                        PullOnWater( tagBaseEffectData* pData );
	Vector3						GetRoleEffectScale( const EEffectScaleType eScaleType, Role* pRole );
	float 						GetEffectScale( const EEffectScaleType eScaleType, const Vector3& vSize );
	Vector3						GetBeAttackPos( Role* pRole );
	bool                        SFXCanMove( const TCHAR* szFileName );
	VOID                        UpdataEffectMatrix( tagBaseEffectData* pData, Role* pRole );
private:
	TObjRef<GameFrameMgr>		m_pGameFrameMgr;
	TSFPTrunk<EffectMgr>		m_Trunk;
	EffecMap					m_mapEffect;				// 特效容器
	DWORD						m_dwCurEffectID;            // 记录当前特效ID
};
