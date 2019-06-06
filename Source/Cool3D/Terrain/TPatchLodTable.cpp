#include "StdAfx.h"
#include ".\tpatchlodtable.h"
#include "..\Util\DebugMisc.h"

namespace Cool3D
{

	TPatchLodTable::TPatchLodTable(void)
	{
		m_width=0;
		m_height=0;
		m_defaultLod=0;
		m_pTable=NULL;
	}

	TPatchLodTable::~TPatchLodTable(void)
	{
		SAFE_DELETEA(m_pTable);
	}

	TPatchLodTable* TPatchLodTable::Inst()
	{
		static TPatchLodTable Inst;
		return &Inst;
	}

	TPatchLodTable* TPatchLodTable::Inst_SimpleTerrain()
	{
		static TPatchLodTable Inst;
		return &Inst;
	}

	void TPatchLodTable::Create(int width,int height)
	{
		Destroy();

		m_width=width;
		m_height=height;
		m_pTable=new BYTE[width*height];
		ZeroMemory(m_pTable,m_width*m_height);
	}

	void TPatchLodTable::Destroy()
	{
		SAFE_DELETEA(m_pTable);
	}

	void TPatchLodTable::Clear(int lod)
	{
		m_defaultLod=lod;
		memset(m_pTable,lod,m_width*m_height);
	}

	void TPatchLodTable::SafeSetLod(int x,int z,int lod)
	{
		if(x<m_width && z<m_height)
			m_pTable[z*m_width+x]=(BYTE)lod;
	}

	int TPatchLodTable::SafeGetLod(int x,int z)
	{
		if(x<0 || x>=m_width)
			return m_defaultLod;
		if(z<0 || z>=m_height)
			return m_defaultLod;

		return m_pTable[z*m_width+x];
	}

}//namespace Cool3D