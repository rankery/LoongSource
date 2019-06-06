#pragma once
#include "..\Math\Math.h"

namespace Cool3D
{
	struct tagEftQuad
	{
		Vector3 pos;
		Vector3 rot;//公告板只使用rot.x
		Vector2	halfSize;
		DWORD	color;

		Vector2	sUV;
		Vector2 eUV;
	};

	class DynamicMesh;
	class CameraBase;
	void CopyQuadArrayToMesh(DynamicMesh *pMesh,tagEftQuad *pQuads,int numQuad,CameraBase *pCamera,const Matrix4& worldMat,int faceType);
}//namespace Cool3D