#pragma once
#include "DummyClient.h"

class MapView;
class FSM;
class Role
{
public:
	Role(void);
	virtual ~Role(void);

	void Update(MapView *pMap,NavMap *pNav);
	void LoadDefaultAvatar(BYTE bySex=0);

	SceneNode *GetSGNode()	{	return m_pSGNode;}
	void AttachToSceneGraph(SceneGraph *pSG);
	Vector3 GetPos()		{	return m_space.GetPos();}
	void SetPos(Vector3 pos);
	void SetYaw(float yaw)	{  m_space.SetYaw(yaw); }

	float GetMoveSpeed();
	float GetDropXzSpeed();
	float GetJumpXzSpeed();
	float GetJumpYSpeed();
	float GetSlideSpeed();

	void SetMoveSpeed(float speed) { m_fMoveSpeed = speed; }
	void SetJumpYSpeed(float speed) { m_fJumpYSpeed = speed; }

	const Vector3& GetRoleSize();

	void PlayTrack(const TCHAR* szTrackName,bool loop);

private:
	void AddSkin(const TCHAR *szMdlName);
	void AddCloth(const TCHAR *szMdlName);

private:
	AvatarNode		*m_pSGNode;
	ObjectSpace		m_space;
	FSM				*m_pFSM;//×´Ì¬»ú
	SGEffectNode	*m_pSelEffect;

	FLOAT			m_fMoveSpeed;
	FLOAT			m_fJumpYSpeed;
};
