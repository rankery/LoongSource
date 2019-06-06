#include "StdAfx.h"
#include "HalfByteMap.h"

namespace WorldBase
{

	HalfByteMap::HalfByteMap(void)
	{
		m_height=m_width=0;
		m_pData=NULL;
		m_numBytes=0;
	}

	HalfByteMap::~HalfByteMap(void)
	{
		Destroy();
	}

	void HalfByteMap::Create( int w,int h,int initVal )
	{
		Destroy();
		//--
		m_numBytes=w*h/2+1;
		m_pData=(BYTE*)malloc(m_numBytes);
		m_width=w;
		m_height=h;

		for(int y=0;y<h;y++)
			for(int x=0;x<w;x++)
				SetValue(x,y,initVal);
	}

	void HalfByteMap::Destroy()
	{
		SAFE_FREE(m_pData);
		m_width=m_height=0;
		m_numBytes=0;
	}

	int HalfByteMap::GetValue( int x,int y ) const
	{
		ASSERT(x>=0 && x<m_width);
		ASSERT(y>=0 && y<m_height);

		int halfByte=m_width*y+x;
		int Byte=halfByte/2;
		if(halfByte%2==0)
			return m_pData[Byte]>>4;
		else
			return m_pData[Byte]&0x0F;
	}

	int HalfByteMap::SafeGetValue( int x,int y ) const
	{
		if(x<0)x=0;
		if(x>=m_width)x=m_width-1;
		if(y<0)y=0;
		if(y>=m_height)y=m_height-1;

		int halfByte=m_width*y+x;
		int Byte=halfByte/2;
		if(halfByte%2==0)
			return m_pData[Byte]>>4;
		else
			return m_pData[Byte]&0x0F;
	}

	void HalfByteMap::SetValue( int x,int y,int val )
	{
		ASSERT(x>=0 && x<m_width);
		ASSERT(y>=0 && y<m_height);

		int halfByte=m_width*y+x;
		int Byte=halfByte/2;
		if(halfByte%2==0)
		{
			m_pData[Byte]&=0x0F;
			m_pData[Byte]|=((val&0x000F)<<4);
		}
		else
		{
			m_pData[Byte]&=0xF0;
			m_pData[Byte]|=(val&0x000F);
		}
	}

	void HalfByteMap::WriteFile( FILE *fp )
	{
		ASSERT(m_pData!=NULL);

		FWriteValue(fp,m_width);
		FWriteValue(fp,m_height);
		fwrite(m_pData,m_numBytes,1,fp);
	}

	void HalfByteMap::ReadFile( IFS *pFS,DWORD hFile )
	{
		FReadValue(pFS,hFile,m_width);
		FReadValue(pFS,hFile,m_height);

		Create(m_width,m_height,false);
		pFS->Read(m_pData,m_numBytes,hFile);
	}

	void HalfByteMap::ExportImage( const TCHAR* szImageFile )
	{
		Image tmp;
		tmp.Create(Width(),Height(),EPF_A8);
		BYTE* pData=(BYTE*)tmp.GetRawBuffer();

		for(UINT z=0;z<Height();z++)
		{
			for(UINT x=0;x<Width();x++)
			{
				*pData=GetValue(x,z)*16;
				pData++;
			}
		}

		tmp.WriteToFile(szImageFile);
	}

	void HalfByteMap::Clone( HalfByteMap* pTarget )
	{
		ASSERT(m_pData!=NULL);

		pTarget->Destroy();
		//--
		pTarget->m_numBytes=m_numBytes;
		pTarget->m_width=m_width;
		pTarget->m_height=m_height;
		pTarget->m_pData=(BYTE*)malloc(m_numBytes);
		memcpy(pTarget->m_pData,m_pData,m_numBytes);	
	}

}//namespace WorldBase
