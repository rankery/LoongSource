#pragma once


/** \class PerfLogger
	\brief 性能LOG生成器
*/
class PerfLogger
{
public:
	PerfLogger();
	~PerfLogger();

	static PerfLogger* Inst();
	void Init();
	DWORD OnCommand( LPCTSTR szCommand, LPCTSTR szParam1 );
	void Updata();

private:
	void LogStart( float fSampleInterval );
	void LogEnd();
	tstring MakeFileName();
	bool WriteFileHead( LPCTSTR szFileName );
	bool LogRunTimeData( LPCTSTR szFileName );
	INT64 FileTime2INT64(const FILETIME* ftTime);
	int GetCPUUsage();
private:
	enum { TEXT_BUFF_SIZE = 1024 };
	TSFPTrunk<PerfLogger>			m_Trunk;
	TObjRef<Util>					m_pUtil;
	TObjRef<GUIRender>				m_pGUIRender;
	tstring                         m_strCurFileName;
	bool                            m_bSampleStart;
	float                           m_fSampleInterval;
	TCHAR							m_szBuff[TEXT_BUFF_SIZE];
	float                           m_fLastSampleTime;
	DWORD                           m_dwNumProcessors;
	HANDLE							m_hProcess;
	INT64							m_nLastCPUTime;   
	INT64							m_nLastCPUSystemTime; 
};


