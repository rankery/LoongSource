#pragma once
#include "npc.h"

/** \class Door
	\brief 门
*/
class Door : public NPC
{
public:
	Door(void);
	virtual ~Door(void);

	//--Role
	virtual void InitFSM();
	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pGameEvent);
	virtual bool IsDead();

	//--NPC
	virtual void InitAttribute( tagNS_GetRemoteCreatureState* pCmd );

	//--
	
	bool GetDoorState() { return m_bState; }
protected:
	void OpenDoor(bool bPlayAction);
	void CloseDoor(bool bPlayAction);

protected:
	DWORD m_dwMapObjID;	//记录地图对象ID
	bool m_bState;		//开关

	DECL_RTTI(Door);
};
