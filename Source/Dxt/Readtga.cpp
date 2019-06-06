#include "StdAfx.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "tga.h"

int TGA::bread() 
{
    return fgetc(m_fp);
}

int TGA::wread()
{
    Byte h, l;

    l = (Byte) bread();
    h = (Byte) bread();
    return (h << 8) | l;
}

/*

Use this to read the whole tga file, allocate memory and return a pointer to the memory,

reads 24/32 tga -> 24 or 32 bpp
does not support 8 bpp palette

flags
swap: 1 = swaps rgb position
pad = pads 24 bpp to 32 bpp (sets alpha to 0xFF)



*/


TGA::TGA()
{
}

NV_ERROR_CODE TGA::ReadTGA(const char * filename, RGBAImage & image)
{
    FILE * fp = fopen( filename, "rb" );

    if( NULL != fp )
        return ReadTGA(fp,image);
    else
    {
        return NV_CANT_OPEN_INPUT_FILE;
    }

    
}

TGA::~TGA()
{
    Release();

}

void TGA::Release()
{

}

NV_ERROR_CODE TGA::ReadTGA(FILE * fp, RGBAImage & image)
{
    int a, r, g, b;

    Release();

    unsigned short bits16;
    m_fp = fp;

    m_id_length = bread();
    m_color_map_type = bread();
    m_type = bread();

    if (!(m_type == 2 || m_type == 10))
    {
        //printf("Only type 2 or 10 supported\n");

        return NV_UNSUPPORTED_FORMAT;
    }
    m_cm_index = wread();
    m_cm_length = wread();

    m_cm_entry_size = bread();

    m_x_org = wread();
    m_y_org = wread();
    m_width = wread();
    m_height = wread();

    m_depth = bread();
    m_desc = bread();

	image.resize(m_width, m_height);

	bool bFlipLeftRight = (m_desc & LR)!=0;  // swap left/right, not supported
	bool bFlipVertical = (m_desc & BT)!=0;    // file requested flip

    //m_size_in_bytes = m_width * m_height * m_bytes_per_pixel;

    fread((char *) m_id, m_id_length, 1, m_fp);
    fread((char *) m_color_palette, m_cm_length, m_cm_entry_size >> 3, m_fp);
    
    //m_size = (int) m_height *(int) m_width;

    if (m_type == 2)
	{
		size_t indexX,indexY;
        switch(m_depth)
        {
        case 16:
            for (int y = 0; y < m_height; y++)
            {
    
                for (int x = 0; x < m_width; x++)
                {
					if(bFlipVertical)
					{
						indexX=x;
						indexY=y;
					}
					else
					{
						indexX=x;
						indexY=m_height-y-1;
					}
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
            for (int y = 0; y < m_height; y++)
            {

                for (int x = 0; x < m_width; x++)
                {
					if(bFlipVertical)
					{
						indexX=x;
						indexY=y;
					}
					else
					{
						indexX=x;
						indexY=m_height-y-1;
					}
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
            for (int y = 0; y < m_height; y++)
            {

                for (int x = 0; x < m_width; x++)
				{
					if(bFlipVertical)
					{
						indexX=x;
						indexY=y;
					}
					else
					{
						indexX=x;
						indexY=m_height-y-1;
					}
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
    }
    else if (m_type == 10)   //RLE TRUE color
    {
        return NV_UNSUPPORTED_FORMAT;
    }

    fclose(m_fp);


    return NV_OK;
}