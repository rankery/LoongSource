#pragma once

namespace Cool3D
{
	/** \class fast_vector
		\brief ¼ò»¯µÄvectorÀà
	*/
	template<class _Type>
	class fast_vector
	{
		int m_capacity;
		int m_size;
		_Type* m_pdata;
	public:
		fast_vector(void)
		{
			m_capacity=0;
			m_size=0;
			m_pdata=NULL;
		}
		fast_vector(int capacity)
		{
			m_capacity=capacity;
			m_size=0;
			m_pdata=new _Type[capacity];
		}
		~fast_vector(void)
		{
			if(m_pdata!=NULL)
			{
				delete[] m_pdata;
				m_pdata=NULL;
			}
		}

		int size(){ return m_size;}
		int capacity(){ return m_capacity;}

		void push_back(const _Type& value)
		{
			if(m_size+1>m_capacity)
				grow(m_size+1);

			m_pdata[m_size]=value;
			m_size++;
		}

		void pop_back()
		{
			if(m_size>0)
				m_size--;
		}

		void resize(int new_size)
		{
			assert(new_size>=0);
			if(new_size>m_capacity)
				grow(new_size);
			m_size=new_size;
		}

		void clear()
		{
			m_size=0;
		}

		_Type& operator[](int index)
		{
			assert(index>=0 && index<m_size);
			return m_pdata[index];
		}

	private:
		void grow(int new_size)
		{
			assert(new_size>0);
			int new_capacity=int(new_size*1.5f);
			if(m_pdata==NULL)
				m_pdata=new _Type[new_capacity];
			else
			{
				_Type* pbuf=new _Type[new_capacity];
				memcpy(pbuf,m_pdata,m_size*sizeof(_Type));
				delete[] m_pdata;
				m_pdata=pbuf;
			}
			m_capacity=new_capacity;
		}

	};
}//namespace Cool3D