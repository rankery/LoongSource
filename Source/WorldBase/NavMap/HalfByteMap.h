#pragma once

namespace WorldBase
{
	/** \class HalfByteMap
		\brief 半字节数据二维表
	*/
	class HalfByteMap
	{
	public:
		HalfByteMap(void);
		virtual ~HalfByteMap(void);

		void Create(int w,int h,int initVal);
		void Destroy();
		int GetValue(int x,int y) const;
		int SafeGetValue(int x,int y) const;
		void SetValue(int x,int y,int val);

		inline UINT Width() const	{ return m_width;};
		inline UINT Height() const	{ return m_height; };
		inline bool IfIndex(int x,int y) const { return x>=0 && x<m_width 
			&& y>=0 && y<m_height;}

		void WriteFile(FILE *fp);
		void ReadFile(IFS *pFS,DWORD hFile);

		void ExportImage(const TCHAR* szImageFile);
		void Clone(HalfByteMap* pTarget);

	protected:
		int		m_width,
				m_height;
		BYTE	*m_pData;
		int		m_numBytes;//buffer size
	};
}//namespace WorldBase

