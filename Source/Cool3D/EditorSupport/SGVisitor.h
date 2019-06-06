#pragma once
#include "..\Cool3D.h"
#include "..\Math\math.h"

namespace Cool3D
{
	class HugeQuadTreeSG;
	class ISGVisitorCallBack;
	class CameraBase;
	/** \class SGVisitor
		\brief SceneGraph±éÀúÆ÷
	*/
	class Cool3D_API SGVisitor
	{
	public:
		SGVisitor(void);
		virtual ~SGVisitor(void);

		virtual bool Start(const TCHAR* szSGFile,float visitorViewDist,float renderViewDist,ISGVisitorCallBack* pCallBack,DWORD dwParam);

	protected:
		void SetCameraLookAt(HugeQuadTreeSG& sg,CameraBase& camera,Vector3& lookAt, bool bWait);
	};




}//namespace Cool3D