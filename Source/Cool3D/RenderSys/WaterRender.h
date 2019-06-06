#pragma once
#include ".\RenderSys.h"

namespace Cool3D
{
	class SceneGraph;

	/**	\class WaterRender
		\brief 河流渲染器，负责所有河流块的渲染
	*/
	class WaterRender
	{
	public:
		void Create();
		void Destroy();

		void Begin();
		bool AttachNode(const RenderItem& item);
		void End(SceneGraph *pSG, vector<RenderItem>& trnArray,const tagViewport& viewPort);

		static WaterRender* Inst();

	private:
		WaterRender();
		virtual ~WaterRender();

		bool m_bCreated;

		class Member;
		Member* m_p;
	};
}//namespace Cool3D