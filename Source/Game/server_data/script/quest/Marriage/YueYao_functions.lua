--备用的月妖刷新的位置坐标
YueYao_Pos={}
YueYao_Pos[1]={x=388, y=820, z=1089}
YueYao_Pos[2]={x=438, y=820, z=1097}
YueYao_Pos[3]={x=450, y=820, z=1031}
YueYao_Pos[4]={x=582, y=820, z=1070}
YueYao_Pos[5]={x=625, y=820, z=1094}
YueYao_Pos[6]={x=671, y=820, z=1053}
YueYao_Pos[7]={x=711, y=820, z=1040}
YueYao_Pos[8]={x=640, y=820, z=1000}
YueYao_Pos[9]={x=648, y=820, z=967}
YueYao_Pos[10]={x=616, y=820, z=916}
YueYao_Pos[11]={x=549, y=820, z=872}
YueYao_Pos[12]={x=539, y=820, z=918}
YueYao_Pos[13]={x=462, y=820, z=899}
YueYao_Pos[14]={x=430, y=820, z=945}
YueYao_Pos[15]={x=393, y=820, z=971}
YueYao_Pos[16]={x=340, y=820, z=982}
YueYao_Pos[17]={x=344, y=820, z=932}
YueYao_Pos[18]={x=364, y=820, z=870}
YueYao_Pos[19]={x=356, y=820, z=829}
YueYao_Pos[20]={x=413, y=820, z=796}
YueYao_Pos[21]={x=448, y=820, z=772}
YueYao_Pos[22]={x=441, y=820, z=677}
YueYao_Pos[23]={x=411, y=820, z=626}
YueYao_Pos[24]={x=364, y=820, z=605}
YueYao_Pos[25]={x=335, y=820, z=677}
YueYao_Pos[26]={x=385, y=820, z=708}
YueYao_Pos[27]={x=335, y=820, z=749}
YueYao_Pos[28]={x=285, y=820, z=736}
YueYao_Pos[29]={x=281, y=820, z=780}
YueYao_Pos[30]={x=321, y=820, z=806}
YueYao_Pos[31]={x=269, y=820, z=816}
YueYao_Pos[32]={x=239, y=820, z=817}
YueYao_Pos[33]={x=217, y=820, z=815}
YueYao_Pos[34]={x=197, y=820, z=829}
YueYao_Pos[35]={x=218, y=820, z=755}
YueYao_Pos[36]={x=194, y=820, z=709}
YueYao_Pos[37]={x=191, y=820, z=681}
YueYao_Pos[38]={x=211, y=820, z=630}
YueYao_Pos[39]={x=247, y=820, z=610}
YueYao_Pos[40]={x=282, y=820, z=605}

--LUA函数，用来确定月妖刷新的地点，并且将改点序号记录在队伍中所有玩家的ScriptData上
function YueYao_GetPoint(MapID, InstanceID, RoleID)         --每次使用月妖之影队伍中的两个人都会获得一个新随机的坐标
	local Pos = math.random(1,40)
	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
	local Role = {}
	Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
	for i=1, 6 do
	if Role[i] and Role[i]~= 4294967295 then
		role.SetRoleScriptData(Role[i], 1, RoleDataType["YueYao_Pos"], Pos)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 13, 0)
		msg.AddMsgEvent(MsgID, 1, 516)       --月妖隐藏在婵娟池的 XXX,XXX。找到该坐标以后，使用月妖之镜可以发现月妖。
		msg.AddMsgEvent(MsgID, 9, YueYao_Pos[Pos].x)
		msg.AddMsgEvent(MsgID, 9, YueYao_Pos[Pos].z)
		msg.DispatchRoleMsgEvent(Role[i], MsgID)
	end
	end
end


--LUA函数，用来刷出月妖，每5级有两种花妖，随机取出一种在ScriptData上记录的位置刷出。
function YueYao_Appear(MapID, InstanceID, RoleID)
	local Pos = role.GetRoleScriptData(RoleID, 1, RoleDataType["YueYao_Pos"])
	local YueYao_Type = math.random(1,2)
	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
	local MaxLevel = 0
	local Level = 0
	local Role = {}
	Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
	for i=1, 6 do
	    if Role[i] and Role[i]~= 4294967295 then
			Level = role.GetRoleLevel(MapID, InstanceID, Role[i])
		    if MaxLevel < Level then          --获取队伍中玩家的最高等级
			MaxLevel = Level
			end
	    end
	end
	local YueYao_ID = math.floor(MaxLevel/5)*2 + 1532000   --通过玩家的最高等级来确定刷出什么等级的怪物
	if YueYao_ID >= 1532002 then
		if YueYao_Type == 1 then                  --每个等级有两个不同模型的怪物，他们的ID相邻
		YueYao_ID = YueYao_ID - 1
		end
	elseif YueYao_ID <= 1532000 then
		YueYao_ID = 1532001
	end
	map.MapCreateCreature(MapID, InstanceID, YueYao_ID, YueYao_Pos[Pos].x, YueYao_Pos[Pos].y, YueYao_Pos[Pos].z)
	local RoleSet = {}    --在刷出怪物以后，清空队伍中玩家的Pos记录
	RoleSet[1], RoleSet[2], RoleSet[3], RoleSet[4], RoleSet[5], RoleSet[6] = role.GetRoleTeamMemID(TeamID)
	for i=1, 6 do
	if RoleSet[i] and RoleSet[i]~= 4294967295 then
	    role.SetRoleScriptData(RoleSet[i], 1, RoleDataType["YueYao_Pos"], 0)
	end
	end
end

--LUA函数，用来判断男女是否二人共处一队，而且互为好友。是返回1，否返回0.
function ManAndWoman(MapID, InstanceID, RoleID)
	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
	local ManInTeam = 0
	local AddSex = 0    --如果值是10则满足一男一女
	local RoleID1 =0
	local RoleID2 =0
	local Role = {}
	Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
	for i=1, 6 do
	    if Role[i] and Role[i]~= 4294967295 then
		    local sex = role.GetRoleSex(Role[i])
		    if sex == 1 then
		        AddSex = AddSex + 3 --是男性的话就加3
			    RoleID1 = Role[i]
		    elseif sex == 0 then
		        AddSex = AddSex + 7 --是女性的话就加7
			    RoleID2 = Role[i]
		    end
				ManInTeam = ManInTeam + 1
	    end
	end
	local FriendShip1 = role.GetFriendShipValue(RoleID1, RoleID2)
	local FriendShip2 = role.GetFriendShipValue(RoleID2, RoleID1)
	if ManInTeam == 2 and AddSex == 10 and  FriendShip1 ~= -1 and FriendShip2 ~= -1 then
	    return 1
	else
		return 0
	end
end

--LUA函数，判断异性是否在自己的20格以内。是返回1，否返回0.
function NearIn20Lattice(MapID, InstanceID, RoleID)
	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
	local sex = role.GetRoleSex(RoleID)
	local a, b, c = unit.GetPosition(MapID, InstanceID, RoleID)  --获取自己的坐标
	local Role = {}
	Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
	for i=1, 6 do
	    if Role[i] and Role[i]~= 4294967295 then
		    local sex_Teammate = role.GetRoleSex(Role[i])
		    if sex_Teammate ~= sex then
		        local x, y ,z = unit.GetPosition(MapID, InstanceID, Role[i])    --由于队伍中有两个人，所以获取唯一个异性的坐标。
			    if x - a < -20 or x - a > 20 or z - c < -20 or z - c > 20 then
				    return 0
			    else
				    return 1
			    end
		    end
	    end
	end
end





