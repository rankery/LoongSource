#pragma once

#include "..\WorldDefine\RoleDefine.h"
#include "CombatSysUtil.h"
#include "Render2D.h"
class Player;

//一个简单的摄像机控制器
class SimpleMapCamera : public CameraEuler
{
public:
	SimpleMapCamera(void);

	void Update(const Vector3& nextLookAt);

private:
	mutable Vector3 m_targetLookAt;
};

#define MAX_ROLE 5
/*
	class: SeleRoleMgr
	brief: 管理选择界面的角色信息与node，选择界面与创建界面的场景与摄像机都为同一个。
*/
class SeleRoleMgr
{
public:
	struct tagSeleRole
	{
		tagSimRole	simRole;
		Player*		pPlayer;
	};

	struct tagSeleRoleHead
	{
		tstring		strName;			//角色姓名
		Vector3		scrPos;				//角色头顶屏幕坐标位置
		Vector3		worldPos;			//角色头顶的世界坐标
		float		scale;

		tagSeleRoleHead()
		{
		}

		tagSeleRoleHead(const tagSeleRoleHead& other)
		{
			Copy(other);
		}

		const tagSeleRoleHead& operator = (const tagSeleRoleHead& other)
		{
			if(this!=&other)
				Copy(other);
			return *this;
		}

	private:
		void Copy(const tagSeleRoleHead& other)
		{
			strName		= other.strName;
		}

	};

	enum 
	{
		ESelect_Role = 0,
		ECreate_Role = 1,
	};
public:
	SeleRoleMgr(void);
	~SeleRoleMgr(void);
	static SeleRoleMgr* Inst();

	// 初始化空场景
	VOID Init();

	// 根据不同的场景设置摄像机的位置
	VOID SetCamera(const Vector3& from, const Vector3& lookat);
	VOID SetCamera(const Vector3& lookat, float fYaw, float fPitch, float fRoll, float fDist);

	void OpenMap(const TCHAR* szMapName, CameraBase* pBindCamera, DWORD dwType=0);
	void CloseMap();

	// 得到摄像机
	SimpleMapCamera* GetCamera() { return &m_Camera; }

	// 得到场景
	NullSceneGraph* GetNullSG() { return m_pNullSG; }
	HugeQuadTreeSG* GetHugeSG() { return m_pSG; }

	//	更新场景
	VOID UpdateSG(float deltaTime);

	// 删除全部角色信息及模型
	VOID Destroy(SceneGraph* pSG);
	VOID Destroy();

	// 创建一个角色时，不立即加入到场景中
	VOID CreateRole(tagSimRole tagRole, Player* pNode/*, SceneGraph* pSG*/);

	// 添加到场景中
	VOID AttachSceneNodeToSG(SceneGraph* pSG);

	// 从场景中移除
	VOID DetachSceneNodeFromSG(SceneGraph* pSG);

	// 删除一个指定的角色
	VOID DeleteRole(DWORD dwRoleID, SceneGraph* pSG);

	// 获得角色的个数
	int GetRoleNum()const { return (int)m_vecSeleRole.size(); }

	// 获得指定角色的基础属性
	const tagSimRole& GetRoleInfo(int i)const;

	// 获得指定角色的ID
	DWORD GetRoleId(int i)const;

	// 
	bool IsReadyTrackEnd(int i)const;

	void PlayReadyTrack(int i)const;

	void PlayStandTrack(int i)const;

	void PlayStandUpTrack(int nCur, int nLast)const;

	void IsStandUpTrack(int nCur)const;
	void IsSitDownTrack(int nLast)const;

	void PlayIdleTrack()const;

	void RefreshTrack();

	const TCHAR* GetHandUpName(EItemTypeEx eWeapon)const;

	// 判断通过鼠标选择的角色
	int GetMouseSelectRole(int mouseX,int mouseY);

	// 
	VOID Update();

	void SetSafeCodeCrc(DWORD dwCrc) { m_dwSafeCodeCrc = dwCrc; }
	void SetDWORDTime(tagDWORDTime& time) { m_tagTime = time; }
	void SetInitSafeCode(BOOL bVal) { m_bInitSafeCode = bVal; }
	DWORD GetSafeCodeCrc() { return m_dwSafeCodeCrc; }
	tagDWORDTime& GetDWORDTime() { return m_tagTime; }
	BOOL GetInitSafeCode() { return m_bInitSafeCode; }

	void SetLastLoginInfo(tagDWORDTime time, DWORD dwIPLast, DWORD dwIP) { m_nTimeLastLogin=time; m_nIPLast=dwIPLast; m_nIP = dwIP; }
	tagDWORDTime& GetTimeLastLogin() { return m_nTimeLastLogin; }
	DWORD GetIPLastLogin() { return m_nIPLast; }
	DWORD GetIPCurrent() { return m_nIP; }

	void DrawSelectedRoleFlag(int nRole);

private:
	TSFPTrunk<SeleRoleMgr>	m_Trunk;
	vector<tagSeleRole>		m_vecSeleRole;		//保存玩家的角色信息
	NullSceneGraph*			m_pNullSG;				//场景
	HugeQuadTreeSG*			m_pSG;
	SimpleMapCamera			m_Camera;			//摄像机
	EntityNode*				m_EntityNode;		//地物
	GameMap					m_gameMap;
	TObjRef<GUIRender>		m_pRender;

	DWORD					m_dwSafeCodeCrc;
	tagDWORDTime			m_tagTime;
	BOOL					m_bInitSafeCode;
	INT						m_nWaterReflect;
	INT						m_nWaterRefract;
	tagDWORDTime			m_nTimeLastLogin;
	DWORD					m_nIPLast;
	DWORD					m_nIP;
	tagSeleRoleHead			m_tagHead;
	IDepthFont*				m_pFont;
	DWORD					m_dwType;


private:
	//重新计算人物的位置
	VOID ResetPostion();
};
