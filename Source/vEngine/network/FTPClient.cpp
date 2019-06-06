////////////////////////////////////////////////////////////////////////////////
//
// The official specification of the File Transfer Protocol (FTP) is the RFC 959.
// Most of the documentation are taken from this RFC.
// This is an implementation of an simple ftp client. I have tried to implement
// platform independent. For the communication i used the classes CBlockingSocket,
// CSockAddr, ... from David J. Kruglinski (Inside Visual C++). These classes are
// only small wrappers for the sockets-API.
// Further I used a smart pointer-implementation from Scott Meyers (Effective C++, 
// More Effective C++, Effective STL).
// The implementation of the logon-sequence (with firewall support) was published 
// in an article on Codeguru by Phil Anderson. 
// The code for the parsing of the different FTP LIST responses is taken from 
// D. J. Bernstein (http://cr.yp.to/ftpparse.html). I only wrapped the c-code in
// a class.
// I haven't tested the code on other platforms, but i think with little 
// modifications it would compile.
// 
// Copyright (c) 2004 Thomas Oswald
//
// Permission to copy, use, sell and distribute this software is granted
// provided this copyright notice appears in all copies.
// Permission to modify the code and to distribute modified code is granted
// provided this copyright notice appears in all copies, and a notice
// that the code was modified is included with the copyright notice.
//
// This software is provided "as is" without express or implied warranty,
// and with no claim as to its suitability for any purpose.
//
// History:
// v1.1 released 2005-12-04
//      - Bug in OpenPassiveDataConnection removed: SendCommand was called before data connection was established.
//      - Bugs in GetSingleResponseLine removed:
//         * Infinite loop if response line doesn't end with CRLF.
//         * Return value of std:string->find must be checked against npos.
//      - Now runs in unicode.
//      - Streams removed.
//      - Explicit detaching of observers are not necessary anymore.
//      - ExecuteDatachannelCommand now accepts an ITransferNotification object. 
//        Through this concept there is no need to write the received files to a file.
//        For example the bytes can be written only in memory or an other tcp stream.
//      - Added an interface for the blocking socket (IBlockingSocket).
//        Therefore it is possible to exchange the socket implementation, e.g. for 
//        writing unit tests (by simulating an specific scenario of an ftp communication).
//      - Replaced the magic numbers concerning the reply codes by a class.
// v1.0 released 2004-10-25
////////////////////////////////////////////////////////////////////////////////

#include "..\stdafx.h"
#include "FTPClient.h"
#include "FTPListParse.h"
#include "FTPFileState.h"
#include <iterator>

namespace vEngine {

using namespace nsHelper;

namespace nsFTP
{
   class CMakeString
   {
   public:
      CMakeString& operator<<(DWORD dwNum)
      {
         DWORD dwTemp = dwNum;
         int iCnt=1; // name lookup of 'iCnt' changed for new ISO 'for' scoping
         for( ; (dwTemp/=10) != 0; iCnt++ )
            ;

         m_str.resize(m_str.size() + iCnt);
      
		 wsprintf(&(*m_str.begin()), _T("%s%u"), m_str.c_str(), dwNum);

         return *this;
      }

      CMakeString& operator<<(const tstring& strAdd)
      {
         m_str += strAdd;
         return *this;
      }
      
      operator tstring() const { return m_str; }
   
   private:
      tstring m_str;
   };

   tstring& ReplaceStr(tstring& strTarget, const tstring& strToReplace, const tstring& strReplacement)
   {
      size_t pos = strTarget.find(strToReplace);
      while( pos != tstring::npos )
      {
         strTarget.replace(pos, strToReplace.length(), strReplacement);
         pos = strTarget.find(strToReplace, pos+1);
      }
      return strTarget;
   }

   class CFile : public ITransferNotification
   {
      FILE* m_pFile;
   public:
      enum T_enOrigin { orBegin=SEEK_SET, orEnd=SEEK_END, orCurrent=SEEK_CUR };

      CFile() : m_pFile(NULL) {}
      ~CFile()
      {
         Close();
      }

      bool Open(const tstring& strFileName, const tstring& strMode)
      {
         m_pFile = fopen(CCnv::ConvertToString(strFileName).c_str(), 
                         CCnv::ConvertToString(strMode).c_str());
         return m_pFile!=NULL;
      }

      bool Close()
      {
         FILE* pFile = m_pFile;
         m_pFile = NULL;
         return pFile && fclose(pFile)==0;
      }

      bool Seek(long lOffset, T_enOrigin enOrigin)
      {
         return m_pFile && fseek(m_pFile, lOffset, enOrigin)==0;
      }

      long Tell()
      {
         if( !m_pFile )
            return -1L;
         return ftell(m_pFile);
      }

      size_t Write(const void* pBuffer, size_t itemSize, size_t itemCount)
      {
         if( !m_pFile )
            return 0;
         return fwrite(pBuffer, itemSize, itemCount, m_pFile);
      }

      size_t Read(void* pBuffer, size_t itemSize, size_t itemCount)
      {
         if( !m_pFile )
            return 0;
         return fread(pBuffer, itemSize, itemCount, m_pFile);
      }

      virtual void OnBytesReceived(const TByteVector& vBuffer, long lReceivedBytes)
      {
         Write(&(*vBuffer.begin()), sizeof(TByteVector::value_type), lReceivedBytes);
      }

      virtual void OnPreBytesSend(TByteVector& vBuffer, size_t& bytesToSend)
      {
         bytesToSend = Read(&(*vBuffer.begin()), sizeof(TByteVector::value_type), vBuffer.size());
      }
   };

   class COutputStream : public ITransferNotification
   {
      const tstring mc_strEolCharacterSequence;
      tstring m_vBuffer;
      tstring::iterator m_itCurrentPos;
   public:
      COutputStream(const tstring& strEolCharacterSequence) :
         mc_strEolCharacterSequence(strEolCharacterSequence),
         m_itCurrentPos(m_vBuffer.end()) {}

      void SetStartPosition()
      {
         m_itCurrentPos = m_vBuffer.begin();
      }
      
      bool GetNextLine(tstring& strLine)// const
      {
         tstring::iterator it = std::search(m_itCurrentPos, m_vBuffer.end(), mc_strEolCharacterSequence.begin(), mc_strEolCharacterSequence.end());
         if( it == m_vBuffer.end() )
            return false;

         strLine.assign(m_itCurrentPos, it);

         m_itCurrentPos = it + mc_strEolCharacterSequence.size();

         return true;
      }

      virtual void OnBytesReceived(const TByteVector& vBuffer, long lReceivedBytes)
      {
         std::copy(vBuffer.begin(), vBuffer.begin()+lReceivedBytes, std::back_inserter(m_vBuffer));
      }
   };

};

using namespace nsFTP;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/// constructor
/// @param[in] pSocket Instance of socket class which will be used for 
///                    communication with the ftp server.
///                    CFTPClient class takes ownership of this instance.
///                    It will be deleted on destruction of this object.
///                    If this pointer is NULL, the CBlockingSocket implementation
///                    will be used. 
///                    This gives the ability to set an other socket class.
///                    For example a socket class can be implemented which simulates
///                    a ftp server (for unit testing).
/// @param[in] uiTimeout Timeout used for socket operation.
/// @param[in] uiBufferSize Size of the buffer used for sending and receiving
///                         data via sockets. The size have an influence on 
///                         the performance. Through empiric test i come to the
///                         conclusion that 2048 is a good size.
/// @param[in] uiResponseWait Sleep time between receive calls to socket when getting 
///                           the response. Sometimes the socket hangs if no wait time
///                           is set. Normally not wait time is necessary.
CFTPClient::CFTPClient(IBlockingSocket* pSocket/*=NULL*/, unsigned int uiTimeout/*=10*/, 
                       unsigned int uiBufferSize/*=2048*/, unsigned int uiResponseWait/*=0*/) :
   mc_uiTimeout(uiTimeout),
   mc_uiResponseWait(uiResponseWait),
   mc_strEolCharacterSequence(_T("\r\n")),
   m_vBuffer(uiBufferSize),
   m_apSckControlConnection(pSocket),
   m_fTransferInProgress(false),
   m_fAbortTransfer(false),
   m_fResumeIfPossible(true)
{
   ASSERT( pSocket );
}

CFTPClient::~CFTPClient()
{
   if( IsTransferringData() )
      Abort();

   if( IsConnected() )
      Logout();
}

/// Attach an observer to the client. You can attach as many observers as you want.
/// The client send notifications (more precisely the virtual functions are called)
/// to the observers.
void CFTPClient::AttachObserver(CFTPClient::CNotification* pObserver)
{
   ASSERT( pObserver );
   if( pObserver )
      m_setObserver.Attach(pObserver);
}

/// Detach an observer from the client.
void CFTPClient::DetachObserver(CFTPClient::CNotification* pObserver)
{
   ASSERT( pObserver );
   if( pObserver )
      m_setObserver.Detach(pObserver);
}

/// Returns a set of all observers currently attached to the client.
CFTPClient::TObserverSet& CFTPClient::GetObservers()
{
   return m_setObserver;
}

/// Enables or disables resuming for file transfer operations.
/// @param[in] fEnable True enables resuming, false disables it.
void CFTPClient::SetResumeMode(bool fEnable/*=true*/)
{
   m_fResumeIfPossible=fEnable;
}

/// Opens the control channel to the FTP server.
/// @param[in] strServerHost IP-address or name of the server
/// @param[in] iServerPort Port for channel. Usually this is port 21.
bool CFTPClient::OpenControlChannel(const tstring& strServerHost, USHORT ushServerPort/*=DEFAULT_FTP_PORT*/)
{
   CloseControlChannel();

   try
   {
      m_apSckControlConnection->Create(SOCK_STREAM);
      CSockAddr adr = m_apSckControlConnection->GetHostByName(CCnv::ConvertToString(strServerHost).c_str(), ushServerPort);
      m_apSckControlConnection->Connect(adr);
   }
   catch(CBlockingSocketException& blockingException)
   {
      ReportError(blockingException.GetErrorMessage(), CCnv::ConvertToTString(__FILE__), __LINE__);
      m_apSckControlConnection->Cleanup();
      return false;
   }

   return true;
}

/// Returns the connection state of the client.
bool CFTPClient::IsConnected() const
{
   return m_apSckControlConnection->operator SOCKET()!=0;
}

/// Returns true if a download/upload is running, otherwise false.
bool CFTPClient::IsTransferringData() const
{
   return m_fTransferInProgress;
}

/// Closes the control channel to the FTP server.
void CFTPClient::CloseControlChannel() 
{
   try
   {
      m_apSckControlConnection->Close();
      m_apCurrentRepresentation.reset(NULL);
   }
   catch(CBlockingSocketException& blockingException)
   {
      blockingException.GetErrorMessage();
      m_apSckControlConnection->Cleanup();
   }
}

/// Analyse the repy code of a ftp-server-response.
/// @param[in] Reply Reply of a ftp server.
/// @retval FTP_OK    All runs perfect.
/// @retval FTP_ERROR Something went wrong. An other response was expected.
/// @retval NOT_OK    The command was not accepted.
int CFTPClient::SimpleErrorCheck(const CReply& Reply) const
{
   if( Reply.Code().IsNegativeReply() )
      return FTP_NOTOK;
   else if( Reply.Code().IsPositiveCompletionReply() )
      return FTP_OK;

   ASSERT( Reply.Code().IsPositiveReply() );

   return FTP_ERROR;
}

/// Logs on to an ftp-server.
/// @param[in] logonInfo Structure with logon information.
bool CFTPClient::Login(const CLogonInfo& logonInfo)
{
   m_LastLogonInfo = logonInfo;

   enum {LO=-2,      ///< Logged On
         ER=-1,      ///< Error
         NUMLOGIN=9, ///< currently supports 9 different login sequences
    };

   int iLogonSeq[NUMLOGIN][18] = {
      // this array stores all of the logon sequences for the various firewalls 
      // in blocks of 3 nums.
      // 1st num is command to send, 
      // 2nd num is next point in logon sequence array if 200 series response  
      //         is rec'd from server as the result of the command,
      // 3rd num is next point in logon sequence if 300 series rec'd
      { 0,LO,3,    1,LO, 6,   2,LO,ER                                  }, // no firewall
      { 3, 6,3,    4, 6,ER,   5,ER, 9,   0,LO,12,   1,LO,15,   2,LO,ER }, // SITE hostname
      { 3, 6,3,    4, 6,ER,   6,LO, 9,   1,LO,12,   2,LO,ER            }, // USER after logon
      { 7, 3,3,    0,LO, 6,   1,LO, 9,   2,LO,ER                       }, // proxy OPEN
      { 3, 6,3,    4, 6,ER,   0,LO, 9,   1,LO,12,   2,LO,ER            }, // Transparent
      { 6,LO,3,    1,LO, 6,   2,LO,ER                                  }, // USER with no logon
      { 8, 6,3,    4, 6,ER,   0,LO, 9,   1,LO,12,   2,LO,ER            }, // USER fireID@remotehost
      { 9,ER,3,    1,LO, 6,   2,LO,ER                                  }, // USER remoteID@remotehost fireID
      {10,LO,3,   11,LO, 6,   2,LO,ER                                  }  // USER remoteID@fireID@remotehost
   };
   
   // are we connecting directly to the host (logon type 0) or via a firewall? (logon type>0)
   tstring   strTemp;
   USHORT    ushPort=0;

   if( logonInfo.FwType() == CFirewallType::None())
   {
      strTemp = logonInfo.Hostname();
      ushPort = logonInfo.Hostport();
   }
   else
   {
      strTemp = logonInfo.FwHost();
      ushPort = logonInfo.FwPort();
   }
   
   tstring strHostnamePort(logonInfo.Hostname());
   if( logonInfo.Hostport()!=DEFAULT_FTP_PORT )
      strHostnamePort = CMakeString() << logonInfo.Hostname() << _T(":") << logonInfo.Hostport(); // add port to hostname (only if port is not 21)

   if( IsConnected() )
      Logout();

   if( !OpenControlChannel(strTemp, ushPort) )
      return false;

   // get initial connect msg off server
   CReply Reply;
   if( !GetResponse(Reply) || !Reply.Code().IsPositiveCompletionReply() )
      return false;
   
   int iLogonPoint=0;

   // go through appropriate logon procedure
   while( true )
   {
      switch(iLogonSeq[logonInfo.FwType().AsEnum()][iLogonPoint])
      {
      case 0:
         strTemp=_T("USER ") + logonInfo.Username();
         break;
      case 1:
         strTemp=_T("PASS ") + logonInfo.Password();
         break;
      case 2:
         strTemp=_T("ACCT ") + logonInfo.Account();
         break;
      case 3:
         strTemp=_T("USER ") + logonInfo.FwUsername();
         break;
      case 4:
         strTemp=_T("PASS ") + logonInfo.FwPassword();
         break;
      case 5:
         strTemp=_T("SITE ") + strHostnamePort;
         break;
      case 6:
         strTemp=_T("USER ") + logonInfo.Username() + _T("@") + strHostnamePort;
         break;
      case 7:
         strTemp=_T("OPEN ") + strHostnamePort;
         break;
      case 8:
         strTemp=_T("USER ") + logonInfo.FwUsername() + _T("@") + strHostnamePort;
         break;
      case 9:
         strTemp=_T("USER ") + logonInfo.Username() + _T("@") + strHostnamePort + _T(" ") + logonInfo.FwUsername();
         break;
      case 10:
         strTemp=_T("USER ") + logonInfo.Username() + _T("@") + logonInfo.FwUsername() + _T("@") + strHostnamePort;
         break;
      case 11:
         strTemp=_T("PASS ") + logonInfo.Password() + _T("@") + logonInfo.FwPassword();
         break;
      }
      
      // send command, get response
      CReply Reply;
      if( !SendCommand(strTemp, Reply) )
         return false;

      if( !Reply.Code().IsPositiveCompletionReply() && !Reply.Code().IsPositiveIntermediateReply() )
         return false;

      const unsigned int uiFirstDigitOfReplyCode = CCnv::TStringToLong(Reply.Code().Value())/100;
      iLogonPoint=iLogonSeq[logonInfo.FwType().AsEnum()][iLogonPoint + uiFirstDigitOfReplyCode-1]; //get next command from array
      switch(iLogonPoint)
      {
      case ER: // ER means somewhat has gone wrong
         {
            ReportError(_T("Logon failed."), CCnv::ConvertToTString(__FILE__), __LINE__);
         }
         return false;
      case LO: // LO means we're fully logged on
         if( ChangeWorkingDirectory(_T("/"))!=FTP_OK )
            return false;
         return true;
      }
   }
   return false;
}

/// Rename a file on the ftp server.
/// @param[in] strOldName Name of the file to rename.
/// @param[in] strNewName The new name for the file.
/// @return see return values of CFTPClient::SimpleErrorCheck
int CFTPClient::Rename(const tstring& strOldName, const tstring& strNewName) const
{
   CReply Reply;
   if( !SendCommand(_T("RNFR ")+strOldName, Reply) )
      return FTP_ERROR;

   if( Reply.Code().IsNegativeReply() )
      return FTP_NOTOK;
   else if( !Reply.Code().IsPositiveIntermediateReply() )
   {
      ASSERT( Reply.Code().IsPositiveCompletionReply() || Reply.Code().IsPositivePreliminaryReply() );
      return FTP_ERROR;
   }

   if( !SendCommand(_T("RNTO ")+strNewName, Reply) )
      return FTP_ERROR;

   return SimpleErrorCheck(Reply);
}
/// Gets the directory listing of the ftp-server. Sends the LIST command to
/// the ftp-server.
/// @param[in] strPath Starting path for the list command.
/// @param[out] vstrFileList Returns a simple list of the files and folders of the specified directory.
/// @param[in] fPasv see documentation of CFTPClient::Passive
bool CFTPClient::List(const tstring& strPath, TStringVector& vstrFileList, bool fPasv) const
{
   COutputStream outputStream(mc_strEolCharacterSequence);
   if( !ExecuteDatachannelCommand(CDatachannelCmd::LIST(), strPath, CRepresentation(CType::ASCII()), fPasv, 0, &outputStream) )
      return false;

   vstrFileList.clear();
   tstring strLine;
   outputStream.SetStartPosition();
   while( outputStream.GetNextLine(strLine) )
      vstrFileList.push_back(strPath + strLine.c_str());

   return true;
}

/// Gets the directory listing of the ftp-server. Sends the NLST command to
/// the ftp-server.
/// @param[in] strPath Starting path for the list command.
/// @param[out] vstrFileList Returns a simple list of the files and folders of the specified the directory.
/// @param[in] fPasv see documentation of CFTPClient::Passive
bool CFTPClient::NameList(const tstring& strPath, TStringVector& vstrFileList, bool fPasv) const
{
   COutputStream outputStream(mc_strEolCharacterSequence);
   if( !ExecuteDatachannelCommand(CDatachannelCmd::NLST(), strPath, CRepresentation(CType::ASCII()), fPasv, 0, &outputStream) )
      return false;

   vstrFileList.clear();
   tstring strLine;
   outputStream.SetStartPosition();
   while( outputStream.GetNextLine(strLine) )
      vstrFileList.push_back(strPath + strLine.c_str());

   return true;
}

/// Gets the directory listing of the ftp-server. Sends the LIST command to
/// the ftp-server.
/// @param[in] strPath Starting path for the list command.
/// @param[out] vFileList Returns a detailed list of the files and folders of the specified directory.
///                       vFileList contains CFTPFileStatus-Objects. These Objects provide a lot of
///                       information about the file/folder.
/// @param[in] fPasv see documentation of CFTPClient::Passive
bool CFTPClient::List(const tstring& strPath, TSpFTPFileStatusVector& vFileList, bool fPasv) const
{
   COutputStream outputStream(mc_strEolCharacterSequence);
   if( !ExecuteDatachannelCommand(CDatachannelCmd::LIST(), strPath, CRepresentation(CType::ASCII()), fPasv, 0, &outputStream) )
      return false;

   vFileList.clear();
   tstring strLine;
   CFTPListParse ftpListParser;
   outputStream.SetStartPosition();
   while( outputStream.GetNextLine(strLine) )
   {
      std::auto_ptr<CFTPFileStatus> pFtpFileStatus(new CFTPFileStatus());
      if( ftpListParser.Parse(*pFtpFileStatus, strLine) )
      {
         pFtpFileStatus->Path() = strPath;
         vFileList.push_back(pFtpFileStatus.release());
      }
   }

   return true;
}

/// Gets the directory listing of the ftp-server. Sends the NLST command to
/// the ftp-server.
/// @param[in] strPath Starting path for the list command.
/// @param[out] vFileList Returns a simple list of the files and folders of the specified directory.
///                       vFileList contains CFTPFileStatus-Objects. Normally these Objects provide 
///                       a lot of information about the file/folder. But the NLST-command provide 
///                       only a simple list of the directory content (no specific information).
/// @param[in] fPasv see documentation of CFTPClient::Passive
bool CFTPClient::NameList(const tstring& strPath, TSpFTPFileStatusVector& vFileList, bool fPasv) const
{
   COutputStream outputStream(mc_strEolCharacterSequence);
   if( !ExecuteDatachannelCommand(CDatachannelCmd::NLST(), strPath, CRepresentation(CType::ASCII()), fPasv, 0, &outputStream) )
      return false;

   vFileList.clear();
   tstring strLine;
   CFTPListParse ftpListParser;
   outputStream.SetStartPosition();
   while( outputStream.GetNextLine(strLine) )
   {
      std::auto_ptr<CFTPFileStatus> pFtpFileStatus(new CFTPFileStatus());
      pFtpFileStatus->Path() = strPath;
      pFtpFileStatus->Name() = strLine;
      vFileList.push_back(pFtpFileStatus.release());
   }

   return true;
}

/// Gets a file from the ftp-server.
/// Uses C functions for file access (very fast).
/// @param[in] strRemoteFile Filename of the sourcefile on the ftp-server.
/// @param[in] strLocalFile Filename of the targetfile on the local computer.
/// @param[in] repType Representation Type (see documentation of CRepresentation)
/// @param[in] fPasv see documentation of CFTPClient::Passive
bool CFTPClient::DownloadFile(const tstring& strRemoteFile, const tstring& strLocalFile, const CRepresentation& repType, bool fPasv) const
{
   CFile file;
   if( !file.Open(strLocalFile, m_fResumeIfPossible?_T("ab"):_T("wb")) )
   {
      ReportError(CError::GetErrorDescription(), CCnv::ConvertToTString(__FILE__), __LINE__);
      return false;
   }
   file.Seek(0, CFile::orEnd);

   long lRemoteFileSize = 0;
   FileSize(strRemoteFile, lRemoteFileSize);

   for( TObserverSet::const_iterator it=m_setObserver.begin(); it!=m_setObserver.end(); ++it )
      (*it)->OnPreReceiveFile(strRemoteFile, strLocalFile, lRemoteFileSize);

   const bool fRet=ExecuteDatachannelCommand(CDatachannelCmd::RETR(), strRemoteFile, repType, fPasv, file.Tell(), &file);

   for( TObserverSet::const_iterator it2=m_setObserver.begin(); it2!=m_setObserver.end(); ++it2 )
      (*it2)->OnPostReceiveFile(strRemoteFile, strLocalFile, lRemoteFileSize);

   return fRet;
}

/// Puts a file on the ftp-server.
/// Uses C functions for file access (very fast).
/// @param[in] strLocalFile Filename of the the local sourcefile which to put on the ftp-server.
/// @param[in] strRemoteFile Filename of the targetfile on the ftp-server.
/// @param[in] fStoreUnique if true, the ftp command STOU is used for saving
///                         else the ftp command STOR is used.
/// @param[in] repType Representation Type (see documentation of CRepresentation)
/// @param[in] fPasv see documentation of CFTPClient::Passive
bool CFTPClient::UploadFile(const tstring& strLocalFile, const tstring& strRemoteFile, bool fStoreUnique, const CRepresentation& repType, bool fPasv) const
{
   CFile file;
   if( !file.Open(strLocalFile, _T("rb")) )
   {
      ReportError(CError::GetErrorDescription(), CCnv::ConvertToTString(__FILE__), __LINE__);
      return false;
   }

   long lRemoteFileSize = 0;
   if( m_fResumeIfPossible )
      FileSize(strRemoteFile, lRemoteFileSize);

   CDatachannelCmd cmd(CDatachannelCmd::STOR());
   if( lRemoteFileSize > 0 )
      cmd = CDatachannelCmd::APPE();
   else if( fStoreUnique )
      cmd = CDatachannelCmd::STOU();

   file.Seek(0, CFile::orEnd);
   long lLocalFileSize = file.Tell();
   file.Seek(lRemoteFileSize, CFile::orBegin);

   TObserverSet::const_iterator it;
   for( it=m_setObserver.begin(); it!=m_setObserver.end(); ++it )
      (*it)->OnPreSendFile(strLocalFile, strRemoteFile, lLocalFileSize);

   const bool fRet=ExecuteDatachannelCommand(cmd, strRemoteFile, repType, fPasv, 0, &file);

   for( it=m_setObserver.begin(); it!=m_setObserver.end(); ++it )
      (*it)->OnPostSendFile(strLocalFile, strRemoteFile, lLocalFileSize);

   return fRet;
}

/// Executes a commando that result in a communication over the data port.
/// @param[in] crDatachannelCmd Command to be executeted.
/// @param[in] strPath Parameter for the command usually a path.
/// @param[in] representation see documentation of CFTPClient::CRepresentation
/// @param[in] fPasv see documentation of CFTPClient::Passive
/// @param[in] dwByteOffset Server marker at which file transfer is to be restarted.
/// @param[in] pObserver Object for observing the execution of the command.
bool CFTPClient::ExecuteDatachannelCommand(const CDatachannelCmd& crDatachannelCmd, const tstring& strPath, const CRepresentation& representation, 
                                           bool fPasv, DWORD dwByteOffset, ITransferNotification* pObserver) const
{
   if( m_fTransferInProgress )
      return false;

   if( !IsConnected() )
      return false;

   // check representation
   if( m_apCurrentRepresentation.get()==NULL )
      m_apCurrentRepresentation.reset(new CRepresentation(CType::ASCII()));
   
   if( representation!=*m_apCurrentRepresentation )
   {
      // transmit representation to server
      if( RepresentationType(representation)!=FTP_OK )
         return false;
      *m_apCurrentRepresentation = representation;
   }

   std::auto_ptr<IBlockingSocket> apSckDataConnection(m_apSckControlConnection->CreateInstance());

   if( fPasv )
   {
      if( !OpenPassiveDataConnection(*apSckDataConnection, crDatachannelCmd, strPath, dwByteOffset) )
         return false;
   }
   else
   {
      if( !OpenActiveDataConnection(*apSckDataConnection, crDatachannelCmd, strPath, dwByteOffset) )
         return false;
   }

   const bool fTransferOK = TransferData(crDatachannelCmd, pObserver, *apSckDataConnection);
   
   apSckDataConnection->Close();

   // get response from ftp server
   CReply Reply;
   if( !fTransferOK || !GetResponse(Reply) || !Reply.Code().IsPositiveCompletionReply() )
      return false;

   return true;
}

/// Executes a commando that result in a communication over the data port.
/// @param[in] crDatachannelCmd Command to be executeted.
/// @param[in] pObserver Object for observing the execution of the command.
/// @param[in] sckDataConnection Socket which is used for sending/receiving data.
bool CFTPClient::TransferData(const CDatachannelCmd& crDatachannelCmd, ITransferNotification* pObserver, IBlockingSocket& sckDataConnection) const
{
   switch( crDatachannelCmd.AsEnum() )
   {
   case CDatachannelCmd::cmdSTOR:
   case CDatachannelCmd::cmdSTOU:
   case CDatachannelCmd::cmdAPPE:
      {
         if( !SendData(pObserver, sckDataConnection) )
            return false;
      }
      break;
   case CDatachannelCmd::cmdRETR:
   case CDatachannelCmd::cmdLIST:
   case CDatachannelCmd::cmdNLST:
      if( !ReceiveData(pObserver, sckDataConnection) )
         return false;
      break;
   default:
      ASSERT( false );
      return false;
   }
   return true;
}

/// Opens an active data connection.
/// @param[out] sckDataConnection
/// @param[in] crDatachannelCmd Command to be executeted.
/// @param[in] strPath Parameter for the command usually a path.
/// @param[in] dwByteOffset Server marker at which file transfer is to be restarted.
bool CFTPClient::OpenActiveDataConnection(IBlockingSocket& sckDataConnection, const CDatachannelCmd& crDatachannelCmd, const tstring& strPath, DWORD dwByteOffset) const
{
   std::auto_ptr<IBlockingSocket> apSckServer(m_apSckControlConnection->CreateInstance());

   USHORT ushLocalSock = 0;
   try
   {
      // INADDR_ANY = ip address of localhost
      // second parameter "0" means that the WINSOCKAPI ask for a port
      CSockAddr csaAddressTemp(INADDR_ANY, 0);
      apSckServer->Create(SOCK_STREAM);
      apSckServer->Bind(csaAddressTemp);
      apSckServer->GetSockAddr(csaAddressTemp);
      ushLocalSock=csaAddressTemp.Port();
      apSckServer->Listen();
   }
   catch(CBlockingSocketException& blockingException)
   {
      ReportError(blockingException.GetErrorMessage(), CCnv::ConvertToTString(__FILE__), __LINE__);
      apSckServer->Cleanup();
      return false;
   }

   // get own ip address
   CSockAddr csaLocalAddress;
   m_apSckControlConnection->GetSockAddr(csaLocalAddress);

   // transmit the socket (ip address + port) to the server
   // the ftp server establishes then the data connection
   if( DataPort(csaLocalAddress.DottedDecimal(), ushLocalSock)!=FTP_OK )
      return false;

   // if resuming is activated then set offset
   if( m_fResumeIfPossible && 
      (crDatachannelCmd==CDatachannelCmd::cmdSTOR || crDatachannelCmd==CDatachannelCmd::cmdRETR || crDatachannelCmd==CDatachannelCmd::cmdAPPE ) &&
       (dwByteOffset!=0 && Restart(dwByteOffset)!=FTP_OK) )
      return false;

   // send FTP command RETR/STOR/NLST/LIST to the server
   CReply Reply;
   if( !SendCommand(GetCmdString(crDatachannelCmd, strPath), Reply) ||
       !Reply.Code().IsPositivePreliminaryReply() )
      return false;

   // accept the data connection
   CSockAddr sockAddrTemp;
   if( !apSckServer->Accept(sckDataConnection, sockAddrTemp) )
      return false;

   return true;
}

/// Opens a passive data connection.
/// @param[out] sckDataConnection
/// @param[in] crDatachannelCmd Command to be executeted.
/// @param[in] strPath Parameter for the command usually a path.
/// @param[in] dwByteOffset Server marker at which file transfer is to be restarted.
bool CFTPClient::OpenPassiveDataConnection(IBlockingSocket& sckDataConnection, const CDatachannelCmd& crDatachannelCmd, const tstring& strPath, DWORD dwByteOffset) const
{
   ULONG   ulRemoteHostIP = 0;
   USHORT  ushServerSock  = 0;

   // set passive mode
   // the ftp server opens a port and tell us the socket (ip address + port)
   // this socket is used for opening the data connection 
   if( Passive(ulRemoteHostIP, ushServerSock)!=FTP_OK )
      return false;

   // establish connection
   CSockAddr sockAddrTemp;
   try
   {
      sckDataConnection.Create(SOCK_STREAM);
      CSockAddr csaAddress(ulRemoteHostIP, ushServerSock); 
      sckDataConnection.Connect(csaAddress);
   }
   catch(CBlockingSocketException& blockingException)
   {
      ReportError(blockingException.GetErrorMessage(), CCnv::ConvertToTString(__FILE__), __LINE__);
      sckDataConnection.Cleanup();
      return false;
   }

   // if resuming is activated then set offset
   if( m_fResumeIfPossible && 
      (crDatachannelCmd==CDatachannelCmd::cmdSTOR || crDatachannelCmd==CDatachannelCmd::cmdRETR || crDatachannelCmd==CDatachannelCmd::cmdAPPE ) &&
       (dwByteOffset!=0 && Restart(dwByteOffset)!=FTP_OK) )
      return false;

   // send FTP command RETR/STOR/NLST/LIST to the server
   CReply Reply;
   if( !SendCommand(GetCmdString(crDatachannelCmd, strPath), Reply) ||
       !Reply.Code().IsPositivePreliminaryReply() )
      return false;

   return true;
}

/// Sends data over a socket to the server.
/// @param[in] pObserver Object for observing the execution of the command.
/// @param[in] sckDataConnection Socket which is used for the send action.
bool CFTPClient::SendData(ITransferNotification* pObserver, IBlockingSocket& sckDataConnection) const
{
   try
   {
      m_fTransferInProgress=true;

      int iNumWrite;
      size_t bytesRead;

      pObserver->OnPreBytesSend(m_vBuffer, bytesRead);

      while( !m_fAbortTransfer && bytesRead!=0 )
      {
         iNumWrite = sckDataConnection.Write(&(*m_vBuffer.begin()), static_cast<int>(bytesRead), mc_uiTimeout);
         ASSERT( iNumWrite == static_cast<int>(bytesRead) );

         for( TObserverSet::const_iterator it=m_setObserver.begin(); it!=m_setObserver.end(); ++it )
            (*it)->OnBytesSent(m_vBuffer, iNumWrite);

         pObserver->OnPreBytesSend(m_vBuffer, bytesRead);
      }

      m_fTransferInProgress=false;
      if( m_fAbortTransfer )
      {
         Abort();
         return false;
      }

   }
   catch(CBlockingSocketException& blockingException)
   {
      m_fTransferInProgress=false;
      ReportError(blockingException.GetErrorMessage(), CCnv::ConvertToTString(__FILE__), __LINE__);
      sckDataConnection.Cleanup();
      return false;
   }

   return true;
}

/// Receives data over a socket from the server.
/// @param[in] pObserver Object for observing the execution of the command.
/// @param[in] sckDataConnection Socket which is used for receiving the data.
bool CFTPClient::ReceiveData(ITransferNotification* pObserver, IBlockingSocket& sckDataConnection) const
{
   try
   {
      m_fTransferInProgress = true;

      for( TObserverSet::const_iterator it=m_setObserver.begin(); it!=m_setObserver.end(); ++it )
         (*it)->OnBeginReceivingData();

      int iNumRead=sckDataConnection.Receive(&(*m_vBuffer.begin()), static_cast<int>(m_vBuffer.size()), mc_uiTimeout);
      long lTotalBytes = iNumRead;
      while( !m_fAbortTransfer && iNumRead!=0 )
      {
         for( TObserverSet::const_iterator it=m_setObserver.begin(); it!=m_setObserver.end(); ++it )
            (*it)->OnBytesReceived(m_vBuffer, iNumRead);

         pObserver->OnBytesReceived(m_vBuffer, iNumRead);

         iNumRead=sckDataConnection.Receive(&(*m_vBuffer.begin()), static_cast<int>(m_vBuffer.size()), mc_uiTimeout);
         lTotalBytes += iNumRead;
      }

      for( TObserverSet::const_iterator it2=m_setObserver.begin(); it2!=m_setObserver.end(); ++it2 )
         (*it2)->OnEndReceivingData(lTotalBytes);

      m_fTransferInProgress=false;
      if( m_fAbortTransfer )
      {
         Abort();
         return false;
      }
   }
   catch(CBlockingSocketException& blockingException)
   {
      m_fTransferInProgress=false;
      ReportError(blockingException.GetErrorMessage(), CCnv::ConvertToTString(__FILE__), __LINE__);
      sckDataConnection.Cleanup();
      return false;
   }

   return true;
}

/// Returns the command string for a specific command.
/// @param[in] crDatachannelCmd Command for which the string should be returned.
/// @param[in] strPath Parameter which have to be added to the command.
tstring CFTPClient::GetCmdString(const CDatachannelCmd& crDatachannelCmd, const tstring& strPath) const
{
   switch( crDatachannelCmd.AsEnum() )
   {
   case CDatachannelCmd::cmdLIST: return strPath.empty()?_T("LIST"):_T("LIST ") + strPath;
   case CDatachannelCmd::cmdNLST: return strPath.empty()?_T("NLST"):_T("NLST ") + strPath;
   case CDatachannelCmd::cmdSTOR: return _T("STOR ") + strPath;
   case CDatachannelCmd::cmdSTOU: return _T("STOU ") + strPath;
   case CDatachannelCmd::cmdRETR: return _T("RETR ") + strPath;
   case CDatachannelCmd::cmdAPPE: return _T("APPE ") + strPath;
   default:
      ASSERT( false );
   }
   return _T("");
}

/// Sends a command to the server.
/// @param[in] strCommand Command to send.
bool CFTPClient::SendCommand(const tstring& strCommand) const
{
   if( !IsConnected() )
      return false;

   try
   {
      for( TObserverSet::const_iterator it=m_setObserver.begin(); it!=m_setObserver.end(); ++it )
         (*it)->OnSendCommand(strCommand);
      m_apSckControlConnection->Write((CCnv::ConvertToString(strCommand) + "\r\n").c_str(), static_cast<int>(strCommand.length())+2, mc_uiTimeout);
   }
   catch(CBlockingSocketException& blockingException)
   {
      ReportError(blockingException.GetErrorMessage(), CCnv::ConvertToTString(__FILE__), __LINE__);
      const_cast<CFTPClient*>(this)->m_apSckControlConnection->Cleanup();
      return false;
   }

   return true;
}

/// Sends a command to the server.
/// @param[in]  strCommand Command to send.
/// @param[out] Reply The Reply of the server to the sent command.
bool CFTPClient::SendCommand(const tstring& strCommand, CReply& Reply) const
{
   if( !SendCommand(strCommand) || !GetResponse(Reply) )
      return false;
   return true;
}

/// This function gets the server response.
/// A server response can exists of more than one line. This function
/// returns the full response (multiline).
/// @param[out] Reply Reply of the server to a command.
bool CFTPClient::GetResponse(CReply& Reply) const
{
   tstring strResponse;
   if( !GetSingleResponseLine(strResponse) )
      return false;

   if( strResponse.length()>3 && strResponse.at(3)==_T('-') )
   {
      tstring strSingleLine(strResponse);
      const int iRetCode=CCnv::TStringToLong(strResponse);
      // handle multi-line server responses
      while( !(strSingleLine.length()>3 && 
               strSingleLine.at(3)==_T(' ') &&
               CCnv::TStringToLong(strSingleLine)==iRetCode) )
      { 
         if( !GetSingleResponseLine(strSingleLine) )
            return false;
         strResponse += _T("\r\n") + strSingleLine;
      }
   }

   bool fRet = Reply.Set(strResponse);

   for( TObserverSet::const_iterator it=m_setObserver.begin(); it!=m_setObserver.end(); ++it )
      (*it)->OnResponse(Reply);

   return fRet;
}

/// Reads a single response line from the server control channel.
/// @param[out] strResponse Response of the server as string.
bool CFTPClient::GetSingleResponseLine(tstring& strResponse) const
{
   if( !IsConnected() )
      return false;

   try
   {
      if( m_qResponseBuffer.empty() )
      {
         // internal buffer is empty ==> get response from ftp-server
         int iNum=0;
         std::string strTemp;

         do
         {
            iNum=m_apSckControlConnection->Receive(&(*m_vBuffer.begin()), static_cast<int>(m_vBuffer.size())-1, mc_uiTimeout);
            if( mc_uiResponseWait !=0 )
               Sleep(mc_uiResponseWait);
            m_vBuffer[iNum] = '\0';
            strTemp+=&(*m_vBuffer.begin());
         } while( iNum==static_cast<int>(m_vBuffer.size())-1 && m_apSckControlConnection->CheckReadability() );

         // each line in response is a separate entry in the internal buffer
         while( strTemp.length() )
         {
            size_t iCRLF=strTemp.find('\n');
            if( iCRLF != std::string::npos )
            {
               m_qResponseBuffer.push(strTemp.substr(0, iCRLF+1));
               strTemp.erase(0, iCRLF+1);
            }
            else
            {
               // this is not rfc standard; normally each command must end with CRLF
               // in this case it doesn't
               m_qResponseBuffer.push(strTemp);
               strTemp.clear();
            }
         }

         if( m_qResponseBuffer.empty() )
            return false;
      }

      // get first response-line from buffer
      strResponse = CCnv::ConvertToTString(m_qResponseBuffer.front().c_str());
      m_qResponseBuffer.pop();

      // remove CrLf if exists
      if( strResponse.length()> 1 && strResponse.substr(strResponse.length()-2)==_T("\r\n") )
         strResponse.erase(strResponse.length()-2, 2);
   }
   catch(CBlockingSocketException& blockingException)
   {
      ReportError(blockingException.GetErrorMessage(), CCnv::ConvertToTString(__FILE__), __LINE__);
      const_cast<CFTPClient*>(this)->m_apSckControlConnection->Cleanup();
      return false;
   }

   return true;
}

/// Executes the ftp command CDUP (change to parent directory).
/// This command is a special case of CFTPClient::ChangeWorkingDirectory 
/// (CWD), and is  included to simplify the implementation of programs for 
/// transferring directory trees between operating systems having different 
/// syntaxes for naming the parent directory.
/// @return see return values of CFTPClient::SimpleErrorCheck
int CFTPClient::ChangeToParentDirectory() const
{
   CReply Reply;
   if( !SendCommand(_T("CDUP"), Reply) )
      return FTP_ERROR;
   return SimpleErrorCheck(Reply);
}

/// Executes the ftp command QUIT.
/// This command terminates a USER and if file transfer is not in progress, 
/// the server closes the control connection. If  file transfer is in progress, 
/// the connection will remain open for result response and the server will 
/// then close it.
/// If the user-process is transferring files for several USERs but does not 
/// wish to close and then reopen connections for each, then the REIN command 
/// should be used instead of QUIT.
/// An unexpected close on the control connection will cause the server to take 
/// the effective action of an abort (ABOR) and a logout.
/// @return see return values of CFTPClient::SimpleErrorCheck
int CFTPClient::Logout()
{
   CReply Reply;
   if( !SendCommand(_T("QUIT"), Reply) )
      return FTP_ERROR;

   CloseControlChannel();

   return SimpleErrorCheck(Reply);
}

/// Executes the ftp command PASV. Set the passive mode.
/// This command requests the server-DTP (data transfer process) on a data to 
/// "listen"  port (which is not its default data port) and to wait for a 
/// connection rather than initiate one upon receipt of a transfer command. 
/// The response to this command includes the host and port address this 
/// server is listening on.
/// @param[out] ulIpAddress IP address the server is listening on.
/// @param[out] ushPort Port the server is listening on.
/// @return see return values of CFTPClient::SimpleErrorCheck
int CFTPClient::Passive(ULONG& ulIpAddress, USHORT& ushPort) const
{
   CReply Reply;
   if( !SendCommand(_T("PASV"), Reply) )
      return FTP_ERROR;

   if( Reply.Code().IsPositiveCompletionReply() )
   {
      if( !GetIpAddressFromResponse(Reply.Value(), ulIpAddress, ushPort) )
         return FTP_ERROR;
   }

   return SimpleErrorCheck(Reply);
}

/// Parses a response string and extracts the ip address and port information.
/// @param[in]  strResponse The response string of a ftp server which holds 
///                         the ip address and port information.
/// @param[out] ulIpAddress Buffer for the ip address.
/// @param[out] ushPort     Buffer for the port information.
/// @retval true  Everything went ok.
/// @retval false An error occurred (invalid format).
bool CFTPClient::GetIpAddressFromResponse(const tstring& strResponse, ULONG& ulIpAddress, USHORT& ushPort) const
{
   // parsing of ip-address and port implemented with a finite state machine
   // ...(192,168,1,1,3,44)...
   enum T_enState { state0, state1, state2, state3, state4 } enState = state0;
   
   tstring strIpAddress, strPort;
   USHORT ushTempPort = 0;
   ULONG  ulTempIpAddress = 0;
   int iCommaCnt = 4;
   for( tstring::const_iterator it=strResponse.begin(); it!=strResponse.end(); ++it )
   {
      switch( enState )
      {
      case state0:
         if( *it == _T('(') )
            enState = state1;
         break;
      case state1:
         if( *it == _T(',') )
         {
            if( --iCommaCnt == 0 )
            {
               enState = state2;
               ulTempIpAddress += CCnv::TStringToLong(strIpAddress.c_str());
            }
            else
            {
               ulTempIpAddress += CCnv::TStringToLong(strIpAddress.c_str())<<8*iCommaCnt;
               strIpAddress.clear();
            }
         }
         else
         {
            if( !_istdigit(*it) )
               return false;
            strIpAddress += *it;
         }
         break;
      case state2:
         if( *it == _T(',') )
         {
            ushTempPort = static_cast<USHORT>(CCnv::TStringToLong(strPort.c_str())<<8);
            strPort.clear();
            enState = state3;
         }
         else
         {
            if( !_istdigit(*it) )
               return false;
            strPort += *it;
         }
         break;
      case state3:
         if( *it == _T(')') )
         {
            // compiler warning if using +=operator
            ushTempPort = ushTempPort + static_cast<USHORT>(CCnv::TStringToLong(strPort.c_str()));
            enState = state4;
         }
         else
         {
            if( !_istdigit(*it) )
               return false;
            strPort += *it;
         }
      }
   }

   if( enState==state4 )
   {
      ulIpAddress = ulTempIpAddress;
      ushPort = ushTempPort;
   }

   return enState==state4;
}

/// Executes the ftp command ABOR.
/// This command tells the server to abort the previous FTP service command 
/// and any associated transfer of data.  The abort command may require 
/// "special action", as discussed in the Section on FTP Commands, to force 
/// recognition by the server.  No action is to be taken if the previous 
/// command has been completed (including data transfer).  The control
/// connection is not to be closed by the server, but the data connection 
/// must be closed.
/// There are two cases for the server upon receipt of this command:<BR>
/// (1) the FTP service command was already completed, or <BR>
/// (2) the FTP service command is still in progress.<BR>
/// In the first case, the server closes the data connection (if it is open)
/// and responds with a 226 reply, indicating that the abort command was 
/// successfully processed.
/// In the second case, the server aborts the FTP service in progress and 
/// closes the data connection, returning a 426 reply to indicate that the 
/// service request terminated abnormally.  The server then sends a 226 reply,
/// indicating that the abort command was successfully processed.
/// @return see return values of CFTPClient::SimpleErrorCheck
int CFTPClient::Abort() const
{
   if( m_fTransferInProgress )
   {
      m_fAbortTransfer = true;
      return FTP_OK;
   }

   m_fAbortTransfer = false;
   CReply Reply;
   if( !SendCommand(_T("ABOR"), Reply) )
      return FTP_ERROR;
   return SimpleErrorCheck(Reply);
}
/// Executes the ftp command PWD (PRINT WORKING DIRECTORY)
/// This command causes the name of the current working directory 
/// to be returned in the reply.
int CFTPClient::PrintWorkingDirectory() const
{
   CReply Reply;
   if( !SendCommand(_T("PWD"), Reply) )
      return FTP_ERROR;
   return SimpleErrorCheck(Reply);
}

/// Executes the ftp command SYST (SYSTEM)
/// This command is used to find out the type of operating system at the server.
/// The reply shall have as its first word one of the system names listed in the 
/// current version of the Assigned Numbers document [Reynolds, Joyce, and 
/// Jon Postel, "Assigned Numbers", RFC 943, ISI, April 1985.].
/// @return see return values of CFTPClient::SimpleErrorCheck
int CFTPClient::System() const
{
   CReply Reply;
   if( !SendCommand(_T("SYST"), Reply) )
      return FTP_ERROR;
   return SimpleErrorCheck(Reply);
}

/// Executes the ftp command NOOP
/// This command does not affect any parameters or previously entered commands. 
/// It specifies no action other than that the server send an FTP_OK reply.
/// @return see return values of CFTPClient::SimpleErrorCheck
int CFTPClient::Noop() const
{
   CReply Reply;
   if( !SendCommand(_T("NOOP"), Reply) )
      return FTP_ERROR;
   return SimpleErrorCheck(Reply);
}

/// Executes the ftp command PORT (DATA PORT)
/// The argument is a HOST-PORT specification for the data port to be used in data 
/// connection. There are defaults for both the user and server data ports, and 
/// under normal circumstances this command and its reply are not needed.  If
/// this command is used, the argument is the concatenation of a 32-bit internet 
/// host address and a 16-bit TCP port address.
/// @param[in] strHostIP IP-address like xxx.xxx.xxx.xxx
/// @param[in] uiPort 16-bit TCP port address.
/// @return see return values of CFTPClient::SimpleErrorCheck
int CFTPClient::DataPort(const tstring& strHostIP, USHORT ushPort) const
{
   tstring strPortCmd;
   // convert the port number to 2 bytes + add to the local IP
   strPortCmd = CMakeString() << _T("PORT ") << strHostIP << _T(",") << (ushPort>>8) << _T(",") << (ushPort&0xFF);

   ReplaceStr(strPortCmd, _T("."), _T(","));

   CReply Reply;
   if( !SendCommand(strPortCmd, Reply) )
      return FTP_ERROR;
   return SimpleErrorCheck(Reply);
}

/// Executes the ftp command TYPE (REPRESENTATION TYPE)
/// see Documentation of nsFTP::CRepresentation
/// @param[in] representation see Documentation of nsFTP::CRepresentation
/// @param[in] iSize Indicates Bytesize for type LocalByte.
/// @return see return values of CFTPClient::SimpleErrorCheck
int CFTPClient::RepresentationType(const CRepresentation& representation, DWORD dwSize) const
{
   tstring strCmd;
   switch( representation.Type().AsEnum() )
   {
   case CType::tyASCII:     strCmd = _T("TYPE A");                             break;
   case CType::tyEBCDIC:    strCmd = _T("TYPE E");                             break;
   case CType::tyImage:     strCmd = _T("TYPE I");                             break;
   case CType::tyLocalByte: strCmd = CMakeString() << _T("TYPE L ") << dwSize; break;
   default:
      ASSERT( false );
      return FTP_ERROR;
   }

   if( representation.Type()==CType::tyASCII || 
       representation.Type()==CType::tyEBCDIC )
   {
      switch( representation.Format().AsEnum() )
      {
      case CTypeFormat::tfNonPrint:        strCmd += _T(" N"); break;
      case CTypeFormat::tfTelnetFormat:    strCmd += _T(" T"); break;
      case CTypeFormat::tfCarriageControl: strCmd += _T(" C"); break;
      default:
         ASSERT( false );
      }
   }

   CReply Reply;
   if( !SendCommand(strCmd, Reply) )
      return FTP_ERROR;
   return SimpleErrorCheck(Reply);
}

/// Executes the ftp command CWD (CHANGE WORKING DIRECTORY)
/// This command allows the user to work with a different directory or dataset 
/// for file storage or retrieval without altering his login or accounting 
/// information. Transfer parameters are similarly unchanged.
/// @param[in] strDirectory Pathname specifying a directory or other system 
///                         dependent file group designator.
/// @return see return values of CFTPClient::SimpleErrorCheck
int CFTPClient::ChangeWorkingDirectory(const tstring& strDirectory) const
{
   ASSERT( !strDirectory.empty() );
   CReply Reply;
   if( !SendCommand(_T("CWD ")+strDirectory, Reply) )
      return FTP_ERROR;
   return SimpleErrorCheck(Reply);
}

/// Executes the ftp command MKD (MAKE DIRECTORY)
/// This command causes the directory specified in the pathname to be created 
/// as a directory (if the pathname is absolute) or as a subdirectory of the 
/// current working directory (if the pathname is relative).
/// @pararm[in] strDirectory Pathname specifying a directory to be created.
/// @return see return values of CFTPClient::SimpleErrorCheck
int CFTPClient::MakeDirectory(const tstring& strDirectory) const
{
   ASSERT( !strDirectory.empty() );
   CReply Reply;
   if( !SendCommand(_T("MKD ")+strDirectory, Reply) )
      return FTP_ERROR;
   return SimpleErrorCheck(Reply);
}

/// Executes the ftp command SITE (SITE PARAMETERS)
/// This command is used by the server to provide services specific to his 
/// system that are essential to file transfer but not sufficiently universal 
/// to be included as commands in the protocol.  The nature of these services 
/// and the specification of their syntax can be stated in a reply to the HELP 
/// SITE command.
/// @param[in] strCmd Command to be executed.
/// @return see return values of CFTPClient::SimpleErrorCheck
int CFTPClient::SiteParameters(const tstring& strCmd) const
{
   ASSERT( !strCmd.empty() );
   CReply Reply;
   if( !SendCommand(_T("SITE ")+strCmd, Reply) )
      return FTP_ERROR;
   return SimpleErrorCheck(Reply);
}

/// Executes the ftp command HELP
/// This command shall cause the server to send helpful information regarding 
/// its implementation status over the control connection to the user.
/// The command may take an argument (e.g., any command name) and return more 
/// specific information as a response.  The reply is type 211 or 214.
/// It is suggested that HELP be allowed before entering a USER command. The 
/// server may use this reply to specify site-dependent parameters, e.g., in 
/// response to HELP SITE.
/// @param[in] strTopic Topic of the requested help.
/// @return see return values of CFTPClient::SimpleErrorCheck
int CFTPClient::Help(const tstring& strTopic) const
{
   CReply Reply;
   if( !SendCommand(strTopic.empty()?_T("HELP"):_T("HELP ")+strTopic, Reply) )
      return FTP_ERROR;
   return SimpleErrorCheck(Reply);
}

/// Executes the ftp command DELE (DELETE)
/// This command causes the file specified in the pathname to be deleted at the 
/// server site.  If an extra level of protection is desired (such as the query, 
/// "Do you really wish to delete?"), it should be provided by the user-FTP process.
/// @param[in] strFile Pathname of the file to delete.
/// @return see return values of CFTPClient::SimpleErrorCheck
int CFTPClient::Delete(const tstring& strFile) const
{
   ASSERT( !strFile.empty() );
   CReply Reply;
   if( !SendCommand(_T("DELE ")+strFile, Reply) )
      return FTP_ERROR;
   return SimpleErrorCheck(Reply);
}

/// Executes the ftp command RMD (REMOVE DIRECTORY)
/// This command causes the directory specified in the pathname to be removed 
/// as a directory (if the pathname is absolute) or as a subdirectory of the 
/// current working directory (if the pathname is relative).
/// @param[in] strDirectory Pathname of the directory to delete.
/// @return see return values of CFTPClient::SimpleErrorCheck
int CFTPClient::RemoveDirectory(const tstring& strDirectory) const
{
   ASSERT( !strDirectory.empty() );
   CReply Reply;
   if( !SendCommand(_T("RMD ")+strDirectory, Reply) )
      return FTP_ERROR;
   return SimpleErrorCheck(Reply);
}

/// Executes the ftp command STRU (FILE STRUCTURE)
/// see documentation of nsFTP::CStructure
/// The default structure is File.
/// @param[in] crStructure see Documentation of nsFTP::CStructure
/// @return see return values of CFTPClient::SimpleErrorCheck
int CFTPClient::FileStructure(const CStructure& crStructure) const
{
   tstring strStructure;
   switch( crStructure.AsEnum() )
   {
   case CStructure::scFile:   strStructure=_T("F"); break;
   case CStructure::scRecord: strStructure=_T("R"); break;
   case CStructure::scPage:   strStructure=_T("P"); break;
   default:
      ASSERT( false );
      return FTP_ERROR;
   }

   CReply Reply;
   if( !SendCommand(_T("STRU ")+strStructure, Reply) )
      return FTP_ERROR;
   return SimpleErrorCheck(Reply);
}

/// Executes the ftp command MODE (TRANSFER MODE)
/// see documentation of nsFTP::CTransferMode
/// The default transfer mode is Stream.
/// @param[in] crTransferMode see Documentation of nsFTP::CTransferMode
/// @return see return values of CFTPClient::SimpleErrorCheck
int CFTPClient::TransferMode(const CTransferMode& crTransferMode) const
{
   tstring strMode;
   switch( crTransferMode.AsEnum() )
   {
   case CTransferMode::tmStream:      strMode=_T("S"); break;
   case CTransferMode::tmBlock:       strMode=_T("B"); break;
   case CTransferMode::tmCompressed:  strMode=_T("C"); break;
   default:
      ASSERT( false );
      return FTP_ERROR;
   }
   
   CReply Reply;
   if( !SendCommand(_T("MODE ")+strMode, Reply) )
      return FTP_ERROR;
   return SimpleErrorCheck(Reply);
}

/// Executes the ftp command STAT (STATUS)
/// This command shall cause a status response to be sent over the control 
/// connection in the form of a reply. The command may be sent during a file 
/// transfer (along with the Telnet IP and Synch signals--see the Section on 
/// FTP Commands) in which case the server will respond with the status of the
/// operation in progress, or it may be sent between file transfers. In the 
/// latter case, the command may have an argument field. 
/// @param[in] strPath If the argument is a pathname, the command is analogous 
///                    to the "list" command except that data shall be transferred 
///                    over the control connection. If a partial pathname is 
///                    given, the server may respond with a list of file names or 
///                    attributes associated with that specification. If no argument 
///                    is given, the server should return general status information 
///                    about the server FTP process. This should include current 
///                    values of all transfer parameters and the status of connections.
/// @return see return values of CFTPClient::SimpleErrorCheck
int CFTPClient::Status(const tstring& strPath) const
{
   CReply Reply;
   if( !SendCommand(strPath.empty()?_T("STAT"):_T("STAT ")+strPath, Reply) )
      return FTP_ERROR;
   return SimpleErrorCheck(Reply);
}

/// Executes the ftp command ALLO (ALLOCATE)
/// This command may be required by some servers to reserve sufficient storage 
/// to accommodate the new file to be transferred.
/// @param[in] iReserveBytes The argument shall be a decimal integer representing 
///                          the number of bytes (using the logical byte size) of 
///                          storage to be reserved for the file. For files sent 
///                          with record or page structure a maximum record or page
///                          size (in logical bytes) might also be necessary; this 
///                          is indicated by a decimal integer in a second argument 
///                          field of the command.
/// @pararm[in] piMaxPageOrRecordSize This second argument is optional. This command 
///                          shall be followed by a STORe or APPEnd command.
///                          The ALLO command should be treated as a NOOP (no operation)
///                          by those servers which do not require that the maximum 
///                          size of the file be declared beforehand, and those servers 
///                          interested in only the maximum record or page size should 
///                          accept a dummy value in the first argument and ignore it.
/// @return see return values of CFTPClient::SimpleErrorCheck
int CFTPClient::Allocate(int iReserveBytes, const int* piMaxPageOrRecordSize/*=NULL*/) const
{
   tstring strCmd;
   if( piMaxPageOrRecordSize==NULL )
      strCmd = CMakeString() << _T("ALLO ") << iReserveBytes;
   else
      strCmd = CMakeString() << _T("ALLO ") << iReserveBytes << _T(" R ") << *piMaxPageOrRecordSize;

   CReply Reply;
   if( !SendCommand(strCmd, Reply) )
      return FTP_ERROR;
   return SimpleErrorCheck(Reply);
}

/// Executes the ftp command SMNT ()
/// @return see return values of CFTPClient::SimpleErrorCheck
int CFTPClient::StructureMount(const tstring& strPath) const
{
   CReply Reply;
   if( !SendCommand(_T("SMNT ")+strPath, Reply) )
      return FTP_ERROR;
   return SimpleErrorCheck(Reply);
}

/// Executes the ftp command (STRUCTURE MOUNT)
/// This command allows the user to mount a different file system data structure 
/// without altering his login or accounting information. Transfer parameters 
/// are similarly unchanged.  The argument is a pathname specifying a directory 
/// or other system dependent file group designator.
/// @return see return values of CFTPClient::SimpleErrorCheck
int CFTPClient::Reinitialize() const
{
   CReply Reply;
   if( !SendCommand(_T("REIN"), Reply) )
      return FTP_ERROR;

   if( Reply.Code().IsPositiveCompletionReply() )
      return FTP_OK;
   else if( Reply.Code().IsPositivePreliminaryReply() )
   {
      if( !GetResponse(Reply) || !Reply.Code().IsPositiveCompletionReply() )
         return FTP_ERROR;
   }
   else if( Reply.Code().IsNegativeReply() )
      return FTP_NOTOK;

   ASSERT( Reply.Code().IsPositiveIntermediateReply() );
   return FTP_ERROR;
}

/// Executes the ftp command REST (RESTART)
/// This command does not cause file transfer but skips over the file to the 
/// specified data checkpoint. This command shall be immediately followed
/// by the appropriate FTP service command which shall cause file transfer 
/// to resume.
/// @param[in] dwPosition Represents the server marker at which file transfer 
///                       is to be restarted.
/// @return see return values of CFTPClient::SimpleErrorCheck
int CFTPClient::Restart(DWORD dwPosition) const
{
   tstring strCmd;
   strCmd = CMakeString() << _T("REST ") << dwPosition;

   CReply Reply;
   if( !SendCommand(strCmd, Reply) )
      return FTP_ERROR;

   if( Reply.Code().IsPositiveIntermediateReply() )
      return FTP_OK;
   else if( Reply.Code().IsNegativeReply() )
      return FTP_NOTOK;

   ASSERT( Reply.Code().IsPositiveReply() );

   return FTP_ERROR;
}

/// Executes the ftp command SIZE
/// Return size of file.
/// SIZE is not specified in RFC 959.
/// @param[in] Pathname of a file.
/// @param[out] Size of the file specified in pathname.
/// @return see return values of CFTPClient::SimpleErrorCheck
int CFTPClient::FileSize(const tstring& strPath, long& lSize) const
{
   CReply Reply;
   if( !SendCommand(_T("SIZE ")+strPath, Reply) )
      return FTP_ERROR;
   lSize = CCnv::TStringToLong(Reply.Value().substr(4).c_str());
   return SimpleErrorCheck(Reply);
}

/// Executes the ftp command MDTM
/// Show last modification time of file.
/// MDTM is not specified in RFC 959.
/// @param[in] strPath Pathname of a file.
/// @param[out] strModificationTime Modification time of the file specified in pathname.
/// @return see return values of CFTPClient::SimpleErrorCheck
int CFTPClient::FileModificationTime(const tstring& strPath, tstring& strModificationTime) const
{
   strModificationTime.erase();

   CReply Reply;
   if( !SendCommand(_T("MDTM ")+strPath, Reply) )
      return FTP_ERROR;

   if( Reply.Value().length()>=18 )
   {
      tstring strTemp(Reply.Value().substr(4));
      size_t iPos=strTemp.find(_T('.'));
      if( iPos>-1 )
         strTemp = strTemp.substr(0, iPos);
      if( strTemp.length()==14 )
         strModificationTime=strTemp;
   }

   if( strModificationTime.empty() )
      return FTP_ERROR;

   return SimpleErrorCheck(Reply);
}

/// Show last modification time of file.
/// @param[in] strPath Pathname of a file.
/// @param[out] tmModificationTime Modification time of the file specified in pathname.
/// @return see return values of CFTPClient::SimpleErrorCheck
int CFTPClient::FileModificationTime(const tstring& strPath, tm& tmModificationTime) const
{
   tstring strTemp;
   const int iRet = FileModificationTime(strPath, strTemp);

   memset(&tmModificationTime, 0, sizeof(tmModificationTime));
   if( iRet==FTP_OK )
   {
      tmModificationTime.tm_year = CCnv::TStringToLong(strTemp.substr(0, 4).c_str());
      tmModificationTime.tm_mon  = CCnv::TStringToLong(strTemp.substr(4, 2).c_str());
      tmModificationTime.tm_mday = CCnv::TStringToLong(strTemp.substr(6, 2).c_str());
      tmModificationTime.tm_hour = CCnv::TStringToLong(strTemp.substr(8, 2).c_str());
      tmModificationTime.tm_min  = CCnv::TStringToLong(strTemp.substr(10, 2).c_str());
      tmModificationTime.tm_sec  = CCnv::TStringToLong(strTemp.substr(12).c_str());
   }
   return iRet;
}

/// Notifies all observers that an error occurred.
/// @param[in] strErrorMsg Error message which is reported to all observers.
/// @param[in] Name of the sourcecode file where the error occurred.
/// @param[in] Line number in th sourcecode file where the error occurred.
void CFTPClient::ReportError(const tstring& strErrorMsg, const tstring& strFile, DWORD dwLineNr) const
{
   for( TObserverSet::const_iterator it=m_setObserver.begin(); it!=m_setObserver.end(); ++it )
      (*it)->OnInternalError(strErrorMsg, strFile, dwLineNr);
}

}