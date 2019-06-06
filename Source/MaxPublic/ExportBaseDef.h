#pragma warning(disable:4251)//Wraning of D3DXVECTOR3 etc need dll export

#include "..\MaxPublic\UnicodeUtil.h"
#include "MathBaseDefine.h"
#include "VertexDefine.h"
#include "MtlDesc.h"

#include <vector>
#include <windows.h>
//--一些输出插件定义的东西, 外部也可能会用到的
#pragma once

namespace Cool3D
{
	//--Light-----------------------------------
	enum LIGHT_TYPE
	{
		LGT_OMNI,
		LGT_SPOT,
		LGT_DIRECT
	};
	enum DECAY_TYPE
	{
		DK_DECAY_NONE,
		DK_DECAY_DIST,
		DK_DECAY_SQUARE
	};
	enum LIGHT_SHAPE
	{
		SHAPE_CONE,
		SHAPE_SQUARE
	};
	
	struct LightDesc
	{
		LIGHT_TYPE m_LightType;
		bool  m_bShadow;  //whether to project shadow
		Vector3 m_Diffuse;
		Vector3 m_Specular;
		Vector3 m_Ambient;
		float m_NearStart;
		float m_NearEnd;
		float m_FarStart;
		float m_FarEnd;
		
		DECAY_TYPE  m_DecayType;
		float m_DecayRadius;
		
		LIGHT_SHAPE m_LightShape;
		float m_HotSize;	//in degrees with spot light,  in units with direct light
		float m_Falloff;	//in degrees,  in units with direct light
		float m_Aspect;		//only useful when lightshape is square
		
		Vector3	m_Position;//for Omni and spot
		Vector3 m_Direction;//for Direct and spot
	};
		
	struct TriFace
	{
		WORD index[3];
	};


	struct MeshTopo
	{
		std::vector<FaceTopo>	faceArray;

		MeshTopo()
		{}
		MeshTopo(const MeshTopo& other)
		{
			faceArray=other.faceArray;
		}
		const MeshTopo& operator = (const MeshTopo& other)
		{
			if(this == &other)
				return *this;
			faceArray=other.faceArray;
			return *this;
		}
	public:
		void ComputeEdgeConnect(const Vert_PNT *pVB);
	};

	struct MeshFaceGroup
	{
		WORD	baseIndex;
		DWORD	mtlID;
		WORD	numVert;
		DWORD	numFaces;
		Vector3 max;
		Vector3 min;

		std::vector<TriFace>	faces;

		MeshTopo topo;//不使用baseIndex
		
		MeshFaceGroup()
		{ baseIndex=mtlID=0;}
		
		void Clone(const MeshFaceGroup& other)
		{
			baseIndex=other.baseIndex;
			mtlID=other.mtlID;
			faces=other.faces;
			numVert=other.numVert;
			numFaces=other.numFaces;
			topo=other.topo;
		}
		
		MeshFaceGroup(const MeshFaceGroup& other)
		{
			Clone(other);
		}
		const MeshFaceGroup& operator = (const MeshFaceGroup& other)
		{
			if(this == &other)
				return *this;
			Clone(other);
			return *this;
		}
	};

	//--关键帧动画中的一帧
	struct VertFrame
	{
		DWORD name;
		float time;//秒
		std::vector<Vert_PNT> vertices;

		VertFrame()
		{}
		VertFrame(const VertFrame& other)
		{
			name=other.name;
			vertices=other.vertices;
		}
		const VertFrame& operator = (const VertFrame& other)
		{
			if(this == &other)
				return *this;
			name=other.name;
			vertices=other.vertices;
			return *this;
		}
	};
}//namespace Cool3D