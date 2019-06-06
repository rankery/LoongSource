/***********************************************************************
 rawping.h - Declares the types, constants and prototypes required to
    use the rawping.cpp module.
***********************************************************************/
#pragma once
namespace vEngine {

//-----------------------------------------------------------------------------
// 两种版本的ping for winsock1.1 and winsock2
//-----------------------------------------------------------------------------
class VENGINE_API Ping
{
public:
	//! dwTimeOut = ping超时,毫秒为单位
	//! bAutoUseSocket2 = 是否自动使用socket2
	BOOL Init(DWORD dwTimeOut);
	VOID Destroy();

	 //! Ping返回handle,失败返回GT_INVALID
	INT StartPing(LPCSTR szIP);
	//! 强制关闭所有ping的线程,注意这样也会删除已经ping得的结果
	VOID ForcedCloseAllPing();
	//! 检查ping是否结束;
	BOOL IsPingFinished(INT nHandle);
	//! 成功返回ping时间,失败返回GT_INVALID
	//! 失败情况分两种:1.可能ping尚未结束 2.远程主机无法ping通
	INT GetPingResult(INT nHandle);

	Ping();
	~Ping();



#pragma pack(1)
	// The IP header
	struct IPHeader {
		BYTE h_len:4;           // Length of the header in dwords
		BYTE version:4;         // Version of IP, 4 bits
		BYTE tos;               // Type of service
		USHORT total_len;       // Length of the packet in dwords
		USHORT ident;           // unique identifier
		USHORT flags;           // Flags
		BYTE ttl;               // Time to live
		BYTE proto;             // Protocol number (TCP, UDP etc)
		USHORT checksum;        // IP checksum
		ULONG source_ip;
		ULONG dest_ip;
	};
#pragma pack()


private:	// ------------------------------------------------------------
#pragma pack(1)
	// ICMP header
	struct ICMPHeader {
		BYTE type;          // ICMP packet type
		BYTE code;          // Type sub code
		USHORT checksum;
		USHORT id;
		USHORT seq;
		ULONG timestamp;    // not part of ICMP, but we need it
	};

	typedef struct {
		unsigned char Ttl;                         // Time To Live
		unsigned char Tos;                         // Type Of Service
		unsigned char Flags;                       // IP header flags
		unsigned char OptionsSize;                 // Size in bytes of options data
		unsigned char *OptionsData;                // Pointer to options data
	} IP_OPTION_INFORMATION, * PIP_OPTION_INFORMATION;

	typedef struct {
		DWORD Address;                             // Replying address
		unsigned long  Status;                     // Reply status
		unsigned long  RoundTripTime;              // RTT in milliseconds
		unsigned short DataSize;                   // Echo data size
		unsigned short Reserved;                   // Reserved for system use
		void *Data;                                // Pointer to the echo data
		IP_OPTION_INFORMATION Options;             // Reply options
	} IP_ECHO_REPLY, * PIP_ECHO_REPLY;
#pragma pack()

	struct tagThreadData
	{
		SOCKET	sock;
		INT		nSeqNum;
		char	szIP[40];
	};

	// function pointer for funtion in icmp.dll
	typedef HANDLE (WINAPI* pfnHV)(VOID);
	typedef BOOL (WINAPI* pfnBH)(HANDLE);
	typedef DWORD (WINAPI* pfnDHDPWPipPDD)(HANDLE, DWORD, LPVOID, WORD,
		PIP_OPTION_INFORMATION, LPVOID, DWORD, DWORD); 

	TSFPTrunk<Ping>		m_Trunk;

	BOOL					m_bUseRawSocket;
	volatile DWORD			m_dwTimeOut;
	volatile BOOL			m_bTerminateAllPingThread;
	std::vector<HANDLE>		m_vecThreadHandle;
	INT						m_nSeqNum;
	TSafeMap<INT, INT>		m_mapPingResult; // 与SeqNum对应的ping时间


	// ICMP.dll相关
	HINSTANCE				m_hICMP;
	pfnHV					m_pIcmpCreateFile;
	pfnBH					m_pIcmpCloseHandle;
	pfnDHDPWPipPDD			m_pIcmpSendEcho;
	HANDLE					m_hPingService;

	// ping线程
	UINT ThreadPing(LPVOID);

	INT SetupForPing(LPCSTR host, SOCKET& sd, sockaddr_in& dest);
	INT SendPing(SOCKET sd, const sockaddr_in& dest, 
        ICMPHeader* send_buf, INT packet_size);
	INT DecodeReply(IPHeader* reply, INT bytes, sockaddr_in* from);
	VOID InitPingPacket(ICMPHeader* icmp_hdr, INT packet_size, INT seq_no);
	USHORT IpChecksum(USHORT* buffer, INT size);
	INT AllocateBuffers(ICMPHeader*& send_buf, IPHeader*& recv_buf, INT packet_size);

};

} // namespace vEngine {




