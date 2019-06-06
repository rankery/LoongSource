#include "StdAfx.h"
#include "NavCarrierNode.h"
#include "NavResSceneMesh.h"

namespace WorldBase
{
	IMP_RTTI_DYNC(NavCarrierNode,NavEntityNode);

	NavCarrierNode::NavCarrierNode(void):m_bRunning(false)
	{
	}

	NavCarrierNode::~NavCarrierNode(void)
	{
	}

	void NavCarrierNode::Serialize( FArchive& ar )
	{
		NavEntityNode::Serialize(ar);
	}

	void NavCarrierNode::Deserialize( FArchive& ar )
	{
		NavEntityNode::Deserialize(ar);
	}

	bool NavCarrierNode::AABBoxCollideBox( const AABBox& box,float curTime )
	{
		if(!m_bRunning)
			return false;

		if(!m_worldBox.Intersects(box))
			return false;

		if(m_pRes!=NULL
			&&m_pRes->IsCreated())
		{
			//--计算平移
			Vector3 translation;
			GetPos(curTime,translation);

			//--修改世界矩阵的逆
			m_toWorldInv._41=-translation.x;
			m_toWorldInv._42=-translation.y;
			m_toWorldInv._43=-translation.z;

			//--将参数从world空间变换到object空间
			AABBox localBox=box;
			localBox.TransformFast(m_toWorldInv,m_scaleInv);

			//--
			if(!m_pRes->AABBoxCollideBox(localBox))
				return false;

			return true;
		}

		return false;		
	}

	bool NavCarrierNode::AABBoxCollideBoxTopHighest( const AABBox& box,float curTime,float& intersectY )
	{
		if(!m_bRunning)
			return false;

		if(!m_worldBox.Intersects(box))
			return false;

		if(m_pRes!=NULL
			&&m_pRes->IsCreated())
		{
			//--计算平移
			Vector3 translation;
			GetPos(curTime,translation);

			//--修改世界矩阵的逆
			m_toWorldInv._41=-translation.x;
			m_toWorldInv._42=-translation.y;
			m_toWorldInv._43=-translation.z;

			//--将参数从world空间变换到object空间
			AABBox localBox=box;
			localBox.TransformFast(m_toWorldInv,m_scaleInv);

			Vector3 intersectPos(0,intersectY,0);
			Vector3 localPos;
			D3DXVec3TransformCoord(&localPos,&intersectPos,&m_toWorldInv);

			//--
			if(!m_pRes->AABBoxCollideBoxTopHighest(localBox,localPos.y))
				return false;

			//--修改世界矩阵
			m_toWorld._41=translation.x;
			m_toWorld._42=translation.y;
			m_toWorld._43=translation.z;

			//--将碰撞点从object空间变换到world空间
			D3DXVec3TransformCoord(&intersectPos,&localPos,&m_toWorld);
			intersectY=intersectPos.y;

			return true;
		}

		return false;
	}

	bool NavCarrierNode::QuerySurfaceType( const AABBox& box,float curTime,float& intersectY,ESurfaceType& out )
	{
		if(!m_bRunning)
			return false;

		if(!m_worldBox.Intersects(box))
			return false;

		if(m_pRes!=NULL
			&&m_pRes->IsCreated())
		{
			//--计算平移
			Vector3 translation;
			GetPos(curTime,translation);

			//--修改世界矩阵的逆
			m_toWorldInv._41=-translation.x;
			m_toWorldInv._42=-translation.y;
			m_toWorldInv._43=-translation.z;

			//--将参数从world空间变换到object空间
			AABBox localBox=box;
			localBox.TransformFast(m_toWorldInv,m_scaleInv);

			Vector3 intersectPos(0,intersectY,0);
			Vector3 localPos;
			D3DXVec3TransformCoord(&localPos,&intersectPos,&m_toWorldInv);

			//--
			if(!m_pRes->AABBoxCollideBoxTopHighest(localBox,localPos.y))
				return false;

			//--修改世界矩阵
			m_toWorld._41=translation.x;
			m_toWorld._42=translation.y;
			m_toWorld._43=translation.z;

			//--将碰撞点从object空间变换到world空间
			D3DXVec3TransformCoord(&intersectPos,&localPos,&m_toWorld);
			intersectY=intersectPos.y;

			out=m_surfaceType;
			return true;
		}

		return false;
	}

	NavSceneNode* NavCarrierNode::Clone()
	{
		NavCarrierNode* pNewNode=new NavCarrierNode;

		NavEntityNode::Clone(pNewNode);

		pNewNode->m_bRunning		= m_bRunning;
		pNewNode->m_speed			= m_speed;
		pNewNode->m_startRunTime	= m_startRunTime;
		pNewNode->m_path			= m_path;

		return pNewNode;
	}

	void NavCarrierNode::PosToWorld( const Vector3& localPos,float curTime,Vector3& out )
	{
		//--计算平移
		Vector3 translation;
		GetPos(curTime,translation);

		//--修改世界矩阵
		m_toWorld._41=translation.x;
		m_toWorld._42=translation.y;
		m_toWorld._43=translation.z;

		//--
		D3DXVec3TransformCoord(&out,&localPos,&m_toWorld);
	}

	void NavCarrierNode::PosToLocal( const Vector3& worldPos,float curTime,Vector3& out )
	{
		//--计算平移
		Vector3 translation;
		GetPos(curTime,translation);

		//--修改世界矩阵
		m_toWorldInv._41=-translation.x;
		m_toWorldInv._42=-translation.y;
		m_toWorldInv._43=-translation.z;

		//--
		D3DXVec3TransformCoord(&out,&worldPos,&m_toWorldInv);
	}

	void NavCarrierNode::DirToLocal( const Vector3& worldDir,float curTime,Vector3& out )
	{
		//--计算平移
		Vector3 translation;
		GetPos(curTime,translation);

		//--修改世界矩阵的逆
		m_toWorldInv._41=-translation.x;
		m_toWorldInv._42=-translation.y;
		m_toWorldInv._43=-translation.z;

		//--
		D3DXVec3TransformNormal(&out,&worldDir,&m_toWorldInv);
	}

	bool NavCarrierNode::AABBoxCollideBoxInLocal( const AABBox& box )
	{
		if(!m_bRunning)
			return false;

		if(m_pRes!=NULL
			&&m_pRes->IsCreated())
		{
			return m_pRes->AABBoxCollideBox(box);
		}

		return false;		
	}

	bool NavCarrierNode::AABBoxCollideBoxTopHighestInLocal( const AABBox& box,float& intersectY )
	{
		if(!m_bRunning)
			return false;

		if(m_pRes!=NULL
			&&m_pRes->IsCreated())
		{

			return m_pRes->AABBoxCollideBoxTopHighest(box,intersectY);
		}

		return false;
	}

	void NavCarrierNode::SetWorldBox( const AABBox& box )
	{
		m_worldBox=box;
	}

	void NavCarrierNode::GetMoveDirAndSpeed( float curTime,Vector3& dir )
	{
		ASSERT(m_bRunning);

		float runTime=fmodf(curTime-m_startRunTime,m_lapRunTime);

		float factor=0.0f;
		for(size_t i=0;i<m_path.size()-1;++i)
		{
			const tagWayPoint& p1=m_path[i];

			if(runTime<p1.stopEndTime)
			{
				dir=MathConst::Zero3;
				return;
			}

			if(runTime<p1.moveEndTime)
			{
				const tagWayPoint& p2=m_path[i+1];
				dir=p2.pos-p1.pos;
				Vec3Normalize(dir);
				dir*=m_speed;
				return;
			}
		}

		tagWayPoint& back=m_path.back();
		if(runTime<back.stopEndTime)
		{
			dir=MathConst::Zero3;
			return;
		}

		tagWayPoint& front=m_path.front();
		dir=front.pos-back.pos;
		Vec3Normalize(dir);
		dir*=m_speed;
	}

	void NavCarrierNode::AdjustJumpDirAndSpeed( float curTime,Vector3& xzDir,float& xzSpeed,float& ySpeed )
	{
		Vector3 carDir;
		GetMoveDirAndSpeed(curTime,carDir);

		Vector3 jmpDir=xzDir;
		jmpDir.y=0.0f;
		Vec3Normalize(jmpDir);
		jmpDir.x*=xzSpeed;
		jmpDir.y=ySpeed;
		jmpDir.z*=xzSpeed;

		Vector3 sumDir=carDir+jmpDir;

		ySpeed=sumDir.y;
		sumDir.y=0.0f;
		xzSpeed=Vec3Len(&sumDir);
		if(xzSpeed>0.0f)
			xzDir=sumDir/xzSpeed;
		else
			xzDir=MathConst::Zero3;
	}

	void NavCarrierNode::ClearPath()
	{
		ASSERT(!m_bRunning);

		m_path.clear();
	}

	void NavCarrierNode::AddWayPoint( const Vector3& pos,float stopTime )
	{
		ASSERT(!m_bRunning);

		tagWayPoint point;
		point.pos=pos;
		point.stopTime=stopTime;
		m_path.push_back(point);
	}

	void NavCarrierNode::SetSpeed( float speed )
	{
		ASSERT(!m_bRunning);

		m_speed=speed;
	}

	void NavCarrierNode::Run( float curTime,float timePos )
	{
		ASSERT(!m_bRunning);

		if(m_path.size()<2)
			return;

		if(m_speed<=0.0f)
			return;

		m_startRunTime=curTime-timePos;

		//--计算运行一圈需要多长时间
		m_lapRunTime=0.0f;
		for(size_t i=0;i<m_path.size()-1;++i)
		{
			tagWayPoint& p1=m_path[i];
			tagWayPoint& p2=m_path[i+1];

			m_lapRunTime+=p1.stopTime;
			p1.stopEndTime=m_lapRunTime;

			m_lapRunTime+=Vec3Dist(p1.pos,p2.pos)/m_speed;
			p1.moveEndTime=m_lapRunTime;
		}

		tagWayPoint& front=m_path.front();
		tagWayPoint& back=m_path.back();
		
		m_lapRunTime+=back.stopTime;
		back.stopEndTime=m_lapRunTime;

		m_lapRunTime+=Vec3Dist(back.pos,front.pos)/m_speed;
		back.moveEndTime=m_lapRunTime;

		//--
		m_bRunning=true;
	}

	float NavCarrierNode::GetRunTime(float curTime)
	{
		ASSERT(m_bRunning);

		return curTime-m_startRunTime;		
	}

	void NavCarrierNode::GetPos( float curTime,Vector3& out )
	{
		ASSERT(m_bRunning);

		float runTime=fmodf(curTime-m_startRunTime,m_lapRunTime);

		float factor;
		for(size_t i=0;i<m_path.size()-1;++i)
		{
			const tagWayPoint& p1=m_path[i];

			if(runTime<p1.stopEndTime)
			{
				out=p1.pos;
				return;
			}

			if(runTime<p1.moveEndTime)
			{
				const tagWayPoint& p2=m_path[i+1];
				factor=(runTime-p1.stopEndTime)/(p1.moveEndTime-p1.stopEndTime);
				if(factor<0.0f)factor=0.0f;
				if(factor>1.0f)factor=1.0f;
				LinearLerp(p1.pos,p2.pos,factor,out);
				return;
			}
		}

		tagWayPoint& back=m_path.back();
		if(runTime<back.stopEndTime)
		{
			out=back.pos;
			return;
		}

		tagWayPoint& front=m_path.front();
		factor=(runTime-back.stopEndTime)/(back.moveEndTime-back.stopEndTime);
		if(factor<0.0f)factor=0.0f;
		if(factor>1.0f)factor=1.0f;
		LinearLerp(back.pos,front.pos,factor,out);
	}

}//namespace WorldBase