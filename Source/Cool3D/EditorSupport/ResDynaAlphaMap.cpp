#include "StdAfx.h"
#include ".\resdynaalphamap.h"
#include "..\util\DebugMisc.h"
#include "..\util\Exception.h"
#include "..\Util\Filename.h"
#include "..\Kernel.h"
#include "..\VFS\IFS.h"
#include "..\Terrain\Heightmap.h"
#include "..\Image\\Image.h"
#include "..\Device\IDeviceTex.h"

namespace Cool3D
{

	IMP_RTTI(ResDynaAlphaMap,ResBase);

	ResDynaAlphaMap::ResDynaAlphaMap(const TCHAR* szName):ResBase(szName)
	{
		m_pHeightmap=NULL;
		m_pImage=NULL;
		m_pTex=NULL;
	}

	ResDynaAlphaMap::~ResDynaAlphaMap(void)
	{
		SAFE_DELETE(m_pHeightmap);
		SAFE_DELETE(m_pImage);
		SAFE_DELETE(m_pTex);
	}


	UINT ResDynaAlphaMap::CreateContent(DWORD param)
	{
		m_pHeightmap=new Heightmap;

		int initVal=HIWORD(param);
		UINT size=LOWORD(param);
		
		//--创建或读取Heightmap
		IFS* pFS=GetSafeFS();
		if(pFS->GetSize(GetName())==-1)
		{
			m_pHeightmap->Create(initVal,size,size,1.0f,1.0f,1.0f);
		}
		else
		{
			DWORD hFile=pFS->Open(GetName());
			m_pHeightmap->ReadFromFile(pFS,hFile);
			pFS->Close(hFile);
		}

		//--创建Image
		m_pImage=new Image;
		m_pImage->Create(size,size,EPF_A8);
		Update2Image();

		//--创建Texture
		m_pTex=Device()->NewTexture();
		m_pTex->CreateFromImage(m_pImage,1);

		//--
		m_size=m_pHeightmap->Width()*m_pHeightmap->Height();
		InterlockedIncrement(&m_bCreated);
		return m_size;
	}

	Heightmap*  ResDynaAlphaMap::GetHeightmap() const
	{
		ASSERT(IsCreated());
		return m_pHeightmap;
	}

	IDeviceTex* ResDynaAlphaMap::GetDeviceTex() const
	{
		ASSERT(IsCreated());
		return m_pTex;
	}

	void ResDynaAlphaMap::UpdateTexture(const RECT *pRc)
	{
		ASSERT(IsCreated());
		Update2Image(pRc);
		Update2Texture();
	}

	void ResDynaAlphaMap::Update2Image(const RECT *pRc)
	{
		RECT rc;
		if(pRc)
		{
			rc=*pRc;
			if(rc.left<0)
				rc.left=0;
			if(rc.top<0)
				rc.top=0;
			if(rc.right>=(int)m_pHeightmap->Width())
				rc.right=m_pHeightmap->Width();
			if(rc.bottom>=(int)m_pHeightmap->Height())
				rc.bottom=m_pHeightmap->Height();
		}
		else
		{
			rc.left=rc.top=0;
			rc.right=m_pHeightmap->Width();
			rc.bottom=m_pHeightmap->Height();
		}

		ASSERT(m_pImage->GetPixelFormat()==EPF_A8);
		BYTE *pPixel=(BYTE*)m_pImage->GetRawBuffer();
		if(pPixel==NULL)
			return;
		int w=m_pImage->Width();
		int x,y;
		BYTE *pCurPixel;
		for(y=rc.top;y<rc.bottom;y++)
		{
			int lineOffset=y*w;
			for(x=rc.left;x<rc.right;x++)
			{
				pCurPixel=pPixel+lineOffset+x;
				*pCurPixel=m_pHeightmap->GetValue(x,y);
			}
		}//endof for

		//m_pImage->WriteToFile(_T("d:\\test.tga"));
	}

	void ResDynaAlphaMap::Update2Texture()
	{
		m_pTex->UpdateFromImage(m_pImage,false);
	}

	void ResDynaAlphaMap::SaveToFile()
	{
		ASSERT(IsCreated());
		FILE* fp=_tfopen(GetName(),_T("wb"));
		if( fp==NULL )
		{
			LOG(_T("error filename:%s\n"),GetName());
			LOG(_tcserror(errno));
			return;
		}
		m_pHeightmap->WriteToFile(fp);		
		fclose(fp);
	}

	void ResDynaAlphaMap::SaveToImage(const TCHAR* szPath,UINT size)
	{
		ASSERT(IsCreated());
		Image *pImg=m_pImage->Clone();
		pImg->Scale(size,size,EIS_Fastest);
		pImg->WriteToFile(szPath);
		delete pImg;
	}


}//namespace Cool3D