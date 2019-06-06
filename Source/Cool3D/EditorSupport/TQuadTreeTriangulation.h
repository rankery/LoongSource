#pragma once

namespace Cool3D
{
	class Heightmap;
	/**	\class TQuadTreeTriangulation
		\brief 使用四叉树为HeightMap产生三角形list
	*/
	class TQuadTreeTriangulation
	{
	public:
		TQuadTreeTriangulation(void);
		virtual ~TQuadTreeTriangulation(void);

		/**	设置数据源
		*/
		void SetDataSrc(Heightmap *pMap,RECT rect,bool forceTri=false)	{	m_pMap=pMap; m_rect=rect; m_bForceTri=forceTri;}
		/**	产生Triangle List
		*/
		void GenerateIndices(float desireResolution);
		/**	产生Triangle List
			\param	预期的分辨率,相邻的更高分辨率
			\remarks 在相邻的节点将使用更高的分辨率,以保证无缝连接
		*/
		void GenerateIndices(float desireResolution,float higherRes);
		/**	得到Index个数
		*/
		UINT GetNumIndices();
		/**	得到Index Buffer
		*/
		WORD* GetIndices();
	protected:
		/**	递归的计算Quad matrix
		*/
		void BuildMatrixR(RECT myRect);
		/**	递归的创建triangle list,并添加到index buffer
		*/
		void BuildTriListR(RECT myRect);
		/**	递归的修补myRect指向的节点的四条边上可能产生的裂缝
		*/
		void FixCrackTopR(RECT myRect,vector<WORD>& fan,bool first=true);
		void FixCrackLeftR(RECT myRect,vector<WORD>& fan,bool first=true);
		void FixCrackRightR(RECT myRect,vector<WORD>& fan,bool first=true);
		void FixCrackBottomR(RECT myRect,vector<WORD>& fan,bool first=true);

		/**	将一个Rect切分成4个子Rect
		*/
		void GetChildrenRect(RECT myRect,RECT *pRect);
		/**	将一个Triangle Fan添加到triangle list的index buffer中
		*/
		void AppendTriFan(vector<WORD>& fan);
		/**	设置Quad matrix的值
			\param x,y是Height map坐标系
		*/
		void SetMatrix(UINT x,UINT y,BYTE val);
		/**	得到Quad Matrix的值
			\param x,y是Height map坐标系
		*/
		BYTE GetMatrix(UINT x,UINT y);
		/**	得到指定的坐标的Vertex index
			\param x,y是Height map坐标系
			\remarks vertex buffer是建立在m_rect指定的区域上的
		*/
		WORD GetVertIndex(UINT x,UINT y);

	protected:
		/**	rect所指向的节点是否要继续细分
		*/
		bool IfNeedSplit(Heightmap *pMap,RECT rect);
		/**	得到height map中rect区域的最大高度变化值
		*/
		float GetMaxDeltaH(Heightmap *pMap,RECT rect);
	protected:
		Heightmap		*m_pMap;
		BYTE			*m_quadMatrix;//quad tree matrix
		UINT			m_matW,
						m_matH;
		RECT			m_rect;
		vector<WORD>	m_indices;

		float			m_desireResolution;
		float			m_higherRes;
		bool			m_bForceTri;//强制三角形化

	protected:
		static float m_minResolution;
	};
}//namespace Cool3D