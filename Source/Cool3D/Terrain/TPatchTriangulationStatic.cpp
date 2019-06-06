#include "StdAfx.h"
#include ".\tpatchtriangulationstatic.h"
#include "..\Util\DebugMisc.h"
#include "..\Util\Exception.h"

namespace Cool3D
{

	///////////////////////////////////////////////////////////////////////////////////////////
	// class TPatchLodStatic
	//

	TPatchLodStatic::TPatchLodStatic()
	{
		m_numGrids = 0;
		m_pParent = NULL;
	}

	TPatchLodStatic::~TPatchLodStatic()
	{
	}

	WORD TPatchLodStatic::MakeIndex(int x, int y)
	{
		return y * m_mapW + x;
	}

	void _AppendToVector(vector<WORD>& src, vector<WORD>& dst)
	{
		if(src.empty()) 
			return;

		dst.reserve(src.size()+dst.size());
		for(size_t i=0;i<src.size();i++)
			dst.push_back(src[i]);
	}

	void _AppendToVector(vector<WORD>& src, vector<WORD>& dst, WORD baseIndex)
	{
		if(src.empty()) 
			return;

		dst.reserve(src.size()+dst.size());
		for(size_t i=0;i<src.size();i++)
			dst.push_back(src[i]+baseIndex);
	}

	void AppendTriFan(vector<WORD>& fan, vector<WORD>& indices)
	{
		size_t num=fan.size();
		ASSERT(num > 0);
		WORD share=fan[0];
		indices.reserve(3*num+indices.size());
		for(size_t i=2;i<num;i++)
		{
			indices.push_back(share);
			indices.push_back(fan[i-1]);
			indices.push_back(fan[i]);
		}
	}

	void TPatchLodStatic::Create(int lod, TPatchTriangulationStatic* pParent)
	{
		ASSERT(lod>=0&&lod<TPatchTriangulationStatic::NUM_LOD);
		ASSERT(pParent);

		m_lod = lod;
		m_mapW = pParent->m_sideVerts[lod];
		m_mapH = pParent->m_sideVerts[lod];
		m_pParent=pParent;
		m_numGrids = pParent->m_sideGrids[lod]*pParent->m_sideGrids[lod];
		ASSERT(m_numGrids<=MAX_GRIDS);

		int sideGrids = pParent->m_sideGrids[lod];
		int gridSizeX = (m_mapW-1) / sideGrids;
		int gridSizeY = (m_mapH-1) / sideGrids;
		
		for( int m=0; m<sideGrids; m++ )
		{
			for( int n=0; n<sideGrids; n++ )
			{
				int x = n*gridSizeX;
				int y = m*gridSizeY;
				int center_x = x + gridSizeX / 2;
				int center_y = y + gridSizeY / 2;
				int x1, y1;
				if( n == sideGrids - 1 )
					x1 = m_mapW - 1;
				else
					x1 = x + gridSizeX;
				if( m == sideGrids - 1 )
					y1 = m_mapH - 1;
				else
					y1 = y + gridSizeY;
				WORD indexCenter = MakeIndex(center_x, center_y);
				WORD indexLeftTop = MakeIndex(x, y);
				WORD indexLeftBottom = MakeIndex(x, y1);
				WORD indexRightTop = MakeIndex(x1, y);
				WORD indexRightBottom = MakeIndex(x1, y1);

				int grid = m * sideGrids + n;
				ASSERT( grid>=0 && grid<m_numGrids );

				//top triangle
				m_indices[grid].push_back(indexCenter);	
				m_indices[grid].push_back(indexLeftTop);
				m_indices[grid].push_back(indexRightTop);

				//right triangle
				m_indices[grid].push_back(indexCenter);
				m_indices[grid].push_back(indexRightTop);
				m_indices[grid].push_back(indexRightBottom);

				//bottom triangle
				m_indices[grid].push_back(indexCenter);
				m_indices[grid].push_back(indexRightBottom);
				m_indices[grid].push_back(indexLeftBottom);

				//left triangle
				m_indices[grid].push_back(indexCenter);
				m_indices[grid].push_back(indexLeftBottom);
				m_indices[grid].push_back(indexLeftTop);

			}
		}
	}

	void TPatchLodStatic::GetIndices(const vector<int>& grids,vector<WORD>& indices)
	{
		for(size_t i=0; i<grids.size(); i++)
		{
			int grid = grids[i];
			ASSERT(grid>=0 && grid<m_numGrids);
			_AppendToVector( m_indices[grid], indices );
		}
		ASSERT(indices.size()%3==0);
	}

	void TPatchLodStatic::GetIndices(const vector<int>& grids,vector<WORD>& indices,WORD baseIndex)
	{
		for(size_t i=0; i<grids.size(); i++)
		{
			int grid = grids[i];
			ASSERT(grid>=0 && grid<m_numGrids);
			_AppendToVector( m_indices[grid], indices, baseIndex );
		}
		ASSERT(indices.size()%3==0);
	}


	void TPatchLodStatic::GetIndices(vector<WORD>& indices)
	{
		for(int i=0; i<MAX_GRIDS; i++)
		{
			_AppendToVector( m_indices[i], indices );
		}
		ASSERT(indices.size()%3==0);
	}



	///////////////////////////////////////////////////////////////////////////////////////////
	// class TPatchTriangulationStatic
	//
	int TPatchTriangulationStatic::m_sideGrids[NUM_LOD]={16};
	int TPatchTriangulationStatic::m_sideVerts[NUM_LOD]={33};

	TPatchTriangulationStatic::TPatchTriangulationStatic(void)
	{
	}

	TPatchTriangulationStatic::~TPatchTriangulationStatic(void)
	{
	}

	TPatchTriangulationStatic* TPatchTriangulationStatic::Inst()
	{
		static TPatchTriangulationStatic Inst;
		return &Inst;
	}

	void TPatchTriangulationStatic::Create()
	{
		for(int i=0; i<NUM_LOD; i++)
			m_lods[i].Create(i,this);
	}

	void TPatchTriangulationStatic::GetIndices(int lod,const vector<int>& grids,vector<WORD>& indices,int& numFace)
	{
		ASSERT(lod>=0 && lod<NUM_LOD);
		m_lods[lod].GetIndices(grids,indices);
		numFace=grids.size()*4;
	}

	void TPatchTriangulationStatic::GetIndices(int lod,vector<WORD>& indices)
	{
		ASSERT(lod>=0 && lod<NUM_LOD);
		m_lods[lod].GetIndices(indices);
	}

}//end namespace Cool3D