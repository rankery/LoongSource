#include "StdAfx.h"
#include ".\SortByMtlAndZVal.h"
#include "..\RenderSys\RenderNode.h"
#include "..\SceneGraph\SceneNode.h"

namespace Cool3D
{
	//--class SortByMtlAndZVal::GroupByTrait2----
	class SortByMtlAndZVal::GroupByTrait2
	{
	public:
		DWORD m_trait2;
		float m_minZVal;
		vector<RenderItem> m_items;

		GroupByTrait2()
		{
			m_minZVal=FLOAT_MAX;
		}

		void AddItem(RenderItem& item)
		{
			m_items.push_back(item);

			if(item.zVal<m_minZVal)
				m_minZVal=item.zVal;
		}

		static bool SortFun(const RenderItem& item1,const RenderItem& item2)
		{
			return item1.zVal<item2.zVal;
		}

		void Sort()
		{
			sort(m_items.begin(),m_items.end(),SortFun);
		}

		void GetItems(vector<RenderItem>& items)
		{
			for(int i=0;i<(int)m_items.size();i++)
				items.push_back(m_items[i]);
		}
	};

	//--class SortByMtlAndZVal::GroupByTrait1-----
	class SortByMtlAndZVal::GroupByTrait1
	{
	public:
		DWORD m_trait1;
		float m_minZVal;
		vector<GroupByTrait2*> m_groups;

		GroupByTrait1()
		{
			m_minZVal=FLOAT_MAX;
		}

		~GroupByTrait1()
		{
			for(int i=0;i<(int)m_groups.size();i++)
			{
				GroupByTrait2* p=m_groups[i];
				delete p;
			}
		}

		void AddItem(RenderItem& item)
		{
			bool bFind=false;
			int i;
			for(i=0;i<(int)m_groups.size();i++)
			{
				if(m_groups[i]->m_trait2==item.pNode->GetMtlTrait2(item.pSGNode))
				{
					bFind=true;
					break;
				}
			}
			if(bFind)
			{
				m_groups[i]->AddItem(item);
			}
			else
			{
				GroupByTrait2* pGroup=new GroupByTrait2;
				pGroup->m_trait2=item.pNode->GetMtlTrait2(item.pSGNode);
				pGroup->AddItem(item);
				m_groups.push_back(pGroup);
			}

			//--
			if(item.zVal<m_minZVal)
				m_minZVal=item.zVal;
		}

		static bool SortFun(const GroupByTrait2* p1,const GroupByTrait2* p2)
		{
			return p1->m_minZVal<p2->m_minZVal;
		}

		void Sort()
		{
			for(int i=0;i<(int)m_groups.size();i++)
				m_groups[i]->Sort();
			sort(m_groups.begin(),m_groups.end(),SortFun);
		}

		void GetItems(vector<RenderItem>& items)
		{
			for(int i=0;i<(int)m_groups.size();i++)
				m_groups[i]->GetItems(items);
		}
	};

	//--class SortByMtlAndZVal::GroupByPri--------
	class SortByMtlAndZVal::GroupByPri
	{
	public:
		int m_pri;
		vector<GroupByTrait1*> m_groups;

		~GroupByPri()
		{
			for(int i=0;i<(int)m_groups.size();i++)
			{
				GroupByTrait1* p=m_groups[i];
				delete p;
			}
		}

		void AddItem(RenderItem& item)
		{
			bool bFind=false;
			int i;
			for(i=0;i<(int)m_groups.size();i++)
			{
				if(m_groups[i]->m_trait1==item.pNode->GetMtlTrait1(item.pSGNode))
				{
					bFind=true;
					break;
				}
			}
			if(bFind)
			{
				m_groups[i]->AddItem(item);
			}
			else
			{
				GroupByTrait1* pGroup=new GroupByTrait1;
				pGroup->m_trait1=item.pNode->GetMtlTrait1(item.pSGNode);
				pGroup->AddItem(item);
				m_groups.push_back(pGroup);
			}
		}

		static bool SortFun(const GroupByTrait1* p1,const GroupByTrait1* p2)
		{
			return p1->m_minZVal<p2->m_minZVal;
		}

		void Sort()
		{
			for(int i=0;i<(int)m_groups.size();i++)
				m_groups[i]->Sort();
			sort(m_groups.begin(),m_groups.end(),SortFun);
		}

		void GetItems(vector<RenderItem>& items)
		{
			for(int i=0;i<(int)m_groups.size();i++)
				m_groups[i]->GetItems(items);
		}
	};

	//--class SortByMtlAndZVal::Sorter--------------
	class SortByMtlAndZVal::Sorter
	{
	public:
		vector<GroupByPri*> m_groups;

		~Sorter()
		{
			for(int i=0;i<(int)m_groups.size();i++)
			{
				GroupByPri* p=m_groups[i];
				delete p;
			}
		}

		void CalcZVal(RenderItem& item,const Matrix4& matView)
		{
			const AABBox& box=item.pNode->GetAABBox();
			Vector3 c=(box.max+box.min)*0.5f;
			Vector4 t1,t2;
			D3DXVec3Transform(&t1,&c,&item.pSGNode->GetWorldMat());
			D3DXVec4Transform(&t2,&t2,&matView);
			item.zVal=t2.z;
		}
		
		void AddItem(RenderItem& item,const Matrix4& matView)
		{
			//--
			CalcZVal(item,matView);

			//--
			bool bFind=false;
			int i;
			for(i=0;i<(int)m_groups.size();i++)
			{
				if(m_groups[i]->m_pri==item.pNode->GetPriority())
				{
					bFind=true;
					break;
				}
			}
			if(bFind)
			{
				m_groups[i]->AddItem(item);
			}
			else
			{
				GroupByPri* pGroup=new GroupByPri;
				pGroup->m_pri=item.pNode->GetPriority();
				pGroup->AddItem(item);
				m_groups.push_back(pGroup);
			}
		}

		void AddItems(vector<RenderItem>& items,const Matrix4& matView)
		{
			for(int i=0;i<(int)items.size();i++)
				AddItem(items[i],matView);
		}

		static bool SortFun(const GroupByPri* p1,const GroupByPri* p2)
		{
			return p1->m_pri>p2->m_pri;
		}

		void Sort()
		{
			for(int i=0;i<(int)m_groups.size();i++)
				m_groups[i]->Sort();
			sort(m_groups.begin(),m_groups.end(),SortFun);
		}

		void GetItems(vector<RenderItem>& items)
		{
			items.clear();
			for(int i=0;i<(int)m_groups.size();i++)
				m_groups[i]->GetItems(items);
		}

	};


	//--class SortByMtlAndZVal---------------------
	void SortByMtlAndZVal::Sort(vector<RenderItem>& items,const Matrix4& matView)
	{
		if(items.empty())
			return;

		Sorter sorter;
		sorter.AddItems(items,matView);
		sorter.Sort();
		sorter.GetItems(items);
	}


}//namespace Cool3D