#include "StdAfx.h"
#include "NavMovePath.h"

namespace WorldBase
{
	NavMovePath::NavMovePath(void)
	{
	}

	NavMovePath::~NavMovePath(void)
	{
	}

	void NavMovePath::AddPoint( const Vector3& pos )
	{
		float dist;
		if(m_path.empty())
		{
			dist=Vec3Dist(m_startPos,pos);
		}
		else
		{
			dist=m_path.back().dist;
			dist+=Vec3Dist(m_path.back().pos,pos);
		}

		m_path.push_back(tagMovePoint(pos,dist));
	}

	void NavMovePath::AddPoint( const Vector3& pos,float dist )
	{
		m_path.push_back(tagMovePoint(pos,dist));
	}
	
	float NavMovePath::GetDist() const
	{
		if(m_path.empty())
			return 0.0f;
		return m_path.back().dist;
	}

	const Vector3& NavMovePath::Back() const
	{
		if(m_path.empty())
			return m_startPos;
		return m_path.back().pos;
	}

	void NavMovePath::SlerpPos( float factor,Vector3& out ) const
	{
		if(m_path.empty())
		{
			out=m_startPos;
			return;
		}

		if(factor<0)
			factor=0;
		if(factor>1)
			factor=1;

		float dist=m_path.back().dist*factor;

		Vector3 prevPos=m_startPos;
		float prevDist=0;
		for(list<tagMovePoint>::const_iterator iter=m_path.begin();
			iter!=m_path.end();++iter)
		{
			if(dist<=iter->dist)
			{
				float r=(dist-prevDist)/(iter->dist-prevDist);
				if(r<0)r=0;
				if(r>1)r=1;
				LinearLerp(prevPos,iter->pos,r,out);
				return;
			}

			prevPos=iter->pos;
			prevDist=iter->dist;
		}

		out=m_path.back().pos;
	}

	
}//namespace WorldBase