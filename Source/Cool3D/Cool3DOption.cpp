#include "StdAfx.h"
#include ".\Cool3DOption.h"
#include ".\kernel.h"
#include ".\Terrain\TerrainMultiLayersRender.h"
#include ".\RenderSys\WaterReflectMapRender.h"
#include ".\RenderSys\WaterRefractMapRender.h"
#include ".\Util\DebugMisc.h"
#include "Audio\AudioSystem.h"

namespace Cool3D
{
	Cool3DOption::Cool3DOption()
	{
		m_options[EO_LightMap]		=ELM_None;
		m_options[EO_TerrainDetail]	=ETD_Precombine;
		m_options[EO_Highlight]		=EHL_None;
		m_options[EO_NormalMap]		=ENM_None;
		m_options[EO_HWSkin]		=EHWS_None;
		m_options[EO_GrassAni]		=EGA_None;
		m_options[EO_TerrainSpecular]=ETS_Open;
		m_options[EO_WaterReflect]	=EWR_None;
		m_options[EO_WaterRefract]	=EWRR_None;
		m_options[EO_TexDetail]		=ETexDetail_High;
		m_options[EO_WaterSpecular]	=EWS_None;
		m_options[EO_DebugInfo]		=EDI_None;
		m_options[EO_Music]			=EM_None;
		m_options[EO_Sound]			=ES_None;
		m_options[EO_DrawGrass]		=EDG_Open;

		m_bEditorMode=false;
	}

	Cool3DOption::~Cool3DOption()
	{
	}

	Cool3DOption* Cool3DOption::Inst()
	{
		static Cool3DOption Inst;
		return &Inst;
	}

	bool Cool3DOption::SetOption(EOption opt,int val)
	{
		switch(opt)
		{
		case EO_LightMap:
			{
				if(Kernel::Inst()->IsEngineCreated()&&!m_bEditorMode)
					return false;

				if(val<0 || val>=ELM_End)
					return false;
			}
			break;
		case EO_TerrainDetail:
			{
				if(Kernel::Inst()->IsEngineCreated()&&!m_bEditorMode)
					return false;

				if(val<0 || val>=ETD_End)
					return false;
			}
			break;
		case EO_Highlight:
			{
				if(Kernel::Inst()->IsEngineCreated()&&!m_bEditorMode)
					return false;

				if(val<0 || val>=EHL_End)
					return false;
			}
			break;
		case EO_NormalMap:
			{
				if(Kernel::Inst()->IsEngineCreated()&&!m_bEditorMode)
					return false;

				if(val<0 || val>=ENM_End)
					return false;
			}
			break;
		case EO_HWSkin:
			{
				if(Kernel::Inst()->IsEngineCreated()&&!m_bEditorMode)
					return false;

				if(val<0 || val>=EHWS_End)
					return false;
			}
			break;
		case EO_GrassAni:
			{
				if(Kernel::Inst()->IsEngineCreated()&&!m_bEditorMode)
					return false;

				if(val<0 || val>=EGA_End)
					return false;
			}
			break;
		case EO_TerrainSpecular:
			{
				if(Kernel::Inst()->IsEngineCreated()&&!m_bEditorMode)
					return false;

				if(val<0 || val>=ETS_End)
					return false;
			}
			break;
		case EO_WaterReflect:
			{
				if(val<0 || val>=EWR_End)
					return false;

				if(val>0)
				{
					WaterReflectMapRender::Inst()->Enable(true);
					WaterReflectMapRender::Inst()->SetEffectLevel((WaterReflectMapRender::EEffectLevel)val);
				}
				else
				{
					WaterReflectMapRender::Inst()->Enable(false);
				}
			}
			break;
		case EO_WaterRefract:
			{
				if(val<0 || val>=EWRR_End)
					return false;
			}
			break;
		case EO_TexDetail:
			{
				if(Kernel::Inst()->IsEngineCreated()&&!m_bEditorMode)
					return false;

				if(val<0 || val>=ETexDetail_End)
					return false;
			}
			break;
		case EO_WaterSpecular:
			{
				if(val<0 || val>=EWS_End)
					return false;
			}
			break;
		case EO_DebugInfo:
			{
				if(Kernel::Inst()->IsEngineCreated()&&!m_bEditorMode)
					return false;

				if(val<0 || val>=EDI_End)
					return false;
			}
			break;
		case EO_Music:
			{
				if(Kernel::Inst()->IsEngineCreated()&&!m_bEditorMode)
					return false;

				if(val<0 || val>=EM_End)
					return false;
			}
			break;
		case EO_Sound:
			{
				if(Kernel::Inst()->IsEngineCreated()&&!m_bEditorMode)
					return false;

				if(val<0 || val>=ES_End)
					return false;
			}
			break;
		case EO_DrawGrass:
			{
				if(val<0 || val>=EDG_End)
					return false;
			}
			break;
		default:
			{
				ASSERT(0 && "unknow option");
				return false;
			}
		}
	
		m_options[opt]=val;
		return true;
	}

	int Cool3DOption::GetOption(EOption opt)
	{
		if(opt>=0 && opt<EO_Num)
			return m_options[opt];
		else
			return -1;
	}

	bool Cool3DOption::IsSupport(EOption opt,int val)
	{
		if(!Kernel::Inst()->IsDeviceCreated())
			return false;

		switch(opt)
		{
		case EO_LightMap:
			{
				switch(val)
				{
				case ELM_None:
				case ELM_Open:
					return true;
				default:
					ASSERT(0);
				}
			}
			break;
		case EO_TerrainDetail:
			{
				switch(val)
				{
				case ETD_Precombine:
				case ETD_Editor:
				case ETD_TwoLayers:
					return true;
				case ETD_MultiLayers:
					return TerrainMultiLayersRender::Inst()->IsHWSurpport();
				default:
					ASSERT(0);
				}
			}
			break;
		case EO_Highlight:
			{
				switch(val)
				{
				case EHL_None:
				case EHL_Open:
					return true;
				default:
					ASSERT(0);
				}
			}
			break;
		case EO_NormalMap:
			{
				switch(val)
				{
				case ENM_None:
					return true;
				case ENM_Open:
					return	Device()->CapsVertexShaderVersion()>=D3DVS_VERSION(2,0)
						&&	Device()->CapsPixelShaderVersion()>=D3DPS_VERSION(2,0);
				default:
					ASSERT(0);
				}
			}
			break;
		case EO_HWSkin:
			{
				switch(val)
				{
				case EHWS_None:
					return true;
				case EHWS_Open:
					return Device()->CapsVertexShaderVersion()>=D3DVS_VERSION(2,0);
				default:
					ASSERT(0);
				}
			}
			break;
		case EO_GrassAni:
			{
				switch(val)
				{
				case EGA_None:
				case EGA_Open:
					return true;
				default:
					ASSERT(0);
				}
			}
			break;
		case EO_TerrainSpecular:
			{
				switch(val)
				{
				case ETS_None:
					return true;
				case ETS_Open:
					return TerrainMultiLayersRender::Inst()->IsHWSurpport();
				default:
					ASSERT(0);
				}
			}
			break;
		case EO_WaterReflect:
			{
				switch(val)
				{
				case EWR_None:
					return true;
				case EWR_Mid:
				case EWR_High:
					return WaterReflectMapRender::Inst()->IsHWSurpport();
				default:
					ASSERT(0);
				}
			}
			break;
		case EO_WaterRefract:
			{
				switch(val)
				{
				case EWRR_None:
					return true;
				case EWRR_Open:
					return WaterRefractMapRender::Inst()->IsHWSurpport();
				default:
					ASSERT(0);
				}
			}
			break;
		case EO_TexDetail:
			{
				return true;
			}
			break;
		case EO_WaterSpecular:
			{
				switch(val)
				{
				case EWS_None:
					return true;
				case EWS_Open:
					return Device()->CapsPixelShaderVersion()>=D3DVS_VERSION(2,0);
				default:
					ASSERT(0);
				}
			}
			break;
		case EO_DebugInfo:
			{
				return true;
			}
			break;
		case EO_Music:
			{
				if( val==EM_Open )
					return AudioSystem::Inst()->IsHWSurpport();

				return true;
			}
			break;
		case EO_Sound:
			{
				if( val==ES_Open )
					return AudioSystem::Inst()->IsHWSurpport();

				return true;
			}
			break;
		case EO_DrawGrass:
			{
				return true;
			}
		default:
			ASSERT(0 && "unknow option");
		}

		return false;
	}

	void Cool3DOption::CheckCreateOptions()
	{
		if(!IsSupport(EO_LightMap,m_options[EO_LightMap]))
			m_options[EO_LightMap]=ELM_None;

		if(!IsSupport(EO_TerrainDetail,m_options[EO_TerrainDetail]))
			m_options[EO_TerrainDetail]=ETD_Precombine;

		if(!IsSupport(EO_Highlight,m_options[EO_Highlight]))
			m_options[EO_Highlight]=EHL_None;

		if(!IsSupport(EO_NormalMap,m_options[EO_NormalMap]))
			m_options[EO_NormalMap]=ENM_None;

		if(!IsSupport(EO_HWSkin,m_options[EO_HWSkin]))
			m_options[EO_HWSkin]=EHWS_None;

		if(!IsSupport(EO_GrassAni,m_options[EO_GrassAni]))
			m_options[EO_GrassAni]=EGA_None;

		if(!IsSupport(EO_TerrainSpecular,m_options[EO_TerrainSpecular]))
			m_options[EO_TerrainSpecular]=ETS_None;

		if(!IsSupport(EO_TexDetail,m_options[EO_TexDetail]))
			m_options[EO_TerrainDetail]=ETexDetail_Low;

		if(!IsSupport(EO_WaterReflect,m_options[EO_WaterReflect]))
			m_options[EO_WaterReflect]=EWR_None;

		if(!IsSupport(EO_WaterRefract,m_options[EO_WaterRefract]))
			m_options[EO_WaterRefract]=EWRR_None;

		if(!IsSupport(EO_WaterSpecular,m_options[EO_WaterSpecular]))
			m_options[EO_WaterSpecular]=EWS_None;

		if(!IsSupport(EO_DebugInfo,m_options[EO_DebugInfo]))
			m_options[EO_DebugInfo]=EDI_None;

		if(!IsSupport(EO_Music,m_options[EO_Music]))
			m_options[EO_Music]=EM_None;

		if(!IsSupport(EO_Sound,m_options[EO_Sound]))
			m_options[EO_Sound]=ES_None;

		//开启NormalMap时自动打开HWSkin
		if(m_options[EO_NormalMap]==ENM_Open)
			m_options[EO_HWSkin]=EHWS_Open;
	}

	void Cool3DOption::SetEditorMode(bool bEditorMode)
	{
		m_bEditorMode=bEditorMode;
	}

}//namespace Cool3D