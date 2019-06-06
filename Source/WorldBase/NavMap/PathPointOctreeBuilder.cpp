#include "StdAfx.h"
#include "PathPointOctreeBuilder.h"
#include "PathPointOctreeNode.h"

namespace WorldBase
{
	namespace
	{
		struct tagPoint 
		{
			Vector3	pos;
			int		id;
		};
		typedef std::vector<tagPoint> PathPointList;
	}

	//---------------------------class PathPointOctreeNodeBuilder------------------
	class PathPointOctreeNodeBuilder : public PathPointOctreeNode
	{
	public:
		PathPointOctreeNodeBuilder(void);

		void WriteFile(FILE *fp);
		void BuildChildBoxes(const AABBox& parentBox,AABBox *pBoxes);
		void Build(const PathPointList& pathPoints,int depth,const AABBox& box);

	private:
		bool	m_bChildren[8];
		int		m_maxDepth;
	};

	PathPointOctreeNodeBuilder::PathPointOctreeNodeBuilder( void )
		: m_maxDepth(6)
	{
		memset(m_bChildren,0,sizeof(m_bChildren));
	}

	void PathPointOctreeNodeBuilder::WriteFile( FILE *fp )
	{
		ASSERT(fp!=NULL);

		FWriteValue(fp,m_box.max);
		FWriteValue(fp,m_box.min);
		FWriteValVector(fp,m_content);

		for(int i=0;i<8;i++)
		{
			FWriteValue(fp,m_bChildren[i]);
			if(m_bChildren[i])
			{
				PathPointOctreeNodeBuilder *pNode=static_cast<PathPointOctreeNodeBuilder*>(m_pChildren[i]);
				pNode->WriteFile(fp);
			}
		}
	}

	void PathPointOctreeNodeBuilder::BuildChildBoxes( const AABBox& parentBox,AABBox *pBoxes )
	{
		float xSize=parentBox.max.x-parentBox.min.x;
		float ySize=parentBox.max.y-parentBox.min.y;
		float zSize=parentBox.max.z-parentBox.min.z;
		xSize*=0.5f;
		ySize*=0.5f;
		zSize*=0.5f;

		/*
			z
			/\
			|
			|
			顶部
			6-----7-----8 max
			|	  |		|
			|	  |		|
			3-----4-----5
			|	  |		|
			|	  |		|
			0-----1-----2

			中间
			15----16----17
			|	  |		|
			|	  |		|
			12----13----14
			|	  |		|
			|	  |		|
			9-----10----11

			底部
			24----25----26
			|	  |		|
			|	  |		|
			21----22----23
			|	  |		|
			|	  |		|
		min 18----19----20	----->x
		*/

		/*
			box0	min->max	21->16
		*/
		AABBox *pBox=pBoxes+0;
		pBox->min.x=parentBox.min.x;
		pBox->max.x=parentBox.max.x-xSize;
		pBox->min.y=parentBox.min.y;
		pBox->max.y=parentBox.max.y-ySize;
		pBox->min.z=parentBox.min.z+zSize;
		pBox->max.z=parentBox.max.z;

		/*
			box1	min->max	22->17
		*/
		pBox=pBoxes+1;
		pBox->min.x=parentBox.min.x+xSize;
		pBox->max.x=parentBox.max.x;
		pBox->min.y=parentBox.min.y;
		pBox->max.y=parentBox.max.y-ySize;
		pBox->min.z=parentBox.min.z+zSize;
		pBox->max.z=parentBox.max.z;

		/*
			box2	min->max	19->14
		*/
		pBox=pBoxes+2;
		pBox->min.x=parentBox.min.x+xSize;
		pBox->max.x=parentBox.max.x;
		pBox->min.y=parentBox.min.y;
		pBox->max.y=parentBox.max.y-ySize;
		pBox->min.z=parentBox.min.z;
		pBox->max.z=parentBox.max.z-zSize;

		/*
			box3	min->max	18->13
		*/
		pBox=pBoxes+3;
		pBox->min.x=parentBox.min.x;
		pBox->max.x=parentBox.max.x-xSize;
		pBox->min.y=parentBox.min.y;
		pBox->max.y=parentBox.max.y-ySize;
		pBox->min.z=parentBox.min.z;
		pBox->max.z=parentBox.max.z-zSize;

		/*
			box4	min->max	12->7
		*/
		pBox=pBoxes+4;
		pBox->min.x=parentBox.min.x;
		pBox->max.x=parentBox.max.x-xSize;
		pBox->min.y=parentBox.min.y+ySize;
		pBox->max.y=parentBox.max.y;
		pBox->min.z=parentBox.min.z+zSize;
		pBox->max.z=parentBox.max.z;

		/*
			box5	min->max	13->8
		*/
		pBox=pBoxes+5;
		pBox->min.x=parentBox.min.x+xSize;
		pBox->max.x=parentBox.max.x;
		pBox->min.y=parentBox.min.y+ySize;
		pBox->max.y=parentBox.max.y;
		pBox->min.z=parentBox.min.z+zSize;
		pBox->max.z=parentBox.max.z;

		/*
			box6	min->max	10->5
		*/
		pBox=pBoxes+6;
		pBox->min.x=parentBox.min.x+xSize;
		pBox->max.x=parentBox.max.x;
		pBox->min.y=parentBox.min.y+ySize;
		pBox->max.y=parentBox.max.y;
		pBox->min.z=parentBox.min.z;
		pBox->max.z=parentBox.max.z-zSize;

		/*
			box7	min->max	9->4
		*/
		pBox=pBoxes+7;
		pBox->min.x=parentBox.min.x;
		pBox->max.x=parentBox.max.x-xSize;
		pBox->min.y=parentBox.min.y+ySize;
		pBox->max.y=parentBox.max.y;
		pBox->min.z=parentBox.min.z;
		pBox->max.z=parentBox.max.z-zSize;
	}

	void PathPointOctreeNodeBuilder::Build( const PathPointList& pathPoints,int depth,const AABBox& box )
	{
		if(pathPoints.size()==0)
			return;

		m_box=box;

		//--判断节点是否继续划分
		if(pathPoints.size()>8
			&& depth<m_maxDepth)
		{
			//--划分子盒子
			AABBox subBox[8];
			BuildChildBoxes(box,subBox);

			//--分配节点给子盒子
			PathPointList subNodes[8];
			int i=0;
			for(i=0;i<(int)pathPoints.size();i++)
			{
				for(int j=0;j<8;j++)
				{
					if(subBox[j].Contain(pathPoints[i].pos))
					{
						subNodes[j].push_back(pathPoints[i]);
					}
				}
			}

			//--递归的创建子节点
			for(i=0;i<8;i++)
			{
				if(subNodes[i].size()>0)
				{
					PathPointOctreeNodeBuilder *pNewNode=new PathPointOctreeNodeBuilder;
					pNewNode->Build(subNodes[i],++depth,subBox[i]);
					m_pChildren[i]=pNewNode;
					m_bChildren[i]=true;
				}
			}
		}
		else
		{
			for(UINT i=0;i<pathPoints.size();i++)
				m_content.push_back(pathPoints[i].id);
		}
	}
	//---------------------------class PathPointOctreeNodeBuilder------------------

	//---------------------------class PathPointOctreeBuilder----------------------
	class PathPointOctreeBuilder::Member
	{
	public:
		PathPointList	m_pathPoints;

	public:
		void CaclAABBox(AABBox& box)
		{
			box.max=Vector3(-FLOAT_MAX,-FLOAT_MAX,-FLOAT_MAX);
			box.min=Vector3(FLOAT_MAX,FLOAT_MAX,FLOAT_MAX);

			for(UINT i=0;i<m_pathPoints.size();i++)
			{
				tagPoint& point=m_pathPoints[i];

				if( box.max.x<point.pos.x )
					box.max.x=point.pos.x;
				if( box.max.y<point.pos.y )
					box.max.y=point.pos.y;
				if( box.max.z<point.pos.z )
					box.max.z=point.pos.z;
				if( box.min.x>point.pos.x )
					box.min.x=point.pos.x;
				if( box.min.y>point.pos.y )
					box.min.y=point.pos.y;
				if( box.min.z>point.pos.z )
					box.min.z=point.pos.z;
			}
		}
	};

	PathPointOctreeBuilder::PathPointOctreeBuilder(void)
		: m_pRoot(0)
	{
		m_p=new Member;
	}

	PathPointOctreeBuilder::~PathPointOctreeBuilder(void)
	{
		SAFE_DELETE(m_pRoot);
		SAFE_DELETE(m_p);
	}

	void PathPointOctreeBuilder::Begin( void )
	{
		m_p->m_pathPoints.clear();
	}

	void PathPointOctreeBuilder::AddPoint( const Vector3& point )
	{
		tagPoint tagp;
		tagp.pos=point;
		tagp.id=m_p->m_pathPoints.size();
		m_p->m_pathPoints.push_back(tagp);
	}

	bool PathPointOctreeBuilder::End( void )
	{
		m_pRoot=new PathPointOctreeNodeBuilder;

		AABBox box;
		m_p->CaclAABBox(box);
		m_pRoot->Build(m_p->m_pathPoints,0,box);

		return true;
	}

	bool PathPointOctreeBuilder::Save( const TCHAR *szFilePath )
	{
		::SetCurrentDirectory(Kernel::Inst()->GetWorkPath());

		FILE* fp=_tfopen(szFilePath,_T("wb"));
		if(fp==NULL)
			return false;

		m_pRoot->WriteFile(fp);

		fclose(fp);
		return true;
	}
	//---------------------------class PathPointOctreeBuilder----------------------
}//namespace WorldBase