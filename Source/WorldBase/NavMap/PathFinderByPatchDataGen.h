#pragma once
#include "PathFinderByPatch.h"

namespace WorldBase
{
	/** \class PathFinderByPatchDataGen
		\brief 基于地形块寻路数据生成器
	*/
	class PathFinderByPatchDataGen
	{
		enum EConst
		{
			EPatchSize=PathFinderByPatch::EPatchSize
		};
	public:
		PathFinderByPatchDataGen(void);
		~PathFinderByPatchDataGen(void);

		void BeginBuild(BitMap* pCangoMap);
		bool EndBuild(const TCHAR* szSavePath,const TCHAR* szMapName,const TCHAR* szFileName);

	private:
		void BuildAreaMap(PathFinderByPatch::tagPatch* pPatch,int baseX,int baseZ,HalfByteMap* pOut);

		void BuildLeftLinkPoints(
			PathFinderByPatch::tagPatch* pPatch,
			PathFinderByPatch::tagPatch* pLeftPatch);
		void BuildRightLinkPoints(
			PathFinderByPatch::tagPatch* pPatch,
			PathFinderByPatch::tagPatch* pRightPatch);
		void BuildTopLinkPoints(
			PathFinderByPatch::tagPatch* pPatch,
			PathFinderByPatch::tagPatch* pTopPatch);
		void BuildBottomLinkPoints(
			PathFinderByPatch::tagPatch* pPatch,
			PathFinderByPatch::tagPatch* pBottomPatch);

		void BuildLeftTopLinkPoints(
			PathFinderByPatch::tagPatch* pPatch,
			PathFinderByPatch::tagPatch* pLeftTopPatch,
			PathFinderByPatch::tagPatch* pLeftPatch,
			PathFinderByPatch::tagPatch* pTopPatch);
		void BuildLeftBottomLinkPoints(
			PathFinderByPatch::tagPatch* pPatch,
			PathFinderByPatch::tagPatch* pLeftBottomPatch,
			PathFinderByPatch::tagPatch* pLeftPatch,
			PathFinderByPatch::tagPatch* pBottomPatch);
		void BuildRightTopLinkPoints(
			PathFinderByPatch::tagPatch* pPatch,
			PathFinderByPatch::tagPatch* pRightTopPatch,
			PathFinderByPatch::tagPatch* pRightPatch,
			PathFinderByPatch::tagPatch* pTopPatch);
		void BuildRightBottomLinkPoints(
			PathFinderByPatch::tagPatch* pPatch,
			PathFinderByPatch::tagPatch* pRightBottomPatch,
			PathFinderByPatch::tagPatch* pRightPatch,
			PathFinderByPatch::tagPatch* pBottomPatch);

	private:
		BitMap*		m_pCangoMap;
	};

}//namespace WorldBase
