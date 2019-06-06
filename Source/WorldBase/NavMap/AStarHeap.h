#pragma once

/** \class AStarHeap
	\brief A*算法二叉小根堆
*/
template<typename T,int MaxSize>
class AStarHeap
{
public:
	AStarHeap(void):m_size(0){}
	~AStarHeap(void){}

	bool push(T val)
	{
		if(m_size+1>=MaxSize)
			return false;

		m_size++;
		if(m_size==1)
		{
			m_heap[m_size]=val;
		}
		else
		{
			//--上溯
			int current=m_size;
			int parent;
			for(;;)
			{
				parent=current/2;
				if(val<m_heap[parent])
				{
					m_heap[current]=m_heap[parent];
					current=parent;
					if(current==1)
						break;
				}
				else
					break;
			}
			m_heap[current]=val;
		}

		return true;
	}
	
	void pop(T& val)
	{
		ASSERT(m_size>0);
		if(m_size==0)
			return;

		val=m_heap[1];

		if(m_size==1)
		{
			m_size--;
		}
		else if(m_size==2)
		{
			m_heap[1]=m_heap[2];
			m_size--;
		}
		else
		{
			//--下沉
			T top=m_heap[m_size];
			m_size--;
			int current=1;
			int child1,child2;
			bool bChild1,bChild2;
			for(;;)
			{
				child1=current*2;
				child2=current*2+1;
				bChild1=child1<=m_size&&m_heap[child1]<top;
				bChild2=child2<=m_size&&m_heap[child2]<top;
				if(bChild1&&bChild2)
				{
					if(m_heap[child1]<m_heap[child2])
					{
						m_heap[current]=m_heap[child1];
						current=child1;
					}
					else
					{
						m_heap[current]=m_heap[child2];
						current=child2;
					}
				}
				else if(bChild1)
				{
					m_heap[current]=m_heap[child1];
					current=child1;
				}
				else if(bChild2)
				{
					m_heap[current]=m_heap[child2];
					current=child2;
				}
				else
					break;
			}
			m_heap[current]=top;
		}
	}

	bool validate(int& errindex)
	{
		int child1,child2;
		for(int i=1;i<m_size;i++)
		{
			child1=i*2;
			if(child1<=m_size&&m_heap[child1]<m_heap[i])
			{
				errindex=i;
				return false;
			}
			child2=i*2+1;
			if(child2<=m_size&&m_heap[child2]<m_heap[i])
			{
				errindex=i;
				return false;
			}
		}

		return true;
	}

	const T& operator[](int index){	return m_heap[index];}

	int size(){ return m_size;}
	void clear(){ m_size=0;}

private:
	T m_heap[MaxSize];//第一个元素空着
	int m_size;
};
