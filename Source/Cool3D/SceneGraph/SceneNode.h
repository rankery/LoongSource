#pragma once
#include "..\Cool3D.h"
#include "..\Math\Math.h"
#include "..\Math\GeomDefine.h"
#include "..\Math\Ray.h"
#include "..\Util\RTTIObj.h"
#include "..\Util\FArchive.h"

namespace Cool3D
{
	class SceneGraph;
	class IMaterial;
	class RenderNode;
	class Frustum;
	class Occluder;

	enum ESceneNodeType
	{
		ESNT_StaticMesh,
	};
	/**	\class SceneNode
		\brief SceneGraph中的一个节点
		\todo 加一个"QueryResInterface()"的接口,用来得到内部的资源指针,因为内部的资源有可能是人物等复杂接口,需要客户端对其进行操作
		\todo 由SceneNode管理IMaterial对象,因为在运行时材质可能改变,主要指换装,而且一个Res中可
		能包含多个材质.
		\todo 子node的world matrix??
		\todo 增加一个Enable()的接口,有些物体需要临时隐藏起来
	*/
	class Cool3D_API SceneNode : public RTTIObj,public ISerializable
	{
	public:
		SceneNode(void);
		virtual ~SceneNode(void);

		//--序列化接口
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);
		//--
		/**	测试是否在视锥内
			\remarks 由Node处理是因为,Node的类别不同其Bounding Volume可能不同,例如LightNode可能为Sphere或者Cone
		*/
		virtual bool IsInFrustum(const Frustum *pFrumstum) const =0;
		virtual void LoadRes(const TCHAR* szName) =0;
		virtual UINT GetNumRenderNode() const =0;
		virtual RenderNode *GetRenderNode(UINT i) const =0;
		virtual const Matrix4& GetWorldMat() const =0;
		virtual void SetWorldMat(const Matrix4 *pMat) =0;
		virtual void GetBox(AABBox& out) const =0;//因为有的SceneNode是即时计算的,所以不直接返回一个指针或者引用
		virtual UINT GetNumMtl() const{ return 0;}
		virtual IMaterial*	GetMtl(int mtlID) =0;
		/**	update this node and all children recursive*/
		virtual void Update(float deltaTime,SceneGraph *pSG);
		/**	强制更新
			\remarks Update()函数只有在Node可见时才会被调用,主要可以用来更新动画等;
				而 FroceUpdate()函数无论Node是否可见都会被调用,主要可以用来处理内部逻辑,
				以及Is Still Loading等检测.
		*/
		virtual void ForceUpdate(float deltaTime,SceneGraph *pSG) =0;
		virtual TResult RayCollide(const Ray& ray);
		
		UINT GetNumOccluder();
		Occluder* GetOccluder(UINT i);
		void AttachOccluder(Occluder* pOccluder)
		{
			m_occluders.push_back(pOccluder);
		}
		void DetachOccluder(Occluder* pOccluder)
		{
			for(vector<Occluder*>::iterator iter=m_occluders.begin();
				iter!=m_occluders.end();++iter)
			{
				if((*iter)==pOccluder)
				{
					m_occluders.erase(iter);
					break;
				}
			}
		}
		/**	向本节点中添加一个子节点,由this管理其生存期
			\remarks 典型的应用有添加ProjectShadow,decal,以及Avatar
		*/
		void AttachChild(SceneNode *pChild)
		{
			pChild->m_parent=this;
			m_children.push_back(pChild);
		}
		
		/** If you detach the nod from parent node,you need DELETE it yourself*/
		void DetachChild(const SceneNode *pNode);
		void DetachChild(const TCHAR* szClassName);
		int GetNumChildren() const					{	return m_children.size();	}
		const SceneNode *GetChild(int i) const		{	return m_children[i];	}
		SceneNode *GetChildVar(int i)		{	return m_children[i];	}

		//userdata
		void SetUserData(const TCHAR* szUserData)	{	m_szUserData=szUserData;}
		const TCHAR* GetUserData() const				{	return m_szUserData.c_str();}
		const SceneNode*	GetParent() const		{	return m_parent; }

		// For scenegraph visitor
		int GetStaticNodeID()			{ return m_staticNodeID;}
		void SetStaticNodeID(int id)	{ m_staticNodeID=id;	}

		void SetLightmap(const TCHAR* szLightmap)	{	m_szLightMap=szLightmap;}
		const TCHAR* GetLightmap()					{	return m_szLightMap.c_str();}

		virtual bool IsLoaded(){ return true;}

	private:
		vector<Occluder*>	m_occluders;
		vector<SceneNode*>	m_children;
		SceneNode*			m_parent;
		tstring				m_szUserData;
		tstring				m_szLightMap;
		int					m_staticNodeID;

		DECL_RTTI(SceneNode);
	};
}//namespace Cool3D