//-----------------------------------------------------------------------------
/*	file: simple_task
	auth: Lyp
	date: 2003/1/1
	last: 2004/3/1
	desc: GT System	任务基本件 1.1

	history: 
	1.1: 
	使用了新的模板参数(your_class),派生类不再需要CallTask成员,结构更清晰

	1.0:
	将以前的任务思想用模板来实现,使用更方便
*/
//-----------------------------------------------------------------------------
#pragma once
#include "simple_map.h"

namespace vEngine{

//-----------------------------------------------------------------------------
// 任务基本件
//-----------------------------------------------------------------------------
template<typename TaskAddin, typename your_class>
class TSimpleTask
{
/*	接口:

	// 创建任务缓冲
	BOOL CreateTaskBuffer(INT nTaskNum, INT nSubTaskNum);

	向所有任务发送结束消息，并设置是否任务变成自动删除型
	VOID EndAllTask(BOOL bSetAutoDel);
	
	FLOAT GetCurTime();
*/

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
public:

	// 此种接口适用于线程函数
	typedef DWORD (your_class::* your_fpTask)(DWORD);
	enum ETaskState	// 任务状态
	{
		ETS_Null = 0,
		ETS_Paused,
		ETS_Ready,
		ETS_FirstRun,
		ETS_Running,
		ETS_Finished,
		ETS_End
	};
	
	struct tag4DWORD
	{
		union{ INT n1; DWORD dw1; FLOAT	f1;	};
		union{ INT n2; DWORD dw2; FLOAT f2; };
		union{ INT n3; DWORD dw3; FLOAT f3; };
		union{ INT n4; DWORD dw4; FLOAT f4; };
	};
	
	template<typename your_class>	// your_fpTask实际上是需要模板指定类型的
	struct tagSubTask
	{
		your_fpTask	fp;				// 本子任务的函数指针
		ETaskState	eStats;			// 当前状态
		
		DWORD		dwID;
		FLOAT		fStartTime;		// 本子任务开始的时间
		FLOAT		fWaitTime;		// 本任务等待上一任务的时间
		FLOAT		fExpectTime;	// 要求的任务执行时间
		BOOL		bExpectTimeout;	// 是否要求执行的时间已经过了
		tag4DWORD	dw4Param;		// 任务参数
		tag4DWORD	dw4Temp;		// 供临时使用的参数
		DWORD		dwRet;			// 子任务返回值
		
		BOOL		bValid;			// 此子任务是否存在
		BOOL		bRequireToEnd;	// 是否外部需要强行终止
	};
	
	// 总任务结构
	template<typename TaskAddin, typename your_class>
	struct tagTask
	{
		tagSubTask<your_class>*	pSubTask;	// 指向子任务
		BOOL		bValid;
		DWORD		dwID;					// 任务ID
		ETaskState	eStats;					// 任务进行状态
		INT			nSubTask;				// 有效的子任务个数
		DWORD		dwRet;					// 总任务最终返回参数
		TaskAddin	addin;					// 任务附加数据
		BOOL		bAutoDel;				// 完成后是否自动销毁
		CHAR		szName[256];			// 任务名字
	};


//-----------------------------------------------------------------------------
// 函数接口
//-----------------------------------------------------------------------------
	
	VOID Update(FLOAT fDeltaTime)
	{
		// 内置时间控制
		m_fDeltaTime = fDeltaTime;
		m_fCurTime += fDeltaTime;

		DoTask(0);
	}

	// 创建任务缓冲
	BOOL CreateTaskBuffer(INT nTaskNum, INT nSubTaskNum)
	{
		ASSERT(nTaskNum>0 && nSubTaskNum>0);

		// 主任务
		m_pTask = new tagTask<TaskAddin, your_class>[nTaskNum];
		if( !m_pTask ) 
			return FALSE;
		
		// 子任务
		for( INT n=0; n<nTaskNum; n++ )
		{
			m_pTask[n].pSubTask = new tagSubTask<your_class>[nSubTaskNum];
			if( !m_pTask[n].pSubTask )
			{
				if( n>0 )
				{
					for( INT m=n-1; m>=0; m-- ) 
						delete[](m_pTask[m].pSubTask);
				}
				delete[](m_pTask);
				return FALSE;
			}
		}

		// 在此处记录最多总任务数与最多子任务数
		m_nMaxTask = nTaskNum;
		m_nMaxSubTask = nSubTaskNum;

		// 清空任务
		for( n=0; n<m_nMaxTask; n++ )
		{
			m_pTask[n].bValid = FALSE;
			for( INT m=0; m<m_nMaxSubTask; m++ )
				m_pTask[n].pSubTask[m].bValid = FALSE;
		}
		return TRUE;
	}


	//-----------------------------------------------------------------------------
	// 增加任务, 返回的是任务序号
	//-----------------------------------------------------------------------------
	BOOL AddTask(LPCSTR szTaskName, TaskAddin* pAddin, BOOL bAutoDel)
	{
		ASSERT(m_nTask>=0 && m_nTask<m_nMaxTask);
		DWORD dwTaskNameCrc = m_pUtil->Crc32(szTaskName);

		// 检查是否有重名的任务
		if( TRUE == m_mapTaskIndex.IsExist(dwTaskNameCrc) )
		{
			ASSERT(0);
			return FALSE;
		}

		for( INT n=0; n<m_nMaxTask; n++ )
		{
			if( !m_pTask[n].bValid ) // 此任务不存在, 可以插进来
			{
				ASSERT(szTaskName);
				ASSERT(strlen(szTaskName) < 255);

				m_pTask[n].eStats = ETS_Ready;
				m_pTask[n].bValid = TRUE;
				m_pTask[n].bAutoDel = bAutoDel;	// 完成后是否自动删除
				m_pTask[n].nSubTask = 0;	// 子任务为零
				m_pTask[n].dwRet = 0;
				strcpy(m_pTask[n].szName, szTaskName);
				
				// 记录名字和序号的对应关系
				m_mapTaskIndex.Add(dwTaskNameCrc, n);
				
				// 拷贝命令
				if( NULL != pAddin )
					memcpy(&m_pTask[n].addin, pAddin, sizeof(TaskAddin));
				else
					ZeroMemory(&m_pTask[n].addin, sizeof(TaskAddin));
				
				m_nTask++;
				return n;
			}
		}
		ASSERT(0); 
		return GT_INVALID;
	}


	//-----------------------------------------------------------------------------
	// 增加一个子任务, 自动找到可以放的位置"顺序"放入
	// 但实际上子任务是顺序执行，当发现中间有无效子任务时即停止，后面的都忽略
	//-----------------------------------------------------------------------------
	INT AddSubTask(LPCSTR szTaskName, your_fpTask fp, tag4DWORD* pParam, 
				   FLOAT fWaitTime, FLOAT fExpectTime)
	{
		INT nTaskIndex = GetTaskIndex(szTaskName);
		if( GT_INVALID == nTaskIndex )
		{
			ASSERT(0);
			return GT_INVALID;
		}

		ASSERT( m_nTask>=0 && m_nTask<=m_nMaxTask );
		ASSERT( nTaskIndex>=0 && nTaskIndex<m_nMaxTask );
		ASSERT( m_pTask[nTaskIndex].bValid );	// 确保指定总任务存在
		ASSERT( fp!=NULL );
		
		for( INT n=0; n<m_nMaxSubTask; n++ )
		{
			if( !m_pTask[nTaskIndex].pSubTask[n].bValid ) // 找到可放的地方
			{
				tagSubTask<your_class> &sub = m_pTask[nTaskIndex].pSubTask[n];
				ZeroMemory( &sub, sizeof(sub) );
				
				sub.bValid = TRUE;
				sub.fp = fp;
				sub.eStats = ETS_Ready;
				sub.fWaitTime = fWaitTime;
				sub.fStartTime = 0.0f;
				sub.fExpectTime = fExpectTime;
				sub.bExpectTimeout = FALSE;
				
				if( pParam )	// 是否需要拷贝参数
					memcpy(&sub.dw4Param, pParam, sizeof(sub.dw4Param));
				
				m_pTask[nTaskIndex].nSubTask++;
				return n;
			}
		}
		ASSERT(0); 
		return GT_INVALID;
	}

	//-----------------------------------------------------------------------------
	// 结束一个总任务，实际上是向所有子任务发送结束消息
	//-----------------------------------------------------------------------------
	VOID EndTask(LPCSTR szTaskName, BOOL bSetAutoDel=TRUE)
	{
		INT nTaskIndex = GetTaskIndex(szTaskName);
		if( GT_INVALID == nTaskIndex )
			return;

		ASSERT( nTaskIndex>=0 && nTaskIndex<m_nMaxTask );
		ASSERT( m_pTask[nTaskIndex].bValid );
		
		// 向所有子任务发送结束消息
		for( INT n=0; n<m_nMaxSubTask; n++ )
		{
			if( m_pTask[nTaskIndex].pSubTask[n].bValid )
				m_pTask[nTaskIndex].pSubTask[n].bRequireToEnd = TRUE;
		}

		m_pTask[nTaskIndex].bAutoDel = bSetAutoDel;
	}


	//-----------------------------------------------------------------------------
	// 删除一个总任务
	//-----------------------------------------------------------------------------
	VOID DelTask(LPCSTR szTaskName)
	{
		INT nTaskIndex = GetTaskIndex(szTaskName);
		if( GT_INVALID == nTaskIndex )
			return;

		ASSERT( nTaskIndex>=0 && nTaskIndex<m_nMaxTask );
		ASSERT( m_pTask[nTaskIndex].bValid );
		
		// 为保证逻辑严密性，任务必须完成后才能删除
		ASSERT( m_pTask[nTaskIndex].eStats == ETS_Finished );	
		m_pTask[nTaskIndex].bValid = FALSE;
		
		// 清除所有子任务
		for( INT n=0; n<m_nMaxSubTask; n++ )
			m_pTask[nTaskIndex].pSubTask[n].bValid = FALSE;
		
		m_pTask[nTaskIndex].nSubTask = 0;
		m_nTask--;

		// 从索引表中删除
		DWORD dwTaskNameCrc = m_pUtil->Crc32(szTaskName);
		m_mapTaskIndex.Erase(dwTaskNameCrc);
	}

	VOID DelTask(INT nTaskIndex)
	{
		ASSERT( nTaskIndex>=0 && nTaskIndex<m_nMaxTask );
		ASSERT( m_pTask[nTaskIndex].bValid );
		
		// 为保证逻辑严密性，任务必须完成后才能删除
		ASSERT( m_pTask[nTaskIndex].eStats == ETS_Finished );	
		m_pTask[nTaskIndex].bValid = FALSE;
		
		// 清除所有子任务
		for( INT n=0; n<m_nMaxSubTask; n++ )
			m_pTask[nTaskIndex].pSubTask[n].bValid = FALSE;
		
		m_pTask[nTaskIndex].nSubTask = 0;
		m_nTask--;

		// 从索引表中删除
		DWORD dwTaskNameCrc = m_pUtil->Crc32(m_pTask[nTaskIndex].szName);
		m_mapTaskIndex.Erase(dwTaskNameCrc);
	}


	//-----------------------------------------------------------------------------
	// 开始一个总任务
	//-----------------------------------------------------------------------------
	VOID StartTask(LPCSTR szTaskName)
	{
		INT nTaskIndex = GetTaskIndex(szTaskName);
		if( GT_INVALID == nTaskIndex )
			return;

		ASSERT( nTaskIndex>=0 && nTaskIndex<m_nMaxTask );
		ASSERT( m_pTask[nTaskIndex].bValid );
		ASSERT( ETS_Ready == m_pTask[nTaskIndex].eStats );
		ASSERT( m_pTask[nTaskIndex].pSubTask[0].bValid );
	
		// 设置任务状态
		m_pTask[nTaskIndex].eStats = ETS_Running;
		
		// 开始第一个子任务
		m_pTask[nTaskIndex].pSubTask[0].eStats = ETS_FirstRun;
		m_pTask[nTaskIndex].pSubTask[0].fStartTime = m_fCurTime;
	}


	//-----------------------------------------------------------------------------
	// 检查指定的任务是否存在，通常用来检测自删除型任务是否执行完毕
	//-----------------------------------------------------------------------------
	BOOL IsTaskExist(LPCSTR szTaskName)
	{
		INT nTaskIndex = GetTaskIndex(szTaskName);
		if( GT_INVALID == nTaskIndex )
			return FALSE;

		ASSERT( nTaskIndex>=0 && nTaskIndex<m_nMaxTask );
		if( FALSE == m_pTask[nTaskIndex].bValid )
			return FALSE;
			
		return TRUE;
	}

	//-----------------------------------------------------------------------------
	// 得到指定任务的数据结构体
	//-----------------------------------------------------------------------------
	tagTask<TaskAddin, your_class>* GetTask(LPCSTR szTaskName)
	{
		INT nTaskIndex = GetTaskIndex(szTaskName);
		if( GT_INVALID == nTaskIndex )
			return GT_INVALID;

		ASSERT( nTaskIndex>=0 && nTaskIndex<m_nMaxTask );
		ASSERT( m_pTask[nTaskIndex].bValid );
		return &m_pTask[nTaskIndex];
	}

	tagTask<TaskAddin, your_class>* GetCurTask()
	{ return &m_pTask[m_nCurTask]; }

	tagSubTask<your_class>* GetCurSubTask()
	{ return &m_pTask[m_nCurTask].pSubTask[m_nCurSubTask]; }


	//-----------------------------------------------------------------------------
	// 得到总任务状态
	//-----------------------------------------------------------------------------
	VOID GetTaskState(LPCSTR szTaskName, BOOL& bFinished, DWORD& dwRet)
	{
		INT nTaskIndex = GetTaskIndex(szTaskName);
		ASSERT( nTaskIndex>=0 && nTaskIndex<m_nMaxTask );
		if( GT_INVALID == nTaskIndex )
			return;
		
		ASSERT( m_pTask[nTaskIndex].bValid );
		
		bFinished = FALSE; dwRet = 0;
		if( ETS_Finished == m_pTask[nTaskIndex].eStats )
		{
			bFinished = TRUE;
			dwRet = m_pTask[nTaskIndex].dwRet;
		}
	}
	
	//-----------------------------------------------------------------------------
	// 得到舞台总任务实际状态
	//-----------------------------------------------------------------------------
	ETaskState GetTaskState(LPCSTR szTaskName)
	{
		INT nTaskIndex = GetTaskIndex(szTaskName);
		ASSERT( nTaskIndex>=0 && nTaskIndex<m_nMaxTask );
		if( GT_INVALID == nTaskIndex )
			return ETS_Null;

		ASSERT( m_pTask[nTaskIndex].bValid );
		return m_pTask[nTaskIndex].eStats;
	}

	//-----------------------------------------------------------------------------
	// 向所有任务发送结束消息，并设置是否任务变成自动删除型
	//-----------------------------------------------------------------------------
	VOID EndAllTask(BOOL bSetAutoDel)
	{
		for( INT n=0; n<m_nMaxTask; n++ )
		{
			if( !m_pTask[n].bValid )
				continue;

			EndTask(n, bSetAutoDel);
		}
	}
	


	//-----------------------------------------------------------------------------
	// 执行任务链
	//-----------------------------------------------------------------------------
	VOID DoTask(DWORD dwParam)
	{
		for( INT n=0; n<m_nMaxTask; n++ )
		{
			if( m_pTask[n].bValid && ( ETS_Running == m_pTask[n].eStats 
				|| ETS_FirstRun == m_pTask[n].eStats ) )
			{
				// for short
				tagTask<TaskAddin, your_class> &t = m_pTask[n];	
				
				// 从第一个任务开始搜索
				for( INT m=0; m<m_nMaxSubTask; m++ )
				{
					// 此子任务不存在,就认为后面的也不存在
					if( FALSE == t.pSubTask[m].bValid )
						break;
					
					// 上一个子任务还没有运行，以后的都不需判断
					if( m>0 && ETS_Ready == t.pSubTask[m-1].eStats )
						break;
					
					switch( t.pSubTask[m].eStats )
					{
					case ETS_Running:	// 运行子任务流程
					case ETS_FirstRun:
						ASSERT( NULL != t.pSubTask[m].fp );
						m_nCurTask = n;
						m_nCurSubTask = m;

						// 设置是否要求执行的时间已经过去
						if( m_fCurTime - t.pSubTask[m].fStartTime 
							>= t.pSubTask[m].fExpectTime )
							t.pSubTask[m].bExpectTimeout = TRUE;

						// 调用任务函数
						(m_pObj->*t.pSubTask[m].fp)( dwParam );
						if( ETS_FirstRun == t.pSubTask[m].eStats )
							t.pSubTask[m].eStats = ETS_Running;

						break;

					case ETS_Ready:
						if( m>0 )
						{
							// 如果WaitTime为负，代表须等待上一子任务完成
							if( t.pSubTask[m].fWaitTime<0.0f )
							{
								if( ETS_Finished == t.pSubTask[m-1].eStats )
								{
									StartSubTask(n, m);	// 启动子任务
								}
							}
							else
							{	// 检查是否到时
								if( m_fCurTime - t.pSubTask[m-1].fStartTime 
									> t.pSubTask[m].fWaitTime )
								{
									StartSubTask(n, m);	// 启动子任务
								}
							}
						}
						break;

						
					case ETS_Finished:
						if( IsAllSubTaskFinish(n) )	// 所有子任务完成
						{
							t.eStats = ETS_Finished;	// 总任务设成完成
							
							// 总任务的返回值等于最后子任务的返回值
							ASSERT( t.nSubTask>0 && t.nSubTask<=m_nMaxSubTask );
							t.dwRet = t.pSubTask[t.nSubTask-1].dwRet;
							
							// 检查是否是自动删除型任务
							if( TRUE == t.bAutoDel )
							{
								DelTask(n);
								goto _next_task;// 防止后继部分对此任务操作
							}
						}
						break;
						

					case ETS_Paused:
						break;

					default: ASSERT(0);	
						break;
					}
				}	// for( INT m=0; m<m_nMaxSubTask; m++ )
			}	// if( m_pTask[n].bValid && ETS_Running == m_pTask[n].eStats )
_next_task:;
		}	// for( INT n=0; n<m_nMaxTask; n++ )
	}

	TSimpleTask():m_fCurTime(0.0f), m_fDeltaTime(0.0f), m_pTask(NULL), 
		m_nMaxTask(0), m_nMaxSubTask(0), m_nCurTask(GT_INVALID), 
		m_nCurSubTask(GT_INVALID), m_nTask(0)
	{ m_pObj=(your_class*)this; m_pUtil = "util"; }

	TSimpleTask(your_class* pThis):m_fCurTime(0.0f), m_fDeltaTime(0.0f), 
		m_pTask(NULL), m_nMaxTask(0), m_nMaxSubTask(0), m_nCurTask(GT_INVALID), 
		m_nCurSubTask(GT_INVALID), m_nTask(0), m_pObj(pThis)
	{ m_pUtil = "util"; }

	virtual ~TSimpleTask()	{ DestroyTaskBuffer(); }

	//-----------------------------------------------------------------------------
	// 标准的等待任务，直到时间到
	//-----------------------------------------------------------------------------
	DWORD ProcWait(DWORD)
	{
		tagSubTask<TaskAddin>* pSub = GetCurSubTask();
		
		if( FALSE == pSub->bExecuted )	// 是否第一次执行
			pSub->bExecuted = TRUE;
		
		if( TRUE == pSub->bRequireToEnd || TRUE == pSub->bExpectTimeout )
			pSub->eStats = ETS_Finished;
	
		return 0;
	}


private:

	//-----------------------------------------------------------------------------
	// 检查一个总任务中的所有子任务是否结束
	//-----------------------------------------------------------------------------
	BOOL IsAllSubTaskFinish(INT nTaskIndex)
	{
		ASSERT( nTaskIndex>=0 && nTaskIndex<m_nMaxTask );
		ASSERT( m_pTask[nTaskIndex].bValid );
		
		for( INT n=0; n<m_pTask[nTaskIndex].nSubTask; n++ )
		{
			if( ETS_Finished != m_pTask[nTaskIndex].pSubTask[n].eStats )
				return FALSE;
		}
		
		return TRUE;
	}

	//-----------------------------------------------------------------------------
	// 开始进行一个子任务
	//-----------------------------------------------------------------------------
	VOID StartSubTask(INT nTaskIndex, INT nSubTaskIndex)
	{
		ASSERT( nTaskIndex>=0 && nTaskIndex<m_nMaxTask );
		ASSERT( nSubTaskIndex>=0 && nSubTaskIndex<m_nMaxSubTask );
		ASSERT( m_pTask[nTaskIndex].pSubTask[nSubTaskIndex].bValid );
		ASSERT( ETS_Finished != m_pTask[nTaskIndex].eStats );
		ASSERT( ETS_Ready == m_pTask[nTaskIndex].pSubTask[nSubTaskIndex].eStats );
		
		m_pTask[nTaskIndex].pSubTask[nSubTaskIndex].eStats = ETS_FirstRun;
		
		// 记录任务开始时间
		m_pTask[nTaskIndex].pSubTask[nSubTaskIndex].fStartTime = m_fCurTime;
	}


	//-----------------------------------------------------------------------------
	// 通过名字得到任务序号，找不到返回GT_INVALID
	//-----------------------------------------------------------------------------
	INT GetTaskIndex(LPCSTR szName)
	{
		DWORD dwTaskNameCrc = m_pUtil->Crc32(szName);
		return m_mapTaskIndex.Peek(dwTaskNameCrc);
	}

	//-----------------------------------------------------------------------------
	// 删除任务缓冲
	//-----------------------------------------------------------------------------
	void DestroyTaskBuffer()
	{
		if( m_nMaxTask==0 && m_nMaxSubTask==0 )
			return;

		ASSERT( m_nMaxTask>0 && m_nMaxSubTask>0 );
		ASSERT( m_pTask != NULL );
		
		// 先删除子任务
		for( INT n=0; n<m_nMaxTask; n++ )
		{
			ASSERT( m_pTask[n].pSubTask!=NULL );
			delete[](m_pTask[n].pSubTask);
			m_pTask[n].pSubTask = NULL;
		}

		// 后删除主任务
		delete[](m_pTask);
		m_pTask = NULL;

		m_nMaxTask = 0;
		m_nMaxSubTask = 0;
	}

	tagTask<TaskAddin, your_class>*	m_pTask;	// 指向TaskBuffer
	your_class* m_pObj;		// 指向实例的指针

	TObjRef<Util>		m_pUtil;
	TMap<DWORD, INT>	m_mapTaskIndex;	// 下标TaskNameCrc,值TaskIndex

	FLOAT	m_fCurTime;
	FLOAT	m_fDeltaTime;

	INT		m_nMaxTask;
	INT		m_nMaxSubTask;
	INT		m_nTask;
	INT		m_nCurTask;
	INT		m_nCurSubTask;

};

}	// namespace vEngine{
