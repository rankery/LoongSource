#include "stdafx.h"
#include "AniCloth_SW.h"
#include "AnimationCtrl.h"
#include "..\VFS\IFS.h"
#define VFS_READ
#include "..\Public\SkinBoneDefine.h"
#include "..\ResSys\ResCloth.h"

namespace Cool3D
{
	namespace
	{
#define EPSILON			0.00001f
#define	MAXKS			30.0f
#define MAXPOSITIVEVEL	1000.0f		// 单位：cm/s
#define MAXNEGATIVEVEL	-1000.0f	// 单位：cm/s

		float DotProduct(const Vector3& vec1, const Vector3& vec2)
		{
			return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
		}
	}

	AniCloth_SW::AniCloth_SW(void)
		: m_pResCloth(0)
		, m_numLod(0)
		, m_IntegratorType(EIT_EULER)
		, m_kr(0.1f)
		, m_xKd(0.8f)
		, m_zKd(0.8f)
		, m_yUpKd(3.2f)
		, m_yDownKd(0.4f)
		, m_dragForceKs(40.0f)
		, m_timeIterations(10)
		, m_maxTimeStep(0.01f)
		, m_isFirst(true)
	{
		m_gravity = Vector3(0.0f,-16.0f,0.0f);
	}

	AniCloth_SW::~AniCloth_SW(void)
	{
		m_pResCloth=NULL;
	}

	void AniCloth_SW::Create(const ResCloth *pCloth)
	{
		m_pResCloth=const_cast<ResCloth*>(pCloth);
		CreateVertStream(pCloth);
		CreateIndices(pCloth);
	}

	void AniCloth_SW::CreateVertStream(const ResCloth *pCloth)
	{
		const vector<SkinVert>& clothVertArray=pCloth->GetClothVerts();
		size_t numVert=clothVertArray.size();

		// 初始化m_vertStream
		m_vertStream.Create(EVType_PNT,numVert);
		Vert_PNT *pVB=(Vert_PNT *)m_vertStream.GetRawBuffer();

		for( size_t i=0; i<numVert; i++ )
		{
			pVB[i].pos=clothVertArray[i].pos;
			pVB[i].normal=clothVertArray[i].normal;
			pVB[i].uv=clothVertArray[i].uv;
		}

		// 初始化mass
		const vector<DWORD>& clothIndex = pCloth->GetClothIndex();

		m_massArray.resize( clothIndex.size() );
		for( size_t i=0; i<m_massArray.size(); i++ )
		{
			tagMass& mass = m_massArray[i];
			mass.v = Vector3(0.0f,0.0f,0.0f);
			mass.f = Vector3(0.0f,0.0f,0.0f);
			mass.index = clothIndex[i];
			mass.oneOverM = 1.0f;
		}
	}

	void AniCloth_SW::CreateIndices(const ResCloth *pCloth)
	{
		m_numLod=0;
		for( int i=0; i<NUM_LOD; i++ )
			m_indices[i].clear();

		// 找出所有lod的index，保存到临时set
		set<int> tmp[NUM_LOD];
		const vector<LodClothSubMesh*>& lodSubMesh=pCloth->GetLodSubMesh();
		for( int i=0; i<(int)lodSubMesh.size(); i++ )
		{
			const LodSubMesh* pLodSubMesh=lodSubMesh[i];
			int numLod=min(NUM_LOD,pLodSubMesh->numLod);
			if( numLod > m_numLod )
				m_numLod=numLod;
			for( int lod=0; lod<numLod; lod++ )
			{
				const FaceBuffer& faceBuffer=pLodSubMesh->lodFaces[lod];
				for( size_t j=0; j<faceBuffer.size(); j++ )
				{
					const TriFace& face=faceBuffer[j];
					tmp[lod].insert(face.index[0]);
					tmp[lod].insert(face.index[1]);
					tmp[lod].insert(face.index[2]);
				}
			}
		}
		// 从临时set导出到vector,使用vector能提高cpu cache命中率?
		for( int i=0; i<m_numLod; i++ )
		{
			set<int>& src=tmp[i];
			vector<int>& dst=m_indices[i];
			for( set<int>::iterator iter=src.begin(); iter!=src.end(); iter++ )
				dst.push_back(*iter);
		}
	}

	void AniCloth_SW::ComputeSkinVB( const NodeAnimationCtrl *pCtrl,int lod )
	{
		if( m_pResCloth==NULL || !m_pResCloth->IsCreated() )
			return;

		const vector<SkinVert>& clothVerts=m_pResCloth->GetClothVerts();
		Vert_PNT *pVB=(Vert_PNT *)m_vertStream.GetRawBuffer();
		size_t numClothVert=clothVerts.size();

		if( lod == 0 || m_numLod == 0 )
		{
			size_t i,j;
			size_t numBoneInf=0;
			for( i=0; i<numClothVert; i++,pVB++ )
			{
				const SkinVert& clothVert=clothVerts[i];
				numBoneInf=clothVert.boneInfs.size();
				if( numBoneInf > 0 )
					pVB->pos=MathConst::Zero3;
				else
					continue;

				for( j=0; j<numBoneInf; j++ )// 骨骼绑定顶点
				{
					const tagBoneInf& inf=clothVert.boneInfs[j];
					const Matrix4& boneSpace=*(pCtrl->GetBoneSpace(inf.boneId));
					pVB->pos+=(inf.offset*boneSpace)*inf.weight;
				}
			}
		}
		else if( lod > 0 && m_numLod > 0 )
		{
			size_t j;
			int index;
			size_t numBoneInf=0;
			lod--;
			if(lod>=m_numLod)
				lod=m_numLod-1;
			vector<int>& indices =m_indices[lod];
			size_t numIndex=indices.size();
			for( size_t i=0; i<numIndex; i++ )
			{
				index=indices[i];
				const SkinVert& clothVert=clothVerts[index];
				Vert_PNT& vert=pVB[index];

				numBoneInf=clothVert.boneInfs.size();
				if( numBoneInf > 0 )
					pVB->pos=MathConst::Zero3;
				else
					continue;

				for( j=0; j<numBoneInf; j++)// 骨骼绑定顶点
				{
					const tagBoneInf& inf=clothVert.boneInfs[j];
					const Matrix4& boneSpace=*(pCtrl->GetBoneSpace(inf.boneId));
					vert.pos+=(inf.offset*boneSpace)*inf.weight;
				}
			}
		}
	}

	void AniCloth_SW::ComputeClothVB(const NodeAnimationCtrl *pCtrl, const Matrix4& worldMat, float deltaTime)
	{
		if( m_pResCloth==NULL || !m_pResCloth->IsCreated() )
			return;

		if( !pCtrl->CanGetBoneSpace() )
			return;

		float lastTime = 0.0f;
		float time = deltaTime * m_timeIterations;

		while(lastTime < time)
		{
			float simTime = time - lastTime;
			if( simTime > m_maxTimeStep )
			{
				simTime = m_maxTimeStep;
			}

			ClothSimulate(pCtrl, worldMat, simTime);
			lastTime += simTime;
		}
	}

	void AniCloth_SW::ClothSimulate( const NodeAnimationCtrl *pCtrl, const Matrix4& worldMat, float deltaTime )
	{
		//// 非碰撞版本
		//ComputeForces(pCtrl,worldMat,deltaTime);

		//switch( m_IntegratorType )
		//{
		//case EIT_EULER:
		//	EulerIntegrate(deltaTime);
		//	break;

		//case EIT_MIDPOINT:
		//	MidPointIntegrate(deltaTime);
		//	break;

		//case EIT_RK4:
		//	RK4Integrate(deltaTime);
		//	break;
		//}

		// 碰撞版本
		int			collisionState;

		ComputeForces(pCtrl,worldMat,deltaTime);

		switch( m_IntegratorType )
		{
		case EIT_EULER:
			EulerIntegrate(deltaTime);
			break;

		case EIT_MIDPOINT:
			MidPointIntegrate(deltaTime);
			break;

		case EIT_RK4:
			RK4Integrate(deltaTime);
			break;
		}

		collisionState = CheckForCollisions( pCtrl );
		if( collisionState == ECT_COLLIDING )
		{
			ResolveCollisions();
		}
	}

	void AniCloth_SW::ComputeForces( const NodeAnimationCtrl *pCtrl, const Matrix4& worldMat, float deltaTime )
	{
		Vector3 deltaP;
		Vector3 deltaV;
		Vector3 springForce;
		float dist;
		float hterm;
		float dterm;

		// 重力
		for( size_t i=0; i<m_massArray.size(); i++ )
		{
			tagMass& mass = m_massArray[i];
			mass.f = Vector3(0.0f,0.0f,0.0f);

			mass.f += m_gravity / mass.oneOverM;

			// 空气阻力
			if( mass.v.y < 0.0f )
				mass.f.y += -m_yDownKd * mass.v.y;
			else
				mass.f.y += -m_yUpKd * mass.v.y;

			mass.f.x += -m_xKd * mass.v.x;
			mass.f.z += -m_zKd * mass.v.z;
		}

		// 布料的内部力
		Vert_PNT *pVB = (Vert_PNT *)m_vertStream.GetRawBuffer();
		const vector<tagSpring>& springs = m_pResCloth->GetSprings();
		for( size_t i=0; i<springs.size(); i++ )
		{
			const tagSpring& spring = springs[i];

			ASSERT( spring.p1 < m_massArray.size() );
			ASSERT( spring.p2 < m_massArray.size() );
			tagMass& mass1 = m_massArray[spring.p1];
			tagMass& mass2 = m_massArray[spring.p2];

			Vert_PNT& p1 = pVB[mass1.index];
			Vert_PNT& p2 = pVB[mass2.index];

			deltaP = p1.pos - p2.pos;
			dist = Vec3Len(&deltaP);

			float deltaDist = fabsf(dist - spring.restLen)/spring.restLen;
			float deltaKs = pow(deltaDist+1.0f,10);
			hterm = (dist - spring.restLen) * min(MAXKS,spring.ks * deltaKs);	// Ks * (dist - rest)

			deltaV = mass1.v - mass2.v;
			dterm = (DotProduct(deltaV,deltaP) * spring.kd ) / dist;	// Damping Term

			springForce = deltaP / dist;
			springForce *= -(hterm + dterm);
			mass1.f += springForce;
			mass2.f -= springForce;
		}

		// 考虑移动的速度
		if( m_isFirst )
		{
			m_vel = Vector3(0.0f,0.0f,0.0f);
			m_isFirst = false;
		}
		else
		{
			m_vel.x = (m_lastPos.x - worldMat._41)/deltaTime;
			m_vel.y = (m_lastPos.y - worldMat._42)/deltaTime;
			m_vel.z = (m_lastPos.z - worldMat._43)/deltaTime;

			m_vel.x *= 0.3f;
			m_vel.z *= 0.3f;
			m_vel.y *= 0.3f;

			if( m_vel.x > MAXPOSITIVEVEL )
				m_vel.x = MAXPOSITIVEVEL;
			else if( m_vel.x < MAXNEGATIVEVEL )
				m_vel.x = MAXNEGATIVEVEL;
			if( m_vel.y > MAXPOSITIVEVEL )
				m_vel.y = MAXPOSITIVEVEL;
			else if( m_vel.y < MAXNEGATIVEVEL )
				m_vel.y = MAXNEGATIVEVEL;
			if( m_vel.z > MAXPOSITIVEVEL )
				m_vel.z = MAXPOSITIVEVEL;
			else if( m_vel.z < MAXNEGATIVEVEL )
				m_vel.z = MAXNEGATIVEVEL;

			Matrix4 invMat;
			D3DXMatrixInverse( &invMat, NULL, &worldMat );
			D3DXVec3TransformNormal( &m_vel, &m_vel, &invMat );
		}
		m_lastPos.x = worldMat._41;
		m_lastPos.y = worldMat._42;
		m_lastPos.z = worldMat._43;

		// 骨骼绑定点的拖拽力
		const vector<DWORD>& tagIndex = m_pResCloth->GetTagIndex();
		for( size_t i=0; i<tagIndex.size(); i++ )
		{
			tagMass& mass = m_massArray[m_massArray.size()-tagIndex.size()+i];
			Vert_PNT& p = pVB[mass.index];

			const vector<SkinVert>& clothVerts = m_pResCloth->GetClothVerts();
			const SkinVert& clothVert = clothVerts[tagIndex[i]];
			ASSERT(clothVert.boneInfs.size()>0);

			Vector3 dragPos(0.0f,0.0f,0.0f);
			for( size_t j=0; j<clothVert.boneInfs.size(); j++ )// 骨骼绑定顶点
			{
				const tagBoneInf& inf=clothVert.boneInfs[j];
				const Matrix4& boneSpace=*(pCtrl->GetBoneSpace(inf.boneId));
				dragPos+=(inf.offset*boneSpace)*inf.weight;
			}

			deltaP = p.pos - dragPos;
			dist = Vec3Len(&deltaP);
			if( dist > 0.0f )
			{
				hterm = dist * m_dragForceKs;
				springForce = deltaP / dist;
				springForce *= -hterm;
				mass.f += springForce;
			}
		}
	}

	void AniCloth_SW::EulerIntegrate( float deltaTime )
	{
		Integrate( deltaTime );
	}

	void AniCloth_SW::MidPointIntegrate( float deltaTime )
	{
		// todo
	}

	void AniCloth_SW::RK4Integrate( float deltaTime )
	{
		// todo
	}

	void AniCloth_SW::Integrate( float deltaTime )
	{
		Vert_PNT *pVB=(Vert_PNT *)m_vertStream.GetRawBuffer();

		float deltaTimeMass;
		for( size_t i=0; i<m_massArray.size(); i++)
		{
			tagMass& mass = m_massArray[i];
			deltaTimeMass = deltaTime * mass.oneOverM;

			// 计算新的位置
			Vert_PNT& vert=pVB[mass.index];
			vert.pos += (mass.v+m_vel) * deltaTime;

			// 计算新的速度
			mass.v = mass.v + mass.f * deltaTimeMass;
		}
	}

	AniCloth_SW::ECollisionTypes AniCloth_SW::CheckForCollisions( const NodeAnimationCtrl *pCtrl )
	{
		ECollisionTypes collisionState = ECT_NOT_COLLIDING;
		float const depthEpsilon = 0.001f;

		m_contactArray.clear();

		Vert_PNT *pVB = (Vert_PNT *)m_vertStream.GetRawBuffer();

		for( size_t i = 0; i<m_massArray.size(); i++ )
		{
			tagMass& mass = m_massArray[i];

			// 与平面碰撞
			vector<tagBlock>& blocks = m_pResCloth->GetBlocks();
			for( size_t blocki=0; blocki<blocks.size(); blocki++ )
			{
				tagBlock& block = blocks[blocki];

				// v0
				Vector3 v0(0.0f,0.0f,0.0f);
				size_t numBoneInf=block.v0.boneInfs.size();
				if( numBoneInf <= 0 )
					break;

				for( size_t j=0; j<numBoneInf; j++ )
				{
					const tagBoneInf& inf=block.v0.boneInfs[j];
					const Matrix4& boneSpace=*(pCtrl->GetBoneSpace(inf.boneId));
					v0+=(inf.offset*boneSpace)*inf.weight;
				}

				// v1
				Vector3 v1(0.0f,0.0f,0.0f);
				numBoneInf=block.v1.boneInfs.size();
				if( numBoneInf <= 0 )
					break;

				for( size_t j=0; j<numBoneInf; j++ )
				{
					const tagBoneInf& inf=block.v1.boneInfs[j];
					const Matrix4& boneSpace=*(pCtrl->GetBoneSpace(inf.boneId));
					v1+=(inf.offset*boneSpace)*inf.weight;
				}

				// v2
				Vector3 v2(0.0f,0.0f,0.0f);
				numBoneInf=block.v2.boneInfs.size();
				if( numBoneInf <= 0 )
					break;

				for( size_t j=0; j<numBoneInf; j++ )
				{
					const tagBoneInf& inf=block.v2.boneInfs[j];
					const Matrix4& boneSpace=*(pCtrl->GetBoneSpace(inf.boneId));
					v2+=(inf.offset*boneSpace)*inf.weight;
				}

				Vector3 normal;
				D3DXVec3Cross( &normal, &(v2-v1), &(v1-v0) );
				D3DXVec3Normalize( &normal, &normal );

				float d = DotProduct(normal,pVB[mass.index].pos - v0);
				if( d < depthEpsilon )
				{
					float relativeVelocity = DotProduct(normal,mass.v);

					if(relativeVelocity < 0.0f)
					{
						collisionState = ECT_COLLIDING;

						tagContact contact;
						contact.index = i;
						contact.normal = normal;
						m_contactArray.push_back(contact);
					}
				}
			}
		}

		return collisionState;
	}

	void AniCloth_SW::ResolveCollisions(void)
	{
		float	VdotN;
		Vector3	Vn,Vt;

		for (size_t i=0; i<m_contactArray.size(); i++)
		{
			tagContact& contact = m_contactArray[i];
			tagMass& mass = m_massArray[contact.index];

			VdotN = DotProduct(contact.normal,mass.v);
			Vn = contact.normal * VdotN;

			Vt = mass.v - Vn;
			Vn *= m_kr;

			mass.v = Vt - Vn;
		}
	}
}//namespace Cool3D