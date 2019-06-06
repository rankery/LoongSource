--用于检测广播播放频率并在通过检测后播放广播的全局函数及其他内容定义



-----------DisRange（广播的投送范围）的可选值------------------
	  DPR_ALL		=	1   --全服广播
	  DPR_INMAP		=	2   --场景广播
	  DPR_ININSTANCE	=	3   --单一副本内部广播
	  DPR_INGUILD		=	4   --帮派广播
	  DPR_INROLE		=	5   --玩家角色广播
	  DPR_INTEAM		=	6   --小队广播
---------------------------------------------------------------
-----------ChenkWith（检测播放频率的参照物）的可选值-----------
	   CFT_BROAD	=	1	--以单条广播表中广播的播放频率为判断标准
	   CFT_IDMSG	=	2	--以单条id_Msg及string_table中的消息文本的播放频率为判断标准
	   CFT_ROLE	=	3	--以单个玩家角色触发广播事件的频率为判断标准
	   CFT_ITEM	=	4	--以某种物品触发广播事件的频率为判断标准
	   CFT_NPC	=	5	--以某个怪物或NPC触发广播事件的频率为判断标准
	   CFT_ACT	=	6	--以某个活动所引发的广播事件的频率为判断标准
	   CFT_QUEST	=	7	--以某个人物所引发的广播事件的频率为判断标准
---------------------------------------------------------------
ChenkTab = {};
ChenkTab[CFT_BROAD] = {LastTime = {}};
ChenkTab[CFT_IDMSG] = {LastTime = {}};
ChenkTab[CFT_ROLE] = {LastTime = {}};
ChenkTab[CFT_ITEM] = {LastTime = {}};
ChenkTab[CFT_NPC] = {LastTime = {}};
ChenkTab[CFT_ACT] = {LastTime = {}};
ChenkTab[CFT_QUEST] = {LastTime = {}};

--播放全服广播
function DispatchBroad_ALL (MsgID, MapID, InstanceID, GuildID, RoleID)	--全服播放
	local isSucc = 0	--函数的返回值，用于检测函数的执行状况
	if MsgID == nil then	--判断MsgID是否为空值
		isSucc = -1
	else
		msg.DispatchBroadcast(MsgID,-1,-1,-1)
		isSucc = 1	--播放成功
	end
	return isSucc
end
--播放场景广播
function DispatchBroad_INMAP(MsgID, MapID, InstanceID, GuildID, RoleID)	--场景播放
	local isSucc = 0	--函数的返回值，用于检测函数的执行状况
	if MsgID == nil then
		isSucc = -1
	else
		msg.DispatchBroadcast(MsgID,MapID,-1,-1)
		isSucc = 1		
		if MapID == 4294967295 then	--若因MapID为-1而导致播放了全服广播，则返回2
			isSucc = 2
		end
	end
	return isSucc
end
--播放单一副本内部广播
function DispatchBroad_ININSTANCE(MsgID, MapID, InstanceID, GuildID, RoleID)	--单一副本内部播放
	local isSucc = 0	--函数的返回值，用于检测函数的执行状况
	if MsgID == nil then
		isSucc = -1
	else
		msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1)
		isSucc = 1
		if MapID == 4294967295 or InstanceID == 4294967295 then	--若因MapID或InstanceID为-1而导致播放了全服广播，则返回2
			isSucc = 2
		end
	end
	return isSucc
end
--播放帮派内部广播
function DispatchBroad_INGUILD(MsgID, MapID, InstanceID, GuildID, RoleID)	--单一帮派内部播放
	local isSucc = 0	--函数的返回值，用于检测函数的执行状况
	if MsgID == nil then
		isSucc = -1
	elseif guild.GetGuildFund(GuildID) == nil then	--检测GuildID是否可用，若不可用，则通过RoleID重新获取一个GuildID
		if guild.GetRoleGuildID(RoleID) == nil then
			isSucc = -2
		else
			local guild_id = guild.GetRoleGuildID(RoleID)
			if guild_id == 4294967295 then	--检测获取到的GuildID是否为-1，若为-1则只给该玩家播放广播
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				isSucc = 2
			else
				msg.DispatchBroadcast(MsgID,-1,-1,guild_id)
				isSucc = 1
			end
		end
	else
		msg.DispatchBroadcast(MsgID,-1,-1,GuildID)
		isSucc = 1
	end
	return isSucc
end
--播放玩家广播
function DispatchBroad_INROLE(MsgID, MapID, InstanceID, GuildID, RoleID)	--向某一玩家角色播放
	local isSucc = 0	--函数的返回值，用于检测函数的执行状况
	if MsgID == nil then
		isSucc = -1
	elseif role.GetRoleName(RoleID) == nil then
		isSucc = -2
	else
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		isSucc = 1
	end
	return isSucc
end
--播放小队广播
function DispatchBroad_INTEAM(MsgID, MapID, InstanceID, GuildID, RoleID)	--向某一玩家小队播放
	local isSucc = 0	--函数的返回值，用于检测函数的执行状况
	if MsgID == nil then
		isSucc = -1
	elseif role.IsRoleHaveTeam(MapID, InstanceID, RoleID) == nil then	--检测是否可获得玩家小队ID，若不可则返回-2
		isSucc = -2
	else 
		local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)	--获取玩家小队ID
		if TeamID == 4294967295 then					--若玩家没有小队，则播放单人广播，且返回2
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
			isSucc = 2
		else
			local Role = {}
			Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
			for i=1,6 do
				if Role[i] and Role[i] ~= 4294967295 then
					msg.DispatchRoleMsgEvent(Role[i], MsgID)
				end
			end
			isSucc = 1
		end
	end
	return isSucc
end

--广播播放函数表（对应不同播放区域）
RunBroadFuncTab = {};
RunBroadFuncTab[DPR_ALL] = DispatchBroad_ALL
RunBroadFuncTab[DPR_INMAP] = DispatchBroad_INMAP
RunBroadFuncTab[DPR_ININSTANCE] = DispatchBroad_ININSTANCE
RunBroadFuncTab[DPR_INGUILD] = DispatchBroad_INGUILD
RunBroadFuncTab[DPR_INROLE] = DispatchBroad_INROLE
RunBroadFuncTab[DPR_INTEAM] = DispatchBroad_INTEAM

function RunBroad(MsgID, MapID, InstanceID, GuildID, RoleID, DisRange, ChenkWith, IndexID, TimerMin)
	if MsgID == nil then return -1 end

	local CanDispath = true
	local dtime = 0
	
	if TimerMin > 0 then	
		if ChenkTab[ChenkWith].LastTime[IndexID] == nil then
			CanDispath = true
		else
			dtime = os.difftime(os.time(),ChenkTab[ChenkWith].LastTime[IndexID])
			if dtime>TimerMin*60 then
				CanDispath = true
			else
				CanDispath = false
			end
		end
	end

	if CanDispath then
		if RunBroadFuncTab[DisRange](MsgID, MapID, InstanceID, GuildID, RoleID)>0 then
			ChenkTab[ChenkWith].LastTime[IndexID] = os.time()
		else
			return -1
		end
	end
end

--[[
	RunBroad函数对DispatchBroadcast和DispatchRoleMsgEvent函数进行了封装，并在播放广播前加入了检测广播播放频率的功能，若播放成功则返回值为实际播放范围的对应值，否则为-1

	RunBroad参数表的具体含义：

	MsgID：需要播放的广播句柄，此句柄应是已注册并完成内容添加的句柄，否则函数将返回-1。

	MapID：广播需要投送的场景CRC值，若要播放全服广播或仅对玩家角色播放，则此处可为-1。

	InstanceID：副本对象ID，默认值为-1

	GuildID：广播需要投送的帮派ID，若不想播放帮派范围广播，则此处可为-1

	RoleID：玩家角色ID，若广播的投送范围为仅向玩家角色播放时，则需要用该值来控制具体收取广播的玩家角色，默认值为-1

	DisRange：广播的投送范围，可选值
		  DPR_ALL		=	0   --全服广播
		  DPR_INMAP		=	1   --场景广播
		  DPR_ININSTANCE	=	2   --单一副本内部广播
		  DPR_INGUILD		=	3   --帮派广播
		  DPR_INROLE		=	4   --玩家角色广播
		  DPR_INTEAM		=	5   --小队广播

	ChenkWith：用于检测广播播放频率的参照物，可选值为：
		   CFT_BROAD	=	0	--以单条广播表中广播的播放频率为判断标准
		   CFT_IDMSG	=	1	--以单条id_Msg及string_table中的消息文本的播放频率为判断标准
		   CFT_ROLE	=	2	--以单个玩家角色触发广播事件的频率为判断标准
		   CFT_ITEM	=	3	--以某种物品触发广播事件的频率为判断标准
		   CFT_NPC	=	4	--以某个怪物或NPC触发广播事件的频率为判断标准
		   CFT_ACT	=	5	--以某个活动所引发的广播事件的频率为判断标准
		   CFT_QUEST	=	6	--以某个人物所引发的广播事件的频率为判断标准

	IndexID：与ChenkWith值一一对应的参照物索引标记值，具体对应关系如下：
		若ChenkWith的值为0（CFT_BROAD），IndexID可传入该广播在broadcast_list.xml中的ID
		若ChenkWith的值为1（CFT_IDMSG），IndexID可传入该广播在id_msg.xml中的ID
		若ChenkWith的值为2（CFT_ROLE），IndexID可传入对应玩家角色的RoleID
		若ChenkWith的值为3（CFT_ITEM），IndexID可传入对应物品的TypeID
		若ChenkWith的值为4（CFT_NPC），IndexID可传入对应怪物或NPC的TypeID
		若ChenkWith的值为5（CFT_ACT），IndexID可传入对应活动的活动ID
		若ChenkWith的值为6（CFT_QUEST），IndexID可传入对应任务的任务ID

	TimerMin：间隔时间（分），如为0则无间隔限制
]]