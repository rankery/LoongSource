#include "StdAfx.h"
#include "Bmp.h"
#include <stdio.h>
#include <stdlib.h>

int BMP::bread() 
{
	return fgetc(m_fp);
}

int BMP::wread()
{
	Byte h, l;

	l = (Byte) bread();
	h = (Byte) bread();
	return (h << 8) | l;
}

UINT BMP::dwread()
{
	WORD h, l;
	l = (WORD) wread();
	h = (WORD) wread();
	return (h << 16) | l;
}

BMP::BMP()
{
}

NV_ERROR_CODE BMP::ReadBMP(const char * filename, RGBAImage & image)
{
	FILE *fp = fopen( filename, "rb" );

	if( NULL != fp )
		return ReadBMP(fp,image);
	else
	{
		return NV_CANT_OPEN_INPUT_FILE;
	}
}

BMP::~BMP()
{
	Release();
}

void BMP::Release()
{

}

NV_ERROR_CODE BMP::ReadBMP(FILE * fp, RGBAImage & image)
{
	int a, r, g, b;

	Release();

	unsigned short bits16;
	m_fp = fp;

	wread();
	m_header.fileSize = dwread();
	m_header.reserved0 = dwread();
	m_header.bitmapDataOffset = dwread();
	m_header.bitmapHeaderSize = dwread();
	m_header.width = dwread();
	m_header.height = dwread();
	m_header.planes = wread();
	m_header.bitsPerPixel = wread();
	m_header.compression = dwread();
	m_header.bitmapDataSize = dwread();
	m_header.hRes = dwread();
	m_header.vRes = dwread();
	m_header.colors = dwread();
	m_header.importantColors = dwread();

	image.resize(m_header.width, m_header.height);

	size_t indexX,indexY;
	switch(m_header.bitsPerPixel)
	{
	case 16:
		for(UINT y=0; y<m_header.height; y++)
		{
			for(UINT x=0; x<m_header.width; x++)
			{
				indexX=x;
				indexY=m_header.height-y-1;

				rgba_t& pix = image(indexY, indexX);

				bits16 = wread();
				b = bits16 & 0x1F;
				g = (bits16 >> 5) & 0x1F;
				r = (bits16 >> 10) & 0x1F;

				b <<= 3;
				g <<= 3;
				r <<= 3;

				pix.r = r;
				pix.g = g;
				pix.b = b;
				pix.a = 0xFF;
			}
		}
		break;

	case 24:
		for(UINT y=0; y<m_header.height; y++)
		{
			for(UINT x=0; x<m_header.width; x++)
			{
				indexX=x;
				indexY=m_header.height-y-1;

				rgba_t& pix = image(indexY, indexX);

				int b = bread();
				int g = bread();
				int r = bread();

				pix.r = r;
				pix.g = g;
				pix.b = b;
				pix.a = 0xFF;
			}
		}
		break;
	case 32:
		for(UINT y=0; y<m_header.height; y++)
		{
			for(UINT x=0; x<m_header.width; x++)
			{
				indexX=x;
				indexY=m_header.height-y-1;

				rgba_t& pix = image(indexY, indexX);

				b = bread();
				g = bread();
				r = bread();
				a = bread();

				pix.r = r;
				pix.g = g;
				pix.b = b;
				pix.a = a;
			}
		}
		break;
	default:
		break;
	}

	fclose(m_fp);

	return NV_OK;
}