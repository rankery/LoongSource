#include "StdAfx.h"
#include "NPCNavMapDoors.h"


namespace WorldBase
{
	NPCNavMapDoors::NPCNavMapDoors(void)
	{
	}

	NPCNavMapDoors::~NPCNavMapDoors(void)
	{
		Destroy();
	}

	void NPCNavMapDoors::LoadFromFile( const TCHAR* szMapName,IFS* pFS )
	{
		Destroy();

		TCHAR szPath[512];
		_stprintf(szPath,_T("data\\map\\%s\\nav\\npcnavmapdoors.data"),szMapName);

		DWORD hFile=pFS->Open(szPath);
		THROW_NULL(hFile,_T("load nav npcnavmapdoors.data fail"));

		//--number of doors
		DWORD dwNumDoors=0;
		FReadValue(pFS,hFile,dwNumDoors);

		//--
		for(DWORD i=0;i<dwNumDoors;i++)
		{
			tagDoor* pDoor=new tagDoor;

			//obj id
			FReadValue(pFS,hFile,pDoor->dwObjID);

			//left top tile
			FReadValue(pFS,hFile,pDoor->leftTopTile);

			//open state bitmap
			pDoor->pOpenStateCangoMap=new BitMap;
			pDoor->pOpenStateCangoMap->ReadFile(pFS,hFile);

			//close state bitmap
			pDoor->pCloseStateCangoMap=new BitMap;
			pDoor->pCloseStateCangoMap->ReadFile(pFS,hFile);

			//insert to map
			map<DWORD,tagDoor*>::iterator iter=m_doorsMap.find(pDoor->dwObjID);
			if(iter==m_doorsMap.end())
			{
				m_doorsMap.insert( make_pair(pDoor->dwObjID,pDoor) );
			}
			else
			{
				delete pDoor;
			}
		}

		pFS->Close(hFile);
	}

	bool NPCNavMapDoors::EditorSave( const vector<tagDoor*>& doors,const TCHAR* szSavePath )
	{
		::SetCurrentDirectory(Kernel::Inst()->GetWorkPath());

		TCHAR szPath[512];
		_stprintf(szPath,_T("%snav\\npcnavmapdoors.data"),szSavePath);

		FILE* fp=_tfopen(szPath,_T("wb"));
		if(fp==NULL)
			return false;

		//--number of doors
		DWORD dwNumDoors=(DWORD)doors.size();
		FWriteValue(fp,dwNumDoors);
		
		//--
		for(size_t i=0;i<doors.size();++i)
		{
			tagDoor* pDoor=doors[i];

			//obj id
			FWriteValue(fp,pDoor->dwObjID);

			//left top tile
			FWriteValue(fp,pDoor->leftTopTile);

			//open state bitmap
			pDoor->pOpenStateCangoMap->WriteFile(fp);

			//close state bitmap
			pDoor->pCloseStateCangoMap->WriteFile(fp);
		}

		fclose(fp);

		return true;
	}

	void NPCNavMapDoors::Destroy()
	{
		for(map<DWORD,tagDoor*>::iterator iter=m_doorsMap.begin();
			iter!=m_doorsMap.end();++iter)
		{
			tagDoor* pDoor=iter->second;
			delete pDoor;
		}
		m_doorsMap.clear();
	}

	void NPCNavMapDoors::Clone( NPCNavMapDoors* pTarget )
	{
		ASSERT(pTarget->m_doorsMap.empty());

		for(map<DWORD,tagDoor*>::iterator iter=m_doorsMap.begin();
			iter!=m_doorsMap.end();++iter)
		{
			tagDoor* pDoor=iter->second;

			tagDoor* pNewDoor=new tagDoor;
			pDoor->Clone(pNewDoor);
			pTarget->m_doorsMap.insert( make_pair(pNewDoor->dwObjID,pNewDoor) );
		}
	}

	void NPCNavMapDoors::OpenCloseDoor( DWORD dwObjID,bool bOpen,BitMap* pCangoMap )
	{
		map<DWORD,tagDoor*>::iterator iter=m_doorsMap.find(dwObjID);
		if(iter!=m_doorsMap.end())
		{
			tagDoor* pDoor=iter->second;
			if(bOpen)
				pDoor->pOpenStateCangoMap->CopyTo(*pCangoMap,pDoor->leftTopTile.x,pDoor->leftTopTile.y);
			else
				pDoor->pCloseStateCangoMap->CopyTo(*pCangoMap,pDoor->leftTopTile.x,pDoor->leftTopTile.y);
		}
	}

	void NPCNavMapDoors::OpenCloseAllDoors( bool bOpen,BitMap* pCangoMap )
	{
		for(map<DWORD,tagDoor*>::iterator iter=m_doorsMap.begin();
			iter!=m_doorsMap.end();++iter)
		{
			tagDoor* pDoor=iter->second;
			if(bOpen)
				pDoor->pOpenStateCangoMap->CopyTo(*pCangoMap,pDoor->leftTopTile.x,pDoor->leftTopTile.y);
			else
				pDoor->pCloseStateCangoMap->CopyTo(*pCangoMap,pDoor->leftTopTile.x,pDoor->leftTopTile.y);
		}
	}
}//namespace WorldBase
