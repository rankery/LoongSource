#pragma once
#include "DummyClient.h"

class MapView
{
public:
	MapView(void);
	virtual ~MapView(void);

	virtual SceneGraph* GetSceneGraph() =0;
	virtual void Update() =0;
	virtual Vector3 GetRolePos() =0;
	virtual void SetRolePos(const Vector3& pos) =0;
	virtual void SetRolePos(int tileX,int tileZ) =0;
	virtual bool RayCollide(const Ray& ray,Vector3& pos) =0;
	virtual POINT World2Tile(const Vector3& wpt) =0;
	virtual void ShowRole(bool bShow) = 0;
	virtual NavMap* GetNav() =0;
	virtual void SetMoveSpeed(float speed)=0;
	virtual void SetJumpYSpeed(float speed)=0;
	virtual void OpenDoor(DWORD dwObjID) =0;
	virtual void CloseDoor(DWORD dwObjID) =0;
};
