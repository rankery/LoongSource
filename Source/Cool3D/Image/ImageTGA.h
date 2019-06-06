#pragma once
#include "..\Cool3D.h"
#include "..\math\Color.h"

class IFS;
namespace Cool3D
{
	class Cool3D_API ImageTGA
	{
		typedef struct
		{
			BYTE		header[6];			// First 6 Useful Bytes From The Header
			UINT		bytesPerPixel;		// Holds Number Of Bytes Per Pixel Used In The TGA File
			UINT		imageSize;			// Used To Store The Image Size When Setting Aside Ram
			UINT		temp;				// Temporary Variable
			UINT		type;	
			UINT		Height;				//Height of Image
			UINT		Width;				//Width ofImage
			UINT		Bpp;				// Bits Per Pixel
		} TGA;
	public:
		struct tagPixel
		{
			BYTE B,G,R,A;
		};
	public:
		ImageTGA(void);
		~ImageTGA(void);

		void Create(UINT width,UINT height,UINT bpp);
		void Destroy();
		bool LoadFromFile(IFS *pFS,const TCHAR* szFileName);
		bool Save(const TCHAR* szFileName);

		void AlphaBlend(ImageTGA *pSrcImg);
		void ApplyAlpha(ImageTGA *pSrcImg);
		void AddColor(int a,int r,int g,int b);
		void MulColor(Color4f color);

		int GetWidth() const	{	return m_width;}
		int GetHeight() const	{	return m_height;}
		int GetBpp() const		{	return m_bpp;}
		BYTE* GetData() const	{	return m_imageData;}

		void ConvertTo32Bit();
		void Resize(int width,int height);
		void Blit(ImageTGA* pSrc,int destX,int destY);

	private:
		bool LoadUncompressedTGA(IFS *pFS,DWORD hFile);
		bool LoadCompressedTGA(IFS *pFS,DWORD hFile);
		bool LoadUncompressedBlackTGA(IFS *pFS,DWORD hFile);
	private:
		BYTE	*m_imageData;	// Image Data (Up To 32 Bits)
		int		m_bpp;			// Image Color Depth In Bits Per Pixel
		int		m_width;			// Image Width
		int		m_height;			// Image Height
	};
}//namespace Cool3D