#pragma once
#include "..\Device\IDevice.h"
#include "..\Device\IVertBuffer.h"
#include "..\Device\IIndexBuffer.h"
#include "..\Kernel.h"

namespace Cool3D
{
	class DynaVBAlloc
	{
	public:
		void *Alloc(UINT size)
		{
			IDevice *pDev=Device();
			IVertBuffer *pVB=pDev->NewVB();
			pVB->Create(size,EB_DynamicWriteOnly,0);//stride暂时设为0,当vert stream拷贝到vb时的时候,stride会被复制
			return pVB;
		}
		void Free(void *pMem)
		{
			IVertBuffer *pVB=(IVertBuffer *)pMem;
			delete pVB;
		}
	};

	class DynaIBAlloc
	{
	public:
		void *Alloc(UINT size)
		{
			IDevice *pDev=Device();
			IIndexBuffer *pIB=pDev->NewIB();
			pIB->CreateWord(size,EB_DynamicWriteOnly);
			return pIB;
		}
		void Free(void *pMem)
		{
			IIndexBuffer *pIB=(IIndexBuffer *)pMem;
			delete pIB;
		}
	};

	class StaticVBAlloc
	{
	public:
		void *Alloc(UINT size)
		{
			IDevice *pDev=Device();
			IVertBuffer *pVB=pDev->NewVB();
			pVB->Create(size,EB_StaticWriteOnly,0);//stride暂时设为0,当vert stream拷贝到vb时的时候,stride会被复制
			return pVB;
		}
		void Free(void *pMem)
		{
			IVertBuffer *pVB=(IVertBuffer *)pMem;
			delete pVB;
		}
	};

	class StaticIBAlloc
	{
	public:
		void *Alloc(UINT size)
		{
			IDevice *pDev=Device();
			IIndexBuffer *pIB=pDev->NewIB();
			pIB->CreateWord(size,EB_StaticWriteOnly);
			return pIB;
		}
		void Free(void *pMem)
		{
			IIndexBuffer *pIB=(IIndexBuffer *)pMem;
			delete pIB;
		}
	};


	template<typename Allocer = DynaVBAlloc>
	class HardwareBufferPool
	{
		struct tagBlock
		{
			void	*pMem;
			int		blockSize;
			bool	bUsed;
			int		useCount;
		};
	public:
		HardwareBufferPool(void)
		{
			m_totalSize = 0;
			m_lastIndex = 0;
			m_maxUseSize = 0;
			m_notHitTimes = 0;
		}
		~HardwareBufferPool(void)
		{
		}

		void* Alloc(int size)
		{
			if(size > m_maxUseSize)
				m_maxUseSize = size;

			vector<tagBlock*> freeBlocks;
			for(int i=0;i<(int)m_blocks.size();i++)
			{
				tagBlock& block = m_blocks[i];
				if(block.bUsed)
					continue;
				if(block.blockSize >= size)
					freeBlocks.push_back(&block);
			}

			if(freeBlocks.size() > 0)
			{
				int r = (m_lastIndex++)%freeBlocks.size();

				tagBlock* pBlock = freeBlocks[r];
				pBlock->bUsed = true;
				pBlock->useCount++;
				return pBlock->pMem;
			}
			else
			{
				m_notHitTimes++;

				//--alloc new
				Allocer a;
				return a.Alloc(size);
			}
		}

		void Free(void *pMem)
		{
			for(size_t i=0;i<m_blocks.size();i++)
			{
				tagBlock& block=m_blocks[i];
				if(block.pMem==pMem)
				{
					block.bUsed = false;
					return;
				}
			}
			
			//--free
			Allocer a;
			a.Free(pMem);
		}


		void Init(const vector<int>& blockCfg)
		{
			m_totalSize=0;

			Allocer a;
			for(int i=0;i<(int)blockCfg.size();i++)
			{
				tagBlock newBlock;
				newBlock.blockSize = blockCfg[i];
				newBlock.pMem = a.Alloc(newBlock.blockSize);
				newBlock.bUsed = false;
				newBlock.useCount = 0;
				m_blocks.push_back(newBlock);

				m_totalSize+=newBlock.blockSize;
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

		void Report(tstring& out,bool bDetail=false)
		{
			out=_T("HardwareBufferPool report----\r\n");
			TCHAR tmp[512];
			DWORD total=0;
			if(bDetail)
			{
				for(size_t i=0;i<m_blocks.size();i++)
				{
					tagBlock& block=m_blocks[i];
					_stprintf_s(tmp,_T("index = %d, use count=%d\r\n"),i,block.useCount);
					out+=tmp;
				}
			}
			_stprintf_s(tmp,_T("total size = %dk\r\n"),m_totalSize/1024);
			out += tmp;
			_stprintf_s(tmp,_T("max used size = %dk\r\n"),m_maxUseSize/1024);
			out += tmp;
			_stprintf_s(tmp,_T("not hit times = %d\r\n"),m_notHitTimes);
			out += tmp;
			out += _T("-------------------------------------------\r\n");
		}

	private:
		int m_totalSize;
		int	m_maxUseSize;
		int	m_lastIndex;
		int	m_notHitTimes;
		vector<tagBlock>	m_blocks;
	};
}//namespace Cool3D