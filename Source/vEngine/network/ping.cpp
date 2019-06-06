//-----------------------------------------------------------------------------
// rawping.cpp - Contains all of the functions essential to sending "ping"
//    packets using Winsock 2 raw sockets.  Depends on ip_checksum.cpp for 
//    calculating IP-style checksums on blocks of data, however.
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "ping.h"

// 使用其它部件
#include "..\console\console_gui.h"


namespace vEngine {
// ICMP packet types
CONST INT ICMP_ECHO_REPLY		= 0;
CONST INT ICMP_DEST_UNREACH		= 3;
CONST INT ICMP_TTL_EXPIRE		= 11;
CONST INT ICMP_ECHO_REQUEST		= 8;
CONST INT ICMP_MIN				= 8;	// Minimum ICMP packet size, in bytes
CONST INT DEFAULT_PACKET_SIZE	= 32;
CONST INT DEFAULT_TTL			= 128;
CONST INT MAX_PING_DATA_SIZE	= 1024;
CONST INT MAX_PING_PACKET_SIZE	= (MAX_PING_DATA_SIZE + sizeof(Ping::IPHeader));
//-----------------------------------------------------------------------------
// construction / destruction
//-----------------------------------------------------------------------------
Ping::Ping():m_Trunk(this)
{
	m_dwTimeOut = 0;
	m_bUseRawSocket = TRUE;
	m_bTerminateAllPingThread = FALSE;
	m_hPingService = 0;
	m_hICMP = 0;

	vEngine::TObjRef<Console> pConsole;
	pConsole->Register(_T("Ping"), m_Trunk.sfp1(&vEngine::Ping::StartPing), _T("ping IP"));
	pConsole->Register(_T("ispinging"), m_Trunk.sfp1(&vEngine::Ping::IsPingFinished), _T("is ping finished"));
	pConsole->Register(_T("getping"), m_Trunk.sfp1(&vEngine::Ping::GetPingResult), _T("get ping result"));
}

Ping::~Ping()
{
	this->Destroy();
}

//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL Ping::Init(DWORD dwTimeOut)
{
	m_dwTimeOut = dwTimeOut;
	m_bUseRawSocket = TRUE;
	m_bTerminateAllPingThread = FALSE;
	m_vecThreadHandle.clear();
	m_nSeqNum = 0;
	m_mapPingResult.Clear(); // 与SeqNum对应的ping时间

	// load icmp.dll
	// Load the ICMP.DLL
	m_hICMP = LoadLibrary(_T("ICMP.DLL"));
	if( m_hICMP == 0 ) 
	{
		ERR(_T("Unable to locate ICMP.DLL!"));
		return FALSE;
	}

	// Get handles to the functions inside ICMP.DLL that we'll need
	m_pIcmpCreateFile = (pfnHV)GetProcAddress(m_hICMP, "IcmpCreateFile");
	m_pIcmpCloseHandle = (pfnBH)GetProcAddress(m_hICMP, "IcmpCloseHandle");
	m_pIcmpSendEcho = (pfnDHDPWPipPDD)GetProcAddress(m_hICMP, "IcmpSendEcho");

	if( !m_pIcmpCreateFile || !m_pIcmpCloseHandle || !m_pIcmpSendEcho ) 
	{
		ERR(_T("Failed to get proc addr for function."));
		return FALSE;
	}

	// Open the ping service
	m_hPingService = m_pIcmpCreateFile();
	if( m_hPingService == INVALID_HANDLE_VALUE )
	{
		ERR(_T("Unable to open ICMP ping service."));
		return FALSE;
	}

    return TRUE;
}

//----------------------------------------------------------------------------- 
// Destroy
//-----------------------------------------------------------------------------
VOID Ping::Destroy()
{
	// 设置结束线程标志
	::InterlockedExchange((LONG*)&m_bTerminateAllPingThread, TRUE);
	for( INT n=0; n<(INT)m_vecThreadHandle.size(); n++ )
	{
		WaitForSingleObject(m_vecThreadHandle[n], 1000);
		SAFE_CLOSE_HANDLE(m_vecThreadHandle[n]);
	}

	if( FALSE == m_bUseRawSocket )
	{
		if( m_hPingService )
			m_pIcmpCloseHandle(m_hPingService);
		
		if( m_hICMP )
			FreeLibrary(m_hICMP);

		m_hPingService = 0;
		m_hICMP = 0;
	}
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
INT Ping::StartPing(LPCSTR szIP)
{
	ASSERT(szIP);
	m_nSeqNum++;

	tagThreadData* pThreadData = new tagThreadData;
	if( NULL == pThreadData )
		return GT_INVALID ;

	strncpy(pThreadData->szIP, szIP, 40);
	pThreadData->nSeqNum = m_nSeqNum;

	// create send and recv ping thread
	HANDLE hPingThreadHandle = (HANDLE)_beginthreadex
		(NULL, 0, (THREADPROC)m_Trunk.sfp1(&vEngine::Ping::ThreadPing), 
		pThreadData, 0, NULL);

	if( hPingThreadHandle  ) // _beginthreadex return 0 on failed
	{//pThreadData由线程负责释放
		m_vecThreadHandle.push_back(hPingThreadHandle);
		return m_nSeqNum;
	}

	SAFE_DEL(pThreadData);
	return GT_INVALID;

}


//-----------------------------------------------------------------------------
//! 强制关闭所有ping的线程,注意这样也会删除已经ping得的结果
//-----------------------------------------------------------------------------
VOID Ping::ForcedCloseAllPing()
{
	// 设置结束线程标志
	::InterlockedExchange((LONG*)&m_bTerminateAllPingThread, TRUE);
	for( INT n=0; n<(INT)m_vecThreadHandle.size(); n++ )
	{
		WaitForSingleObject(m_vecThreadHandle[n], 1000*10);
		SAFE_CLOSE_HANDLE(m_vecThreadHandle[n]);
	}

	
	m_vecThreadHandle.clear();
	m_nSeqNum = 0;
	m_mapPingResult.Clear(); // 与SeqNum对应的ping时间
	m_bTerminateAllPingThread = FALSE;
}


//-----------------------------------------------------------------------------
// 检查ping是否结束
//-----------------------------------------------------------------------------
BOOL Ping::IsPingFinished(INT nHandle)
{
	if( FALSE == m_mapPingResult.IsExist(nHandle) )
		return FALSE;

	return TRUE;
}

//-----------------------------------------------------------------------------
// 成功返回ping时间,失败返回GT_INVALID
// 失败情况分两种:1.可能ping尚未结束 2.远程主机无法ping通
//-----------------------------------------------------------------------------
INT Ping::GetPingResult(INT nHandle)
{
	if( FALSE == m_mapPingResult.IsExist(nHandle) )
		return GT_INVALID;

	// 注意,下一句也可能返回GT_INVALID,因为可能远程主机无法ping通
	return m_mapPingResult.Peek(nHandle);

}


//-----------------------------------------------------------------------------
// ping线程
//-----------------------------------------------------------------------------
UINT Ping::ThreadPing(LPVOID pVoid)
{
	// 获取线程初始数据
	tagThreadData* pThreadData = (tagThreadData*)pVoid;
	INT nSeqData = pThreadData->nSeqNum;
	char szIP[40];
	memset(szIP, 0, 40);
	strncpy(szIP, pThreadData->szIP, 39);
	SAFE_DEL(pThreadData);

	// Set up for pinging
	SOCKET sock;
	sockaddr_in dest;
	ZeroMemory(&dest, sizeof(dest));
	if( SetupForPing(szIP, sock, dest) < 0 )
		return GT_INVALID;
	DWORD dwAddr = dest.sin_addr.S_un.S_addr;
	// 计算重试次数, 其中GT_NET_BLOCK_TIME是以微秒为单位
	INT nRetryTimes = m_dwTimeOut / (GT_NET_BLOCK_TIME/1000);

	if( m_bUseRawSocket )
	{
		//----------------------------------
		// first init the rcv buffer
		sockaddr_in source;
		ZeroMemory(&source, sizeof(source));
		int len = sizeof(source);
		INT packet_size = max(sizeof(ICMPHeader), DEFAULT_PACKET_SIZE);
		TCHAR szRcvTemp[MAX_PING_PACKET_SIZE];
		IPHeader* recv_buf = (IPHeader*)szRcvTemp;


		// then init the send buffer
		TCHAR szSendTemp[DEFAULT_PACKET_SIZE];
		ICMPHeader* send_buf = (ICMPHeader*)szSendTemp;
		InitPingPacket(send_buf, DEFAULT_PACKET_SIZE, m_nSeqNum);
		// send ping
		if( SendPing(sock, dest, send_buf, DEFAULT_PACKET_SIZE ) < 0 )
		{
			return GT_INVALID;
		}
		//receive
		while( FALSE == m_bTerminateAllPingThread && nRetryTimes-- >= 0 )
		{
			// 设置阻塞时间
			fd_set fdread;

			timeval block_time;
			block_time.tv_sec = 0;
			block_time.tv_usec = GT_NET_BLOCK_TIME;

			// "select" will change fdread, so this must put in the loop
			// Wait for the ping reply
			FD_ZERO(&fdread);
			FD_SET(sock, &fdread);
			INT nResult = select(0, &fdread, NULL, NULL, &block_time);

			if( nResult == 1 )
			{
				nResult = recvfrom(sock, (char*)recv_buf, 
					MAX_PING_PACKET_SIZE + sizeof(IPHeader), 0, (sockaddr*)&source, &len);

				unsigned short header_len = recv_buf->h_len * 4;
				ICMPHeader* icmphdr = (ICMPHeader*)
					((char*)recv_buf + header_len);

				INT nPackageSeq = icmphdr->seq;
				if( nPackageSeq != nSeqData )
					continue;
	
				nResult = DecodeReply(recv_buf, packet_size, &source);

				//存放结果后退出
				m_mapPingResult.Add(nPackageSeq, nResult);
				closesocket(sock);
				_endthreadex(0);
				return 0;
			}

			if( SOCKET_ERROR == nResult )
			{
				ERR(_T("recvfrom failed in ping thread"));
				break;
			}
		}
		
		m_mapPingResult.Add(nSeqData, GT_INVALID);
		closesocket(sock);
		_endthreadex(GT_INVALID);
		return GT_INVALID;
	}
	
	else
	{
		// Build ping packet
		char acPingBuffer[64];
		memset(acPingBuffer, '\xAA', sizeof(acPingBuffer));
		PIP_ECHO_REPLY pIpe = (PIP_ECHO_REPLY)GlobalAlloc(
			GMEM_FIXED | GMEM_ZEROINIT,	sizeof(IP_ECHO_REPLY) + sizeof(acPingBuffer));
		if( pIpe == 0 ) 
		{
			ERR(_T("Failed to allocate global ping packet buffer"));
			return GT_INVALID;
		}

		pIpe->Data = acPingBuffer;
		pIpe->DataSize = sizeof(acPingBuffer);      

		// Send the ping packet
		DWORD dwStatus = m_pIcmpSendEcho(m_hPingService, dwAddr, 
			acPingBuffer, sizeof(acPingBuffer), NULL, pIpe, 
			sizeof(IP_ECHO_REPLY) + sizeof(acPingBuffer), m_dwTimeOut);
		if( dwStatus != 0 ) 
		{
			IMSG(_T("Addr: %d.%d.%d.%d, RTT:%dms, TTL:%d\r\n"),
				int(LOBYTE(LOWORD(pIpe->Address))),
				int(HIBYTE(LOWORD(pIpe->Address))),
				int(LOBYTE(HIWORD(pIpe->Address))),
				int(HIBYTE(HIWORD(pIpe->Address))),
				int(pIpe->RoundTripTime), int(pIpe->Options.Ttl));

			//存放结果后退出
			m_mapPingResult.Add(nSeqData, (INT)pIpe->RoundTripTime);
			GlobalFree(pIpe);
			_endthreadex(0);
			return 0;
		}

		IMSG(_T("Error obtaining info from ping packet.\r\n"));
		m_mapPingResult.Add(nSeqData, GT_INVALID);
		GlobalFree(pIpe);
		_endthreadex(GT_INVALID);
		return GT_INVALID;
	}
}

//-----------------------------------------------------------------------------
// Creates the Winsock structures necessary for sending and recieving
// ping packets.  host can be either a dotted-quad IP address, or a
// host name.  ttl is the time to live (a.k.a. number of hops) for the
// packet.  The other two parameters are outputs from the function.
// Returns < 0 for failure.
//-----------------------------------------------------------------------------
INT Ping::SetupForPing(LPCSTR host, SOCKET& sd, sockaddr_in& dest)
{
    // Create the socket
    sd = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, 0, 0, 0);
    if( sd == INVALID_SOCKET )
	{
        ERR(_T("Failed to create raw socket"));
        return GT_INVALID;
    }

	INT ttl = DEFAULT_TTL;
    if( setsockopt(sd, IPPROTO_IP, IP_TTL, (const char*)&ttl, 
            sizeof(ttl)) == SOCKET_ERROR) 
	{
        ERR(_T("TTL setsockopt failed"));
        return GT_INVALID;
    }

    // Initialize the destination host info block
    memset(&dest, 0, sizeof(dest));

    // Turn first passed parameter into an IP address to ping
    unsigned int addr = inet_addr(host);
    if (addr != INADDR_NONE) 
	{
        // It was a dotted quad number, so save result
        dest.sin_addr.s_addr = addr;
        dest.sin_family = AF_INET;
    }
    else 
	{
        // Not in dotted quad form, so try and look it up
        hostent* hp = gethostbyname(host);
        if (hp != 0) 
		{
            // Found an address for that host, so save it
            memcpy(&(dest.sin_addr), hp->h_addr, hp->h_length);
            dest.sin_family = hp->h_addrtype;
        }
        else 
		{
            // Not a recognized hostname either!
            ERR(_T("Failed to resolve"));
            return GT_INVALID;
        }
    }

    return 0;
}



//-----------------------------------------------------------------------------
// Fill in the fields and data area of an ICMP packet, making it 
// packet_size bytes by padding it with a byte pattern, and giving it
// the given sequence number.  That completes the packet, so we also
// calculate the checksum for the packet and place it in the appropriate
// field.
//-----------------------------------------------------------------------------
VOID Ping::InitPingPacket(ICMPHeader* icmp_hdr, INT packet_size, INT seq_no)
{
    // Set up the packet's fields
    icmp_hdr->type = ICMP_ECHO_REQUEST;
    icmp_hdr->code = 0;
    icmp_hdr->checksum = 0;
    icmp_hdr->id = (USHORT)GetCurrentProcessId();
    icmp_hdr->seq = seq_no;
    icmp_hdr->timestamp = GetTickCount();

    // "You're dead meat now, packet!"
    const unsigned long int deadmeat = 0xDEADBEEF;
    char* datapart = (char*)icmp_hdr + sizeof(ICMPHeader);
    int bytes_left = packet_size - sizeof(ICMPHeader);
    while( bytes_left > 0 )
	{
        memcpy(datapart, &deadmeat, min(int(sizeof(deadmeat)), bytes_left));
        bytes_left -= sizeof(deadmeat);
        datapart += sizeof(deadmeat);
    }

    // Calculate a checksum on the result
    icmp_hdr->checksum = IpChecksum((USHORT*)icmp_hdr, packet_size);
}


//-----------------------------------------------------------------------------
// Send an ICMP echo ("ping") packet to host dest by way of sd with
// packet_size bytes.  packet_size is the total size of the ping packet
// to send, including the ICMP header and the payload area; it is not
// checked for sanity, so make sure that it's at least 
// sizeof(ICMPHeader) bytes, and that send_buf points to at least
// packet_size bytes.  Returns < 0 for failure.
//-----------------------------------------------------------------------------
INT Ping::SendPing(SOCKET sd, const sockaddr_in& dest, ICMPHeader* send_buf, 
					INT packet_size)
{
    // Send the ping packet in send_buf as-is
    int bwrote = sendto(sd, (char*)send_buf, packet_size, 0, 
            (sockaddr*)&dest, sizeof(dest));
    
	if( bwrote == SOCKET_ERROR ) 
	{
        ERR(_T("send failed"));
        return GT_INVALID;
    }
    else if (bwrote < packet_size) 
	{
        IMSG(_T("sent %d bytes\r\n"), bwrote);
    }

    return 0;
}



//-----------------------------------------------------------------------------
// Decode and output details about an ICMP reply packet.  Returns -1
// on failure, -2 on "try again" and 0 on success.
//-----------------------------------------------------------------------------
INT Ping::DecodeReply(IPHeader* reply, INT bytes, sockaddr_in* from) 
{
    // Skip ahead to the ICMP header within the IP packet
    unsigned short header_len = reply->h_len * 4;
    ICMPHeader* icmphdr = (ICMPHeader*)((char*)reply + header_len);

    // Make sure the reply is sane
    if (bytes < header_len + ICMP_MIN) 
	{
        ERR(_T("too few bytes from %s"), inet_ntoa(from->sin_addr));
        return GT_INVALID;
    }
    else if( icmphdr->type != ICMP_ECHO_REPLY )
	{
        if( icmphdr->type != ICMP_TTL_EXPIRE ) 
		{
            if( icmphdr->type == ICMP_DEST_UNREACH )
			{
                IMSG(_T("Destination unreachable\r\n"));
            }
            else 
			{
                ERR(_T("Unknown ICMP packet type %d received"), int(icmphdr->type));
            }
            return GT_INVALID;
        }
        // If "TTL expired", fall through.  Next test will fail if we
        // try it, so we need a way past it.
    }
    else if( icmphdr->id != (USHORT)GetCurrentProcessId() ) 
	{
        // Must be a reply for another pinger running locally, so just
        // ignore it.
        return -2;
    }

    // Okay, we ran the gamut, so the packet must be legal -- dump it
    IMSG(_T("%d bytes from %s icmp_seq %d\r\n"), bytes, inet_ntoa(from->sin_addr), icmphdr->seq);
	INT nTime = GetTickCount() - icmphdr->timestamp;
    if (icmphdr->type == ICMP_TTL_EXPIRE) 
	{
        IMSG(_T("Ping TTL expired.\r\n"));
    }
    else
	{
		IMSG(_T("TTL:%d, time:%d ms\r\n"), reply->ttl, nTime);
    }

    return nTime;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
USHORT Ping::IpChecksum(USHORT* buffer, INT size) 
{
    unsigned long cksum = 0;
    
    // Sum all the words together, adding the final byte if size is odd
    while (size > 1) {
        cksum += *buffer++;
        size -= sizeof(USHORT);
    }
    if (size) {
        cksum += *(UCHAR*)buffer;
    }

    // Do a little shuffling
    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >> 16);
    
    // Return the bitwise complement of the resulting mishmash
    return (USHORT)(~cksum);
}

INT Ping::AllocateBuffers(ICMPHeader*& send_buf, IPHeader*& recv_buf, INT packet_size)
{


    
    return 0;
}



} // namespace vEngine {
