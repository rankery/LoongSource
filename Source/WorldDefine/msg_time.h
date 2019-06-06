//-----------------------------------------------------------------------------
//!\brief 客户端与服务器之间时间同步
//-----------------------------------------------------------------------------
#pragma once
#pragma pack(push,1)

CMD_START(NS_SynchronismTime)
	DWORD dwTime;
CMD_END

#pragma pack(pop)