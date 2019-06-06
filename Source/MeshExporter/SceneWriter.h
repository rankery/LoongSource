#include "SceneContext.h"
#include "ExpMesh.h"
#include "MeshExportBaseDef.h"
#pragma once

#include <string>
using namespace std;
#include "..\MaxPublic\subMesh.h"

namespace Cool3D
{
	/**	\class SceneWriter
		\brief abstract Strategy of writing, 也结合Builder模式, 提供基本部件的写入功能,供派生类重用.
	*/
	class SceneWriter  
	{
	public:
		virtual bool Write(const TCHAR* szPath, INode *pNode=NULL)=0;
		SceneWriter();
		virtual ~SceneWriter();
		EVertType GetVertType() const { return m_vertType; }
		void SetVertType(EVertType vertType) { m_vertType=vertType; }
		
	protected:
		EVertType	m_vertType;
		/**	把SceneContext中的Mtl写入文件
			\return Mtl的个数
		*/
		int WriteMtlFromSceneContext(FILE *fp, const TCHAR* szFilePath);
		/**	把一个ExpMesh对象中的VertBuf拷贝到Out中
		*/
		void GetVert(ExpMesh *pMesh, std::vector<BYTE>& out);
		/**	把一个ExpMesh对象中的所有SubMesh拷贝到Out中
		*/
		void GetSubMeshs(WORD baseIndex,WORD numVert,ExpMesh* pMesh, std::vector<SubMesh>& out);
		void BuildLodSubMesh(const SubMesh& src,LodSubMesh& out);
		virtual void Prepare();
		void GetPos(BYTE *m_pBuf, UINT i,Vector3& out);
	};

	SceneWriter *GreateWriter(const TCHAR* szPath);	
}