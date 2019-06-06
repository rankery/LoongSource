#include "StdAfx.h"
#include ".\devpixelstagefixed.h"
#include "..\Kernel.h"
#include "..\Device\IDevice.h"
#include "..\Util\Exception.h"


namespace Cool3D
{
	DevPixelStageFixed::DevPixelStageFixed(void)
	{
		m_color=RenderSys::WhiteRenderColor;
		m_numTexUsed=0;
		m_bMultiPass=false;
	}

	DevPixelStageFixed::~DevPixelStageFixed(void)
	{
	}

	bool DevPixelStageFixed::IsTexUsed(IDeviceTex* pTex)
	{
		bool find=false;
		for(TexStageArray::iterator iter=m_stages.begin();
			iter!=m_stages.end();++iter)
		{
			if(pTex==iter->pTex)
			{
				find=true;
				break;
			}
		}
		return find;
	}

	bool DevPixelStageFixed::IsTexUsed(IRenderTexture* pRTex)
	{
		bool find=false;
		for(TexStageArray::iterator iter=m_stages.begin();
			iter!=m_stages.end();++iter)
		{
			if(pRTex==iter->pRTex)
			{
				find=true;
				break;
			}
		}
		return find;
	}

	int DevPixelStageFixed::CalcTexUsed(const tagTexStage& newStage)
	{
		int texUsed=m_numTexUsed;
		if(newStage.pTex!=NULL)
		{
			if(!IsTexUsed(newStage.pTex))
				texUsed++;
		}
		else if(newStage.pRTex!=NULL)
		{
			if(!IsTexUsed(newStage.pRTex))
				texUsed++;
		}
		return texUsed;
	}

	void DevPixelStageFixed::AddStage(const tagTexStage& newState)
	{
		//--统计同时使用的贴图个数
		m_numTexUsed=CalcTexUsed(newState);

		//--add to array
		m_stages.push_back(newState);
	}

	void DevPixelStageFixed::Set2Device()
	{
		IDevice *pDev=Device();

		//--曝光效果材质预先除以2
		tagRenderColor color=m_color;
		color.diffuse.R*=0.5f;
		color.diffuse.G*=0.5f;
		color.diffuse.B*=0.5f;
		color.ambient.R*=0.5f;
		color.ambient.G*=0.5f;
		color.ambient.B*=0.5f;
		color.emissive.R*=0.5f;
		color.emissive.G*=0.5f;
		color.emissive.B*=0.5f;

		//--
		pDev->FSetRenderColor(&color);
		DWORD texIndex=0;
		int i = 0;
		for(i=0;i<(int)m_stages.size();i++)
		{
			if(m_stages[i].uvTransType!=EUVT_Disable)
			{
				pDev->MatLoad(EMatrixStackType(EMS_Tex0+i),m_stages[i].uvTrans.GetMatrix());
				texIndex|=TexStage2MatrixIndex(i);
			}
		}
		if(texIndex!=0)
		{
			pDev->FFlushMatrix(false,false,false,true,texIndex);
		}

		//--
		i=0;
		for(TexStageArray::iterator iter=m_stages.begin();
			iter!=m_stages.end();++iter)
			SetStage2Device(i++,*iter);

		//--When rendering using vertex shaders, 
		//--each stage's texture coordinate index must be set to its default value. 
		//--The default index for each stage is equal to the stage index. 
		//--Set this state to the zero-based index of the coordinate set for each vertex that this texture stage uses.
		const int numStages=pDev->CapsMaxTextureStages();
		for(int j=i; j<numStages; j++)
			pDev->FSetTexCoordSrc(j,j);

		if(i<numStages)
		{
			pDev->FSetTextureColorOp(i,ETOP_Disable);
			pDev->FSetTextureAlphaOp(i,ETOP_Disable);
		}

		for(vector<RenderState>::iterator iter=m_states.begin();
			iter!=m_states.end();++iter)
		{
			pDev->SetRenderState(iter->first,iter->second);
		}

	}

	void DevPixelStageFixed::SetStage2Device(UINT i,const tagTexStage& stage,bool bSetTex)
	{
		IDevice *pDev=Device();
		if(bSetTex)
		{
			if(stage.pRTex)
				pDev->SetTexture(i,stage.pRTex);
			else
				pDev->SetTexture(i,stage.pTex);
			pDev->SetSimplerAddress(i,stage.addressU,stage.addressV,stage.addressW);
			pDev->SetSimplerFilter(i,stage.magFilter,stage.minFilter);
			pDev->SetSimplerMipFilter(i,stage.mipFilter);
			pDev->FSetTextureColorOp(i,stage.texOp,stage.texArg1,stage.texArg2);
			pDev->FSetTextureAlphaOp(i,stage.alphaOp,stage.alphaArg1,stage.alphaArg2);
			if(stage.bAutoUVIndex)
				pDev->FSetTexCoordSrc(i,i);
			else
				pDev->FSetTexCoordSrc(i,stage.uvIndex,stage.uvIndexFlags);
			pDev->FSetUVTrans(i,stage.uvTransType);
		}
		if(stage.texOp==ETOP_BumpEnvMap)
			pDev->FSetTexBumpMat(i,stage.matBump);
	}

	void DevPixelStageFixed::AppendState(ERStateType state,DWORD value)
	{
		RenderState rs;
		rs.first=state;
		rs.second=value;
		m_states.push_back(rs);
	}

	void DevPixelStageFixed::ModifyState(ERStateType state,DWORD value)
	{
		for(vector<RenderState>::iterator iter=m_states.begin();
			iter!=m_states.end();++iter)
		{
			if(iter->first==state)
			{
				iter->second=value;
				break;
			}
		}
	}
}//namespace Cool3D