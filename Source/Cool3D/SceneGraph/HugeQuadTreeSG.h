#pragma once
#include <set>
using namespace std;
#include "..\Cool3D.h"
#include "..\Math\GeomDefine.h"
#include "scenegraph.h"
#include "QuadTreeData.h"

class IFS;
namespace Cool3D
{
	class SGQuadTreeNode;
	class SGSimpleTerrain;
	class IRenderTexture;
	class IRenderCubeTexture;
	class IRenderToEnvMap;
	class IDeviceVizQuery;
	class Frustum;
	/**	\class HugeQuadTreeSG 
		\brief 大型的四叉树场景管理,提供对连续世界的支持
		\remarks 
		\par 外部只需要向此SG中增加Node,他会自动根据Camera的位置来管理Node的几何数据的生存期
		\par View Zone是一个以Camera的From点为中心的AABBox,进入此Box的Node则加载入内存,否则要清除出内存
	*/
	class Cool3D_API HugeQuadTreeSG :	public SceneGraph
	{
	public:
		enum EViewZoneType
		{
			EVZT_LitNode=0,
			EVZT_MidNode,
			EVZT_BigNode,
			EVZT_DynamicNode,
			EVZT_SimpleTerrain,
			EVZT_Num,
		};

		enum EQuadTreeType
		{
			EQTT_SmallNode=0,
			EQTT_MidNode,
			EQTT_BigNode,
			EQTT_Num
		};
	public:
		HugeQuadTreeSG(void);
		virtual ~HugeQuadTreeSG(void);

		//--SceneGraph
		virtual void LoadFromFile(IFS *pFS,const TCHAR* szMapName);
		virtual void AttachDynamicNode(SceneNode *pNode);
		virtual void DetachDynamicNode(const SceneNode *pNode);
		virtual void Render(float deltaTime);
		virtual void BuildProjectorMesh(const tagLight *pLight,const AABBox& box,DynamicMesh *pMesh);
		virtual void BuildDecalMesh( const AABBox& box,DynamicMesh *pMesh, const Vector3* pNormal );
		virtual void BindCamera(CameraBase *pCamera,const bool bSetDefaultFogDist=true);
		virtual void RenderReflectMap(CameraBase* pCamera,IRenderTexture* pRT,float surfaceHeight,bool bRenderTrnOnly);
		virtual void SyncInit();
		virtual void SetSceneFog(const tagDistFog& fog);

		//-- 连续世界的支持函数
		/**	设置View Zone的大小
		*/
		void SetViewZoneSize(EViewZoneType type,const Vector3& size);
		const Vector3& GetViewZoneSize(EViewZoneType type);
		/**	根据当前的ViewZone的大小和Camera的位置计算出ViewZone
		*/
		void CalCurrentViewZone(EViewZoneType type,AABBox& out);
		/**	使得ViewZone失效,即下次Render时更新ViewZone
			\remarks 一般在Camera移动时调用
		*/
		void InvalidViewZone();

		AABBox GetBox() const;
  
		//测试视点是否包含在物体中以及遮挡视点的物体半透明(输入参数为当前视点位置和上一次的视点位置以及看位置，返回为与视点碰撞的物体的包围盒)
		bool TestContainBetweenViewPointAndSceneObject(Vector3 vFrom,Vector3 vOldFrom,Vector3 vLookAt,Cool3D::AABBox& box);

		TResult RayCollide(const Ray& ray);

		/** 获取所有静态Node数量
			\remarks 包括未加载到内存中的
		*/
		DWORD GetNumStaticNodes();
		/** 获取内存中的静态Node
		*/
		void GetStaticNodes(vector<SceneNode*>& sceneNodes);

		bool RenderForRayCollide(Vector3& origin,Vector3& dir,float dist,IRenderTexture* pRenderTex,IDeviceVizQuery* pVizQuery);
		void RenderHemisphere(Vector3& eye,Vector3& dir,float dist,IRenderTexture* pRenderTex,IRenderToEnvMap* pRenderToEnvMap);
		void RenderShadowMap(CameraBase& camera,IRenderTexture* pRenderTex);
		void RenderCubeShadowMap(CameraBase& camera,IRenderCubeTexture* pRenderTex);

	private:
		Vector3			m_viewZoneSize[EVZT_Num];
		QuadTreeData	m_data[EQTT_Num];
		SGQuadTreeNode	*m_pRoot[EQTT_Num];
		bool			m_bLoaded;

		set<SceneNode*>	m_dynaList;	//set的删除比list快,而且可以防止重复attach

		SGSimpleTerrain* m_pSimpleTerrain;

		DECL_RTTI_DYNC(HugeQuadTreeSG);
	};
}//namespace Cool3D