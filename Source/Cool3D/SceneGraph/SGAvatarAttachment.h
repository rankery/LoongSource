#pragma once
#include "EntityNode.h"
#include "..\Cool3D.h"

namespace Cool3D
{
	/**	\class SGAvatarAttachment 
		\brief 将一个模型挂载到某块bone上
	*/
	class Cool3D_API SGAvatarAttachment :	public EntityNode
	{
	public:
		SGAvatarAttachment(void);
		virtual ~SGAvatarAttachment(void);

		//--序列化接口
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);
		//--SceneNode
		virtual void Update(float deltaTime,SceneGraph *pSG);
		
		//--
		/**	把本结点邦定到父结点的Tag node(szTagName)上
		    如果本结点上有与szTagName同名的Tag node，则附加此Tag node的偏移
		*/
		void BindToTagNode(const TCHAR* szTagName);

		bool GetModelSpace(const TCHAR* szTagName,Matrix4& out);
		bool HaveTagNode(const TCHAR* szTagName) const;
		// 获得挂载点名称
		const tstring GetTagNodeName() const;
		// 绑定的骨骼是否镜像
		bool IsBindMirroredBone() const;
	private:
		bool GetSrcTagNodeOffset( Vector3* pOut );
		class Member;
		Member *m_p;

		DECL_RTTI_DYNC(SGAvatarAttachment);
	};
}//namespace Cool3D