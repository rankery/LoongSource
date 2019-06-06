#pragma once
#include "AStarHeap.h"

namespace WorldBase
{
	/** \class IAStarPathFinderCallBack
		\brief A*寻路算法回调接口
	*/
	class IAStarPathFinderCallBack
	{
	public:
		enum EDir
		{
			ED_Left,
			ED_Right,
			ED_Top,
			ED_Bottom,
			ED_LeftTop,
			ED_RightTop,
			ED_LeftBottom,
			ED_RightBottom
		};

		/** A*调用该函数判断是否可以从指定tile通向另一个tile
			\param	in	sx		源tile的x坐标
			\param	in	sz		源tile的z坐标
			\param	in	sa		源tile的区域
			\param	in	dir		搜索方向
			\param	in	dx		目标tile的x坐标
			\param	in	dz		目标tile的z坐标
			\param	out pda		可达的目标tile的区域
			\return	目标区域数量
		*/
		virtual int GetWay(int sx,int sz,int sa,EDir dir,int dx,int dz,int* pda) =0;
	};

	/** \class AStarPathFinder
		\brief A*寻路算法
	*/
	class AStarPathFinder
	{
	public:
		enum EConstPublic
		{
			EMaxArea=10,		
		};

		struct tagPathPoint
		{
			int x,z;
			int area;
		};

		enum EResult
		{
			ER_Failed,
			ER_Arrive,
			ER_GetNearPos,
		};

	private:
		enum EConstPrivate
		{
			ELocalMapSize=128,
		};

		struct tagVisitNode
		{
			tagVisitNode(){}
			tagVisitNode(int _x,int _z,int _area,float _value,float _dist2Start)
				:x(_x),z(_z),area(_area),value(_value),dist2Start(_dist2Start)
			{}
			bool operator<(const tagVisitNode& other)
			{
				return value<other.value;
			}
			int	x,z;			//坐标
			int area;			//区域
			float value;		//估价值
			float dist2Start;	//与起点距离
		};

		struct tagMapNode
		{
			float dist2Start;
			float dist2Target;
			int parentX;
			int parentZ;
			int parentArea;
		};

	public:
		void SetCallBack(IAStarPathFinderCallBack* pCallBack);
		EResult FindPath(int sx,int sz,int sa,int dx,int dz,int da,list<tagPathPoint>& path);

		static AStarPathFinder* Inst();
	private:
		AStarPathFinder(void);
		virtual ~AStarPathFinder(void);

		inline int GlobalX2LocalX(int gx){ return gx-m_localMapStartPos.x;}
		inline int GlobalZ2LocalZ(int gz){ return gz-m_localMapStartPos.y;}
		inline int LocalX2GlobalX(int lx){ return lx+m_localMapStartPos.x;}
		inline int LocalZ2GlobalZ(int lz){ return lz+m_localMapStartPos.y;}

		EResult FindPathCore(list<tagPathPoint>& path);

		bool FindNearPos(int& nearX,int& nearZ,int& nearArea);
		void BuildPath(list<tagPathPoint>& path,int targetX,int targetZ,int targetArea);
		bool Visit(int x,int z,int area,int parentX,int parentZ,int parentArea,float dist2Start);

		void TraceHeap();

	private:
		IAStarPathFinderCallBack*		m_pCallBack;
		bool							m_visitFlag[ELocalMapSize][ELocalMapSize][EMaxArea];
		tagMapNode						m_visitMap[ELocalMapSize][ELocalMapSize][EMaxArea];
		Point							m_startPos;
		Point							m_targetPos;
		int								m_startArea;
		int								m_targetArea;
		Point							m_localMapStartPos;
		AStarHeap<tagVisitNode,1000>	m_visitHeap;
	};

}//namespace WorldBase
