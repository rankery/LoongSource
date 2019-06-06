#include "StdAfx.h"
#include "..\SpeedTreeRT\include\SpeedTreeRT.h"

#include "..\Util\Exception.h"
#include ".\treemtl.h"
#include "..\ResSys\ResMgr.h"
#include "..\ResSys\ResTexture.h"
#include "..\Kernel.h"
#include "..\Device\IDevice.h"

namespace F3D
{
	IMP_RTTI_DYNC(TreeMtl,IMaterial);

	TreeMtl::TreeMtl(void)
	{
		m_texBranch=NULL;
		m_texShadow=NULL;
		m_texComposite=NULL;
	}

	TreeMtl::~TreeMtl(void)
	{
		if(m_texBranch)
		{
			ResMgr::Inst()->ReleaseTexture(m_texBranch);
			m_texBranch=NULL;
		}
		if(m_texShadow)
		{
			ResMgr::Inst()->ReleaseTexture(m_texShadow);
			m_texShadow=NULL;
		}
		if(m_texComposite)
		{
			ResMgr::Inst()->ReleaseTexture(m_texComposite);
			m_texComposite=NULL;
		}
	}

	char* __MakeDDSExt(const char* src)
	{
		static char szFile[_MAX_PATH];
		strcpy(szFile,src);
		size_t len=strlen(szFile);
		ASSERT(len>4);
		strcpy(szFile+len-4,".dds");
		return szFile;
	}
	void TreeMtl::CreateForTree(CSpeedTreeRT *pTree)
	{
		CSpeedTreeRT::STextures textureInfo;
		pTree->GetTextures(textureInfo);
		m_texBranch=ResMgr::Inst()->NewTexture(textureInfo.m_pBranchTextureFilename);

		if (textureInfo.m_pSelfShadowFilename != NULL
			&& strlen(textureInfo.m_pSelfShadowFilename))
		{
			m_texShadow=ResMgr::Inst()->NewTexture(__MakeDDSExt(textureInfo.m_pSelfShadowFilename));
		}

		if(textureInfo.m_pCompositeFilename!=NULL
			&& strlen(textureInfo.m_pCompositeFilename)>0)
			m_texComposite=ResMgr::Inst()->NewTexture(__MakeDDSExt(textureInfo.m_pCompositeFilename));
		else
		{
			if(textureInfo.m_uiFrondTextureCount>0)
				m_texComposite=ResMgr::Inst()->NewTexture(textureInfo.m_pFrondTextureFilenames[0]);
		}

	}

	void TreeMtl::ActiveBranch()
	{
		Device()->SetSimplerFilter(0,ETFilter_LINEAR,ETFilter_LINEAR);
		Device()->SetSimplerMipFilter(0,ETFilter_LINEAR);
		if(m_texBranch && m_texBranch->IsCreated())
			Device()->SetTexture(0,m_texBranch->GetDeviceTex());
		else
			Device()->SetTexture(0,(IDeviceTex*)NULL);
		if(m_texShadow && m_texShadow->IsCreated())
			Device()->SetTexture(1,m_texShadow->GetDeviceTex());
		else
			Device()->SetTexture(1,(IDeviceTex*)NULL);
	}

	void TreeMtl::ActiveComposite()
	{
		Device()->SetSimplerFilter(0,ETFilter_LINEAR,ETFilter_LINEAR);
		Device()->SetSimplerMipFilter(0,ETFilter_LINEAR);
		if(m_texComposite && m_texComposite->IsCreated())
			Device()->SetTexture(0,m_texComposite->GetDeviceTex());
		else
			Device()->SetTexture(0,(IDeviceTex*)NULL);
	}
}//namespace F3D