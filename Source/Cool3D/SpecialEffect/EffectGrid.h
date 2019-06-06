#pragma once
#include "effectbase.h"

namespace Cool3D
{
	/**	\class EffectGrid 
		\brief 网格扭曲
		\todo 创建曲面
	*/
	class Cool3D_API EffectGrid : public EffectBase
	{
	public:
		struct tagGridProp : public tagProp
		{
			TCHAR	szPathName[32];	//使用那个Path
			int		xDiv,
					yDiv;//x*y个网格
			Vector3	offset;
			Vector3 initRot;

			int		faceType;//0平面,1双曲面,2球形公告板,3筒形公告板

			DWORD	dwReserve[7];

			tagGridProp():offset(0,0,0),initRot(0,0,0)
			{
				xDiv=yDiv=9;
				faceType=0;

				memset(szPathName,0,sizeof(szPathName));
				_tcscpy_s(szPathName,sizeof(szPathName)/sizeof(TCHAR),_T("none"));

				memset(dwReserve,0,sizeof(dwReserve));
			}

		};
		struct tagState
		{
			float	lastTime;	//这个状态持续的时间,可以用来与下一状态进行插值
			Color4f color;		//颜色
			Vector2 uvScale;	//贴图重复次数
			Vector2 size;		//大小

			Vector3 rotVel;		//自转速度

			float	waveSpeed;	//波的传播速度
			Vector3	wavePower;	//波在三个方向的振幅

			DWORD	dwReserve[8];

			tagState():color(1,1,1,1),uvScale(1,1),size(100,100),rotVel(0,0,0),
				wavePower(0,4,0)
			{
				lastTime=1.0f;
				waveSpeed=0.5f;

				memset(dwReserve,0,sizeof(dwReserve));
			}
		};
	public:
		EffectGrid(void);
		virtual ~EffectGrid(void);

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
		tagGridProp GetProp() const;
		int GetNumStates() const;
		tagState GetState(int i) const;
		//创建接口
		void BeginBuild(const tagGridProp& prop);	//基本属性
		void AddKeyState(const tagState& state);
		void EndBuild();

	private:
		class Member;
		Member *m_p;

		DECL_RTTI_DYNC(EffectGrid);
	};
}//namespace Cool3D