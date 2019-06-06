#pragma once

namespace WorldBase
{
	/** \class PathPointDistMap
		\brief 导航点之间最短距离二维表
	*/
	class PathPointDistMap
	{
	public:
		PathPointDistMap(void);
		~PathPointDistMap(void);

		void Create(int numPoints);
		void Clone(PathPointDistMap* pTarget);
		void WriteFile(FILE* fp);
		void ReadFile(IFS* pFS,DWORD hFile);

		inline float& operator()(int point1,int point2)
		{
			int index=point1*m_numPoints+point2;
			ASSERT(index<(int)m_data.size());
			return m_data[index];
		}

	private:
		vector<float> m_data;
		int	m_numPoints;
	};
}//namespace WorldBase
