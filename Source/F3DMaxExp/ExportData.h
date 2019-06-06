#pragma once

#include "F3DMaxExp.h"
#include "..\MaxPublic\UnicodeUtil.h"

struct F3D_MAX_API tagMaxNode
{
	tstring strOriginNode;
	tstring strNodeName;
	tstring strExportPath;
	bool	bExpLightmap;
	float	lightmapSize;
	bool	bExpNormalmap;
	bool	bExpCollideBox;
	bool	bExpOpcity;
	float	CollideBoxSize;
	bool	bMultiPass;
	int		sampleRate;
};

class F3D_MAX_API ExportData
{
public:
	ExportData();
	~ExportData();

	BOOL	LoadFSCNBatchFile(LPCSTR szBatchFile);
	BOOL	LoadFSMBatchFile(LPCTSTR szBatchFile);
	BOOL	LoadSkinBatchFile(LPCTSTR szBatchFile);
	BOOL	LoadSkelBatchFile(LPCTSTR szBatchFile);

	INT		GetMaxNum(void);
	LPCTSTR	GetMaxFile(INT index);
	LPCTSTR GetMaxBasePath(INT index);
	LPCTSTR GetMaxMtlPath(INT index);
	INT		GetNodeNum(INT maxIndex);
	const tagMaxNode* GetNode(INT maxIndex,INT nodeIndex);

private:
	struct tagExportData
	{
		tstring	strMaxFile;
		tstring strExportBasePath;
		tstring	strExportMtlPath;
		std::vector<tagMaxNode>	maxNodes;
	};

	typedef std::vector<tagExportData> ExportDatas;
	ExportDatas	m_exportDatas;
};
