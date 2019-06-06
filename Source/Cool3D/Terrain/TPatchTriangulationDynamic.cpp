#include "StdAfx.h"
#include ".\tpatchtriangulationdynamic.h"
#include "..\Util\DebugMisc.h"
#include "..\Util\Exception.h"

namespace Cool3D
{

	///////////////////////////////////////////////////////////////////////////////////////////
	// class TPatchLodDynamic
	//

	TPatchLodDynamic::TPatchLodDynamic()
	{
		m_pParent=NULL;
	}

	TPatchLodDynamic::~TPatchLodDynamic()
	{
	}

	void TPatchLodDynamic::Create(TPatchTriangulationDynamic* pParent,int lod)
	{
		m_pParent=pParent;
		m_lod=lod;
		m_mapH=m_mapW=pParent->m_sideVerts[lod];

		vector<int> cfg;
		int size=(m_mapW-1)*(m_mapH-1)*4*3*sizeof(WORD);
		if(m_lod==0)
			cfg.push_back(size);//lod0只有一种三角形排列方案
		else
		{
			for(int i=0;i<20;i++)
				cfg.push_back(size);
		}
		m_ibPool.Init(cfg);
	}

	void TPatchLodDynamic::Destroy()
	{
		m_ibPool.Destroy();
	}

	int _MakeKey(int leftLod,int topLod,int rightLod,int bottomLod)
	{
		int key=0;
		key|=((leftLod&0xFF)<<24);
		key|=((topLod&0xFF)<<16);
		key|=((rightLod&0xFF)<<8);
		key|=(bottomLod&0xFF);
		return key;
	}

	const TPatchLodDynamic::tagIB& TPatchLodDynamic::_GetIB(int leftLod,int topLod,int rightLod,int bottomLod)
	{
		int key=_MakeKey(leftLod,topLod,rightLod,bottomLod);

		map<int,tagIB>::iterator iter=m_ibCache.find(key);
		if(iter==m_ibCache.end())
		{
			tagIB newIB;

			BuildIndices(leftLod,topLod,rightLod,bottomLod,newIB.indices);

			int size=newIB.indices.size()*sizeof(WORD);
			newIB.pIB=(IIndexBuffer*)m_ibPool.Alloc(size);
			newIB.pIB->CopyFromMem(&newIB.indices[0],size);

			pair<map<int,tagIB>::iterator,bool> ret
				=m_ibCache.insert(make_pair(key,newIB));
			ASSERT(ret.second==true);
			iter=ret.first;
		}

		return iter->second;
	}

	IIndexBuffer* TPatchLodDynamic::GetIB_HW(int leftLod,int topLod,int rightLod,int bottomLod,int& numFaces)
	{
		const tagIB& ib=_GetIB(leftLod,topLod,rightLod,bottomLod);
		numFaces=ib.indices.size()/3;
		return ib.pIB;
	}

	const vector<WORD>& TPatchLodDynamic::GetIB(int leftLod,int topLod,int rightLod,int bottomLod)
	{
		const tagIB& ib=_GetIB(leftLod,topLod,rightLod,bottomLod);
		return ib.indices;
	}


	WORD TPatchLodDynamic::MakeIndex(int x, int y)
	{
		return y * m_mapW + x;
	}

	void _AppendTriFan(vector<WORD>& fan, vector<WORD>& indices)
	{
		size_t num=fan.size();
		ASSERT(num > 0);
		WORD share=fan[0];
		for(size_t i=2;i<num;i++)
		{
			indices.push_back(share);
			indices.push_back(fan[i-1]);
			indices.push_back(fan[i]);
		}
	}

	void TPatchLodDynamic::BuildIndices(int leftLod,int topLod,int rightLod,int bottomLod,vector<WORD>& indices)
	{
		int sideGrids = m_pParent->m_sideGrids[m_lod];
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

				//top triangle
				if( m == 0 )
				{
					MakeTopTri(sideGrids, x, y, gridSizeX,
						indexCenter, indexLeftTop, indexRightTop, topLod, indices);
				}
				else
				{
					indices.push_back(indexCenter);	
					indices.push_back(indexLeftTop);
					indices.push_back(indexRightTop);
				}

				//right triangle
				if( n == sideGrids - 1 )
				{
					MakeRightTri(sideGrids, x1, y, gridSizeY,
						indexCenter, indexRightTop, indexRightBottom, rightLod, indices);
				}
				else
				{
					indices.push_back(indexCenter);
					indices.push_back(indexRightTop);
					indices.push_back(indexRightBottom);
				}

				//bottom triangle
				if( m == sideGrids - 1 )
				{
					MakeBottomTri(sideGrids, x, y1, gridSizeX,
						indexCenter, indexLeftBottom, indexRightBottom, bottomLod, indices);
				}
				else
				{
					indices.push_back(indexCenter);
					indices.push_back(indexRightBottom);
					indices.push_back(indexLeftBottom);
				}

				//left triangle
				if( n == 0 )
				{
					MakeLeftTri(sideGrids, x, y, gridSizeY,
						indexCenter, indexLeftTop, indexLeftBottom, leftLod, indices);
				}
				else
				{
					indices.push_back(indexCenter);
					indices.push_back(indexLeftBottom);
					indices.push_back(indexLeftTop);
				}						

			}
		}

		ASSERT(indices.size()%3==0);
	}

	void TPatchLodDynamic::MakeTopTri(
		int sideGrids, int x, int y, int gridSizeX,
		int indexCenter, int indexLeftTop, int indexRightTop, int topLod,vector<WORD>& indices)
	{
		int edgeGrids=m_pParent->m_sideGrids[topLod];

		if(sideGrids<edgeGrids)
		{
			vector<WORD> fan;
			fan.push_back(indexCenter);
			fan.push_back(indexLeftTop);

			int subTri = edgeGrids / sideGrids;
			int subTriSize = gridSizeX / subTri;
			for( int i=1; i<subTri; i++ )
			{
				int newIndex = MakeIndex(x+subTriSize*i,y);
				fan.push_back(newIndex);
			}

			fan.push_back(indexRightTop);

			_AppendTriFan(fan,indices);
		}
		else
		{
			indices.push_back(indexCenter);	
			indices.push_back(indexLeftTop);
			indices.push_back(indexRightTop);
		}
	}

	void TPatchLodDynamic::MakeBottomTri(
		int sideGrids, int x, int y1, int gridSizeX,
		int indexCenter, int indexLeftBottom, int indexRightBottom, int bottomLod,vector<WORD>& indices)
	{
		int edgeGrids=m_pParent->m_sideGrids[bottomLod];

		if( sideGrids < edgeGrids )
		{
			vector<WORD> fan;
			fan.push_back(indexCenter);
			fan.push_back(indexRightBottom);

			int subTri = edgeGrids / sideGrids;
			int subTriSize = gridSizeX / subTri;
			for( int i=subTri-1; i>0; i-- )
			{
				int newIndex = MakeIndex(x+subTriSize*i,y1);
				fan.push_back(newIndex);
			}

			fan.push_back(indexLeftBottom);

			_AppendTriFan(fan,indices);
		}
		else
		{
			indices.push_back(indexCenter);	
			indices.push_back(indexRightBottom);
			indices.push_back(indexLeftBottom);
		}
	}

	void TPatchLodDynamic::MakeLeftTri(
		int sideGrids, int x, int y, int gridSizeY,
		int indexCenter, int indexLeftTop, int indexLeftBottom, int leftLod,vector<WORD>& indices)
	{
		int edgeGrids=m_pParent->m_sideGrids[leftLod];

		if( sideGrids < edgeGrids )
		{
			vector<WORD> fan;
			fan.push_back(indexCenter);
			fan.push_back(indexLeftBottom);

			int subTri = edgeGrids / sideGrids;
			int subTriSize = gridSizeY / subTri;
			for( int i=subTri-1; i>0; i-- )
			{
				int newIndex = MakeIndex(x,y+subTriSize*i);
				fan.push_back(newIndex);
			}

			fan.push_back(indexLeftTop);

			_AppendTriFan(fan,indices);
		}
		else
		{
			indices.push_back(indexCenter);	
			indices.push_back(indexLeftBottom);
			indices.push_back(indexLeftTop);
		}
	}

	void TPatchLodDynamic::MakeRightTri(
		int sideGrids, int x1, int y, int gridSizeY,
		int indexCenter, int indexRightTop, int indexRightBottom, int rightLod,vector<WORD>& indices)
	{
		int edgeGrids=m_pParent->m_sideGrids[rightLod];

		if( sideGrids < edgeGrids )
		{
			vector<WORD> fan;
			fan.push_back(indexCenter);
			fan.push_back(indexRightTop);

			int subTri = edgeGrids / sideGrids;
			int subTriSize = gridSizeY / subTri;
			for( int i=1; i<subTri; i++ )
			{
				int newIndex = MakeIndex(x1,y+subTriSize*i);
				fan.push_back(newIndex);
			}

			fan.push_back(indexRightBottom);

			_AppendTriFan(fan,indices);
		}
		else
		{
			indices.push_back(indexCenter);	
			indices.push_back(indexRightTop);
			indices.push_back(indexRightBottom);
		}
	}



	///////////////////////////////////////////////////////////////////////////////////////////
	// class TPatchTriangulationDynamic
	//
	int TPatchTriangulationDynamic::m_sideGrids[NUM_LOD]={16,8,4};
	int TPatchTriangulationDynamic::m_sideVerts[NUM_LOD]={33,33,33};

	TPatchTriangulationDynamic::TPatchTriangulationDynamic(void)
	{
	}

	TPatchTriangulationDynamic::~TPatchTriangulationDynamic(void)
	{
	}

	void TPatchTriangulationDynamic::Create()
	{
		for(int i=0;i<NUM_LOD;i++)
			m_lods[i].Create(this,i);
	}

	void TPatchTriangulationDynamic::Destroy()
	{
		for(int i=0;i<NUM_LOD;i++)
			m_lods[i].Destroy();
	}

	IIndexBuffer* TPatchTriangulationDynamic::GetIB_HW(int lod,int leftLod,int topLod,int rightLod,int bottomLod,int& numVerts,int& numFaces)
	{
		ASSERT(lod>=0 && lod<NUM_LOD);
		ASSERT(leftLod>=0 && leftLod<NUM_LOD);
		ASSERT(topLod>=0 && topLod<NUM_LOD);
		ASSERT(rightLod>=0 && rightLod<NUM_LOD);
		ASSERT(bottomLod>=0 && bottomLod<NUM_LOD);

		numVerts=m_sideVerts[lod]*m_sideVerts[lod];

		return m_lods[lod].GetIB_HW(leftLod,topLod,rightLod,bottomLod,numFaces);
	}

	const vector<WORD>& TPatchTriangulationDynamic::GetIB(int lod,int leftLod,int topLod,int rightLod,int bottomLod)
	{
		ASSERT(lod>=0 && lod<NUM_LOD);
		ASSERT(leftLod>=0 && leftLod<NUM_LOD);
		ASSERT(topLod>=0 && topLod<NUM_LOD);
		ASSERT(rightLod>=0 && rightLod<NUM_LOD);
		ASSERT(bottomLod>=0 && bottomLod<NUM_LOD);

		return m_lods[lod].GetIB(leftLod,topLod,rightLod,bottomLod);
	}

	TPatchTriangulationDynamic* TPatchTriangulationDynamic::Inst()
	{
		static TPatchTriangulationDynamic Inst;
		return &Inst;
	}
	

}//end namespace Cool3D