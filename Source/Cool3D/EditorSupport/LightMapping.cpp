#include "StdAfx.h"
#include ".\LightMapping.h"
#include ".\SGVisitor.h"
#include ".\IUICallBack.h"
#include "..\Device\IVertShader.h"
#include "..\Device\IPixelShader.h"
#include "..\Device\IRenderTexture.h"
#include "..\Device\IRenderCubeTexture.h"
#include "..\Device\IDeviceVizQuery.h"
#include "..\Device\IDeviceTex.h"
#include "..\Device\StateBlock.h"
#include ".\LightMapTempGen.h"
#include "..\Image\Image.h"
#include "..\SceneGraph\HugeQuadTreeSG.h"
#include "..\SceneGraph\SceneNode.h"
#include "..\SceneGraph\SGLightNode.h"
#include "..\Terrain\TerrainPatch.h"
#include "..\SceneGraph\EntityNode.h"
#include "..\ResSys\ResMgr.h"
#include "..\RenderSys\RenderSys.h"
#include "..\Kernel.h"
#include "..\RenderMtl\IMaterial.h"
#include "..\RenderMtl\MGeneral.h"
#include "..\RenderMtl\MStatic.h"
#include "..\RenderMtl\MKeyFrame.h"
#include "..\RenderMtl\MConstColor.h"
#include "..\RenderSys\RenderNode.h"
#include "AmbientOcclusion.h"

#include <process.h>	// for _beginthreadex
#include "..\Device\DX9Device\DX9RenderTex.h"
#include "..\Device\DX9Device\DX9device.h"
using namespace Detail;
namespace Cool3D
{
	BOOL f_bTerminateThreadLightMappingWriteFile = FALSE;

	//--class LightMapping::Member----------------------------
	class LightMapping::Member
	{

		static const int CubeTextureSize=2048;
		static const int MaxMaterialNum=40;

	public:
		struct tagLightEx
		{
			tagLight	light;
			float		noBlockRange;
		};
		Vector4 diffuseMtls[MaxMaterialNum];
		Vector4 ambientMtls[MaxMaterialNum];
		Vector4 emissive[MaxMaterialNum];

	public:
		IVertShader*		m_pDirectVS;
		IPixelShader*		m_pDirectPS;
		IVertShader*		m_pPointVS;
		IPixelShader*		m_pPointPS;
		IVertShader*		m_pShadowDirectVS;
		IPixelShader*		m_pShadowDirectPS;
		IVertShader*		m_pShadowPointVS;
		IPixelShader*		m_pShadowPointPS;
		IVertShader*		m_pDepthDirectVS;
		IPixelShader*		m_pDepthDirectPS;
		IVertShader*		m_pDepthPointVS;
		IPixelShader*		m_pDepthPointPS;
		IDeviceVizQuery*	m_pVizQuery;
		IRenderTexture*		m_pVizQueryRT;
		IRenderTexture*		m_pDepthRT;
		IRenderCubeTexture*	m_pDepthCubeRT;
		tagLight			m_sunLight;
		Color4f				m_skyCol;

	private:
		bool _MakeShadowMap(IRenderTexture* pRenderTex,IDeviceTex* pPosMap,Matrix4& viewprjMat)
		{
			IDevice* pDev=Device();

			/*
			0____1
			|    |
			3____2
			*/
			Vert_P vert[4];
			vert[0].pos=Vector3(0,		0,		0);
			vert[1].pos=Vector3(1.0f,	0,		0);
			vert[2].pos=Vector3(1.0f,	1.0f,	0);
			vert[3].pos=Vector3(0,		1.0f,	0);

			RenderSys* pSys=Kernel::Inst()->GetRenderSys();

			pRenderTex->Begin();
			StateBlock block;
			block.SaveState(ERS_DepthTestEnable);	pDev->SetRenderState(ERS_DepthTestEnable,false);
			block.SaveState(ERS_DepthWriteEnable);	pDev->SetRenderState(ERS_DepthWriteEnable,false);
			block.SaveState(ERS_CullMode);			pDev->SetCullMode(ECull_None);

			pDev->SetStreamDefine(pSys->GetStreamDefine(EVType_P));

			m_pShadowDirectPS->SetConstValue("g_matViewPrj",(void*)&viewprjMat,sizeof(Matrix4));
			pDev->SetVertShader(m_pShadowDirectVS);
			pDev->SetPixelShader(m_pShadowDirectPS);

			pDev->SetTexture(0,pPosMap);
			pDev->SetSimplerAddress(0,ETAddress_Clamp,ETAddress_Clamp,ETAddress_Clamp);
			pDev->SetSimplerFilter(0,ETFilter_POINT,ETFilter_POINT);
			pDev->SetSimplerMipFilter(0,ETFilter_NONE);

			pDev->SetTexture(1,m_pDepthRT);
			pDev->SetSimplerAddress(1,ETAddress_Clamp,ETAddress_Clamp,ETAddress_Clamp);
			pDev->SetSimplerFilter(1,ETFilter_POINT,ETFilter_POINT);
			pDev->SetSimplerMipFilter(1,ETFilter_NONE);

			pDev->DrawMem(EDraw_TriangleFan,2,&vert[0],sizeof(Vert_P));
			pRenderTex->End();
			block.Apply();

			//pPosMap->WriteToFile(_T("c:\\pos.tga"));
			//pRenderTex->WriteToFile(_T("c:\\shadowmap.tga"));

			pDev->SetTexture(0,(IDeviceTex*)NULL);
			pDev->SetTexture(1,(IDeviceTex*)NULL);
			pDev->SetVertShader(NULL);
			pDev->SetPixelShader(NULL);
			SAFE_DELETE(pPosMap);

			return true;
		}

		bool _MakeCubeShadowMap(IRenderTexture* pRenderTex,IDeviceTex* pPosMap,tagLight& light)
		{
			IDevice* pDev=Device();

			/*
			0____1
			|    |
			3____2
			*/
			Vert_P vert[4];
			vert[0].pos=Vector3(0,		0,		0);
			vert[1].pos=Vector3(1.0f,	0,		0);
			vert[2].pos=Vector3(1.0f,	1.0f,	0);
			vert[3].pos=Vector3(0,		1.0f,	0);

			RenderSys* pSys=Kernel::Inst()->GetRenderSys();

			pRenderTex->Begin();
			StateBlock block;
			block.SaveState(ERS_DepthTestEnable);	pDev->SetRenderState(ERS_DepthTestEnable,false);
			block.SaveState(ERS_DepthWriteEnable);	pDev->SetRenderState(ERS_DepthWriteEnable,false);
			block.SaveState(ERS_CullMode);			pDev->SetCullMode(ECull_None);

			pDev->SetStreamDefine(pSys->GetStreamDefine(EVType_P));

			Vector3 textureSize=Vector3((float)CubeTextureSize,(float)CubeTextureSize,(float)CubeTextureSize);
			Vector3 textureSizeDiv=Vector3(1.0f/(float)CubeTextureSize,1.0f/(float)CubeTextureSize,1.0f/(float)CubeTextureSize);
			float lightRange=light.range*2.0f;
			m_pShadowPointPS->SetConstValue("g_vecLightPosWorld",(void*)&light.pos,sizeof(Vector3));
			m_pShadowPointPS->SetConstValue("g_vecTextureSize",(void*)&textureSize,sizeof(Vector3));
			m_pShadowPointPS->SetConstValue("g_vecTextureSizeDiv",(void*)&textureSizeDiv,sizeof(Vector3));
			m_pShadowPointPS->SetConstValue("g_fZDist",(void*)&lightRange,sizeof(float));
			pDev->SetVertShader(m_pShadowPointVS);
			pDev->SetPixelShader(m_pShadowPointPS);

			pDev->SetTexture(0,pPosMap);
			pDev->SetSimplerAddress(0,ETAddress_Clamp,ETAddress_Clamp,ETAddress_Clamp);
			pDev->SetSimplerFilter(0,ETFilter_POINT,ETFilter_POINT);
			pDev->SetSimplerMipFilter(0,ETFilter_NONE);

			pDev->SetTexture(1,m_pDepthCubeRT);
			pDev->SetSimplerAddress(1,ETAddress_Clamp,ETAddress_Clamp,ETAddress_Clamp);
			pDev->SetSimplerFilter(1,ETFilter_POINT,ETFilter_POINT);
			pDev->SetSimplerMipFilter(1,ETFilter_NONE);

			pDev->DrawMem(EDraw_TriangleFan,2,&vert[0],sizeof(Vert_P));
			pRenderTex->End();
			block.Apply();

			pDev->SetTexture(0,(IDeviceTex*)NULL);
			pDev->SetTexture(1,(IDeviceTex*)NULL);
			pDev->SetVertShader(NULL);
			pDev->SetPixelShader(NULL);
			SAFE_DELETE(pPosMap);

			return true;
		}

		bool _MakeDepthMap(HugeQuadTreeSG* pSG,CameraBase& camera)
		{
			RenderNode::m_pSpecialVS=m_pDepthDirectVS;
			RenderNode::m_pSpecialPS=m_pDepthDirectPS;

			pSG->RenderShadowMap(camera,m_pDepthRT);

			RenderNode::m_pSpecialVS=NULL;
			RenderNode::m_pSpecialPS=NULL;

			//m_pDepthRT->WriteToFile(_T("c:\\depth.tga"));

			return true;
		}

		void _GetCubeMapView( DWORD dwFace, Vector3& lookat, Vector3& up )
		{
			switch( dwFace )
			{
			case D3DCUBEMAP_FACE_POSITIVE_X:
				lookat	= D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
				up		= D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
				break;
			case D3DCUBEMAP_FACE_NEGATIVE_X:
				lookat	= D3DXVECTOR3(-1.0f, 0.0f, 0.0f );
				up		= D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
				break;
			case D3DCUBEMAP_FACE_POSITIVE_Y:
				lookat	= D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
				up		= D3DXVECTOR3( 0.0f, 0.0f,-1.0f );
				break;
			case D3DCUBEMAP_FACE_NEGATIVE_Y:
				lookat	= D3DXVECTOR3( 0.0f,-1.0f, 0.0f );
				up		= D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
				break;
			case D3DCUBEMAP_FACE_POSITIVE_Z:
				lookat	= D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
				up		= D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
				break;
			case D3DCUBEMAP_FACE_NEGATIVE_Z:
				lookat	= D3DXVECTOR3( 0.0f, 0.0f,-1.0f );
				up		= D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
				break;
			}
		}

		bool _MakeCubeDepthMap(HugeQuadTreeSG* pSG,tagLightEx& light)
		{
			RenderNode::m_pSpecialVS=m_pDepthPointVS;
			RenderNode::m_pSpecialPS=m_pDepthPointPS;

			for( int pass=0; pass<6; ++pass )
			{
				if( !m_pDepthCubeRT->SetPass(pass) )
					return false;

				CameraBase camera;
				Vector3 from,lookat,up;
				_GetCubeMapView((DWORD)pass, lookat, up);
				from=light.light.pos;
				lookat=from+lookat;
				camera.SetLookAt(from, lookat, up);
				camera.SetPerspective((float)m_pDepthCubeRT->Size(),(float)m_pDepthCubeRT->Size(),FLOAT_PI/2.0f,0.1f,light.light.range*2.0f);

				Matrix4 viewprjMat;
				D3DXMatrixMultiply(&viewprjMat,camera.GetView(),camera.GetPrj());
				RenderNode::m_pSpecialVS->SetConstValue("g_matCubeMapViewPrj",(void*)&viewprjMat,sizeof(Matrix4));
				RenderNode::m_pSpecialVS->SetConstValue("g_vecLightPosWorld",(void*)&light.light.pos,sizeof(Vector3));

				float zfar=light.light.range*2.0f;
				RenderNode::m_pSpecialPS->SetConstValue("g_fNoBlockRange",(void*)&light.noBlockRange,sizeof(float));
				RenderNode::m_pSpecialPS->SetConstValue("g_fZFar",(void*)&zfar,sizeof(float));

				pSG->RenderCubeShadowMap(camera,m_pDepthCubeRT);
			}

			RenderNode::m_pSpecialVS=NULL;
			RenderNode::m_pSpecialPS=NULL;

			//m_pDepthCubeRT->WriteToFile(_T("c:\\depth.tga"));

			return true;
		}

		void CopyColor4fToVector4(Vector4& v,const Color4f& c)
		{
			v.x=c.R;
			v.y=c.G;
			v.z=c.B;
			v.w=c.A;
		}

	public:
		Member()
		{
			IDevice* pDev=Device();

			m_pDirectVS=pDev->NewVS();
			m_pDirectVS->CreateFromFile(_T("data\\system\\radiosity\\directlight.vs"),"vs_3_0","main");
			m_pDirectPS=pDev->NewPS();
			m_pDirectPS->CreateFromFile(_T("data\\system\\radiosity\\directlight.ps"),"ps_3_0","main");
			m_pPointVS=pDev->NewVS();
			m_pPointVS->CreateFromFile(_T("data\\system\\radiosity\\pointlight.vs"),"vs_3_0","main");
			m_pPointPS=pDev->NewPS();
			m_pPointPS->CreateFromFile(_T("data\\system\\radiosity\\pointlight.ps"),"ps_3_0","main");

			m_pShadowDirectVS=pDev->NewVS();
			m_pShadowDirectVS->CreateFromFile(_T("data\\system\\radiosity\\shadowmap_direct.vs"),"vs_2_0","main");
			m_pShadowDirectPS=pDev->NewPS();
			m_pShadowDirectPS->CreateFromFile(_T("data\\system\\radiosity\\shadowmap_direct.ps"),"ps_2_0","main");
			m_pShadowPointVS=pDev->NewVS();
			m_pShadowPointVS->CreateFromFile(_T("data\\system\\radiosity\\shadowmap_point.vs"),"vs_2_0","main");
			m_pShadowPointPS=pDev->NewPS();
			m_pShadowPointPS->CreateFromFile(_T("data\\system\\radiosity\\shadowmap_point.ps"),"ps_2_a","main");

			m_pDepthDirectVS=pDev->NewVS();
			m_pDepthDirectVS->CreateFromFile(_T("data\\system\\radiosity\\depthmap_direct.vs"),"vs_2_0","main");
			m_pDepthDirectPS=pDev->NewPS();
			m_pDepthDirectPS->CreateFromFile(_T("data\\system\\radiosity\\depthmap_direct.ps"),"ps_2_0","main");

			m_pDepthPointVS=pDev->NewVS();
			m_pDepthPointVS->CreateFromFile(_T("data\\system\\radiosity\\depthmap_point.vs"),"vs_2_0","main");
			m_pDepthPointPS=pDev->NewPS();
			m_pDepthPointPS->CreateFromFile(_T("data\\system\\radiosity\\depthmap_point.ps"),"ps_2_0","main");

			m_pVizQuery=pDev->NewVizQuery();
			m_pVizQuery->Create();

			m_pVizQueryRT=pDev->NewRenderTexture();
			m_pVizQueryRT->Create(32,32,EPF_A8R8G8B8,true,false,1);

			m_pDepthRT=pDev->NewRenderTexture();
			m_pDepthRT->Create(2048,2048,EPF_R32F,true,false,1);

			m_pDepthCubeRT=pDev->NewRenderCubeTexture();
			m_pDepthCubeRT->Create(CubeTextureSize,EPF_R32F,true,1);
		}

		~Member()
		{
			SAFE_DELETE(m_pDirectVS);
			SAFE_DELETE(m_pDirectPS);
			SAFE_DELETE(m_pPointVS);
			SAFE_DELETE(m_pPointPS);
			SAFE_DELETE(m_pVizQuery);
			SAFE_DELETE(m_pVizQueryRT);
			SAFE_DELETE(m_pDepthRT);
			SAFE_DELETE(m_pDepthCubeRT);
			SAFE_DELETE(m_pShadowDirectVS);
			SAFE_DELETE(m_pShadowDirectPS);
			SAFE_DELETE(m_pShadowPointVS);
			SAFE_DELETE(m_pShadowPointPS);
			SAFE_DELETE(m_pDepthDirectVS);
			SAFE_DELETE(m_pDepthDirectPS);
			SAFE_DELETE(m_pDepthPointVS);
			SAFE_DELETE(m_pDepthPointPS);
		}

		Vector3 CalcNormal(Vector4* normal,Vector3& pos,tagLight& light)
		{
			if(fabs(normal->w-2.0f)>0.1f)//单面材质
				return Vector3(normal->x,normal->y,normal->z);

			Vector3 dir;
			if(light.type==Light_Direct)
			{
				dir=-light.dir;
			}
			else if(light.type==Light_Point)
			{
				dir=light.pos-pos;
			}
			Vec3Normalize(dir);

			Vector3 normal1(normal->x,normal->y,normal->z);
			Vector3 normal2(-normal->x,-normal->y,-normal->z);
			Vec3Normalize(normal1);
			Vec3Normalize(normal2);

			float dot1=D3DXVec3Dot(&normal1,&dir);
			float dot2=D3DXVec3Dot(&normal2,&dir);
			
			if(dot1>dot2)
				return normal1;
			else
				return normal2;
		}

		bool LightCost(Vector3& pos,Vector3& normal,tagLight& light)
		{
			bool bRet=false;

			if(light.type==Light_Direct)
			{
				Vector3 dir=-light.dir;
				Vec3Normalize(dir);
				Vec3Normalize(normal);
				float dot=D3DXVec3Dot(&normal,&dir);
				if(dot>-0.2f)
					bRet=true;
			}
			else if(light.type==Light_Point)
			{
				float dist=Vec3Dist(pos,light.pos);
				if(dist<=light.range)
				{
					Vector3 dir=light.pos-pos;
					Vec3Normalize(dir);
					Vec3Normalize(normal);
					float dot=D3DXVec3Dot(&normal,&dir);
					if(dot>-0.2f)
						bRet=true;
				}
			}
			else
				ASSERT(0);

			return bRet;
		}

		bool LightCost(const AABBox& nodeBox,const tagLight& light)
		{
			switch( light.type )
			{
			case Light_Direct:
				return true;
			case Light_Point:
				{
					//--AABBox与球体是否相交
					const Vector3& center=light.pos;
					float radius=light.range;
					const Vector3& min=nodeBox.min;
					const Vector3& max=nodeBox.max;

					float s, d = 0;
					if( center.x < min.x )
					{
						s=center.x-min.x;
						d+=s*s;
					}
					else if( center.x > max.x )
					{
						s=center.x-max.x;
						d+=s*s;
					}
					if( center.y < min.y )
					{
						s=center.y-min.y;
						d+=s*s;
					}
					else if( center.y > max.y )
					{
						s=center.y-max.y;
						d+=s*s;
					}
					if( center.z < min.z )
					{
						s=center.z-min.z;
						d+=s*s;
					}
					else if( center.z > max.z )
					{
						s=center.z-max.z;
						d+=s*s;
					}					

					return d <= radius * radius;
				}
			case Light_Spot:
				return false;
			default:
				return false;
			}
		}

		IRenderTexture* MakeShadowMap(SceneNode* pNode,tagLightEx& light,HugeQuadTreeSG* pSG)
		{
			IDeviceTex* pPosMap=LoadPosMap(pNode);

			IDevice* pDev=Device();
			IRenderTexture* pRenderTex=pDev->NewRenderTexture();
			pRenderTex->Create(pPosMap->Width(),pPosMap->Height(),EPF_R32F,false,false);

			pRenderTex->Begin();
			pDev->Clear(true,false,false,0,1,0);
			pRenderTex->End();

			AABBox nodeBox;
			pNode->GetBox(nodeBox);
			if( LightCost(nodeBox,light.light) )
			{
				CameraBase camera;

				if( light.light.type==Light_Direct )
				{
					float dist=100000.0f;
					Vector3 dir=light.light.dir;
					D3DXVec3Normalize(&dir,&dir);
					Vector3 lookAt=nodeBox.GetCenter();
					Vector3 lookFrom=lookAt-dir*dist*0.5f;
					camera.SetLookAt(lookFrom,lookAt);
					camera.SetOrtho((float)m_pDepthRT->Width(),(float)m_pDepthRT->Height(),0,dist);
					camera.AdjustAdaptive(nodeBox);

					if( _MakeDepthMap(pSG,camera) )
					{
						Matrix4 viewprjMat;
						D3DXMatrixMultiply(&viewprjMat,camera.GetView(),camera.GetPrj());

						_MakeShadowMap(pRenderTex,pPosMap,viewprjMat);
					}
				}
				else if( light.light.type==Light_Point )
				{
					if( _MakeCubeDepthMap(pSG,light) )
					{
						_MakeCubeShadowMap(pRenderTex,pPosMap,light.light);
					}
				}
				else if( light.light.type=Light_Spot ) // todo	
				{
				}
			}

			//pRenderTex->WriteToFile(_T("c:\\shadowmap.tga"));

			return pRenderTex;
		}

		IDeviceTex* MakeShadowMap_RayCollide(SceneNode* pNode,tagLightEx& light,HugeQuadTreeSG* pSG)
		{
			Image posMap;
			tstring szPosMap=LightMapTempGen::MakePosMapName(pNode->GetLightmap());
			bool bRet=posMap.LoadFromFile(szPosMap.c_str());
			THROW_FALSE(bRet,_T("load posmap"));

			Image normalMap;
			tstring szNormalMap=LightMapTempGen::MakeNormalMapName(pNode->GetLightmap());
			bRet=normalMap.LoadFromFile(szNormalMap.c_str());
			THROW_FALSE(bRet,_T("load normalmap"));
			
			Image shadowMap;
			shadowMap.Create(posMap.Width(),posMap.Height(),EPF_A8);

			Vector4* p=(Vector4*)posMap.GetRawBuffer();
			Vector4* n=(Vector4*)normalMap.GetRawBuffer();
			BYTE*	 s=(BYTE*)shadowMap.GetRawBuffer();

			
			for(UINT y=0;y<posMap.Height();y++)
			{
				for(UINT x=0;x<posMap.Width();x++)
				{
					*s=0;

					if(n->w!=0)
					{
						Vector3 pos(p->x,p->y,p->z);
						Vector3 normal=CalcNormal(n,pos,light.light);
						if(LightCost(pos,normal,light.light))
						{
							float dist;
							Vector3 lgtDir;
							if(light.light.type==Light_Direct)
							{
								dist=100000.0f;
								lgtDir=-light.light.dir;
							}
							else
							{
								dist=Vec3Dist(light.light.pos,pos);
								lgtDir=light.light.pos-pos;
								dist-=light.noBlockRange;
							}

							if(dist<=0.0f
								|| pSG->RenderForRayCollide(pos,lgtDir,dist,m_pVizQueryRT,m_pVizQuery))
								*s=255;
						}
					}

					p++;
					n++;
					s++;
				}
			}

			//--
			IDeviceTex* pShadowMap=Device()->NewTexture();
			pShadowMap->CreateFromImage(&shadowMap,1);
			return pShadowMap;
		}

		IDeviceTex* LoadPosMap(SceneNode* pNode)
		{
			IDeviceTex* pTex=Device()->NewTexture();
			IFS* pFS=Kernel::Inst()->DefaultFS();
			tstring szPath=LightMapTempGen::MakePosMapName(pNode->GetLightmap());
			pTex->LoadFromFile(pFS,szPath.c_str(),1);
			return pTex;
		}

		IDeviceTex* LoadNormalMap(SceneNode* pNode)
		{
			IDeviceTex* pTex=Device()->NewTexture();
			IFS* pFS=Kernel::Inst()->DefaultFS();
			tstring szPath=LightMapTempGen::MakeNormalMapName(pNode->GetLightmap());
			pTex->LoadFromFile(pFS,szPath.c_str(),1);
			return pTex;
		}

		IDeviceTex* LoadAOMap(SceneNode *pNode)
		{
			IDeviceTex* pTex=Device()->NewTexture();
			IFS* pFS=Kernel::Inst()->DefaultFS();
			tstring szPath=AmbientOcclusion::MakeAOMapName(pNode->GetLightmap());
			pTex->LoadFromFile(pFS,szPath.c_str(),1);
			return pTex;
		}

		void SearchSGLights(SceneNode* pNode,HugeQuadTreeSG* pSG,vector<tagLightEx>& lights)
		{
			AABBox box;
			pNode->GetBox(box);

			vector<SceneNode*> sceneNodes;
			pSG->GetStaticNodes(sceneNodes);

			for(vector<SceneNode*>::iterator iter=sceneNodes.begin();
				iter!=sceneNodes.end();++iter)
			{
				SceneNode* pSGNode=*iter;
				if(pSGNode->IS_STATIC_CLASS(SGLightNode))
				{
					SGLightNode *pLgt=(SGLightNode*)pSGNode;
					if(!pLgt->IsDynamic()
						&& pLgt->LightCost(box)>0.005f)
					{
						tagLightEx lightEx;
						lightEx.light=*pLgt->GetLight();
						lightEx.noBlockRange=pLgt->GetNoBlockRange();
						lights.push_back(lightEx);
					}
				}
			}
		}

		void GetMtl(SceneNode* pNode,UINT index,tagRenderColor& mtl)
		{
			mtl=RenderSys::WhiteRenderColor;

			if(pNode->IS_STATIC_CLASS(EntityNode))
			{
				EntityNode* pEntity=(EntityNode*)pNode;

				IMaterial* pMtl=pEntity->GetMtl(index);
				if(pMtl->IS_STATIC_CLASS(MStatic))
				{
					MStatic* pMG=(MStatic*)pMtl;
					if(pMG->m_pColor!=NULL)
						mtl=pMG->m_pColor->m_color;
				}
				else if(pMtl->IS_STATIC_CLASS(MKeyFrame))
				{
					MKeyFrame *pMKF=(MKeyFrame*)pMtl;
					if(pMKF->m_pColor!=NULL)
						mtl=pMKF->m_pColor->m_color;
				}
			}
		}

		void GetMtl(SceneNode *pNode)
		{
			memset(diffuseMtls,0,MaxMaterialNum*sizeof(diffuseMtls[0]));
			memset(ambientMtls,0,MaxMaterialNum*sizeof(ambientMtls[0]));
			memset(emissive,0,MaxMaterialNum*sizeof(emissive[0]));

			if(pNode->IS_STATIC_CLASS(TerrainPatch))
			{
				CopyColor4fToVector4(diffuseMtls[0],RenderSys::WhiteRenderColor.diffuse);
				CopyColor4fToVector4(ambientMtls[0],RenderSys::WhiteRenderColor.ambient);
				CopyColor4fToVector4(emissive[0],RenderSys::WhiteRenderColor.emissive);

				TerrainPatch* pPatch=(TerrainPatch*)pNode;
				IMaterial* pMtl=pPatch->GetMtl(TerrainPatch::EMType_Editor);
				if(pMtl!=NULL
					&&pMtl->IS_STATIC_CLASS(MGeneral))
				{
					MGeneral* pMG=(MGeneral*)pMtl;
					if(pMG->m_pColor!=NULL)
					{
						CopyColor4fToVector4(diffuseMtls[0],pMG->m_pColor->m_color.diffuse);
						CopyColor4fToVector4(ambientMtls[0],pMG->m_pColor->m_color.ambient);
						ambientMtls[0].x = ambientMtls[0].y = ambientMtls[0].z = ambientMtls[0].w = 1.0f;
						CopyColor4fToVector4(emissive[0],pMG->m_pColor->m_color.emissive);
					}
				}
			}
			else
			{
				UINT mtlNum=pNode->GetNumMtl();
				THROW_FALSE( MaxMaterialNum>=mtlNum, _T("材质数量超过规定数目40！") );

				for( UINT i=0; i<mtlNum; i++ )
				{
					tagRenderColor mtl;
					GetMtl(pNode, i, mtl);
					CopyColor4fToVector4(diffuseMtls[i],mtl.diffuse);
					// CopyColor4fToVector4(ambientMtls[i],mtl.ambient);
					ambientMtls[i].x = ambientMtls[i].y = ambientMtls[i].z = ambientMtls[i].w = 1.0f;
					CopyColor4fToVector4(emissive[i],mtl.emissive);
				}
			}
		}
	};

	//--class LightMapping------------------------------------
	LightMapping::LightMapping(IUICallBack* pUI)
		: m_pUI(pUI)
		, m_bCalcAO(FALSE)
	{
		m_p=new Member;
	}

	LightMapping::~LightMapping(void)
	{
		delete m_p;
	}

	bool LightMapping::OnVisitSceneNode(HugeQuadTreeSG* pSG,SceneNode* pNode,int index,int totalNum,DWORD param)
	{
		DWORD dwTime = timeGetTime();
		if(_tcslen(pNode->GetLightmap())==0)
			return true;

		vector<Member::tagLightEx> lights;
		Member::tagLightEx sunLightEx;
		sunLightEx.light=m_p->m_sunLight;
		sunLightEx.noBlockRange=0.0f;
		lights.push_back(sunLightEx);
		m_p->SearchSGLights(pNode,pSG,lights);

		IDeviceTex* pPosMap=m_p->LoadPosMap(pNode);
		IDeviceTex* pNormalMap=m_p->LoadNormalMap(pNode);
		IDeviceTex* pAOMap=NULL;
		if( m_bCalcAO )
		{
			pAOMap=m_p->LoadAOMap(pNode);
		}

		IDevice* pDev=Device();
		IRenderTexture* pRenderTex=pDev->NewRenderTexture();
		if(pNode->IS_STATIC_CLASS(TerrainPatch))
			pRenderTex->Create(pPosMap->Width(),pPosMap->Height(),EPF_A8R8G8B8,false,false,1);
		else
			pRenderTex->Create(pPosMap->Width(),pPosMap->Height(),EPF_X8R8G8B8,false,false,1);

		/*
		0____1
		|    |
		3____2
		*/
		Vert_P vert[4];
		vert[0].pos=Vector3(0,		0,		0);
		vert[1].pos=Vector3(1.0f,	0,		0);
		vert[2].pos=Vector3(1.0f,	1.0f,	0);
		vert[3].pos=Vector3(0,		1.0f,	0);

		RenderSys* pSys=Kernel::Inst()->GetRenderSys();

		pRenderTex->Begin();
		pDev->Clear(true,false,false,0,1,0);
		pRenderTex->End();

		for(int i=0;i<(int)lights.size();i++)
		{
			Member::tagLightEx& light=lights[i];

			IRenderTexture *pShadowMap=m_p->MakeShadowMap(pNode,light,pSG);
			//IDeviceTex* pShadowMap=m_p->MakeShadowMap_RayCollide(pNode,light,pSG);

			pRenderTex->Begin();

			StateBlock block;
			block.SaveState(ERS_AlphaBlendEnable);	pDev->EnableAlphaBlend(true);
			block.SaveState(ERS_SrcBlend);
			block.SaveState(ERS_DstBlend);			pDev->SetAlphaBlend(EBlend_One,EBlend_One);
			block.SaveState(ERS_DepthTestEnable);	pDev->SetRenderState(ERS_DepthTestEnable,false);
			block.SaveState(ERS_DepthWriteEnable);	pDev->SetRenderState(ERS_DepthWriteEnable,false);
			block.SaveState(ERS_CullMode);			pDev->SetCullMode(ECull_None);

			pDev->SetStreamDefine(pSys->GetStreamDefine(EVType_P));

			pDev->SetTexture(0,pPosMap);
			pDev->SetSimplerAddress(0,ETAddress_Clamp,ETAddress_Clamp,ETAddress_Clamp);
			pDev->SetSimplerFilter(0,ETFilter_POINT,ETFilter_POINT);
			pDev->SetSimplerMipFilter(0,ETFilter_NONE);

			pDev->SetTexture(1,pNormalMap);
			pDev->SetSimplerAddress(1,ETAddress_Clamp,ETAddress_Clamp,ETAddress_Clamp);
			pDev->SetSimplerFilter(1,ETFilter_POINT,ETFilter_POINT);
			pDev->SetSimplerMipFilter(1,ETFilter_NONE);

			pDev->SetTexture(2,pShadowMap);
			pDev->SetSimplerAddress(2,ETAddress_Clamp,ETAddress_Clamp,ETAddress_Clamp);
			pDev->SetSimplerFilter(2,ETFilter_POINT,ETFilter_POINT);
			pDev->SetSimplerMipFilter(2,ETFilter_NONE);

			if( m_bCalcAO )
			{
				pDev->SetTexture(3,pAOMap);
				pDev->SetSimplerAddress(3,ETAddress_Clamp,ETAddress_Clamp,ETAddress_Clamp);
				pDev->SetSimplerFilter(3,ETFilter_POINT,ETFilter_POINT);
				pDev->SetSimplerMipFilter(3,ETFilter_NONE);
			}

			m_p->GetMtl(pNode);
			UINT mtlNum=pNode->GetNumMtl();
			if( mtlNum==0 )
				mtlNum=1;

			if(light.light.type==Light_Direct)
			{
				m_p->m_pDirectPS->SetConstValue("g_lightDir",&light.light.dir,sizeof(light.light.dir));
				m_p->m_pDirectPS->SetVectorArray("g_vecDiffuseMtl",&m_p->diffuseMtls[0],mtlNum);
				m_p->m_pDirectPS->SetVectorArray("g_vecAmbientMtl",&m_p->ambientMtls[0],mtlNum);
				m_p->m_pDirectPS->SetVectorArray("g_vecEmissive",&m_p->emissive[0],mtlNum);
				m_p->m_pDirectPS->SetColor4f("g_lightDiffuse",light.light.diffuse);

				if( m_bCalcAO )
					m_p->m_pDirectPS->SetColor4f("g_lightAmbient",m_p->m_skyCol);
				else
					m_p->m_pDirectPS->SetColor4f("g_lightAmbient",light.light.ambient);

				m_p->m_pDirectPS->SetConstValue("g_bCalcAO",&m_bCalcAO,sizeof(m_bCalcAO));
				m_p->m_pDirectPS->SetInt("g_numMtl",mtlNum);

				pDev->SetVertShader(m_p->m_pDirectVS);
				pDev->SetPixelShader(m_p->m_pDirectPS);
			}
			else if(light.light.type==Light_Point)
			{
				m_p->m_pPointPS->SetConstValue("g_lightPos",&light.light.pos,sizeof(light.light.pos));
				m_p->m_pPointPS->SetVectorArray("g_vecDiffuseMtl",&m_p->diffuseMtls[0],mtlNum);
				m_p->m_pPointPS->SetColor4f("g_lightDiffuse",light.light.diffuse);
				m_p->m_pPointPS->SetConstValue("g_lightRange",&light.light.range,sizeof(light.light.range));
				m_p->m_pPointPS->SetConstValue("g_lightAtten",&light.light.attenuation,sizeof(light.light.attenuation));
				m_p->m_pPointPS->SetInt("g_numMtl",mtlNum);

				pDev->SetVertShader(m_p->m_pPointVS);
				pDev->SetPixelShader(m_p->m_pPointPS);
			}

			pDev->DrawMem(EDraw_TriangleFan,2,&vert[0],sizeof(Vert_P));

			pRenderTex->End();

			block.Apply();
			SAFE_DELETE(pShadowMap);
		}

		// 将显存纹理拷到内存纹理，然后交给线程写盘
		IDirect3DSurface9* pSurface = ((DX9RenderTex*)pRenderTex)->GetDX9Surface();
		IDirect3DSurface9* pDestSurface = NULL;
		IDirect3DTexture9* pTempTex = NULL;
		HRESULT hr = 0;
		while( TRUE )
		{
			if(pNode->IS_STATIC_CLASS(TerrainPatch))
				hr = D3DXCreateTexture(DX9Dev(), pPosMap->Width(), pPosMap->Height(), 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pTempTex);
			else
				hr = D3DXCreateTexture(DX9Dev(), pPosMap->Width(), pPosMap->Height(), 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &pTempTex);

			if( D3D_OK == hr)
				break;

			Sleep(1000);
		}
		pTempTex->GetSurfaceLevel(0, &pDestSurface);
		DX9Dev()->GetRenderTargetData(pSurface, pDestSurface);
		SAFE_RELEASE(pDestSurface);
		tagThreadData* pData = new tagThreadData;
		pData->pTexture = pTempTex;
		pData->strName = pNode->GetLightmap();
		m_listRT.PushBack(pData);
		// pRenderTex->WriteToFile(pNode->GetLightmap());

		//--
		pDev->SetTexture(0,(IDeviceTex*)NULL);
		pDev->SetTexture(1,(IDeviceTex*)NULL);
		pDev->SetTexture(2,(IDeviceTex*)NULL);
		pDev->SetTexture(3,(IDeviceTex*)NULL);
		pDev->SetVertShader(NULL);
		pDev->SetPixelShader(NULL);
		SAFE_DELETE(pPosMap);
		SAFE_DELETE(pNormalMap);
		SAFE_DELETE(pAOMap);
		SAFE_DELETE(pRenderTex);

		//--
		m_pUI->SetProgress(index+1,totalNum);

		while(m_listRT.Size()>50)
			Sleep(1000);	// 避免内存上涨过快

		// 显示调试信息
		// m_pUI->AddMsg(_T("index=%d, time=%d, list:%d\r\n"), index, timeGetTime() - dwTime, m_listRT.Size());
		return true;
	}

	UINT WINAPI ThreadLightMappingWriteFile(LPVOID pParam);
	bool LightMapping::Start(const TCHAR* szSGPath,const tagLight* pSunLight,Color4f& skyCol,BOOL bCalcAO)
	{
		m_pUI->SetProgress(0,1);

		ResMgr::Inst()->EnableIOAsync(false);
		m_p->m_sunLight=*pSunLight;
		m_p->m_skyCol=skyCol;
		m_bCalcAO=bCalcAO;

		f_bTerminateThreadLightMappingWriteFile = FALSE;
		HANDLE hHandle = (HANDLE)_beginthreadex(NULL, 0, ThreadLightMappingWriteFile, &m_listRT, 0, NULL);

		SGVisitor visitor;
		bool bResult = visitor.Start(szSGPath,128.0f*50.0f,512.0f*50.0f,this,0);

		// 等待写盘完成
		f_bTerminateThreadLightMappingWriteFile = TRUE;
		::WaitForSingleObject( hHandle, INFINITE );
		return bResult;
	}


	UINT WINAPI ThreadLightMappingWriteFile(LPVOID pParam)
	{
		TSafeList<LightMapping::tagThreadData*>* pList = (TSafeList<LightMapping::tagThreadData*>*)pParam;

		while( !f_bTerminateThreadLightMappingWriteFile || pList->Size() > 0 )
		{
			while( pList->Size() > 0 )
			{
				LightMapping::tagThreadData* pData = pList->PopFront();
				IDirect3DTexture9* pRenderTexture = pData->pTexture;
				IDirect3DSurface9* pSurface = NULL;
				pRenderTexture->GetSurfaceLevel(0, &pSurface);

				while(TRUE)
				{
					if( D3D_OK == D3DXSaveSurfaceToFile(pData->strName.c_str(), D3DXIFF_TGA, pSurface, NULL, NULL) )
						break;
				}


				SAFE_RELEASE(pSurface);
				SAFE_RELEASE(pRenderTexture);
				SAFE_DELETE(pData);
			}

			::WaitForSingleObject(pList->GetEvent(), 100);
		}
		return 0;
	}

}//namespace Cool3D