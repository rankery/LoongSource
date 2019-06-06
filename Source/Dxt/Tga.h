#include <float.h>


#include <windows.h>
#include <dxtlib/dxtlib.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <io.h>
#include <string>
#include <stdio.h>

#include <dds\nvErrorCodes.h>

typedef unsigned char Byte;

#define LR 0x10
#define BT 0x20
  
#define TGA_SWAP            0x0001
#define TGA_PAD             0x0002
#define TGA_COMPRESSED      0x0004
#define TGA_FLIP_VERTICAL   0x008

class TGA
{
public:
    TGA();
    ~TGA();

    

    NV_ERROR_CODE ReadTGA(const char * filename, RGBAImage & image);
    NV_ERROR_CODE ReadTGA(FILE * fp, RGBAImage & image);

    int WriteTGA(const char *filename, RGBAImage & image, bool bFlipVertical);


    int GetRLECount(unsigned char *data);
    int GetRawPixelCount(unsigned char *data);

	int GetDepth() const { return m_depth; }

	int GetWidth() const { return m_width; }
	int GetHeight()const { return m_height; }


    //unsigned char * GetDataPointer() {return m_data;}
    //unsigned short GetWidth() {return m_width;}
    //unsigned short GetHeight() {return m_height;}
    //unsigned int GetBytesPerPixel() { return m_bytes_per_pixel;}
    void Release();

private:
    unsigned char m_id_length;          /* 0 */
    unsigned char m_color_map_type;     /* 1 */
    unsigned char m_type;         /* 2 */
    unsigned short m_cm_index;            /* 3 4 */
    unsigned short m_cm_length;           /* 5 6 */
    unsigned char m_cm_entry_size;      /* 7 */
    unsigned short m_x_org;         /* 8 9 */
    unsigned short m_y_org;         /* 10 11 */
    unsigned short m_width;         /* 12 13 */
    unsigned short m_height;        /* 14 15 */
    unsigned char m_depth;        /* 16 */
    unsigned char m_desc;         /* 17 */

    unsigned char m_id[256];

    unsigned char m_color_palette[256 * 3];

    FILE *m_fp;


    //int m_size;
    //int m_size_in_bytes;
    //int m_bytes_per_pixel;


    int wread(void);
    int bread(void);
    void wwrite(unsigned short data);
    void bwrite(unsigned char data);

};
