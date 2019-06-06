#include "stdafx.h"
#include "F3DExpMtl.h"
#include "MsgBox.h"
using namespace std;

namespace Cool3D
{
	tstring		F3DExpMtl::s_szBasePath;
	tstring		F3DExpMtl::s_szSubPath;
	tstring		F3DExpMtl::s_szOriginNode;
	bool		F3DExpMtl::s_bAutoCopy=true;
	bool		F3DExpMtl::s_bExportLMUV=false;
	bool		F3DExpMtl::s_bExportBUUV=false;
	bool		F3DExpMtl::s_bMultiPass=false;
	bool		F3DExpMtl::s_bExportOpcity=false;
	bool		F3DExpMtl::s_bExportBox=false;
	MsgBox		*F3DExpMtl::s_pMsgBox=NULL;
	int			F3DExpMtl::s_iLightMapSize=0;
	float		F3DExpMtl::s_fCollideBoxSize=50.0f;

	void Point3ToColor4f(Vector4& color,const Point3& pt,float alpha=1.0f)
	{
		color.x=pt.x;
		color.y=pt.y;
		color.z=pt.z;
		color.w=alpha;
	}

	F3DExpMtl::F3DExpMtl(Mtl* pMtl,int subID,int myID,Mtl *pBakedMtl)
	{
		//--id
		m_pMaxMtl=pMtl;
		m_nMaxSubID=subID;
		m_nID=myID;
		m_pBakedMtl=pBakedMtl;

		//--export data
		assert(pMtl!=NULL);

		Mtl *pMaxMtl=NULL;
		if(subID == -1)
			pMaxMtl=pMtl;
		else
			pMaxMtl=pMtl->GetSubMtl(subID);
		assert(pMaxMtl!=NULL);

		Point3ToColor4f(m_ambient,pMaxMtl->GetAmbient());
		Point3ToColor4f(m_diffuse,pMaxMtl->GetDiffuse(),1-pMaxMtl->GetXParency());
		Color specular= pMaxMtl->GetSpecular();
		specular*=pMaxMtl->GetShinStr();
		Point3ToColor4f(m_specular,specular);
		m_power=pMaxMtl->GetShininess()*128;
		if(pMaxMtl->GetSelfIllumColorOn())
			Point3ToColor4f(m_emissive,pMaxMtl->GetSelfIllumColor());
		else
		{
			float illum=pMaxMtl->GetSelfIllum();
			Point3ToColor4f(m_emissive,Point3(illum,illum,illum));
		}

		if(pMaxMtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0))
		{
			StdMat *pStd=static_cast<StdMat*>(pMaxMtl);
			m_bTwoSide=(pStd->GetTwoSided()!=0);
		}
		else
			m_bTwoSide=false;

		//--export Texmap
		// should auto copy texture to export path??
		m_mapfile[EMap_Diffuse]=GetTexMapPath(pMaxMtl,ID_DI);//diffuse
		//if(m_mapfile[EMap_Diffuse].length() > 0)//有Diffuse贴图, 则取消Diffuse颜色 why?Diffuse颜色是作为材质用的
		//{
		//	m_diffuse=Vector4(1,1,1,1);
		//}
		m_mapfile[EMap_Specular]=GetTexMapPath(pMaxMtl,ID_SP);//specular
		m_mapfile[EMap_Bump]=GetTexMapPath(pMaxMtl,ID_BU);//bump
		if(pBakedMtl != NULL)
			m_mapfile[EMap_Lighting]=GetTexMapPath(pBakedMtl,ID_DI);

		//环境反射贴图
		m_mapfile[EMap_Sphere]=GetTexMapPath(pMaxMtl, ID_RL);

		m_opacityVal=255;
		m_bMultiPass=false;
		tstring szOpcityMap=GetTexMapPath(pMaxMtl,ID_OP);//if have opacity map
		if(szOpcityMap.empty())
			m_bOpacityMap=false;
		else
		{
			m_bOpacityMap=true;
			if(_tcsicmp(szOpcityMap.c_str(),m_mapfile[EMap_Diffuse].c_str()) != 0)
			{
				/*为什么不行?
				if(s_pMsgBox)
					s_pMsgBox->ShowMsg("Bad Opcity map,%s\r\n",szOpcityMap.c_str());
					*/
			}
			if(pMaxMtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0))
			{
				StdMat *pStd=static_cast<StdMat*>(pMaxMtl);
				float op=pStd->GetOpacity(0);
				m_opacityVal=255.0f*op;
			}
			if(m_opacityVal==255
				&& s_bMultiPass)
				m_bMultiPass=true;
		}
	}
	
	F3DExpMtl::~F3DExpMtl()
	{
		
	}
	
	tstring F3DExpMtl::GetTexMapPath(Mtl *pMtl, DWORD mapid)
	{
		tstring ret;
		Texmap *pMap=pMtl->GetSubTexmap(mapid);
		if(pMap == NULL)
			return ret;
		if(pMap->ClassID()!=Class_ID(BMTEX_CLASS_ID, 0x00))
			return ret;

		BitmapTex *pBmp=static_cast<BitmapTex*>(pMap);
		ret=pBmp->GetMapName();

		return ret;
	}
	
	void AutoCopyMapFile(const TCHAR* szSrcPath,tstring& szOut)
	{
		assert(szSrcPath!=NULL);

		tstring szFileName=szSrcPath;
		//计算出文件名称,去掉路径
		int p=szFileName.find_last_of(_T('\\'));
		if(p!=tstring::npos)
		{
			szFileName=szFileName.substr(p+1);
		}
		
		//计算输出的绝对路径,并且拷贝到该路径
		tstring szDest;
		if(F3DExpMtl::s_bAutoCopy)
		{
			szDest=F3DExpMtl::s_szBasePath;
			int len=szDest.length();
			if(len>0
				&& szDest[len-1]!=_T('\\'))
				szDest+=_T('\\');
			
			len=F3DExpMtl::s_szSubPath.length();
			assert(len>0 && F3DExpMtl::s_szSubPath[0]!=_T('\\'));
			szDest+=F3DExpMtl::s_szSubPath;
			
			len=szDest.length();
			if(len>0
				&& szDest[len-1]!=_T('\\'))
				szDest+=_T('\\');
			szDest+=szFileName;
			
			BOOL ret=CopyFile(szSrcPath,szDest.c_str(),FALSE);
			if(!ret)
			{
				//TODO: fixe this
				/*
				MsgBox::Inst()->ShowMsg("File Copy failed.src=%s,dest=%s\r\n",
					szSrcPath,szDest.c_str());
					*/
			}
		}

		szOut=F3DExpMtl::s_szSubPath;
		int len=szOut.length();
		if(len>0
			&& szOut[len-1]!=_T('\\'))
			szOut+=_T('\\');
		szOut+=szFileName;
	}

	void F3DExpMtl::GetMtlDesc(tagMtlDesc &out)
	{
		memset(&out,0,sizeof(tagMtlDesc));
		out.m_ambient=m_ambient;
		out.m_bOpcityMap=m_bOpacityMap;
		out.m_opcityVal=m_opacityVal;
		out.m_bTwoSide=m_bTwoSide;
		out.m_diffuse=m_diffuse;
		out.m_emissive=m_emissive;
		out.m_id=m_nID;
		out.m_power=m_power;
		out.m_specular=m_specular;
		out.m_bMultiPass=m_bMultiPass;

		tstring szMap;
		for(int i=0;i<EMap_Max;i++)
		{
			if(m_mapfile[i].empty())
				continue;
			
			AutoCopyMapFile(m_mapfile[i].c_str(),szMap);
			
			wcscpy(out.m_map[i],CharToWide(szMap.c_str()).c_str());
			/*为什么不行?
			if(s_pMsgBox)
			{
			s_pMsgBox->ShowMsg("Mtl%d,map%d=",m_nID,i);
			s_pMsgBox->ShowMsg(out.m_map[i]);
			s_pMsgBox->ShowMsg("\r\n");
		}*/
		}
	}
	
}//namespace Cool3D