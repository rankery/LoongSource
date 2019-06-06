#pragma once

#include "MathBaseDefine.h"
namespace Cool3D
{
	//--所有的顶点格式都必须把Position放在第一个位置
	enum EVertType
	{
		EVType_None,
		EVType_PNT,
		EVType_PT2,
		EVType_PD,
		EVType_PNT2,
		EVType_PN,
		EVType_PTp,
		EVType_PTD,
		EVType_PSprite,
		EVType_PtT,
		EVType_PtD,
		EVType_PtT4,
		EVType_PtDT,
		EVType_PtDT2,
		EVType_Water,
		EVType_Trn4,
		EVType_T4,
		EVType_Trn2,
		EVType_T2,
		EVType_PT,
		EVType_PTpD,
		EVType_P,
		EVType_PNDT,
		EVType_PNGT,
		EVType_PNWIT,
		EVType_PNGWIT,
		EVType_NT,
		EVType_Num,
	};

	struct Vert_PNT
	{
		Vector3 pos;
		Vector3 normal;
		Vector2 uv;
	};

	struct Vert_PT2
	{
		Vector3 pos;
		Vector2 uv;
		Vector2 uv2;
	};

	struct Vert_PD
	{
		Vector3 pos;
		DWORD	diffuse;
	};

	struct Vert_PT
	{
		Vector3 pos;
		Vector2 uv;
	}
	;
	struct Vert_PNT2
	{
		Vector3 pos;
		Vector3 normal;
		Vector2 uv;
		Vector2 uv2;
	};

	struct Vert_PN
	{
		Vector3 pos;
		Vector3 normal;
	};

	const int TRN_MAX_UV_SET=4;
	struct Vert_T4
	{
		Vector2 uv[TRN_MAX_UV_SET];
	};

	struct Vert_T2
	{
		Vector2 uv[2];
	};

	/** 主要用于projector*/
	struct Vert_PTp
	{
		Vector3 pos;
		Vector3 uvw;	//uv+透视校正因子
	};

	struct Vert_PTpD
	{
		Vector3 pos;
		DWORD diffuse;
		Vector3 uvw;	//uv+透视校正因子
	};

	struct Vert_PTD//使用顶点颜色和贴图坐标,主要是特效用
	{
		Vector3 pos;
		DWORD	diffuse;
		Vector2 uv;
	};

	struct Vert_PSprite	//point sprite
	{
		Vector3	pos;
		float	size;
		DWORD	diffuse;
	};

	struct Vert_PtT
	{
		Vector4 pos;	//经过转换的位置(屏幕坐标)
		Vector2 uv;
	};

	struct Vert_PtD
	{
		Vector4 pos;	//经过转换的位置(屏幕坐标)
		DWORD	diffuse;
	};

	struct Vert_PtT4
	{
		Vector4 pos;
		Vector2 uv;
		Vector2 uv2;
		Vector2 uv3;
		Vector2	uv4;
	};

	struct Vert_PtDT
	{
		Vector4 pos;
		DWORD	diffuse;
		Vector2 uv;
	};

	struct Vert_PtDT2
	{
		Vector4 pos;
		DWORD	diffuse;
		Vector2 uv;
		Vector2 uv2;
	};


	struct Vert_P
	{
		Vector3 pos;
	};

	struct Vert_PNDT
	{
		Vector3 pos;
		Vector3 normal;
		DWORD	diffuse;
		Vector2 uv;
	};

	struct Vert_PNGT
	{
		Vector3 pos;
		Vector3 normal;
		Vector3	tangent;
		Vector2 uv;
	};

	struct Vert_PNWIT
	{
		Vector3 pos;
		Vector3 normal;
		Vector4 blendWeight;
		DWORD	blendIndices;
		Vector2 uv;
	};

	struct Vert_PNGWIT
	{
		Vector3 pos;
		Vector3 normal;
		Vector3	tangent;
		Vector4 blendWeight;
		DWORD	blendIndices;
		Vector2 uv;
	};

	struct Vert_NT
	{
		Vector3 normal;
		Vector2	uv;
	};

	inline UINT GetVertSize(EVertType type)
	{
		switch(type)
		{
		case EVType_PNT:	return sizeof(Vert_PNT);
		case EVType_PT2:	return sizeof(Vert_PT2);
		case EVType_PD:		return sizeof(Vert_PD);
		case EVType_PNT2:	return sizeof(Vert_PNT2);
		case EVType_PN:		return sizeof(Vert_PN);
		case EVType_PTp:	return sizeof(Vert_PTp);
		case EVType_PTD:	return sizeof(Vert_PTD);
		case EVType_PSprite: return sizeof(Vert_PSprite);
		case EVType_PtT:	return sizeof(Vert_PtT);
		case EVType_PtD:	return sizeof(Vert_PtD);
		case EVType_PtT4:	return sizeof(Vert_PtT4);
		case EVType_PtDT:	return sizeof(Vert_PtDT);
		case EVType_PtDT2:  return sizeof(Vert_PtDT2);
		case EVType_T4:		return sizeof(Vert_T4);
		case EVType_T2:		return sizeof(Vert_T2);
		case EVType_PT:		return sizeof(Vert_PT);
		case EVType_PTpD:	return sizeof(Vert_PTpD);
		case EVType_P:		return sizeof(Vert_P);
		case EVType_PNDT:	return sizeof(Vert_PNDT);
		case EVType_PNGT:	return sizeof(Vert_PNGT);
		case EVType_PNWIT:	return sizeof(Vert_PNWIT);
		case EVType_PNGWIT:	return sizeof(Vert_PNGWIT);
		case EVType_NT:		return sizeof(Vert_NT);
		}

		return 0;
	}


	const UINT EDGE_MAX_FACE=32;
	struct FaceEdge
	{
		WORD vert[2];	//组成此Edge的两个顶点的Index
		WORD face[EDGE_MAX_FACE];	//共享此Edge的两个Face
		FaceEdge()
		{
			memset(this,0xFF,sizeof(FaceEdge));
		}
	};
	struct FaceTopo
	{
		FaceEdge edges[3];
		float	plane[4];
	};

	enum{MAX_BONE_MATRIX=80};
	struct tagBoneMatrixArray
	{
		int      nNumMat;
		Matrix4  matBone[MAX_BONE_MATRIX];
		tagBoneMatrixArray() : nNumMat( 0 ) {}
	};
	
}//endof namespace Cool3D