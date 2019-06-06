#pragma once
#include "NavAsyncResIO.h"

class IFS;
namespace WorldBase
{
	/**	\class DefaultResFactory
	*/
	template <class ResType>
	class DefaultResFactory
	{
	public:
		/**	资源池中没有同名资源时调用
		*/
		ResType* NewRes(const TCHAR* szName)
		{
			return new ResType(szName);
		}
	};


	/**	\class ResPool
	\brief 资源Pool
	\par 负责管理所有活动的资源, 并按照一定的数量来Cache资源
	*/
	template<class ResType, class ResFactory = DefaultResFactory<ResType> >
	class NavResPool
	{
		typedef map<tstring,ResType*> ResMap;
	public:
		NavResPool(UINT size)
		{
			m_nMaxSize=size;
			m_nCurSize=0;
		}
		virtual ~NavResPool(void)
		{
			FinalRelease();
		}

		/**	创建一个新的资源
		\remarks 有可能返回空(当Factory返回空时)
		*/
		ResType *NewRes(const TCHAR* _szName,IFS *pFS,DWORD param=0,bool bAsync=false)
		{
			if(_szName == NULL)
				return NULL;
			TCHAR szName[MAX_PATH]={0};
			_tcscpy_s(szName,MAX_PATH,_szName);
			_tcslwr_s(szName,MAX_PATH);

			ResType *pRet=FindInPool(szName,true);
			if(pRet!=NULL)
			{
				pRet->IncRef();

				return pRet;
			}

			ResFactory factory;
			pRet=factory.NewRes(szName);
			if(pRet==NULL)
				return NULL;
			pRet->AttachFS(pFS);
			if(bAsync)
			{
				pRet->CreateContentAsync(param);
			}
			else
			{
				m_nCurSize+=pRet->CreateContent(param);
			}
			pRet->IncRef();



			int size=m_nCurSize-m_nMaxSize;
			if(size>0)
				MakeRoom(size);

			if(bAsync)
				m_loadingList[pRet->GetName()]=pRet;
			else
				m_activeList[pRet->GetName()]=pRet;
			return pRet;
		}
		/**	外部释放一个资源
		\remarks 并没有真正的delete res object,而是减少引用计数或者放入DeadList中,
		只有在腾出空间时才真正delete
		\see MakeRoom()
		*/
		void DelRes(ResType *pRes)
		{
			if(pRes->GetRef() == 1)
			{
				ResMap::iterator fIter=m_activeList.find(pRes->GetName());

				if(fIter != m_activeList.end())
				{
					m_activeList.erase(fIter);				
					//--dead list中的所有Res的引用计数都是0
					if(m_nCurSize<m_nMaxSize)//如果还有,空间,则暂存起来
					{
						pRes->DecRef();
						m_deadList.insert(make_pair(pRes->GetName(),pRes));
					}
					else
					{
						m_nCurSize-=pRes->GetSize();
						NavAsyncResIO::Inst()->QueueRequest(EResIO_Delete,pRes,0);
					}
				}//endof if(in active list)
				else
				{
					//must in loading list
					fIter=m_loadingList.find(pRes->GetName());
					ASSERT(fIter!=m_loadingList.end());

					m_nCurSize-=pRes->GetSize();
					NavAsyncResIO::Inst()->QueueRequest(EResIO_Delete,pRes,0);
					m_loadingList.erase(fIter);
				}
			}
			else
			{
				ASSERT(pRes->GetRef() > 1);
				pRes->DecRef();
			}
		}

		/**	执行垃圾收集,将所有dead res清除
		*/
		void DoGC()
		{
			ResMap::iterator iter=m_deadList.begin();
			for(;iter!=m_deadList.end();iter++)
			{
				ResType *pRes=(*iter).second;
				m_nCurSize-=pRes->GetSize();
				NavAsyncResIO::Inst()->QueueRequest(EResIO_Delete,pRes,0);
			}
			m_deadList.clear();
		}

		int GetCurSize() const	{	return m_nCurSize;}

		void UpdateLoadingList()
		{
			for(ResMap::iterator iter=m_loadingList.begin();
				iter!=m_loadingList.end();)
			{
				ResType *pRes=(*iter).second;
				if(pRes->IsCreated())
				{
					//Load完成
					m_nCurSize+=pRes->GetSize();
					//移动到Activelist
					iter=m_loadingList.erase(iter);
					m_activeList[pRes->GetName()]=pRes;
				}
				else
					++iter;
			}//endof for
		}

		void ExportInfo(vector<pair<tstring,int> >& resInfoArray)
		{
			for(ResMap::iterator iter=m_activeList.begin();
				iter!=m_activeList.end();++iter)
			{
				ResType *pRes=(*iter).second;
				pair<tstring,int> resInfo;
				resInfo.first=pRes->GetName();
				resInfo.second=pRes->GetSize();
				resInfoArray.push_back(resInfo);
			}

			for(ResMap::iterator iter=m_loadingList.begin();
				iter!=m_loadingList.end();++iter)
			{
				ResType *pRes=(*iter).second;
				pair<tstring,int> resInfo;
				resInfo.first=pRes->GetName();
				resInfo.second=pRes->GetSize();
				resInfoArray.push_back(resInfo);
			}

			for(ResMap::iterator iter=m_deadList.begin();
				iter!=m_deadList.end();++iter)
			{
				ResType *pRes=(*iter).second;
				pair<tstring,int> resInfo;
				resInfo.first=pRes->GetName();
				resInfo.second=pRes->GetSize();
				resInfoArray.push_back(resInfo);
			}
		}
	private:
		/**	释放所有资源,不管外部是否仍存在引用,在程序最后调用
		*/
		void FinalRelease()
		{
			DoGC();
			//--
			ResMap::iterator iter=m_activeList.begin();
			for(;iter!=m_activeList.end();iter++)
			{
				ResType *pRes=(*iter).second;
				m_nCurSize-=pRes->GetSize();
				NavAsyncResIO::Inst()->QueueRequest(EResIO_Delete,pRes,0);
			}
			m_activeList.clear();

			//--free loading list
			iter=m_loadingList.begin();
			for(;iter!=m_loadingList.end();iter++)
			{
				ResType *pRes=(*iter).second;
				m_nCurSize-=pRes->GetSize();
				NavAsyncResIO::Inst()->QueueRequest(EResIO_Delete,pRes,0);
			}
			m_loadingList.clear();

		}
		/**	在已有的两个list中查找
		\param	szName:资源名称
		reactive:如果在dead list中发现,是否将其移动到active list
		*/
		ResType *FindInPool(const TCHAR* szName,bool reactive)
		{
			ResMap::iterator iter=m_activeList.find(szName);
			if(iter!=m_activeList.end())
			{
				return (*iter).second;
			}

			iter=m_loadingList.find(szName);
			if(iter!=m_loadingList.end())
			{
				return (*iter).second;
			}

			iter=m_deadList.find(szName);
			if(iter!=m_deadList.end())
			{
				ResType *pRet=(*iter).second;
				if(reactive)//重新激活
				{
					m_deadList.erase(iter);
					m_activeList.insert(make_pair(pRet->GetName(),pRet));
				}
				return pRet;
			}

			return NULL;
		}
		/**	腾出指定大小的空间备用
		*/
		bool MakeRoom(int size)//删除deadList中的资源以腾出空间,并修正m_nCurSize
		{
			if(m_deadList.empty())
				return false;

			int total=0;

			ResMap::iterator iter=m_deadList.begin();
			for(;iter!=m_deadList.end();)
			{
				ResType *pRes=(*iter).second;
				total+=pRes->GetSize();
				iter=m_deadList.erase(iter);
				NavAsyncResIO::Inst()->QueueRequest(EResIO_Delete,pRes,0);
				if(total>=size)
					break;
			}

			m_nCurSize-=total;

			return total > size;
		}
	private:
		ResMap	m_deadList;
		ResMap	m_activeList;
		ResMap	m_loadingList;//正在Loading的资源,load完成后计算size

		UINT	m_nMaxSize;
		UINT	m_nCurSize;//当前所有Res的Size的总和
	};
}//namespace WorldBase