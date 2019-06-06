#include "..\MeshExportBaseDef.h"
#include "..\SceneWriter.h"
#include <vector>
#include <set>
#include <string>
using namespace std;
#include "AABBTreeBuilder.h"
#include "BintreeBuilder.h"

#include "..\StaticMesh\StaticMeshWriter.h"

#pragma once

namespace Cool3D
{
	class SceneMeshFile : public StaticMeshWriter  
	{
	public:
		SceneMeshFile();
		virtual ~SceneMeshFile();

		virtual bool Write(const TCHAR* szPath, INode *pNode=NULL);

	protected:
		virtual void Prepare();

	private:
		AABBTreeBuilder	m_boxTree;
		BintreeBuilder	m_bintree;
	};
	
}//namespace Cool3D