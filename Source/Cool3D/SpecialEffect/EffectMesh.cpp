#include "StdAfx.h"
#include ".\effectmesh.h"
#include "..\SceneGraph\EntityNode.h"
#include "..\Math\Transform.h"
#include "EffectPathCtrl.h"
#include "SGeffectNode.h"
#include "..\RenderMtl\IMaterial.h"
#include "..\RenderMtl\MGeneral.h"
#include "..\Animation\AnimationCtrl.h"
#include "..\RenderMtl\MMFrameBlend.h"
#include "..\RenderMtl\MMDepthFrame.h"
#include "EffectTarget.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(EffectMesh,EffectBase);

	class EffectMesh::Member
	{
	public:
		EntityNode			*m_pNode;
		bool				m_bAttached;//是否Attach到了SGEffectNode
		tagMeshProp			m_prop;
		vector<tagState>	m_keyStates;
		int					m_curState;
		float				m_stateTime;

		Vector3				curRot;
		Vector3				curPos;
		bool				m_bActive;//现在是否已经激活

		bool				m_bEffectMtlOK;

		Member():curRot(0,0,0),curPos(0,0,0)
		{
			m_pNode=NULL;
			m_curState=0;
			m_stateTime=0;
			m_bAttached=false;
			m_bActive=true;
			m_bEffectMtlOK=false;
		}
		~Member()	
		{
			FreeSceneNode();
		}

		void FreeSceneNode()
		{
			if(m_pNode==NULL)
				return;

			SceneNode *pFather=const_cast<SceneNode*>(m_pNode->GetParent());
			if(pFather!=NULL &&	m_bAttached)
			{
				pFather->DetachChild(m_pNode);
				m_bAttached=false;
			}

			SAFE_DELETE(m_pNode);
			m_bEffectMtlOK=false;
		}

		bool CreateEffectMtl(const tagMtlEffectDesc& desc)
		{
			int numMtl=m_pNode->GetNumMtl();
			if(numMtl <=0)
				return false;
			for(int i=0;i<numMtl;i++)
			{
				IMaterial *pMainMtl=m_pNode->GetMtl(i);
				IMaterial *pMtl=NULL;
				if(pMainMtl->IS_KIND_OF(MGeneral))
				{
					MGeneral *pMG=(MGeneral*)pMainMtl;
					pMtl=pMG->m_pDiffuse;
					if( pMtl->IS_KIND_OF( IRenderMtl ) )
					{
						IRenderMtl* pRenderMtl = (IRenderMtl*)pMtl;
						pRenderMtl->m_bEnableLight = desc.bEnableLight;
					}
					pMG->InvalidateRenderCache();
				}

				if(pMtl==NULL)
					continue;

				switch(desc.blendType)
				{
				case 1://alpha blend
					{
						MMFrameBlend *pBlend=new MMFrameBlend;
						pBlend->m_blendEnable=true;
						pBlend->m_src=EBlend_SrcAlpha;
						pBlend->m_dst=EBlend_InvSrcAlpha;
						pMtl->AttachModifier(pBlend);
					}
					break;
				case 2://additive blend
					{
						MMFrameBlend *pBlend=new MMFrameBlend;
						pBlend->m_blendEnable=true;
						pBlend->m_src=EBlend_SrcAlpha;
						pBlend->m_dst=EBlend_One;
						pMtl->AttachModifier(pBlend);
					}
					break;
				case 3://颜色相乘
					{
						MMFrameBlend *pBlend=new MMFrameBlend;
						pBlend->m_blendEnable=true;
						pBlend->m_src=EBlend_Zero;
						pBlend->m_dst=EBlend_SrcColor;
						pMtl->AttachModifier(pBlend);
					}
					break;
				case 4://屏幕反色
					{
						MMFrameBlend *pBlend=new MMFrameBlend;
						pBlend->m_blendEnable=true;
						pBlend->m_src=EBlend_InvDestColor;//只有Src color为纯白,才是正确的屏幕反色
						pBlend->m_dst=EBlend_Zero;
						pMtl->AttachModifier(pBlend);
					}
					break;
				default://0没有alpha混合
					break;
				}//endof switch

				if(!desc.bTestZ)
				{
					MMDepthFrame *pMDepth=new MMDepthFrame;
					pMDepth->m_depthWriteEnable=false;
					pMDepth->m_depthTestEnable=false;
					pMtl->AttachModifier(pMDepth);
				}
				else
				{
					// 有Alpha混合就不写ZBuffer，避免不正确的混合
					if( desc.blendType >= 1 && desc.blendType <= 4 )
					{
						MMDepthFrame *pMDepth=new MMDepthFrame;
						pMDepth->m_depthWriteEnable=false;
						pMDepth->m_depthTestEnable=true;
						pMtl->AttachModifier(pMDepth);
					}
				}
			}//endof for()

			return true;
		}

		void UpdateEntityNode(const Vector3& scale,float alpha,SGEffectNode *pSGNode)
		{
			if(m_pNode==NULL)
				return;

			//--update matrix
			Transform trans;
			trans.Scale(scale.x,scale.y,scale.z);
			trans.Rotate(curRot.y,curRot.x,curRot.z);
			trans.Translate(curPos);

			//变换到路径的空间
			EffectPathCtrl* pPath=pSGNode->GetEffectPath(m_prop.szPathName);
			if(pPath)
			{
				Transform transPath;
				Vector3 rotPath=pPath->GetCurRot();
				transPath.Rotate(rotPath.y,rotPath.x,rotPath.z);
				Vector3 posPath=pPath->GetCurPos();
				transPath.Translate(posPath);
				trans.CombineTransRight(transPath.GetMatrix());
			}

			//变换到父节点空间
			if(pSGNode!=NULL)
				trans.CombineTransRight(&pSGNode->GetWorldMat());

			m_pNode->SetWorldMat(trans.GetMatrix());

			//--update alpha
			m_pNode->SetTransparent(alpha);
		}
	};

	EffectMesh::EffectMesh(void)
	{
		m_p=new Member;
	}

	EffectMesh::~EffectMesh(void)
	{
		delete m_p;
	}

	
	void EffectMesh::Serialize(FArchive& ar)
	{
		ar.ValueWrite(m_p->m_prop);
		SerialValVector(ar,m_p->m_keyStates);

		EffectBase::Serialize(ar);
	}

	void EffectMesh::Deserialize(FArchive& ar)
	{
		tagMeshProp prop;
		ar.ValueRead(prop);
		vector<tagState> keyStates;
		DeserialValVector(ar,keyStates);

		//build
		BeginBuild(prop);
		for(size_t i=0;i<keyStates.size();i++)
			AddKeyState(keyStates[i]);
		EndBuild();

		EffectBase::Deserialize(ar);
	}
	
	RenderNode* EffectMesh::GetRenderNode(int i)
	{
		return NULL;
	}

	void EffectMesh::Update(SceneGraph* pSG,SGEffectNode *pSGNode,CameraBase *pCamera,float deltaTime,float runTime)
	{
		if(m_p->m_pNode==NULL)
			return;

		//--是否启动
		if(!m_p->m_bActive)
		{
			if(runTime > m_p->m_prop.m_startTime)
				m_p->m_bActive=true;
			else
			{
				if(m_p->m_pNode!=NULL && m_p->m_bAttached)
				{
					pSGNode->DetachChild(m_p->m_pNode);
					m_p->m_bAttached=false;
				}
			}
		}
		if(!m_p->m_bActive)
			return;

		//如果特效已经active,但是还没有Attach
		if(m_p->m_pNode!=NULL )
		{
			if( !m_p->m_bAttached || NULL == m_p->m_pNode->GetParent() )
			{
				pSGNode->AttachChild( m_p->m_pNode );
				m_p->m_bAttached=true;
			}
		}

		m_p->m_stateTime+=deltaTime;

		//--插值出当前的状态
		m_p->m_curState=AdvanceState(m_p->m_curState,m_p->m_keyStates.size(),
			m_p->m_keyStates[m_p->m_curState].lastTime,m_p->m_stateTime,
			m_p->m_prop.m_loop);
		ASSERT(m_p->m_curState>=0 
			&& m_p->m_curState<(int)m_p->m_keyStates.size());
		tagState curState=m_p->m_keyStates[m_p->m_curState];

		if(m_p->m_keyStates.size()>1)//至少有两帧,才进行插值
		{
			int nextState=m_p->m_curState+1;

			bool bLerp=true;
			if(nextState>=(int)m_p->m_keyStates.size())
			{
				if(!m_p->m_prop.m_loop)//已经播放到最后一个状态,而且不是循环播放,则不需插值
					bLerp=false;
				nextState=0;
			}

			if(bLerp)
			{
				tagState& ts=m_p->m_keyStates[nextState];
				float r=m_p->m_stateTime/curState.lastTime;

				curState.alpha	=	LinearLerp(curState.alpha,ts.alpha,r);
				D3DXVec3Lerp(&curState.scale,&curState.scale,&ts.scale,r);
				D3DXVec3Lerp(&curState.rotVel,&curState.rotVel,&ts.rotVel,r);
			}//endof if(bLerp)
		}

		m_p->curRot+=curState.rotVel*deltaTime;
		m_p->curPos+=curState.vel*deltaTime;

		if(m_bClosing)
		{
			EffectBase::UpdateClosingFade(runTime,m_p->m_prop.m_closeTime);
			curState.alpha*=m_closingFade;
		}
		m_p->UpdateEntityNode(curState.scale,curState.alpha,pSGNode);

		if(!m_p->m_bEffectMtlOK)
		{
			m_p->m_bEffectMtlOK =  m_p->m_bEffectMtlOK=m_p->CreateEffectMtl(m_mtlDesc);
		}

		EffectBase::Update(pSG,pSGNode,pCamera,deltaTime,runTime);
	}

	void EffectMesh::ResetPlayState()
	{
		m_p->m_curState=0;
		m_p->m_stateTime=0;

		m_p->curRot=Deg2Rad3f(m_p->m_prop.initRot);
		m_p->curPos=m_p->m_prop.offset;

		m_p->m_bActive=m_p->m_prop.m_bActive;

		//播放动画
		AnimationCtrl *pAniCtrl=m_p->m_pNode->GetAniCtrl();
		if(pAniCtrl)
		{
			vector<tstring> trackNames;
			pAniCtrl->GetTrackSet(trackNames);
			if(!trackNames.empty())
			{
				bool loopMode=true;
				if(!m_p->m_prop.m_loop)
					loopMode=false;
				pAniCtrl->OpenTrack(trackNames[0].c_str(),loopMode);

				pAniCtrl->ApplyTime(1.0f/100.0f);
			}
		}

		EffectBase::ResetPlayState();
	}

	const TCHAR* EffectMesh::GetName() const
	{
		return m_p->m_prop.m_szName;
	}

	void EffectMesh::CreateMtl(const tagMtlEffectDesc& desc)
	{
		m_mtlDesc=desc;
	}

	EffectMesh::tagMeshProp EffectMesh::GetProp() const
	{
		return m_p->m_prop;
	}

	int EffectMesh::GetNumStates() const
	{
		return m_p->m_keyStates.size();
	}

	EffectMesh::tagState EffectMesh::GetState(int i) const
	{
		ASSERT(i>=0 && i<GetNumStates());
		return m_p->m_keyStates[i];
	}
	
	void EffectMesh::BeginBuild(const tagMeshProp& prop)
	{
		m_p->m_prop=prop;
		m_p->m_keyStates.clear();
		m_p->m_curState=0;
	}

	void EffectMesh::AddKeyState(const tagState& state)
	{
		m_p->m_keyStates.push_back(state);
	}

	void EffectMesh::EndBuild()
	{
		//删除老的EntityNode，创建新的
		m_p->FreeSceneNode();
		m_p->m_pNode=new EntityNode;
		m_p->m_pNode->LoadRes(m_p->m_prop.szMeshName);

		//播放动画
		ResetPlayState();
	}

	void EffectMesh::Rename(const TCHAR* szNewName)
	{
		_tcscpy_s(m_p->m_prop.m_szName,sizeof(m_p->m_prop.m_szName)/sizeof(TCHAR),szNewName);
	}

	void EffectMesh::OnDetachFromSGNode(SGEffectNode *pSGNode)
	{
		if(m_p->m_pNode==NULL)
			return;

		const SceneNode *pFather=m_p->m_pNode->GetParent();
		ASSERT(pSGNode == pFather);
		if(pSGNode!=NULL &&	m_p->m_bAttached)
		{
			pSGNode->DetachChild(m_p->m_pNode);
			m_p->m_bAttached=false;
		}
	}

	void EffectMesh::SetGroupID(int id)
	{
		m_p->m_prop.m_groupID=id;
	}

	float EffectMesh::GetPlayTime()
	{
		if(m_p->m_prop.m_loop)
			return -1;

		float time=0;
		for(int i=0;i<(int)m_p->m_keyStates.size();i++)
		{
			time+=m_p->m_keyStates[i].lastTime;
		}
		return time;
	}

	bool EffectMesh::GetBox(AABBox& out) const
	{
		ASSERT(NULL!=m_p->m_pNode);
		if( !m_p->m_pNode->IsLoaded() )
			return false;

		Vector3 vRot = Deg2Rad3f(m_p->m_prop.initRot);
		Vector3 vPos = m_p->m_prop.offset;
		for(int i=0;i<(int)m_p->m_keyStates.size();i++)
		{
			const tagState& state = m_p->m_keyStates[i];
			AABBox box = m_p->m_pNode->GetLocalBox();
			vPos += state.vel * state.lastTime;
			vRot += state.rotVel * state.lastTime;
			Transform trans;
			trans.Translate( vPos );
			trans.Rotate( vRot.y, vRot.x, vRot.z );
			box.Transform( *trans.GetMatrix() );
			out.Merge( box );
		}
		return true;
	}

	int EffectMesh::GetLOD() const
	{
		return m_p->m_prop.m_nLOD;
	}
}//namespace Cool3D