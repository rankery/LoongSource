#include "StdAfx.h"
#include ".\heightmap.h"
#include "..\Kernel.h"
#include "..\Image\Image.h"
#include "..\Math\Noise.h"
#include "..\Math\GeomDefine.h"
#include "..\ResSys\VertStream.h"
#include "..\VFS\IFS.h"

namespace Cool3D
{
	Heightmap::Heightmap(void)
	{
		m_xScale=1.0f;	
		m_zScale=1.0f;
		m_heightMap8=NULL;
		m_heightMap16=NULL;
		m_yScale=1.0f;
		m_hmapWidth=0;
		m_hmapHeight=0;
		m_startPos=Vector2(0,0);
		m_format=EF_8Bit;
	}

	Heightmap::~Heightmap(void)
	{
		Destroy();
	}

	void Heightmap::Destroy()
	{
		SAFE_FREE(m_heightMap8);
		SAFE_FREE(m_heightMap16);

		m_xScale=1.0f;	
		m_zScale=1.0f;
		m_heightMap8=NULL;
		m_heightMap16=NULL;
		m_yScale=1.0f;
		m_hmapWidth=0;
		m_hmapHeight=0;
		m_startPos=Vector2(0,0);
	}

	Heightmap *Heightmap::NewSubMap(RECT rect) const
	{
		Heightmap *pNewMap=new Heightmap;
		pNewMap->m_xScale=m_xScale;
		pNewMap->m_yScale=m_yScale;
		pNewMap->m_zScale=m_zScale;
		pNewMap->CreateData(rect.right-rect.left,rect.bottom-rect.top,m_format);
		pNewMap->m_startPos.x=rect.left*m_xScale;
		pNewMap->m_startPos.y=rect.top*m_zScale;

		for(UINT y=0;y<pNewMap->m_hmapHeight;y++)
		{
			for(UINT x=0;x<pNewMap->m_hmapWidth;x++)
			{
				int by=y+rect.top;
				int bx=x+rect.left;
				pNewMap->SetValue(x,y,GetValue(bx,by));
			}
		}//endof for
		return pNewMap;
	}

	void Heightmap::SetSubMap( Heightmap *pSubMap,RECT& rect )
	{
		for(UINT y=0;y<pSubMap->m_hmapHeight;y++)
		{
			for(UINT x=0;x<pSubMap->m_hmapWidth;x++)
			{
				int by=y+rect.top;
				int bx=x+rect.left;
				SetValue(bx,by,pSubMap->GetValue(x,y));
			}
		}
	}

	bool Heightmap::ExportImage(const TCHAR* szFileName)
	{
		Image img;

		if(!img.Create(m_hmapWidth,m_hmapHeight,EPF_R8G8B8))
			return false;
		
		int numPixel=m_hmapWidth*m_hmapHeight;
		BYTE *imgData=(BYTE*)img.GetRawBuffer();

		if(m_format==EF_8Bit)
		{
			for(int i=0;i<numPixel;i++)
			{
				imgData[0] = m_heightMap8[i];
				imgData[1] = m_heightMap8[i];
				imgData[2] = m_heightMap8[i];
				
				imgData+=3;//下一个RGB组
			}
		}
		else
		{
			float minh,maxh;
			RECT rect={0,0,m_hmapWidth,m_hmapHeight};
			GetHeightRange(minh,maxh,rect);

			for(int i=0;i<numPixel;i++)
			{
				imgData[0] = BYTE(m_heightMap16[i]/maxh*255);
				imgData[1] = BYTE(m_heightMap16[i]/maxh*255);
				imgData[2] = BYTE(m_heightMap16[i]/maxh*255);

				imgData+=3;//下一个RGB组
			}
		}

		return img.WriteToFile(szFileName);
	}

	bool Heightmap::ImportImage(const TCHAR* szFileName,float xScale,float zScale,float yScale)
	{
		Destroy();

		//--load image
		Image img;
		if(!img.LoadFromFile(szFileName))
		{
			Kernel::Inst()->SetLastError(_T("图像文件加载失败:%s"),szFileName);
			return false;
		}
		if(24!=img.GetBpp())
		{
			Kernel::Inst()->SetLastError(_T("图像文件不是24位图像:%s"),szFileName);
			return false;
		}

		CreateData(img.Width(),img.Height(),EF_16Bit);
		m_xScale=xScale;
		m_zScale=zScale;
		m_yScale=1;

		//--copy data
		int numPixel=m_hmapWidth*m_hmapHeight;
		BYTE *imgData=(BYTE*)img.GetRawBuffer();
		for(int i=0;i<numPixel;i++)
		{
			m_heightMap16[i]=WORD((*imgData)*yScale);
			imgData+=3;//下一个RGB组
		}

		return true;
	}

	void Heightmap::AutoCenterStartPos()
	{
		m_startPos.x=-0.5f*m_hmapWidth*m_xScale;
		m_startPos.y=-0.5f*m_hmapHeight*m_zScale;
	}

	void Heightmap::CreateData(UINT width,UINT height,EFormat format)
	{
		ASSERT(m_heightMap8==NULL && m_heightMap16==NULL);
		ASSERT(width > 0);
		ASSERT(height > 0);

		m_hmapWidth=width;
		m_hmapHeight=height;
		m_format=format;

		int numPixel=m_hmapWidth*m_hmapHeight;
		if(format==EF_8Bit)
			m_heightMap8=(BYTE*)malloc(numPixel*sizeof(BYTE));
		else
			m_heightMap16=(WORD*)malloc(numPixel*sizeof(WORD));
	}

	bool Heightmap::WriteToFile(FILE *fp) const
	{
		if(fp==NULL)
			return false;
		fwrite(&m_format,sizeof(EFormat),1,fp);
		fwrite(&m_xScale,sizeof(float),1,fp);
		fwrite(&m_yScale,sizeof(float),1,fp);
		fwrite(&m_zScale,sizeof(float),1,fp);
		fwrite(&m_startPos,sizeof(m_startPos),1,fp);
		fwrite(&m_hmapWidth,sizeof(UINT),1,fp);
		fwrite(&m_hmapHeight,sizeof(UINT),1,fp);
		if(m_format==EF_8Bit)
			fwrite(m_heightMap8,m_hmapWidth*m_hmapHeight*sizeof(BYTE),1,fp);
		else
			fwrite(m_heightMap16,m_hmapWidth*m_hmapHeight*sizeof(WORD),1,fp);

		return true;
	}

	bool Heightmap::ReadFromFile(IFS* pFS,DWORD hFile)
	{
		Destroy();
		//--
		if(pFS==NULL || hFile==NULL)
			return false;
		pFS->Read(&m_format,sizeof(EFormat),hFile);
		pFS->Read(&m_xScale,sizeof(float),hFile);
		pFS->Read(&m_yScale,sizeof(float),hFile);
		pFS->Read(&m_zScale,sizeof(float),hFile);
		pFS->Read(&m_startPos,sizeof(m_startPos),hFile);
		pFS->Read(&m_hmapWidth,sizeof(UINT),hFile);
		pFS->Read(&m_hmapHeight,sizeof(UINT),hFile);
		CreateData(m_hmapWidth,m_hmapHeight,m_format);
		if(m_format==EF_8Bit)
			pFS->Read(m_heightMap8,m_hmapWidth*m_hmapHeight*sizeof(BYTE),hFile);
		else
			pFS->Read(m_heightMap16,m_hmapWidth*m_hmapHeight*sizeof(WORD),hFile);

		//m_yScale=48;//test
		return true;
	}

	void Heightmap::Serialize(FArchive& ar)
	{
		ar<<m_xScale;
		ar<<m_yScale;
		ar<<m_zScale;
		ar<<m_startPos;
		ar<<m_hmapWidth;
		ar<<m_hmapHeight;
		if(m_format==EF_8Bit)
			ar.BufferWrite(m_heightMap8,m_hmapWidth*m_hmapHeight*sizeof(BYTE));
		else
			ar.BufferWrite(m_heightMap16,m_hmapWidth*m_hmapHeight*sizeof(WORD));
	}

	void Heightmap::Deserialize(FArchive& ar,EFormat format)
	{
		Destroy();
		ar>>m_xScale;
		ar>>m_yScale;
		ar>>m_zScale;
		ar>>m_startPos;
		ar>>m_hmapWidth;
		ar>>m_hmapHeight;
		CreateData(m_hmapWidth,m_hmapHeight,format);
		if(m_format==EF_8Bit)
			ar.BufferRead(m_heightMap8,m_hmapWidth*m_hmapHeight*sizeof(BYTE));
		else
			ar.BufferRead(m_heightMap16,m_hmapWidth*m_hmapHeight*sizeof(WORD));
	}

	void Heightmap::Create(int initVal,UINT width,UINT height,float xScale,float zScale,float yScale,EFormat format)
	{
		CreateData(width,height,format);
		m_xScale=xScale;
		m_zScale=zScale;
		m_yScale=yScale;

		UINT i=0;
		if(format==EF_8Bit)
		{
			for(UINT y=0;y<height;y++)
			{
				for(UINT x=0;x<width;x++)
				{
					m_heightMap8[i++]=initVal;
				}
			}
		}
		else
		{
			for(UINT y=0;y<height;y++)
			{
				for(UINT x=0;x<width;x++)
				{
					m_heightMap16[i++]=initVal;
				}
			}		
		}
	}

	void Heightmap::CreateByNoise(float freq,UINT width,UINT height,float xScale,float zScale,float yScale,EFormat format)
	{
		Noise ns;
		ns.SetFreq(freq);
		ns.Generate(width,height);
		//ns.SaveToRAW("d:\\test.raw");
		CreateData(width,height,format);

		m_xScale=xScale;
		m_zScale=zScale;
		m_yScale=yScale;

		UINT i=0;
		if(m_format==EF_8Bit)
		{
			for(UINT y=0;y<height;y++)
			{
				for(UINT x=0;x<width;x++)
				{
					m_heightMap8[i++]=BYTE(ns.GetData(x,y)*255);
				}
			}		
		}
		else
		{
			for(UINT y=0;y<height;y++)
			{
				for(UINT x=0;x<width;x++)
				{
					m_heightMap16[i++]=WORD(ns.GetData(x,y)*10000);
				}
			}		
		}
	}

	void Heightmap::GetVertNormal(UINT x,UINT z,Vector3& out) const
	{
		//--see <<Game Programming Gems 3>>,4.2,Fast Heightfield normal cal
		/*
					h2
				  |
				  |
		  h3______h______h1
				  |
				  |
				   h4
		*/
		out.x=(GetSafeHeight(x-4,z)-GetSafeHeight(x+4,z));//((h1-h3)
		out.z=(GetSafeHeight(x,z-4)-GetSafeHeight(x,z+4));//(h2-h4)
		out.y=8.0f*m_xScale;
		D3DXVec3Normalize(&out,&out);

		//out=Vector3(0,1,0);//test
	}

	void Heightmap::GetHeightRange(float& minh,float &maxh,RECT rect)
	{
		int minH=255;
		int maxH=-255;
		for(int z=rect.top;z<rect.bottom;z++)
		{
			for(int x=rect.left;x<rect.right;x++)
			{
				int h=GetValue(x,z);
				if(h<minH)
					minH=h;
				if(h>maxH)
					maxH=h;
			}
		}
		minh=minH*m_yScale;
		maxh=maxH*m_yScale;
	}

	void Heightmap::BuildAABBox(AABBox *pBox,const RECT* pRect)
	{
		ASSERT(pBox!=NULL);
		if(pRect==NULL)
		{
			RECT rect;
			rect.left=rect.top=0;
			rect.right=m_hmapWidth;
			rect.bottom=m_hmapHeight;
			pBox->min.x=m_startPos.x;
			pBox->min.z=m_startPos.y;
			pBox->max.x=m_startPos.x+m_xScale*m_hmapWidth;
			pBox->max.z=m_startPos.y+m_zScale*m_hmapHeight;
			GetHeightRange(pBox->min.y,pBox->max.y,rect);
		}
		else
		{
			pBox->min.x=m_startPos.x+m_xScale*pRect->left;
			pBox->min.z=m_startPos.y+m_zScale*pRect->top;
			pBox->max.x=pBox->min.x+m_xScale*(pRect->right-pRect->left);
			pBox->max.z=pBox->min.z+m_zScale*(pRect->bottom-pRect->top);
			GetHeightRange(pBox->min.y,pBox->max.y,*pRect);
		}
	}

	void Heightmap::FillVertStream_PNT(VertStream *pStream,int sideVerts,RECT rect,bool uvInRect)
	{
		ASSERT(pStream!=NULL);

		int stepX=(rect.right-rect.left)/(sideVerts-1);
		int stepZ=(rect.bottom-rect.top)/(sideVerts-1);

		ASSERT(pStream->GetType()==EVType_PNT);
		ASSERT(pStream->GetNumVert()==sideVerts*sideVerts);
		Vert_PNT *pVert=(Vert_PNT *)pStream->GetRawBuffer();

		if(uvInRect)
		{
			float rectW=float(rect.right-rect.left-1);
			float rectH=float(rect.bottom-rect.top-1);

			for(int z=0;z<sideVerts;z++)
			{
				for(int x=0;x<sideVerts;x++)
				{
					int hx,hz;
					if(x==sideVerts-1)
						hx=rect.right-1;
					else
						hx=rect.left+x*stepX;	
					if(z==sideVerts-1)
						hz=rect.bottom-1;
					else
						hz=rect.top+z*stepZ;

					GetVertPos(hx,hz,pVert->pos);
					GetVertNormal(hx,hz,pVert->normal);

					pVert->uv.x=float(hx-rect.left)/rectW;
					pVert->uv.y=float(hz-rect.top)/rectH;

					//--
					pVert++;
				}
			}
		}
		else
		{
			float mapW=(float)m_hmapWidth*GetXScale();
			float mapH=(float)m_hmapHeight*GetZScale();

			for(int z=0;z<sideVerts;z++)
			{
				for(int x=0;x<sideVerts;x++)
				{
					int hx,hz;
					if(x==sideVerts-1)
						hx=rect.right-1;
					else
						hx=rect.left+x*stepX;	
					if(z==sideVerts-1)
						hz=rect.bottom-1;
					else
						hz=rect.top+z*stepZ;

					GetVertPos(hx,hz,pVert->pos);
					GetVertNormal(hx,hz,pVert->normal);

					pVert->uv.x=pVert->pos.x/mapW;
					pVert->uv.y=pVert->pos.z/mapH;

					//--
					pVert++;
				}
			}
		}

	}

	void Heightmap::FillVertStream_PN(VertStream *pStream,int sideVerts,RECT rect)
	{
		ASSERT(pStream!=NULL);

		int stepX=(rect.right-rect.left)/(sideVerts-1);
		int stepZ=(rect.bottom-rect.top)/(sideVerts-1);

		ASSERT(pStream->GetType()==EVType_PN);
		ASSERT(pStream->GetNumVert()==sideVerts*sideVerts);
		Vert_PN *pVert=(Vert_PN *)pStream->GetRawBuffer();
		for(int z=0;z<sideVerts;z++)
		{
			for(int x=0;x<sideVerts;x++)
			{
				int hx,hz;
				if(x==sideVerts-1)
					hx=rect.right-1;
				else
					hx=rect.left+x*stepX;	
				if(z==sideVerts-1)
					hz=rect.bottom-1;
				else
					hz=rect.top+z*stepZ;

				GetVertPos(hx,hz,pVert->pos);
				GetVertNormal(hx,hz,pVert->normal);

				//--
				pVert++;
			}
		}
	}

	void Heightmap::FillVertStream_P(VertStream *pStream,int sideVerts,RECT rect)
	{
		ASSERT(pStream!=NULL);

		int stepX=(rect.right-rect.left)/(sideVerts-1);
		int stepZ=(rect.bottom-rect.top)/(sideVerts-1);

		ASSERT(pStream->GetType()==EVType_P);
		ASSERT(pStream->GetNumVert()==sideVerts*sideVerts);
		Vert_P *pVert=(Vert_P *)pStream->GetRawBuffer();
		for(int z=0;z<sideVerts;z++)
		{
			for(int x=0;x<sideVerts;x++)
			{
				int hx,hz;
				if(x==sideVerts-1)
					hx=rect.right-1;
				else
					hx=rect.left+x*stepX;	
				if(z==sideVerts-1)
					hz=rect.bottom-1;
				else
					hz=rect.top+z*stepZ;

				GetVertPos(hx,hz,pVert->pos);

				//--
				pVert++;
			}
		}
	}

	float Heightmap::GetSafeHeight(int x,int z) const
	{
		if(x<0)
			x=0;
		if(x>(int)m_hmapWidth-1)
			x=m_hmapWidth-1;
		if(z<0)
			z=0;
		if(z>(int)m_hmapHeight-1)
			z=m_hmapHeight-1;
		return GetHeight(x,z);
	}

	int Heightmap::GetSafeVal(int x,int z) const
	{
		if(x<0)
			x=0;
		if(x>(int)m_hmapWidth-1)
			x=m_hmapWidth-1;
		if(z<0)
			z=0;
		if(z>(int)m_hmapHeight-1)
			z=m_hmapHeight-1;
		return GetValue(x,z);
	}

	void Heightmap::SafeAddValue(int x,int z,int& val)
	{
		if(!IfIndex(x,z))
			return;
		UINT index=GetIndex(x,z);
		if(m_format==EF_8Bit)
		{
			int oldVal=m_heightMap8[index];
			int newVal=oldVal+val;
			if(newVal<0)
				newVal=0;
			if(newVal>255)
				newVal=255;

			val=newVal-oldVal;
			m_heightMap8[index]=newVal;
		}
		else
		{
			int oldVal=m_heightMap16[index];
			int newVal=oldVal+val;
			if(newVal<0)
				newVal=0;
			if(newVal>65535)
				newVal=65535;

			val=newVal-oldVal;
			m_heightMap16[index]=newVal;
		}
	}
	void Heightmap::SafeSetVal(int x,int z,int& val)
	{
		if(!IfIndex(x,z))
			return;
		if(m_format==EF_8Bit)
		{
			if(val<0)
				val=0;
			if(val>255)
				val=255;
			m_heightMap8[GetIndex(x,z)]=val;
		}
		else
		{
			if(val<0)
				val=0;
			if(val>65535)
				val=65535;
			m_heightMap16[GetIndex(x,z)]=val;
		}
	}

	bool Heightmap::RayCollision(const Ray& ray,Vector3& pt,float maxLength,bool bAutoGroundHeight)
	{
		float step=min(m_xScale,m_zScale);
		step*=0.5f;
		
		Vector3 linePt;
		POINT mapPt=World2Tile(ray.origin);
		if(IfIndex(mapPt.x,mapPt.y) &&
			GetHeight(mapPt.x,mapPt.y) > ray.origin.y)//如果ray起点位于地形下面,那么返回false
			step=-step;
		float h;
		for(float len=0;len<maxLength&&len>-maxLength;len+=step)
		{
			linePt=ray.dir*len+ray.origin;
			mapPt=World2Tile(linePt);
			if(IfIndex(mapPt.x,mapPt.y))//如果在heightmap范围内
			{
				h=GetHeight(mapPt.x,mapPt.y);
				if(linePt.y<h && step>0)//如果射线上这个点已经低于地面了
				{
					pt=linePt;
					if(bAutoGroundHeight)
						pt.y=h;
					return true;
				}
				if (linePt.y>h && step<0)
				{
					pt=linePt;
					if(bAutoGroundHeight)
						pt.y=h;
					return true;
				}
			}
		}
		return false;
		/*错误的
		Vector3 vEnd=ray.origin+ray.dir*maxLength;
		POINT start=World2Tile(ray.origin);
		POINT end=World2Tile(vEnd);

		float h=GetHeight(start.x,start.y);
		if(h < ray.origin.y)//如果ray起点位于地形下面,那么返回false
			return false;

		//从start到end画一条直线,如果直线上某点高于所对应的Ray上的点,则碰撞
		int x1=start.x;
		int y1=start.y;
		int x2=end.x;
		int y2=end.y;
		int x,y,dx,dy,s1,s2,t,interchange,f,i;
		x=start.x;y=start.y;

		if (x2>=x1) 
		{
			dx=x2-x1;
			s1=1;
		}
		else
		{
			dx=x1-x2;
			s1=-1;
		}
		if (y2>=y1) 
		{
			dy=y2-y1;
			s2=1;
		}
		else 
		{
			dy=y1-y2;
			s2=-1;
		}
		if (dy>dx) 
		{
			t=dx;
			dx=dy;
			dy=t;
			interchange=1;
		}
		else interchange=0;

		f=dy+dy-dx;
		for (i=1;i<=dx;i++)
		{
			//--检测
			if(x<0 || x>=m_hmapWidth
				|| y<0 || y>=m_hmapHeight)
				return false;
			Vector3 linePt=Tile2World(x,y);
			float prj=0;
			int pr=ProjectPointToLineSegment(linePt,ray.origin,vEnd,prj);
			ASSERT(pr==2);//一定在线段内
			Vector3 rayPt=ray.origin+ray.dir*prj;
			if(rayPt.y<linePt.y)
			{
				pt=rayPt;
				return true;
			}
			//--
			if (f>=0)	
			{
				if (interchange) x+=s1;
				else y+=s2;
				f-=dx+dx;
			}
			if (interchange) y+=s2;
			else x+=s1;
			f+=dy+dy;
		}


		return false;
		*/
	}

	void Heightmap::CopyTo(Heightmap& out) const
	{
		out.Create(0,m_hmapWidth,m_hmapHeight,m_xScale,m_zScale,m_yScale,m_format);
		out.m_startPos=m_startPos;
		if(m_format==EF_8Bit)
		{
			ASSERT(out.m_heightMap8!=NULL);
			memcpy(out.m_heightMap8,m_heightMap8,sizeof(BYTE)*m_hmapWidth*m_hmapHeight);
		}
		else
		{
			ASSERT(out.m_heightMap16!=NULL);
			memcpy(out.m_heightMap16,m_heightMap16,sizeof(WORD)*m_hmapWidth*m_hmapHeight);
		}
	}

	void Heightmap::ConvertTo16Bit()
	{
		if(m_format!=EF_8Bit)
			return;

		ASSERT(m_heightMap8!=NULL);
		ASSERT(m_heightMap16==NULL);

		int numPixel=m_hmapWidth*m_hmapHeight;
		m_heightMap16=(WORD*)malloc(numPixel*sizeof(WORD));
		for(int i=0;i<numPixel;i++)
		{
			m_heightMap16[i]=WORD(m_heightMap8[i]*m_yScale);
		}

		m_yScale=1;
		m_format=EF_16Bit;
		SAFE_FREE(m_heightMap8);
	}

}//namespace Cool3D