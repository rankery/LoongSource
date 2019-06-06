#include "StdAfx.h"
#include "AreaMap.h"

namespace WorldBase
{
	AreaMap::AreaMap(void)
	{}

	AreaMap::~AreaMap(void)
	{}

	void AreaMap::ReadFile( IFS* pFS,DWORD hFile )
	{
		FReadValue(pFS,hFile,m_type);
		if(m_type==0)
			FReadValue(pFS,hFile,m_oneAreaIndex);
		else if(m_type==1)
			m_bitmap.ReadFile(pFS,hFile);
		else if(m_type==2)
			m_halfByteMap.ReadFile(pFS,hFile);
		else
			ASSERT(0);
	}


	void AreaMap::WriteFile( FILE* fp )
	{
		FWriteValue(fp,m_type);
		if(m_type==0)
			FWriteValue(fp,m_oneAreaIndex);
		else if(m_type==1)
			m_bitmap.WriteFile(fp);
		else if(m_type==2)
			m_halfByteMap.WriteFile(fp);
		else
			ASSERT(0);
	}

	int AreaMap::GetValue( int x,int z )
	{
		if(m_type==0)
			return m_oneAreaIndex;
		else if(m_type==1)
			return m_bitmap.SafeGetValue(x,z)?1:0;
		else if(m_type==2)
			return m_halfByteMap.SafeGetValue(x,z);
		else
		{
			ASSERT(0);
			return 0;
		}
	}

	int AreaMap::SafeGetValue( int x,int z )
	{
		if(m_type==0)
			return m_oneAreaIndex;
		else if(m_type==1)
			return m_bitmap.SafeGetValue(x,z)?1:0;
		else if(m_type==2)
			return m_halfByteMap.SafeGetValue(x,z);
		else
		{
			ASSERT(0);
			return 0;
		}
	}

	void AreaMap::Create( HalfByteMap* pSource )
	{
		//--统计最大值和最小值
		int maxValue=0;
		int minValue=16;
		UINT x,z;
		for(z=0;z<pSource->Height();z++)
		{
			for(x=0;x<pSource->Width();x++)
			{
				if(pSource->GetValue(x,z)>maxValue)
					maxValue=pSource->GetValue(x,z);
				if(pSource->GetValue(x,z)<minValue)
					minValue=pSource->GetValue(x,z);
			}
		}

		//--
		if(maxValue==minValue)//只有一个区域
		{
			m_type=0;
			m_oneAreaIndex=(BYTE)minValue;
		}
		else if(minValue==0&&maxValue==1)//只有两个区域
		{
			m_type=1;

			m_bitmap.Create(pSource->Width(),pSource->Height(),false);
			for(z=0;z<pSource->Height();z++)
			{
				for(x=0;x<pSource->Width();x++)
				{
					if(pSource->GetValue(x,z)==1)
						m_bitmap.SetValue(x,z,true);
				}
			}
		}
		else//多个区域
		{
			m_type=2;
			pSource->Clone(&m_halfByteMap);
		}
	}

}//namespace WorldBase
