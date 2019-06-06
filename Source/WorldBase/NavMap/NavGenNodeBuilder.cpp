#include "stdafx.h"
#include "NavGenNodeBuilder.h"

namespace WorldBase
{
	const int MAX_SIZE=120000;
	list<AABBox> boxList[MAX_SIZE];
	int	NavGenNodeBuilder::m_idHold=0;

	NavGenNodeBuilder::NavGenNodeBuilder()
		: m_epsilon(20.0f)
	{
		m_boxSize.x=50.0f;
		m_boxSize.y=50.0f;
		m_boxSize.z=50.0f;
	}

	NavGenNodeBuilder::~NavGenNodeBuilder()
	{

	}

	void NavGenNodeBuilder::Build( vector<NavGenBuilder::tagNode>& nodes,vector<AABBox>& boxes,int id,int lv,NavGenBuilder::EBoxSize boxSize )
	{
		if( nodes.size()==0 )
			return;

		if( boxSize==NavGenBuilder::EBS_Common )
		{
			m_boxSize.x	= 50.0f;
			m_boxSize.y	= 50.0f;
			m_boxSize.z	= 50.0f;
			m_epsilon	= 20.0f;
		}
		else if( boxSize==NavGenBuilder::EBS_Small )
		{
			m_boxSize.x	= 25.0f;
			m_boxSize.y	= 25.0f;
			m_boxSize.z	= 25.0f;
			m_epsilon	= 10.0f;
		}
		else
		{
			m_boxSize.x	= 100.0f;
			m_boxSize.y	= 100.0f;
			m_boxSize.z	= 100.0f;
			m_epsilon	= 40.0f;
		}

		//预划分子盒子集，进行合并，然后进行八叉树划分
		//--生成包裹盒
		m_box.max=Vector3(-FLOAT_MAX,-FLOAT_MAX,-FLOAT_MAX);
		m_box.min=Vector3(FLOAT_MAX,FLOAT_MAX,FLOAT_MAX);
		for(UINT index=0;index<nodes.size();index++)
			MergeNode(nodes[index]);
		m_box.min-=Vector3(100.0f,100.0f,100.0f);
		m_box.max+=Vector3(100.0f,100.0f,100.0f);

		Prepare(nodes,boxes,m_box);
		MergeOctreeBoxes(boxes);

		vector<tagNode> mergedNodes;
		mergedNodes.resize(boxes.size());
		for(UINT i=0;i<mergedNodes.size();i++)
		{
			mergedNodes[i].box=boxes[i];
			mergedNodes[i].id=i;
		}

		//--计算出包裹这些scene nodes的盒子
		ComputeBoxForNodes(mergedNodes,m_box);

		BuildOctree(mergedNodes,id,lv,m_box);
	}

	bool NavGenNodeBuilder::BoxIfNeedSplit( const AABBox& box )
	{
		return (box.max.x-box.min.x>m_boxSize.x
			&& box.max.y-box.min.y>m_boxSize.y
			&& box.max.z-box.min.z>m_boxSize.z);
	}

	bool NavGenNodeBuilder::NodeIfNeedSplit( UINT numNodes )
	{
		//return numNodes>10 && m_lv<4;
		return numNodes>10 && m_lv<10;
	}

	void NavGenNodeBuilder::BuildChildBoxes( const AABBox& parentBox,AABBox *pBoxes,int numBoxes )
	{
		float xSize=parentBox.max.x-parentBox.min.x;
		float ySize=parentBox.max.y-parentBox.min.y;
		float zSize=parentBox.max.z-parentBox.min.z;
		xSize*=0.5f;
		ySize*=0.5f;
		zSize*=0.5f;

		AABBox *pBox=pBoxes+0;
		pBox->min.x=parentBox.min.x;
		pBox->max.x=parentBox.max.x-xSize;
		pBox->min.y=parentBox.min.y;
		pBox->max.y=parentBox.max.y-ySize;
		pBox->min.z=parentBox.min.z+zSize;
		pBox->max.z=parentBox.max.z;

		pBox=pBoxes+1;
		pBox->min.x=parentBox.min.x+xSize;
		pBox->max.x=parentBox.max.x;
		pBox->min.y=parentBox.min.y;
		pBox->max.y=parentBox.max.y-ySize;
		pBox->min.z=parentBox.min.z+zSize;
		pBox->max.z=parentBox.max.z;

		pBox=pBoxes+2;
		pBox->min.x=parentBox.min.x+xSize;
		pBox->max.x=parentBox.max.x;
		pBox->min.y=parentBox.min.y;
		pBox->max.y=parentBox.max.y-ySize;
		pBox->min.z=parentBox.min.z;
		pBox->max.z=parentBox.max.z-zSize;

		pBox=pBoxes+3;
		pBox->min.x=parentBox.min.x;
		pBox->max.x=parentBox.max.x-xSize;
		pBox->min.y=parentBox.min.y;
		pBox->max.y=parentBox.max.y-ySize;
		pBox->min.z=parentBox.min.z;
		pBox->max.z=parentBox.max.z-zSize;

		pBox=pBoxes+4;
		pBox->min.x=parentBox.min.x;
		pBox->max.x=parentBox.max.x-xSize;
		pBox->min.y=parentBox.min.y+ySize;
		pBox->max.y=parentBox.max.y;
		pBox->min.z=parentBox.min.z+zSize;
		pBox->max.z=parentBox.max.z;

		pBox=pBoxes+5;
		pBox->min.x=parentBox.min.x+xSize;
		pBox->max.x=parentBox.max.x;
		pBox->min.y=parentBox.min.y+ySize;
		pBox->max.y=parentBox.max.y;
		pBox->min.z=parentBox.min.z+zSize;
		pBox->max.z=parentBox.max.z;

		pBox=pBoxes+6;
		pBox->min.x=parentBox.min.x+xSize;
		pBox->max.x=parentBox.max.x;
		pBox->min.y=parentBox.min.y+ySize;
		pBox->max.y=parentBox.max.y;
		pBox->min.z=parentBox.min.z;
		pBox->max.z=parentBox.max.z-zSize;

		pBox=pBoxes+7;
		pBox->min.x=parentBox.min.x;
		pBox->max.x=parentBox.max.x-xSize;
		pBox->min.y=parentBox.min.y+ySize;
		pBox->max.y=parentBox.max.y;
		pBox->min.z=parentBox.min.z;
		pBox->max.z=parentBox.max.z-zSize;
	}

	const AABBox& NavGenNodeBuilder::MergeNode( const NavGenBuilder::tagNode& node )
	{
		if(node.box.max.x > m_box.max.x)
			m_box.max.x=node.box.max.x;
		if(node.box.max.y > m_box.max.y)
			m_box.max.y=node.box.max.y;
		if(node.box.max.z > m_box.max.z)
			m_box.max.z=node.box.max.z;

		if(node.box.min.x < m_box.min.x)
			m_box.min.x=node.box.min.x;
		if(node.box.min.y < m_box.min.y)
			m_box.min.y=node.box.min.y;
		if(node.box.min.z < m_box.min.z)
			m_box.min.z=node.box.min.z;

		return m_box;
	}

	void NavGenNodeBuilder::Prepare( vector<NavGenBuilder::tagNode>& nodes,vector<AABBox>& mergedBoxes,AABBox& parentBox )
	{
		//--划分子盒子
		AABBox subBox[8];
		BuildChildBoxes(parentBox,subBox,8);

		//--分配三角形给子盒子
		vector<NavGenBuilder::tagNode> subNodes[8];
		UINT i;
		for(i=0;i<nodes.size();i++)
		{
			for(int j=0;j<8;j++)
			{
				if(subBox[j].IntersectsTriangle(nodes[i].a,nodes[i].b,nodes[i].c))
				{
					subNodes[j].push_back(nodes[i]);
				}
			}
		}

		//--判断子盒子大小，决定是否继续划分
		if(!BoxIfNeedSplit(subBox[0]))
		{
			for(i=0;i<8;i++)
			{
				if(subNodes[i].size()>0)
				{
					AABBox& sub=subBox[i];
					AABBox tempFace;
					tempFace.max=Vector3(-FLOAT_MAX,-FLOAT_MAX,-FLOAT_MAX);
					tempFace.min=Vector3(FLOAT_MAX,FLOAT_MAX,FLOAT_MAX);
					for(UINT faceIndex=0;faceIndex<subNodes[i].size();faceIndex++)
					{
						tempFace.Merge(subNodes[i][faceIndex].box);
					}

					AABBox tempBox;
					sub.Intersects(tempFace,tempBox);

					mergedBoxes.push_back(tempBox);
				}
			}
			return;
		}

		//--递归的创建子节点
		for(i=0;i<8;i++)
		{
			if(subNodes[i].size()>0)
			{
				Prepare(subNodes[i],mergedBoxes,subBox[i]);
			}
		}
	}

	void NavGenNodeBuilder::BuildOctree( vector<tagNode>& nodes,int id,int level,const AABBox& box )
	{
		if(nodes.size()==0)
			return;

		NavGenNode::m_id=id;
		NavGenNode::m_lv=level;
		m_box=box;

		//--判断节点是否继续划分
		if(NodeIfNeedSplit(nodes.size()))
		{
			////--划分子盒子
			//AABBox subBox[8];
			//SplitBalanceBox(nodes,subBox);

			////--分配节点给子盒子
			//vector<tagNode> subNodes[8];
			//size_t i=0;
			//for(i=0;i<nodes.size();i++)
			//{
			//	for(int j=0;j<8;j++)
			//	{
			//		if(subBox[j].Intersects(nodes[i].box))
			//		{
			//			subNodes[j].push_back(nodes[i]);
			//		}
			//	}
			//}

			////--递归的创建子节点
			//for(i=0;i<8;i++)
			//{
			//	if(subNodes[i].size()>0)
			//	{
			//		NavGenNodeBuilder *pNewNode=new NavGenNodeBuilder;
			//		pNewNode->BuildOctree(subNodes[i],NexID(),m_lv+1,subBox[i]);
			//		m_pChildren[i]=pNewNode;
			//		m_childrenID[i]=true;
			//	}
			//}

			//计算分割面
			AABBox splitBox[2];
			Vector4 plane=SelectSplitPlane(nodes,splitBox);

			//划分节点
			UINT i=0;
			vector<tagNode> subNodes[2];
			for(i=0;i<nodes.size();i++)
			{
				if(splitBox[0].Intersects(nodes[i].box))
				{
					subNodes[0].push_back(nodes[i]);
				}
				if(splitBox[1].Intersects(nodes[i].box))
				{
					subNodes[1].push_back(nodes[i]);
				}
			}

			//递归
			for(i=0;i<2;i++)
			{
				if(subNodes[i].size()>0)
				{
					NavGenNodeBuilder *pNode=new NavGenNodeBuilder;
					pNode->BuildOctree(subNodes[i],NexID(),m_lv+1,splitBox[i]);
					m_pChildren[i]=pNode;
					m_childrenID[i]=true;
				}
			}
		}
		else
		{
			for(UINT i=0;i<nodes.size();i++)
				m_content.push_back(nodes[i].id);
		}
	}

	void NavGenNodeBuilder::ComputeBoxForNodes( vector<tagNode>& nodes,AABBox& box )
	{
		box.max=Vector3(-FLOAT_MAX,-FLOAT_MAX,-FLOAT_MAX);
		box.min=Vector3(FLOAT_MAX,FLOAT_MAX,FLOAT_MAX);

		for(UINT i=0;i<nodes.size();i++)
		{
			box.Merge(nodes[i].box);
		}
	}

	int NavGenNodeBuilder::CalSplitPlaneScore( vector<tagNode>& nodes,Vector4 plane )
	{
		int front=0,back=0;

		for(size_t i=0;i<nodes.size();i++)
		{
			AABBox& box=nodes[i].box;
			Vector3 p=box.GetCenter();
			float d=plane.x*p.x+plane.y*p.y+plane.z*p.z+plane.w;
			if(d>0)
				front++;
			else
				back++;
		}

		return abs(front-back);
	}

	void NavGenNodeBuilder::SplitBalanceBox( vector<tagNode>& nodes,AABBox splitBox[8] )
	{
		Vector3 size=m_box.max-m_box.min;
		Vector3 p=(m_box.max+m_box.min)*-0.5f;
		const int numTest=64;
		int s;

		//yz plane
		int miniScore=nodes.size();
		int xblanceS=1;
		for(s=1;s<numTest;s++)
		{
			Vector4 plane(1,0,0,m_box.min.x+size.x/numTest*s);
			plane.w*=-1.0f;
			int ss=CalSplitPlaneScore(nodes,plane);//blance
			if(ss<miniScore)
			{
				miniScore=ss;
				xblanceS=s;
			}
		}

		//xz plane
		miniScore=nodes.size();
		int yblanceS=1;
		for(s=1;s<numTest;s++)
		{
			Vector4 plane(0,1,0,m_box.min.y+size.y/numTest*s);
			plane.w*=-1.0f;
			int ss=CalSplitPlaneScore(nodes,plane);//blance
			if(ss<miniScore)
			{
				miniScore=ss;
				yblanceS=s;
			}
		}

		//xy plane
		miniScore=nodes.size();
		int zblanceS=1;
		for(s=1;s<numTest;s++)
		{
			Vector4 plane(0,0,1,m_box.min.z+size.z/numTest*s);
			plane.w*=-1.0f;
			int ss=CalSplitPlaneScore(nodes,plane);//blance
			if(ss<miniScore)
			{
				miniScore=ss;
				zblanceS=s;
			}
		}

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
		box0	min->max	18->13
		*/
		splitBox[0].min=m_box.min;
		splitBox[0].max=Vector3(m_box.min.x+size.x/numTest*xblanceS,
			m_box.min.y+size.y/numTest*yblanceS,
			m_box.min.z+size.z/numTest*zblanceS);
		/*
		box1	min->max	19->14
		*/
		splitBox[1].min=Vector3(m_box.min.x+size.x/numTest*xblanceS,
			m_box.min.y,
			m_box.min.z);
		splitBox[1].max=Vector3(m_box.max.x,
			m_box.min.y+size.y/numTest*yblanceS,
			m_box.min.z+size.z/numTest*zblanceS);
		/*
		box2	min->max	21->16
		*/
		splitBox[2].min=Vector3(m_box.min.x,m_box.min.y,m_box.min.z+size.z/numTest*zblanceS);
		splitBox[2].max=Vector3(m_box.min.x+size.x/numTest*xblanceS,
			m_box.min.y+size.y/numTest*yblanceS,
			m_box.max.z);
		/*
		box3	min->max	22->17
		*/
		splitBox[3].min=Vector3(m_box.min.x+size.x/numTest*xblanceS,
			m_box.min.y,m_box.min.z+size.z/numTest*zblanceS);
		splitBox[3].max=Vector3(m_box.max.x,m_box.min.y+size.y/numTest*yblanceS,m_box.max.z);
		/*
		box4	min->max	9->4
		*/
		splitBox[4].min=Vector3(m_box.min.x,m_box.min.y+size.y/numTest*yblanceS,m_box.min.z);
		splitBox[4].max=Vector3(m_box.min.x+size.x/numTest*xblanceS,
			m_box.max.y,m_box.min.z+size.z/numTest*zblanceS);
		/*
		box5	min->max	10->5
		*/
		splitBox[5].min=Vector3(m_box.min.x+size.x/numTest*xblanceS,
			m_box.min.y+size.y/numTest*yblanceS,m_box.min.z);
		splitBox[5].max=Vector3(m_box.max.x,m_box.max.y,m_box.min.z+size.z/numTest*zblanceS);
		/*
		box6	min->max	12->7
		*/
		splitBox[6].min=Vector3(m_box.min.x,m_box.min.y+size.y/numTest*yblanceS,m_box.min.z+size.z/numTest*zblanceS);
		splitBox[6].max=Vector3(m_box.min.x+size.x/numTest*xblanceS,m_box.max.y,m_box.max.z);
		/*
		box7	min->max	13->8
		*/
		splitBox[7].min=Vector3(m_box.min.x+size.x/numTest*xblanceS,
			m_box.min.y+size.y/numTest*yblanceS,
			m_box.min.z+size.z/numTest*zblanceS);
		splitBox[7].max=m_box.max;
	}

	void NavGenNodeBuilder::MergeOctreeBoxes( vector<AABBox>& octreeBoxes )
	{
		UINT oldBoxNum;
		do 
		{
			oldBoxNum = octreeBoxes.size();
			MergeOctreeBoxesByXAxis(octreeBoxes);
			MergeOctreeBoxesByZAxis(octreeBoxes);
			MergeOctreeBoxesByYAxis(octreeBoxes);
		} while( oldBoxNum != octreeBoxes.size() );
	}

	void NavGenNodeBuilder::MergeOctreeBoxesByXAxis( vector<AABBox>& octreeBoxes )
	{
		int numList=0;

		//--在x方向进行合并
		//--y轴、z轴最小值相等的box放入一个list，且x轴方向长度相等
		for(UINT i=0;i<octreeBoxes.size();i++)
		{
			AABBox& box=octreeBoxes[i];
			bool bInsert=false;
			for(int j=0;j<numList;j++)
			{
				if(boxList[j].size()==0)
					break;
				boxListIter iter=boxList[j].begin();
				if(fabsf((*iter).min.y-box.min.y)<m_epsilon
					&& fabsf((*iter).min.z-box.min.z)<m_epsilon
					&& fabsf((*iter).max.y-box.max.y)<m_epsilon
					&& fabsf((*iter).max.z-box.max.z)<m_epsilon)
				{
					boxList[j].push_back(box);
					bInsert=true;
					break;
				}
			}
			//--新建一个List
			if(!bInsert)
			{
				boxList[numList++].push_back(box);
			}
		}

		for(int i=0;i<MAX_SIZE;i++)
		{
			if(boxList[i].size()==0)
				break;

			int numBoxes=0;
			do 
			{
				numBoxes=boxList[i].size();

				//每个前置节点与后置节点进行合并，直到最后一个节点
				boxListIter iter=boxList[i].begin();
				for(;iter!=boxList[i].end();++iter)
				{
					boxListIter behind=iter;
					for(++behind;behind!=boxList[i].end();)
					{
						AABBox& preNode=(*iter);
						AABBox& nexNode=(*behind);
						//如果两个节点的盒子相邻则合并，并且删除后置节点
						if(fabsf(preNode.min.x-nexNode.max.x)<m_epsilon
							|| fabsf(preNode.max.x-nexNode.min.x)<m_epsilon)
						{
							preNode.Merge(nexNode);
							behind=boxList[i].erase(behind);
							continue;
						}
						++behind;
					}
				}
			} while (numBoxes!=boxList[i].size());
		}

		//把合并后的box重新放入octreeBoxes
		octreeBoxes.clear();
		for(int i=0;i<MAX_SIZE;i++)
		{
			if(boxList[i].size()==0)
				break;

			for(boxListIter iter=boxList[i].begin();iter!=boxList[i].end();++iter)
			{
				octreeBoxes.push_back(*iter);
			}
		}

		for(int i=0;i<MAX_SIZE;i++)
		{
			if(boxList[i].size()==0)
				break;

			boxList[i].clear();
		}
	}

	void NavGenNodeBuilder::MergeOctreeBoxesByZAxis( vector<AABBox>& octreeBoxes )
	{
		int numList=0;

		//在z方向进行合并，这是盒子已经并不是同等大小了，需要判断盒子的size决定是否可以合并
		for(UINT i=0;i<octreeBoxes.size();i++)
		{
			AABBox& box=octreeBoxes[i];
			bool bInsert=false;
			for(int j=0;j<numList;j++)
			{
				if(boxList[j].size()==0)
					break;
				boxListIter iter=boxList[j].begin();
				if(fabsf((*iter).min.y-box.min.y)<m_epsilon
					&& fabsf((*iter).min.x-box.min.x)<m_epsilon
					&& fabsf((*iter).max.y-box.max.y)<m_epsilon
					&& fabsf((*iter).max.x-box.max.x)<m_epsilon)
				{
					boxList[j].push_back(box);
					bInsert=true;
					break;
				}
			}
			//--新建一个List
			if(!bInsert)
			{
				boxList[numList++].push_back(box);
			}
		}

		for(int i=0;i<MAX_SIZE;i++)
		{
			if(boxList[i].size()==0)
				break;

			int numBoxes=0;
			do 
			{
				numBoxes=boxList[i].size();

				//每个前置节点与后置节点进行合并，直到最后一个节点
				boxListIter iter=boxList[i].begin();
				for(;iter!=boxList[i].end();++iter)
				{
					boxListIter behind=iter;
					for(++behind;behind!=boxList[i].end();)
					{
						//如果两个节点的盒子相邻则合并，并且删除后置节点
						AABBox& preNode=*iter;
						AABBox& nexNode=*behind;
						if(fabsf(preNode.min.z-nexNode.max.z)<m_epsilon
							|| fabsf(preNode.max.z-nexNode.min.z)<m_epsilon)
						{
							preNode.Merge(nexNode);
							behind=boxList[i].erase(behind);
							continue;
						}
						++behind;
					}
				}
			} while (numBoxes!=boxList[i].size());
		}

		//把合并后的box重新放入octreeBoxes
		octreeBoxes.clear();
		for(int i=0;i<MAX_SIZE;i++)
		{
			if(boxList[i].size()==0)
				break;

			for(boxListIter iter=boxList[i].begin();iter!=boxList[i].end();++iter)
			{
				octreeBoxes.push_back(*iter);
			}
		}

		for(int i=0;i<MAX_SIZE;i++)
		{
			if(boxList[i].size()==0)
				break;

			boxList[i].clear();
		}
	}

	void NavGenNodeBuilder::MergeOctreeBoxesByYAxis( vector<AABBox>& octreeBoxes )
	{
		int numList=0;

		//在y方向进行合并
		for(UINT i=0;i<octreeBoxes.size();i++)
		{
			AABBox& box=octreeBoxes[i];
			bool bInsert=false;
			for(int j=0;j<numList;j++)
			{
				if(boxList[j].size()==0)
					break;
				boxListIter iter=boxList[j].begin();
				if(fabsf((*iter).min.z-box.min.z)<m_epsilon
					&& fabsf((*iter).min.x-box.min.x)<m_epsilon
					&& fabsf((*iter).max.z-box.max.z)<m_epsilon
					&& fabsf((*iter).max.x-box.max.x)<m_epsilon)
				{
					boxList[j].push_back(box);
					bInsert=true;
					break;
				}
			}
			//--新建一个List
			if(!bInsert)
			{
				boxList[numList++].push_back(box);
			}
		}

		for(int i=0;i<MAX_SIZE;i++)
		{
			if(boxList[i].size()==0)
				break;

			int numBoxes=0;
			do 
			{
				numBoxes=boxList[i].size();

				//每个前置节点与后置节点进行合并，直到最后一个节点
				boxListIter iter=boxList[i].begin();
				for(;iter!=boxList[i].end();++iter)
				{
					boxListIter behind=iter;
					for(++behind;behind!=boxList[i].end();)
					{
						//如果两个节点的盒子相邻则合并，并且删除后置节点
						AABBox& preNode=*iter;
						AABBox& nexNode=*behind;
						if(fabsf(preNode.min.y-nexNode.max.y)<m_epsilon
							|| fabsf(preNode.max.y-nexNode.min.y)<m_epsilon)
						{
							preNode.Merge(nexNode);
							behind=boxList[i].erase(behind);
							continue;
						}
						++behind;
					}
				}
			} while (numBoxes!=boxList[i].size());
		}

		//把合并后的box重新放入octreeBoxes
		octreeBoxes.clear();
		for(int i=0;i<MAX_SIZE;i++)
		{
			if(boxList[i].size()==0)
				break;

			for(boxListIter iter=boxList[i].begin();iter!=boxList[i].end();++iter)
			{
				octreeBoxes.push_back(*iter);
			}
		}

		for(int i=0;i<MAX_SIZE;i++)
		{
			if(boxList[i].size()==0)
				break;

			boxList[i].clear();
		}
	}

	Cool3D::Vector4 NavGenNodeBuilder::SelectSplitPlane( vector<tagNode>& nodes,AABBox splitBox[2] )
	{
		Vector4 retPlane(0,1,0,0);
		Vector3 size=m_box.max-m_box.min;
		const int numTest=64;
		TCHAR axis;

		//找到主轴
		if(size.x>size.y)
		{
			if(size.x>size.z)
				axis=_T('x');
			else
				axis=_T('z');
		}
		else
		{
			if(size.y>size.z)
				axis=_T('y');
			else
				axis=_T('z');
		}

		switch(axis)
		{
		case _T('x')://yz plane
			{
				int miniScore=nodes.size();
				int blanceS=1;
				for(int s=1;s<numTest;s++)
				{
					Vector4 plane(1,0,0,m_box.min.x+size.x/numTest*s);
					plane.w*=-1.0f;
					int ss=CalSplitPlaneScore(nodes,plane);//blance
					if(ss<miniScore)
					{
						retPlane=plane;
						miniScore=ss;
						blanceS=s;
					}
				}
				splitBox[0].min=m_box.min;
				splitBox[0].max=Vector3(m_box.min.x+size.x/numTest*blanceS,m_box.max.y,m_box.max.z);
				splitBox[1].min=Vector3(m_box.min.x+size.x/numTest*blanceS,m_box.min.y,m_box.min.z);
				splitBox[1].max=m_box.max;
			}
			break;
		case _T('y'):
			{
				int miniScore=nodes.size();
				int blanceS=1;
				for(int s=1;s<numTest;s++)
				{
					Vector4 plane(0,1,0,m_box.min.y+size.y/numTest*s);
					plane.w*=-1.0f;
					int ss=CalSplitPlaneScore(nodes,plane);//blance
					if(ss<miniScore)
					{
						retPlane=plane;
						miniScore=ss;
						blanceS=s;
					}
				}
				splitBox[0].min=m_box.min;
				splitBox[0].max=Vector3(m_box.max.x,m_box.min.y+size.y/numTest*blanceS,m_box.max.z);
				splitBox[1].min=Vector3(m_box.min.x,m_box.min.y+size.y/numTest*blanceS,m_box.min.z);
				splitBox[1].max=m_box.max;
			}
			break;
		case _T('z'):
			{
				int miniScore=nodes.size();
				int blanceS=1;
				for(int s=1;s<numTest;s++)
				{
					Vector4 plane(0,0,1,m_box.min.z+size.z/numTest*s);
					plane.w*=-1.0f;
					int ss=CalSplitPlaneScore(nodes,plane);//blance
					if(ss<miniScore)
					{
						retPlane=plane;
						miniScore=ss;
						blanceS=s;
					}
				}
				splitBox[0].min=m_box.min;
				splitBox[0].max=Vector3(m_box.max.x,m_box.max.y,m_box.min.z+size.z/numTest*blanceS);
				splitBox[1].min=Vector3(m_box.min.x,m_box.min.y,m_box.min.z+size.z/numTest*blanceS);
				splitBox[1].max=m_box.max;
			}
			break;
		}

		return retPlane;
	}
}