#pragma once
#include "effectbase.h"

namespace Cool3D
{
	/**	\class EffectParticleEmitter
		\brief 粒子发射器
		\remarks 
		.负责计算粒子发出时的初始状态,粒子发出时各种状态的变化速度(粒子根据这些速度自己演变)
	*/
	class Cool3D_API EffectParticleEmitter : public EffectBase
	{
	public:
		struct tagPEMProp:public tagProp
		{
			TCHAR		szPathName[32];	//使用那个Path
			int			shape;			//发射器的形状(0球,1box,2cone,不应包含位置信息),粒子随机的从这个形状中产生
			int			maxParticles;	//最大粒子个数
			int			initParNum;		//初始化时产生多少个粒子
			Vector3		pathOffset;
			int			velType;		//初始粒子的速度方向产生方式(0离心方向,1指定速度)
			int			polygonType;	//0为sprite,1为spark like,2普通面片

			float		gForce;		//重力系数(影响速度)
			float		airForce;	//空气阻力系数(影响速度)

			Vector3		initRot;	//初始自转

			bool		bParAutoFade;	//自动计算粒子Alpha淡出的速度
			bool		bToWorld;		//粒子是否跟随发射器移动(true不跟随)

			BYTE		byReserve[30];

			tagPEMProp()
			{
				memset(szPathName,0,sizeof(szPathName));
				_tcscpy_s(szPathName,sizeof(szPathName)/sizeof(TCHAR),_T("none"));

				shape=0;
				maxParticles=200;
				initParNum=0;
				pathOffset=Vector3(0,0,0);
				velType=0;
				polygonType=0;
				gForce=airForce=0;
				initRot=Vector3(0,0,0);

				bToWorld = true;
				memset(dwReserve,0,sizeof(dwReserve));
			}
		};
		struct tagState
		{
			float		lastTime;
			Vector4		shapeParam;		//(球:x为R),(box:xyz为size),(cone:bottomR,topR,Height)
			Vector3		rotVel;			//发射器自转速度
			float		emitSpeed;		//每秒钟产生多少个粒子

			float		parLifeTimeMax;	//粒子的生存时间
			Vector3		parVelDir;		//速度的方向
			float		parSpeedMax;	//粒子的初速率
			float		parSpeedMin;

			float		parSwingMax,	//粒子的摇摆
						parSwingMin;

			Vector3		parRotSpeedMax,	//粒子的自转.因为粒子的Quad总是面向摄像机的,所以自转只有一个参数
						parRotSpeedMin;
		
			float		parSizeMax;		//粒子初始大小,最大值
			float		parSizeMin;		//粒子初始大小,最小值,每次产生粒子在max~min之间随机
			float		parSizeVel;		//粒子大小的变化速度
			
			Color4f		parColor;		//粒子的初始颜色
			Color4f		parColorVel;	//粒子的颜色变化速度

			float		parLifeTimeMin;
			DWORD	dwReserve[7];

			tagState():lastTime(1),shapeParam(10,0,0,0),rotVel(0,0,0),emitSpeed(10),
				parLifeTimeMax(6),parLifeTimeMin(6),
				parVelDir(0,1,0),parSpeedMax(8),parSpeedMin(8),
				parSwingMax(0),parSwingMin(0),
				parRotSpeedMax(0,0,0),parRotSpeedMin(0,0,0),
				parSizeMax(20),parSizeMin(20),parSizeVel(0),
				parColor(1,1,1,1),parColorVel(0,0,0,0)
			{
				memset(dwReserve,0,sizeof(dwReserve));
			}
		};
	public:
		EffectParticleEmitter(void);
		virtual ~EffectParticleEmitter(void);

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
		virtual bool IsClosed();
		virtual void SetGroupID(int id);
		virtual float GetPlayTime();
		virtual bool GetBox(AABBox& out) const;
		virtual int GetLOD() const;

		void OnSetWorldMat( const Matrix4& matWorld );
		//--
		//编辑器数据导出接口
		tagPEMProp GetProp() const;
		int GetNumStates() const;
		tagState GetState(int i) const;
		//创建接口
		void BeginBuild(const tagPEMProp& prop);	//基本属性
		void AddKeyState(const tagState& state);
		void EndBuild();

		void SetPathOffset(const Vector3& pos);

		static bool s_bLimitMaxParticleNum;//是否限制最大粒子数量
	private:
		class Member;
		Member *m_p;

		DECL_RTTI_DYNC(EffectParticleEmitter);
	};
}//namespace Cool3D