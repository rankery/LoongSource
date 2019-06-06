
#pragma once

namespace Cool3D
{
	/**	\class FreePtrVector
		\brief delete一个vector中的所有指针, 并且清空此vector
	*/
	template<class T>
		class FreePtrVector
	{
	public:
		void operator()(vector<T*>& array)
		{
			for(size_t i=0;i<array.size();i++)
			{
				T* pObj=array[i];
				delete pObj;
			}
			array.clear();
		}
	};
}//namespace Cool3D