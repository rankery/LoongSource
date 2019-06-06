#pragma once
#include "effectbase.h"

namespace Cool3D
{
	/**	\class EffectQuad 
		\brief 片状特效
	*/
	class Cool3D_API EffectQuad : public EffectBase
	{
	public:
		struct tagQuadProp : public tagProp
		{
			TCHAR	szPathName[32];	//使用那个Path
			Vector3 offset;			//与Path点的偏移量
			Vector3 initRot;	//初始自转
			BYTE	faceType;	//0普通片,1球形billboard,2筒行billboard
			int		tailSize;	//拖尾长度(残影),间隔指定时间记录下Quad的状态,Alphav逐渐降低,作为残影
			float	tailInterval;//拖尾间隔时间
			float	tailLife;	//残影的生命周期,如果为0,则自动根据残影个数和残影间隔计算出

			DWORD	dwReserve[8];

			tagQuadProp():offset(0,0,0),initRot(0,0,0),faceType(0),
				tailSize(1),tailInterval(0.1f),tailLife(0)
			{
				memset(szPathName,0,sizeof(szPathName));
				_tcscpy_s(szPathName,sizeof(szPathName)/sizeof(TCHAR),_T("none"));

				memset(dwReserve,0,sizeof(dwReserve));
			}
		};

		struct tagState
		{
			float	lastTime;//这个状态持续的时间,可以用来与下一状态进行插值
			Color4f color;	//颜色
			Vector2 size;	//大小
			Vector3 rotVel;		//rot变化速度

			DWORD	dwReserve[8];

			tagState()	
			{	
				memset(this,0,sizeof(*this));
				lastTime=1.0f;
				color=Color4f(1,1,1,1);
				size=Vector2(100,100);

				memset(dwReserve,0,sizeof(dwReserve));
			}
		};
	public:
		EffectQuad(void);
		virtual ~EffectQuad(void);

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
		tagQuadProp GetProp() const;
		int GetNumStates() const;
		tagState GetState(int i) const;
		//创建接口
		void BeginBuild(const tagQuadProp& prop);	//基本属性
		void AddKeyState(const tagState& state);
		void EndBuild();

		void SetOffset( const Vector3& pos );

	private:
		class Member;
		Member *m_p;

		DECL_RTTI_DYNC(EffectQuad);
	};
}//namespace Cool3D