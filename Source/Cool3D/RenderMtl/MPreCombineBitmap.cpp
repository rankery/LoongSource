#include "StdAfx.h"
#include ".\mprecombinebitmap.h"
#include "..\Image\ImageTGA.h"
#include "..\ResSys\ResTexture.h"
#include "..\Device\IDevice.h"
#include "..\Device\IDeviceTex.h"
#include "..\Kernel.h"

namespace Cool3D
{
	class MPreCombineBitmap::Member
	{
	public:
		vector<tstring>	m_layerImg;
		vector<Color4f>	m_layerColor;
		ImageTGA		*m_pTmpImg;//用来创建贴图,贴图创建完成后会删除
		int				m_colorOp;
		int				m_finalW,
						m_finalH;
		tstring			m_szAlphaMap;

		Member()	
		{	
			m_pTmpImg=NULL;
			m_colorOp=0;
			m_finalW=m_finalH=-1;
			
		}
		~Member()	{	SAFE_DELETE(m_pTmpImg);}

		void ClearLayers()
		{
			m_layerImg.clear();
			m_layerColor.clear();
		}
		void AddLayer(tstring szImg,Color4f color)
		{
			m_layerImg.push_back(szImg);
			m_layerColor.push_back(color);
		}
	};
	//--
	IMP_RTTI_DYNC(MPreCombineBitmap,MBitmap);

	MPreCombineBitmap::MPreCombineBitmap(void)
	{
		m_p=new Member;
		MBitmap::m_pBitmap=NULL;
	}

	MPreCombineBitmap::~MPreCombineBitmap(void)
	{
		delete m_p;
		SAFE_DELETE(MBitmap::m_pBitmap);
	}

	UINT MPreCombineBitmap::GetNumLayers() const
	{
		ASSERT(m_p->m_layerColor.size()==m_p->m_layerImg.size());
		return m_p->m_layerColor.size();
	}

	tstring& MPreCombineBitmap::GetLayer(UINT i,Color4f& color)
	{
		ASSERT(i<GetNumLayers());
		color=m_p->m_layerColor[i];

		return m_p->m_layerImg[i];
	}

	void MPreCombineBitmap::Serialize(FArchive& ar)
	{
		ar<<m_p->m_colorOp;
		ar<<m_p->m_finalW;
		ar<<m_p->m_finalH;
		ASSERT(m_p->m_layerColor.size()==m_p->m_layerImg.size());
		int numLayer=m_p->m_layerColor.size();
		ar<<numLayer;
		for(int i=0;i<numLayer;i++)
		{
			ar<<m_p->m_layerColor[i];
			ar<<m_p->m_layerImg[i];
		}

	}

	void MPreCombineBitmap::Deserialize(FArchive& ar)
	{
		ar>>m_p->m_colorOp;
		ar>>m_p->m_finalW;
		ar>>m_p->m_finalH;

		int numLayer=0;
		ar>>numLayer;

		BeginCombine();
		for(int i=0;i<numLayer;i++)
		{
			Color4f color;
			tstring szImgFile;
			ar>>color;
			ar>>szImgFile;
			AddLayer(szImgFile.c_str(),color);
		}
		EndCombine();
	}

	void MPreCombineBitmap::SetColorOp(int op/*0add,1sub,2mul*/)
	{
		m_p->m_colorOp=op;
	}

	int MPreCombineBitmap::GetColorOp() const
	{
		return m_p->m_colorOp;
	}

	void MPreCombineBitmap::BeginCombine()
	{
		ASSERT(m_p->m_pTmpImg==NULL);
		IMaterial::InvalidateRenderCache();
		SAFE_DELETE(MBitmap::m_pBitmap);
		m_p->ClearLayers();
	}

	void SaveTGA(ImageTGA* pImg,EPixelFormat format=EPF_A8R8G8B8)
	{
		IDeviceTex *pTex=Device()->NewTexture();
		pTex->CreateFromData(pImg->GetWidth(),
			pImg->GetHeight(),format,
			pImg->GetData(),
			1);
		pTex->WriteToFile(_T("d:\\test.tga"));
		SAFE_DELETE(pTex);
	}

	void MPreCombineBitmap::AddLayer(const TCHAR* szImgFile,Color4f colorOffset)
	{
		m_p->AddLayer(szImgFile,colorOffset);
		if(m_p->m_pTmpImg==NULL)//第一层
		{
			m_p->m_pTmpImg=new ImageTGA;
			m_p->m_pTmpImg->LoadFromFile(Kernel::Inst()->DefaultFS(),
				szImgFile);
			m_p->m_pTmpImg->ConvertTo32Bit();
			//SaveTGA(m_p->m_pTmpImg);//test
			
			switch(m_p->m_colorOp)
			{
			case 2://mul
				m_p->m_pTmpImg->MulColor(colorOffset);
				break;
			case 1://sub
				m_p->m_pTmpImg->AddColor(int(colorOffset.A*255),-int(colorOffset.R*255),-int(colorOffset.G*255),-int(colorOffset.B*255));
				break;
			case 0:
			default:
				m_p->m_pTmpImg->AddColor(int(colorOffset.A*255),int(colorOffset.R*255),int(colorOffset.G*255),int(colorOffset.B*255));
			}
			
			//SaveTGA(m_p->m_pTmpImg);//test
			return;
		}

		ImageTGA img;
		if(img.LoadFromFile(Kernel::Inst()->DefaultFS(),szImgFile))
		{
			m_p->m_pTmpImg->AlphaBlend(&img);
		}
	}

	tstring MPreCombineBitmap::GetAlphaMap()
	{
		return m_p->m_szAlphaMap;
	}

	void MPreCombineBitmap::ApplyAlpha(const TCHAR* szAlphaMap)
	{
		ImageTGA img;
		if(img.LoadFromFile(ResMgr::Inst()->SafeGetFS(ResMgr::EFST_System),szAlphaMap))
		{
			m_p->m_pTmpImg->ApplyAlpha(&img);
			m_p->m_szAlphaMap=szAlphaMap;
		}
	}

	void MPreCombineBitmap::AddLayer(const TCHAR* szImgFile,const TCHAR* szAlphaMap)
	{
		ASSERT(m_p->m_pTmpImg!=NULL);//第一层不允许使用alpha map

		ImageTGA img;
		if(!img.LoadFromFile(Kernel::Inst()->DefaultFS(),szImgFile))
			return;

		ImageTGA alphaMap;
		if(!alphaMap.LoadFromFile(Kernel::Inst()->DefaultFS(),szAlphaMap))
			return;
		//SaveTGA(&alphaMap);//test

		img.ConvertTo32Bit();
		img.ApplyAlpha(&alphaMap);
		m_p->m_pTmpImg->AlphaBlend(&img);

		//SaveTGA(m_p->m_pTmpImg);//test
	}

	void MPreCombineBitmap::SetFinalSize(int w,int h)
	{
		m_p->m_finalW=w;
		m_p->m_finalH=h;
	}

	void MPreCombineBitmap::EndCombine()
	{
		ASSERT(m_p->m_pTmpImg!=NULL);
		if(m_p->m_finalW!=-1 && m_p->m_finalH!=-1)
		{
			//todo
		}

		//Image32Blender::AddColor(m_p->m_pTmpImg,255,0,0,0);
		//m_p->m_pTmpImg->WriteToFile("d:\\test.tga");

		MBitmap::m_pBitmap=new ResTexture(_T("None"));
		IDeviceTex *pTex=Device()->NewTexture();
		pTex->CreateFromData(m_p->m_pTmpImg->GetWidth(),
			m_p->m_pTmpImg->GetHeight(),EPF_A8R8G8B8,
			m_p->m_pTmpImg->GetData(),
			0);
		m_pBitmap->AttachTex(pTex);
#if 0
		static int i=0;
		TCHAR szFileName[256];
		_stprintf(szFileName,_T("d:\\test_%d.tga"),i++);
		pTex->WriteToFile(szFileName);
#endif

		//--
		delete m_p->m_pTmpImg;
		m_p->m_pTmpImg=NULL;
	}
}//namespace Cool3D