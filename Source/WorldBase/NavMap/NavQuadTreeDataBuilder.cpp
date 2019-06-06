#include "StdAfx.h"
#include ".\navquadtreedatabuilder.h"
#include "NavSGQuadTreeNode.h"
#include "NavSceneNode.h"
#include "NavEntityNode.h"

namespace WorldBase
{
	class NavQuadTreeNodeBuilder: public NavSGQuadTreeNode
	{
	public:
		NavQuadTreeNodeBuilder():NavSGQuadTreeNode(NULL)
		{}
		virtual ~NavQuadTreeNodeBuilder()
		{
			m_pContent.clear();//自己不管理那些scene node的生存期
		}

		static int  m_childNum;
		static int	m_idHold;
		static int NexID()	{	return ++m_idHold;}

		static int m_depth;//深度
		static int m_numTreeNodes;//四叉树结点数
		static int m_maxSceneNodes;//单个四叉树结点最大scenenode数

	protected:
		static void ComputeBoxForNodes(vector<NavSceneNode *>& sceneNodes,AABBox& box)
		{
			box.max=Vector3(-FLOAT_MAX,-FLOAT_MAX,-FLOAT_MAX);
			box.min=Vector3(FLOAT_MAX,FLOAT_MAX,FLOAT_MAX);

			AABBox nodeBox;
			for(size_t i=0;i<sceneNodes.size();i++)
			{
				NavSceneNode* pNode=sceneNodes[i];
				pNode->GetBox(nodeBox);

				if( nodeBox.max.x > 1000000.0f
					|| nodeBox.max.x < -1000000.0f
					|| nodeBox.max.y > 1000000.0f
					|| nodeBox.max.y < -1000000.0f
					|| nodeBox.max.z > 1000000.0f
					|| nodeBox.max.z < -1000000.0f )
				{
					NavEntityNode *pEntity = (NavEntityNode*)pNode;
					LOG(_T("\nNAVERROR-------%s\n"),pEntity->GetResName());
				}

				box.Merge(nodeBox);
			}
		}

		void BuildChildBoxes(AABBox& sub0,AABBox& sub1,AABBox& sub2,AABBox& sub3)
		{
			/*将m_box在XZ平面切分成4个小box

						--------|--------m_box.max
						|       |		|
						|   0   |	1	|
						|-------|-------|
						|       |		|	   /\Z
						|   3   |	2	|		|
			   m_box.min|_______|_______|		|-->x
			*/

			float xSize=m_box.max.x-m_box.min.x;
			float zSize=m_box.max.z-m_box.min.z;
			xSize*=0.5f;
			zSize*=0.5f;

			sub0.max.y=m_box.max.y;
			sub0.min.y=m_box.min.y;
			sub0.max.x=m_box.min.x+xSize;
			sub0.max.z=m_box.max.z;
			sub0.min.x=m_box.min.x;
			sub0.min.z=m_box.max.z-zSize;

			sub1.max.y=m_box.max.y;
			sub1.min.y=m_box.min.y;
			sub1.max=m_box.max;
			sub1.min.x=sub1.max.x-xSize;
			sub1.min.z=sub1.max.z-zSize;

			sub2.max.y=m_box.max.y;
			sub2.min.y=m_box.min.y;
			sub2.max.x=m_box.max.x;
			sub2.max.z=m_box.max.z-zSize;
			sub2.min.x=sub2.max.x-xSize;
			sub2.min.z=sub2.max.z-zSize;


			sub3.max.y=m_box.max.y;
			sub3.min.y=m_box.min.y;
			sub3.min=m_box.min;
			sub3.max.x=sub3.min.x+xSize;
			sub3.max.z=sub3.min.z+zSize;
		}

		bool IfNeedSplit(int numSceneNodes) const
		{
			return numSceneNodes>4 && m_lv<10;
		}

	public:
		void Build(vector<NavSceneNode *> sceneNodes,int myID,int myLV)
		{
			ASSERT(sceneNodes.size()>0);
			//--
			NavSGQuadTreeNode::m_id=myID;
			NavSGQuadTreeNode::m_lv=myLV;

			if(myLV>m_depth)
				m_depth=myLV;

			m_numTreeNodes++;

			//--计算出包裹这些scene nodes的盒子
			ComputeBoxForNodes(sceneNodes,NavSGQuadTreeNode::m_box);

			
			//--
			if(IfNeedSplit((int)sceneNodes.size()))
			{
				//--将所有的scene nodes按照空间分成四份,分配给children
				AABBox subBox[4];
				BuildChildBoxes(subBox[0],subBox[1],subBox[2],subBox[3]);
				for(int k=0;k<4;k++)//使得盒子变胖一些
				{
					Vector3 vSize=subBox[k].max-subBox[k].min;
					vSize*=0.25f;

					subBox[k].max+=vSize;
					subBox[k].min-=vSize;
				}

				vector<NavSceneNode*> subNodes[4];
				AABBox nodeBox;
				size_t i;
				for(i=0;i<sceneNodes.size();i++)
				{
					sceneNodes[i]->GetBox(nodeBox);
					for(int j=0;j<4;j++)
					{
						if(subBox[j].Contain(nodeBox))
						{
							subNodes[j].push_back(sceneNodes[i]);
							sceneNodes[i]=NULL;
							break;
						}
					}//endof for(j)
				}//end of for each node
				
				//--递归的创建子节点
				for(i=0;i<4;i++)
				{
					if(subNodes[i].size()>0)
					{
						NavQuadTreeNodeBuilder *pNewNode=new NavQuadTreeNodeBuilder;
						pNewNode->Build(subNodes[i],NexID(),m_lv+1);
						m_pChildren[i]=pNewNode;

						m_childrenID[i]=pNewNode->GetID();
						m_childrenBox[i]=pNewNode->GetBox();
					}
				}

				//--没有被子盒子完全包含的放入父节点
				for(i=0;i<sceneNodes.size();i++)
				{
					if(sceneNodes[i]!=NULL)
					{
						m_pContent.push_back(sceneNodes[i]);
						m_childNum++;
					}
				}
			}
			else
			{
				m_childNum+=sceneNodes.size();
				m_pContent=sceneNodes;
			}

			if((int)m_pContent.size()>m_maxSceneNodes)
				m_maxSceneNodes=(int)m_pContent.size();

		}//endof Build()

		void GetChildrenR(vector<NavSGQuadTreeNode*>& out)
		{
			int i=0;
			for(i=0;i<4;i++)
			{
				if(m_pChildren[i])
					out.push_back(m_pChildren[i]);
			}

			for(i=0;i<4;i++)
			{
				NavQuadTreeNodeBuilder *pChild=(NavQuadTreeNodeBuilder*)m_pChildren[i];
				if(pChild)
					pChild->GetChildrenR(out);
			}
		}
	};
	int	NavQuadTreeNodeBuilder::m_idHold=5000;
	int NavQuadTreeNodeBuilder::m_childNum=0;
	int NavQuadTreeNodeBuilder::m_depth=0;
	int NavQuadTreeNodeBuilder::m_numTreeNodes=0;
	int NavQuadTreeNodeBuilder::m_maxSceneNodes=0;

	NavQuadTreeDataBuilder::NavQuadTreeDataBuilder(void)
	{
		m_pRoot=NULL;
	}

	NavQuadTreeDataBuilder::~NavQuadTreeDataBuilder(void)
	{
	}

	void NavQuadTreeDataBuilder::Begin()
	{
		SAFE_DELETE(m_pRoot);
		m_allNode.clear();
	}

	void NavQuadTreeDataBuilder::AddNode(NavSceneNode *pNode)
	{
		m_allNode.push_back(pNode);
	}

	bool NavQuadTreeDataBuilder::End(const TCHAR* szSaveFile)
	{
		if(m_allNode.size()<=0)
		{
			return false;
		}

		NavQuadTreeNodeBuilder::m_depth=0;
		NavQuadTreeNodeBuilder::m_maxSceneNodes=0;
		NavQuadTreeNodeBuilder::m_numTreeNodes=0;

		m_pRoot=new NavQuadTreeNodeBuilder;
		m_pRoot->Build(m_allNode,0,0);
		if( m_pRoot->GetContentSize() == m_allNode.size() && m_allNode.size()>10 )
		{
			LOG(_T("navmap.fsg生成失败：碰撞数据全部在根节点！\n"));
			return false;
		}

		LogTreeInfo();

		bool ret=true;
		if(szSaveFile!=NULL)
			ret=SaveToFile(szSaveFile);
		//--
		delete m_pRoot;

		return ret;
	}

	
	bool NavQuadTreeDataBuilder::SaveToFile(const TCHAR* szFileName)
	{
		if(m_pRoot==NULL)
		{
			return false;
		}

		FArchiveAdv ar;
		try
		{
			ar.OpenForWrite(szFileName);
		}
		catch (...)
		{
			return false;
		}

		//--得到一个节点数组
		vector<NavSGQuadTreeNode*>	allQuadTreeNodes;
		allQuadTreeNodes.push_back(m_pRoot);
		m_pRoot->GetChildrenR(allQuadTreeNodes);

		//--写入文件头
		tagHeader header;
		memset(&header,0,sizeof(header));
		_tcscpy((TCHAR*)header.magic,_T("QTD"));
		header.ver=1;
		header.numItem=allQuadTreeNodes.size();

		FILE *fp=ar.GetWriteFile();
		ASSERT(fp!=NULL);
		FWriteValue(fp,header);

		//--写入Item数组
		header.numItem=allQuadTreeNodes.size();
		header.itemOffset=ftell(fp);
		vector<tagItem>	items;
		items.resize(header.numItem);
		size_t i;
		for(i=0;i<items.size();i++)
		{
			memset(&items[i],0,sizeof(tagItem));
			FWriteValue(fp,items[i]);
		}

		//--写入节点数组,并且记录每个节点的offset到item数组
		for(i=0;i<allQuadTreeNodes.size();i++)
		{
			NavSGQuadTreeNode *pQTN=allQuadTreeNodes[i];
			tagItem& item=items[i];
			item.id=pQTN->GetID();
			item.offset=ftell(fp);
			pQTN->Serialize(ar);
		}

		//--回写Item数组
		fseek(fp,header.itemOffset,SEEK_SET);
		for(i=0;i<items.size();i++)
			FWriteValue(fp,items[i]);
		
		//--回写header
		fseek(fp,0,SEEK_SET);
		FWriteValue(fp,header);

		//--
		ar.Close();
		return true;
	}

	void NavQuadTreeDataBuilder::LogTreeInfo()
	{
		LOG(_T("--NavQuadTree Info-----------------------------------\n"));
		LOG(_T("depth:%d\n"),NavQuadTreeNodeBuilder::m_depth);
		LOG(_T("num tree nodes:%d\n"),NavQuadTreeNodeBuilder::m_numTreeNodes);
		LOG(_T("num scene nodes:%d\n"),m_allNode.size());
		LOG(_T("max scene nodes per tree node:%d\n"),NavQuadTreeNodeBuilder::m_maxSceneNodes);
		LOG(_T("avage scene nodes per tree node:%d\n"),m_allNode.size()/NavQuadTreeNodeBuilder::m_numTreeNodes);
		LOG(_T("--NavQuadTree Info-----------------------------------\n"));

	}
}//namespace WorldBase