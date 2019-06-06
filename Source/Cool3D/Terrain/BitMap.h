#pragma once
#include "..\Cool3D.h"

class IFS;
namespace Cool3D
{
	/**	\class BitMap
		\biref 每个象素为1bit的map, 主要是为了处理Height map的visibility
	*/
	class Cool3D_API BitMap
	{
	public:
		BitMap(void);
		virtual ~BitMap(void);

		void Create(int w,int h,bool initVal);
		void Destroy();
		inline bool GetValue(int x,int y) const;
		void SetValue(int x,int y,bool val);
		bool SafeGetValue(int x,int y) const;
		void SafeSetValue(int x,int y,bool val);

		inline UINT Width() const	{ return m_width;};
		inline UINT Height() const	{ return m_height; };
		inline bool IfIndex(int x,int y) const { return x>=0 && x<m_width 
													&& y>=0 && y<m_height;}

		void WriteFile(FILE *fp);
		void ReadFile(IFS *pFS,DWORD hFile);

		void Clone(BitMap *pTarget);
		void ExportImage(TCHAR* szFileName);

		/** 将自己的内容copy到指定的对象中
		*/
		void CopyTo(BitMap& out,int destx,int desty) const;
		void CopyTo(BitMap& out,int srcx,int srcy,int w,int h,int destx,int desty) const;

	protected:
		int		m_width,
				m_height;
		DWORD	*m_pData;//使用4字节指针,可以提高寻址速度
		int		m_numBytes;//buffer size
	};
}//namespace Cool3D