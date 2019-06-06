#pragma once

class IFS;

namespace Cool3D
{
	class ResBase;

	enum EResIOCmd
	{
		EResIO_Load,
		EResIO_Delete
	};

	struct tagResReq
	{
		EResIOCmd	cmd;
		ResBase*	pRes;
		DWORD		param;

		tagResReq(EResIOCmd	eCmd,ResBase* pResbase,DWORD dwParam)
		{
			cmd		= eCmd;
			pRes	= pResbase;
			param	= dwParam;	
		}

		tagResReq(const tagResReq& other)
		{
			Copy(other);
		}

		//定义一个copy构造函数
		const tagResReq& operator =(const tagResReq& other)
		{
			if(this!=&other)
				Copy(other);
			return *this;
		}

	private:
		void Copy(const tagResReq& other)
		{
			cmd		= other.cmd;
			pRes	= other.pRes;
			param	= other.param;
		}
	};

	/**	\class ResRequestQueueTS
		\brief 线程安全的资源请求队列
	*/
	class ResRequestQueueTS
	{
	public:
		ResRequestQueueTS(void);
		virtual ~ResRequestQueueTS(void);

		/**	向队列中增加一个请求
			\remarks delete命令只能由ResPool发出,否则会与ResPool冲突
			\see ResPool
		*/
		void Push(EResIOCmd cmd,ResBase *pRes,DWORD param);
		/**	从队列中取出一个请求,该请求会从队列中删除
		*/
		tagResReq Pop();
		/**	队列是否为空
		*/
		bool IsEmpty();
		UINT GetSize();
		/**	取得事件，当队列中加入消息时，此事件被激活
		*/
		HANDLE	GetEvent() { return m_hEvent; }

		/**	取得Push计数
		*/
		UINT GetPushCount() { return m_pushCount; }
		/**	取得Pop计数
		*/
		UINT GetPopCount()  { return m_popCount;  }
		/** Push&Pop计数清零
		*/
		void ZeroPushPopCount();

		void CancleByFS(IFS* pFS);

	private:
		CRITICAL_SECTION	m_Lock;
		HANDLE				m_hEvent;

		deque<tagResReq>	m_queue;

		volatile UINT		m_pushCount;
		volatile UINT		m_popCount;
	};
}//namespace Cool3D