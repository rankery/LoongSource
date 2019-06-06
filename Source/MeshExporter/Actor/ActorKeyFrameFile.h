#pragma once

#include "..\MeshExportBaseDef.h"
#include "..\SceneWriter.h"
#include <vector>
#include <set>
#include <string>
using namespace std;
#include "..\..\MaxPublic\AKFile.h"
#include "..\..\MaxPublic\UnicodeUtil.h"
#include "..\SceneMesh\OctreeBuilder.h"

namespace Cool3D
{
	/**	\class ActorKeyFrameFile
		\brief 建立.AK文件或者把动作融合进已有的.AK文件
	*/
	class ActorKeyFrameFile : public SceneWriter
	{
	public:
		bool Write(const char* szPath, INode *pNode=NULL);
		void MergeActionFromSceneContext(DWORD action, INode *pParent, TimeValue start, TimeValue end, int sampleRate);
		bool PreLoad(const char* szPath);
		ActorKeyFrameFile();
		virtual ~ActorKeyFrameFile();

		void GetTrackSet(set<tstring>& out);
		void RemoveAction(DWORD action);
		void SetExportFrame(bool first) { m_bExportFirst=first; }
	private:
		void GetSubMeshs();
		void AddFrameFromSceneContext(DWORD action, INode *pParent, TimeValue time, float frmTime);
		void GetBox(VertFrame *pFrm,Vector3& max,Vector3& min);
		void Prepare();
	private:
		VertFrame* LoadFrame(FILE *fp, UINT numVert, const VertFrame* preFrame);
		std::vector<LodSubMesh> m_subMeshs;
		std::vector<VertFrame*>	m_frameBuf;
		OctreeBuilder m_octree;

		int		m_firstFrame;		// 动作的第一帧序列号
		int		m_lastFrame;		// 动作的第二帧序列号
		bool	m_bExportFirst;		// 导出动作的第一帧的Nav
	};
}//namespace Cool3D