#include "stdafx.h"
#include ".\TRiverPatchTriangulationDynamic.h"

namespace Cool3D
{
	TRiverPatchLODDynamic::TRiverPatchLODDynamic()
	{
		m_pParent=NULL;
	}

	TRiverPatchLODDynamic::~TRiverPatchLODDynamic()
	{
		//
	}

	void TRiverPatchLODDynamic::Create( TRiverPatchTriangulationDynamic* pParent,int lod )
	{
		m_pParent=pParent;
		m_lod=lod;
		m_mapH=m_mapW=pParent->m_sideVerts[lod];

		vector<int> cfg;
		int size=(m_mapW-1)*(m_mapH-1)*4*3*sizeof(WORD);
		cfg.push_back(size);
		m_ibPool.Init(cfg);
	}

	void TRiverPatchLODDynamic::Destroy()
	{
		m_ibPool.Destroy();
	}

	IIndexBuffer* TRiverPatchLODDynamic::GetIB_HW( int& numFaces )
	{
		const tagIB& ib=_GetIB();
		numFaces=ib.indices.size()/3;
		return ib.pIB;
	}

	const vector<WORD>& TRiverPatchLODDynamic::GetIB()
	{
		const tagIB& ib=_GetIB();
		return ib.indices;
	}

	WORD TRiverPatchLODDynamic::MakeIndex( int x, int y )
	{
		return y*m_mapW+x;
	}

	void TRiverPatchLODDynamic::BuildIndices( vector<WORD>& indices )
	{
		int sideGrids=m_pParent->m_sideGrids[m_lod];
		int gridSizeX=(m_mapW-1)/sideGrids;
		int gridSizeY=(m_mapH-1)/sideGrids;

		for( int m=0;m<sideGrids;m++ )
		{
			for( int n=0;n<sideGrids;n++ )
			{
				int x=n*gridSizeX;
				int y=m*gridSizeY;
				int x1, y1;
				if( n==sideGrids-1 )
					x1=m_mapW-1;
				else
					x1=x+gridSizeX;
				if( m==sideGrids-1 )
					y1=m_mapH-1;
				else
					y1=y+gridSizeY;
				WORD indexLeftTop=MakeIndex(x,y);
				WORD indexLeftBottom=MakeIndex(x,y1);
				WORD indexRightTop=MakeIndex(x1,y);
				WORD indexRightBottom=MakeIndex(x1,y1);

				//right-top triangle
				indices.push_back(indexLeftTop);
				indices.push_back(indexRightTop);
				indices.push_back(indexRightBottom);

				//left-bottom triangle
				indices.push_back(indexLeftTop);
				indices.push_back(indexRightBottom);
				indices.push_back(indexLeftBottom);
			}
		}

		ASSERT(indices.size()%3==0);
	}

	const TRiverPatchLODDynamic::tagIB& TRiverPatchLODDynamic::_GetIB()
	{
		map<int,tagIB>::iterator iter=m_ibCache.find(m_lod);
		if(iter==m_ibCache.end())
		{
			tagIB newIB;
			BuildIndices(newIB.indices);

			int size=newIB.indices.size()*sizeof(WORD);
			newIB.pIB=(IIndexBuffer*)m_ibPool.Alloc(size);
			newIB.pIB->CopyFromMem(&newIB.indices[0],size);

			pair<map<int,tagIB>::iterator,bool> ret=m_ibCache.insert(make_pair(m_lod,newIB));
			ASSERT(ret.second==true);
			iter=ret.first;
		}

		return iter->second;
	}

	int TRiverPatchTriangulationDynamic::m_sideGrids[NUM_LOD]={16,8,2};
	int TRiverPatchTriangulationDynamic::m_sideVerts[NUM_LOD]={17,17,17};

	TRiverPatchTriangulationDynamic::TRiverPatchTriangulationDynamic( void )
	{
		//
	}

	TRiverPatchTriangulationDynamic::~TRiverPatchTriangulationDynamic( void )
	{
		//
	}

	void TRiverPatchTriangulationDynamic::Create()
	{
		for(int i=0;i<NUM_LOD;i++)
			m_lods[i].Create(this,i);
	}

	void TRiverPatchTriangulationDynamic::Destroy()
	{
		for(int i=0;i<NUM_LOD;i++)
			m_lods[i].Destroy();
	}

	IIndexBuffer* TRiverPatchTriangulationDynamic::GetIB_HW( int lod,int& numVerts,int& numFaces )
	{
		ASSERT(lod>=0 && lod<NUM_LOD);

		numVerts=m_sideVerts[lod]*m_sideVerts[lod];

		return m_lods[lod].GetIB_HW(numFaces);
	}

	const vector<WORD>& TRiverPatchTriangulationDynamic::GetIB( int lod )
	{
		ASSERT(lod>=0 && lod<NUM_LOD);

		return m_lods[lod].GetIB();
	}

	TRiverPatchTriangulationDynamic* TRiverPatchTriangulationDynamic::Inst()
	{
		static TRiverPatchTriangulationDynamic Inst;
		return &Inst;
	}
}//namespace Cool3D