////////////////////////////////////////////////////////////////////////////////
//
// Code taken from D. J. Bernstein (C ==> C++)
//
// ftpparse.c, ftpparse.h: library for parsing FTP LIST responses
// 20001223
// D. J. Bernstein, djb@cr.yp.to
// http://cr.yp.to/ftpparse.html
//
// Commercial use is fine, if you let me know what programs you're using this in.
//
// Currently covered formats:
// EPLF.
// UNIX ls, with or without gid.
// Microsoft FTP Service.
// Windows NT FTP Server.
// VMS.
// WFTPD.
// NetPresenz (Mac).
// NetWare.
// MSDOS.
//
// Definitely not covered: 
// Long VMS filenames, with information split across two lines.
// NCSA Telnet FTP server. Has LIST = NLST (and bad NLST for directories).
//
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Definements.h"


namespace vEngine {

namespace nsFTP
{

class CFTPFileStatus;

/// Implements the parsing of the string returned by the LIST command.
class CFTPListParse
{
public:
   CFTPListParse();
   ~CFTPListParse();

public:
   bool Parse(CFTPFileStatus& ftpFileStatus, const tstring& strLineToParse);

   bool IsEPLS(LPCTSTR pszLine);
   bool ParseEPLF(CFTPFileStatus& ftpFileStatus, LPCTSTR pszLine, int iLength);

   bool IsUNIXStyleListing(LPCTSTR pszLine);
   bool ParseUNIXStyleListing(CFTPFileStatus& ftpFileStatus, LPCTSTR pszLine, int iLength);

   bool IsMultiNetListing(LPCTSTR pszLine);
   bool ParseMultiNetListing(CFTPFileStatus& ftpFileStatus, LPCTSTR pszLine, int iLength);

   bool IsMSDOSListing(LPCTSTR pszLine);
   bool ParseMSDOSListing(CFTPFileStatus& ftpFileStatus, LPCTSTR pszLine, int iLength);

private:
   bool CheckMonth(LPCTSTR pszBuffer, LPCTSTR pszMonthName) const;
   int  GetMonth(LPCTSTR pszBuffer, int iLength) const;
   bool GetLong(LPCTSTR pszLong, int iLength, long& lResult) const;
   long GetYear(time_t time) const;

   long ToTAI(long lYear, long lMonth, long lMDay) const;
   long GuessTAI(long lMonth, long lMDay);

private:
   time_t        m_tmBase;       // time() value on this OS at the beginning of 1970 TAI
   long          m_lCurrentYear; // approximation to current year
   static TCHAR* m_Months[12];
};

}


}