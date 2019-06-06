#include "StdAfx.h"
#include ".\bitmap.h"
#include "..\Util\Exception.h"
#include "..\VFS\IFS.h"
#include "..\VFS\FileUtil.h"
#include "..\Image\Image.h"

namespace Cool3D
{
	const int US=32;//unit size in bits
	const DWORD HEX1=((unsigned) 0x00000001);

	BitMap::BitMap(void)
	{
		m_height=m_width=0;
		m_pData=NULL;
		m_numBytes=0;
	}

	BitMap::~BitMap(void)
	{
		Destroy();
	}

	void BitMap::Clone(BitMap *pTarget)
	{
		pTarget->Create(m_width,m_height,1);
		ASSERT(m_numBytes==pTarget->m_numBytes);
		memcpy(pTarget->m_pData,m_pData,m_numBytes);
	}

	void BitMap::Create(int w,int h,bool initVal)
	{
		Destroy();
		//--
		int numDWords=((w*h)/US)+1;
		m_numBytes=numDWords*US/8;
		m_pData=(DWORD*)malloc(m_numBytes);
		if( initVal )
			memset(m_pData,0xFF,m_numBytes);
		else
			memset(m_pData,0,m_numBytes);
		m_width=w;
		m_height=h;
	}

	void BitMap::Destroy()
	{
		SAFE_FREE(m_pData);
		m_width=m_height=0;
		m_numBytes=0;
	}

	bool BitMap::GetValue(int x,int y) const
	{
		ASSERT(x>=0 && x<m_width);
		ASSERT(y>=0 && y<m_height);
		int bit=m_width*y+x;
		//ASSERT(bit < m_numBytes);
		return (*(m_pData+bit/US) & (HEX1 << (US-1-(bit % US))))!=0;
	}

	bool BitMap::SafeGetValue( int x,int y ) const
	{
		if(x<0)x=0;
		if(x>=m_width)x=m_width-1;
		if(y<0)y=0;
		if(y>=m_height)y=m_height-1;
		int bit=m_width*y+x;
		return (*(m_pData+bit/US) & (HEX1 << (US-1-(bit % US))))!=0;
	}

	void BitMap::SetValue(int x,int y,bool val)
	{
		ASSERT(x>=0 && x<m_width);
		ASSERT(y>=0 && y<m_height);
		int bit=m_width*y+x;
		//ASSERT(bit < m_numBytes);
		if(val)
			*(m_pData+bit/US) |= (HEX1 << (US-1-(bit % US)));//set bit to 1
		else
			*(m_pData+bit/US) &= (~(HEX1 << (US-1-(bit % US))));//set bit to 0
	}

	void BitMap::WriteFile(FILE *fp)
	{
		ASSERT(m_pData!=NULL);

		FWriteValue(fp,m_width);
		FWriteValue(fp,m_height);
		fwrite(m_pData,m_numBytes,1,fp);
	}
	
	void BitMap::ReadFile(IFS *pFS,DWORD hFile)
	{
		FReadValue(pFS,hFile,m_width);
		FReadValue(pFS,hFile,m_height);

		Create(m_width,m_height,false);
		pFS->Read(m_pData,m_numBytes,hFile);
	}

	void BitMap::SafeSetValue( int x,int y,bool val )
	{
		if(x<0 || x>=m_width)
			return;
		if(y<0 || y>=m_height)
			return;
		SetValue(x,y,val);
	}

	void BitMap::ExportImage( TCHAR* szFileName )
	{
		Image tmp;
		tmp.Create(Width(),Height(),EPF_A8);
		BYTE* pData=(BYTE*)tmp.GetRawBuffer();

		for(UINT z=0;z<Height();z++)
		{
			for(UINT x=0;x<Width();x++)
			{
				if(GetValue(x,z))
					*pData=255;
				else
					*pData=0;
				pData++;
			}
		}

		tmp.WriteToFile(szFileName);
	}

	void BitMap::CopyTo( BitMap& out,int destx,int desty ) const
	{
		ASSERT(destx+m_width<(int)out.Width());
		ASSERT(desty+m_height<(int)out.Height());

		int x,y;
		for(y=0;y<m_height;++y)
		{
			for(x=0;x<m_width;++x)
			{
				out.SafeSetValue(x+destx,y+desty,GetValue(x,y));
			}
		}
	}

	void BitMap::CopyTo( BitMap& out,int srcx,int srcy,int w,int h,int destx,int desty ) const
	{
		ASSERT(destx+w<(int)out.Width() && desty+h<(int)out.Height());
		ASSERT(srcx+w<m_width && srcy+h<m_height);

		int x,y;
		for(y=0;y<h;++y)
		{
			for(x=0;x<w;++x)
			{
				out.SafeSetValue(x+destx,y+desty,SafeGetValue(srcx+x,srcy+y));
			}
		}
	}
}//namespace Cool3D