#pragma once
#include "effectbase.h"

namespace Cool3D
{
	/**	\class EffectExplosive 
		\brief 片,球爆炸特效
	*/
	class Cool3D_API EffectExplosive :	public EffectBase
	{
	public:
		struct tagExpProp: public tagProp
		{
			TCHAR	szPathName[32];	//使用那个Path
			Vector3 offset;			//与Path点的偏移量
			Vector3 rot;
			BYTE	faceType;		//0普通面片,1球形billboard,2筒型billboard
			BYTE	bodyType;		//0Plane,1Sphere
			Vector4	bodyParam;		//plane(x,y大小),sphere(x=>R)
			int		xDiv,			//切分参数
					yDiv;
			float	powerDec;		//对于非对称型body(plane),爆炸威力的衰减(离开中心点远则爆炸威力降低),(0到1之间,越小衰减越快)
			float	maxPower;
			float	minPower;
			float	airForce;//空气阻力系数
			float	gForce;	//重力系数

			Vector3	dir;		//爆炸方向
			Vector2	uvScale;	//贴图重复次数
			float	maxLifeTime;
			float	minLifeTime;

			BYTE	byReserve[64];
			
			tagExpProp()
			{
				_tcscpy_s(szPathName,sizeof(szPathName)/sizeof(TCHAR),_T("none"));
				offset=rot=Vector3(0,0,0);
				faceType=0;
				bodyType=0;
				bodyParam=Vector4(500,500,500,500);
				xDiv=yDiv=20;
				powerDec=1;
				maxPower=200;
				minPower=120;
				airForce=0.2f;
				gForce=-12;
				dir=Vector3(0,1,0);
				uvScale=Vector2(1,1);
				maxLifeTime=5;
				minLifeTime=2;
			}
		};
	public:
		EffectExplosive(void);
		virtual ~EffectExplosive(void);

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
		virtual bool GetBox(AABBox& out) const;
		virtual int GetLOD() const;

		//--
		void Build(const tagExpProp& prop);
		tagExpProp GetProperty() const;

	private:
		class Member;
		Member *m_p;

		DECL_RTTI_DYNC(EffectExplosive);
	};
}//namespace Cool3D