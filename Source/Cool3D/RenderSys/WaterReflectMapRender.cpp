#include "StdAfx.h"
#include ".\WaterReflectMapRender.h"
#include "..\SceneGraph\SceneGraph.h"
#include "..\Terrain\SGRiverPatch.h"
#include "..\Kernel.h"
#include "..\Device\IRenderTexture.h"
#include "..\Math\Transform.h"
#include <list>
using namespace std;

namespace Cool3D
{
	//--class tagReflectMap----------------------------------------------------------
	struct tagReflectMap						//反射图信息
	{
		float				surfaceHeight;		//水面高度
		int					indexRT;			//RT索引
		vector<SceneNode*>	sceneNodes;			//相关河流sceneNode
		float				distToCam;			//与镜头距离
		UINT				riverID;
		Matrix4				mat;

		void CalcDistToCam(CameraBase* pCamera)
		{
			distToCam=FLOAT_MAX;
			for(int i=0; i<(int)sceneNodes.size(); i++)
			{
				AABBox box;
				sceneNodes[i]->GetBox(box);

				float dist=box.DistToPoint(pCamera->GetPos());
				if(dist<distToCam)				
					distToCam=dist;
			}			
		}

	};


	//--class Member-----------------------------------------------------------------
	class WaterReflectMapRender::Member
	{
	public:
		static const int NUM_RT=8;					//程序启动时创建的RT数量
		
		struct tagRenderTex							//RenderTex信息
		{
			IRenderTexture*		pRT;				//RT指针
			int					size;				//尺寸
			bool				bUsed;				//是否被占用
		};

	public:
		tagRenderTex			m_renderTex[NUM_RT];//程序启动时就创建好的RT
		list<tagReflectMap>		m_reflectMaps;		//反射图列表
		SceneGraph*				m_pSG;				//SceneGraph
		EEffectLevel			m_effectLevel;		//效果级别

	public:
		Member()
		{
			m_effectLevel=EL_High;
		}

		void Init()
		{
			for(int i=0;i<NUM_RT;i++)
			{
				int size=0;
				if(i<2)
					size=512;
				else if(i<5)
					size=256;
				else
					size=128;

				m_renderTex[i].pRT=Device()->NewRenderTexture();
				m_renderTex[i].pRT->Create(size,size,EPF_R5G6B5);
				m_renderTex[i].size=size;
				m_renderTex[i].bUsed=false;
			}
		}

		void Destroy()
		{
			for(int i=0;i<NUM_RT;i++)
			{
				delete m_renderTex[i].pRT;
			}
		}

		void BeginAttachNode()
		{
			m_reflectMaps.clear();
			for(int i=0;i<NUM_RT;i++)
			{
				m_renderTex[i].bUsed=false;
			}

		}

		class SortByDist
		{
		public:
			bool operator()(const tagReflectMap& p1,const tagReflectMap& p2)
			{
				return p1.distToCam<p2.distToCam;
			}
		};

		void EndAttachNode()
		{
			//--计算反射图对应的SceneNode与镜头距离
			list<tagReflectMap>::iterator iter;
			for(iter=m_reflectMaps.begin();
				iter!=m_reflectMaps.end();iter++)
			{
				iter->CalcDistToCam(m_pSG->GetCamera());
			}

			//--由近到远排序
			SortByDist sortObj;
			m_reflectMaps.sort(sortObj);

			//--由近到远分配RT
			for(iter=m_reflectMaps.begin();
				iter!=m_reflectMaps.end();)
			{
				int index=AssignRT(iter->distToCam);
				if(index!=-1)
				{
					iter->indexRT=index;
					iter++;
				}
				else
				{
					iter=m_reflectMaps.erase(iter);
				}
			}
		}

		list<tagReflectMap>::iterator FindReflectMap(UINT riverID)
		{
			list<tagReflectMap>::iterator iter=m_reflectMaps.begin();
			for(; iter!=m_reflectMaps.end(); iter++)
			{
				if(iter->riverID==riverID)
					break;
			}
			return iter;
		}

		int AssignRT(float distToCam)
		{
			//--
			int bestSize=0;
			if(m_effectLevel==EL_Low)
			{
				if(distToCam<10000.0f)
					bestSize=256;
				else if(distToCam<30000.0f)
					bestSize=128;
				else
					return -1;
			}
			else
			{
				if(distToCam<5000.0f)
					bestSize=512;
				else if(distToCam<10000.0f)
					bestSize=256;
				else if(distToCam<30000.0f)
					bestSize=128;
				else
					return -1;
			}

			//--
			for(int i=0;i<NUM_RT;i++)
			{
				if(m_renderTex[i].size<=bestSize
					&& m_renderTex[i].bUsed==false)
				{
					m_renderTex[i].bUsed=true;
					return i;
				}
			}
			return -1;
		}

		void AttachNode(SceneNode* pSceneNode)
		{
			ASSERT(pSceneNode->IS_STATIC_CLASS(SGRiverPatch));

			SGRiverPatch *pRiverPatch=(SGRiverPatch*)pSceneNode;
			if(pRiverPatch->GetSurfaceHeight()>=m_pSG->GetCamera()->GetPos().y)//忽略看不到的水面
				return;

			list<tagReflectMap>::iterator iter=FindReflectMap(pRiverPatch->GetRiverID());
			if(iter!=m_reflectMaps.end())
			{
				iter->sceneNodes.push_back(pSceneNode);
			}
			else
			{
				tagReflectMap reflectMap;
				reflectMap.surfaceHeight = pRiverPatch->GetSurfaceHeight();
				reflectMap.sceneNodes.push_back(pSceneNode);
				reflectMap.riverID=pRiverPatch->GetRiverID();
				m_reflectMaps.push_back(reflectMap);
			}
		}

		void Render()
		{
			if(m_reflectMaps.empty())
				return;

			list<tagReflectMap>::iterator iter;
			for(iter=m_reflectMaps.begin();
				iter!=m_reflectMaps.end(); iter++)
			{
				Render(*iter);
			}
		}

		void CreateViewPrjMat(CameraBase *pCamera,Matrix4& matViewPrj)
		{
			/* 
			函数功能：	    将反射贴图像放映幻灯片一样投影到水面，水面下的镜头就是放映机

			实现方法：		修改RWaterMesh顶点uv坐标值为如下值：
							1,顶点world坐标经放映机viewPrj矩阵变换，再除以w
							2,转换成纹理坐标：乘0.5，加0.5


			遇到问题：	    倒影发生扭曲。
							原因是每个像素的w可能不同，导致每个像素的uv值不是线性的，不能
							通过顶点插值得到，必须单独计算。

			解决方法：		1,将顶点uv值设置为放映机view空间坐标值
							2,设置uv变换矩阵为放映机proj矩阵
							3,设置uv变换标志EUVT_Project，实现逐像素自动除w的功能
							4,问题来了，乘0.5，加0.5操作放在哪？
							答案是：放在Proj矩阵里面。
							什么？这也行？最后的值肯定不对(以u的计算为例)：

							(x*0.5+0.5)/w != x/w*0.5+0.5

							好吧，改一下：

							(x*0.5+0.5*w)/w

							根据dx中关于透视投影矩阵定义，这个w就是view空间的z值，所以uv变换矩阵：

							Matrix4 mat=*pCamera->GetPrj();
							mat._11*=0.5f;	//实现x*0.5	
							mat._22*=-0.5f;	//实现y*-0.5
							mat._31+=0.5f;  //实现+0.5*w
							mat._32+=0.5f;	//实现+0.5*w
			*/

			Transform trans;
			Matrix4 viewMat=*pCamera->GetView();
			trans.LoadMatrix(&viewMat);
			Matrix4 projMat=*pCamera->GetPrj();
			projMat._11*=0.5f;
			projMat._22*=-0.5f;
			projMat._31+=0.5f;
			projMat._32+=0.5f;
			trans.CombineTransRight(&projMat);
			matViewPrj=*trans.GetMatrix();
		}

		void Render(tagReflectMap& reflectMap)
		{
			CameraBase* pCam=m_pSG->GetCamera();
			IDevice* pDev=Device();

			//--在水面下对称位置放置一个摄像机
			CameraBase newCamera;
			newCamera.SetPerspective((float)pDev->GetClientWidth(),(float)pDev->GetClientHeight(),
				pCam->GetFOV(),pCam->GetZNear(),pCam->GetZFar());

			Vector3 from=pCam->GetPos();
			Vector3 lookAt=pCam->GetLookat();
			from.y=reflectMap.surfaceHeight-(from.y-reflectMap.surfaceHeight);
			lookAt.y=reflectMap.surfaceHeight-(lookAt.y-reflectMap.surfaceHeight);
			newCamera.SetLookAt(from,lookAt);

			//--渲染反射图
			bool bRenderTrnOnly=(m_effectLevel==EL_Low || m_effectLevel==EL_Mid);
			m_pSG->RenderReflectMap(&newCamera,m_renderTex[reflectMap.indexRT].pRT,reflectMap.surfaceHeight,bRenderTrnOnly);
			//m_renderTex[reflectMap.indexRT].pRT->WriteToFile(_T("d:\\test.tga"));

			CreateViewPrjMat(&newCamera,reflectMap.mat);

			//--将反射图投影到水面
			for(int i=0;i<(int)reflectMap.sceneNodes.size();i++)
			{
				SceneNode* pSceneNode=reflectMap.sceneNodes[i];
				if(pSceneNode->IS_STATIC_CLASS(SGRiverPatch))
				{
					((SGRiverPatch*)pSceneNode)->SetReflectMap(m_renderTex[reflectMap.indexRT].pRT);
				}
			}
		}
	};



	//--class WaterReflectMapRender-----------------------------------------------------
	WaterReflectMapRender::WaterReflectMapRender(void)
	{
		m_p=new Member;
		m_bEnable=false;
	}

	WaterReflectMapRender::~WaterReflectMapRender(void)
	{
		delete m_p;
	}

	void WaterReflectMapRender::Init()
	{
		if(!IsHWSurpport())
			return;

		m_p->Init();
	}

	void WaterReflectMapRender::Destroy()
	{
		if(!IsHWSurpport())
			return;

		m_p->Destroy();
	}

	WaterReflectMapRender* WaterReflectMapRender::Inst()
	{
		static WaterReflectMapRender Inst;
		return &Inst;
	}

	void WaterReflectMapRender::Begin(SceneGraph* pSG)
	{
		if(!m_bEnable)
			return;

		m_p->m_pSG=pSG;
		m_p->BeginAttachNode();
	}

	void WaterReflectMapRender::AttachNode(SceneNode* pSceneNode)
	{
		if(!m_bEnable)
			return;

		if(!pSceneNode->IS_STATIC_CLASS(SGRiverPatch))
			return;

		SGRiverPatch* pPatch=(SGRiverPatch*)pSceneNode;
		if(!pPatch->IsEnableEffect())//不允许反射效果
			return;

		m_p->AttachNode(pSceneNode);
	}

	void WaterReflectMapRender::End()
	{
		if(!m_bEnable)
			return;

		m_p->EndAttachNode();
		m_p->Render();
	}

	bool WaterReflectMapRender::IsHWSurpport()
	{
		IDevice* pDev=Device();
		if( pDev!=NULL )
		{
			return pDev->CapsMaxTextureStages()>=3
				&& pDev->FCapsMaxSimultaneousTextures()>=3
				&& pDev->CapsVertexShaderVersion()>=D3DVS_VERSION(2,0)
				&& pDev->CapsPixelShaderVersion()>=D3DPS_VERSION(2,0);
		}
		return false;
	}

	void WaterReflectMapRender::Enable(bool bEnable)
	{
		if(bEnable && !IsHWSurpport())//硬件不支持时无法开启效果
			return;

		m_bEnable=bEnable;
	}

	void WaterReflectMapRender::SetEffectLevel(EEffectLevel level)
	{
		m_p->m_effectLevel=level;
	}

	WaterReflectMapRender::EEffectLevel WaterReflectMapRender::GetEffectLevel()
	{
		return m_p->m_effectLevel;
	}

	bool WaterReflectMapRender::GetRViewPrjMatrix( UINT riverID,Matrix4& mat )
	{
		for(list<tagReflectMap>::iterator iter=m_p->m_reflectMaps.begin();
			iter!=m_p->m_reflectMaps.end();++iter)
		{
			if(riverID==iter->riverID)
			{
				mat=iter->mat;
				return true;
			}
		}

		return false;
	}
}//end namespace Cool3D