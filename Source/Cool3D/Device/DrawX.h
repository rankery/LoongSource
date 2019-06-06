#pragma once
#include "..\Public\VertexDefine.h"
#include "..\Math\GeomDefine.h"
#include "..\Cool3D.h"
#include "IDevice.h"	

namespace Cool3D
{
	class VertStream;
	class IStreamDefine;
	class IMaterial;
	class Cool3D_API DrawX
	{
	public:
		DrawX(void);
		~DrawX(void);

		/**	画一个以坐标原点为球心半径为1的球		*/
		static void DrawSphere();
		/**	简单的画出一个字符串		*/
		static void DrawDebugString(const TCHAR* szString,int x,int y,DWORD color=0xFFFFFFFF);
		/**	使用3D坐标在3D空间中画出一个矩形		*/
		static void Draw3DQuad(const Vert_PD* pVert);
		/**	画出一个AABBox		*/
		static void DrawAABBox(const AABBox& box,DWORD color=0xFFFF0000);
		/**	使用3D坐标在3D空间中画出一条线段		*/
		static void Draw3DLine(const Vector3& p1,const Vector3& p2,DWORD color);
		/**	使用屏幕象素坐标在画一个2D的Quad		*/
		static void Draw2DQuad(int x,int y,int width,int height,bool bLine=true,DWORD color=0xFFFFFFFF);
		/**	使用屏幕象素坐标在画一他条线		*/
		static void Draw2DLine(int sx,int sy,int ex,int ey,DWORD color=0xFFFFFFFF);
		/** 使用多个屏幕象素坐标画基本图元			*/
		static void Draw2DPrimitiveUP(EDrawType eType, LPPOINT lpPoints, int nVerCount, DWORD color=0xFFFFFFFF);

		static void BeginMtl();
		static void EndMtl();
	private:
		static void CreateDefaultMtl();
	private:
		static void SetStreamDefine(int type=0/*0:3D,1:2D*/);
		static IStreamDefine *m_pVertDef;
		static IStreamDefine *m_pVertDef2D;
		static VertStream	*m_pSphereVerts;
		static IMaterial	*m_pMtl3D;
	};
}//namespace Cool3D