#pragma once

/** 旋转镜头游戏事件
*/
struct tagRotateCameraEvent : public tagGameEvent
{
	tagRotateCameraEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{}
};

class Role;
/**	\class ClientCamera 
	\brief 客户端镜头控制
*/
class ClientCamera : public CameraEuler
{
public:
	VOID SetDefault(FLOAT fZFar);
	VOID SetFov(float deg);

	VOID Update(Role *pLookRole,NavMap *pNavMap);
	
	VOID PlayQuake();	//! 镜头震动

	VOID EnableFlyMode(bool enable)	{m_bFly=enable;}
	bool IsFly() { return m_bFly; }

    VOID SetHeightRate(FLOAT rate) { 
        m_HeightScaleRate = rate;
    }

	ClientCamera();

private:
	TObjRef<InputDX8>			m_pInput;
	TObjRef<Console>			m_pConsole;
	TObjRef<GameFrameMgr>		m_pFrmMgr;

	POINT m_ptLast;
	
	float m_fExpectDist;		// 期望距离
	float m_fOccultDist;		// 镜头被挡住时的dist
	DWORD m_dwLastOccultTime;	// 镜头上次被挡住的时间
	

	bool m_bFly;				// 是否是飞翔模式
	bool m_bRotating;			// 正在用鼠标旋转镜头

	bool m_bQuaking;			// 是否进行镜头震动处理
	DWORD m_dwQuakeStartTime;	// 镜头震动开始时间

	BOOL m_bDecTransparent;		
	FLOAT m_fTransparent;

    FLOAT m_HeightScaleRate;    //! 镜头LookAt的高度缩放比例 add by hyu
	
	virtual void Occult(NavMap *pNavMap);
	void UpdateQuake(Vector3& nextLookAt);

	void UpdateRoleTransparent();
};





