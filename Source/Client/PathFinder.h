#pragma once

class LocalPlayer;
/** \class PathFinder
	\brief 寻路器
*/
class PathFinder
{
	enum EStep//寻路阶段
	{
		EStep_EnterRoad,	//非道路区域寻路，尝试进入道路区域
		EStep_OnRoad,		//道路区域寻路
		EStep_LeaveRoad,	//道路区域寻路完成，在非道路区域寻路
	};
public:
	PathFinder(void);
	virtual ~PathFinder(void);

	bool Init(const Vector3& startPos,const Vector3& endPos,NavMap* pNav,LocalPlayer* pRole,bool bOnLand);
	bool GetPathPoint(NavMap* pNav,LocalPlayer* pRole,Vector3& out);

private:
	bool PreparePath_EnterRoad(NavMap* pNav,LocalPlayer* pRole);
	bool PreparePath_OnRoad(NavMap* pNav,LocalPlayer* pRole);
	bool PreparePath_LeaveRoad(NavMap* pNav,LocalPlayer* pRole);

protected:
	bool					m_bOnLand;		//是否在陆地
	list<Point>				m_patchPath;	//基于地块寻路返回的路径
	list<Vector3>			m_tilePath;		//基于格子寻路返回的路径
	Vector3					m_endPos;		//保存寻路终点
	EStep					m_step;			//寻路阶段
	PlayerNavMap::EResult	m_resultByPatch;//保存地块寻路返回值
};
