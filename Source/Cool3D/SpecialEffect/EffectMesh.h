#pragma once
#include "effectbase.h"

namespace Cool3D
{
	/**	\class EffectMesh 
		\brief 控制一个模型的空间运动
		\remarks 
		.模型必须是StaticMesh
		.模型必须只有一个材质,否则无法和EffectBase的Mtl接口统一
	*/
	class Cool3D_API EffectMesh : public EffectBase
	{
	public:
		struct tagMeshProp : public tagProp
		{
			TCHAR		szPathName[32];	//使用那个Path
			TCHAR		szMeshName[64];
			Vector3		offset;
			Vector3		initRot;

			DWORD	dwReserve[8];

			tagMeshProp():offset(0,0,0),initRot(0,0,0)
			{
				memset(szPathName,0,sizeof(szPathName));
				_tcscpy_s(szPathName,sizeof(szPathName)/sizeof(TCHAR),_T("none"));
				memset(szMeshName,0,sizeof(szMeshName));

				memset(dwReserve,0,sizeof(dwReserve));
			}
		};

		struct tagState
		{
			float	lastTime;	//这个状态持续的时间,可以用来与下一状态进行插值
			float	alpha;		//透明度
			Vector3 scale;		//大小
			Vector3 rotVel;		//自转速度
			Vector3	vel;		//移动速度,如果附加了路径,则变成了与路径偏移的速度

			DWORD	dwReserve[8];
			
			tagState()	
			{	
				memset(this,0,sizeof(*this));
				lastTime=1.0f;
				alpha=1.0f;
				scale=Vector3(1,1,1);

				memset(dwReserve,0,sizeof(dwReserve));
			}
		};
	public:
		EffectMesh(void);
		virtual ~EffectMesh(void);

		//--序列化接口
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);
		//--EffectBase
		virtual int GetNumRenderNodes()	{	return 0;}
		virtual RenderNode* GetRenderNode(int i);
		virtual void OnDetachFromSGNode(SGEffectNode *pSGNode);
		virtual void Update(SceneGraph* pSG,SGEffectNode *pSGNode,CameraBase *pCamera,float deltaTime,float runTime);
		virtual void ResetPlayState();
		virtual const TCHAR* GetName() const;
		virtual void Rename(const TCHAR* szNewName);
		virtual void SetGroupID(int id);
		virtual void CreateMtl(const tagMtlEffectDesc& desc);
		virtual float GetPlayTime();
		virtual bool GetBox(AABBox& out) const;
		virtual int GetLOD() const;

		//--
		//编辑器数据导出接口
		tagMeshProp GetProp() const;
		int GetNumStates() const;
		tagState GetState(int i) const;
		//创建接口
		void BeginBuild(const tagMeshProp& prop);
		void AddKeyState(const tagState& state);
		void EndBuild();
	private:
		class Member;
		Member *m_p;

		DECL_RTTI_DYNC(EffectMesh);
	};
}//namespace Cool3D