#include "StdAfx.h"
#include "NavHeightmap.h"

namespace WorldBase
{
	NavHeightmap::NavHeightmap(void)
	{
	}

	NavHeightmap::~NavHeightmap(void)
	{
	}

	bool NavHeightmap::IfWillSlide( float x,float z,float slideFall)
	{
		int tx=int(x/m_xScale);
		int tz=int(z/m_zScale);
		if(!IfIndex(tx,tz))
			return false;

		int tx1=tx-1;
		int tx2=tx+1;
		int tz1=tz-1;
		int tz2=tz+1;
		if(tx1<0)tx1=0;
		if(tz1<0)tz1=0;
		if(tx2>=(int)m_hmapWidth)tx2=m_hmapWidth-1;
		if(tz2>=(int)m_hmapHeight)tz2=m_hmapHeight-1;

		float h=GetHeight(tx,tz);
		if(h-GetHeight(tx1,tz1)>slideFall)return true;
		if(h-GetHeight(tx, tz1)>slideFall)return true;
		if(h-GetHeight(tx2,tz1)>slideFall)return true;
		if(h-GetHeight(tx1,tz )>slideFall)return true;
		if(h-GetHeight(tx2,tz )>slideFall)return true;
		if(h-GetHeight(tx1,tz2)>slideFall)return true;
		if(h-GetHeight(tx, tz2)>slideFall)return true;
		if(h-GetHeight(tx2,tz2)>slideFall)return true;

		return false;
	}

	bool NavHeightmap::SafeCalcSlideDir( float x,float z,Vector3& out )
	{
		bool bFind=false;
		POINT pt=World2Tile(x,z);
		float maxH=-FLOAT_MAX;
		int maxx=pt.x,maxz=pt.y;
		for(int i=0;i<3;i++)
		{
			POINT center;
			center.x = maxx;
			center.y = maxz;

			int tx1=center.x-1;
			int tx2=center.x+1;
			int tz1=center.y-1;
			int tz2=center.y+1;

			float h=GetSafeHeight(center.x,center.y);

			float h1=h-GetSafeHeight(tx1,tz1);
			float h2=h-GetSafeHeight(center.x,tz1);
			float h3=h-GetSafeHeight(tx2,tz1);
			float h4=h-GetSafeHeight(tx1,center.y);
			float h5=h-GetSafeHeight(tx2,center.y);
			float h6=h-GetSafeHeight(tx1,tz2);
			float h7=h-GetSafeHeight(center.x,tz2);
			float h8=h-GetSafeHeight(tx2,tz2);
			
			float maxH=-FLOAT_MAX;
			if(h1>0
				&&h1>maxH)
			{
				maxH=h1;
				maxx=tx1;
				maxz=tz1;
				bFind=true;
			}
			if(h2>0
				&&h2>maxH)
			{
				maxH=h2;
				maxx=center.x;
				maxz=tz1;
				bFind=true;
			}
			if(h3>0
				&&h3>maxH)
			{
				maxH=h3;
				maxx=tx2;
				maxz=tz1;
				bFind=true;
			}
			if(h4>0
				&&h4>maxH)
			{
				maxH=h4;
				maxx=tx1;
				maxz=center.y;
				bFind=true;
			}
			if(h5>0
				&&h5>maxH)
			{
				maxH=h5;
				maxx=tx2;
				maxz=center.y;
				bFind=true;
			}
			if(h6>0
				&&h6>maxH)
			{
				maxH=h6;
				maxx=tx1;
				maxz=tz2;
				bFind=true;
			}
			if(h7>0
				&&h7>maxH)
			{
				maxH=h7;
				maxx=center.x;
				maxz=tz2;
				bFind=true;
			}
			if(h8>0
				&&h8>maxH)
			{
				maxH=h8;
				maxx=tx2;
				maxz=tz2;
				bFind=true;
			}
		}

		Vector3 pos1=SafeTile2World(pt.x,pt.y);
		Vector3 pos2=SafeTile2World(maxx,maxz);
		out=pos2-pos1;
		Vec3Normalize(out);

		return bFind;
	}

	float NavHeightmap::SafeGetGroundHeight( float x,float z)
	{
		POINT pt=World2Tile(x,z);
		return GetSafeHeight(pt.x,pt.y);
	}

	float NavHeightmap::SafeGetGroundHeight( float x1,float z1,float x2,float z2 )
	{
		POINT p1=SafeWorld2Tile(x1,z1);
		POINT p2=SafeWorld2Tile(x2,z2);

		float h1=GetHeight(p1.x,p1.y);
		float h2=GetHeight(p2.x,p1.y);
		float h3=GetHeight(p1.x,p2.y);
		float h4=GetHeight(p2.x,p2.y);

		float maxh=max(h1,h2);
		maxh=max(maxh,h3);
		maxh=max(maxh,h4);
		return maxh;
	}

	float NavHeightmap::SafeGetGroundHeightSlerp( float x,float z)
	{
		POINT pt=World2Tile(x,z);
		Vector3 pos=SafeTile2World(pt.x,pt.y);

		float h2=GetSafeHeight(pt.x+1,pt.y);
		float h3=GetSafeHeight(pt.x+1,pt.y+1);
		float h4=GetSafeHeight(pt.x,pt.y+1);

		float r=(x-pos.x)/m_xScale;
		float hix1=r*h2+(1-r)*pos.y;
		float hix2=r*h3+(1-r)*h4;

		r=(z-pos.z)/m_zScale;
		return r*hix2+(1-r)*hix1;
	}

	float NavHeightmap::SafeGetGroundHeightSlerp( float x1,float z1,float x2,float z2 )
	{
		POINT p1=SafeWorld2Tile(x1,z1);
		POINT p2=SafeWorld2Tile(x2,z2);

		float h1=GetHeight(p1.x,p1.y);
		float h2=GetHeight(p2.x,p1.y);
		float h3=GetHeight(p1.x,p2.y);
		float h4=GetHeight(p2.x,p2.y);

		float maxx,maxz;
		float maxh;
		if(h1>h2)
		{
			maxh=h1;
			maxx=x1;
			maxz=z1;
		}
		else
		{
			maxh=h2;
			maxx=x2;
			maxz=z1;
		}
		if(h3>maxh)
		{
			maxh=h3;
			maxx=x1;
			maxz=z2;
		}
		if(h4>maxh)
		{
			maxh=h4;
			maxx=x2;
			maxz=z2;
		}

		return SafeGetGroundHeightSlerp(maxx,maxz);
	}


}//namespace WorldBase