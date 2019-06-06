#pragma once
#include "effectbase.h"

namespace Cool3D
{
	/**	\class EffectMsg 
		\brief 特效消息
		\remarks 可以设置某个时刻产生某个消息,用来通知游戏层,例如攻击点消息
	*/
	class Cool3D_API EffectMsg : public EffectBase
	{
	public:
		struct tagState
		{
			float	msgTime;	//整个特效运行了多久之后产生这个消息
			union//消息FourCC
			{
				TCHAR	szMsg[5];
				DWORD	dwMsg;
			};

			union
			{
				TCHAR	szMsgParam1[5];
				DWORD	dwMsgParam1;
			};

			union
			{
				TCHAR	szMsgParam2[5];
				DWORD	dwMsgParam2;
			};
			

			DWORD	dwReserve[8];

			tagState()
			{
				msgTime=0;
				dwMsg=0;
				dwMsgParam1=dwMsgParam2=0;

				memset(dwReserve,0,sizeof(dwReserve));
			}
		};
	public:
		EffectMsg(void);
		virtual ~EffectMsg(void);

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
		virtual bool GetBox(AABBox& out) const{ return true; }
		virtual int GetLOD() const { return 0; }

		//--
		//编辑器数据导出接口
		tagProp GetProp() const;
		int GetNumStates() const;
		tagState GetState(int i) const;
		//创建接口
		void BeginBuild(const tagProp& prop);	//基本属性
		void AddKeyState(const tagState& state);
		void EndBuild();

	private:
		class Member;
		Member *m_p;

		DECL_RTTI_DYNC(EffectMsg);
	};
}//namespace Cool3D