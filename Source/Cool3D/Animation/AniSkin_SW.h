#pragma once
#include "..\ResSys\VertStream.h"
#include <vector>
using namespace std;

namespace Cool3D
{
	class NodeAnimationCtrl;
	class ResSkin;
	/**	\class AniSkin_SW
		\brief 使用软件计算skin的vert buffer
	*/
	class AniSkin_SW
	{
		enum{NUM_LOD=2};
	public:
		AniSkin_SW(void);
		virtual ~AniSkin_SW(void);

		/**	从ResSkin得到参数创建自己的内容	*/
		void Create(const ResSkin *pSkin);
		/**	使用ani cltr当前的状态来更自己的Vert stream */
		void ComputeSkinVB(const NodeAnimationCtrl *pCtrl, int lod);

		const VertStream* GetVertStream() const {	return &m_vertStream; }

	protected:
		void CreateVertStream(const ResSkin *pSkin);
		void CreateIndices(const ResSkin *pSkin);
	protected:
		ResSkin					*m_pResSkin;
		VertStream				m_vertStream;
		vector<int>				m_indices[NUM_LOD];
		int						m_numLod;
	};
}//namespace Cool3D