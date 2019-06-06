#pragma once
#include "effectbase.h"

namespace Cool3D
{
	/**	\class EffectBeam 
		\brief 光带特效
		\remarks 
		\todo 可以加入Noise
	*/
	class Cool3D_API EffectBeam : public EffectBase
	{
	public:
		struct tagBeamProp : public tagProp
		{
			TCHAR	szPathName[32];	//使用那个Path
			int		div;		//分割次数
			float	sampleTime;	//采样的时间间隔
			float	divVScale;	//贴图V坐标

			DWORD	dwReserve[8];

			tagBeamProp()
			{
				memset(szPathName,0,sizeof(szPathName));
				_tcscpy_s(szPathName,sizeof(szPathName)/sizeof(TCHAR),_T("none"));

				div=64;
				sampleTime=0.1f;
				divVScale=1.0f;

				memset(dwReserve,0,sizeof(dwReserve));
			}
		};
		struct tagState
		{
			float	lastTime;
			float	width;			//宽度
			float	lenWidthInc;	//宽度随着长度的增加而增加(每次分割是计算增加)
			Color4f	headColor;		//头部颜色
			Color4f endColor;		//尾部颜色

			DWORD	dwReserve[8];

			tagState():headColor(1,1,1,1),endColor(1,1,1,1)
			{
				lastTime=1;
				width=50;
				lenWidthInc=0;

				memset(dwReserve,0,sizeof(dwReserve));
			}
		};
	public:
		EffectBeam(void);
		virtual ~EffectBeam(void);

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
		tagBeamProp GetProp() const;
		int GetNumStates() const;
		tagState GetState(int i) const;
		//创建接口
		void BeginBuild(const tagBeamProp& prop);
		void AddKeyState(const tagState& state);
		void EndBuild();

	private:
		class Member;
		Member *m_p;

		DECL_RTTI_DYNC(EffectBeam);
	};
}//namespace Cool3D