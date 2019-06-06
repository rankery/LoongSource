
#pragma once
#include "..\Cool3D.h"
#include "..\Math\CameraEuler.h"
#include "..\Math\Color.h"
namespace Cool3D
{
	class NullSceneGraph;
	class EntityNode;

	/**	\class SimpleView
		\brief 简单的模型浏览,可以用鼠标控制观察角度
	*/
	class Cool3D_API SimpleView
	{
	public:
		SimpleView(void);
		virtual ~SimpleView(void);

		/**	渲染到指定的窗口
			\param hWnd,目标窗口
			\param bAdaptive,是否使得渲染图像充满整个窗口
		*/
		void Render(HWND hWnd,bool bAdaptive=true);
		/**	加载需要观察的模型
		*/
		virtual void LoadMdl(const TCHAR* szMdlName);

		//--操作控制
		void OnLButtonDown(UINT nFlags, POINT point);
		void OnLButtonUp(UINT nFlags, POINT point);
		void OnMouseMove(UINT nFlags, POINT point);
		void OnRButtonDown(UINT nFlags, POINT point);
		void OnRButtonUp(UINT nFlags, POINT point);
		void OnMouseWheel(UINT nFlags, short zDelta, POINT pt);

		EntityNode* GetEntityNode() {	return m_pEntity;}
		CameraEuler* GetCamera()	{	return &m_camera;}
		void SetBkColor(Color4ub bk)	{	m_bkColor=bk;}
	protected:
		void Begin();
		virtual void RenderView(bool bAdaptive);
		void End(HWND hWnd);
	protected:
		DWORD			m_lastRender;	//使用自主的计时器
		NullSceneGraph	*m_pSG;
		EntityNode		*m_pEntity;
		CameraEuler		m_camera;
		Color4ub		m_bkColor;

		//--ctrl
		bool	m_bDrag,
				m_bRightDrag;
		POINT	m_DragPt;
	};
}//namespace Cool3D