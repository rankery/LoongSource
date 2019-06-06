#pragma once
#include "..\ResSys\VertStream.h"
#include <vector>
using namespace std;

namespace Cool3D
{
	class NodeAnimationCtrl;
	class ResCloth;
	/**	\class AniCloth_SW
		\brief 使用软件计算cloth的vert buffer
	*/
	class AniCloth_SW
	{
		enum{NUM_LOD=2};

		struct tagMass 
		{
			Vector3	v;			// 速度
			Vector3 f;			// 力
			DWORD	index;		// 顶点索引
			float	oneOverM;	// 1/m

			tagMass()
			{
				memset(this,0,sizeof(tagMass));
				oneOverM = 1.0f;
			}
		};

	public:
		AniCloth_SW(void);
		virtual ~AniCloth_SW(void);

		void Create(const ResCloth *pCloth);

		/**	计算骨骼绑定顶点
		*/
		void ComputeSkinVB(const NodeAnimationCtrl *pCtrl,int lod);
		/** 计算布料顶点
			\remark	布料模拟需要每隔固定时间进行模拟计算，所以应该在forceupdate时调用
		*/
		void ComputeClothVB(const NodeAnimationCtrl *pCtrl,const Matrix4& worldMat,float deltaTime);

		const VertStream* GetVertStream() const { return &m_vertStream; }

	protected:
		void CreateVertStream(const ResCloth *pCloth);
		void CreateIndices(const ResCloth *pCloth);

		/** 布料模拟
		*/
		void ClothSimulate(const NodeAnimationCtrl *pCtrl,const Matrix4& worldMat,float deltaTime);

		/**	计算影响布料顶点力（内力、外力）
		*/
		void ComputeForces(const NodeAnimationCtrl *pCtrl,const Matrix4& worldMat,float deltaTime);

		/**	欧拉一次积分
		*/
		void EulerIntegrate(float deltaTime);

		/**	中点二次积分
		*/
		void MidPointIntegrate(float deltaTime);

		/**	RK四次积分
		*/
		void RK4Integrate(float deltaTime);

		void Integrate(float deltaTime);

	public:
		enum EIntegratorType
		{
			EIT_EULER		= 0,	// 欧拉一次积分
			EIT_MIDPOINT,			// 中点二次积分
			EIT_RK4					// RK四次积分
		};

	private:
		enum ECollisionTypes
		{
			ECT_NOT_COLLIDING = 0,	// 没有碰撞
			ECT_COLLIDING			// 发生碰撞
		};

		struct tagContact 
		{
			int		index;
			Vector3 normal;
		};

	private:
		ECollisionTypes CheckForCollisions(const NodeAnimationCtrl *pCtrl);
		void ResolveCollisions(void);

	protected:
		ResCloth				*m_pResCloth;
		VertStream				m_vertStream;
		vector<int>				m_indices[NUM_LOD];
		int						m_numLod;

		vector<tagMass>			m_massArray;
		vector<tagContact>		m_contactArray;

		EIntegratorType			m_IntegratorType;
		int						m_timeIterations;	// 积分倍数，加速布料动画的计算
		float					m_maxTimeStep;		// 最大计算步长
		Vector3					m_gravity;			// 重力加速度
		float					m_kr;				// 恢复系数
		float					m_xKd;				// x轴向空气的阻尼系数
		float					m_zKd;				// z轴向空气的阻尼系数
		float					m_yUpKd;			// y轴向向上空气的阻尼系数
		float					m_yDownKd;			// y轴向向下空气的阻尼系数
		float					m_dragForceKs;		// 骨骼绑定点拖拽力的恢复系数
		bool					m_isFirst;			//
		Vector3					m_lastPos;
		Vector3					m_vel;
	};
}//namespace Cool3D