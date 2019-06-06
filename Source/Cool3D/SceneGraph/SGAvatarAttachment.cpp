#include "StdAfx.h"
#include ".\sgavatarattachment.h"
#include "..\Util\Exception.h"
#include "AvatarNode.h"
#include "..\ResSys\ResSkeleton.h"
#include "..\ResSys\ResKeyFrame.h"
#include "..\ResSys\ResStaticMesh.h"
#include "..\Animation\AnimationCtrl.h"

namespace Cool3D
{
	class SGAvatarAttachment::Member
	{
	public:
		int			m_tagIndex;//绑定到哪个tag ndoe
		Matrix4		m_finalToWorld;
		tstring		m_szTagName;
		bool        m_bSelOffsetFinded;
		Vector3     m_vSelOffset;
		bool        m_bIsBindMirroredBone;
	public:
		Member() : m_vSelOffset( 0.0f, 0.0f, 0.0f )
		{
			m_tagIndex=-1;
			D3DXMatrixIdentity(&m_finalToWorld);
			m_bSelOffsetFinded=false;
			m_bIsBindMirroredBone=false;
		}
		~Member()	{}

	};

	//------------------------------------------------
	IMP_RTTI_DYNC(SGAvatarAttachment,EntityNode);

	SGAvatarAttachment::SGAvatarAttachment(void)
	{
		m_p=new Member;
	}

	SGAvatarAttachment::~SGAvatarAttachment(void)
	{
		delete m_p;
	}

	void SGAvatarAttachment::Serialize(FArchive& ar)
	{
		ar<<m_p->m_szTagName;

		EntityNode::Serialize(ar);
	}

	void SGAvatarAttachment::Deserialize(FArchive& ar)
	{
		tstring m_szTagNameDesc;
		ar>>m_szTagNameDesc;
		BindToTagNode(m_szTagNameDesc.c_str());
		EntityNode::Deserialize(ar);
	}

	void SGAvatarAttachment::Update(float deltaTime,SceneGraph *pSG)
	{
		if(m_p->m_szTagName.empty())
			return;

		//--find bone index if necessary
		if(m_p->m_tagIndex==-1)
		{
			ASSERT(GetParent()!=NULL);
			ASSERT(GetParent()->IS_KIND_OF(AvatarNode));
			AvatarNode *pParent=(AvatarNode *)GetParent();
			ResSkeleton *pSkel=const_cast<ResSkeleton *>(pParent->GetResource());
			if(pSkel!=NULL
				&& pSkel->IsCreated())
			{
				m_p->m_tagIndex=pSkel->GetBoneID(m_p->m_szTagName.c_str());
				if( -1 != m_p->m_tagIndex )
					m_p->m_bIsBindMirroredBone = pParent->GetSkeletonAniCtrl()->BoneIsMirrored( m_p->m_tagIndex );
			}
		}

		//--
		if(m_p->m_tagIndex!=-1)
		{
			ResRenderable* pRes = GetResObj();
			if( NULL != pRes && pRes->IsCreated() && !m_p->m_bSelOffsetFinded )
			{
				GetSrcTagNodeOffset( &m_p->m_vSelOffset );
				m_p->m_bSelOffsetFinded = true;
			}
			m_p->m_finalToWorld = MathConst::IdentityMat;
			m_p->m_finalToWorld._41 = -m_p->m_vSelOffset.x; 
			m_p->m_finalToWorld._42 = -m_p->m_vSelOffset.y; 
			m_p->m_finalToWorld._43 = -m_p->m_vSelOffset.z;

			ASSERT(GetParent()!=NULL);
			ASSERT(GetParent()->IS_KIND_OF(AvatarNode));
			AvatarNode *pParent=(AvatarNode *)GetParent();
			const Matrix4& boneMat=(*(pParent->GetSkeletonAniCtrl()->GetBoneSpace(m_p->m_tagIndex)));
			
			m_p->m_finalToWorld *= boneMat;
			m_p->m_finalToWorld=m_p->m_finalToWorld*pParent->GetWorldMat();//变换到父节点的空间

			EntityNode::SetWorldMat(&m_p->m_finalToWorld);//update world matrix and aabbox
		}

		EntityNode::Update(deltaTime,pSG);
	}

	void SGAvatarAttachment::BindToTagNode(const TCHAR* szTagName)
	{
		ASSERT(szTagName!=NULL);
		m_p->m_szTagName=szTagName;
		m_p->m_tagIndex = -1;
		m_p->m_bSelOffsetFinded=false;
		m_p->m_vSelOffset=Vector3(0.0f,0.0f,0.0f);
		m_p->m_bIsBindMirroredBone=false;
	}

	const tstring SGAvatarAttachment::GetTagNodeName() const
	{
		return m_p->m_szTagName;
	}

	bool SGAvatarAttachment::IsBindMirroredBone() const
	{
		return m_p->m_bIsBindMirroredBone;
	}

	bool SGAvatarAttachment::GetSrcTagNodeOffset( Vector3* pOut )
	{
		ASSERT( NULL != pOut );
		
		ResRenderable* pRes = GetResObj();
		ASSERT( NULL != pRes && pRes->IsCreated() );

		// tag_XXXX
		tstring strName = m_p->m_szTagName.c_str() + 4;
		const DWORD dwNameFourCC = _MakeFourCC( strName );

		if( pRes->IS_STATIC_CLASS( ResStaticMesh ) )
		{
			ResStaticMesh* pResStaticMesh = (ResStaticMesh*)pRes;
			const TagNodeFrame& NodeFrame = pResStaticMesh->GetTagNodes();
			for( size_t i = 0; i < NodeFrame.nodes.size(); i++ )
			{
				if( dwNameFourCC == NodeFrame.nodes[i].name )
				{
					*pOut = NodeFrame.nodes[i].trans;
					return true;
				}
			}
			return false;
		}
		else if( pRes->IS_STATIC_CLASS( ResKeyFrame ) )
		{
			ResKeyFrame* pResKeyFrame = (ResKeyFrame*)pRes;
			const vector<TagNodeFrame>& NodeFrameArray = pResKeyFrame->GetTagFrameArray();
			if( NodeFrameArray.empty() )
				return false;
			
			// 取最后一帧的挂载点
			const TagNodeFrame& NodeFrame = NodeFrameArray[NodeFrameArray.size()-1];
			for( size_t i = 0; i < NodeFrame.nodes.size(); i++ )
			{
				if( dwNameFourCC == NodeFrame.nodes[i].name )
				{
					*pOut = NodeFrame.nodes[i].trans;
					return true;
				}
			}
			return false;
		}
		else
		{
			return false;
		}
	}

	bool SGAvatarAttachment::GetModelSpace( const TCHAR* szTagName,Matrix4& out )
	{
		ResRenderable* pRes = GetResObj();
		if( NULL == pRes || !pRes->IsCreated() )
			return false;

		// tag_XXXX
		tstring strName = szTagName + 4;
		const DWORD dwNameFourCC = _MakeFourCC( strName );

		if( pRes->IS_STATIC_CLASS( ResStaticMesh ) )
		{
			ResStaticMesh* pResStaticMesh = (ResStaticMesh*)pRes;
			const TagNodeFrame& NodeFrame = pResStaticMesh->GetTagNodes();
			for( size_t i = 0; i < NodeFrame.nodes.size(); i++ )
			{
				if( dwNameFourCC == NodeFrame.nodes[i].name )
				{
					Matrix4 matTrans,matRotate;
					D3DXMatrixRotationQuaternion(&matRotate, &NodeFrame.nodes[i].rotat);
					D3DXMatrixTranslation(&matTrans, NodeFrame.nodes[i].trans.x, NodeFrame.nodes[i].trans.y, NodeFrame.nodes[i].trans.z);
					out=matRotate*matTrans;
					return true;
				}
			}
			return false;
		}
		else if( pRes->IS_STATIC_CLASS( ResKeyFrame ) )
		{
			ResKeyFrame* pResKeyFrame = (ResKeyFrame*)pRes;
			const vector<TagNodeFrame>& NodeFrameArray = pResKeyFrame->GetTagFrameArray();
			if( NodeFrameArray.empty() )
				return false;

			// 取最后一帧的挂载点
			const TagNodeFrame& NodeFrame = NodeFrameArray[NodeFrameArray.size()-1];
			for( size_t i = 0; i < NodeFrame.nodes.size(); i++ )
			{
				if( dwNameFourCC == NodeFrame.nodes[i].name )
				{
					Matrix4 matTrans,matRotate;
					D3DXMatrixRotationQuaternion(&matRotate, &NodeFrame.nodes[i].rotat);
					D3DXMatrixTranslation(&matTrans, NodeFrame.nodes[i].trans.x, NodeFrame.nodes[i].trans.y, NodeFrame.nodes[i].trans.z);
					out=matRotate*matTrans;
					return true;
				}
			}
			return false;
		}
		else
		{
			return false;
		}
	}
	bool SGAvatarAttachment::HaveTagNode(const TCHAR* szTagName) const
	{
		ResRenderable* pRes = GetResObj();
		if( NULL == pRes || !pRes->IsCreated() )
			return false;

		// tag_XXXX
		tstring strName = szTagName + 4;
		const DWORD dwNameFourCC = _MakeFourCC( strName );

		if( pRes->IS_STATIC_CLASS( ResStaticMesh ) )
		{
			ResStaticMesh* pResStaticMesh = (ResStaticMesh*)pRes;
			const TagNodeFrame& NodeFrame = pResStaticMesh->GetTagNodes();
			for( size_t i = 0; i < NodeFrame.nodes.size(); i++ )
			{
				if( dwNameFourCC == NodeFrame.nodes[i].name )
					return true;
			}
			return false;
		}
		else if( pRes->IS_STATIC_CLASS( ResKeyFrame ) )
		{
			ResKeyFrame* pResKeyFrame = (ResKeyFrame*)pRes;
			const vector<TagNodeFrame>& NodeFrameArray = pResKeyFrame->GetTagFrameArray();
			if( NodeFrameArray.empty() )
				return false;

			// 取最后一帧的挂载点
			const TagNodeFrame& NodeFrame = NodeFrameArray[NodeFrameArray.size()-1];
			for( size_t i = 0; i < NodeFrame.nodes.size(); i++ )
			{
				if( dwNameFourCC == NodeFrame.nodes[i].name )
					return true;
			}
			return false;
		}
		else
		{
			return false;
		}
	}
}//namespace Cool3D