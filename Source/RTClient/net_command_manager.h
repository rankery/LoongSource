//网络消息管理器

#ifndef NET_COMMAND_MANAGER_
#define NET_COMMAND_MANAGER_

struct tagNetCmd;

template<typename T>
class NetCommandManager
{
public:
	typedef DWORD (T::*NETMESSAGEHANDLE)(tagNetCmd *);

	NetCommandManager()
	{
		m_handles.clear();
	}
	~NetCommandManager() { }

	//注册网络消息对应的回调函数
	BOOL Register(LPCSTR command_name, NETMESSAGEHANDLE fp);
	VOID UnRegisterAll();

	BOOL HandleCommand(tagNetCmd *msg, DWORD msg_size, T *session);
private:
	struct NetCommand
	{
		std::string			command_name;
		NETMESSAGEHANDLE	message_handle;
		UINT32				recv_times;
	};
	typedef std::map<DWORD, NetCommand*> NetCommandMap;
	std::map<DWORD, NetCommand*> m_handles;
};


template<typename T>
BOOL NetCommandManager<T>::Register(LPCSTR command_name, NETMESSAGEHANDLE fp)
{
	DWORD command_crc = Crc32(command_name);
	NetCommand *command = NULL;
	NetCommandMap::iterator it = m_handles.find(command_crc);
	if (it != m_handles.end())
	{
		//已经有同样的消息注册过了
		return FALSE;
	}

	command = new NetCommand;
	command->recv_times = 0;
	command->command_name = command_name;
	command->message_handle = fp;
	m_handles.insert(std::make_pair(command_crc, command));

	return TRUE;
}

template<typename T>
VOID NetCommandManager<T>::UnRegisterAll()
{
	NetCommand *command = NULL;
	NetCommandMap::iterator it = m_handles.begin();
	for (; it != m_handles.end(); ++it)
	{
		command = it->second;
		if(command != NULL)
		{
			delete command;
			command = NULL;
		}
	}

	m_handles.clear();
}

template<typename T>
BOOL NetCommandManager<T>::HandleCommand(tagNetCmd *msg, DWORD msg_size, T *session)
{
	if (session == NULL || msg == NULL)
	{
		return FALSE;
	}
	
	NetCommand *command = NULL;
	NetCommandMap::iterator it = m_handles.find(msg->dwID);
	if (it == m_handles.end())
	{
		return FALSE;
	}
	if (msg->dwSize != msg_size)
	{
		return FALSE;
	}
	command = it->second;
	command->recv_times++;
	NETMESSAGEHANDLE handle = command->message_handle;
	if (handle == NULL)
	{
		return FALSE;
	}

	(session->*handle)(msg);

	return TRUE;
}
#endif /* NET_COMMAND_MANAGER_ */