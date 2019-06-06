#pragma once
#include "effectbase.h"

namespace Cool3D
{
	/**	\class EffectSound 
		\brief 特效所配的音效
	*/
	class Cool3D_API EffectSound :	public EffectBase
	{
	public:
		static bool OP_bEnable;
	public:
		struct tagSoundProp : public tagProp
		{
			bool	b3DSound;		//是否使用3D音效
			TCHAR	szPathName[32];	//使用那个Path
			float	timeLoop;	//循环间隔时间

			DWORD	dwReserve[7];

			tagSoundProp()
			{
				b3DSound=true;
				memset(szPathName,0,sizeof(szPathName));
				_tcscpy_s(szPathName,sizeof(szPathName)/sizeof(TCHAR),_T("none"));
				timeLoop = 0;

				memset(dwReserve,0,sizeof(dwReserve));
			}
		};
		struct tagState
		{
			float	soundTime;	//整个特效运行了多久之后播放这个音效
			bool	bLoop;		//这个音效是否循环播放
			TCHAR	szSoundPath[100];
			float   minDist;    //最小距离
			float   maxDist;    //最大距离
			float   volume;     //音量
			DWORD	dwReserve[5];

			tagState()
			{
				memset(this,0,sizeof(*this));
				minDist = 100.0f;
				maxDist = 5000.0f;
				volume = 1.0f;
			}
		};
	public:
		EffectSound(void);
		virtual ~EffectSound(void);

		//--序列化接口
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);
		//--EffectBase
		virtual int GetNumRenderNodes()	{	return 0;}
		virtual RenderNode* GetRenderNode(int i);
		virtual void Update(SceneGraph* pSG,SGEffectNode *pSGNode,CameraBase *pCamera,float deltaTime,float runTime);
		virtual void ResetPlayState();
		virtual const TCHAR* GetName() const;
		virtual void Rename(const TCHAR* szNewName);
		virtual void SetGroupID(int id){}
		virtual bool IsClosed();
		virtual void Close(float sgNodeRunTime);
		virtual bool GetBox(AABBox& out) const;
		virtual int GetLOD() const;

		//--
		//编辑器数据导出接口
		tagSoundProp GetProp() const;
		int GetNumStates() const;
		tagState GetState(int i) const;
		//创建接口
		void BeginBuild(const tagSoundProp& prop);	//基本属性
		void AddKeyState(const tagState& state);
		void EndBuild();

	private:
		class Member;
		Member *m_p;

		DECL_RTTI_DYNC(EffectSound);
	};
}//namespace Cool3D