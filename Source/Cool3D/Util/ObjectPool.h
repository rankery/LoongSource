#pragma once
#include "DebugMisc.h"
#include <list>
#include <set>
using namespace std;

namespace Cool3D
{
	template<class ObjType>
	class DefaultObjFactory
	{
	public:
		ObjType* NewObj()	{	return new ObjType;}
		void DelObj(ObjType *pObj)	{	delete pObj; }
	};
	/**	\class ObjectPool
		\brief 对象池,在创建时一次分配n个对象供运行时使用
	*/
	template<class ObjType,class ObjFactory=DefaultObjFactory<ObjType> >
	class ObjectPool
	{
	public:
		ObjectPool(int size)
		{
			m_numAlloced=0;
			m_maxAlloced=0;
			for(int i=0;i<size;i++)
			{
				ObjFactory of;
				ObjType *pObj=of.NewObj();
				m_objs.push_back(pObj);
#ifdef DEBUG
				m_objSet.insert(pObj);
#endif
			}
		}

		virtual ~ObjectPool(void)
		{
			Destroy();
		}

		void Destroy()
		{
			ObjFactory of;
			for(list<ObjType*>::iterator iter=m_objs.begin();
				iter!=m_objs.end();++iter)
			{
				ObjType *pObj=(*iter);
				of.DelObj(pObj);
			}
			m_objs.clear();
#ifdef DEBUG
			m_objSet.clear();
#endif
		}

		ObjType* AllocObj()
		{
			ASSERT(!m_objs.empty() && "Obj pool overflow");
			if(m_objs.empty())
				return NULL;
			else
			{
				ObjType *pRet=m_objs.back();
				m_objs.pop_back();
				m_numAlloced++;
				if(m_numAlloced>m_maxAlloced)
					m_maxAlloced=m_numAlloced;
				return pRet;
			}
		}

		void FreeObj(ObjType *pObj)
		{
#ifdef DEBUG
			ASSERT(m_objSet.find(pObj)!=m_objSet.end());//必须是我们这里分配出去的
#endif
			m_objs.push_back(pObj);
			m_numAlloced--;
		}
	
		int GetMaxAlloced() const	{	return m_maxAlloced;}
		bool IsEmpty() const		{	return m_objs.empty();}
	protected:
		list<ObjType*>	m_objs;
		int				m_numAlloced;
		int				m_maxAlloced;
#ifdef DEBUG
		set<ObjType*>	m_objSet;
#endif
	};
}//namespace Cool3D