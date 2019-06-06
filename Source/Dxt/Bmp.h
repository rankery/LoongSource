#pragma once

#include <dxtlib/dxtlib.h>
#include <dds\nvErrorCodes.h>
#include <stdio.h>
typedef unsigned char Byte;

class BMP
{
	typedef struct
	{
		char id[2];
		unsigned long fileSize;
		unsigned long reserved0;
		unsigned long bitmapDataOffset;
		unsigned long bitmapHeaderSize;
		unsigned long width;
		unsigned long height;
		unsigned short planes;
		unsigned short bitsPerPixel;
		unsigned long compression;
		unsigned long bitmapDataSize;
		unsigned long hRes;
		unsigned long vRes;
		unsigned long colors;
		unsigned long importantColors;
	} BMPHeader_t;


public:
	BMP();
	~BMP();

	NV_ERROR_CODE ReadBMP(const char * filename, RGBAImage & image);
	NV_ERROR_CODE ReadBMP(FILE * fp, RGBAImage & image);

	int GetDepth() const { return m_header.bitsPerPixel; }

	int GetWidth() const { return m_header.width; }
	int GetHeight()const { return m_header.height; }

	void Release();

private:
	FILE *m_fp;
	BMPHeader_t m_header;

	int wread(void);
	UINT dwread(void);
	int bread(void);
	void wwrite(unsigned short data);
	void bwrite(unsigned char data);

};