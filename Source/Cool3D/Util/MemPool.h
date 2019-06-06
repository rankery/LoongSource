#pragma once
//#include "DebugMisc.h"

namespace Cool3D
{

	class CRTAlloc
	{
	public:
		void *Alloc(UINT size)
		{
			return malloc(size);
		}
		void Free(void *pMem)
		{
			free(pMem);
		}
	};
	/**	\class MemPool
		\brief 内存池
		\remarks 一次性创建多个不同大小的内存块备用,适用用于频繁分配/释放小块内存的场合
	*/
	template<typename Allocer = CRTAlloc>
	class MemPool
	{
		struct tagBlock
		{
			void	*pMem;			//分配的内存指针,或者是NULL
			DWORD	dwBlockSize;	//向系统分配的内存的大小
			DWORD	dwUsedSize;		//使用的大小,小于等于block size
			bool	bInUsing;		//是否正在被使用

			DWORD	dwMaxUsedSize;	//最大的使用大小
			int		nUseCount;		//被使用的次数
		};
	public:
		MemPool()
		{
			m_maxNotHitSize = 0;
			m_notHitTimes = 0;
		}
		virtual ~MemPool()
		{
			Destroy();
		}

		void *Alloc(DWORD size)
		{
			//--find in pool
			for(size_t i=0;i<m_blocks.size();i++)
			{
				tagBlock& block=m_blocks[i];
				if(block.bInUsing)
					continue;
				if(block.dwBlockSize>=size)//size合适
				{
					block.bInUsing=true;
					if(size>block.dwMaxUsedSize)
						block.dwMaxUsedSize=size;
					block.dwUsedSize=size;
					block.nUseCount++;
					return block.pMem;
				}
			}

			if(size>m_maxNotHitSize)
			{
				m_maxNotHitSize=size;
			}
			m_notHitTimes++;

			//--alloc new
			Allocer a;
			return a.Alloc(size);
		}
		
		void Free(void *pMem)
		{
			//--是否说pool里面的?
			for(size_t i=0;i<m_blocks.size();i++)
			{
				tagBlock& block=m_blocks[i];
				if(block.pMem==pMem)
				{
					block.bInUsing=false;
					return;
				}
			}

			//--free
			Allocer a;
			a.Free(pMem);
		}

		/**	按照设置分配所有内存
			\param 有多少块,每个块有多少字节
		*/
		void Init(const vector<unsigned int>& blocksCfg)
		{
			size_t numBlocks=blocksCfg.size();
			Allocer a;
			for(size_t i=0;i<numBlocks;i++)
			{
				tagBlock block;
				memset(&block,0,sizeof(tagBlock));
				block.pMem=a.Alloc(blocksCfg[i]);
				block.dwBlockSize=blocksCfg[i];
				m_blocks.push_back(block);
			}
		}
		void Destroy()
		{
			Allocer a;
			for(size_t i=0;i<m_blocks.size();i++)
			{
				tagBlock& block=m_blocks[i];
				a.Free(block.pMem);
			}
			m_blocks.clear();
		}

		void Report(tstring& out)
		{
			out=_T("MemPool report----\r\n");
			TCHAR tmp[512];
			DWORD total=0;
			for(size_t i=0;i<m_blocks.size();i++)
			{
				tagBlock& block=m_blocks[i];
				_stprintf(tmp,_T("block size=%dk,use count=%d,max usedsize=%dk\r\n"),
					block.dwBlockSize/1024,block.nUseCount,block.dwMaxUsedSize/1024);
				out+=tmp;
				total+=block.dwBlockSize;
			}
			_stprintf(tmp,_T("--total block size=%dk.\r\n"),total/1024);
			out+=tmp;
			_stprintf(tmp,_T("--not hit times=%d, max not hit size=%dk.\r\n"),m_notHitTimes, m_maxNotHitSize/1024);
			out+=tmp;

		}

	private:
		vector<tagBlock>	m_blocks;
		DWORD				m_notHitTimes;	// 未命中次数
		DWORD				m_maxNotHitSize;// 最大的未命中请求大小

	private://noncopyable
		MemPool(const MemPool& m);
		const MemPool& operator = (const MemPool& m);
	};
}//namespace Cool3D