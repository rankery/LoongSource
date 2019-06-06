#include "StdAfx.h"
#include "ExportData.h"
#include "..\MaxPublic\UnicodeUtil.h"
#include "tinyxml\tinyxml.h"

ExportData::ExportData()
{

}

ExportData::~ExportData()
{

}

BOOL ExportData::LoadFSCNBatchFile( LPCSTR szBatchFile )
{
	m_exportDatas.clear();

	if( szBatchFile == NULL || !_tcscmp(szBatchFile, _T("")) ) 
		return FALSE;

	string strDoc = szBatchFile;
	TiXmlDocument doc(strDoc.c_str());
	if( !doc.LoadFile() )
		return FALSE;

	TiXmlHandle hDoc(&doc);
	TiXmlHandle hMax(0);
	TiXmlElement *pMax = hDoc.FirstChildElement("scenemesh").Element();
	while( pMax != NULL )
	{
		tagExportData tagED;
		tagED.strMaxFile = pMax->Attribute( "File" );
		tagED.strExportBasePath = pMax->Attribute( "BasePath" );
		tstring strExportMdlPath = pMax->Attribute( "MdlPath" );
		tagED.strExportMtlPath = pMax->Attribute( "MtlPath" );

		hMax = TiXmlHandle( pMax );//FirstChild().Element();
		TiXmlElement* pNode = hMax.FirstChild( "node" ).Element();
		while( pNode != NULL )
		{
			tagMaxNode tagMN;
			LPCSTR szTemp;
			int iOnOff;

			// Node名称
			szTemp = pNode->Attribute( "Name" );
			if( szTemp == NULL || strlen(szTemp) == 0 )
			{
				pNode = pNode->NextSiblingElement();
				continue;
			}
			tagMN.strNodeName = szTemp;

			// 导出名称
			szTemp = pNode->Attribute( "ExpName" );
			if( szTemp == NULL || strlen(szTemp) == 0 )
			{
				pNode = pNode->NextSiblingElement();
				continue;
			}
			tagMN.strExportPath = szTemp;
			tagMN.strExportPath = tagED.strExportBasePath + strExportMdlPath + tagMN.strExportPath;

			// 是否导出lightmap的UV通道
			tagMN.bExpLightmap = true;

			// lightmap的尺寸
			szTemp = pNode->Attribute( "LightmapSize" );
			if( szTemp == NULL || strlen(szTemp) == 0 )
			{
				pNode = pNode->NextSiblingElement();
				continue;
			}
			tagMN.lightmapSize = (float)atof(szTemp);

			// 是否导出normalmap
			szTemp = pNode->Attribute( "NormalMap" );
			if( szTemp == NULL || strlen(szTemp) == 0 )
			{
				pNode = pNode->NextSiblingElement();
				continue;
			}
			iOnOff = atoi(szTemp);
			tagMN.bExpNormalmap = iOnOff==1 ? true : false;

			// 是否导出碰撞盒
			tagMN.bExpCollideBox = true;

			// 是否导出镂空材质碰撞盒
			szTemp = pNode->Attribute( "Opcity" );
			if( szTemp == NULL || strlen(szTemp) == 0 )
			{
				pNode = pNode->NextSiblingElement();
				continue;
			}
			iOnOff = atoi(szTemp);
			tagMN.bExpOpcity = iOnOff==1 ? true : false;

			// 碰撞盒尺寸
			szTemp = pNode->Attribute( "CollideBoxSize" );
			if( szTemp == NULL || strlen(szTemp) == 0 )
			{
				pNode = pNode->NextSiblingElement();
				continue;
			}
			tagMN.CollideBoxSize = (float)atof(szTemp);

			tagED.maxNodes.push_back(tagMN);

			pNode = pNode->NextSiblingElement();
		}

		m_exportDatas.push_back(tagED);

		pMax = pMax->NextSiblingElement();
	}

	return TRUE;
}

BOOL ExportData::LoadFSMBatchFile( LPCTSTR szBatchFile )
{
	m_exportDatas.clear();

	if( szBatchFile == NULL || !_tcscmp(szBatchFile, _T("")) ) 
		return FALSE;

	string strDoc = szBatchFile;
	TiXmlDocument doc(strDoc.c_str());
	if( !doc.LoadFile() )
		return FALSE;

	TiXmlHandle hDoc(&doc);
	TiXmlHandle hMax(0);

	TiXmlElement *pMax = hDoc.FirstChildElement("staticmesh").Element();
	while( pMax != NULL )
	{
		tagExportData tagED;
		tagED.strMaxFile = pMax->Attribute( "File" );
		tagED.strExportBasePath = pMax->Attribute( "BasePath" );
		tstring strExportMdlPath = pMax->Attribute( "MdlPath" );
		tagED.strExportMtlPath = pMax->Attribute( "MtlPath" );

		hMax = TiXmlHandle( pMax );//FirstChild().Element();
		TiXmlElement* pNode = hMax.FirstChild( "node" ).Element();
		while( pNode != NULL )
		{
			tagMaxNode tagMN;
			LPCSTR szTemp;
			int iOnOff;

			// 中心点名称
			szTemp = pNode->Attribute( "OriginNode" );
			if( szTemp == NULL )
			{
				szTemp = "";
			}
			tagMN.strOriginNode = szTemp;

			// Node名称
			szTemp = pNode->Attribute( "Name" );
			if( szTemp == NULL || strlen(szTemp) == 0 )
			{
				pNode = pNode->NextSiblingElement();
				continue;
			}
			tagMN.strNodeName = szTemp;

			// 导出名称
			szTemp = pNode->Attribute( "ExpName" );
			if( szTemp == NULL || strlen(szTemp) == 0 )
			{
				pNode = pNode->NextSiblingElement();
				continue;
			}
			tagMN.strExportPath = szTemp;
			tagMN.strExportPath = tagED.strExportBasePath + strExportMdlPath + tagMN.strExportPath;

			// 是否导出lightmap的UV通道
			tagMN.bExpLightmap = true;

			// lightmap的尺寸
			szTemp = pNode->Attribute( "LightmapSize" );
			if( szTemp == NULL || strlen(szTemp) == 0 )
			{
				pNode = pNode->NextSiblingElement();
				continue;
			}
			tagMN.lightmapSize = (float)atof(szTemp);

			// 是否导出normalmap
			szTemp = pNode->Attribute( "NormalMap" );
			if( szTemp == NULL || strlen(szTemp) == 0 )
			{
				pNode = pNode->NextSiblingElement();
				continue;
			}
			iOnOff = atoi(szTemp);
			tagMN.bExpNormalmap = iOnOff==1 ? true : false;

			tagED.maxNodes.push_back(tagMN);

			pNode = pNode->NextSiblingElement();
		}

		m_exportDatas.push_back(tagED);

		pMax = pMax->NextSiblingElement();
	}

	return TRUE;
}

BOOL ExportData::LoadSkinBatchFile( LPCTSTR szBatchFile )
{
	m_exportDatas.clear();

	if( szBatchFile == NULL || !_tcscmp(szBatchFile, _T("")) ) 
		return FALSE;

	string strDoc = szBatchFile;
	TiXmlDocument doc(strDoc.c_str());
	if( !doc.LoadFile() )
		return FALSE;

	TiXmlHandle hDoc(&doc);
	TiXmlHandle hMax(0);

	TiXmlElement *pMax = hDoc.FirstChildElement("skin").Element();
	while( pMax != NULL )
	{
		tagExportData tagED;
		tagED.strMaxFile = pMax->Attribute( "File" );
		tagED.strExportBasePath = pMax->Attribute( "BasePath" );
		tstring strExportMdlPath = pMax->Attribute( "MdlPath" );
		tagED.strExportMtlPath = pMax->Attribute( "MtlPath" );

		hMax = TiXmlHandle( pMax );//FirstChild().Element();
		TiXmlElement* pNode = hMax.FirstChild( "node" ).Element();
		while( pNode != NULL )
		{
			tagMaxNode tagMN;
			LPCSTR szTemp;
			int iOnOff;

			// Node名称
			szTemp = pNode->Attribute( "Name" );
			if( szTemp == NULL || strlen(szTemp) == 0 )
			{
				pNode = pNode->NextSiblingElement();
				continue;
			}
			tagMN.strNodeName = szTemp;

			// 导出名称
			szTemp = pNode->Attribute( "ExpName" );
			if( szTemp == NULL || strlen(szTemp) == 0 )
			{
				pNode = pNode->NextSiblingElement();
				continue;
			}
			tagMN.strExportPath = szTemp;
			tagMN.strExportPath = tagED.strExportBasePath + strExportMdlPath + tagMN.strExportPath;

			// 是否导出normalmap
			szTemp = pNode->Attribute( "NormalMap" );
			if( szTemp == NULL || strlen(szTemp) == 0 )
			{
				pNode = pNode->NextSiblingElement();
				continue;
			}
			iOnOff = atoi(szTemp);
			tagMN.bExpNormalmap = iOnOff==1 ? true : false;

			// 多遍渲染
			szTemp = pNode->Attribute( "MultiPass" );
			if( szTemp == NULL || strlen(szTemp) == 0 )
			{
				pNode = pNode->NextSiblingElement();
				continue;
			}
			iOnOff = atoi(szTemp);
			tagMN.bMultiPass = iOnOff==1 ? true : false;

			tagED.maxNodes.push_back(tagMN);

			pNode = pNode->NextSiblingElement();
		}

		m_exportDatas.push_back(tagED);

		pMax = pMax->NextSiblingElement();
	}

	return TRUE;
}

BOOL ExportData::LoadSkelBatchFile( LPCTSTR szBatchFile )
{
	m_exportDatas.clear();

	if( szBatchFile == NULL || !_tcscmp(szBatchFile, _T("")) ) 
		return FALSE;

	string strDoc = szBatchFile;
	TiXmlDocument doc(strDoc.c_str());
	if( !doc.LoadFile() )
		return FALSE;

	TiXmlHandle hDoc(&doc);
	TiXmlHandle hMax(0);

	TiXmlElement *pMax = hDoc.FirstChildElement("skeleton").Element();
	while( pMax != NULL )
	{
		tagExportData tagED;
		tagED.strMaxFile = pMax->Attribute( "File" );
		tstring strExportMdlPath = pMax->Attribute( "MdlPath" );

		hMax = TiXmlHandle( pMax );//FirstChild().Element();
		TiXmlElement* pNode = hMax.FirstChild( "node" ).Element();
		while( pNode != NULL )
		{
			tagMaxNode tagMN;
			LPCSTR szTemp;

			// Node名称
			szTemp = pNode->Attribute( "ExpName" );
			if( szTemp == NULL || strlen(szTemp) == 0 )
			{
				pNode = pNode->NextSiblingElement();
				continue;
			}
			tagMN.strNodeName = szTemp;

			// 导出名称
			tagMN.strExportPath = strExportMdlPath;

			// 采样率
			szTemp = pNode->Attribute( "SampleRate" );
			if( szTemp == NULL || strlen(szTemp) == 0 )
			{
				pNode = pNode->NextSiblingElement();
				continue;
			}
			tagMN.sampleRate = atoi(szTemp);

			tagED.maxNodes.push_back(tagMN);

			pNode = pNode->NextSiblingElement();
		}

		m_exportDatas.push_back(tagED);

		pMax = pMax->NextSiblingElement();
	}

	return TRUE;
}

INT ExportData::GetMaxNum( void )
{
	return (INT)m_exportDatas.size();
}

LPCTSTR ExportData::GetMaxFile( INT index )
{
	if( index < (INT)m_exportDatas.size() )
	{
		tagExportData& exportData = m_exportDatas[index];
		return exportData.strMaxFile.c_str();
	}

	return NULL;
}

LPCTSTR ExportData::GetMaxBasePath( INT index )
{
	if( index < (INT)m_exportDatas.size() )
	{
		tagExportData& exportData = m_exportDatas[index];
		return exportData.strExportBasePath.c_str();
	}

	return NULL;
}

LPCTSTR ExportData::GetMaxMtlPath( INT index )
{
	if( index < (INT)m_exportDatas.size() )
	{
		tagExportData& exportData = m_exportDatas[index];
		return exportData.strExportMtlPath.c_str();
	}

	return NULL;
}

INT ExportData::GetNodeNum( INT maxIndex )
{
	if( maxIndex < (INT)m_exportDatas.size() )
	{
		tagExportData& exportData = m_exportDatas[maxIndex];
		return (INT)exportData.maxNodes.size();
	}

	return 0;
}

const tagMaxNode* ExportData::GetNode( INT maxIndex,INT nodeIndex )
{
	if( maxIndex < (INT)m_exportDatas.size() )
	{
		tagExportData& exportData = m_exportDatas[maxIndex];
		if( nodeIndex < (INT)exportData.maxNodes.size() )
		{
			return &(exportData.maxNodes[nodeIndex]);
		}
	}

	return NULL;
}