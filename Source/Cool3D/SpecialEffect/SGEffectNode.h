#pragma once
#include "..\scenegraph\scenenode.h"
#include "EffectMsg.h"

namespace Cool3D
{
	class EffectBase;
	class EffectPathCtrl;
	/**	\class SGEffectNode 
		\brief 特效的SceneGraph节点
		\remarks 如果有父节点,则跟随父节点在世界中移动
	*/
	class Cool3D_API SGEffectNode : public SceneNode
	{
	public:
		enum { MAX_LOD = 4 };		// 最大LOD
		static int sUserLod;        // 外部设置的LOD（与SceneGraph计算出的LOD取较大的一个）
		struct tagEffectRenderNode
		{
			int nLOD;
			RenderNode* pRenderNode;
			tagEffectRenderNode( const int n, RenderNode* rendernode ) : 
			nLOD( n ), pRenderNode( rendernode ){}
		};

		SGEffectNode(void);
		virtual ~SGEffectNode(void);

		
		//--序列化接口
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);
		//--ScenneNode
		virtual bool IsInFrustum(const Frustum *pFrumstum) const;
		virtual void LoadRes(const TCHAR* szName);
		virtual UINT GetNumRenderNode() const;
		virtual RenderNode *GetRenderNode(UINT i) const;
		virtual const Matrix4& GetWorldMat() const;
		virtual void SetWorldMat(const Matrix4 *pMat);
		virtual void GetBox(AABBox& out) const;
		virtual IMaterial*	GetMtl(int mtlID);
		virtual void Update(float deltaTime,SceneGraph *pSG);
		virtual void ForceUpdate(float deltaTime,SceneGraph *pSG);
		
		//--
		void SetUserUpdate(bool bUserUpdate);//由用户手动调用Update,忽略scene graph的自动Update
		void UserUpdate(float deltaTime,SceneGraph *pSG);
		void SetBoxCenter(const Vector3& center);
		void SetUserWorldMat(bool bUserWorldMat);
		void SetUserPlay(bool bUserPlay);//由用户手动调用播放/关闭接口，加载完成后不会立即播放
		bool ReBuildLocalBox();
		int  GetCurLOD(); //获取当前的LOD

		void Play();//!开始播放
		void Close(); //!结束播放(有淡出效果)
		bool IsClosed();//!是否所有的特效元素都完全关闭了?
		void Pause();//!暂停播放
		void Resume();//!继续播放
		void SetTimeScale(float timeScale);//!设置播放速度
		void Show(bool bShow)	{	m_bShow=bShow;}
		float GetPlayTime();//!播放时长，无限长返回-1
		
		//!	得到本帧发生的消息
		const vector<EffectMsg::tagState>& GetFrmMsgArray() const;
		//!	添加一个帧消息,提供给EffectBase派生类调用
		void PushFrmMsg(EffectMsg::tagState msg);
		
		
		//--editor
		void AddEffect(EffectBase *pEffect);
		void AddEffectPath(EffectPathCtrl *pPath);
		void RemoveEffect(const TCHAR* szName,bool bAutoDel);
		void RemovePath(const TCHAR* szName,bool bAutoDel);
		EffectBase *FindEffect(const TCHAR* szName);
		EffectPathCtrl *FindPath(const TCHAR* szName);

		void RebuildEffectRenderNodes();
		float GetRunTime() const;//得到运行了多久

		void GetEffectArray(vector<EffectBase*>& out);
		void GetEffectPathArray(vector<EffectPathCtrl*>& out);

		bool IsBuild();

		//--
		EffectPathCtrl* GetEffectPath(const TCHAR* szName);

		// 专门为target路径提供的接口，方便外部设置、更新路径
		void ResetTargetPath( const Vector3& vStart, const Vector3& vTarget );
		void ChangeTargetPathTarget( const Vector3& vTarget );
		bool IsTargetPathArrive();
	protected:
		virtual void MainUpdate(float deltaTime,SceneGraph *pSG);
		void UpdataWorldBox();
	private:
		bool m_bShow;
		class Member;
		Member	*m_p;

		DECL_RTTI_DYNC(SGEffectNode);
	};
}//namespace Cool3D