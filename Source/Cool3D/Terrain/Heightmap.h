#pragma once
#include "..\Cool3D.h"
#include "..\Math\Math.h"
#include "..\Math\Ray.h"
#include "..\Util\Exception.h"
#include "..\Public\VertexDefine.h"
#include "..\Util\FArchive.h"

class IFS;
namespace Cool3D
{
	class AABBox;
	class VertStream;
	/**	\class Heightmap
		\brief 高度图
	*/
	class Cool3D_API Heightmap
	{
	public:
		enum EFormat
		{
			EF_8Bit,
			EF_16Bit
		};
	public:
		Heightmap(void);
		virtual ~Heightmap(void);

		inline UINT Width() const	{ return m_hmapWidth; }
		inline UINT Height() const	{ return m_hmapHeight; }
		inline float GetXScale() const	{ return m_xScale; }
		inline float GetYScale() const	{ return m_yScale; }
		inline float GetZScale() const	{ return m_zScale; }
		inline Vector2 GetStartPos() const { return m_startPos;}
		inline EFormat GetFormat() const { return m_format;}
		inline void SetXScale(float s)		{	m_xScale=s;}
		inline void SetYScale(float s)		{	m_yScale=s;}
		inline void SetZScale(float s)		{	m_zScale=s;}
		inline void SetStartPos(Vector2& pos) { m_startPos=pos;}

		inline bool IfIndex(int x,int z)
		{ return x>=0 && z>=0 && x<(int)m_hmapWidth && z<(int)m_hmapHeight; }
		inline POINT World2Tile(const Vector3& pt) const
		{
			POINT p;
			p.x=long((pt.x-m_startPos.x)/m_xScale);
			p.y=long((pt.z-m_startPos.y)/m_zScale);
			return p;
		}
		inline Vector3 Tile2World(UINT x,UINT z) const
		{
			return Vector3(x*m_xScale+m_startPos.x,GetHeight(x,z),
				z*m_zScale+m_startPos.y);
		}
		inline float GetHeight(UINT x,UINT z) const
		{
			ASSERT(x<m_hmapWidth);
			ASSERT(z<m_hmapHeight);
			if(m_format==EF_8Bit)
				return m_heightMap8[GetIndex(x,z)]*m_yScale;
			else
				return m_heightMap16[GetIndex(x,z)]*m_yScale;
		}
		inline UINT GetIndex(UINT x,UINT z) const
		{
			return z*m_hmapWidth+x;
		}
		inline int GetValue(UINT x,UINT z) const
		{
			ASSERT(x<m_hmapWidth);
			ASSERT(z<m_hmapHeight);
			if(m_format==EF_8Bit)
				return m_heightMap8[GetIndex(x,z)];
			else
				return m_heightMap16[GetIndex(x,z)];
		}
		inline void SetValue(UINT x,UINT z,int val)
		{
			ASSERT(x<m_hmapWidth);
			ASSERT(z<m_hmapHeight);
			if(m_format==EF_8Bit)
				m_heightMap8[GetIndex(x,z)]=val;
			else
				m_heightMap16[GetIndex(x,z)]=val;
		}

		void SafeAddValue(int x,int z,int& val);
		void SafeSetVal(int x,int z,int& val);

		inline void GetVertPos(UINT x,UINT z,Vector3& out) const
		{
			out.x=m_startPos.x+x*m_xScale;
			out.z=m_startPos.y+z*m_zScale;
			out.y=GetHeight(x,z);
		}
		inline void GetVertPos(UINT index,Vector3& out) const
		{
			UINT x=index%m_hmapWidth;
			UINT z=index/m_hmapWidth;
			GetVertPos(x,z,out);
		}
		void GetVertNormal(UINT x,UINT z,Vector3& out) const;
		void GetVertNormal(int index,Vector3& out) const
		{
			UINT x=index%m_hmapWidth;
			UINT z=index/m_hmapWidth;
			GetVertNormal(x,z,out);
		}
		/**	进行坐标clip,然后在得到高度
		*/
		float GetSafeHeight(int x,int z) const;
		int GetSafeVal(int x,int z) const;
		
		
		inline bool IsCreated() const	{ return m_heightMap8!=0 || m_heightMap16!=0;}
		void Destroy();
		/**	从一个24bit的图像中去第一个通道来创建
		*/
		bool ImportImage(const TCHAR* szFileName,float xScale,float zScale,float yScale);
		bool ExportImage(const TCHAR* szFileName);
		/**	使用噪音函数创建随机地形
		*/
		void CreateByNoise(float freq,UINT width,UINT height,float xScale,float zScale,float yScale,EFormat format=EF_8Bit);
		void Create(int initVal,UINT width,UINT height,float xScale,float zScale,float yScale,EFormat format=EF_8Bit);
		/**	创建Vertex stream的内容
		*/
		void FillVertStream_PNT(VertStream *pStream,int sideVerts,RECT rect,bool uvInRect);
		void FillVertStream_PN(VertStream *pStream,int sideVerts,RECT rect);
		void FillVertStream_P(VertStream *pStream,int sideVerts,RECT rect);

		/**	检测射线与地形的交点
		*/
		bool RayCollision(const Ray& ray,Vector3& pt,float maxLength=100000.0f,bool bAutoGroundHeight=true);

		bool WriteToFile(FILE *fp) const;
		bool ReadFromFile(IFS* pFS,DWORD hFile);
		void Serialize(FArchive& ar);
		void Deserialize(FArchive& ar,EFormat format=EF_8Bit);
		void BuildAABBox(AABBox *pBox,const RECT* pRect=NULL);
		void GetHeightRange(float& minh,float &maxh,RECT rect);

		void AutoCenterStartPos();

		Heightmap *NewSubMap(RECT rect) const;
		void SetSubMap(Heightmap *pSubMap,RECT& rect);

		//! 将自己的内容copy到指定的对象中
		void CopyTo(Heightmap& out) const;

		BYTE *GetRawBuffer() const	{	
			if(m_format==EF_8Bit)
				return m_heightMap8;
			else
				return (BYTE*)m_heightMap16;
		}

		void ConvertTo16Bit();
		void ClampPos(int& x,int& z);

	protected:
		void CreateData(UINT width,UINT height,EFormat format);
	protected:
		float			m_xScale,		//单个格子的大小
						m_yScale,		//m_heightMap[i]*m_yScale得到最后的高度
						m_zScale;
		Vector2			m_startPos;		//起始坐标,map的(0,0)象素对应的vert在世界中的位置
		UINT			m_hmapWidth,
						m_hmapHeight;	//Height的大小
		BYTE*			m_heightMap8;
		WORD*			m_heightMap16;
		EFormat			m_format;		//格式
	};
}//namespace Cool3D