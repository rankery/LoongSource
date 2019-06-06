#pragma once

namespace WorldBase
{
	/** \class OutlinePathFinder
		\brief 轮廓跟踪寻路算法
	*/
	class OutlinePathFinder
	{
	private:
		enum EWay
		{
			EW_None,
			EW_Top,
			EW_Left,
			EW_Right,
			EW_Bottom,
			EW_LeftTop,
			EW_RightTop,
			EW_LeftBottom,
			EW_RightBottom,
		};
		struct tagBlock		//障碍物
		{
			POINT inPt;		//接入点
			POINT outPt;	//接出点
		};
	public:
		OutlinePathFinder(void);
		~OutlinePathFinder(void);

		/** 设置可行走区域图
		*/
		void SetCangoMap(BitMap* pCangoMap);
		/** 设置绕行障碍物时最多允许遍历多少个格子
		*/
		void SetMaxRoundTiles(int numTiles);
		/** 寻路
		*/
		bool FindPath(int x1,int z1,int x2,int z2,list<POINT>& path);

	private:
		bool FindBlocks(int x1,int z1,int x2,int z2,tagBlock* pBlocks,int& numBlocks);
		void AddPathPoint(int x1,int z1,list<POINT>& path,int pointX,int pointZ);
		void AddPathPoints(int x1,int z1,const POINT* pPoints,int numPoints,list<POINT>& path);
		int RoundBlock_CW(const tagBlock* pBlock,POINT* pTiles,int& numTiles);
		int RoundBlock_CCW(const tagBlock* pBlock,POINT* pTiles,int& numTiles);

	private:
		BitMap* m_pCangoMap;
		int m_maxRoundTiles;
	};
}//namespace WorldBase
