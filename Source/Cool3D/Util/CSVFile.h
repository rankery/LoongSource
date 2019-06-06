#pragma once
#include "..\VFS\IFS.h"

namespace Cool3D
{
	class Cool3D_API CSVFile
	{
	public:
		CSVFile(void);
		~CSVFile(void);

		bool OpenFile(IFS *pFS,const TCHAR* szFileName);
		void BeginReadLine();
		vector<tstring> GetNextLine();
		void CloseFile();
		bool IsEnd() const;
	private:
		TCHAR *m_szCSVBuffer;
		int	 m_nCharNum;//字符数(不包括0结束符)
		int	 m_nCharIndex;
	};
}//namespace Cool3D