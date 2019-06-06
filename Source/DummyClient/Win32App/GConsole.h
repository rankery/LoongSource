#pragma once

/**	\class GConsole
	\brief ¿ØÖÆÌ¨
*/
class GConsole
{
public:
	typedef vector<tstring>	AgvList;
private:
	enum GConsoleConst
	{
		CONSOLE_MAX_LINE=20
	};
	struct tagConCmd
	{
		TCHAR szKeyword[32];
		TCHAR szHelp[100];
		void (*fun)(const AgvList& agvList);
	};
public:
	GConsole(void);
	virtual ~GConsole(void);

	void Init(RECT rect);
	void RegCommand(void(*fun)(const AgvList& agvList),LPCTSTR keyword,LPCTSTR help);
	void Print(LPCTSTR format,...);
	void OnChar(TCHAR c);
	void Draw();
	void SetActive(bool bActive)	{	m_bactive=bActive;}
	bool IsActive()	{	return m_bactive;}
	void LogToFile(const TCHAR* fileName);

private:
	void RunCommand();
	void CmdHelp();
private:
	RECT				m_rect;
	vector<tagConCmd>	m_cmds;
	TCHAR				m_inputbuf[100];
public:
	TCHAR				m_showbuf[CONSOLE_MAX_LINE][100];
private:
	bool				m_bactive;
	vector<tstring>      vecStrHis;
	int                 m_nCur;
};
