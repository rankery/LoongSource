#include "StdAfx.h"
#include ".\imagetga.h"
#include "..\Kernel.h"
#include "..\VFS\IFS.h"
#include "..\Util\Exception.h"
#include "..\Util\FMemory.h"

namespace Cool3D
{
	ImageTGA::ImageTGA(void)
	{
		m_imageData=NULL;
		m_bpp=m_width=m_height=0;
	}

	ImageTGA::~ImageTGA(void)
	{
		Destroy();
	}

	void ImageTGA::Create(UINT width,UINT height,UINT bpp)
	{
		ASSERT(bpp==32||bpp==24||bpp==8);
		ASSERT(width>0&&height>0);
		
		Destroy();

		m_width=width;
		m_height=height;
		m_bpp=bpp;

		UINT bytesPerPixel=bpp/8;
		m_imageData=(BYTE*)malloc(bytesPerPixel*width*height);
	}

	void ImageTGA::Destroy()
	{
		SAFE_FREE(m_imageData);
		m_bpp=m_width=m_height=0;
	}

	bool ImageTGA::LoadFromFile(IFS *pFS,const TCHAR* szFileName)
	{
		typedef struct
		{
			BYTE	Header[12];	// TGA File Header
		} TGAHeader;

		//--
		if(pFS==NULL || szFileName==NULL)
			return false;

		DWORD hFile=pFS->Open(szFileName);
		if(hFile==NULL)
		{
			Kernel::Inst()->SetLastError(_T("Image TGA file open failed,%s"),szFileName);
			return false;
		}

		TGAHeader tgaheader; // TGA header
		pFS->Read(&tgaheader,sizeof(tgaheader),hFile);

		BYTE uTGAcompare[12] = {0,0,2, 0,0,0,0,0,0,0,0,0};	// Uncompressed TGA Header
		BYTE ubTGAcompare[12] = {0,0,3, 0,0,0,0,0,0,0,0,0};	// Uncompressed Black TGA Header
		BYTE cTGAcompare[12] = {0,0,10,0,0,0,0,0,0,0,0,0};	// Compressed TGA Header

		
		if(memcmp(uTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)// See if header matches the predefined header of 
		{															// an Uncompressed TGA image
			LoadUncompressedTGA(pFS,hFile);						// If so, jump to Uncompressed TGA loading code
		}
		else if(memcmp(ubTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)		// See if header matches the predefined header of
		{																	// an RLE compressed TGA image
			LoadUncompressedBlackTGA(pFS,hFile);							// If so, jump to Compressed TGA loading code
		}
		else if(memcmp(cTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)		// See if header matches the predefined header of
		{																	// an RLE compressed TGA image
			LoadCompressedTGA(pFS,hFile);							// If so, jump to Compressed TGA loading code
		}
		else
		{
			Kernel::Inst()->SetLastError(_T("Image TGA unknown format,%s"),szFileName);
			pFS->Close(hFile);
			return false;
		}

		//--
		pFS->Close(hFile);
		return true;
	}

	bool ImageTGA::LoadUncompressedTGA(IFS *pFS,DWORD hFile)
	{
		TGA tga;

		if(pFS->Read(tga.header,sizeof(tga.header),hFile)==0)	// Attempt to read header
		{
			TRACE(_T("Could not read info header\n"));
			return false;
		}
		
		m_width  = tga.header[1] * 256 + tga.header[0];					// Determine The TGA Width	(highbyte*256+lowbyte)
		m_height = tga.header[3] * 256 + tga.header[2];					// Determine The TGA Height	(highbyte*256+lowbyte)
		m_bpp	 = tga.header[4];										// Determine Bits Per Pixel
		tga.Width		= m_width;										// Copy width to local structure
		tga.Height		= m_height;										// Copy width to local structure
		tga.Bpp			= m_bpp;										// Copy width to local structure

		if((m_width <= 0) || (m_height <= 0) 
			|| ((m_bpp != 24) && (m_bpp !=32) && (m_bpp !=8)))	//Make sure all texture info is ok
		{
			TRACE(_T("Invalid texture information\n"));
			return false;
		}

		tga.bytesPerPixel	= (tga.Bpp / 8);									// Compute BYTES per pixel
		tga.imageSize		= (tga.bytesPerPixel * tga.Width * tga.Height);		// Compute amout of memory needed to store image
		m_imageData	= (BYTE *)malloc(tga.imageSize);					// Allocate that much memory

		if(m_imageData == NULL)											// If it wasnt allocated correctly..
		{
			TRACE(_T("Could not allocate memory for image\n"));
			return false;
		}
#if 0
		if(pFS->Read(m_imageData, tga.imageSize, hFile) != tga.imageSize)	// Attempt to read image data
		{
			TRACE(_T("Could not read image data\n"));
			return false;
		}
#else
		//vflip
		int imgPitch=tga.Width*tga.bytesPerPixel;
		BYTE *pData=m_imageData+imgPitch*(tga.Height-1);
		for(UINT i=0;i<tga.Height;i++)
		{
			pFS->Read(pData,imgPitch,hFile);
			pData-=imgPitch;
		}
#endif

		// Byte Swapping Optimized By Steve Thomas
		/*for(int cswap = 0; cswap < (int)tga.imageSize; cswap += tga.bytesPerPixel)
		{
			m_imageData[cswap] ^= m_imageData[cswap+2] ^=
				m_imageData[cswap] ^= m_imageData[cswap+2];
		}*/

		return true;
	}

	bool ImageTGA::LoadUncompressedBlackTGA(IFS *pFS,DWORD hFile)
	{
		TGA tga;

		if(pFS->Read(tga.header,sizeof(tga.header),hFile)==0)	// Attempt to read header
		{
			TRACE(_T("Could not read info header\n"));
			return false;
		}

		m_width  = tga.header[1] * 256 + tga.header[0];					// Determine The TGA Width	(highbyte*256+lowbyte)
		m_height = tga.header[3] * 256 + tga.header[2];					// Determine The TGA Height	(highbyte*256+lowbyte)
		m_bpp	 = tga.header[4];										// Determine Bits Per Pixel
		tga.Width		= m_width;										// Copy width to local structure
		tga.Height		= m_height;										// Copy width to local structure
		tga.Bpp			= m_bpp;										// Copy width to local structure

		if((m_width <= 0) || (m_height <= 0) 
			|| (m_bpp !=8))	//Make sure all texture info is ok
		{
			TRACE(_T("Invalid texture information\n"));
			return false;
		}

		tga.bytesPerPixel	= (tga.Bpp / 8);									// Compute BYTES per pixel
		tga.imageSize		= (tga.bytesPerPixel * tga.Width * tga.Height);		// Compute amout of memory needed to store image
		m_imageData	= (BYTE *)malloc(tga.imageSize);					// Allocate that much memory

		if(m_imageData == NULL)											// If it wasnt allocated correctly..
		{
			TRACE(_T("Could not allocate memory for image\n"));
			return false;
		}
#if 1
		if(pFS->Read(m_imageData, tga.imageSize, hFile) != tga.imageSize)	// Attempt to read image data
		{
			TRACE(_T("Could not read image data\n"));
			return false;
		}
#else
		//vflip
		int imgPitch=tga.Width*tga.bytesPerPixel;
		BYTE *pData=m_imageData+imgPitch*(tga.Height-1);
		for(UINT i=0;i<tga.Height;i++)
		{
			pFS->Read(pData,imgPitch,hFile);
			pData-=imgPitch;
		}
#endif

		// Byte Swapping Optimized By Steve Thomas
		/*for(int cswap = 0; cswap < (int)tga.imageSize; cswap += tga.bytesPerPixel)
		{
		m_imageData[cswap] ^= m_imageData[cswap+2] ^=
		m_imageData[cswap] ^= m_imageData[cswap+2];
		}*/

		return true;
	}

	bool ImageTGA::LoadCompressedTGA(IFS *pFS,DWORD hFile)
	{
		//TGA tga;
		ASSERT(0 && _T("todo"));

		return true;
	}

	void ImageTGA::ApplyAlpha(ImageTGA *pSrcImg)
	{
		if(m_imageData==NULL)
			return;
		if(pSrcImg==NULL || pSrcImg->GetData()==NULL)
			return;

		ASSERT(pSrcImg->GetWidth()==m_width);
		ASSERT(pSrcImg->GetHeight()==m_height);
		ASSERT(pSrcImg->GetBpp()==32||pSrcImg->GetBpp()==8);
		ASSERT(m_bpp==32);

		if(pSrcImg->GetBpp()==32)
		{
			tagPixel *pDstData=(tagPixel*)m_imageData;
			tagPixel *pSrcData=(tagPixel*)pSrcImg->GetData();

			int x,y;
			tagPixel *pDP;
			tagPixel *pSP;

			for(y=0;y<m_height;y++)
			{
				for(x=0;x<m_width;x++)
				{
					pDP=&pDstData[y*m_width+x];
					pSP=&pSrcData[y*m_width+x];
					pDP->A=pSP->A;
				}
			}//endof for(y);		
		}
		else if(pSrcImg->GetBpp()==8)
		{
			tagPixel *pDstData=(tagPixel*)m_imageData;
			BYTE *pSrcData=pSrcImg->GetData();

			int x,y;
			tagPixel *pDP;
			BYTE *pSP;

			for(y=0;y<m_height;y++)
			{
				for(x=0;x<m_width;x++)
				{
					pDP=&pDstData[y*m_width+x];
					pSP=&pSrcData[y*m_width+x];
					pDP->A=*pSP;
				}
			}//endof for(y);		
		}

	}

	void ImageTGA::AlphaBlend(ImageTGA *pSrcImg)
	{
		if(m_imageData==NULL)
			return;
		if(pSrcImg==NULL || pSrcImg->GetData()==NULL)
			return;

		ASSERT(pSrcImg->GetWidth()==m_width);
		ASSERT(pSrcImg->GetHeight()==m_height);
		ASSERT(pSrcImg->GetBpp()==32);
		ASSERT(m_bpp==32);

		tagPixel *pDstData=(tagPixel*)m_imageData;
		tagPixel *pSrcData=(tagPixel*)pSrcImg->GetData();

		int x,y,t,a,inva;
		tagPixel *pDP;
		tagPixel *pSP;

		for(y=0;y<m_height;y++)
		{
			for(x=0;x<m_width;x++)
			{
				pDP=&pDstData[y*m_width+x];
				pSP=&pSrcData[y*m_width+x];
				a=pSP->A;
				inva=255-a;

				t=(pSP->R*a+pDP->R*inva)/255;
				pDP->R=t;
				t=(pSP->G*a+pDP->G*inva)/255;
				pDP->G=t;
				t=(pSP->B*a+pDP->B*inva)/255;
				pDP->B=t;
			}
		}//endof for(y);
	}

	void ImageTGA::AddColor(int a,int r,int g,int b)
	{
		if(m_imageData==NULL)
			return;

		ASSERT(m_bpp==32);
		if(a==0
			&&r==0
			&&g==0
			&&b==0)
			return;

		tagPixel *pData=(tagPixel *)m_imageData;
		if(pData==NULL)
			return;
		int numPixel=m_width*m_height;
		int t;
		for(int i=0;i<numPixel;i++)
		{
			t=pData->A+a;
			if(t<0) t=0; if(t>255) t=255;
			pData->A=t;

			t=pData->R+r;
			if(t<0) t=0; if(t>255) t=255;
			pData->R=t;

			t=pData->G+g;
			if(t<0) t=0; if(t>255) t=255;
			pData->G=t;

			t=pData->B+b;
			if(t<0) t=0; if(t>255) t=255;
			pData->B=t;
			pData++;
		}//endof for
	}

	void ImageTGA::MulColor(Color4f color)
	{
		if(m_imageData==NULL)
			return;

		ASSERT(m_bpp==32);
		if(color.A==1.0f
			&& color.R==1.0f
			&& color.G==1.0f
			&& color.B==1.0f)
			return;

		tagPixel *pData=(tagPixel*)m_imageData;
		if(pData==NULL)
			return;
		int numPixel=m_width*m_height;
		for(int i=0;i<numPixel;i++)
		{
			pData->A=BYTE(pData->A*color.A);
			pData->R=BYTE(pData->R*color.R);
			pData->G=BYTE(pData->G*color.G);
			pData->B=BYTE(pData->B*color.B);
			pData++;
		}
	}

	void ImageTGA::ConvertTo32Bit()
	{
		if(m_bpp==32)
			return;

		if(m_bpp==24)
		{
			BYTE* pOldData = m_imageData;

			int newSize = m_height * m_width * 4;
			m_imageData	= (BYTE *)malloc(newSize);
			if( m_imageData == NULL )
			{
				TRACE(_T("Could not allocate memory for image\n"));
				free(pOldData);
				return;
			}

			struct tagPixel24
			{
				BYTE B,G,R;
			};
			tagPixel24* pOldP = (tagPixel24*)pOldData;
			tagPixel* pNewP = (tagPixel*)m_imageData;
			for( int i=0; i<m_width*m_height; i++ )
			{
				pNewP->R = pOldP->R;
				pNewP->G = pOldP->G;
				pNewP->B = pOldP->B;
				pNewP->A = 1;

				pOldP++;
				pNewP++;
			}

			m_bpp = 32;

			free(pOldData);
		}
		else if(m_bpp==8)
		{
			BYTE* pOldData = m_imageData;

			int newSize = m_height * m_width * 4;
			m_imageData	= (BYTE *)malloc(newSize);
			THROW_NULL(m_imageData,NULL);

			BYTE* pOldP = pOldData;
			tagPixel* pNewP = (tagPixel*)m_imageData;
			for( int i=0; i<m_width*m_height; i++ )
			{
				pNewP->R = 255;
				pNewP->G = 255;
				pNewP->B = 255;
				pNewP->A = *pOldP;

				pOldP++;
				pNewP++;
			}

			m_bpp = 32;

			free(pOldData);
		}
	}

	bool ImageTGA::Save(const TCHAR* szFileName)
	{
		if(m_imageData==NULL ||
			m_bpp!=32)//只支持32位
			return false;

		//open file
		FILE *file=_tfopen(szFileName,_T("wb"));
		if(!file)
		{
			return false;
		}

		//save header
		unsigned char headerUncomp[] = {0,0,2,0,0,0,0,0,0,0,0,0};
		fwrite(headerUncomp,sizeof(unsigned char),12,file);
		unsigned short width = GetWidth();
		fwrite(&width,sizeof(short),1,file);
		unsigned short height = GetHeight();
		fwrite(&height,sizeof(short),1,file);
		unsigned char colDepth = 32;
		fwrite(&colDepth,sizeof(char),1,file);
		unsigned char blank = 0;
		fwrite(&blank,sizeof(char),1,file);

		//save data
		BYTE *p = m_imageData;
		for(int y=0;y<GetHeight();y++)
		{
			for(int x=0;x<GetWidth();x++)
			{
				p+=2;
				fwrite(p,sizeof(BYTE),1,file);
				p--;
				fwrite(p,sizeof(BYTE),1,file);
				p--;
				fwrite(p,sizeof(BYTE),1,file);
				p+=3;
				fwrite(p,sizeof(BYTE),1,file);
				p++;
			}
		}

		//close file
		fclose(file);
		return true;
	}

	_inline BYTE GET_DWORD_A(DWORD color)
	{
		return BYTE(color>>24);
	}

	_inline BYTE GET_DWORD_R(DWORD color)
	{
		return BYTE((color>>16)&0xff);
	}

	_inline BYTE GET_DWORD_G(DWORD color)
	{
		return BYTE((color>>8)&0xff);
	}

	_inline BYTE GET_DWORD_B(DWORD color)
	{
		return BYTE(color&0xff);
	}

	_inline DWORD SET_DWORD_ARGB(BYTE a,BYTE r,BYTE g,BYTE b)
	{
		return DWORD((a<<24) | (r<<16) | (g<<8) | b);
	}

	void ImageTGA::Resize(int width,int height)
	{
		if(m_bpp!=8)//目前只支持8bit
			return;

		if(m_width==width
			&&m_height==height)
			return;

		BYTE* pOldData = m_imageData;

		int newSize = width * height;
		m_imageData	= (BYTE *)malloc(newSize);
		THROW_NULL(m_imageData,NULL);

		BYTE* pOldP = pOldData;
		BYTE* pNewP = m_imageData;

		int scaleX=m_width/width;
		int scaleY=m_height/height;

		int src_x,src_y;
		int src_index,dst_index;
		for(int dst_y=0;dst_y<height;dst_y++)
		{
			for(int dst_x=0;dst_x<width;dst_x++)
			{
				src_x=dst_x*scaleX;
				src_y=dst_y*scaleY;

				if(src_x>=m_width)
					src_x=m_width-1;
				if(src_y>=m_height)
					src_y=m_height-1;

				src_index=src_x+src_y*m_width;
				dst_index=dst_x+dst_y*width;

				pNewP[dst_index]=pOldP[src_index];
			}
		}

		m_width=width;
		m_height=height;

		free(pOldData);
	}

	void ImageTGA::Blit(ImageTGA* pSrc,int destX,int destY)
	{
		ASSERT(GetBpp()==pSrc->GetBpp());

		BYTE *pD = (BYTE *)GetData();
		BYTE *pS = (BYTE *)pSrc->GetData();

		int dw = GetWidth();
		int dh = GetHeight();
		int sw = pSrc->GetWidth();
		int sh = pSrc->GetHeight();
		ASSERT(destX+sw<=dw && destY+sh<=dh);
		int dstLineSize=dw*GetBpp()/8;
		int srcLineSize=sw*GetBpp()/8;
		int dstXOffset=destX*GetBpp()/8;

		for(int dy=destY,sy=0; 
			dy<destY+sh; 
			dy++,sy++)
		{
			FMemcpy(&pD[dy*dstLineSize+dstXOffset],&pS[sy*srcLineSize],srcLineSize);
		}
	}

}//namespace Cool3D