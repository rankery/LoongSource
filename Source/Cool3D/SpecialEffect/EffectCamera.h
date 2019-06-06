#pragma once
#include "effectbase.h"

namespace Cool3D
{
	/**	\class EffectCamera 
		\brief 镜头特效
	*/
	class Cool3D_API EffectCamera : public EffectBase
	{
	public:
		struct tagCameraProp : public tagProp
		{
			float   attenuationFactor;  // 衰减系数
			float   range;              // 影响范围
			DWORD	dwReserve[32];

			tagCameraProp() : attenuationFactor( 0.0f ), range( 1000.0f )
			{
				memset(dwReserve,0,sizeof(dwReserve));
			}
		};

		struct tagState
		{
			float	lastTime;			// 这个状态持续的时间,可以用来与下一状态进行插值
			Vector3 swing;				// 振幅
			DWORD	dwReserve[8];

			tagState()	
			{	
				memset(this,0,sizeof(*this));
				lastTime=1.0f;

				memset(dwReserve,0,sizeof(dwReserve));
			}
		};
	public:
		EffectCamera(void);
		virtual ~EffectCamera(void);

		//--序列化接口
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);
		//--EffectBase
		virtual int GetNumRenderNodes()	{ return 0;}
		virtual RenderNode* GetRenderNode(int i) { return NULL; }
		virtual void Update(SceneGraph* pSG,SGEffectNode *pSGNode,CameraBase *pCamera,float deltaTime,float runTime);
		virtual void ResetPlayState();
		virtual const TCHAR* GetName() const;
		virtual void Rename(const TCHAR* szNewName);
		virtual void SetGroupID(int id){}
		virtual float GetPlayTime();
		virtual bool GetBox(AABBox& out) const;
		virtual int GetLOD() const;

		//--
		//编辑器数据导出接口
		tagCameraProp GetProp() const;
		int GetNumStates() const;
		tagState GetState(int i) const;
		//创建接口
		void BeginBuild(const tagCameraProp& prop);	//基本属性
		void AddKeyState(const tagState& state);
		void EndBuild();

	private:
		class Member;
		Member *m_p;

		DECL_RTTI_DYNC(EffectCamera);
	};
}//namespace Cool3D