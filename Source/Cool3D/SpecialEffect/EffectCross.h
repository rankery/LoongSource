#pragma once
#include "effectbase.h"

namespace Cool3D
{
	/**	\class EffectCross 
		\brief 十字交叉的面片序列
	*/
	class Cool3D_API EffectCross :	public EffectBase
	{
	public:
		struct tagCrossProp: public tagProp
		{
			TCHAR	szPathName[32];	//使用那个Path
			Vector3 offset;			//与Path点的偏移量
			Vector3 initRot;		//初始自转
			int		numFace;		//单个方向的附加面数(共四个方向),0则只有一个十字
			bool	bHFace;			//是否产生横向顶面
			
			BYTE	byReserve[64];

			tagCrossProp()
			{
				_tcscpy_s(szPathName,sizeof(szPathName)/sizeof(TCHAR),_T("none"));
				offset=initRot=Vector3(0,0,0);
				numFace=0;
				bHFace=false;

				memset(byReserve,0,sizeof(byReserve));
			}
		};

		struct tagState
		{
			float	lastTime;//这个状态持续的时间,可以用来与下一状态进行插值
			Color4f color;	//颜色
			Vector2 size;	//大小
			Vector3 rotVel;	//rot变化速度

			float	faceDist;//每个面之间的基本间隔
			float	faceDistInc;//沿着单个方向,没产生一个面,距离增加多少
			Vector2	faceSizeInc;//沿单个方向,每产生一个面,大小增加多少
			float	faceAlphaDec;//沿单个方向,每产生一个面,透明度减少多少

			BYTE	byReserve[64];

			tagState()
			{
				memset(this,0,sizeof(*this));
				lastTime=1.0f;
				color=Color4f(1,1,1,1);
				size=Vector2(200,200);
				faceDist=50;
			}

		};
	public:
		EffectCross(void);
		virtual ~EffectCross(void);

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
		tagCrossProp GetProp() const;
		int GetNumStates() const;
		tagState GetState(int i) const;
		//创建接口
		void BeginBuild(const tagCrossProp& prop);	//基本属性
		void AddKeyState(const tagState& state);
		void EndBuild();

	private:
		class Member;
		Member *m_p;

		DECL_RTTI_DYNC(EffectCross);
	};
}//namespace Cool3D