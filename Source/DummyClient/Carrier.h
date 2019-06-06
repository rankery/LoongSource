#pragma once
#include "DummyClient.h"

class MapView;
class Carrier
{
	enum EState
	{
		EState_None,
		EState_Moving,
		EState_Stop,
	};
public:
	Carrier(void);
	virtual ~Carrier(void);

	bool Init(const tagMapCarrier& carrier,const vector<tagMapWayPoint>& wayPoints,NavMap *pNav);
	void Update(MapView *pMap,NavMap *pNav);

	void AttachToSceneGraph(SceneGraph *pSG);

private:
	EntityNode*			m_pSGNode;
	ObjectSpace			m_space;
	NavCarrierNode*		m_pNavNode;
	vector<Vector3>		m_wayPoints;
	float				m_stopTime;

	EState				m_state;
	int					m_curWayPoint;
	Vector3				m_startPos;
	Vector3				m_endPos;
	float				m_totalMoveTime;
	float				m_startMoveTime;
	bool				m_bForward;

	float				m_startStopTime;
};
