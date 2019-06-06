#include "StdAfx.h"
#include "PathFinder.h"
#include "LocalPlayer.h"

PathFinder::PathFinder(void)
{}

PathFinder::~PathFinder(void)
{}

bool PathFinder::Init(const Vector3& startPos,const Vector3& endPos,NavMap* pNav,LocalPlayer* pRole,bool bOnLand)
{
	//--检查当前位置是否可以寻路
	if(!pRole->IsSwimState()
		&&!pRole->IsOnWaterState()
		&&!pNav->GetCollider()->IfCanFindPath(pRole->GetPos(),pRole->GetRoleSize()))
	{
		return false;
	}

	//--保存终点
	m_endPos=endPos;

	m_bOnLand=bOnLand;

	m_patchPath.clear();
	m_tilePath.clear();

	//--基于地块寻路
	bool bOnRoad=pNav->GetPlayerNavMap()->IfCanGo(startPos.x,startPos.z,true);
	if(bOnRoad)//道路区域
	{
		m_step=EStep_OnRoad;

		m_resultByPatch=pNav->GetPlayerNavMap()->FindPathByPatch(startPos,endPos,true,m_patchPath);
		if(m_resultByPatch==PlayerNavMap::ER_Failed)
			return false;
		if(m_patchPath.empty())
			return false;

	}
	else//自动区域
	{
		m_step=EStep_EnterRoad;

		m_resultByPatch=pNav->GetPlayerNavMap()->FindPathByPatch(startPos,endPos,false,m_patchPath);
		if(m_resultByPatch==PlayerNavMap::ER_Failed)
			return false;
		if(m_patchPath.empty())
			return false;
	}
	
	return true;
}

bool PathFinder::GetPathPoint( NavMap* pNav,LocalPlayer* pRole,Vector3& out )
{
	if(m_tilePath.empty())//格子路径走完了
	{
		switch(m_step)
		{
		case EStep_EnterRoad:
			{
				if(!PreparePath_EnterRoad(pNav,pRole))
					return false;
			}
			break;
		case EStep_OnRoad:
			{
				if(!PreparePath_OnRoad(pNav,pRole))
					return false;
			}
			break;
		case EStep_LeaveRoad:
			{
				if(!PreparePath_LeaveRoad(pNav,pRole))
					return false;
			}
			break;
		}
	}

	out=m_tilePath.front();
	m_tilePath.pop_front();
	return true;
}

bool PathFinder::PreparePath_EnterRoad(NavMap* pNav,LocalPlayer* pRole)
{
	PlayerNavMap::EResult result;

	if(m_patchPath.size()==0)//size==0
	{
		return false;
	}
	else if(m_patchPath.size()==1)//size==1
	{
		Point tile=m_patchPath.front();
		m_patchPath.pop_front();

		result=pNav->GetPlayerNavMap()->FindPathByTile(pRole->GetPos(),tile,pRole->GetRoleSize(),false,true,false,m_bOnLand,pNav->GetCollider(),m_tilePath);
	}
	else//size>1
	{
		m_patchPath.pop_front();
		Point tile=m_patchPath.front();

		result=pNav->GetPlayerNavMap()->FindPathByTile(pRole->GetPos(),tile,pRole->GetRoleSize(),false,true,true,m_bOnLand,pNav->GetCollider(),m_tilePath);
	}

	switch(result)
	{
	//--到达终点--------------------------------
	case PlayerNavMap::ER_Arrive:		
		{
			if(m_tilePath.empty())
				return false;
		}
		break;
	//--进入道路区域----------------------------
	case PlayerNavMap::ER_EnterRoad:	
		{
			if(m_tilePath.empty())
				return false;

			//--在道路区域按地块寻路
			m_resultByPatch=pNav->GetPlayerNavMap()->FindPathByPatch(m_tilePath.back(),m_endPos,true,m_patchPath);
			if(m_resultByPatch==PlayerNavMap::ER_Failed)
				return false;
			if(m_patchPath.empty())
				return false;

			m_step=EStep_OnRoad;
		}
		break;
	//--失败-------------------------------------
	case PlayerNavMap::ER_Failed:		
	//--到达最近点-------------------------------
	case PlayerNavMap::ER_GetNearPos:
	//--其它-------------------------------------
	default:
		return false;
	}

	return true;
}

bool PathFinder::PreparePath_OnRoad(NavMap* pNav,LocalPlayer* pRole)
{
	PlayerNavMap::EResult result;

	if(m_patchPath.size()==0)//size==0
	{
		if(m_resultByPatch==PlayerNavMap::ER_Arrive)//已到达终点
		{
			return false;
		}
		else//未到达终点
		{
			m_step=EStep_LeaveRoad;

			m_resultByPatch=pNav->GetPlayerNavMap()->FindPathByPatch(pRole->GetPos(),m_endPos,false,m_patchPath);
			if(m_resultByPatch==PlayerNavMap::ER_Failed)
				return false;
			if(m_patchPath.empty())
				return false;

			return PreparePath_LeaveRoad(pNav,pRole);
		}
	}
	else if(m_patchPath.size()==1)//size==1
	{
		Point tile=m_patchPath.front();
		m_patchPath.pop_front();

		result=pNav->GetPlayerNavMap()->FindPathByTile(pRole->GetPos(),tile,pRole->GetRoleSize(),true,false,false,m_bOnLand,pNav->GetCollider(),m_tilePath);
	}
	else//size>1
	{
		m_patchPath.pop_front();
		Point tile=m_patchPath.front();

		result=pNav->GetPlayerNavMap()->FindPathByTile(pRole->GetPos(),tile,pRole->GetRoleSize(),true,false,true,m_bOnLand,pNav->GetCollider(),m_tilePath);
	}

	switch(result)
	{
	case PlayerNavMap::ER_Arrive://到达终点	
		{
			if(m_tilePath.empty())
				return false;
		}
		break;
	case PlayerNavMap::ER_Failed://失败	
	case PlayerNavMap::ER_GetNearPos://到达最近点
	default://其它
		return false;
	}

	return true;
}

bool PathFinder::PreparePath_LeaveRoad(NavMap* pNav,LocalPlayer* pRole)
{
	PlayerNavMap::EResult result;

	if(m_patchPath.size()==0)//size==0
	{
		return false;
	}
	else if(m_patchPath.size()==1)//size==1
	{
		Point tile=m_patchPath.front();
		m_patchPath.pop_front();

		result=pNav->GetPlayerNavMap()->FindPathByTile(pRole->GetPos(),tile,pRole->GetRoleSize(),false,false,false,m_bOnLand,pNav->GetCollider(),m_tilePath);
	}
	else//size>1
	{
		m_patchPath.pop_front();
		Point tile=m_patchPath.front();

		result=pNav->GetPlayerNavMap()->FindPathByTile(pRole->GetPos(),tile,pRole->GetRoleSize(),false,false,true,m_bOnLand,pNav->GetCollider(),m_tilePath);
	}

	switch(result)
	{
	case PlayerNavMap::ER_Arrive://到达终点	
		{
			if(m_tilePath.empty())
				return false;
		}
		break;
	case PlayerNavMap::ER_Failed://失败	
	case PlayerNavMap::ER_GetNearPos://到达最近点
	default://其它
		return false;
	}

	return true;
}