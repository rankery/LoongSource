#include "StdAfx.h"
#include ".\picfont.h"

#include ".\draw2dImp.h"
#include "..\Util\Exception.h"
#include "..\Device\IDevice.h"
#include "..\Device\IDeviceTex.h"
#include "..\Kernel.h"
#include "..\ResSys\ResTexture.h"


namespace Cool3D
{
	class PicFont::Member
	{
	public:
		ResTexture*		m_pASCIITex;		// 储存ASCII码符号的图片纹理
		int				m_nTextHeight;		// 需要绘制文字的高度
		int				m_nTextWidth;		// 需绘制文字的宽度
		int				m_nSpaceWidth;		// 字符之间的间距
		int				m_nSpaceHeight;		// 行与行之间的间距
		bool			m_bCreated;			// 文字图片纹理是否创建成功

		Member()	
		{
			m_bCreated = false;
			m_nSpaceHeight = 8;
			m_nSpaceWidth = 2;
			m_nTextWidth = 16;
			m_nTextHeight = 16;
			m_pASCIITex = NULL;
		}

		ResTexture* GetASCIITex()
		{
			if(m_pASCIITex)
				return m_pASCIITex;
			return NULL;
		}

		~Member()	
		{
		}
	};

	PicFont::PicFont(void)
	{
		m_p=new Member;
	}

	PicFont::~PicFont(void)
	{
		delete m_p;
	}

	void PicFont::Destroy()
	{
		if (m_p->m_pASCIITex!=NULL)	// 释放纹理资源
		{
			IDraw2D::Inst()->ReleaseTexture(m_p->m_pASCIITex);
		}	
		m_p->m_pASCIITex=NULL;
	}

	bool PicFont::Create(const TCHAR* szPicFileName,IFS *pFS)
	{
		if (m_p->m_bCreated) return true;
		//创建一个图片字体的纹理
		m_p->m_pASCIITex = IDraw2D::Inst()->NewTexture(szPicFileName,pFS);
		
		if (m_p->m_pASCIITex)
		{
			m_p->m_bCreated = true;		// 创建成功
			return true;
		}
		else 
		{
			m_p->m_bCreated = false;	// 创建失败
			return false;
		}
	}

	void PicFont::SetDrawCharSize(const int w/* =16 */,const int h/* =16 */)
	{	// 文字的宽度和高度都大于0的时候为合法的设置，否则保持默认值16像素
		if(w>=0)		m_p->m_nTextWidth  = w;
		
		if(h>=0)		m_p->m_nTextHeight = h;		
	}

	void PicFont::SetSpaceSize(const int w /* =2 */, const int h /* =8 */)
	{
		if(w>=0)		m_p->m_nSpaceWidth  = w;

		if(h>=0)		m_p->m_nSpaceHeight = h;
	}
	
	void PicFont::SetDrawCharHeight(const int nH)
	{
		if (nH>=0) m_p->m_nTextHeight = nH;			
	}

	void PicFont::SetDrawCharWidth(const int nW)
	{
		if(nW>=0) m_p->m_nTextWidth  = nW;
	}

	void PicFont::SetSpaceWidth(const int w)
	{
		if(w>=0) m_p->m_nSpaceWidth  = w;
	}

	void PicFont::SetSpaceHeight(const int h)
	{
		if (h>=0) m_p->m_nSpaceHeight = h;			
	}

	
	int PicFont::GetDrawCharWidth()
	{
		return m_p->m_nTextWidth;
	}

	int PicFont::GetDrawCharHeight()
	{
		return m_p->m_nTextHeight;
	}

	int PicFont::GetSpaceWidth()
	{
		return m_p->m_nSpaceWidth;
	}

	int PicFont::GetSpaceHeight()
	{
		return m_p->m_nSpaceHeight;
	}

	const TCHAR* PicFont::GetASCIITexPath() const
	{
		if(!m_p->m_bCreated || m_p->m_pASCIITex == NULL)
			return NULL;

		return (m_p->m_pASCIITex->GetName());
	}


	//------------------------------------------------
	// 绘制需要的ASCII码字符串
	//------------------------------------------------
	void PicFont::DrawString(const TCHAR* szText,int x,int y,float z,DWORD dwColor,DWORD dwTextEffectColor,int nCount)
	{
		if (m_p->m_bCreated==false || m_p->m_pASCIITex==NULL)		//纹理未创建成功
			return;
		if(szText ==NULL)	return;			//需要绘制的字符串为空
		
		IDevice *pDev = Device();

		if(x<0 || x>= (int)pDev->GetClientWidth())	return;		// 如果绘制的起点点不在屏幕范围内退出
		if(y<0 || y>= (int)pDev->GetClientHeight())	return;

		RECT destRect,srcRect;				//屏幕绘制区域和图片纹理区域
		const TCHAR *p = szText;			//保存字符指针
		int nTotal = nCount;				//计算绘制字符串的长度
		int nLength = _tcslen( szText );	
		if( nTotal < 0 || nTotal > nLength )//nCount为-1或者大于字符串的长度,绘制整个字符串
		{
			nTotal = nLength;
		}
		const TCHAR* pEnd = p + nTotal;		//字符串结束的指针

		int xOrg = x;							//绘制的起始点x
		int yOrg = y;							//绘制的起始点y
		int nTexCharWidth  = (m_p->m_pASCIITex->GetDeviceTex()->Width())/16;		//对应纹理中每个字符宽度
		int nTexCharHeight = (m_p->m_pASCIITex->GetDeviceTex()->Height())/8;		//对应纹理中每个字符高度
		int nLineCharCount = 0;					//一行中字符串的位置

		for( ; p<pEnd; )						// 遍历字符串中的每一个字符
		{
			if( TCHAR(*p) > 0 && TCHAR(*p) < 128)//ASCII(0~127)
			{
				if( *p == _T('\n') )	//回车换行
				{
					xOrg = x;
					yOrg += m_p->m_nTextHeight + m_p->m_nSpaceHeight;
					p++;
					nLineCharCount = 0;
					continue;
				}
				if( *p == _T('\t') )	//Tab占四个字符
				{
					p++;
					nLineCharCount +=4;	
					xOrg += ((m_p->m_nSpaceWidth + m_p->m_nTextWidth)<<2);				
					continue;
				}
				//todo:针对不同的图片作不同的运算
				//这里采用的是16*8字符的纹理图片
				int cx=((*p)%16)*nTexCharWidth;		// X Position Of Current Character in Texture
				int cy=((int)((*p)/16))*nTexCharHeight;		// Y Position Of Current Character in Texture
				
				if(cy<0 || cy>=8*nTexCharHeight) continue;		//不合法的纹理图片位置

				//屏幕上绘制的位置
				destRect.left	= xOrg;
				destRect.top	= yOrg;
				destRect.right	= xOrg + m_p->m_nSpaceWidth + m_p->m_nTextWidth;
				destRect.bottom	= yOrg + m_p->m_nSpaceHeight + m_p->m_nTextHeight;
				//对应的纹理的区域，指定纹理的像素坐标
				srcRect.left	= cx;
				srcRect.top		= cy;
				srcRect.bottom	= (cy + nTexCharWidth);
				srcRect.right	= (cx + nTexCharHeight);

				//描边处理
				if((dwTextEffectColor&0xFF000000)!=0x00000000)
				{
					//IDraw2D::Inst()->DrawWithZ(destRect,srcRect,m_p->m_pASCIITex,z,dwTextEffectColor);
					RECT rcEffect;
					
					//rcEffect.left	= destRect.left-1;
					//rcEffect.top	= destRect.top-1;
					//rcEffect.right	= destRect.right-1;
					//rcEffect.bottom	= destRect.bottom-1;	
					//IDraw2D::Inst()->DrawWithZ(rcEffect,srcRect,m_p->m_pASCIITex,z,dwTextEffectColor);
					
					rcEffect.left	= destRect.left+1;
					rcEffect.top	= destRect.top+1;
					rcEffect.right	= destRect.right+1;
					rcEffect.bottom	= destRect.bottom+1;	
					IDraw2D::Inst()->Draw(&rcEffect,&srcRect,m_p->m_pASCIITex,dwTextEffectColor,ETFilter_LINEAR,z);
					
					rcEffect.left	= destRect.left+1;
					rcEffect.top	= destRect.top-1;
					rcEffect.right	= destRect.right+1;
					rcEffect.bottom	= destRect.bottom-1;	
					IDraw2D::Inst()->Draw(&rcEffect,&srcRect,m_p->m_pASCIITex,dwTextEffectColor,ETFilter_LINEAR,z);
					
					rcEffect.left	= destRect.left-1;
					rcEffect.top	= destRect.top+1;
					rcEffect.right	= destRect.right-1;
					rcEffect.bottom	= destRect.bottom+1;	
					IDraw2D::Inst()->Draw(&rcEffect,&srcRect,m_p->m_pASCIITex,dwTextEffectColor,ETFilter_LINEAR,z);
				}

				IDraw2D::Inst()->Draw(&destRect,&srcRect,m_p->m_pASCIITex,dwColor,ETFilter_LINEAR,z);

				p++;

				nLineCharCount++;
				// 如果出现一行显示不下的情况
				if( nLineCharCount >= 
					((int)pDev->GetClientWidth()-x)/(m_p->m_nTextWidth + m_p->m_nSpaceWidth) )
				{
					nLineCharCount = 0;
					xOrg = x;
					yOrg += m_p->m_nTextHeight + m_p->m_nSpaceHeight;
				}
				else
				{
					xOrg += m_p->m_nTextWidth + m_p->m_nSpaceWidth;
				}
			}
			else		// 非ASCII(0~127)码字符,不予处理
				continue;
		}
	}




	
	class PicFontEx::Member
	{
	public:
		map<TCHAR,ResTexture*> m_mapTex;     //字符对应的纹理
		int				m_nSpaceWidth;		// 字符之间的间距
		int				m_nSpaceHeight;		// 行与行之间的间距

		Member()
		{
			m_nSpaceWidth=0;
			m_nSpaceHeight=0;
		}
		~Member()
		{
		}
   	};

	PicFontEx::PicFontEx()
	{
		m_p=new Member;
	}
	PicFontEx::~PicFontEx(void)
	{
		delete m_p;
	}
	void PicFontEx::Destroy()
	{
		// 释放纹理资源
		map<TCHAR,ResTexture*>::iterator it=m_p->m_mapTex.begin();
		for (;it!=m_p->m_mapTex.end();it++)
		{
			IDraw2D::Inst()->ReleaseTexture((*it).second);
			(*it).second=NULL;
		}
		m_p->m_mapTex.clear();
	}

	bool PicFontEx::LoadPic(TCHAR ch,const TCHAR* szPicFileName,IFS *pFS)
	{
		if (!m_p->m_mapTex.empty()&& m_p->m_mapTex.find(ch)!=m_p->m_mapTex.end())
		{
			//已经使用过的字符不加载图片
			return false;
		}

		//加载对应的图片,并保存到map中
		m_p->m_mapTex[ch]=IDraw2D::Inst()->NewTexture(szPicFileName,pFS);

		return true;
	}

	void PicFontEx::DrawString(const TCHAR* szText,int x,int y,float z,
		DWORD dwColor/* =0xFFFFFFFF */,int nCount/* =-1 */,int nStyle,float fZoom)
	{
		if(szText==NULL)return;

		int length=_tcslen(szText);
		if(nCount!=-1)
		{
			length=length>nCount?nCount:length;
		}
		//计算文字对齐方式 
		CalStringPos(szText,nStyle,x,y);

		RECT destRect,srcRect;					//屏幕绘制区域和图片纹理区域
		int xOrg = x;							//绘制的起始点x
		int yOrg = y;	
		TCHAR c;
		map<TCHAR,ResTexture*>::iterator it;
		int texW=0,texH=0;
		for (int i=0;i<length;i++)
		{
			c=szText[i];

			//处理特殊字符
			if(c==_T('\n'))//换行
			{
				yOrg += (int)(texH*fZoom) + m_p->m_nSpaceHeight;
				xOrg = x;
				texH=0;
				continue;
			}
			if(c==_T('\t'))	//默认空一个，方便外部控制
			{
				xOrg += 1*((int)(texW*fZoom) + m_p->m_nSpaceHeight);
				continue;
			}

			//查找是否在map中
			it=m_p->m_mapTex.find(c);
			if(it==m_p->m_mapTex.end())
			{
				continue;	//没有保存
			}

			texW=((*it).second)->GetDeviceTex()->Width();
			int h=((*it).second)->GetDeviceTex()->Height();
			texH=h>texH?h:texH;

			//非屏幕范围内部绘制
			if(xOrg>=(int)Device()->GetClientWidth())
			{
				continue;
			}
			if(yOrg>=(int)Device()->GetClientHeight())
			{
				continue;
			}
			//屏幕上绘制的位置
			destRect.left	= xOrg;
			destRect.top	= yOrg;
			destRect.right	= xOrg + (LONG)(texW*fZoom);
			destRect.bottom	= yOrg + (LONG)(texH*fZoom);
			//对应的纹理的区域，指定纹理的像素坐标
			srcRect.left	= 0;
			srcRect.top		= 0;
			srcRect.bottom	= texH;
			srcRect.right	= texW;
			IDraw2D::Inst()->Draw(&destRect, &srcRect, (*it).second,dwColor, ETFilter_POINT, 0, IDraw2D::EAWT_Write, z);

			//更新起始位置
			xOrg += (int)(texW*fZoom) +m_p->m_nSpaceWidth;
		}
	}

	void PicFontEx::SetSpaceSize(const int w /* =2 */, const int h /* =2 */)
	{
		m_p->m_nSpaceWidth=w;
		m_p->m_nSpaceHeight=h;
	}
	void PicFontEx::SetSpaceWidth(const int w)
	{
		m_p->m_nSpaceWidth=w;
	}
	void PicFontEx::SetSpaceHeight(const int h)
	{
		m_p->m_nSpaceHeight=h;
	}
	int PicFontEx::GetSpaceHeight()
	{
		return m_p->m_nSpaceHeight;
	}
	int PicFontEx::GetSpaceWidth()
	{
		return m_p->m_nSpaceWidth;
	}

	void PicFontEx::GetDrawStringSize(const TCHAR* szText, int& nWidth, int& nHeight)
	{
		ASSERT(szText!=NULL);
		int length=_tcslen(szText);
		int xWidth=0;//行宽度
		int xOrg = 0;//绘制的起始点x
		TCHAR c;
		map<TCHAR,ResTexture*>::iterator it;
		int texW=0, texH =0;
		for (int i=0;i<length;i++)
		{
			c=szText[i];

			//处理特殊字符
			if(c==_T('\n'))//换行
			{
				xOrg = 0;//清零
				continue;
			}
			if(c==_T('\t'))
				continue;

			//查找是否在map中
			it=m_p->m_mapTex.find(c);
			if(it==m_p->m_mapTex.end())
			{
				continue;	//没有保存
			}

			texW=((*it).second)->GetDeviceTex()->Width();
			INT nH=((*it).second)->GetDeviceTex()->Height();
			if( nH > texH )
				texH = nH;
			ASSERT(texW!=0);
			xOrg += texW;
			xWidth = xWidth>xOrg?xWidth:xOrg;
		}

		nWidth = xWidth;
		nHeight = texH;
	}

	void PicFontEx::CalStringPos(const TCHAR* szText,int nStyle ,int& x,int& y)
	{
		if(nStyle==0)//居左对齐可以不需要计算
			return;
	
		int nWidth, nHeight;
		this->GetDrawStringSize(szText, nWidth, nHeight);

		switch(nStyle)
		{
		case 1://居中对齐
			x -= nWidth/2;
			break;
		case 2://居右对齐
			x -= nWidth;
			break;
		default:
			break;
		}
	}
	
}//namespace Cool3D