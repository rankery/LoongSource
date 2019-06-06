#include "StdAfx.h"
#include ".\noise.h"
#include "..\Util\Exception.h"
#include "Math.h"
#include "perlin.h"

namespace Cool3D
{
	using namespace Detail;

	Noise::Noise(void)
	{
		m_pData=NULL;
		m_numOfOctaves=8;
		m_frequency=4.0f;
		m_amplitude=64;
	}

	Noise::~Noise(void)
	{
		Destroy();
	}

	void Noise::SetParam(float amplitude,UINT numOfOctaves)
	{
		m_amplitude=amplitude;
		m_numOfOctaves=numOfOctaves;
	}

	void Noise::Destroy()
	{
		SAFE_FREE(m_pData);
	}

	void Noise::ReGenerate()
	{
		Detail::Perlin *perlin=new Detail::Perlin(m_numOfOctaves,m_frequency,m_amplitude,GetTickCount());
		ASSERT(m_pData!=NULL);
		
		float xf=1.0f/m_width;
		float yf=1.0f/m_height;
		float max=-FLOAT_MAX;
		float min=FLOAT_MAX;
		UINT x,y;
		for(y=0;y<m_height;y++)
		{
			for(x=0;x<m_width;x++)
			{
				UINT index=y*m_width+x;
				float n=perlin->Get(x*xf,y*yf);
				m_pData[index]=n;
				if(n>max)
					max=n;
				if(n<min)
					min=n;
			}
		}

		//TRACE("MAX=%.2f,MIN=%.2f\r\n",max,min);
		//Perlin产生的值域在[-振幅,+振幅]之间
		//--缩放到0-1的区间
		//float offset=fabsf(min);
		//float range=max-min;
		float offset=m_amplitude;
		float range=m_amplitude*2;
		
		for(y=0;y<m_height;y++)
		{
			for(x=0;x<m_width;x++)
			{
				UINT index=y*m_width+x;
				m_pData[index]=(m_pData[index]+offset)/range;
			}
		}
		delete perlin;
	}

	void Noise::Generate(UINT width,UINT height)
	{
		Destroy();//可重复生成

		m_pData=(DataType*)malloc(width*height*sizeof(DataType));
		m_width=width;
		m_height=height;
		
		ReGenerate();//gen noise
	}

	float Noise::GetData(UINT x,UINT y)
	{
		ASSERT(x<m_width);
		ASSERT(y<m_height);

		return m_pData[y*m_width+x];
	}

	void Noise::SaveToRAW(const TCHAR* szFileName)
	{
		//写入单通道的RAW
		FILE *fp=NULL;
		if(_tfopen_s(&fp,szFileName,_T("wb"))!=0)
			return;
		for(UINT y=0;y<m_height;y++)
		{
			for(UINT x=0;x<m_width;x++)
			{
				BYTE r=BYTE(m_pData[y*m_width+x]*255);
				fwrite(&r,sizeof(BYTE),1,fp);
			}
		}


		fclose(fp);
	}
}//namespace Cool3D