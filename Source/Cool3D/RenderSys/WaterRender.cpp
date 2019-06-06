#include "stdafx.h"
#include ".\WaterRender.h"
#include "..\RenderSys\WaterReflectMapRender.h"
#include "..\RenderSys\WaterRefractMapRender.h"
#include "..\RenderSys\RenderNode.h"
#include "..\Terrain\SGRiverPatch.h"
#include "..\Kernel.h"
#include "..\Cool3DOption.h"
#include "..\SceneGraph\SceneGraph.h"
#include "..\Math\Transform.h"
#include "..\RenderMtl\MWaterReflect.h"
#include "..\RenderMtl\MWaterRefract.h"
#include "..\RenderMtl\MConstColor.h"
#include "..\ResSys\ResVertShader.h"
#include "..\ResSys\ResPixelShader.h"
#include "..\Device\IVertShader.h"
#include "..\Device\IPixelShader.h"

namespace Cool3D
{
	class WaterRender::Member
	{
	public:
		map<DWORD,vector<RenderItem>>	m_rivers;
		ResPixelShader*                 m_pSpPS;               // specular
		ResVertShader*					m_pRlVS;               // reflect
		ResPixelShader*                 m_pRlPS;
		ResVertShader*                  m_pRlSpVS;             // reflect + specular
		ResPixelShader*                 m_pRlSpPS;
		ResVertShader*                  m_pRlRrVS;             // reflect + refract
		ResPixelShader*                 m_pRlRrPS;
		ResVertShader*                  m_pRlRrSpVS;           // reflect + refract + specular
		ResPixelShader*                 m_pRlRrSpPS;

		typedef pair<DWORD,vector<RenderItem>> riverPair;

	public:
		Member() :
		m_pSpPS( NULL ),
		m_pRlVS( NULL ),
		m_pRlPS( NULL ),
		m_pRlSpVS( NULL ),
		m_pRlSpPS( NULL ),
		m_pRlRrVS( NULL ),
		m_pRlRrPS( NULL ),
		m_pRlRrSpVS( NULL ),
		m_pRlRrSpPS( NULL )
		{
		}

		~Member()
		{
			RES_MGR_RELEASE_PS( m_pSpPS );
			RES_MGR_RELEASE_VS( m_pRlVS );
			RES_MGR_RELEASE_PS( m_pRlPS );
			RES_MGR_RELEASE_VS( m_pRlSpVS );
			RES_MGR_RELEASE_PS( m_pRlSpPS );
			RES_MGR_RELEASE_VS( m_pRlRrVS );
			RES_MGR_RELEASE_PS( m_pRlRrPS );
			RES_MGR_RELEASE_VS( m_pRlRrSpVS );
			RES_MGR_RELEASE_PS( m_pRlRrSpPS );
		}

		bool Create()
		{
			const bool bSupportSpcular = Device()->CapsPixelShaderVersion() >= D3DPS_VERSION(1,4);
			const bool bSupportReflect = WaterReflectMapRender::Inst()->IsHWSurpport();
			const bool bSupportRefract = WaterRefractMapRender::Inst()->IsHWSurpport();
			if( bSupportSpcular )
			{
				m_pSpPS = ResMgr::Inst()->NewPS(_T("data\\system\\shader\\water_s.ps"),MAKELONG(1,4));
			}
			if( bSupportReflect )
			{
				m_pRlVS = ResMgr::Inst()->NewVS(_T("data\\system\\shader\\water_rl.vs"),MAKELONG(2,0));
				m_pRlPS = ResMgr::Inst()->NewPS(_T("data\\system\\shader\\water_rl.ps"),MAKELONG(2,0));
			}
			if( bSupportSpcular && bSupportReflect )
			{
				m_pRlSpVS = ResMgr::Inst()->NewVS(_T("data\\system\\shader\\water_rl_s.vs"),MAKELONG(2,0));
				m_pRlSpPS = ResMgr::Inst()->NewPS(_T("data\\system\\shader\\water_rl_s.ps"),MAKELONG(2,0));
			}
			if( bSupportReflect && bSupportRefract )
			{
				m_pRlRrVS = ResMgr::Inst()->NewVS(_T("data\\system\\shader\\water_rl_rr.vs"),MAKELONG(2,0));
				m_pRlRrPS = ResMgr::Inst()->NewPS(_T("data\\system\\shader\\water_rl_rr.ps"),MAKELONG(2,0));
			}
			if( bSupportSpcular && bSupportReflect && bSupportRefract )
			{
				m_pRlRrSpVS = ResMgr::Inst()->NewVS(_T("data\\system\\shader\\water_rl_rr_s.vs"),MAKELONG(2,0));
				m_pRlRrSpPS = ResMgr::Inst()->NewPS(_T("data\\system\\shader\\water_rl_rr_s.ps"),MAKELONG(2,0));
			}

			return true;
		}

		void AddPatch(const RenderItem& item)
		{
			if(!item.pSGNode->IS_STATIC_CLASS(SGRiverPatch))
				return;

			SGRiverPatch *pSGNode=(SGRiverPatch*)item.pSGNode;

			UINT riverID=pSGNode->GetRiverID();

			map<DWORD,vector<RenderItem>>::iterator findIter=m_rivers.find(riverID);
			if(findIter!=m_rivers.end())
			{
				findIter->second.push_back(item);
			}
			else
			{
				vector<RenderItem> rivers;
				rivers.push_back(item);
				m_rivers.insert(riverPair(riverID,rivers));
			}
		}

		bool NeedRenderRefractMap()
		{
			for(map<DWORD,vector<RenderItem>>::iterator iter=m_rivers.begin();
				iter!=m_rivers.end();++iter)
			{
				for(UINT i=0;i<iter->second.size();i++)
				{
					RenderItem& item=iter->second[i];
					SGRiverPatch* pRiverPatch = (SGRiverPatch*)item.pSGNode;
					if( pRiverPatch->IsEnableEffect() )
						return true;
				}
			}
			return false;
		}

		void Render(SceneGraph *pSG,vector<RenderItem>& trnArray,const tagViewport& viewPort)
		{
			if(m_rivers.empty())
				return;

			IDevice* pDev=Device();
			RenderSys* pRenderSys=Kernel::Inst()->GetRenderSys();
			pDev->SetStreamDefine(pRenderSys->GetStreamDefine(EVType_Water));

			//set world matrix
			pDev->MatLoad(EMS_World,&MathConst::IdentityMat);//upload world matrix of this node
			pDev->FFlushMatrix(true,false,false,false);//flush world matrix only

			const bool bOpenSpecular = Cool3DOption::Inst()->GetOption(Cool3DOption::EO_WaterSpecular)==Cool3DOption::EWS_Open;
			const bool bOpenReflect  = Cool3DOption::Inst()->GetOption(Cool3DOption::EO_WaterReflect)!=Cool3DOption::EWR_None;
			const bool bOpenRefract  = Cool3DOption::Inst()->GetOption(Cool3DOption::EO_WaterRefract)==Cool3DOption::EWRR_Open;

			StateBlock sb;
			sb.SaveState(ERS_FSpecularEnable);
			
			if( bOpenReflect && bOpenRefract && NeedRenderRefractMap() )
				WaterRefractMapRender::Inst()->RenderRefractMap( m_rivers, trnArray, pSG, viewPort );

			for(map<DWORD,vector<RenderItem>>::iterator iter=m_rivers.begin();
				iter!=m_rivers.end();++iter)
			{
				RenderItem& mtlItem=iter->second[0];
				SGRiverPatch* pFirstPatch = (SGRiverPatch*)mtlItem.pSGNode;

				// 高光
				if( bOpenSpecular && pFirstPatch->IsEnableEffect() )
					pDev->SetRenderState( ERS_FSpecularEnable, true );
				else
					pDev->SetRenderState( ERS_FSpecularEnable, false );

				// 设置shader
				if( pFirstPatch->IsEnableEffect() )
				{
					if( bOpenReflect )
					{
						IMaterial *pMtl=mtlItem.pSGNode->GetMtl(0);
						if( !bOpenRefract )                                                        // 反射(高光)
						{
							ASSERT(pMtl->IS_STATIC_CLASS(MWaterReflect));
							MWaterReflect *pWRl=(MWaterReflect*)pMtl;
							if( !SetShadersReflect( bOpenSpecular, pWRl, iter->first, pSG ) )
								continue;
						}
						else                                                                       // 反射+折射(高光)
						{
							ASSERT(pMtl->IS_STATIC_CLASS(MWaterRefract));
							MWaterRefract *pWRr=(MWaterRefract*)pMtl;
							if( !SetShadersRefract( bOpenSpecular, pWRr, iter->first, pSG ) )
								continue;
						}
					}
					else                                                                            // 只有高光
					{
						if( bOpenSpecular )
						{
							if( !SetShadersSpecular() )
								continue;
						}
					}
				}

				for(UINT i=0;i<iter->second.size();i++)
				{
					RenderItem& item=iter->second[i];
					SGRiverPatch* pRiverPatch = (SGRiverPatch*)item.pSGNode;
					if( bOpenReflect && bOpenRefract && pRiverPatch->IsEnableEffect() )
						pRiverPatch->SetRefractMap( WaterRefractMapRender::Inst()->GetRefractRTex() );

					item.pNode->Draw(pRenderSys,item.pSGNode,item.nLod,&item,pSG);
				}

				RenderNode::m_pSpecialVS=NULL;
				RenderNode::m_pSpecialPS=NULL;
			}

			sb.Apply();
		}

		bool SetShadersSpecular()
		{
			if( NULL == m_pSpPS || !m_pSpPS->IsCreated() || NULL == m_pSpPS->GetPS() )
				return false;
			else
			{
				RenderNode::m_pSpecialPS = m_pSpPS->GetPS();
				return true;
			}
		}

		bool SetShadersReflect( const bool bSpecular, MWaterReflect* pMtl, const DWORD dwRiverID, SceneGraph* pSG )
		{
			IVertShader* pVS = NULL;
			IPixelShader* pPS = NULL;
			if( !bSpecular )
			{
				if( ( NULL == m_pRlVS || !m_pRlVS->IsCreated() || NULL == m_pRlVS->GetVS() ) ||
					( NULL == m_pRlPS || !m_pRlPS->IsCreated() || NULL == m_pRlPS->GetPS() ) )
					return false;
				else
				{
					pVS = m_pRlVS->GetVS();
					pPS = m_pRlPS->GetPS();
				}
			}
			else
			{
				 if( ( NULL == m_pRlSpVS || !m_pRlSpVS->IsCreated() || NULL == m_pRlSpVS->GetVS() ) ||
					 ( NULL == m_pRlSpPS || !m_pRlSpPS->IsCreated() || NULL == m_pRlSpPS->GetPS() ) )
					 return false;
				 else
				 {
					 pVS = m_pRlSpVS->GetVS();
					 pPS = m_pRlSpPS->GetPS();
				 }
			}

			Matrix4 matViewPrj;
			if(!WaterReflectMapRender::Inst()->GetRViewPrjMatrix(dwRiverID,matViewPrj))
				return false;

			Transform trans;
			trans.LoadMatrix(pSG->GetCamera()->GetView());
			trans.CombineTransRight(pSG->GetCamera()->GetPrj());
			pVS->SetConstValue("g_matWorldViewPrj",(void*)trans.GetMatrix(),sizeof(Matrix4));
			pVS->SetMatrix("g_matViewPrj",&matViewPrj);

			Vector2 vUVScroll=pMtl->m_bumpUVScroll*Kernel::Inst()->GetAccumTime();
			vUVScroll.x = fmod( vUVScroll.x, 10.0f );
			vUVScroll.y = fmod( vUVScroll.y, 10.0f );
			pVS->SetFloatArray("g_vUVTranslation",(float*)&vUVScroll,2);

			pPS->SetFloat("g_bumpScale",pMtl->m_bumpScale);
			pPS->SetFloatArray("g_blendColor",(float*)&pMtl->m_blendColor,4);

			if( bSpecular )
			{
				tagLight light = pSG->GetSunLight();
				Vector3 lightDir = -light.dir;
				Vector3 camera=pSG->GetCamera()->GetPos();
				Vec3Normalize(lightDir);
				pVS->SetFloatArray("g_directDir",(float*)&lightDir,3);
				pVS->SetFloatArray("g_eyePos",(float*)&camera,3);
				pVS->SetFloatArray("g_specularMtl",(float*)&pMtl->m_color.m_color.specular,4);
				pVS->SetFloatArray("g_specularLightIntensity",(float*)&light.specular,4);

				pPS->SetFloat("g_specularPow",pMtl->m_color.m_color.power);
				pPS->SetFloat("g_specularScale",pMtl->m_specularScale);
			}

			RenderNode::m_pSpecialVS = pVS;
			RenderNode::m_pSpecialPS = pPS;
			return true;
		}

		bool SetShadersRefract( const bool bSpecular, MWaterRefract* pMtl, const DWORD dwRiverID, SceneGraph* pSG )
		{
			IVertShader* pVS = NULL;
			IPixelShader* pPS = NULL;
			if( !bSpecular )
			{
				if( ( NULL == m_pRlRrVS || !m_pRlRrVS->IsCreated() || NULL == m_pRlRrVS->GetVS() ) ||
					( NULL == m_pRlRrPS || !m_pRlRrPS->IsCreated() || NULL == m_pRlRrPS->GetPS() ) )
					return false;
				else
				{
					pVS = m_pRlRrVS->GetVS();
					pPS = m_pRlRrPS->GetPS();
				}
			}
			else
			{
				if( ( NULL == m_pRlRrSpVS || !m_pRlRrSpVS->IsCreated() || NULL == m_pRlRrSpVS->GetVS() ) ||
					( NULL == m_pRlRrSpPS || !m_pRlRrSpPS->IsCreated() || NULL == m_pRlRrSpPS->GetPS() ) )
					return false;
				else
				{
					pVS = m_pRlRrSpVS->GetVS();
					pPS = m_pRlRrSpPS->GetPS();
				}
			}

			Matrix4 matViewPrj;
			if(!WaterReflectMapRender::Inst()->GetRViewPrjMatrix(dwRiverID,matViewPrj))
				return false;

			Transform trans;
			trans.LoadMatrix(pSG->GetCamera()->GetView());
			trans.CombineTransRight(pSG->GetCamera()->GetPrj());
			pVS->SetConstValue("g_matWorldViewPrj",(void*)trans.GetMatrix(),sizeof(Matrix4));
			pVS->SetMatrix("g_matViewPrj",&matViewPrj);

			Vector2 vUVScroll=pMtl->m_bumpUVScroll*Kernel::Inst()->GetAccumTime();
			vUVScroll.x = fmod( vUVScroll.x, 10.0f );
			vUVScroll.y = fmod( vUVScroll.y, 10.0f );
			pVS->SetFloatArray("g_vUVTranslation",(float*)&vUVScroll,2);
			Vector3 camera=pSG->GetCamera()->GetPos();
			pVS->SetFloatArray("g_eyePos",(float*)&camera,3);

			pPS->SetFloat("g_bumpScale",pMtl->m_bumpScale);

			if( bSpecular )
			{
				tagLight light = pSG->GetSunLight();
				Vector3 lightDir = -light.dir;
				Vec3Normalize(lightDir);
				pVS->SetFloatArray("g_directDir",(float*)&lightDir,3);
				pVS->SetFloatArray("g_specularMtl",(float*)&pMtl->m_color.m_color.specular,4);
				pVS->SetFloatArray("g_specularLightIntensity",(float*)&light.specular,4);

				pPS->SetFloat("g_specularPow",pMtl->m_color.m_color.power);
				pPS->SetFloat("g_specularScale",pMtl->m_specularScale);
			}

			RenderNode::m_pSpecialVS = pVS;
			RenderNode::m_pSpecialPS = pPS;

			return true;
		}
	};

	WaterRender* WaterRender::Inst()
	{
		static WaterRender Inst;
		return &Inst;
	}

	void WaterRender::Create()
	{
		m_p=new Member;
		m_bCreated=m_p->Create();
	}

	void WaterRender::Destroy()
	{
		SAFE_DELETE(m_p);
	}

	void WaterRender::Begin()
	{
		if(m_bCreated)
		{
			m_p->m_rivers.clear();
		}
	}

	bool WaterRender::AttachNode( const RenderItem& item )
	{
		if(!m_bCreated)
			return false;

		m_p->AddPatch(item);
		return true;
	}

	void WaterRender::End( SceneGraph *pSG, vector<RenderItem>& trnArray, const tagViewport& viewPort )
	{
		if(m_bCreated)
			m_p->Render(pSG,trnArray,viewPort);
	}

	WaterRender::WaterRender()
	{
		//
	}

	WaterRender::~WaterRender()
	{
		Destroy();
	}
}//namespace Cool3D