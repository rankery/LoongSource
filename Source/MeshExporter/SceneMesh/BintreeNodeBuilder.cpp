#include "BintreeNodeBuilder.h"
#include "..\Math\GeomDefine.h"

BintreeNodeBuilder::BintreeNodeBuilder()
{
	//
}

BintreeNodeBuilder::~BintreeNodeBuilder()
{
	//
}

void BintreeNodeBuilder::Build( vector<BintreeBuilder::tagNode>& nodes,const AABBox& box,int lv )
{
	if(nodes.size()==0)
		return;

	m_lv=lv;
	m_box=box;

	if(IfNeedSplit(nodes))
	{
		//计算分割面
		AABBox splitBox[8];
		SplitBalanceBox(nodes,splitBox);

		//划分节点
		UINT i=0;
		vector<BintreeBuilder::tagNode> subNodes[8];
		for(i=0;i<nodes.size();i++)
		{
			for(int boxIndex=0;boxIndex<8;boxIndex++)
			{
				//splitBox[boxIndex].Intersects(nodes[i].box)
				if(splitBox[boxIndex].IntersectsTriangle(nodes[i].a,nodes[i].b,nodes[i].c))
					subNodes[boxIndex].push_back(nodes[i]);
			}
		}

		//递归
		for(i=0;i<8;i++)
		{
			if(subNodes[i].size()>0)
			{
				BintreeNodeBuilder *pNode=new BintreeNodeBuilder;
				pNode->Build(subNodes[i],splitBox[i],lv+1);
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

bool BintreeNodeBuilder::IfNeedSplit( vector<BintreeBuilder::tagNode>& nodes )
{
	return (nodes.size()>8 && m_lv<5);
}

int BintreeNodeBuilder::CalSplitPlaneScore( vector<BintreeBuilder::tagNode>& nodes,Vector4 plane )
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

void BintreeNodeBuilder::SplitBalanceBox( vector<BintreeBuilder::tagNode>& nodes,AABBox splitBox[8] )
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