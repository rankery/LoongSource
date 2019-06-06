#include "StdAfx.h"
#include ".\sgbladeflare.h"
#include "..\RenderSys\DynamicMesh.h"
#include "..\RenderMtl\MConstColor.h"
#include "..\RenderMtl\MMFrameBlend.h"
#include "..\RenderMtl\MBitmap.h"
#include "..\Public\TagNodeDefine.h"
#include "EntityNode.h"
#include "..\ResSys\ResStaticMesh.h"
#include "..\Kernel.h"
#include "..\Math\Transform.h"
#include "..\Util\SpeedAnalyser.h"

namespace Cool3D
{
	const float FlareSampleTime=1.0f/20;//采样间隔时间
	struct tagFlareLine
	{
		float	time;//出生的时刻
		Vector3	pt[10];

		tagFlareLine()
		{
			memset(this,0,sizeof(*this));
		}
	};
	IMP_RTTI_DYNC(SGBladeFlare,SceneNode);

	class SGBladeFlare::Member
	{
	public:
		DynamicMesh				*m_pRMesh;
		MBitmap* 				m_pMtl;
		list<tagFlareLine>		m_track;//记录每帧的轨迹
		float					m_trackTimeLen;
		int						m_numSeg;
		vector<Vector3>			m_tagNodePos;
		Color4ub				m_color;

		Member()
		{
			m_pRMesh=NULL;
			m_pMtl=new MBitmap;
			m_pMtl->SetTexture(0,_T("data\\system\\BladeFlare\\tex0.tga"));
			m_pMtl->m_addressU=m_pMtl->m_addressV=m_pMtl->m_addressW=ETAddress_Clamp;
			m_pMtl->m_magFilter=m_pMtl->m_minFilter=m_pMtl->m_mipFilter=ETFilter_LINEAR;
			m_pMtl->m_bTwoSide=true;
			m_pMtl->m_bEnableLight=false;
			MMFrameBlend *pBlend=new MMFrameBlend;
			pBlend->m_blendEnable=true;
			pBlend->m_src=EBlend_SrcAlpha;
			pBlend->m_dst=EBlend_One;
			m_pMtl->AttachModifier(pBlend);
			
			m_color = Color4ub(255,255,255,255);
		
			m_trackTimeLen=2;
			m_numSeg=2;
		}
		~Member()
		{
			SAFE_DELETE(m_pMtl);
			SAFE_DELETE(m_pRMesh);
		}

		void UpdateRes(EntityNode *pFatherNode)
		{
			//已经创建好了则返回
			if(m_pRMesh!=NULL)
				return;

			ResRenderable *pRes=pFatherNode->GetResObj();
			ASSERT(pRes!=NULL && pRes->IS_STATIC_CLASS(ResStaticMesh));
			ResStaticMesh *pStaticMesh=(ResStaticMesh *)pRes;
			if(pStaticMesh==NULL || !pStaticMesh->IsCreated())
				return;

			//拷贝初始点
			m_tagNodePos.clear();
			const TagNodeFrame& tagNodes=pStaticMesh->GetTagNodes();
			int numTagNodes=tagNodes.nodes.size();
			for(int i=0;i<numTagNodes;i++)
			{
				const tagTagNode& tNode=tagNodes.nodes[i];
				DWORD name = tNode.name & 0xFF;
				if( name=='f' || name=='F' )
				{
					m_tagNodePos.push_back(tNode.trans);
				}
			}
			ASSERT(m_tagNodePos.size()<8);
			if(m_tagNodePos.size()>=8)
			{
				Kernel::Inst()->SetLastError(_T("BladeFlare find too many tagNodes,res=%s"),
					pRes->GetName());
				return;
			}

			if(m_tagNodePos.size()<2)
			{
				Kernel::Inst()->SetLastError(_T("BladeFlare find 0 tagNode,res=%s"),
					pRes->GetName());
				return;
			}


			//计算总共会有多少段
			m_numSeg=int(m_trackTimeLen/FlareSampleTime);
			if(m_numSeg>128)
			{
				m_numSeg=128;
				m_trackTimeLen=m_numSeg*FlareSampleTime;
			}

			//生成Mesh
			int numTri=(m_numSeg-1)*(m_tagNodePos.size()-1)*2;
			m_pRMesh=new DynamicMesh;
			m_pRMesh->Create(0,EDraw_TriangleList,EVType_PT,numTri*3);
			m_pRMesh->SetDrawLimit(0);

		}

		void UpdateFlare(EntityNode *pFatherNode)
		{
			float deltaTime=FlareSampleTime;
			if(m_track.size()>0)
			{
				const tagFlareLine& lastLine=(*m_track.rbegin());
				float lastSampleTime=lastLine.time;
				deltaTime=Kernel::Inst()->GetAccumTime()-lastSampleTime;
			}

			//如果采样间隔时间超过阀值,则新增一条采样线
			//if(deltaTime >= FlareSampleTime)
			{
				tagFlareLine newLine;
				newLine.time=Kernel::Inst()->GetAccumTime();
				Transform trans;
				trans.LoadMatrix(&pFatherNode->GetWorldMat());
				for(size_t i=0;i<m_tagNodePos.size();i++)
				{
					ASSERT(i<8);;
					newLine.pt[i]=m_tagNodePos[i];
					trans.TransVec3(newLine.pt[i]);
				}//endof for

				//如果记录满了,则需要删除最老的线
				if(m_track.size()==m_numSeg)
					m_track.pop_front();
				m_track.push_back(newLine);
			}//endof if
		}

		void UpdateMesh()
		{
			//至少有两条线才能形成面
			if(m_track.size()<2)
			{
				m_pRMesh->SetDrawLimit(0);
				return;
			}

			VertStream *pVStream=m_pRMesh->GetVertStream();
			int numVert=pVStream->GetNumVert();
			Vert_PT *pVB=(Vert_PT *)pVStream->GetRawBuffer();
			int vi=0;

			list<tagFlareLine>::iterator iter=m_track.begin();
			list<tagFlareLine>::iterator lastIter=iter;
			iter++;
			int numPt=m_tagNodePos.size();
			float nowTime=Kernel::Inst()->GetAccumTime();
			float vStep=1.0f/(numPt-1);
			float uStep=1.0f/(m_track.size()-1);
			float u=uStep;
			for(;iter!=m_track.end();iter++)
			{
				//--get line data
				const tagFlareLine& lastLine=*lastIter;
				const tagFlareLine& line=*iter;
				lastIter=iter;
				
				for(int i=0;i<numPt-1;i++)
				{
					//--triangle 1
					pVB[vi].pos=lastLine.pt[i];
					pVB[vi].uv.x=u-uStep;
					pVB[vi].uv.y=i*vStep;
					vi++;

					pVB[vi].pos=lastLine.pt[i+1];
					pVB[vi].uv.x=u-uStep;
					pVB[vi].uv.y=(i+1)*vStep;
					vi++;

					pVB[vi].pos=line.pt[i+1];
					pVB[vi].uv.x=u;
					pVB[vi].uv.y=(i+1)*vStep;
					vi++;

					//--tri 2
					pVB[vi].pos=lastLine.pt[i];
					pVB[vi].uv.x=u-uStep;
					pVB[vi].uv.y=i*vStep;
					vi++;

					pVB[vi].pos=line.pt[i];
					pVB[vi].uv.x=u;
					pVB[vi].uv.y=i*vStep;
					vi++;

					pVB[vi].pos=line.pt[i+1];
					pVB[vi].uv.x=u;
					pVB[vi].uv.y=(i+1)*vStep;
					vi++;		

				}//endof for(i)
				ASSERT(vi<=numVert);

				u+=uStep;
			}//endof for
			ASSERT(vi<=numVert);
			m_pRMesh->SetDrawLimit(vi/3);
		}

		void ClearFlare()
		{
			m_track.clear();
		}

	};

	//--
	SGBladeFlare::SGBladeFlare(void)
	{
		m_p=new Member;
		m_bEnable=true;
	}

	SGBladeFlare::~SGBladeFlare(void)
	{
		delete m_p;
	}

	void SGBladeFlare::Serialize(FArchive& ar)
	{
		ASSERT(0 && "no imp");
	}

	void SGBladeFlare::Deserialize(FArchive& ar)
	{
		ASSERT(0 && "no imp");
	}

	bool SGBladeFlare::IsInFrustum(const Frustum *pFrumstum) const
	{
		return true;
	}

	void SGBladeFlare::LoadRes(const TCHAR* szName)
	{
		ASSERT(0 && "SGBladeFlare no res");
	}

	UINT SGBladeFlare::GetNumRenderNode() const
	{
		if(!m_bEnable || m_p->m_pRMesh==NULL)
			return 0;
		else
			return 1;
	}

	RenderNode *SGBladeFlare::GetRenderNode(UINT i) const
	{
		ASSERT(i==0);
		return m_p->m_pRMesh;
	}

	const Matrix4& SGBladeFlare::GetWorldMat() const
	{
		return MathConst::IdentityMat;
	}

	void SGBladeFlare::SetWorldMat(const Matrix4 *pMat)
	{
		ASSERT(0 && "SGBladeFlare no world matrix");
	}

	void SGBladeFlare::GetBox(AABBox& out) const
	{
		//ASSERT(0 && "no imp");
		out=MathConst::SmallBox;
	}

	IMaterial*	SGBladeFlare::GetMtl(int mtlID)
	{
		ASSERT(mtlID==0);
		return m_p->m_pMtl;
	}

	void SGBladeFlare::Enable(bool bEnable)
	{
		m_bEnable = bEnable;
		if(!bEnable)
		{
			m_p->ClearFlare();
		}
	}

	void SGBladeFlare::Update(float deltaTime,SceneGraph *pSG)
	{
		if(!m_bEnable)
			return;

		const SceneNode *pNode=GetParent();
		ASSERT(pNode->IS_KIND_OF(EntityNode));
		EntityNode *pFather=(EntityNode *)pNode;
		if(m_p->m_pRMesh==NULL)
			m_p->UpdateRes(pFather);

		if(m_p->m_pRMesh!=NULL)
		{
			m_p->UpdateFlare(pFather);
			m_p->UpdateMesh();
		}
	}

	void SGBladeFlare::ForceUpdate(float deltaTime,SceneGraph *pSG)
	{
		//have no child,do nothing
	}

	bool SGBladeFlare::Init(float trackTimeLen)
	{
		trackTimeLen=0.5f;

		const SceneNode *pFather=GetParent();
		ASSERT(pFather!=NULL);
		if(pFather==NULL)
			return false;

		if(!pFather->IS_KIND_OF(EntityNode))
			return false;

		EntityNode *pEntityNode=(EntityNode *)pFather;
		ResRenderable *pRes=pEntityNode->GetResObj();
		if(pRes==NULL
			|| !pRes->IS_STATIC_CLASS(ResStaticMesh))
			return false;

		m_p->m_trackTimeLen=trackTimeLen;
		m_bEnable=true;

        return true;
	}

	void SGBladeFlare::SetColor(Color4ub color)
	{
		m_p->m_color = color;
	}

}//namespace Cool3D