#include "StdAfx.h"
#include ".\eftquaddef.h"
#include "..\RenderSys\DynamicMesh.h"
#include "..\Math\Transform.h"
#include "..\Math\CameraBase.h"

namespace Cool3D
{
	void CopyQuadArrayToMesh(DynamicMesh *pMesh,tagEftQuad *pQuads,int numQuad,
		CameraBase *pCamera,const Matrix4& worldMat,int faceType)
	{
		VertStream *pVStream=pMesh->GetVertStream();
		if(pVStream == NULL)
			return;

		ASSERT(pVStream);
		ASSERT(pVStream->GetNumVert()==numQuad*6);

		Vert_PTD *pVert=(Vert_PTD *)pVStream->GetRawBuffer();
		int vertIndex=0;
		int i;
		Vert_PTD quadVert[4];
		
		if(faceType!=0)
		{
			Matrix4 mdlView=worldMat;
			mdlView*=(*pCamera->GetView());

			Vector3 viewRight(mdlView._11,mdlView._21,mdlView._31);
			Vector3 viewUp(mdlView._12,mdlView._22,mdlView._32);
			Vector3 viewForward(mdlView._13,mdlView._23,mdlView._33);
			Vec3Normalize( viewRight );
			Vec3Normalize( viewUp );
			Vec3Normalize( viewForward );
			for(i=0;i<numQuad;i++)
			{
				Vector3 xaxis=viewRight;
				Vector3 yaxis=MathConst::AxisY;
				if(faceType==1)
					yaxis=viewUp;

				Transform trans;
				trans.RotateAxis(viewForward,pQuads[i].rot.x);
				trans.TransVec3(xaxis);
				trans.TransVec3(yaxis);

				xaxis*=pQuads[i].halfSize.x;
				yaxis*=pQuads[i].halfSize.y;

				//--
				DWORD tmpC=pQuads[i].color;

				quadVert[0].pos=pQuads[i].pos+ xaxis + yaxis;
				quadVert[0].diffuse=tmpC;

				quadVert[1].pos=pQuads[i].pos+xaxis - yaxis;
				quadVert[1].diffuse=tmpC;

				quadVert[2].pos=pQuads[i].pos- xaxis - yaxis;
				quadVert[2].diffuse=tmpC;

				quadVert[3].pos=pQuads[i].pos- xaxis + yaxis;
				quadVert[3].diffuse=tmpC;

				quadVert[0].uv=Vector2(pQuads[i].sUV.x,pQuads[i].sUV.y);
				quadVert[1].uv=Vector2(pQuads[i].sUV.x,pQuads[i].eUV.y);
				quadVert[2].uv=Vector2(pQuads[i].eUV.x,pQuads[i].eUV.y);
				quadVert[3].uv=Vector2(pQuads[i].eUV.x,pQuads[i].sUV.y);

				pVert[vertIndex++]=quadVert[0];
				pVert[vertIndex++]=quadVert[1];
				pVert[vertIndex++]=quadVert[2];

				pVert[vertIndex++]=quadVert[2];
				pVert[vertIndex++]=quadVert[3];
				pVert[vertIndex++]=quadVert[0];
			}
		}//endof if
		else
		{
			Transform trans(Transform::ECM_ToRight);
			for(i=0;i<numQuad;i++)
			{
				trans.LoadIdentity();
				trans.Rotate(pQuads[i].rot.y,pQuads[i].rot.x,pQuads[i].rot.z);
				trans.Translate(pQuads[i].pos);

				//假定原始Quad是在XY面,计算出经过自转+平移的Quad
				quadVert[0].pos=Vector3(-pQuads[i].halfSize.x,pQuads[i].halfSize.y,0);
				quadVert[1].pos=Vector3(-pQuads[i].halfSize.x,-pQuads[i].halfSize.y,0);
				quadVert[2].pos=Vector3(pQuads[i].halfSize.x,-pQuads[i].halfSize.y,0);
				quadVert[3].pos=Vector3(pQuads[i].halfSize.x,pQuads[i].halfSize.y,0);
				trans.TransVec3(quadVert[0].pos);
				trans.TransVec3(quadVert[1].pos);
				trans.TransVec3(quadVert[2].pos);
				trans.TransVec3(quadVert[3].pos);

				//--填充颜色
				Color4ub tmpC(pQuads[i].color);
				DWORD c=tmpC;

				quadVert[0].diffuse=c;
				quadVert[1].diffuse=c;
				quadVert[2].diffuse=c;
				quadVert[3].diffuse=c;

				quadVert[0].uv=Vector2(pQuads[i].sUV.x,pQuads[i].sUV.y);
				quadVert[1].uv=Vector2(pQuads[i].sUV.x,pQuads[i].eUV.y);
				quadVert[2].uv=Vector2(pQuads[i].eUV.x,pQuads[i].eUV.y);
				quadVert[3].uv=Vector2(pQuads[i].eUV.x,pQuads[i].sUV.y);


				pVert[vertIndex++]=quadVert[0];
				pVert[vertIndex++]=quadVert[1];
				pVert[vertIndex++]=quadVert[2];

				pVert[vertIndex++]=quadVert[2];
				pVert[vertIndex++]=quadVert[3];
				pVert[vertIndex++]=quadVert[0];
			}//endof for
		}//endof else

		pMesh->SetDrawLimit(vertIndex/3);
	}
}//namespace Cool3D