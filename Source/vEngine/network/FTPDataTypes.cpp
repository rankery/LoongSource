////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////

#include "..\stdafx.h"
#include "FTPDataTypes.h"

namespace vEngine {
using namespace nsFTP;

/// returns the string which is used for display
tstring CFirewallType::AsDisplayString() const
{
   switch( m_enFirewallType )
   {
   case ftNone:                              return _T("no firewall");
   case ftSiteHostName:                      return _T("SITE hostname");
   case ftUserAfterLogon:                    return _T("USER after logon");
   case ftProxyOpen:                         return _T("proxy OPEN");
   case ftTransparent:                       return _T("Transparent");
   case ftUserWithNoLogon:                   return _T("USER with no logon");
   case ftUserFireIDatRemotehost:            return _T("USER fireID@remotehost");
   case ftUserRemoteIDatRemoteHostFireID:    return _T("USER remoteID@remotehost fireID");
   case ftUserRemoteIDatFireIDatRemoteHost:  return _T("USER remoteID@fireID@remotehost");
   }
   ASSERT( false );
   return _T("");
}

/// return the string which is used for storage (e.g. in an XML- or INI-file)
tstring CFirewallType::AsStorageString() const
{
   switch( m_enFirewallType )
   {
   case ftNone:                              return _T("NO_FIREWALL");
   case ftSiteHostName:                      return _T("SITE_HOSTNAME");
   case ftUserAfterLogon:                    return _T("USER_AFTER_LOGON");
   case ftProxyOpen:                         return _T("PROXY_OPEN");
   case ftTransparent:                       return _T("TRANSPARENT");
   case ftUserWithNoLogon:                   return _T("USER_WITH_NO_LOGON");
   case ftUserFireIDatRemotehost:            return _T("USER_FIREID@REMOTEHOST");
   case ftUserRemoteIDatRemoteHostFireID:    return _T("USER_REMOTEID@REMOTEHOST_FIREID");
   case ftUserRemoteIDatFireIDatRemoteHost:  return _T("USER_REMOTEID@FIREID@REMOTEHOST");
   }
   ASSERT( false );
   return _T("");
}

/// returns all available firewall types
void CFirewallType::GetAllTypes(TFirewallTypeVector& vTypes)
{
   vTypes.resize(9);
   vTypes[0] = ftNone;
   vTypes[1] = ftSiteHostName;
   vTypes[2] = ftUserAfterLogon;
   vTypes[3] = ftProxyOpen;
   vTypes[4] = ftTransparent;
   vTypes[5] = ftUserWithNoLogon;
   vTypes[6] = ftUserFireIDatRemotehost;
   vTypes[7] = ftUserRemoteIDatRemoteHostFireID;
   vTypes[8] = ftUserRemoteIDatFireIDatRemoteHost;
}

CLogonInfo::CLogonInfo() :
   m_ushHostport(DEFAULT_FTP_PORT),
   m_strUsername(ANONYMOUS_USER),
   m_ushFwPort(DEFAULT_FTP_PORT),
   m_FwType(CFirewallType::None())
{
}

CLogonInfo::CLogonInfo(const tstring& strHostname, USHORT ushHostport, const tstring& strUsername, 
                       const tstring& strPassword, const tstring& strAccount) :
   m_strHostname(strHostname),
   m_ushHostport(ushHostport),
   m_strUsername(strUsername),
   m_strPassword(strPassword),
   m_strAccount(strAccount),
   m_ushFwPort(DEFAULT_FTP_PORT),
   m_FwType(CFirewallType::None())
{
}

CLogonInfo::CLogonInfo(const tstring& strHostname, USHORT ushHostport, const tstring& strUsername, const tstring& strPassword,
                       const tstring& strAccount, const tstring& strFwHostname, const tstring& strFwUsername, 
                       const tstring& strFwPassword, USHORT ushFwPort, const CFirewallType& crFwType) :
   m_strHostname(strHostname),
   m_ushHostport(ushHostport),
   m_strUsername(strUsername),
   m_strPassword(strPassword),
   m_strAccount(strAccount),
   m_strFwHostname(strFwHostname),
   m_strFwUsername(strFwUsername),
   m_strFwPassword(strFwPassword),
   m_ushFwPort(ushFwPort),
   m_FwType(crFwType)
{
}

void CLogonInfo::SetHost(const tstring& strHostname, USHORT ushHostport, const tstring& strUsername, 
                         const tstring& strPassword, const tstring& strAccount)
{
   m_strHostname  = strHostname;
   m_ushHostport  = ushHostport;
   m_strUsername  = strUsername;
   m_strPassword  = strPassword;
   m_strAccount   = strAccount;
}

void CLogonInfo::SetFirewall(const tstring& strFwHostname, const tstring& strFwUsername, const tstring& strFwPassword,
                             USHORT ushFwPort, const CFirewallType& crFwType)
{
   m_strFwHostname   = strFwHostname;
   m_strFwUsername   = strFwUsername;
   m_strFwPassword   = strFwPassword;
   m_ushFwPort       = ushFwPort;
   m_FwType          = crFwType;
}

}