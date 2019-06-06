//-----------------------------------------------------------------------------
//!\file ini_obj.cpp
//!\brief ini文件对象
//!
//!\date 2004-04-03
//! last 2004-04-03
//!\author Lyp
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "ini_obj.h"

// 使用其它部件
#include "..\vfs\vfs.h"
#include "var_container.h"

namespace vEngine {
//-----------------------------------------------------------------------------
//!	construction
//-----------------------------------------------------------------------------
INIObj::INIObj()
{
	m_mapOffset.clear();
	m_mapSectionName.clear();
	m_pData = NULL;
	m_dwDataSize = 0;
}

//-----------------------------------------------------------------------------
//!	destruction
//-----------------------------------------------------------------------------
INIObj::~INIObj()
{
	Unload();
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BOOL INIObj::Load( LPCSTR szVFSName, LPCTSTR szFileName, const INT nMethod)
{
	if( m_pData )
		Unload();

	m_pVFS = szVFSName;

	// 使用文件系统将INI一次性读入
	m_dwDataSize = m_pVFS->GetSize(szFileName);
	if( GT_INVALID == m_dwDataSize )
		return FALSE;

	m_pData = (BYTE*)malloc(m_dwDataSize+sizeof(TCHAR)*2);
	if( NULL == m_pData )
		return FALSE;

	m_dwDataSize = m_pVFS->LoadFile(m_pData, szFileName);
	if( GT_INVALID == m_dwDataSize )
		return FALSE;

	// 对于加密的ini文件，前四个字节为密钥值，所以需要用密钥进行一次解密
	if( GT_VALID(nMethod) )
	{
		ASSERT(1 == nMethod || 0 == nMethod);
		m_pUtil->Decrypt(m_pData+sizeof(INT), m_pData, m_dwDataSize-sizeof(INT), *(INT*)m_pData, nMethod);
		*(DWORD*)( m_pData + m_dwDataSize - sizeof(TCHAR)*2) = 0x0a000d00; // little-endian	
	}

	// 在文件最后添加回车换行符(很多文件都不是以0d0a结束)
	*(DWORD*)( m_pData + m_dwDataSize ) = 0x0a000d00; // little-endian

	// 下面开始分析整个INI,记录下所有键和键值的偏移,并存入m_mapOffset
	std::map<DWORD, VEC_DWORD> mapOneSection;
	tstring strTemp, strSectionName;
	INT nSectionOffset = GT_INVALID, nValueOffset = GT_INVALID, nKeyOffset = 0; 
	INT	nKeySize = 0;
	INT nCommentStart = GT_INVALID;
	TCHAR* pChar = (TCHAR*)m_pData;

	for( DWORD n=0; n<(m_dwDataSize+4)/sizeof(TCHAR); n++ )
	{
		switch( *(pChar+n) )
		{
		case _T('['):	// 区域信息开始
			if( GT_INVALID != nCommentStart )
				continue;
			if( false == strSectionName.empty() )
			{
				// 把上一区域的信息保存下来
				DWORD dwID = m_pUtil->Crc32( strSectionName.c_str() );
				// 看有没有重复的ID(几率非常非常小，不过为保险起见，还是检查一下)
				ASSERT( m_mapSectionName.find( dwID ) == m_mapSectionName.end() );
				m_mapSectionName.insert(
					std::pair<DWORD,tstring>(dwID, strSectionName) );
				m_mapOffset.insert( 
					std::pair<DWORD, MAP_VEC_DWORD>(dwID, mapOneSection) );
				mapOneSection.clear();
			}
			nSectionOffset = n+1;
			nKeyOffset = GT_INVALID;
			nValueOffset = GT_INVALID;
			break;

		case _T(']'):	// 区域信息结束
			if( GT_INVALID != nCommentStart )
				continue;

			// 记录区域的名字
			if( nSectionOffset != GT_INVALID && n - nSectionOffset > 0 )
				strSectionName.assign(pChar+nSectionOffset,n-nSectionOffset);

			nSectionOffset = GT_INVALID;
			break;

		case _T('='):	// 键与值的间隔
			if( GT_INVALID != nCommentStart )
				continue;
			if( GT_INVALID != nKeyOffset )
			{
				if( n - nKeyOffset > 0 )	// key的长度不能为零
				{
					nKeySize = n - nKeyOffset;
					nValueOffset = n+1;
				}
				else
					nKeyOffset = GT_INVALID;
			}
			break;

		case _T(';'):	// 整行注释
			nCommentStart = n;
			break;

		case 0x0a:	// 行尾
			if( GT_INVALID != nKeyOffset && GT_INVALID != nValueOffset )
			{
				// 记录此行的键和值的偏移量
				std::vector<DWORD> vecTemp;	// 存储一行中的所有单元格的偏移
				vecTemp.push_back( nKeyOffset );
				vecTemp.push_back( nKeySize );
				vecTemp.push_back( nValueOffset );
				if( GT_INVALID != nCommentStart )
					vecTemp.push_back( nCommentStart - nValueOffset );
				else
					vecTemp.push_back( n-1 - nValueOffset );
				// 键
				strTemp.assign(pChar+nKeyOffset, nKeySize);
				// tab换成空格
				while( strTemp.find(_T('\t')) != strTemp.npos )
					strTemp.replace(strTemp.find(_T('\t')), 1, 1, _T(' '));
				// 去掉头尾的空格并算出id
				if( strTemp.find_first_not_of(_T(' ')) != -1 )
					strTemp.assign(strTemp,strTemp.find_first_not_of(_T(' ')),
						strTemp.find_last_not_of(_T(' '))-strTemp.find_first_not_of(_T(' '))+1);
				DWORD dwID = m_pUtil->Crc32((LPCTSTR)strTemp.c_str());
				// 看有没有重复的ID(几率非常非常小，不过为保险起见，还是检查一下)
				ASSERT( mapOneSection.find( dwID ) == mapOneSection.end() );
				mapOneSection.insert(std::pair<DWORD, VEC_DWORD>(dwID, vecTemp));
			}

			nSectionOffset = nValueOffset = GT_INVALID;
			nKeyOffset = n+1;
			nCommentStart = GT_INVALID;
			break;
		}
	}

	if( false == strSectionName.empty() )
	{
		// 把上一区域的信息保存下来
		DWORD dwID = m_pUtil->Crc32( strSectionName.c_str() );
		m_mapSectionName.insert(std::pair<DWORD,tstring>(dwID, strSectionName));
		m_mapOffset.insert(std::pair<DWORD, MAP_VEC_DWORD>(dwID, mapOneSection));
	}

	return TRUE;
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
VOID INIObj::Unload()
{
	free(m_pData);

	m_mapOffset.clear();
	m_mapSectionName.clear();
	m_pData = NULL;
	m_dwDataSize = 0;
}


//-----------------------------------------------------------------------------
//!\param dwSize 返回value数据大小
//!\return 指向value数据的PBYTE指针
//-----------------------------------------------------------------------------
PBYTE INIObj::Read(DWORD& dwSize, LPCTSTR szKeyName, LPCTSTR szSectionName)
{
	ASSERT( szKeyName );
	ASSERT( szSectionName );

	if( NULL == m_pData )
		return NULL;

	std::map<DWORD, MAP_VEC_DWORD>::iterator it = m_mapOffset.begin();
	it = m_mapOffset.find(m_pUtil->Crc32(szSectionName));
	if( it == m_mapOffset.end() )
		return NULL;

	std::map<DWORD, VEC_DWORD>::iterator real_it;
	real_it = it->second.find(m_pUtil->Crc32(szKeyName));
	if( real_it == it->second.end() )
			return NULL;

	if( real_it->second.size() < 4 )	// 错误的数据
		return NULL;

	// (real_it->second)[0]: key name offset
	// (real_it->second)[1]: key name size
	dwSize = (real_it->second)[3];	// (real_it->second)[3]: value size
	return (m_pData+(real_it->second)[2]); // (real_it->second)[2]: value offset
}


//-----------------------------------------------------------------------------
//! 将数据传向容器
//!\param 容器的引用
//!\return 是否成功
//-----------------------------------------------------------------------------
BOOL INIObj::PutToContainer(VarContainer* pVarContainer)
{
	if( !pVarContainer )
		return FALSE;

	INT nTemp = 0;
	DWORD dwKeySize = 0, dwValueSize = 0;
	tstring strKey, strValue;
	std::map<DWORD, MAP_VEC_DWORD>::iterator it;
	std::map<DWORD, VEC_DWORD>::iterator real_it;
	std::map<DWORD, tstring>::iterator section_it;
	TCHAR* pText = (TCHAR*)m_pData;

	for( it=m_mapOffset.begin(); it!=m_mapOffset.end(); ++it )
	{
		section_it = m_mapSectionName.find( it->first );
		if( section_it == m_mapSectionName.end() )
			return FALSE;	// 找不到对应的区域名字字符串

		for( real_it=it->second.begin(); real_it!=it->second.end(); ++real_it )
		{
			// 得到key的名字
			// (real_it->second)[1]: key name size
			dwKeySize = (real_it->second)[1];	
			// (real_it->second)[0]: key name offset
			strKey.assign((LPCTSTR)(pText+(real_it->second)[0]), dwKeySize);

			// tab换成空格
			while( strKey.find(_T('\t')) != strKey.npos )
				strKey.replace(strKey.find(_T('\t')), 1, 1, _T(' '));
			// 去掉头尾的空格
			m_pUtil->CutSpaceOfStringHeadAndTail(strKey);
			// 和区域的名字组合
			strKey += _T(" ") + section_it->second;	
			
			// (real_it->second)[3]: value size
			dwValueSize = (real_it->second)[3];	
			strValue.assign((LPCTSTR)(pText+(real_it->second)[2]), dwValueSize);

			// tab换成空格
			while( strValue.find(_T('\t')) != strValue.npos )
				strValue.replace(strValue.find(_T('\t')), 1, 1, _T(' '));
			// 去掉头尾的空格
			m_pUtil->CutSpaceOfStringHeadAndTail(strValue);
			// 添加到容器中去
			pVarContainer->Add(strValue.c_str(), strKey.c_str());
		}
	}

	return TRUE;
}




}	// namespace vEngine {
