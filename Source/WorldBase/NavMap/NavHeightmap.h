#pragma once

namespace WorldBase
{
	/**	\class NavHeightmap
		\brief ∏ﬂ∂»Õº
	*/
	class NavHeightmap : public Heightmap
	{
	public:
		NavHeightmap(void);
		virtual ~NavHeightmap(void);

		//--
		inline POINT World2Tile(float x,float z) const
		{
			POINT p;
			p.x=long((x-m_startPos.x)/m_xScale);
			p.y=long((z-m_startPos.y)/m_zScale);
			return p;
		}
		inline POINT SafeWorld2Tile(float x,float z) const
		{
			POINT p;
			p.x=long((x-m_startPos.x)/m_xScale);
			p.y=long((z-m_startPos.y)/m_zScale);
			if(p.x<0)p.x=0;
			if(p.x>=(long)m_hmapWidth)p.x=m_hmapWidth-1;
			if(p.y<0)p.y=0;
			if(p.y>=(long)m_hmapHeight)p.y=m_hmapHeight-1;
			return p;
		}
		inline Vector3 SafeTile2World(UINT x,UINT z) const
		{
			return Vector3(x*m_xScale+m_startPos.x,GetSafeHeight(x,z),
				z*m_zScale+m_startPos.y);
		}
		bool IfWillSlide(float x,float z,float slideFall);
		bool SafeCalcSlideDir(float x,float z,Vector3& out);
		float SafeGetGroundHeight(float x,float z);
		float SafeGetGroundHeight(float x1,float z1,float x2,float z2);
		float SafeGetGroundHeightSlerp(float x,float z);
		float SafeGetGroundHeightSlerp(float x1,float z1,float x2,float z2);
	};
}//namespace WorldBase