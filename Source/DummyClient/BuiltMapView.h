#pragma once
#include "mapview.h"
#include "Role.h"
#include "Carrier.h"

using namespace std;

//Build完成后的Map,完全按照游戏的命名规则
class BuiltMapView :
	public MapView
{
public:
	static float ViewDistSN;
	static float ViewDistMN;
	static float ViewDistBN;
	static float ViewDistDN;
	static float ViewDistST;
	static bool	NoMonster;//把怪物隐藏掉
public:
	BuiltMapView(void);
	virtual ~BuiltMapView(void);

	virtual SceneGraph* GetSceneGraph()		{	return m_pSG;}
	virtual void Update();
	virtual Vector3 GetRolePos();
	virtual void SetRolePos(const Vector3& pos);
	virtual void SetRolePos(int tileX,int tileZ);
	virtual POINT World2Tile(const Vector3& wpt);
	virtual NavMap* GetNav()				{   return &m_nav;}
	Vector3 Tile2World(int tileX,int tileZ);

	virtual void ShowRole(bool bShow)
	{
		if(m_pSG)
		{
			if(bShow)
				m_pSG->AttachDynamicNode(m_role.GetSGNode());
			else
				m_pSG->DetachDynamicNode(m_role.GetSGNode());
		}
	}

	int LoadMap(const TCHAR* szMapName,CameraBase *pCamera);
	bool RayCollide(const Ray& ray,Vector3& pos);

	virtual void SetMoveSpeed(float speed) { m_role.SetMoveSpeed(speed); }
	virtual  void SetJumpYSpeed(float speed) { m_role.SetJumpYSpeed(speed); }

	virtual void OpenDoor(DWORD dwObjID);
	virtual void CloseDoor(DWORD dwObjID);
private:
	void UpdateMapArea();
	void OnLeaveMapArea( const int nIndex );
	void OnEnterMapArea( const int nIndex );
private:
	HugeQuadTreeSG*		m_pSG;
	NavMap				m_nav;
	Role				m_role;

	int					m_numNPC;
	bool				m_bLoading;
	GameMap				m_gameMap;
	vector<HRGN>		m_mapAreaRgn;			//地图区域
	int					m_curMapAreaIndex;		//当前玩家所在地图区域索引
	vector<Carrier*>	m_carriers;				//载具
};
