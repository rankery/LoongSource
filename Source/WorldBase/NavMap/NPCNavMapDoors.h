#pragma once
#include "..\WorldBase.h"

namespace WorldBase
{
	/** \class NPCNavMapDoors
		\brief NPC格子导航图中的门
	*/
	class NPCNavMapDoors
	{
	public:
		struct tagDoor						//门数据
		{	
			DWORD	dwObjID;				//编辑器中指定的物件ID
			BitMap*	pOpenStateCangoMap;		//开启状态导航图
			BitMap* pCloseStateCangoMap;	//关闭状态导航图
			POINT	leftTopTile;			//导航图左上角位置

			tagDoor()
			{
				pOpenStateCangoMap=NULL;
				pCloseStateCangoMap=NULL;
			}

			~tagDoor()
			{
				SAFE_DELETE(pOpenStateCangoMap);
				SAFE_DELETE(pCloseStateCangoMap);
			}

			void Clone(tagDoor* pTarget)
			{
				ASSERT(pTarget->pOpenStateCangoMap==NULL);
				ASSERT(pTarget->pCloseStateCangoMap==NULL);
				ASSERT(pOpenStateCangoMap!=NULL);
				ASSERT(pCloseStateCangoMap!=NULL);

				pTarget->dwObjID=dwObjID;

				pTarget->pOpenStateCangoMap=new BitMap;
				pOpenStateCangoMap->Clone(pTarget->pOpenStateCangoMap);

				pTarget->pCloseStateCangoMap=new BitMap;
				pCloseStateCangoMap->Clone(pTarget->pCloseStateCangoMap);

				pTarget->leftTopTile=leftTopTile;
			}
		};
	public:
		NPCNavMapDoors(void);
		~NPCNavMapDoors(void);

		/** 加载数据
		*/
		void LoadFromFile(const TCHAR* szMapName,IFS* pFS);
		void Destroy();
		void Clone(NPCNavMapDoors* pTarget);

		/** 修改导航图，开启\关闭指定门
		*/
		void OpenCloseDoor(DWORD dwObjID,bool bOpen,BitMap* pCangoMap);
		/** 修改导航图，开启\关闭所有门
		*/
		void OpenCloseAllDoors(bool bOpen,BitMap* pCangoMap);

		//--editor suppport
		static bool EditorSave(const vector<tagDoor*>& doors,const TCHAR* szSavePath);

	private:
		map<DWORD,tagDoor*>	m_doorsMap;
	};
}//namespace WorldBase
