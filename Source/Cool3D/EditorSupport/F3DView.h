
#pragma once
#include "..\F3D.h"
#include "..\Math\CameraEuler.h"
namespace F3D
{
	class NullSceneGraph;
	class EntityNode;

	/**	\class F3DView
		\brief 简单的模型浏览,可以用鼠标控制观察角度
	*/
	class F3D_API F3DView
	{
	public:
		F3DView(void);
		virtual ~F3DView(void);

		/**	渲染到指定的窗口
			\param hWnd,目标窗口
			\param bAdaptive,是否使得渲染图像充满整个窗口
		*/
		void Render(HWND hWnd,bool bAdaptive=true);
		/**	加载需要观察的模型
		*/
		virtual void LoadMdl(const char* szMdlName);

		//--操作控制
		void OnLButtonDown(UINT nFlags, POINT point);
		void OnLButtonUp(UINT nFlags, POINT point);
		void OnMouseMove(UINT nFlags, POINT point);
		void OnRButtonDown(UINT nFlags, POINT point);
		void OnRButtonUp(UINT nFlags, POINT point);
		void OnMouseWheel(UINT nFlags, short zDelta, POINT pt);
	protected:
		void Begin();
		virtual void RenderView(bool bAdaptive);
		void End(HWND hWnd);
	protected:
		DWORD			m_lastRender;	//使用自主的计时器
		NullSceneGraph	*m_pSG;
		EntityNode		*m_pEntity;
		CameraEuler		m_camera;

		//--ctrl
		bool	m_bDrag,
				m_bRightDrag;
		POINT	m_DragPt;
	};
}//namespace F3D