#include "StdAfx.h"
#include ".\terraineditor.h"
#include "..\ResSys\VertStream.h"
#include "..\Device\IDevice.h"
#include "..\Device\IIndexBuffer.h"
#include "..\Device\IVertBuffer.h"
#include "..\Device\IDeviceTex.h"
#include "..\Device\StateBlock.h"
#include "..\Kernel.h"
#include "..\Image\Image.h"
#include "..\Util\Exception.h"
#include "..\Util\FMemory.h"
#include "..\RenderSys\RenderSys.h"
#include "..\RenderSys\MtlRenderCache.h"
#include "..\SceneGraph\SceneGraph.h"
#include "..\Math\CameraBase.h"
#include "..\Math\Noise.h"
#include "..\Terrain\Heightmap.h"
#include "TQuadTreeTriangulation.h"
#include "..\Terrain\TerrainPatchRes.h"
#include "..\Terrain\TerrainMtlMultiLayers.h"
#include "..\RenderMtl\MConstColor.h"
#include "BrushOp.h"
#include "TMtlEditor.h"
#include "TEditorPatchProxy.h"
#include "TMapLayerExt.h"
#include "..\VFS\IFS.h"
#include "..\VFS\FileUtil.h"
#include "..\SceneGraph\NullSceneGraph.h"
#include "..\Terrain\SGTrnDecoPatch.h"
#include "..\Terrain\SGGrassPatch.h"
#include "..\Terrain\TerrainPatchRes.h"
#include <shellapi.h>
#include "..\Math\Frustum.h"
#include "..\Device\DrawX.h"
#include "..\Util\FileUtil.h"
#include "..\SceneGraph\sglightnode.h"
#include ".\TRiverEditor.h"
#include ".\TEditorPatch.h"
#include ".\EditorDecoFile.h"
#include "..\EditorSupport\TRiverPatchProxy.h"
#include "..\Terrain\BitMap.h"

namespace Cool3D
{

	class SortByDistValFun
	{
	public:
		Vector3 m_center;

		SortByDistValFun(const Vector3& center):m_center(center)
		{}

		bool operator()(const SceneNode* pNode1,const SceneNode* pNode2)
		{
			AABBox box1;
			AABBox box2;
			pNode1->GetBox(box1);
			pNode2->GetBox(box2);
			Vector3 c1=(box1.max+box1.min)*0.5f;
			Vector3 c2=(box2.max+box2.min)*0.5f;
			Vector3 t1=c1-m_center;
			Vector3 t2=c2-m_center;
			float d1=D3DXVec3Length(&t1);
			float d2=D3DXVec3Length(&t2);
			return d1<d2;
		}
	};

	void _SortByDistVal(vector<SceneNode*>& array,const Vector3& center)
	{
		SortByDistValFun sortObj(center);
		sort(array.begin(),array.end(),sortObj);
	}

	//todo:使用单独的alpha通道texture,而非预先混合的贴图来处理layer
	//1.所有layer的u,v scale必须一致,因为vert buffer只有一个
	const TCHAR* const TerrainEditorBasePath=_T(".\\Data\\TerrainEditor");
	namespace Detail
	{
		Noise	BOP_ApplyNoise::m_noise;
	};
	using namespace Detail;

	int TerrainEditor::OP_Draw_Dist=1000;
	int TerrainEditor::OP_Edit_Dist=300;
	int TerrainEditor::OP_Deco_Draw_Size=160;

	class TerrainEditor::Member
	{
	public:
		typedef vector<SceneNode*>::iterator SNVecIter;
		typedef set<SceneNode*>::iterator SNSetIter;

		Heightmap		*m_pHMap;
		Heightmap		*m_pWaterMap;//其实应该是一个bit map,为了方便编辑,所以使用height map

		//--map
		Vector2			m_layerUVScale[TRN_MAX_UV_SET];//一个图素占用多少个格子
		tagTMapLayerExt	m_layers[MAX_MAP_LAYER];	//最多MAX层
		
		tagTDecoLayer	m_decos[MAX_DECO_LAYER];	//最多MAX层
		Heightmap		m_decoDensityMap[MAX_DECO_LAYER];//装饰层浓度图,与上面一一对应
		SGTrnDecoPatch*	m_decosView[MAX_DECO_LAYER];
		Vector3			m_lastDecoUpdatePos;

		TRiverEditor	m_riverEditor;

		//--编辑器渲染
		vector<TEditorPatchProxy*>	m_patchArray;
		
		//--uv vb
		IVertBuffer		*m_pTexCoordVB;

		//--
		bool			m_bDrawWireFrame;	//以线框模式画图
		tstring			m_szDetailMap;
		MConstColor		m_wireColor;
		EEditFocus		m_editFocus;
		tstring			m_szName;
		bool			m_bDisRole;			//在绘制地形时显示一个方块,与人物等高

		BrushOpRecorder	m_brushRecorder;

		EditorDecoFile	m_decosFile[MAX_DECO_LAYER];

		Color4f			m_mtlDiffuse;		//漫反射材质
		Color4f			m_mtlAmbient;		//环境光材质
		Color4f			m_mtlSpecular;		//高光材质
		float			m_mtlPower;			//镜面指数
	public:
		Member():m_lastDecoUpdatePos(0,0,0)
		{
			for(int i=0;i<TerrainEditor::MAX_MAP_LAYER;i++)
				m_layers[i].Zero();
			FZeroMem(m_decos,sizeof(m_decos));
			FZeroMem(m_decosView,sizeof(m_decosView));
			m_pHMap=NULL;
			m_bDrawWireFrame=false;
			m_wireColor.m_bUsingVertColor=true;
			m_szName=_T("default");
			m_szDetailMap=_T("");
			memset(m_layerUVScale,0,sizeof(m_layerUVScale));
			m_layerUVScale[0]=Vector2(1.0f,1.0f);
			m_bDisRole=false;
			m_pTexCoordVB=NULL;
			m_mtlDiffuse.R=m_mtlDiffuse.G=m_mtlDiffuse.B=m_mtlDiffuse.A=1.0f;
			m_mtlAmbient.R=m_mtlAmbient.G=m_mtlAmbient.B=m_mtlAmbient.A=1.0f;
			m_mtlSpecular.R=m_mtlSpecular.G=m_mtlSpecular.B=m_mtlSpecular.A=1.0f;
			m_mtlPower=8.0f;
		}

		~Member()
		{
			FreeAll();
		}

		void FreeAll()
		{
			int i;
			for(i=0;i<TerrainEditor::MAX_MAP_LAYER;i++)
			{
				m_layers[i].Zero();
			}
			FZeroMem(m_decos,sizeof(m_decos));
			for(i=0;i<TerrainEditor::MAX_DECO_LAYER;i++)
			{
				m_decoDensityMap[i].Destroy();
				SAFE_DELETE(m_decosView[i]);
			}

			m_brushRecorder.Clear();
			SAFE_DELETE(m_pHMap);

			SAFE_DELETE(m_pTexCoordVB);

			m_riverEditor.ClearRivers();

			FreePatchs();
		}

		void FreePatchs()
		{
			for(size_t i=0;i<m_patchArray.size();i++)
			{
				TEditorPatchProxy *pPatch=m_patchArray[i];
				delete pPatch;
			}
			m_patchArray.clear();
		}

		bool GetShowPointLights( NullSceneGraph *pSG,vector<SceneNode*>& lights )
		{
			set<SceneNode*> lightList=pSG->GetSceneNodes();

			for(SNSetIter iter=lightList.begin();iter!=lightList.end();++iter)
			{
				SceneNode *pNode=(SceneNode*)(*iter);
				if(pNode->IS_STATIC_CLASS(SGLightNode))
				{
					SGLightNode *pLight=(SGLightNode*)(*iter);
					lights.push_back(pLight);
				}
			}

			if(lights.size()>0)
				return true;
			else
				return false;
		}

		//--更新所有Patch proxy的状态,并且画出
		void UpdateAndDrawPatchs(CameraBase *pCamera,TerrainEditor* pEditor,SceneGraph *pSG)
		{
			POINT pt=m_pHMap->World2Tile(pCamera->GetLookat());
			if(pt.x<EPatchSize/2)
				pt.x=EPatchSize/2;
			if(pt.y<EPatchSize/2)
				pt.y=EPatchSize/2;
			if(pt.x>(int)m_pHMap->Width()-EPatchSize/2)
				pt.x=m_pHMap->Width()-EPatchSize/2;
			if(pt.y>(int)m_pHMap->Height()-EPatchSize/2)
				pt.y=m_pHMap->Height()-EPatchSize/2;

            RECT drawRect,editRect;
			drawRect.left=pt.x-TerrainEditor::OP_Draw_Dist;
			drawRect.right=pt.x+TerrainEditor::OP_Draw_Dist;
			drawRect.top=pt.y-TerrainEditor::OP_Draw_Dist;
			drawRect.bottom=pt.y+TerrainEditor::OP_Draw_Dist;
			editRect.left=pt.x-TerrainEditor::OP_Edit_Dist;
			editRect.right=pt.x+TerrainEditor::OP_Edit_Dist;
			editRect.top=pt.y-TerrainEditor::OP_Edit_Dist;
			editRect.bottom=pt.y+TerrainEditor::OP_Edit_Dist;

			Frustum frustum;
			frustum.Extract(pCamera->GetView(),pCamera->GetPrj());

			for(size_t i=0;i<m_patchArray.size();i++)
			{
				TEditorPatchProxy *pPatch=m_patchArray[i];
				pPatch->Update(pEditor,m_pHMap,drawRect,editRect);

				TEditorPatch *pRealPatch=pPatch->GetPatch();
				if(pRealPatch==NULL)
					continue;

				//--add lights
				int numLights=0;
				if(pSG->IS_STATIC_CLASS(NullSceneGraph))
				{
					NullSceneGraph* pNullSG=(NullSceneGraph*)pSG;
					vector<SceneNode*> lightList;
					if( GetShowPointLights(pNullSG,lightList) )
					{
						RECT rc;
						pPatch->GetMapRect(rc);
						Vector3 leftTop,rightBottom;
						m_pHMap->GetVertPos(rc.left,rc.top,leftTop);
						m_pHMap->GetVertPos(rc.right-1,rc.bottom-1,rightBottom);
						Vector3 center=(leftTop+rightBottom)*0.5f;
						_SortByDistVal(lightList,center);

						numLights=min((int)lightList.size(),7);
						for(int i=0;i<numLights;i++)
						{
							SGLightNode* pLightNode=(SGLightNode*)lightList[i];

							tagLight *pLightInfo=pLightNode->GetLight();
							Device()->FSetLight(i+1,pLightInfo);
						}
					}
				}

				pPatch->Draw(frustum);

				//--clear lights
				for (int i=0;i<numLights;i++)
					Device()->FSetLight(i+1,NULL);
			}
		}

		//创建当前Height map的所有Patch的proxy
		void CreatePatchs()
		{
			//--顺便生成一下Noise
			Detail::BOP_ApplyNoise::m_noise.SetFreq(4.0f);
			Detail::BOP_ApplyNoise::m_noise.Generate(64,64);
			//--
			FreePatchs();

			int index=0;
			for(int y=0;y<(int)m_pHMap->Height();y+=EPatchSize)
			{
				for(int x=0;x<(int)m_pHMap->Width();x+=EPatchSize)
				{
					RECT rect;
					rect.left=x;
					rect.right=min(x+EPatchSize+1,(int)m_pHMap->Width());
					rect.top=y;
					rect.bottom=min(y+EPatchSize+1,(int)m_pHMap->Height());

					TEditorPatchProxy *pPatch=new TEditorPatchProxy;
					pPatch->Create(rect,index);
					m_patchArray.push_back(pPatch);

					index++;
				}
			}//endof for(y)
		}

		RECT CalDecoViewRect(CameraBase *pCamera)
		{
			int halfSize=OP_Deco_Draw_Size/2;
			Ray camRay;
			camRay.origin=pCamera->GetLookat();
			camRay.dir=camRay.origin-pCamera->GetPos();
			D3DXVec3Normalize(&camRay.dir, &camRay.dir);
			Vector3 lookAt;
			m_pHMap->RayCollision(camRay, lookAt);
			POINT pt=m_pHMap->World2Tile(lookAt);
			
			RECT rc;
			rc.left=pt.x-halfSize;
			rc.top=pt.y-halfSize;
			rc.right=pt.x+halfSize;
			rc.bottom=pt.y+halfSize;

			if(rc.left<0)
			{
				rc.right-=rc.left;
				rc.left=0;
			}
			if(rc.top<0)
			{
				rc.bottom-=rc.top;
				rc.top=0;
			}
			if(rc.right>(int)m_pHMap->Width())
			{
				int offset=rc.right-m_pHMap->Width();
				rc.right-=offset;
				rc.left-=offset;
				if(rc.left<0)
					rc.left=0;
			}
			if(rc.bottom>(int)m_pHMap->Height())
			{
				int offset=rc.bottom-m_pHMap->Height();
				rc.bottom-=offset;
				rc.top-=offset;
				if(rc.top<0)
					rc.top=0;
			}

			return rc;
		}

		void UpdateDecoView(int slot,bool bForceRebuild)
		{
			if(m_decosView[slot]==NULL)
				return;

			RECT rc;
			rc.left=rc.top=0;
			rc.right=rc.bottom=80;
			m_decosView[slot]->Build(&m_decoDensityMap[slot],m_pHMap,m_decosFile[slot],m_decos[slot],&rc);
		}

		void UpdateDecoViewMesh(int slot,CameraBase *pCamera,int style)
		{
			//--style -1--delete 0--draw 1--add
			if(m_decosView[slot]==NULL)
				return;

			RECT rc=CalDecoViewRect(pCamera);
			switch(style)
			{
			case -1:
				{
					m_decosView[slot]->DelMesh(&m_decoDensityMap[slot],m_pHMap,m_decosFile[slot],
						m_decos[slot],&rc,true);
				}
				break;
			case 0:
				{
					m_decosView[slot]->GetMesh(m_pHMap,m_decosFile[slot],
						m_decos[slot],&rc,true);
				}
				break;
			case 1:
				{
					m_decosView[slot]->BuildMesh(&m_decoDensityMap[slot],m_pHMap,m_decosFile[slot],
						m_decos[slot],&rc,true);
				}
				break;
			}
		}

		int FindDecoLayer(const TCHAR *szLayerName)
		{
			if(szLayerName==NULL
				|| _tcslen(szLayerName)==0)
				return -1;
			int slot=-1;
			for(int i=0;i<MAX_DECO_LAYER;i++)
			{
				if(_tcsicmp(m_decos[i].szName,szLayerName)==0)
				{
					slot=i;
					break;
				}
			}//endof for(i)

			return slot;
		}

		void FillDefaultUVSet(float uScale,float vScale)
		{
			for(int i=0;i<TRN_MAX_UV_SET;i++)
				m_layerUVScale[i]=Vector2(uScale,vScale);
			m_layerUVScale[TRN_MAX_UV_SET-1].x=EPatchSize;
			m_layerUVScale[TRN_MAX_UV_SET-1].y=EPatchSize;
		}

		void FillTexCoordVB(VertStream& vertStream)
		{
			ASSERT(vertStream.GetType()==EVType_T4);
			ASSERT(vertStream.GetNumVert()==EPatchVerts);

			Vector2 uvStep[TRN_MAX_UV_SET];
			float vertSpace=EPatchSize/(EPatchSideVerts-1);//两个顶点之间相隔多少个格子
			int i;
			for(i=0;i<TRN_MAX_UV_SET;i++)
			{
				uvStep[i].x=1/m_layerUVScale[i].x*vertSpace;
				uvStep[i].y=1/m_layerUVScale[i].y*vertSpace;
			}

			Vert_T4* pVert=(Vert_T4*)vertStream.GetRawBuffer();
			for(int z=0; z<EPatchSideVerts; z++)
			{
				for(int x=0; x<EPatchSideVerts; x++)
				{
					for(i=0;i<TRN_MAX_UV_SET;i++)
					{
						pVert->uv[i].x=uvStep[i].x*x;
						pVert->uv[i].y=uvStep[i].y*z;
					}
					pVert++;
				}
			}
		}

		void CreateTexCoordVB()
		{
			SAFE_DELETE(m_pTexCoordVB);

			VertStream vs;
			vs.Create(EVType_T4,EPatchVerts);
			FillTexCoordVB(vs);

			m_pTexCoordVB=Device()->NewVB();
			m_pTexCoordVB->Create(vs.GetBufferSize(),EB_StaticWriteOnly,vs.GetStride());
			m_pTexCoordVB->CopyFromVertStream(&vs);
		}

		void UpdatePatchVB(TerrainEditor* pEditor,const tagTEditBrush* pBrush)
		{
			RECT rc;
			if (pBrush!=NULL)
			{
				rc.left=pBrush->centerX-pBrush->outer;
				rc.right=pBrush->centerX+pBrush->outer;
				rc.top=pBrush->centerZ-pBrush->outer;
				rc.bottom=pBrush->centerZ+pBrush->outer;
			}

			for(size_t i=0;i<m_patchArray.size();i++)
			{
				TEditorPatchProxy *pPatch=m_patchArray[i];
				if (pBrush!=NULL)
					pPatch->UpdateVB(pEditor,m_pHMap,&rc);
				else
					pPatch->UpdateVB(pEditor,m_pHMap,NULL);
			}
		}
	};

	TerrainEditor::TerrainEditor(void)
	{
		m_p=new Member;
	}

	TerrainEditor::~TerrainEditor(void)
	{
		delete m_p;
	}

	void TerrainEditor::SetGroundRole(bool bVal)
	{
		m_p->m_bDisRole = bVal;
	}

	void TerrainEditor::DrawGroundRole(float &x, float &y, float& z)
	{
		if(m_p->m_pHMap==NULL)
			return;
		if(m_p->m_bDisRole)
		{
			Vector3 point(x,0,z);
			POINT tilePt;
			tilePt=m_p->m_pHMap->World2Tile(point);
			float groundH=m_p->m_pHMap->GetSafeHeight(tilePt.x,tilePt.y);
			Vector3 size(30,175,60);
			Vector3 halfSize=size*0.5f;
			AABBox box;
			box.max=point+halfSize;
			box.min=point-halfSize;
			box.min.y=groundH;
			box.max.y=groundH+size.y;
			DrawX::DrawAABBox(box,0xFFFF0000);
		}
	}

	void TerrainEditor::SetName(const TCHAR* szName)
	{
		ASSERT(szName!=NULL);
		m_p->m_szName=szName;
		//--
		TCHAR szSavePath[512]={0};
		_stprintf_s(szSavePath,512,_T("%s\\%s"),TerrainEditorBasePath,szName);
		::CreateDirectory(TerrainEditorBasePath,NULL);
		::CreateDirectory(szSavePath,NULL);
		TEditorPatchProxy::SetCachePath(szSavePath);

		m_p->m_riverEditor.SetName(szName);
	}

	const TCHAR* TerrainEditor::GetName()
	{
		return m_p->m_szName.c_str();
	}

	void TerrainEditor::SetDrawOption(bool bWireFrame,bool showDecoLayer)
	{
		m_p->m_bDrawWireFrame=bWireFrame;
	}

	void TerrainEditor::GetDrawOption(bool& bWireFrame,bool& showDecoLayer)
	{
		bWireFrame=m_p->m_bDrawWireFrame;
		showDecoLayer=true;
	}

	void TerrainEditor::DrawPatchEdge()
	{
		if(m_p->m_pHMap==NULL
			|| !m_p->m_pHMap->IsCreated())//not created
			return;
	
		for(size_t i=0;i<m_p->m_patchArray.size();i++)
		{
			TEditorPatchProxy *pPatch=m_p->m_patchArray[i];
			pPatch->DrawPatchEdge(this);
		}
	}

	void TerrainEditor::DrawSubPatchEdge()
	{
		if(m_p->m_pHMap==NULL
			|| !m_p->m_pHMap->IsCreated())//not created
			return;

		for(size_t i=0;i<m_p->m_patchArray.size();i++)
		{
			TEditorPatchProxy *pPatch=m_p->m_patchArray[i];
			pPatch->DrawSubPatchEdge(this);
		}
	}

	void TerrainEditor::DrawTerrain( SceneGraph *pSG )
	{
		IDevice *pDev=Device();
		CameraBase *pCamera=pSG->GetCamera();

		Device()->SetVB(1,m_p->m_pTexCoordVB);

		m_p->UpdateAndDrawPatchs(pCamera,this,pSG);
	}

	void TerrainEditor::DrawRiver( SceneGraph *pSG )
	{
		m_p->m_riverEditor.Draw(pSG);
	}

	void TerrainEditor::DrawDeco( SceneGraph *pSG )
	{
		CameraBase *pCamera=pSG->GetCamera();

		bool needUpdate=false;
		if(Vec3Dist(m_p->m_lastDecoUpdatePos,pCamera->GetLookat()) > 50*6)
		{
			m_p->m_lastDecoUpdatePos=pCamera->GetLookat();
			needUpdate=true;
		}

		for(int i=0;i<MAX_DECO_LAYER;i++)
		{
			if(m_p->m_decosView[i])
			{
				if(needUpdate)
					m_p->UpdateDecoViewMesh(i,pCamera,0);
				Kernel::Inst()->GetRenderSys()->RenderSceneNode(pCamera,m_p->m_decosView[i],NULL,false,0);
			}
		}
	}

	void TerrainEditor::Draw(SceneGraph *pSG)
	{
		if(m_p->m_pHMap==NULL
			|| !m_p->m_pHMap->IsCreated())//not created
			return;

		IDevice *pDev=Device();
		CameraBase *pCamera=pSG->GetCamera();

		//--setup Matrix
		pDev->MatPush(EMS_Prj);
		pDev->MatPush(EMS_View);
		pDev->MatPush(EMS_World);
		pDev->MatLoad(EMS_Prj,	pCamera->GetPrj());
		pDev->MatLoad(EMS_View,	pCamera->GetView());
		pDev->MatLoad(EMS_World,&MathConst::IdentityMat);
		pDev->FFlushMatrix(true,true,true,false);

		StateBlock sb;
		sb.SaveState(ERS_FillMode);
		sb.SaveState(ERS_FLightEnable);
		if(m_p->m_bDrawWireFrame)
		{
			sb.SaveState(ERS_AntialiasedLine);
			pDev->SetRenderState(ERS_AntialiasedLine,0);
			pDev->SetFillMode(EFill_Wireframe);
		}
		else
		{
			pDev->SetFillMode(EFill_Solid);
		}

		DrawTerrain(pSG);
		DrawRiver(pSG);
		DrawDeco(pSG);

		pDev->MatPop(EMS_World);
		pDev->MatPop(EMS_View);
		pDev->MatPop(EMS_Prj);
		pDev->FFlushMatrix(true,true,true,false);

		sb.Apply();
	}
	
	void TerrainEditor::DrawGroundCircle(float centreX,float centreZ,UINT r)
	{
		if(m_p->m_pHMap==NULL)
			return;
		DWORD color=0xFFFFFF00;
		float radius=r*m_p->m_pHMap->GetXScale();
		vector<Vert_PD> vertBuf;
	
		Vert_PD v;
		v.diffuse=color;
		POINT tilePt;

		float offset=8.0f;
		Vert_PD startPt;
		startPt.diffuse=color;
		startPt.pos.x=centreX+radius;
		startPt.pos.z=centreZ;
		tilePt=m_p->m_pHMap->World2Tile(startPt.pos);
		startPt.pos.y=m_p->m_pHMap->GetSafeHeight(tilePt.x,tilePt.y)+offset;
		vertBuf.push_back(startPt);

		int numSplit=max(36,r*r);
		for(int i=1;i<numSplit;i++)
		{
			float a=(FLOAT_PI*2/numSplit)*i;
			v.pos.x=centreX+cosf(a)*radius;
			v.pos.z=centreZ+sinf(a)*radius;
			tilePt=m_p->m_pHMap->World2Tile(v.pos);
			v.pos.y=m_p->m_pHMap->GetSafeHeight(tilePt.x,tilePt.y)+offset;
			vertBuf.push_back(v);
		}
		vertBuf.push_back(startPt);

		DrawLineStrip(vertBuf);
	}

	void TerrainEditor::DrawGroundLine(int sx,int sy,int length,float angle,DWORD color)
	{
		if(m_p->m_pHMap==NULL)
			return;

		if(sx<0)	sx=0;		// 当线段的起始点小于零时取0
		if(sy<0)	sy=0;
		//float lineLen = length * m_p->m_pHMap->GetXScale();	//线段的真正长度
		
		vector<Vert_PD> vertBuf;

		Vert_PD v;
		POINT tilePt;
		v.diffuse=color;	// 取颜色
		
		//--Line
		float offset=8.0f;		
		for(UINT i=1;i<(UINT)length;i++)
		{			
			v.pos.x = (sx + cosf(_DegToRad(angle))*i)*m_p->m_pHMap->GetXScale();
			v.pos.z = (sy + sinf(_DegToRad(angle))*i)*m_p->m_pHMap->GetXScale();
			tilePt=m_p->m_pHMap->World2Tile(v.pos);
			v.pos.y = m_p->m_pHMap->GetSafeHeight(tilePt.x,tilePt.y) + offset;
			vertBuf.push_back(v);
		}

		DrawLineStrip(vertBuf);
	}

	void TerrainEditor::DrawGroundLine(int sx,int sy,int dx,int dy,DWORD color)
	{
		if(m_p->m_pHMap==NULL)
			return;

		if(sx<0)	sx=0;		// 当线段的起始点小于零时取0
		if(sy<0)	sy=0;
		if(dx<0)	dx=0;		// 当线段的终止点小于零时取0
		if(dy<0)	dy=0;
		if(sx>(int)m_p->m_pHMap->Width()-1)
			sx=m_p->m_pHMap->Width()-1;
		if(sy>(int)m_p->m_pHMap->Height()-1)
			sy=m_p->m_pHMap->Height()-1;
		if(dx>(int)m_p->m_pHMap->Width()-1)
			dx=m_p->m_pHMap->Width()-1;
		if(dy>(int)m_p->m_pHMap->Height()-1)
			dy=m_p->m_pHMap->Height()-1;

		float length = sqrt((float)((float)(dx-sx)*(dx-sx) + (float)(dy-sy)*(dy-sy)));	//线段的长度
		float angle;											// 线段的斜率
		if(dx==sx)
		{
			if (dy > sy)
			{
				angle = _DegToRad(90.0f);
			}
			else			
				angle = _DegToRad(270.0f);
		}
		else if(dx > sx)
			angle =	atan(((float)(dy-sy))/((float)(dx-sx)));
		else
			angle =	3.1415926f + atan(((float)(dy-sy))/((float)(dx-sx)));

		
		vector<Vert_PD> vertBuf;

		Vert_PD v;
		POINT tilePt;
		v.diffuse=color;	// 取颜色

		//--Line
		float offset=8.0f;

		v.pos.x = sx*m_p->m_pHMap->GetXScale();			// 第一个点
		v.pos.z = sy*m_p->m_pHMap->GetZScale();
		tilePt=m_p->m_pHMap->World2Tile(v.pos);
		v.pos.y = m_p->m_pHMap->GetSafeHeight(tilePt.x,tilePt.y) + offset;
		vertBuf.push_back(v);

		for(UINT i=1;i<= length;i++)			// <=直到最后一点
		{			
			v.pos.x = (sx + cosf(/*_DegToRad*/(angle))*i)*m_p->m_pHMap->GetXScale();
			v.pos.z = (sy + sinf(/*_DegToRad*/(angle))*i)*m_p->m_pHMap->GetXScale();
			
			tilePt=m_p->m_pHMap->World2Tile(v.pos);
			v.pos.y = m_p->m_pHMap->GetSafeHeight(tilePt.x,tilePt.y) + offset;
			vertBuf.push_back(v);
		}

		DrawLineStrip(vertBuf);
	}

	void TerrainEditor::DrawLineStrip(vector<Vert_PD>& vertBuf)
	{
		IDevice *pDev=Device();
		StateBlock sb;
		sb.SaveState(ERS_FLightEnable);
		pDev->FEnableLight(false);

		Kernel::Inst()->GetRenderSys()->PrepMtl(&m_p->m_wireColor);//设置材质状态为使用顶点颜色
		MtlRenderCache *pMtlCache=m_p->m_wireColor.GetRenderCache();
		pMtlCache->Begin();
		pMtlCache->BeginPass(0);
		pDev->SetStreamDefine(Kernel::Inst()->GetRenderSys()->GetStreamDefine(EVType_PD));
		pDev->DrawMem(EDraw_LineStrip,vertBuf.size()-1,&(*vertBuf.begin()),sizeof(Vert_PD));
		pMtlCache->EndPass();
		pMtlCache->End();
		sb.Apply();
	}

	void TerrainEditor::DrawGroundQuad(int left,int right,int top,int bottom,DWORD color)
	{
		if(m_p->m_pHMap==NULL)
			return;

		if(left<0)
			left=0;
		if(top<0)
			top=0;
		if(right>(int)m_p->m_pHMap->Width()-1)
			right=m_p->m_pHMap->Width()-1;
		if(bottom>(int)m_p->m_pHMap->Height()-1)
			bottom=m_p->m_pHMap->Height()-1;

		vector<Vert_PD> vertBuf;

		Vert_PD v;
		v.diffuse=color;

		UINT x=left;
		UINT y;
		//--left
		float offset=8.0f;
		for(y=top;y<(UINT)bottom;y++)
		{
			v.pos=m_p->m_pHMap->Tile2World(x,y);
			v.pos.y+=offset;
			vertBuf.push_back(v);
		}

		//--bottom
		y=bottom;
		for(x=left;x<(UINT)right;x++)
		{
			v.pos=m_p->m_pHMap->Tile2World(x,y);
			v.pos.y+=offset;
			vertBuf.push_back(v);
		}
		//--right
		x=right;
		for(y=bottom;y>(UINT)top;y--)
		{
			v.pos=m_p->m_pHMap->Tile2World(x,y);
			v.pos.y+=offset;
			vertBuf.push_back(v);
		}
		
		//--top
		y=top;
		for(x=right;x>(UINT)left;x--)
		{
			v.pos=m_p->m_pHMap->Tile2World(x,y);
			v.pos.y+=offset;
			vertBuf.push_back(v);
		}

		DrawLineStrip(vertBuf);
	}

	bool TerrainEditor::ImportHeightMap(const TCHAR* szFileName,float xScale,float zScale,float yScale,Vector2 layerUVScale)
	{
		m_p->m_brushRecorder.Clear();
		SAFE_DELETE(m_p->m_pHMap);

		//--
		m_p->m_pHMap=new Heightmap;
		m_p->m_pHMap->ImportImage(szFileName,xScale,zScale,yScale);
		m_p->FillDefaultUVSet(layerUVScale.x,layerUVScale.y);
		m_p->CreateTexCoordVB();

		//--create render data
		m_p->CreatePatchs();
		return true;
	}

	bool TerrainEditor::ExportHeightMap(const TCHAR* szFileName)
	{
		if(m_p->m_pHMap == NULL)
			return false;

		return m_p->m_pHMap->ExportImage(szFileName);
	}

	void TerrainEditor::Create(BYTE initVal,UINT width,UINT height,float xScale,float zScale,float yScale,Vector2 layerUVScale)
	{
		m_p->FreeAll();
		m_p->m_pHMap=new Heightmap;

		m_p->m_pHMap->Create(initVal,width,height,xScale,zScale,yScale,Heightmap::EF_16Bit);
		m_p->FillDefaultUVSet(layerUVScale.x,layerUVScale.y);
		m_p->CreateTexCoordVB();

		//--create render data
		m_p->CreatePatchs();
	}

	void TerrainEditor::CreateByNoise(float freq,UINT width,UINT height,float xScale,float zScale,float yScale,Vector2 layerUVScale)
	{
		m_p->FreeAll();
		m_p->m_pHMap=new Heightmap;
		m_p->m_pHMap->CreateByNoise(freq,width,height,xScale,zScale,yScale,Heightmap::EF_16Bit);
		m_p->FillDefaultUVSet(layerUVScale.x,layerUVScale.y);
		m_p->CreateTexCoordVB();
		
		//--create render data
		m_p->CreatePatchs();
	}

	void TerrainEditor::ModAtt(float xScale,float zScale,float yScale)
	{
		if(m_p->m_pHMap!=NULL)
		{
			m_p->m_pHMap->SetXScale(xScale);
			m_p->m_pHMap->SetYScale(yScale);
			m_p->m_pHMap->SetZScale(zScale);
		}
	}

	void TerrainEditor::ExportHeightMap(Image *pImg)
	{}

	void TerrainEditor::ClearLayers()
	{
		int i;
		for(i=0;i<MAX_MAP_LAYER;i++)
		{
			m_p->m_layers[i].Zero();
		}

		for(size_t j=0;j<m_p->m_patchArray.size();j++)
		{
			TEditorPatchProxy *pPatch=m_p->m_patchArray[j];
			pPatch->ClearLayers();
		}
	}

	bool TerrainEditor::RemoveLayer(const TCHAR* szLayerName)
	{
		bool find=false;
		int i;
		for(i=0;i<MAX_MAP_LAYER;i++)
		{
			if(_tcsicmp(m_p->m_layers[i].szName,szLayerName)==0)
			{
				find=true;
				break;
			}
		}//end of for

		if(i==0)
		{
			Kernel::Inst()->SetLastError(_T("Terrain editor: 基础图层(layer0)不可以删除."));
			return false;
		}

		//--删除
		m_p->m_layers[i].Zero();
		if(find)
		{
			for(size_t j=0;j<m_p->m_patchArray.size();j++)
			{
				TEditorPatchProxy *pPatch=m_p->m_patchArray[j];
				pPatch->RemoveLayer(szLayerName);
			}
		}//endof if

		return find;
	}

	bool TerrainEditor::AddLayer(const TCHAR* szLayerName,const TCHAR* szTileFileName,BYTE uvSetIndex,BYTE surfaceType)
	{
		ASSERT(szLayerName!=NULL);
		ASSERT(szTileFileName!=NULL);
		TCHAR szDriver[16];
		_tsplitpath_s(szTileFileName,szDriver,16,NULL,0,NULL,0,NULL,0);
		if(_tcslen(szDriver)>0)
		{
			Kernel::Inst()->SetLastError(_T("TerrainEditor::Layer Tile贴图需要相对路径,%s"),szTileFileName);
			return false;
		}

		if(uvSetIndex>=TRN_MAX_UV_SET
			|| m_p->m_layerUVScale[uvSetIndex].x==0.0f
			|| m_p->m_layerUVScale[uvSetIndex].y==0.0f)
		{
			Kernel::Inst()->SetLastError(_T("TerrainEditor::Layer bad uv set"));
			return false;
		}

		int i;
		//--检查是否有重名字的
		for(i=0;i<MAX_MAP_LAYER;i++)
		{
			if(m_p->m_layers[i].szName[0]==0)
				continue;
			if(_tcsicmp(m_p->m_layers[i].szName,szLayerName)==0)
			{
				Kernel::Inst()->SetLastError(_T("Terrain editor: layer [%s]已存在."),szLayerName);
				return false;
			}
		}//end of for

		//--
		tagTMapLayer *pNewLayer=NULL;
		//--find a free layer slot
		for(i=0;i<MAX_MAP_LAYER;i++)
		{
			if(m_p->m_layers[i].szName[0]==0
				&& m_p->m_layers[i].szTexFile[0]==0)
			{
				pNewLayer=&m_p->m_layers[i];
				break;
			}
		}
		if(pNewLayer==NULL)
		{
			Kernel::Inst()->SetLastError(_T("Terrain editor: layer个数已经达到最大."));
			return false;
		}
		//--copy data
		_tcscpy_s(pNewLayer->szName,sizeof(pNewLayer->szName)/sizeof(TCHAR),szLayerName);
		_tcscpy_s(pNewLayer->szTexFile,sizeof(pNewLayer->szTexFile)/sizeof(TCHAR),szTileFileName);
		pNewLayer->uvSetIndex=uvSetIndex;
		pNewLayer->surfaceType=surfaceType;

		//--update patch
		int layerIndex=i;
		for(size_t j=0;j<m_p->m_patchArray.size();j++)
		{
			TEditorPatchProxy *pPatch=m_p->m_patchArray[j];
			pPatch->AddLayerMtl(this,layerIndex,layerIndex==0?255:0);
		}

		return true;
	}

	const tagTMapLayer *TerrainEditor::GetMapLayer(UINT index) const
	{
		if(index >= MAX_MAP_LAYER)
			return NULL;
		else
			return &m_p->m_layers[index];
	}
	
	tagTMapLayer* TerrainEditor::FindMapLayer(const TCHAR* szLayerName)
	{
		for(int i=0;i<MAX_MAP_LAYER;i++)
		{
			if(_tcsicmp(m_p->m_layers[i].szName,szLayerName)==0)
				return &m_p->m_layers[i];
		}//end of for
		return NULL;
	}

	void TerrainEditor::MoveLayer(const TCHAR* szLayerName,int offset)
	{
		int find=-1;
		for(int i=0;i<MAX_MAP_LAYER;i++)
		{
			if(_tcsicmp(m_p->m_layers[i].szName,szLayerName)==0)
			{
				find=i;
				break;
			}
		}//end of for
		if(find==-1)//没有找到
			return;
		if(find==0)//base不能移动
			return;
		//--交换一下存储
		int other=find+offset;
		if(other<=0)//等于0也不行,因为base layer不允许移动
			return;
		if(other>=TerrainEditor::MAX_MAP_LAYER)
			return;

		tagTMapLayerExt tmp=m_p->m_layers[find];
		m_p->m_layers[find]=m_p->m_layers[other];
		m_p->m_layers[other]=tmp;

		//--
		for(size_t i=0;i<m_p->m_patchArray.size();i++)
		{
			TEditorPatchProxy *pPatch=m_p->m_patchArray[i];
			pPatch->MoveLayer(szLayerName,offset);
		}
	}
	
	void TerrainEditor::ApplyBrush2Layer(const tagTEditBrush& brush,const TCHAR* szLayerName)
	{
		if(szLayerName==NULL || szLayerName[0]==0)
			return;
		for(size_t i=0;i<m_p->m_patchArray.size();i++)
		{
			TEditorPatchProxy *pPatch=m_p->m_patchArray[i];
			pPatch->BrushLayerMtl(szLayerName,brush);
		}
	}

	void TerrainEditor::SmoothLayer(const tagTEditBrush& brush,const TCHAR* szLayerName)
	{
		if(szLayerName==NULL || szLayerName[0]==0)
			return;
		for(size_t i=0;i<m_p->m_patchArray.size();i++)
		{
			TEditorPatchProxy *pPatch=m_p->m_patchArray[i];
			pPatch->SmoothLayerMtl(szLayerName,brush);
		}
	}
	
	void TerrainEditor::NoiseLayer(const tagTEditBrush& brush,float noiseFreq,const TCHAR* szLayerName)
	{
		if(szLayerName==NULL || szLayerName[0]==0)
			return;
		for(size_t i=0;i<m_p->m_patchArray.size();i++)
		{
			TEditorPatchProxy *pPatch=m_p->m_patchArray[i];
			pPatch->NoiseLayerMtl(szLayerName,brush,noiseFreq);
		}
	}

	void TerrainEditor::ApplyBrush2HMap(const tagTEditBrush& brush)
	{
		BOP_AddBrush burshOp(&m_p->m_brushRecorder);
		DoBrush(m_p->m_pHMap,brush,burshOp);
		m_p->UpdatePatchVB(this,&brush);
	}

	void TerrainEditor::FlattenHMap(const tagTEditBrush& brush)
	{
		BOP_CalAverage avg;
		DoBrush(m_p->m_pHMap,brush,avg);
		if(avg.GetCount() > 0)
		{
			BOP_ApplyVal applyVal(avg.GetResult(),&m_p->m_brushRecorder);
			DoBrush(m_p->m_pHMap,brush,applyVal);
		}
		m_p->UpdatePatchVB(this,&brush);
	}

	void TerrainEditor::ApplyValue2HMap(const tagTEditBrush& brush)
	{
		int strength=int((float)brush.strength/m_p->m_pHMap->GetYScale());
		BOP_ApplyVal applyVal(strength,&m_p->m_brushRecorder);
		DoBrush(m_p->m_pHMap,brush,applyVal);
		m_p->UpdatePatchVB(this,&brush);
	}

	void TerrainEditor::SmoothHMap(const tagTEditBrush& brush)
	{
		int filterSize=abs(brush.strength);
		//--todo leili 暂时代码
		if(filterSize>10)
			filterSize=10;
		BOP_SmoothFilter flt(filterSize,&m_p->m_brushRecorder);
		DoBrush(m_p->m_pHMap,brush,flt);
		flt.Apply(m_p->m_pHMap);
		m_p->UpdatePatchVB(this,&brush);
	}
	
	void TerrainEditor::NoiseHMap(const tagTEditBrush& brush,float noiseFreq)
	{
		if(m_p->m_pHMap==NULL)
			return;
		
		POINT tPt={brush.centerX,brush.centerZ};
		tPt.x-=brush.outer;
		tPt.y-=brush.outer;

		BOP_ApplyNoise ns(brush.outer*2,noiseFreq,tPt.x,tPt.y,&m_p->m_brushRecorder);
		DoBrush(m_p->m_pHMap,brush,ns);

		m_p->UpdatePatchVB(this,&brush);
	}


	namespace Detail
	{
		struct tagTEditorFileHeader
		{
			TCHAR	magic[4];
			BYTE	ver;//1表示8位高度图;2表示16位高度图
			Vector2	layerUVScale[TRN_MAX_UV_SET];
			UINT	hmapWidth;
			UINT	hmapHeight;
			DWORD	hmapOffset;
			UINT	numMapLayer;
			DWORD	mapLayerOffset;
			UINT	numDecoLayer;
			DWORD	decoLayerOffset;

			Color4f	diffuse;			//漫反射材质
			Color4f	ambient;			//环境光材质
			Color4f	specular;			//高光材质
			float	power;

			DWORD	reserve[51];
		};
	};
	
	using namespace Detail;
	bool TerrainEditor::LoadInfo()
	{
		::SetCurrentDirectory(Kernel::Inst()->GetWorkPath());
		TCHAR szPath[512];
		_stprintf_s(szPath,512,_T("%s\\%s\\TEditor.te"),TerrainEditorBasePath,m_p->m_szName.c_str());

		IFS *pFS=Kernel::Inst()->DefaultFS();
		DWORD hFile=pFS->Open(szPath);
		if(hFile==NULL)
		{
			Kernel::Inst()->SetLastError(_T("TerrainEditor:地形存盘文件打开失败:%s."),szPath);
			return false;
		}

		//--load header
		tagTEditorFileHeader header;
		pFS->Read(&header,sizeof(header),hFile);
		memcpy(m_p->m_layerUVScale,header.layerUVScale,
			TRN_MAX_UV_SET*sizeof(Vector2));
		m_p->m_layerUVScale[TRN_MAX_UV_SET-1].x=EPatchSize;
		m_p->m_layerUVScale[TRN_MAX_UV_SET-1].y=EPatchSize;
		m_p->m_mtlDiffuse=header.diffuse;
		m_p->m_mtlAmbient=header.ambient;
		m_p->m_mtlSpecular=header.specular;
		m_p->m_mtlPower=header.power;

		//--create height map
		m_p->FreeAll();
		m_p->m_pHMap=new Heightmap;
		pFS->Seek(hFile,header.hmapOffset,FILE_BEGIN);
		m_p->m_pHMap->ReadFromFile(pFS,hFile);
		//--create render data
		m_p->CreatePatchs();

		//--load map layers
		int i;
		tagTMapLayer mapLayer;
		pFS->Seek(hFile,header.mapLayerOffset,FILE_BEGIN);
		for(i=0;i<(int)header.numMapLayer && i<MAX_MAP_LAYER;i++)
		{
			pFS->Read(&mapLayer,sizeof(mapLayer),hFile);
			AddLayer(mapLayer.szName,mapLayer.szTexFile,mapLayer.uvSetIndex,mapLayer.surfaceType);
		}

		//--load deco layers
		if(header.ver!=0)//因为Deco结构体改变了,所以第一版本存的Deco是不能读取的
		{
			for(i=0;i<(int)header.numDecoLayer && i<MAX_DECO_LAYER;i++)
			{
				pFS->Read(&m_p->m_decos[i],sizeof(tagTDecoLayer),hFile);
			}
		}

		//--
		pFS->Close(hFile);

		//--读出装饰层浓度图,然后创建装饰层预览
		for(i=0;i<MAX_DECO_LAYER;i++)
		{
			if(m_p->m_decos[i].szName[0]!=0)
			{
				_stprintf(szPath,
					_T("%s\\%s\\Deco_%s.dmap"),
					TerrainEditorBasePath,
					m_p->m_szName.c_str(),
					m_p->m_decos[i].szName);
				EditorDecoFile& decoFile=m_p->m_decosFile[i];
				decoFile.Load(pFS,szPath);

				//按照地形高度图的大小和格子大小初始化一个浓度图
				m_p->m_decoDensityMap[i].Create(0,m_p->m_pHMap->Width(),m_p->m_pHMap->Height(),
					m_p->m_pHMap->GetXScale(),m_p->m_pHMap->GetZScale(),1.0f);

				PostEditChangeDecoLayer(m_p->m_decos[i].szName,true);
			}
		}

		//--读取河流
		m_p->m_riverEditor.Load();

		//--
		m_p->CreateTexCoordVB();

		return true;
	}
	
	bool TerrainEditor::SaveInfo() const
	{
		if(!IsHeightmapCreated())
		{
			Kernel::Inst()->SetLastError(_T("TerrainEditor:地形还没有创建,存盘没意义."));
			return false;
		}
		if(m_p->m_szName.empty())
		{
			Kernel::Inst()->SetLastError(_T("TerrainEditor:地形的name为空,无法存盘."));
			return false;
		}
		::SetCurrentDirectory(Kernel::Inst()->GetWorkPath());
		TCHAR szPath[512];
		_stprintf_s(szPath,512,_T("%s\\%s\\TEditor.te"),TerrainEditorBasePath,m_p->m_szName.c_str());
		FILE *fp=NULL;
		if(_tfopen_s(&fp,szPath,_T("wb"))!=0)
		{
			Kernel::Inst()->SetLastError(_T("TerrainEditor:地形存盘文件打开失败:%s."),szPath);
			return false;
		}

		//--write header
		tagTEditorFileHeader header;
		FZeroMem(&header,sizeof(header));
		_tcscpy_s((TCHAR*)header.magic,sizeof(header.magic)/sizeof(TCHAR),_T("TE"));
		header.ver=2;
		memcpy(header.layerUVScale,m_p->m_layerUVScale,
			TRN_MAX_UV_SET*sizeof(Vector2));
		header.hmapWidth=m_p->m_pHMap->Width();
		header.hmapHeight=m_p->m_pHMap->Height();
		header.diffuse=m_p->m_mtlDiffuse;
		header.ambient=m_p->m_mtlAmbient;
		header.specular=m_p->m_mtlSpecular;
		header.power=m_p->m_mtlPower;
		fwrite(&header,sizeof(header),1,fp);

		//--write height map
		header.hmapOffset=ftell(fp);
		m_p->m_pHMap->WriteToFile(fp);

		//--write map layers
		int i;
		header.numMapLayer=MAX_MAP_LAYER;
		header.mapLayerOffset=ftell(fp);
		for(i=0;i<MAX_MAP_LAYER;i++)
		{
			fwrite(&m_p->m_layers[i],sizeof(tagTMapLayer),1,fp);
		}

		//--deco layers
		header.numDecoLayer=MAX_DECO_LAYER;
		header.decoLayerOffset=ftell(fp);
		for(i=0;i<MAX_DECO_LAYER;i++)
		{
			fwrite(&m_p->m_decos[i],sizeof(tagTDecoLayer),1,fp);
		}

		//--write back header
		fseek(fp,0,SEEK_SET);
		fwrite(&header,sizeof(header),1,fp);
		fclose(fp);

		//--save patch cache
		int numPatch=(int)m_p->m_patchArray.size();
		for(i=0;i<numPatch;i++)
		{
			TEditorPatchProxy *pPatch=m_p->m_patchArray[i];
			pPatch->SaveCache();
		}

		//--存储装饰层，一个装饰层一个文件
		for(i=0;i<MAX_DECO_LAYER;i++)
		{
			Heightmap& hmap=m_p->m_decoDensityMap[i];//for short
			if(hmap.Width()>0 && hmap.Height()>0)
			{
				EditorDecoFile& decoFile=m_p->m_decosFile[i];
				_stprintf(szPath,
					_T("%s\\%s\\Deco_%s.dmap"),
					TerrainEditorBasePath,
					m_p->m_szName.c_str(),
					m_p->m_decos[i].szName);
				if(!decoFile.Save(szPath))
					return false;
			}
		}

		//存河流
		m_p->m_riverEditor.Save();

		return true;
	}
	
	bool TerrainEditor::BuildTerrain(const TCHAR* szBasePath,IUICallBack* pUI)
	{
		::SetCurrentDirectory(Kernel::Inst()->GetWorkPath());
		TCHAR szDriver[16];
		_tsplitpath_s(szBasePath,szDriver,16,NULL,0,NULL,0,NULL,0);
		if(_tcslen(szDriver)>0)
		{
			Kernel::Inst()->SetLastError(_T("TerrainEditor::BuildTerrain需要相对路径,%s"),szBasePath);
			return false;
		}

		if(!DeleteAllFiles(szBasePath))
			return false;

		//地块
		int i;
		pUI->SetProgress(0,(int)m_p->m_patchArray.size());
		for(i=0;i<(int)m_p->m_patchArray.size();i++)
		{
			TEditorPatchProxy *pPatch=m_p->m_patchArray[i];
			RECT rc;
			pPatch->GetMapRect(rc);
			if((UINT)rc.right<m_p->m_pHMap->Width())
				rc.right--;
			if((UINT)rc.bottom<m_p->m_pHMap->Height())
				rc.bottom--;
			if(!pPatch->BuildRunTimeData(this,m_p->m_pHMap,szBasePath,pUI))
				return false;

			pUI->SetProgress(i+1,m_p->m_patchArray.size());
		}

		//远处地块四叉树
		NullSceneGraph tmpSG;
		for(int y=0;y<(int)m_p->m_pHMap->Height();y+=ESimplePatchSize)
		{
			for(int x=0;x<(int)m_p->m_pHMap->Width();x+=ESimplePatchSize)
			{
				RECT rect;
				rect.left=x;
				rect.right=min(x+ESimplePatchSize+1,(int)m_p->m_pHMap->Width());
				rect.top=y;
				rect.bottom=min(y+ESimplePatchSize+1,(int)m_p->m_pHMap->Height());

				tmpSG.CreateSimpleTerrainPatch(m_p->m_pHMap,rect);
			}
		}
		TCHAR szFileName[512];
		_stprintf_s(szFileName,512,_T("%ssimple_terrain.fsg"),szBasePath);
		bool ok=tmpSG.SaveAsQuadTreeData(szFileName);
		if(!ok)
			return false;

		return true;
	}

	bool TerrainEditor::BuildRiver(const TCHAR* szBasePath,IUICallBack* pUI)
	{
		::SetCurrentDirectory(Kernel::Inst()->GetWorkPath());
		TCHAR szDriver[16];
		_tsplitpath_s(szBasePath,szDriver,16,NULL,0,NULL,0,NULL,0);
		if(_tcslen(szDriver)>0)
		{
			Kernel::Inst()->SetLastError(_T("TerrainEditor::BuildRiver需要相对路径,%s"),szBasePath);
			return false;
		}

		if(!DeleteAllFiles(szBasePath))
			return false;

		m_p->m_riverEditor.Build(szBasePath,m_p->m_pHMap,pUI);

		return true;
	}

	bool TerrainEditor::WriteRiverDataToTerrainPatch(const TCHAR* szTerrainBasePath,IUICallBack* pUI)
	{
		const UINT nRiverNum = m_p->m_riverEditor.GetRiverNum();
		if( 0 == nRiverNum )
			return true;

		// 把所有河流都加载进来
		for( UINT nRiver = 0; nRiver < nRiverNum; nRiver++ )
		{
			const TRiverEditor::tagRiver& river = m_p->m_riverEditor.GetRiverByIndex( nRiver );
			for( size_t nPath = 0; nPath < river.patchs.size(); nPath++ )
			{
				TRiverPatchProxy* pRiverPatchProxy = river.patchs[nPath];
				if( NULL != pRiverPatchProxy )
					pRiverPatchProxy->LoadCache( &m_p->m_riverEditor, river.pHMap );
			}
		}


		pUI->SetProgress( 0, m_p->m_patchArray.size() );

		TCHAR szPath[512];
		for(size_t nTerrainPatch = 0; nTerrainPatch < m_p->m_patchArray.size(); nTerrainPatch++)
		{
			// 从地形块资源文件中读取header
			TEditorPatchProxy *pPatch=m_p->m_patchArray[nTerrainPatch];
			_stprintf(szPath,_T("%sTrn_%03d.patch"),szTerrainBasePath,pPatch->GetID());
			FILE *fp = _tfopen(szPath,_T("rb+"));
			if(NULL==fp)
			{
				pUI->AddMsg( _T("\t打开地块文件%s失败。\r\n"), szPath );
				continue;
			}

			TerrainPatchRes::tagHeader header;
			memset(&header,0,sizeof(TerrainPatchRes::tagHeader));
			size_t readsize=fread(&header,sizeof(header),1,fp);

			// 计算河流块数量
			int nCurRiverPatch = 0;
			int nNumRiverPatch = 0;
			for(UINT i=0;i<nRiverNum;i++)
			{
				const TRiverEditor::tagRiver& river = m_p->m_riverEditor.GetRiverByIndex( i );
				nNumRiverPatch+=river.patchs.size();
			}

			int nHaveWaterNum = 0;
			// 遍历河流
			for( UINT nRiver = 0; nRiver < nRiverNum; nRiver++ )
			{
				const TRiverEditor::tagRiver& river = m_p->m_riverEditor.GetRiverByIndex( nRiver );
				tagRiverMtlDesc* pRiverMtlDesc = m_p->m_riverEditor.GetRiverMtlDescByIndex( nRiver );
				for( size_t nPath = 0; nPath < river.patchs.size(); nPath++ )
				{
					TRiverPatchProxy* pRiverPatchProxy = river.patchs[nPath];
					if( NULL == pRiverPatchProxy || !pRiverPatchProxy->HaveWater( &m_p->m_riverEditor, river.pHMap ) )
					{
						continue;
					}

					const RECT& rcRiverPatch = pRiverPatchProxy->GetRect();
					AABBox boxRiverPatch;
					river.pHMap->BuildAABBox( &boxRiverPatch, &rcRiverPatch );
					AABBox boxTerrainPath = header.box;
					boxTerrainPath.min.y = -FLOAT_MAX;
					boxTerrainPath.max.y = FLOAT_MAX;
					if( boxTerrainPath.Contain( boxRiverPatch.GetCenter() ) )
					{
						// 一个地块与超过1个河流块相交
						if( 0 == nHaveWaterNum )
						{
							header.riverID = river.id;
							header.underWaterFogStart = pRiverMtlDesc->underWaterFogStart;
							header.underWaterFogEnd = pRiverMtlDesc->underWaterFogEnd;
							header.underWaterFogColor = pRiverMtlDesc->underWaterFogColor;
						}
						else
						{
							pUI->AddMsg( _T("\t错误: 河流%d与河流%d在（%.0f,%.0f）重叠!\r\n"), 
								header.riverID, river.id, header.box.min.x/m_p->m_pHMap->GetXScale(), header.box.min.z/m_p->m_pHMap->GetZScale() );
						}
						nHaveWaterNum++;
					}

				} // for river patchs
			} // for rivers

			// 写入地形块资源文件
			fseek(fp,0,SEEK_SET);
			fwrite(&header,sizeof(header),1,fp);
			fclose(fp);

			pUI->SetProgress( nTerrainPatch+1, m_p->m_patchArray.size() );
		} // for terrain patchs


		// 释放河流
		for( UINT nRiver = 0; nRiver < nRiverNum; nRiver++ )
		{
			const TRiverEditor::tagRiver& river = m_p->m_riverEditor.GetRiverByIndex( nRiver );
			for( size_t nPath = 0; nPath < river.patchs.size(); nPath++ )
			{
				TRiverPatchProxy* pRiverPatchProxy = river.patchs[nPath];
				if( NULL != pRiverPatchProxy )
					pRiverPatchProxy->ClearCache();
			}
		}

		return true;
	}

	void TerrainEditor::SetDetailMap(const TCHAR* szPath)
	{
		m_p->m_szDetailMap=szPath;
	}

	const TCHAR* TerrainEditor::GetDetailMap() const
	{
		return m_p->m_szDetailMap.c_str();
	}

	UINT TerrainEditor::GetHMapWidth()
	{
		return m_p->m_pHMap->Width();
	}
	UINT TerrainEditor::GetHMapHeight()
	{
		return m_p->m_pHMap->Height();
	}
	BYTE TerrainEditor::GetHMapValue(UINT hmapX,UINT hmapY)
	{
		return m_p->m_pHMap->GetValue(hmapX,hmapY);
	}
	float TerrainEditor::GetHMapYScale()
	{
		return m_p->m_pHMap->GetYScale();
	}
	float TerrainEditor::GetHMapXScale()
	{
		return m_p->m_pHMap->GetXScale();
	}
	float TerrainEditor::GetHMapZScale()
	{
		return m_p->m_pHMap->GetZScale();
	}
	Vector2 TerrainEditor::GetStartPos()
	{
		return m_p->m_pHMap->GetStartPos();
	}
	bool TerrainEditor::IsHeightmapCreated() const
	{
		if(m_p->m_pHMap==NULL)
			return false;
		return m_p->m_pHMap->IsCreated();
	}
	POINT TerrainEditor::World2Tile(const Vector3& wpt)
	{
		return m_p->m_pHMap->World2Tile(wpt);
	}
	bool TerrainEditor::MousePick(UINT cursorX,UINT cursorY,CameraBase *pCamera,Vector3& out)
	{
		if(m_p->m_pHMap==NULL)
			return false;
		Ray mRay;
		pCamera->GetMouseRay(cursorX,cursorY,mRay);
		return m_p->m_pHMap->RayCollision(mRay,out);
	}
	void TerrainEditor::SetEditFocus(EEditFocus focus)
	{
		m_p->m_editFocus=focus;
	}

	Vector2 TerrainEditor::GetLayerUVScale(BYTE uvSetIndex) const
	{
		ASSERT(uvSetIndex<TRN_MAX_UV_SET);
		return m_p->m_layerUVScale[uvSetIndex];
	}

	void TerrainEditor::SetLayerUVScale(BYTE uvSetIndex,Vector2 uvScale)
	{
		ASSERT(uvSetIndex<TRN_MAX_UV_SET);
		m_p->m_layerUVScale[uvSetIndex]=uvScale;

		m_p->CreateTexCoordVB();
	}

	void TerrainEditor::ResetNoise()
	{
		Detail::BOP_ApplyNoise::m_noise.ReGenerate();
	}

	const Heightmap *TerrainEditor::GetHeightmap() const
	{
		return m_p->m_pHMap;
	}

	Heightmap *TerrainEditor::GetHeightmapVar()
	{
		return m_p->m_pHMap;
	}

	bool TerrainEditor::AddDecoLayer(const TCHAR * szLayerName)
	{
		if(m_p->m_pHMap==NULL)
			return false;

		tagTDecoLayer *pFreeSlot=NULL;
		Heightmap *pMap=NULL;
		int slot;
		for(int i=0;i<MAX_DECO_LAYER;i++)
		{
			if(m_p->m_decos[i].szName[0]==0)
			{
				slot=i;
				pFreeSlot=&m_p->m_decos[i];
				pMap=&m_p->m_decoDensityMap[i];
				break;
			}
		}

		if(pFreeSlot==NULL)
			return false;
		
		//--
		tagTDecoLayer defaultDeco;
		*pFreeSlot=defaultDeco;
		_tcscpy_s(pFreeSlot->szName,sizeof(pFreeSlot->szName)/sizeof(TCHAR),szLayerName);

		//按照地形高度图的大小和格子大小初始化一个浓度图
		pMap->Create(0,m_p->m_pHMap->Width(),m_p->m_pHMap->Height(),
			m_p->m_pHMap->GetXScale(),m_p->m_pHMap->GetZScale(),1.0f);

		return true;
	}

	SGTrnDecoPatch *NewTrnDecoNode(int type)
	{
		switch(type)
		{
		case 0://grass
			return new SGGrassPatch;
		default:
			ASSERT(0 && _T("unknow deco type"));
		}
		return NULL;
	}

	void TerrainEditor::PostEditChangeDecoLayer(const TCHAR * szLayerName,bool bForceRebuild)
	{
		int slot=m_p->FindDecoLayer(szLayerName);
		if(slot==-1)
			return;

		bool buildView=false;
		if(m_p->m_decosView[slot]==NULL)
			buildView=true;
		else
		{
			if(m_p->m_decosView[slot]->GetDecoType()
				!= m_p->m_decos[slot].type)
			{
				SAFE_DELETE(m_p->m_decosView[slot]);
				buildView=true;
			}
		}

		if(buildView)
		{
			m_p->m_decosView[slot]=NewTrnDecoNode(m_p->m_decos[slot].type);
		}
		m_p->UpdateDecoView(slot,bForceRebuild);
	}
	
	bool TerrainEditor::RemoveDecoLayer(const TCHAR * szLayerName)
	{
		for(int i=0;i<MAX_DECO_LAYER;i++)
		{
			if(_tcsicmp(m_p->m_decos[i].szName,szLayerName)==0)
			{
				tagTDecoLayer defaultDeco;
				m_p->m_decos[i]=defaultDeco;
				SAFE_DELETE(m_p->m_decosView[i]);
				m_p->m_decoDensityMap[i].Destroy();
				m_p->m_decosFile[i].Clear();
				return true;
			}
		}
		return false;
	}

	tagTDecoLayer* TerrainEditor::GetDecoLayer(const TCHAR * szLayerName)
	{
		for(int i=0;i<MAX_DECO_LAYER;i++)
		{
			if(_tcsicmp(m_p->m_decos[i].szName,szLayerName)==0)
			{
				return &m_p->m_decos[i];
			}
		}

		return NULL;
	}

	tagTDecoLayer* TerrainEditor::GetDecoLayer(int index)
	{
		if(index>=0 && index<MAX_DECO_LAYER)
			return &m_p->m_decos[index];
		else
			return NULL;
	}

	void TerrainEditor::BrushDecoDensityMap(const TCHAR* szLayerName,tagTEditBrush& brush,CameraBase *pCamera)
	{
		int slot=m_p->FindDecoLayer(szLayerName);
		if(slot==-1)
			return;

		bool add=true;
		if(brush.strength<0)
		{
			brush.strength=-brush.strength;
			add=false;
		}

		BOP_ApplyVal burshOp(brush.strength);
		DoBrush(&m_p->m_decoDensityMap[slot],brush,burshOp);

		if(add)
			m_p->UpdateDecoViewMesh(slot,pCamera,1);
		else
			m_p->UpdateDecoViewMesh(slot,pCamera,-1);
	}

	void TerrainEditor::NoiseDecoDensityMap(const TCHAR* szLayerName,const tagTEditBrush& brush,float noiseFreq,CameraBase *pCamera)
	{
		int slot=m_p->FindDecoLayer(szLayerName);
		if(slot==-1)
			return;

		POINT tPt={brush.centerX,brush.centerZ};
		tPt.x-=brush.outer;
		tPt.y-=brush.outer;

		BOP_ApplyNoise ns(brush.outer*2,noiseFreq,tPt.x,tPt.y);
		DoBrush(&m_p->m_decoDensityMap[slot],brush,ns);

		m_p->UpdateDecoViewMesh(slot,pCamera,1);
	}

	void TerrainEditor::ImportDensityMap(const TCHAR* szLayerName,const TCHAR* szImgPath,CameraBase *pCamera)
	{
		int slot=m_p->FindDecoLayer(szLayerName);

		if(slot==-1)
			return;

		Image testImg;
		bool bImgOk=false;
		if(m_p->m_pHMap!=NULL
			&& testImg.LoadFromFile(szImgPath))
		{
			//大小必须和高度图一致
			if(testImg.Width() == m_p->m_pHMap->Width()
				&& testImg.Height() == m_p->m_pHMap->Height())
				bImgOk=true;
		}
		
		if(!bImgOk)
		{
			Kernel::Inst()->SetLastError(_T("Import Density map failed,file=%s"),szImgPath);
			return;
		}

		m_p->m_decoDensityMap[slot].ImportImage(szImgPath,1,1,1);

		m_p->UpdateDecoViewMesh(slot,pCamera,0);
	}

	Heightmap *TerrainEditor::GetDecoDensityMap(const TCHAR * szLayerName)
	{
		int slot=m_p->FindDecoLayer(szLayerName);

		if(slot==-1)
			return NULL;

		return &m_p->m_decoDensityMap[slot];
	}

	EditorDecoFile * TerrainEditor::GetDecoFile( const TCHAR *szLayerName )
	{
		int slot=m_p->FindDecoLayer(szLayerName);

		if(slot==-1)
			return NULL;

		return &m_p->m_decosFile[slot];
	}

	bool TerrainEditor::GetDecoVBAndIB(const TCHAR * szLayerName,const RECT& rect,
		VertStream * pVertStream, vector<WORD>& indexArray)
	{
		int slot=m_p->FindDecoLayer(szLayerName);

		if(slot==-1)
			return false;

		return m_p->m_decosView[slot]->GetVertStream(
			m_p->m_pHMap,
			m_p->m_decosFile[slot],
			&rect,
			m_p->m_decos[slot],
			pVertStream,
			indexArray);
	}

	void TerrainEditor::RefreshDecoView(CameraBase *pCamera)
	{
		for(int i=0;i<MAX_DECO_LAYER;i++)
		{
			if(m_p->m_decosView[i])
			{
				m_p->UpdateDecoViewMesh(i,pCamera,0);
			}
		}
		m_p->m_lastDecoUpdatePos=pCamera->GetLookat();
	}

	void TerrainEditor::NewRiver(int riverHeight)
	{
		m_p->m_riverEditor.AddRiver(
			riverHeight,m_p->m_pHMap->Width(),m_p->m_pHMap->Height(),
			m_p->m_pHMap->GetXScale(),m_p->m_pHMap->GetZScale(),1.0f);
	}

	void TerrainEditor::DelRiver(UINT riverIndex)
	{
		m_p->m_riverEditor.RemoveRiver(riverIndex);
	}

	int TerrainEditor::GetRiverNum()
	{
		return m_p->m_riverEditor.GetRiverNum();
	}

	bool TerrainEditor::IsRiverOverLayGround(Vector3& vecGround)
	{
		return m_p->m_riverEditor.IsRiverOverLayGround(vecGround);
	}

	TRiverEditor *TerrainEditor::GetRiverEditor()
	{
		return &m_p->m_riverEditor;
	}

	void TerrainEditor::GetLayers(Vector3& pos,vector<int>& layers)
	{
		if(m_p->m_pHMap==NULL
			|| !m_p->m_pHMap->IsCreated())//not created
			return;

		POINT ptMap=World2Tile(pos);

		RECT rect;
		for(size_t i=0;i<m_p->m_patchArray.size();i++)
		{
			TEditorPatchProxy *pPatch=m_p->m_patchArray[i];
			if(pPatch!=NULL)
			{
				pPatch->GetMapRectNoOverlap(rect);
				if(PtInRect(&rect,ptMap))
				{
					pPatch->GetLayers(ptMap,this,m_p->m_pHMap,layers);
					break;
				}
			}
		}
	}

	void TerrainEditor::EndBrushRecord()
	{
		m_p->m_brushRecorder.EndOpRecord();
	}

	void TerrainEditor::UndoBrush()
	{
		m_p->m_brushRecorder.UndoOp(m_p->m_pHMap);
		m_p->UpdatePatchVB(this,NULL);
	}

	void TerrainEditor::RedoBrush()
	{
		m_p->m_brushRecorder.RedoOp(m_p->m_pHMap);
		m_p->UpdatePatchVB(this,NULL);
	}

	bool TerrainEditor::GetUndoBrushEnable()
	{
		return m_p->m_brushRecorder.CanUndo();
	}

	bool TerrainEditor::GetRedoBrushEnable()
	{
		return m_p->m_brushRecorder.CanRedo();
	}

	bool TerrainEditor::ApplyLightMapToCombineImage(const TCHAR* szLightMapPath,const TCHAR* szTrnPath,IUICallBack* pUI)
	{
		if(m_p->m_pHMap==NULL
			|| !m_p->m_pHMap->IsCreated())//not created
			return false;

		pUI->SetProgress(0,(int)m_p->m_patchArray.size());

		for(int i=0;i<(int)m_p->m_patchArray.size();i++)
		{
			TEditorPatchProxy *pPatch=m_p->m_patchArray[i];
			if(!pPatch->ApplyLightMapToCombineImage(szLightMapPath,szTrnPath))
				return false;

			pUI->SetProgress(i+1,(int)m_p->m_patchArray.size());
		}

		return true;
	}

	bool TerrainEditor::ApplyLightMapToDecoLayer( const TCHAR* szLightMapPath,const TCHAR* szTrnPath,IUICallBack* pUI )
	{
		if(m_p->m_pHMap==NULL
			|| !m_p->m_pHMap->IsCreated())//not created
			return false;

		pUI->SetProgress(0,(int)m_p->m_patchArray.size());

		for(int i=0;i<(int)m_p->m_patchArray.size();i++)
		{
			TEditorPatchProxy *pPatch=m_p->m_patchArray[i];
			if(!pPatch->ApplyLightMapToDecoLayer(szLightMapPath,szTrnPath,this))
				return false;

			pUI->SetProgress(i+1,(int)m_p->m_patchArray.size());
		}

		return true;
	}

	void TerrainEditor::ApplyBrush2River( const tagTEditBrush& brush,UINT riverIndex )
	{
		m_p->m_riverEditor.ApplyBrush2River(riverIndex,brush);
	}

	void TerrainEditor::SmoothRiver( const tagTEditBrush& brush,UINT riverIndex )
	{
		m_p->m_riverEditor.SmoothRiver(riverIndex,brush);
	}

	void TerrainEditor::NoiseRiver( const tagTEditBrush& brush,float noiseFreq,UINT riverIndex )
	{
		m_p->m_riverEditor.NoiseRiver(riverIndex,brush,noiseFreq);
	}

	void TerrainEditor::BuildNavHeightmap( Heightmap& nav,IUICallBack* pUI )
	{
		if(m_p->m_pHMap==NULL
			|| !m_p->m_pHMap->IsCreated())//not created
			return;

		pUI->SetProgress(0,(int)m_p->m_patchArray.size());

		m_p->m_pHMap->CopyTo(nav);//复制宽、高、xz缩放 
		nav.SetYScale(1.0f);//y缩放设置为1，以提高精度

		for(int i=0;i<(int)m_p->m_patchArray.size();i++)
		{
			TEditorPatchProxy *pPatch=m_p->m_patchArray[i];
			pPatch->BuildNavHeightmap(m_p->m_pHMap,nav);

			pUI->SetProgress(i+1,(int)m_p->m_patchArray.size());
		}

		return ;
	}

	void TerrainEditor::BuildGroundTypeMap( Heightmap& layerType,IUICallBack* pUI )
	{
		if(m_p->m_pHMap==NULL
			|| !m_p->m_pHMap->IsCreated())
			return;

		ResMgr::Inst()->EnableIOAsync(false);//关闭多线程读盘
		bool bAsync = ResMgr::Inst()->IsIOAsync();

		pUI->SetProgress(0,(int)m_p->m_patchArray.size());
		layerType.Create(0,m_p->m_pHMap->Width(),m_p->m_pHMap->Height(),1,1,1);
		for(UINT i=0;i<m_p->m_patchArray.size();i++)
		{
			TEditorPatchProxy *pPatch=m_p->m_patchArray[i];

			RECT rc;
			pPatch->GetMapRect(rc);

			Heightmap typeMap;
			typeMap.Create(0,rc.right-rc.left,rc.bottom-rc.top,1,1,1);
			if(!pPatch->BuildLayerTypeMap(this,m_p->m_pHMap,&typeMap,pUI))
				return;

			for(UINT x=0;x<typeMap.Width();x++)
			{
				for(UINT y=0;y<typeMap.Height();y++)
				{
					typeMap.SetValue(x,y,m_p->m_layers[typeMap.GetValue(x,y)].surfaceType);
				}
			}

			layerType.SetSubMap(&typeMap,rc);

			pUI->SetProgress(i+1,m_p->m_patchArray.size());
		}

		ResMgr::Inst()->EnableIOAsync(bAsync);//恢复现场
	}

	void TerrainEditor::SetMtlDiffuse( const Color4f& mtlColor )
	{
		m_p->m_mtlDiffuse=mtlColor;

		for(size_t i=0;i<m_p->m_patchArray.size();i++)
		{
			TEditorPatchProxy *pPatch=m_p->m_patchArray[i];
			pPatch->SetMtlDiffuse(mtlColor);
		}
	}

	void TerrainEditor::SetMtlAmbient( const Color4f& mtlColor )
	{
		m_p->m_mtlAmbient=mtlColor;

		for(size_t i=0;i<m_p->m_patchArray.size();i++)
		{
			TEditorPatchProxy *pPatch=m_p->m_patchArray[i];
			pPatch->SetMtlAmbient(mtlColor);
		}
	}

	void TerrainEditor::SetMtlSpecular( const Color4f& mtlColor )
	{
		m_p->m_mtlSpecular=mtlColor;

		for(size_t i=0;i<m_p->m_patchArray.size();i++)
		{
			TEditorPatchProxy *pPatch=m_p->m_patchArray[i];
			pPatch->SetMtlSpecular(mtlColor);
		}
	}

	void TerrainEditor::SetMtlPower( float power )
	{
		m_p->m_mtlPower=power;

		for(size_t i=0;i<m_p->m_patchArray.size();i++)
		{
			TEditorPatchProxy *pPatch=m_p->m_patchArray[i];
			pPatch->SetMtlPower(power);
		}
	}

	const Color4f& TerrainEditor::GetMtlDiffuse()
	{
		return m_p->m_mtlDiffuse;
	}

	const Color4f& TerrainEditor::GetMtlAmbient()
	{
		return m_p->m_mtlAmbient;
	}

	const Color4f& TerrainEditor::GetMtlSpecular()
	{
		return m_p->m_mtlSpecular;
	}

	float TerrainEditor::GetMtlPower()
	{
		return m_p->m_mtlPower;
	}

	bool TerrainEditor::BuildWaterBitmap( BitMap& bitmap,IUICallBack* pUI )
	{
		bool bRet=false;
		ResMgr::Inst()->EnableIOAsync(false);
		bool bAsync = ResMgr::Inst()->IsIOAsync();
		bRet=m_p->m_riverEditor.BuildWaterBitmap(bitmap,m_p->m_pHMap,pUI);
		ResMgr::Inst()->EnableIOAsync(bAsync);
		return bRet;
	}

	bool TerrainEditor::BuildWaterHMap( Heightmap& hmap,IUICallBack* pUI )
	{
		bool bRet=false;
		ResMgr::Inst()->EnableIOAsync(false);
		bool bAsync = ResMgr::Inst()->IsIOAsync();
		bRet=m_p->m_riverEditor.BuildWaterHMap(hmap,m_p->m_pHMap,pUI);
		ResMgr::Inst()->EnableIOAsync(bAsync);
		return bRet;
	}
}//namespace Cool3D
