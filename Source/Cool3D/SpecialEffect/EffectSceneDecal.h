#pragma once
#include "effectbase.h"

namespace Cool3D
{
	/**	\class		EffectSceneDecal 
		\brief		场景贴花
		\remarks	
	*/
	class Cool3D_API EffectSceneDecal : public EffectBase
	{
	public:
		struct tagDecalProp : public tagProp
		{
			TCHAR	 szPathName[32];	   // 使用那个Path
			Vector3  offset;			   // 与Path点的偏移量
			float    initRot;	           // 初始自转
			BOOL     bUseDefaultMesh;	   // 在没有生成DecalMesh时使用默认Mesh
			BOOL     bOnlyUseDefaultMesh;  // 只使用默认Mesh
			float    rangeFront;           // 贴花范围（前方）
			float    rangeBack;            // 贴花范围（后方）
			DWORD	 dwReserve[8];

			tagDecalProp()
			{
				memset(szPathName,0,sizeof(szPathName));
				_tcscpy_s(szPathName,sizeof(szPathName)/sizeof(TCHAR),_T("none"));

				offset = Vector3( 0.0f, 0.0f, 0.0f );
				initRot = 0.0f;
				bUseDefaultMesh = 0;
				bOnlyUseDefaultMesh = 0;
				rangeFront = 50.0f;
				rangeBack = 50.0f;

				memset(dwReserve,0,sizeof(dwReserve));
			}
		};

		struct tagState
		{
			float	lastTime;	// 这个状态持续的时间,可以用来与下一状态进行插值
			Color4f color;		// 颜色
			Vector2 size;		// 大小
			float	rotVel;		// rot变化速度

			DWORD	dwReserve[8];

			tagState()	
			{	
				memset(this,0,sizeof(*this));
				lastTime=1.0f;
				color=Color4f(1,1,1,1);
				size=Vector2(200,200);
				
				memset(dwReserve,0,sizeof(dwReserve));
			}
		};
	public:
		EffectSceneDecal(void);
		virtual ~EffectSceneDecal(void);

		//--序列化接口
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);
		//--EffectBase
		virtual int GetNumRenderNodes()	{	return 1;}
		virtual RenderNode* GetRenderNode(int i);
		virtual void Update(SceneGraph* pSG,SGEffectNode *pSGNode,CameraBase *pCamera,float deltaTime,float runTime);
		virtual void ResetPlayState();
		virtual const TCHAR* GetName() const;
		virtual void Rename(const TCHAR* szNewName);
		virtual void CreateMtl(const tagMtlEffectDesc& desc);
		virtual void SetGroupID(int id);
		virtual bool GetBox(AABBox& out) const;
		virtual int GetLOD() const;

		// 手动设置贴花位置，默认为特效节点的位置+offset
		void SetUserPos(const Vector3& pos);
		// 设置贴花方向，默认为Y轴方向
		void SetDir( const Vector3& vDir, const Vector3& vUpVec );
		// 获得贴花大小
		const Vector2& GetDecalSize() const;
		float GetFrontRange() const;
		float GetBackRange() const;

		//编辑器数据导出接口
		tagDecalProp GetProp() const;
		int GetNumStates() const;
		tagState GetState(int i) const;
		//创建接口
		void BeginBuild(const tagDecalProp& prop);
		void AddKeyState(const tagState& state);
		void EndBuild();

	private:
		class Member;
		Member *m_p;

		DECL_RTTI_DYNC(EffectSceneDecal);
	};
}//namespace Cool3D