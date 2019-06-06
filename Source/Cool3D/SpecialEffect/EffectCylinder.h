#pragma once
#include "effectbase.h"

namespace Cool3D
{
	/**	\class EffectCylinder 
		\brief 筒状/带状/星状特效
	*/
	class Cool3D_API EffectCylinder : public EffectBase
	{
	public:
		struct tagCylinderProp : public tagProp
		{
			TCHAR	szPathName[32];	//使用那个Path
			int		div;			//将圆周分割多少次
			Vector3 offset;			//与Path点的偏移量
			float	uScale;
			Vector3	initRot;		//初始旋转

			DWORD	dwReserve[8];

			tagCylinderProp():div(12),offset(0,0,0),uScale(1),initRot(0,0,0)
			{
				memset(szPathName,0,sizeof(szPathName));
				_tcscpy_s(szPathName,sizeof(szPathName)/sizeof(TCHAR),_T("none"));

				memset(dwReserve,0,sizeof(dwReserve));
			}
		};
		struct tagState
		{
			float	lastTime;		//这个状态持续的时间,可以用来与下一状态进行插值
			float	numCircle;		//圈数
			float	high;			//筒的高度,带的宽度(垂直方向长度)
			float	topRadius;		//顶半径
			float	bottomRadius;	//底半径
			float	divTopRadiusInc;	//分割时顶半径增长速度
			float	divBottomRadiusInc;	//分割时底半径增长速度
			float	topShrink;		//顶收缩程度,用来生成五角星,六芒星等
			float	bottomShrink;	//底收缩程度
			float	divTopRise;		//每次分割顶面高度上升速度
			float	divBottomRise;	//每次分割底面高度上升速度
			Color4f	topColor;		//顶部顶点颜色
			Color4f	bottomColor;	//底部顶点颜色

			Vector3	rotVel;			//自转速度

			DWORD	dwReserve[8];

			tagState():topColor(1,1,1,1),bottomColor(1,1,1,1),rotVel(0,0,0)
			{
				lastTime=1.0f;
				numCircle=1.0f;
				high=50.0f;
				topRadius=80.0f;
				bottomRadius=40.0f;
				divTopRadiusInc=0;
				divBottomRadiusInc=0;
				topShrink=0.0f;
				bottomShrink=0.0f;
				divTopRise=0.0f;
				divBottomRise=0.0f;

				memset(dwReserve,0,sizeof(dwReserve));
			}
		};
	public:
		EffectCylinder(void);
		virtual ~EffectCylinder(void);

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
		virtual void SetGroupID(int id);
		virtual float GetPlayTime();
		virtual bool GetBox(AABBox& out) const;
		virtual int GetLOD() const;

		//--
		//编辑器数据导出接口
		tagCylinderProp GetProp() const;
		int GetNumStates() const;
		tagState GetState(int i) const;
		//创建接口
		void BeginBuild(const tagCylinderProp& prop);
		void AddKeyState(const tagState& state);
		void EndBuild();

	private:
		class Member;
		Member *m_p;

		DECL_RTTI_DYNC(EffectCylinder);
	};
}//namespace Cool3D