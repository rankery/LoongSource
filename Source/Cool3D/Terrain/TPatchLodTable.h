#pragma once
#include "..\Cool3D.h"

namespace Cool3D
{
	/**	\class TPatchLodTable
		\biref 快速获取地块Lod
	*/
	class TPatchLodTable
	{
	public:
		void Create(int width,int height);
		void Destroy();
		void Clear(int lod);
		void SafeSetLod(int x,int z,int lod);
		int SafeGetLod(int x,int z);

		static TPatchLodTable* Inst();
		static TPatchLodTable* Inst_SimpleTerrain();
	private:
		TPatchLodTable(void);
		virtual ~TPatchLodTable(void);

		int m_width;
		int m_height;
		int m_defaultLod;
		BYTE* m_pTable;
	};
}//namespace Cool3D