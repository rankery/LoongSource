#include "StdAfx.h"
#include "NavResMgr.h"
#include "NavResPool.h"
#include "NavResSceneMesh.h"
#include "NavResHeightmap.h"
#include "NavResBitMap.h"

namespace WorldBase
{
	/**	根据文件名后缀来new不同的NavRes派生类对象
	*/
	template <class ResType>
	class NavResFactory
	{
	public:
		ResType* NewRes(const TCHAR* szName)
		{
			TCHAR szExt[15];
			_tsplitpath_s(szName,NULL,0,NULL,0,NULL,0,szExt,15);
			_tcslwr_s(szExt,15);

			if(_tcscmp(szExt,_T(".nav")) == 0)//scene mesh
				return new NavResSceneMesh(szName);
			if(_tcscmp(szExt,_T(".hmap"))==0)	//terrain patch
				return new NavResHeightmap(szName);
			if(_tcscmp(szExt,_T(".bmap"))==0)	//bitmap
				return new NavResBitMap(szName);

			return NULL;
		}
	};

	typedef NavResPool<NavResBase,NavResFactory<NavResBase>> MapResPool;
	struct NavResMgr::Member
	{
		IFS					*m_pFS[EFST_Num];
		MapResPool			*m_pResPool;
		bool				m_bAsync;
		StdFS				m_defaultFS;
		CriticalSection		m_lock;//资源池临界区
		bool				m_bPoolThreadSync;//资源池是否需要线程同步

		Member()
		{
			memset(m_pFS,0,EFST_Num*sizeof(IFS*));
			m_pResPool=NULL;

			m_bAsync=false;
			m_bPoolThreadSync=false;
		}
		~Member()
		{
			SAFE_DELETE(m_pResPool);
		}
	};

	NavResMgr::NavResMgr(void)
	{
		m_p=new Member;
	}

	NavResMgr::~NavResMgr(void)
	{
		Destroy();
	}

	static NavResMgr g_resMgr;
	NavResMgr *NavResMgr::Inst()
	{
		return &g_resMgr;
	}

	void NavResMgr::Destroy()
	{
		NavAsyncResIO::Inst()->Destroy();
		SAFE_DELETE(m_p);
	}

	void NavResMgr::Init(UINT poolSize,bool bPoolThreadSync)
	{
		m_p->m_bPoolThreadSync=bPoolThreadSync;
		m_p->m_pResPool=new MapResPool(poolSize);
		NavAsyncResIO::Inst()->Create();
	}

	UINT NavResMgr::GetNumLoading()
	{
		return NavAsyncResIO::Inst()->GetQueueSize();
	}

	void NavResMgr::EnableIOAsync(bool bAsync)
	{
		m_p->m_bAsync=bAsync;
	}

	bool NavResMgr::IsIOAsync() const
	{
		return m_p->m_bAsync;
	}

	void NavResMgr::SetFS(EFSType type,IFS* pFS)
	{
		ASSERT(type>=0&&type<EFST_Num);
		m_p->m_pFS[type]=pFS;
	}

	IFS* NavResMgr::GetFS(EFSType type)
	{
		ASSERT(type>=0&&type<EFST_Num);
		return m_p->m_pFS[type];
	}

	IFS* NavResMgr::GetFS(const TCHAR* szResName)
	{
		if(_tcsnicmp(szResName,_T("data\\system"),11)==0)
			return m_p->m_pFS[EFST_System];
		if(_tcsnicmp(szResName,_T("data\\animation"),14)==0)
			return m_p->m_pFS[EFST_System];
		if(_tcsnicmp(szResName,_T("data\\model"),10)==0)
			return m_p->m_pFS[EFST_Model];
		if(_tcsnicmp(szResName,_T("data\\map"),8)==0)
			return m_p->m_pFS[EFST_Map];
		return NULL;
	}

	IFS* NavResMgr::DefaultFS()
	{
		return &m_p->m_defaultFS;
	}

	IFS* NavResMgr::SafeGetFS(const TCHAR* szResName)
	{
		IFS* pFS=GetFS(szResName);
		if(pFS==NULL)
			pFS=&m_p->m_defaultFS;
		return pFS;
	}

	IFS* NavResMgr::SafeGetFS(EFSType type)
	{
		ASSERT(type>=0&&type<EFST_Num);
		if(m_p->m_pFS[type]==NULL)
			return &m_p->m_defaultFS;
		return m_p->m_pFS[type];
	}

	void NavResMgr::OnFSChanged(EFSType type)
	{
		NavAsyncResIO::Inst()->CancleByFS(m_p->m_pFS[type]);
	}

	NavResBase* NavResMgr::NewRes(const TCHAR* szName,DWORD param)
	{
		IFS *pFS=SafeGetFS(szName);
		if(m_p->m_bPoolThreadSync)m_p->m_lock.Lock();

		NavResBase* pRes=m_p->m_pResPool->NewRes(szName,pFS,param,m_p->m_bAsync);

		if(m_p->m_bPoolThreadSync)m_p->m_lock.Unlock();
		return pRes;
	}

	void NavResMgr::ReleaseRes(NavResBase *pRes)
	{
		if(m_p->m_bPoolThreadSync)m_p->m_lock.Lock();

		m_p->m_pResPool->DelRes(pRes);

		if(m_p->m_bPoolThreadSync)m_p->m_lock.Unlock();
	}

	void NavResMgr::Update()
	{
		if(m_p->m_bPoolThreadSync)m_p->m_lock.Lock();

		m_p->m_pResPool->UpdateLoadingList();

		if(m_p->m_bPoolThreadSync)m_p->m_lock.Unlock();
	}

	bool CompResInfo(const pair<tstring,int>& info1,const pair<tstring,int>& info2)
	{
		return info1.second > info2.second;
	}

	void NavResMgr::Report()
	{
		vector<pair<tstring,int> > navArray;
		m_p->m_pResPool->ExportInfo(navArray);

		std::sort(navArray.begin(),navArray.end(),CompResInfo);
		LOG(_T("--Nav pool report------------------------------------------------------\r\n"));
		int sum=0;
		for(size_t i=0;i<navArray.size();i++)
		{
			sum+=navArray[i].second;
			LOG(_T("%04d\t%04dKB\t%s\r\n"),i,navArray[i].second/1024,navArray[i].first.c_str());
		}
		LOG(_T("Total=%dKB\r\n"),sum/1024);
	}

	void NavResMgr::DoGC()
	{
		if(m_p->m_bPoolThreadSync)m_p->m_lock.Lock();

		m_p->m_pResPool->DoGC();

		if(m_p->m_bPoolThreadSync)m_p->m_lock.Unlock();
	}

	int NavResMgr::GetAsyncQueueSize()
	{
		return NavAsyncResIO::Inst()->GetQueueSize();
	}

	void NavResMgr::GetLoadCount( UINT& finishedCount,UINT& reqCount )
	{
		finishedCount=NavAsyncResIO::Inst()->GetQueuePopCount();
		reqCount=NavAsyncResIO::Inst()->GetQueuePushCount();
	}

	void NavResMgr::ZeroLoadCount()
	{
		NavAsyncResIO::Inst()->ZeroQueuePushPopCount();
	}

}//namespace WorldBase