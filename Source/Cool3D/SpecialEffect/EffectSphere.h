#pragma once
#include "effectbase.h"

namespace Cool3D
{
	/**	\class EffectSphere 
		\brief 一个几何球体
	*/
	class Cool3D_API EffectSphere : public EffectBase
	{
	public:
		struct tagSphereProp : public tagProp
		{
			TCHAR	szPathName[32];	//使用那个Path
			Vector3 offset;			//与Path点的偏移量
			Vector3 initRot;		//初始自转,角度
			float	radius;
			Vector2	pitchRange;//角度范围(mini,max)
			Vector2	yawRange;
			int		slices;	//分割次数
			Vector2	uvScale;	//贴图重复次数
			
			BYTE	byReserve[64];

			tagSphereProp()
			{
				_tcscpy_s(szPathName,sizeof(szPathName)/sizeof(TCHAR),_T("none"));
				offset=initRot=Vector3(0,0,0);
				radius=200;
				yawRange=Vector2(0,360);
				pitchRange=Vector2(0,180);
				slices=16;
				uvScale=Vector2(1,1);

				memset(byReserve,0,sizeof(byReserve));
			}
		};

		struct tagState
		{
			float	lastTime;//这个状态持续的时间,可以用来与下一状态进行插值
			Color4f color;	//颜色
			Vector3 rotVel;	//rot变化速度
			Vector3	sacle;
						
			BYTE	byReserve[64];

			tagState():color(1,1,1,1),rotVel(0,0,0),sacle(1,1,1)
			{
				lastTime=1.0f;

				memset(byReserve,0,sizeof(byReserve));
			}
		};

	public:
		EffectSphere(void);
		virtual ~EffectSphere(void);

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
		virtual void SetGroupID(int id);
		virtual float GetPlayTime();
		virtual bool GetBox(AABBox& out) const;
		virtual int GetLOD() const;

		//--
		//编辑器数据导出接口
		tagSphereProp GetProp() const;
		int GetNumStates() const;
		tagState GetState(int i) const;
		//创建接口
		void BeginBuild(const tagSphereProp& prop);	//基本属性
		void AddKeyState(const tagState& state);
		void EndBuild();

	private:
		class Member;
		Member *m_p;

		DECL_RTTI_DYNC(EffectSphere);
	};
}//namespace Cool3D