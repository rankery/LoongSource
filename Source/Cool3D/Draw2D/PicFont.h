#pragma once
#include "..\Cool3D.h"
#include ".\idraw2d.h"

class IFS;

namespace Cool3D
{
	/**	\class PicFont
		\brief 使用图片的字体
		\remarks .使用一个256*256的灰度图片(带Alpha通道),存储ASCII码为0~127所有字符
	*/
	class Cool3D_API PicFont
	{
	public:
		PicFont(void);
		~PicFont(void);


		//--必备函数
		/**载入并创建ASCII码符号纹理，声明一个PicFont对象的时候首先调用该函数
			par：szPicFileName	指定Ascii码符号的纹理图片
		*/
		bool Create(const TCHAR* szPicFileName,IFS *pFS);

		//! 设置绘制时每个字符的大小，默认为16*16像素
		void SetDrawCharSize(const int w=16,const int h=16);

		/**绘制指定屏幕上的起始点坐标的ASCII字符串，
			par:	szText  指定的ASCII字符串
					x,y	    屏幕窗口绘制的起始坐标
					dwColor 为指定绘制的颜色，默认为白色,
					nCount  指定绘制字符串中字符的个数,默认为-1全部绘制
					dwTextEffectColor	文字描边颜色0x00000000表示不描边
		*/
		void DrawString(const TCHAR* szText,int x,int y,float z,DWORD dwColor=0xFFFFFFFF,
			DWORD dwTextEffectColor=0x00000000, int nCount=-1);
	
		void Destroy();

		//--辅助函数
		void	SetSpaceSize(const int w =2, const int h =8);	//设置行内字符的间距和行间距
		void	SetSpaceWidth(const int w);			//设置行内的字符间距
		void	SetSpaceHeight(const int h);		//设置行间距
		void	SetDrawCharWidth(const int nW);		//设置字符的宽度
		void	SetDrawCharHeight(const int nH);	//设置字符的高度
		int		GetDrawCharWidth();					//取得字符的宽度
		int		GetDrawCharHeight();				//取得字符的高度
		int		GetSpaceWidth();					//取得行内的字符间距
		int		GetSpaceHeight();					//取得行间距
		const TCHAR* GetASCIITexPath() const;		//取得纹理图片的路径
	
	private:
		class Member;
		Member *m_p;
	};


	/**	\class PicFontEx
	\brief 使用图片的字体
	\remarks .使用指字符和对应的图片，绘制字符串之前一定要加载字符对应的图片
	*/
	class Cool3D_API PicFontEx
	{
	public:
		PicFontEx(void);
		~PicFontEx(void);


		//--必备函数
		/**载入并创建ASCII码符号纹理，声明一个PicFont对象的时候首先调用该函数
		par：szPicFileName	指定Ascii码符号的纹理图片
				ch			指定图片对应的字符
		*/
		bool LoadPic(TCHAR ch,const TCHAR* szPicFileName,IFS *pFS);

		/**绘制指定屏幕上的起始点坐标的ASCII字符串，
		par:	szText  指定的ASCII字符串
		x,y	    屏幕窗口绘制的起始坐标
		dwColor 为指定绘制的颜色，默认为白色,
		nCount  指定绘制字符串中字符的个数,默认为-1全部绘制
		nSyle	文字对齐方式=0:左对齐，1：左对齐， 2：右对齐
		nZoom	纹理的缩放比例
		*/
		void DrawString(const TCHAR* szText,int x,int y,float z,DWORD dwColor=0xFFFFFFFF,int nCount=-1,int nSyle=1,float fZoom=1.0f);
		void GetDrawStringSize(const TCHAR* szText, int& nWidth, int& nHeight);

		void Destroy();

		//--辅助函数
		void	SetSpaceSize(const int w =2, const int h =2);	//设置行内字符的间距和行间距
		void	SetSpaceWidth(const int w);			//设置行内的字符间距
		void	SetSpaceHeight(const int h);		//设置行间距
		int		GetSpaceWidth();					//取得行内的字符间距
		int		GetSpaceHeight();					//取得行间距

	private:
		//nStyle=0:左对齐，1：左对齐， 2：右对齐
		void CalStringPos(const TCHAR* szText,int nStyle ,int& x,int& y);

	private:
		class Member;
		Member *m_p;
	};
}//namespace Cool3D