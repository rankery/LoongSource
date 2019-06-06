--[[固定活动群雄逐鹿侠客组

--ScriptData中各个字段意义说明
--0:活动的届数。1- 20为前10名小队编号，顺序按照名次进行存储。

--活动中玩家数据(主键RoleID, KillPlayer, IsDeadLeave, IsInMap)
Act18_RoleData = {} --侠客组

--活动中玩家数据(主键主键小队编号, Act18_RoleData1, Act18_RoleData2, Act18_RoleData3，Act18_RoleData4，Act18_RoleData5，Act18_RoleData6)
Act18_TeamRoleData = {}

--活动中的玩家小队数据(主键小队编号, Score, JoinTime, LeaderID)
Act18_TeamData = {}

--固定活动已开始的时间（单位是分钟）
Act18_LastTime = 0

--活动中小队编号最大值
Act18_TeamIndex = 1

--活动当前届数
Act18_Expire = 0

--是否已经创建了副本
IsCreateAct18Instance = 0

--活动奖励
Act18_RewardItem = {1220005, 1230005, 1210003, 1420007, 1420011, 1420015, 1420019, 1420031, 1420035}

--活动中刷出的晶球数量
Act18_CrystalNum = 0

--记录每个玩家杀掉的敌人的次数
PKAct18_killrole = {}

--------------------------------------------------------------------------------------------------------
local function RoleRevive(MapID, InstanceID, RoleID, ReviveHP, ReviveMP, fx, fy, fz, RebornMapID, Act_TeamRoleData)
	--获得玩家的小队编号
	local TeamKey = role.GetRoleScriptData(RoleID, 1, RoleDataType["Act18_TeamKey"])

	if Act_TeamRoleData[TeamKey][RoleID].KillPlayer == 0 then
		fx = 2156
		fy = 18363
		fz = 2420
		RebornMapID = 3017299663

		--设置玩家为死亡后离开
		Act_TeamRoleData[TeamKey][RoleID].IsDeadLeave = true
	else
		--随机得到一个复活点坐标
		local Index = math.random(1, 8)
		fx = m22_RebornPos[Index].x
		fy = m22_RebornPos[Index].y
		fz = m22_RebornPos[Index].z
		RebornMapID = 2983743695

		复活次数减1
		Act_TeamRoleData[TeamKey][RoleID].KillPlayer = Act_TeamRoleData[TeamKey][RoleID].KillPlayer - 1

		--同步客户端复活次数
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 60,	152+InstanceID)		                --组别
		msg.AddMsgEvent(MsgID, 17,	Act_TeamRoleData[TeamKey][RoleID].KillPlayer)	--复活次数
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	end
	unit.AddBuff(MapID, InstanceID, RoleID, 9999001, RoleID)
	return ReviveHP, ReviveMP, fx, fy, fz, RebornMapID
end

--------------------------------------------------------------------------------------------------------
local function PlayerEnter(MapID, InstanceID, RoleID, Act_Expire, Act_TeamRoleData, Act_TeamData, TeamKey)
    --获得玩家上次参加活动的届数
	local RoleExpire = role.GetRoleScriptData(RoleID, 1, RoleDataType["Act18_Expire"])

	if RoleExpire == Act_Expire then
	    --如果玩家是死亡后离开活动
		--if Act_TeamRoleData[TeamKey][RoleID] ~= nil then
		    if InstanceID == 1 then
		        --扣除玩家2000银
			    role.DecRoleSilver(MapID, InstanceID, RoleID, 2000, 1001)
			end

			if InstanceID == 2 then
			    --扣除玩家5000银
			    role.DecRoleSilver(MapID, InstanceID, RoleID, 5000, 1001)
			end

			if InstanceID == 3 then
			    --扣除玩家10000银
			    role.DecRoleSilver(MapID, InstanceID, RoleID, 10000, 1001)
			end

			if InstanceID == 4 then
			    --扣除玩家20000银
			    role.DecRoleSilver(MapID, InstanceID, RoleID, 20000, 1001)
			end

			Act_TeamRoleData[TeamKey][RoleID].IsDeadLeave = false
		--end

		--如果玩家没有队伍, 统计活动中小队成员的数量
		local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
		local TeamKey = role.GetRoleScriptData(RoleID, 1, RoleDataType["Act18_TeamKey"])
		local TeamNum = 0
		local TeamateInMap = 4294967295
		if TeamID == 4294967295 then
		    for k in pairs(Act_TeamRoleData[TeamKey]) do
		        if Act_TeamRoleData[TeamKey][k].IsInMap then
		            TeamNum = TeamNum + 1
		            TeamateInMap = k
		        end
		    end
		end

		--活动中只有一个小队队友
		if TeamNum == 1 then
		    local TeamateTeamID = role.IsRoleHaveTeam(MapID, InstanceID, TeamateInMap)
		    --队友没有小队, 则创建一个小队
		    if TeamateTeamID == 4294967295 then
		        team.CreateTeam(TeamateInMap, RoleID)
		    else   --有小队，则加入该小队
		        team.AddTeamMember(TeamateTeamID, RoleID)
		    end
		end

		--玩家加入原来的小队
		if TeamNum > 1 then
		   TeamID = role.IsRoleHaveTeam(MapID, InstanceID, TeamateInMap)
		   team.AddTeamMember(TeamID, RoleID)
		end

		Act_TeamRoleData[TeamKey][RoleID].IsInMap = true
	else
	    --玩家第一次进入活动
--		table.insert(Act_TeamRoleData[TeamKey], RoleID, {KillPlayer=3, IsDeadLeave=false, IsInMap=true})

	    --设置玩家ScriptData为当前活动的届数
	    role.SetRoleScriptData(RoleID, 1, RoleDataType["Act18_Expire"], Act_Expire)

	    if not Act_TeamData[TeamKey] then
--	        table.insert(Act_TeamData, TeamKey, {Score = 0, JoinTime = 0, LeaderID = RoleID})
	        Act_TeamData[TeamKey] = {Score = 0, JoinTime = 0, LeaderID = RoleID}
	        Act_TeamRoleData[TeamKey] = {}
	    end

	    Act_TeamRoleData[TeamKey][RoleID] = {KillPlayer=0, IsDeadLeave=false, IsInMap=true}
		if InstanceID == 1 then
		    if Act18_RoleData[RoleID] == nil then
			    Act18_RoleData[RoleID] = {KillPlayer=0}
			end
		elseif InstanceID == 2 then
			if Act19_RoleData[RoleID] == nil then
			    Act19_RoleData[RoleID] = {KillPlayer=0}
			end
		elseif InstanceID == 3 then
		    if Act20_RoleData[RoleID] == nil then
			    Act20_RoleData[RoleID] = {KillPlayer=0}
			end
		elseif InstanceID == 4 then
			if Act21_RoleData[RoleID] == nil then
			    Act21_RoleData[RoleID] = {KillPlayer=0}
			end
		end
	end
end

--------------------------------------------------------------------------------------------------------
--同步客户端小队积分
function SendScore2Teamate(ActGroup, Score, TeamRoleData)
    for k in pairs(TeamRoleData) do
        if TeamRoleData[k].IsInMap then
	        --同步客户端积分
	        local MsgID = msg.BeginMsgEvent()
	        msg.AddMsgEvent(MsgID, 60,	ActGroup)							--组别
	        msg.AddMsgEvent(MsgID, 14,	Score)                      		--积分
	        msg.DispatchRoleMsgEvent(k, MsgID)
        end
	end
end

--------------------------------------------------------------------------------------------------------
--存储当前排名
function SaveAct18TeamData()
    act.SetActScriptData(18, 1, 0, Act18_Expire)

    local Index = 1
    for k in pairsByKeys(Act18_TeamData, Act18_Cmp) do
        act.SetActScriptData(18, 1, Index, k)
        Index = Index + 1
        if Index > 10 then
            break
        end
	end
end
--------------------------------------------------------------------------------------------------------
--固定活动开始
function act18_OnStart(ActID)
	--初始化活动已进行时间
	Act18_LastTime = 0

	--初始化水晶球数量
	Act18_CrystalNum = 0

	Act18_Expire = act.GetActScriptData(18, 1, 0)

	--同步所有场中中玩家在线时间
	for n in pairs(Act18_TeamRoleData) do
		for k in pairs(Act18_TeamRoleData[n]) do
		    if Act18_TeamRoleData[n][k].IsInMap then
		        --发送活动剩余时间
	            local MsgID = msg.BeginMsgEvent()
	            msg.AddMsgEvent(MsgID, 60,	153)
	            msg.AddMsgEvent(MsgID, 16,	1800)							                 --剩余时间
	            msg.DispatchRoleMsgEvent(k, MsgID)


                --设置玩家为入擂台状态(用于客户端敌我判断）
	            role.SetRoleState(RoleID, 4, false)
	        end
	    end
	end

	for k in pairs(m22_SpawnPos) do
		m22_SpawnPos[k].Crystal40 = false
		m22_SpawnPos[k].Crystal60 = false
		m22_SpawnPos[k].Crystal80 = false
		m22_SpawnPos[k].Crystal100 = false
	end
end

function act18_OnTimerMin(ActID)
	--累加活动进行时间
    Act18_LastTime = Act18_LastTime + 1

		for k in pairs(Act18_TeamRoleData) do
			for n in pairs(Act18_TeamRoleData[k]) do
				--玩家在活动地图中
				if Act18_TeamRoleData[k][n].IsInMap then
					--玩家没有死亡
					Act18_TeamData[k].Score = Act18_TeamData[k].Score + 6
					--玩家是否在活跃区域
					if map.IsInArea(2983743695, 1, n, 636)	then
						if unit.IsDead(2983743695, 1, n) then
							Act18_TeamData[k].Score = Act18_TeamData[k].Score + 6
						end
					end
				end
			end
			--同步客户端积分
			SendScore2Teamate(153, Act18_TeamData[k].Score, Act18_TeamRoleData[k])
		end
    if Act18_LastTime % 5 == 0 then
	    local total = 0
		local avaliable = {}
		for ii=1,20 do
			if m22_SpawnPos[ii].Crystal40 == false then
				total = total+1
				avaliable[total] = ii
			end
		end
		while Act18_CrystalNum < 5 and total > 0 do
		    --随机一个坐标
			local Index = math.random(1,total)
			Index = avaliable[Index]
		    avaliable[Index] = avaliable[total]
			Act18_CrystalNum = Act18_CrystalNum + 1
			m22_SpawnPos[Index].Crystal40 = true
		    total = total -1
			--随机得到一个水晶球ID
			local CrystalBallID = math.random(1520001, 1520007)
			--刷出水晶球
			local CreatureID = map.MapCreateCreature(2983743695, 1, CrystalBallID, m22_SpawnPos[Index].x, m22_SpawnPos[Index].y, m22_SpawnPos[Index].z)
			cre.SetCreatureScriptData(2983743695, 1, CreatureID, 1, 0, Index)
		end
	end

	--累加玩家参加活动的时间
	for k in pairs(Act18_TeamData) do
	    Act18_TeamData[k].JoinTime = Act18_TeamData[k].JoinTime + 1
	    if (Act18_TeamData[k].JoinTime % 10) == 0 then
		    for n in pairs(Act18_TeamRoleData[k]) do
		        if Act18_TeamRoleData[k][n].IsInMap and false == unit.IsDead(2983743695, 1, n)then
			        --奖励2000银
			        role.AddRoleSilver(2983743695, 1, n, 2000, 1001)
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 101, 2853)   --您参与活动的累计时间达到10的倍数，获得额外的金钱奖励
					msg.DispatchRoleMsgEvent(n, MsgID)
			    end
			end
		end
	end
end

--活动结束
function act18_OnEnd(actID)
	--删除活动副本
	IsCreateAct18Instance = 0
	SaveAct18TeamData()
    PKAct18_killrole = {}
    Act18_RoleData = {}
	local rank ={}
	for i=1,10 do
	    rank[act.GetActScriptData(18, 1, i)] = i
	end

	--随机抽取一件奖励给活动中存活的玩家
	for k in pairs(Act18_TeamData) do
	    --随机抽取一件奖励给活动中存活的玩家
		for n in pairs(Act18_TeamRoleData[k]) do
			if Act18_TeamRoleData[k][n].IsInMap then
				local AddExp = role.GetRoleLevelUpExp(2983743695, 1, n)
				local rolelevel = role.GetRoleLevel(2983743695, 1, n)
				local Scoreact = Act18_TeamData[k].Score
				if Scoreact >3000 then
					Scoreact = 3000
				end
				local exp = AddExp * Scoreact / 3000 * 130 / (rolelevel - 25) / (rolelevel - 25)
				role.AddRoleExp(2983743695, 1, n, exp)
				local wushiyin = 0
				if Act18_TeamData[k].Score >= 2000 then
					wushiyin = 30
				elseif Act18_TeamData[k].Score >= 1000 then
					wushiyin = 20
				elseif Act18_TeamData[k].Score >= 500 then
					wushiyin = 15
				elseif Act18_TeamData[k].Score >= 250 then
					wushiyin = 10
				elseif Act18_TeamData[k].Score >= 150 then
					wushiyin = 6
				elseif Act18_TeamData[k].Score >= 100 then
					wushiyin = 4
				elseif Act18_TeamData[k].Score >= 50 then
					wushiyin = 2
				else
					wushiyin = 1
				end
				if rank[k] ~= nil then
					if rank[k] == 1 then
						--role.AddRoleItem(2983743695, 1, n, 2612621, 1, -1, 13, 1001)
						--role.AddRoleItem(2983743695, 1, n, 2612629, 10, -1, 13, 1001)
						role.AddRoleItem(2983743695, 1, n, 2616345, wushiyin, -1, 13, 1001)
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 71, 2857)  --您在群雄逐鹿活动中获得第一名，得到经验<color=0xffff0000>XX<color=0xfffff7e0>点和<color=0xffff0000>X<color=0xfffff7e0>个<color=0xff00a2ff>武士印<color=0xfffff7e0>。您可以在涿鹿英雄榜处领取专属称号和<color=0xffff0000>10<color=0xfffff7e0>个<color=0xff00a2ff>英雄帖<color=0xfffff7e0>经验<color=0xffff0000>XX<color=0xfffff7e0>点！
						msg.AddMsgEvent(MsgID, 9, exp)
						msg.AddMsgEvent(MsgID, 9, wushiyin)
						msg.DispatchRoleMsgEvent(n, MsgID)
					elseif rank[k] <= 10 then
						local itemnum = 1
						if rank[k] == 2 or rank[k] == 3 then
							itemnum = 8
						elseif rank[k] == 4 or rank[k] == 5 or rank[k] == 6 then
							itemnum = 4
						elseif rank[k] == 7 or rank[k] == 8 or rank[k] == 9 or rank[k] == 10 then
							itemnum = 2
						end
					   --role.AddRoleItem(2983743695, 1, n, 2612629, itemnum, -1, 13, 1001)
						role.AddRoleItem(2983743695, 1, n, 2616345, wushiyin, -1, 13, 1001)
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 71, 2858)  --恭喜您在群雄逐鹿活动中获得第X名，得到经验<color=0xffff0000>XX<color=0xfffff7e0>点和<color=0xffff0000>X<color=0xfffff7e0>个<color=0xff00a2ff>武士印<color=0xfffff7e0>。您可以在涿鹿英雄榜处领取<color=0xffff0000>X<color=0xfffff7e0>个<color=0xff00a2ff>英雄帖<color=0xfffff7e0>！
						msg.AddMsgEvent(MsgID, 9, rank[k])
						msg.AddMsgEvent(MsgID, 9, exp)
						msg.AddMsgEvent(MsgID, 9, wushiyin)
						msg.AddMsgEvent(MsgID, 9, itemnum)
						msg.DispatchRoleMsgEvent(n, MsgID)
					end
				else
					role.AddRoleItem(2983743695, 1, n, 2616345, wushiyin, -1, 13, 1001)
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 71, 2859)  --您在群雄逐鹿活动中获得经验<color=0xffff0000>XX<color=0xfffff7e0>点和<color=0xffff0000>X<color=0xfffff7e0>个<color=0xff00a2ff>武士印<color=0xfffff7e0>希望您再接再励。
					msg.AddMsgEvent(MsgID, 9, exp)
					msg.AddMsgEvent(MsgID, 9, wushiyin)
					msg.DispatchRoleMsgEvent(n, MsgID)
				end
				随机抽取一件物品作为奖励
				if 1 <= role.GetBagFreeSize(n) then
					role.AddRoleItem(2983743695, 1, n, 2612621, 1, -1, 13, 1001)

					--提示玩家"您参与英雄乱武活动，获得###奖励，希望下次再接再厉。"
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 71, 164)
					msg.AddMsgEvent(MsgID, 4, Act18_RewardItem[Index])
					msg.DispatchRoleMsgEvent(n, MsgID)
				end
			end
		end
	end
	map.DeleteInstance(2983743695, 1)
end

--活动中广播(State = 0-即将开始 1-开始 2-即将结束 3-结束 )
function act18_ActivityBroad(State)
    if PVPactivity_trigger ~= 1 then
	    return
	end
    --活动即将开始
    if State == 0 then
        --通知全服活动即将开始
	    local MsgID = msg.BeginMsgEvent()
	    msg.AddMsgEvent(MsgID, 100, 165)
	    msg.DispatchWorldMsgEvent(MsgID)
    end
end

--注册该活动的相关事件
aux.RegisterActEvent(18, 2, "act18_OnStart") 	--侠客组
aux.RegisterActEvent(18, 4, "act18_OnTimerMin") --侠客组
aux.RegisterActEvent(18, 3, "act18_OnEnd") 		--侠客组
aux.RegisterActEvent(18, 6, "act18_ActivityBroad")
aux.RegisterActEvent(103, 2, "act18_OnStart") 	--侠客组
aux.RegisterActEvent(103, 4, "act18_OnTimerMin") --侠客组
aux.RegisterActEvent(103, 3, "act18_OnEnd") 		--侠客组
aux.RegisterActEvent(103, 6, "act18_ActivityBroad")
--------------------------------------------------------------------------------------------------------
--todo玩家进入活动场景
function m22_PlayerEnter(MapID, InstanceID, RoleID)
    local Min  		= tonumber(os.date("%M"))
	local Second	= tonumber(os.date("%S"))
	local leftSec 	= (29 - Min) * 60 + 59 - Second		--活动剩余时间
	--设置玩家为PK状态(用于客户端敌我判断）
    role.SetRoleState(RoleID, 8, false)
    --得到玩家小队编号
	local TeamKey = role.GetRoleScriptData(RoleID, 1, RoleDataType["Act18_TeamKey"])

	--侠客组
	if InstanceID == 1 then
        PlayerEnter(MapID, InstanceID, RoleID, Act18_Expire, Act18_TeamRoleData, Act18_TeamData, TeamKey)
	    --发送活动界面给客户端
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 60,	153)								                --侠客组
		msg.AddMsgEvent(MsgID, 25,	18)									                --活动ID
		msg.AddMsgEvent(MsgID, 14,	Act18_TeamData[TeamKey].Score)		                --积分
		msg.AddMsgEvent(MsgID, 17,	Act18_TeamRoleData[TeamKey][RoleID].KillPlayer)     --复活次数
		msg.AddMsgEvent(MsgID, 18,	TeamKey)                                            --队伍编号
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	end

    --豪杰组
    if InstanceID == 2 then
        PlayerEnter(MapID, InstanceID, RoleID, Act19_Expire, Act19_TeamRoleData, Act19_TeamData, TeamKey)
	    --发送活动界面给客户端
        local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 60,	154)								                --豪杰组
		msg.AddMsgEvent(MsgID, 25,	19)									                --活动ID
		msg.AddMsgEvent(MsgID, 14,	Act19_TeamData[TeamKey].Score)		                --积分
		msg.AddMsgEvent(MsgID, 17,	Act19_TeamRoleData[TeamKey][RoleID].KillPlayer)     --复活次数
		msg.AddMsgEvent(MsgID, 18,	TeamKey)                                            --队伍编号
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
    end

    --英雄组
    if InstanceID == 3 then
        PlayerEnter(MapID, InstanceID, RoleID, Act20_Expire, Act20_TeamRoleData, Act20_TeamData, TeamKey)
	    --发送活动界面给客户端
        local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 60,	155)								                --英雄组
		msg.AddMsgEvent(MsgID, 25,	20)									                --活动ID
		msg.AddMsgEvent(MsgID, 14,	Act20_TeamData[TeamKey].Score)		                --积分
		msg.AddMsgEvent(MsgID, 17,	Act20_TeamRoleData[TeamKey][RoleID].KillPlayer)     --复活次数
		msg.AddMsgEvent(MsgID, 18,	TeamKey)                                            --队伍编号
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
    end

    --天下组
    if InstanceID == 4 then
        PlayerEnter(MapID, InstanceID, RoleID, Act21_Expire, Act21_TeamRoleData, Act21_TeamData, TeamKey)
	    --发送活动界面给客户端
	    local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 60,	156)								                --天下组
		msg.AddMsgEvent(MsgID, 25,	21)									                --活动ID
		msg.AddMsgEvent(MsgID, 14,	Act21_TeamData[TeamKey].Score)		                --积分
		msg.AddMsgEvent(MsgID, 17,	Act21_TeamRoleData[TeamKey][RoleID].KillPlayer)     --复活次数
		msg.AddMsgEvent(MsgID, 18,	TeamKey)                                            --队伍编号
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
    end

    --判断活动是否已经开始
	local bStart = act.GetActIsStart(18)
	if bStart then
        --发送活动剩余时间
	    local MsgID = msg.BeginMsgEvent()
	    msg.AddMsgEvent(MsgID, 60,	153)								                 --侠客组
	    msg.AddMsgEvent(MsgID, 16,	leftSec)							                 --剩余时间
	    msg.DispatchRoleMsgEvent(RoleID, MsgID)


        --设置玩家为入擂台状态(用于客户端敌我判断）
	    role.SetRoleState(RoleID, 8, true)
	end
end

--注册进入活动地图事件
aux.RegisterMapEvent("m22", 2, "m22_PlayerEnter")

-------------------------------------------------------------------------------------------------------
--玩家离开活动场景
function m22_PlayerLeave(MapID, InstanceID, RoleID)
	--获得玩家的小队编号
	local TeamKey = role.GetRoleScriptData(RoleID, 1, RoleDataType["Act18_TeamKey"])

	--设置玩家离开入擂台状态(用于客户端敌我判断）
	role.UnSetRoleState(RoleID, 8, true)
	--同步客户端关闭活动界面
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 60, -1)								--关闭活动界面
	msg.DispatchRoleMsgEvent(RoleID, MsgID)

	--侠客组
	if InstanceID == 1 then
		Act18_TeamRoleData[TeamKey][RoleID].IsInMap = false
	end

	--豪杰组
	if InstanceID == 2 then
		Act19_TeamRoleData[TeamKey][RoleID].IsInMap = false
	end

	--英雄组
	if InstanceID == 3 then
		Act20_TeamRoleData[TeamKey][RoleID].IsInMap = false
	end

	--天下组
	if InstanceID == 4 then
		Act21_TeamRoleData[TeamKey][RoleID].IsInMap = false
	end
end

--注册离开活动地图事件
aux.RegisterMapEvent("m22", 3, "m22_PlayerLeave")

-------------------------------------------------------------------------------------------------------
--玩家在活动中复活
function m22_RoleRevive(MapID, InstanceID, RoleID, ReviveType, ReviveHP, ReviveMP, fx, fy, fz, RebornMapID)
	--玩家选择回城复活
	if ReviveType == 0 then
		ReviveHP = role.GetRoleAttValue(MapID, InstanceID, RoleID, 8)
		ReviveMP = role.GetRoleAttValue(MapID, InstanceID, RoleID, 10)

		--玩家默认有3次活动复活机会。玩家选择回城复活，消耗1次活动复活机会，随机抽区场景内的1个活动复活点进行复活。
		if InstanceID == 1 then
            return RoleRevive(MapID, InstanceID, RoleID, ReviveHP, ReviveMP, fx, fy, fz, RebornMapID, Act18_TeamRoleData)
		end

		--豪杰组
		if InstanceID == 2 then
		    return RoleRevive(MapID, InstanceID, RoleID, ReviveHP, ReviveMP, fx, fy, fz, RebornMapID, Act19_TeamRoleData)
		end

		--英雄组
		if InstanceID == 3 then
			return RoleRevive(MapID, InstanceID, RoleID, ReviveHP, ReviveMP, fx, fy, fz, RebornMapID, Act20_TeamRoleData)
		end

		--天下组
		if InstanceID == 4 then
			return RoleRevive(MapID, InstanceID, RoleID, ReviveHP, ReviveMP, fx, fy, fz, RebornMapID, Act21_TeamRoleData)
		end
	end

	return ReviveHP, ReviveMP, fx, fy, fz, RebornMapID
end

aux.RegisterMapEvent("m22",  11, "m22_RoleRevive")

-------------------------------------------------------------------------------------------------------
--活动中玩家死亡todo
function m22_RoleDie(MapID, InstanceID, RoleID, KillerID)
	if unit.IsPlayer(KillerID) then
	    --获得玩家的小队编号
		local KillerTeamKey = role.GetRoleScriptData(KillerID, 1, RoleDataType["Act18_TeamKey"])
		local RoleTeamKey = role.GetRoleScriptData(RoleID, 1, RoleDataType["Act18_TeamKey"])

		--击杀一个玩家，获得积分增加，增加量=10×[1-（自身等级-目标等级）/10]，最小取1
		local RoleLevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
		if RoleLevel < 40 then             --防止等级不合适时出错
		    RoleLevel = 40
		elseif RoleLevel > 100 then
		    RoleLevel = 100
		end
		local KillerLevel = role.GetRoleLevel(MapID, InstanceID, KillerID)
		local Score = 30*(1-(KillerLevel - RoleLevel)/10)
		--local PunishScore = 1
		if Score < 1 then
			Score = 1
		end

		--侠客组
		if InstanceID == 1 then
			活动后15分钟
		    if Act18_LastTime > 15 then
		    	--击杀积分排名前10名的玩家，获得积分量等于普通状态的2倍
		    	local Index = 0
		    	for k in pairsByKeys(Act18_TeamData, Act18_Cmp) do
		    		if k == RoleTeamKey then
		    			Score = Score*2
		    			--PunishScore = 5
		    			break
		    		end

		    		if Index > 10 then
		    			break
		    		end

	    			Index = Index + 1
	    		end
	    	end
			--增加击杀数
			Act18_RoleData[KillerID].KillPlayer = Act18_RoleData[KillerID].KillPlayer + 1
			if Act18_RoleData[KillerID].KillPlayer % 10 == 0 and Act18_RoleData[KillerID].KillPlayer ~= 0 then
				--通知全服活动即将开始
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 102, 100108)    --【战报】XXX勇不可挡，完成X人斩！
				msg.AddMsgEvent(MsgID, 2, KillerID)
				msg.AddMsgEvent(MsgID, 9, Act18_RoleData[KillerID].KillPlayer)
				msg.DispatchBroadcast(MsgID, MapID, InstanceID,-1)
			end
			Act18_TeamRoleData[KillerTeamKey][KillerID].KillPlayer = Act18_TeamRoleData[KillerTeamKey][KillerID].KillPlayer + 1
			--计算击杀经验奖励
			local killexp = PKAct_killexp[RoleLevel]
            if PKAct18_killrole[KillerID] == nil then
			    PKAct18_killrole[KillerID] = {}
				if PKAct18_killrole[KillerID][RoleID] == nil then
				    PKAct18_killrole[KillerID][RoleID] = 0
				else
				    PKAct18_killrole[KillerID][RoleID] = PKAct18_killrole[KillerID][RoleID] + 1
				end
			else
			    if PKAct18_killrole[KillerID][RoleID] == nil then
				    PKAct18_killrole[KillerID][RoleID] = 0
				else
				    PKAct18_killrole[KillerID][RoleID] = PKAct18_killrole[KillerID][RoleID] + 1
				end
			end
			if PKAct18_killrole[KillerID][RoleID] < 10 then
			    role.AddRoleExp(2983743695, 1, KillerID, math.floor(killexp*(10-PKAct18_killrole[KillerID][RoleID])/10))
			    Score = math.floor(Score*(10-PKAct18_killrole[KillerID][RoleID])/10)
			else
			    Score = 0
			end
			--增加击杀者积分todo
			Act18_TeamData[KillerTeamKey].Score = Act18_TeamData[KillerTeamKey].Score + Score
			SendScore2Teamate(153, Act18_TeamData[KillerTeamKey].Score, Act18_TeamRoleData[KillerTeamKey])

			if PKAct18_killrole[KillerID][RoleID] < 10 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 101, 2860)           --击杀XXX，获得经验XXX点，积分XXX点
				msg.AddMsgEvent(MsgID, 2, RoleID)
				msg.AddMsgEvent(MsgID, 9, math.floor(killexp*(10-PKAct18_killrole[KillerID][RoleID])/10))
				msg.AddMsgEvent(MsgID, 9, Score)
				msg.DispatchRoleMsgEvent(KillerID, MsgID)
			end

			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 60,	153)								                --侠客组
			msg.AddMsgEvent(MsgID, 17,	Act18_RoleData[KillerID].KillPlayer)     --杀人数
			msg.DispatchRoleMsgEvent(KillerID, MsgID)
			--加少死亡者积分
			--Act18_TeamData[RoleTeamKey].Score = Act18_TeamData[RoleTeamKey].Score - PunishScore
			--SendScore2Teamate(153, Act18_TeamData[RoleTeamKey].Score, Act18_TeamRoleData[RoleTeamKey])
		end

		--豪杰组
		if InstanceID == 2 then
		    活动后15分钟
		    if Act19_LastTime > 15 then
		    	--击杀积分排名前10名的玩家，获得积分量等于普通状态的2倍
		    	local Index = 0
		    	for k in pairsByKeys(Act19_TeamData, Act19_Cmp) do
		    		if k == RoleTeamKey then
		    			Score = Score*2
		    			--PunishScore = 5
		    			break
		    		end

		    		if Index > 10 then
		    			break
		    		end

	    			Index = Index + 1
	    		end
	    	end
			--增加击杀数
			Act19_RoleData[KillerID].KillPlayer = Act19_RoleData[KillerID].KillPlayer + 1
			if Act19_RoleData[KillerID].KillPlayer % 10 == 0 and Act19_RoleData[KillerID].KillPlayer ~= 0 then
				--通知全服活动即将开始
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 102, 100108)    --【战报】XXX勇不可挡，完成X人斩！
				msg.AddMsgEvent(MsgID, 2, KillerID)
				msg.AddMsgEvent(MsgID, 9, Act19_RoleData[KillerID].KillPlayer)
				msg.DispatchBroadcast(MsgID, MapID, InstanceID,-1)
			end
			Act19_TeamRoleData[KillerTeamKey][KillerID].KillPlayer = Act19_TeamRoleData[KillerTeamKey][KillerID].KillPlayer + 1
			--计算击杀经验奖励
			local killexp = PKAct_killexp[RoleLevel]
            if PKAct19_killrole[KillerID] == nil then
			    PKAct19_killrole[KillerID] = {}
				if PKAct19_killrole[KillerID][RoleID] == nil then
				    PKAct19_killrole[KillerID][RoleID] = 0
				else
				    PKAct19_killrole[KillerID][RoleID] = PKAct19_killrole[KillerID][RoleID] + 1
				end
			else
			    if PKAct19_killrole[KillerID][RoleID] == nil then
				    PKAct19_killrole[KillerID][RoleID] = 0
				else
				    PKAct19_killrole[KillerID][RoleID] = PKAct19_killrole[KillerID][RoleID] + 1
				end
			end
			if PKAct19_killrole[KillerID][RoleID] < 10 then
			    role.AddRoleExp(2983743695, 2, KillerID, math.floor(killexp*(10-PKAct19_killrole[KillerID][RoleID])/10))
			    Score = math.floor(Score*(10-PKAct19_killrole[KillerID][RoleID])/10)
			else
			    Score = 0
			end
			--增加击杀者积分todo
			Act19_TeamData[KillerTeamKey].Score = Act19_TeamData[KillerTeamKey].Score + Score
			SendScore2Teamate(154, Act19_TeamData[KillerTeamKey].Score, Act19_TeamRoleData[KillerTeamKey])

			if PKAct19_killrole[KillerID][RoleID] < 10 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 101, 2860)           --击杀XXX，获得经验XXX点，积分XXX点
				msg.AddMsgEvent(MsgID, 2, RoleID)
				msg.AddMsgEvent(MsgID, 9, math.floor(killexp*(10-PKAct19_killrole[KillerID][RoleID])/10))
				msg.AddMsgEvent(MsgID, 9, Score)
				msg.DispatchRoleMsgEvent(KillerID, MsgID)
			end

			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 60,	154)								                --侠客组
			msg.AddMsgEvent(MsgID, 17,	Act19_RoleData[KillerID].KillPlayer)     --杀人数
			msg.DispatchRoleMsgEvent(KillerID, MsgID)
		end

		--英雄组
		if InstanceID == 3 then
		    活动后15分钟
		    if Act19_LastTime > 15 then
		    	--击杀积分排名前10名的玩家，获得积分量等于普通状态的2倍
		    	local Index = 0
		    	for k in pairsByKeys(Act19_TeamData, Act19_Cmp) do
		    		if k == RoleTeamKey then
		    			Score = Score*2
		    			--PunishScore = 5
		    			break
		    		end

		    		if Index > 10 then
		    			break
		    		end

	    			Index = Index + 1
	    		end
	    	end
			--增加击杀数
			Act20_RoleData[KillerID].KillPlayer = Act20_RoleData[KillerID].KillPlayer + 1
			if Act20_RoleData[KillerID].KillPlayer % 10 == 0 and Act20_RoleData[KillerID].KillPlayer ~= 0 then
				--通知全服活动即将开始
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 102, 100108)    --【战报】XXX勇不可挡，完成X人斩！
				msg.AddMsgEvent(MsgID, 2, KillerID)
				msg.AddMsgEvent(MsgID, 9, Act20_RoleData[KillerID].KillPlayer)
				msg.DispatchBroadcast(MsgID, MapID, InstanceID,-1)
			end
			Act20_TeamRoleData[KillerTeamKey][KillerID].KillPlayer = Act20_TeamRoleData[KillerTeamKey][KillerID].KillPlayer + 1
			--计算击杀经验奖励
			local killexp = PKAct_killexp[RoleLevel]
            if PKAct20_killrole[KillerID] == nil then
			    PKAct20_killrole[KillerID] = {}
				if PKAct20_killrole[KillerID][RoleID] == nil then
				    PKAct20_killrole[KillerID][RoleID] = 0
				else
				    PKAct20_killrole[KillerID][RoleID] = PKAct20_killrole[KillerID][RoleID] + 1
				end
			else
			    if PKAct20_killrole[KillerID][RoleID] == nil then
				    PKAct20_killrole[KillerID][RoleID] = 0
				else
				    PKAct20_killrole[KillerID][RoleID] = PKAct20_killrole[KillerID][RoleID] + 1
				end
			end
			if PKAct20_killrole[KillerID][RoleID] < 10 then
			    role.AddRoleExp(2983743695, 3, KillerID, math.floor(killexp*(10-PKAct20_killrole[KillerID][RoleID])/10))
			    Score = math.floor(Score*(10-PKAct20_killrole[KillerID][RoleID])/10)
			else
			    Score = 0
			end
			--增加击杀者积分todo
			Act20_TeamData[KillerTeamKey].Score = Act20_TeamData[KillerTeamKey].Score + Score
			SendScore2Teamate(155, Act20_TeamData[KillerTeamKey].Score, Act20_TeamRoleData[KillerTeamKey])

			if PKAct20_killrole[KillerID][RoleID] < 10 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 101, 2860)           --击杀XXX，获得经验XXX点，积分XXX点
				msg.AddMsgEvent(MsgID, 2, RoleID)
				msg.AddMsgEvent(MsgID, 9, math.floor(killexp*(10-PKAct20_killrole[KillerID][RoleID])/10))
				msg.AddMsgEvent(MsgID, 9, Score)
				msg.DispatchRoleMsgEvent(KillerID, MsgID)
			end

			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 60,	155)								                --侠客组
			msg.AddMsgEvent(MsgID, 17,	Act20_RoleData[KillerID].KillPlayer)     --杀人数
			msg.DispatchRoleMsgEvent(KillerID, MsgID)
		end

		--天下组
		if InstanceID == 4 then
		    活动后15分钟
		    if Act21_LastTime > 15 then
		    	--击杀积分排名前10名的玩家，获得积分量等于普通状态的2倍
		    	local Index = 0
		    	for k in pairsByKeys(Act21_TeamData, Act21_Cmp) do
		    		if k == RoleTeamKey then
		    			Score = Score*2
		    			--PunishScore = 5
		    			break
		    		end

		    		if Index > 10 then
		    			break
		    		end

	    			Index = Index + 1
	    		end
	    	end
			--增加击杀数
			Act21_RoleData[KillerID].KillPlayer = Act21_RoleData[KillerID].KillPlayer + 1
			if Act21_RoleData[KillerID].KillPlayer % 10 == 0 and Act21_RoleData[KillerID].KillPlayer ~= 0 then
				--通知全服活动即将开始
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 102, 100108)    --【战报】XXX勇不可挡，完成X人斩！
				msg.AddMsgEvent(MsgID, 2, KillerID)
				msg.AddMsgEvent(MsgID, 9, Act21_RoleData[KillerID].KillPlayer)
				msg.DispatchBroadcast(MsgID, MapID, InstanceID,-1)
			end
			Act21_TeamRoleData[KillerTeamKey][KillerID].KillPlayer = Act21_TeamRoleData[KillerTeamKey][KillerID].KillPlayer + 1
			--计算击杀经验奖励
			local killexp = PKAct_killexp[RoleLevel]
            if PKAct21_killrole[KillerID] == nil then
			    PKAct21_killrole[KillerID] = {}
				if PKAct21_killrole[KillerID][RoleID] == nil then
				    PKAct21_killrole[KillerID][RoleID] = 0
				else
				    PKAct21_killrole[KillerID][RoleID] = PKAct21_killrole[KillerID][RoleID] + 1
				end
			else
			    if PKAct21_killrole[KillerID][RoleID] == nil then
				    PKAct21_killrole[KillerID][RoleID] = 0
				else
				    PKAct21_killrole[KillerID][RoleID] = PKAct21_killrole[KillerID][RoleID] + 1
				end
			end
			if PKAct21_killrole[KillerID][RoleID] < 10 then
			    role.AddRoleExp(2983743695, 4, KillerID, math.floor(killexp*(10-PKAct21_killrole[KillerID][RoleID])/10))
			    Score = math.floor(Score*(10-PKAct21_killrole[KillerID][RoleID])/10)
			else
			    Score = 0
			end
			--增加击杀者积分todo
			Act21_TeamData[KillerTeamKey].Score = Act21_TeamData[KillerTeamKey].Score + Score
			SendScore2Teamate(156, Act21_TeamData[KillerTeamKey].Score, Act21_TeamRoleData[KillerTeamKey])

			if PKAct21_killrole[KillerID][RoleID] < 10 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 101, 2860)           --击杀XXX，获得经验XXX点，积分XXX点
				msg.AddMsgEvent(MsgID, 2, RoleID)
				msg.AddMsgEvent(MsgID, 9, math.floor(killexp*(10-PKAct21_killrole[KillerID][RoleID])/10))
				msg.AddMsgEvent(MsgID, 9, Score)
				msg.DispatchRoleMsgEvent(KillerID, MsgID)
			end

			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 60,	156)								                --侠客组
			msg.AddMsgEvent(MsgID, 17,	Act21_RoleData[KillerID].KillPlayer)     --杀人数
			msg.DispatchRoleMsgEvent(KillerID, MsgID)
		end
	end
end

aux.RegisterMapEvent("m22", 5, "m22_RoleDie")

-------------------------------------------------------------------------------------------------------
--排序比较函数
function Act18_Cmp(a,b)
    if Act18_TeamData[b].Score < Act18_TeamData[a].Score then
		return true
	elseif Act18_TeamData[b].Score == Act18_TeamData[a].Score and Act18_TeamData[b].JoinTime < Act18_TeamData[a].JoinTime then
		return true
	else
		return false
    end
end

-------------------------------------------------------------------------------------------------------
--玩家在活动中掉线后上线
function m22_CanTakeOverWhenOnline(MapID, RoleID)
	--修正玩家的上线地图和坐标todo
	return 3017299663, 2156, 18363, 2420
end

aux.RegisterMapEvent("m22",  12, "m22_CanTakeOverWhenOnline")

-------------------------------------------------------------------------------------------------------
--找到地图的出口和坐标
function m22_GetExporMapAndCoord(MapID, InstanceID, RoleID)
	return 1, 3017299663, 2156, 18363, 2420
end

aux.RegisterMapEvent("m22",  13, "m22_GetExporMapAndCoord")

-------------------------------------------------------------------------------------------------------
--找到最合适的副本实例
function m22_GetOnePerfectMap(MapID, InstanceID, RoleID)
	local RoleLevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
    if Act18_RoleData[RoleID] ~= nil then
	    return 1
	end

	if Act19_RoleData[RoleID] ~= nil then
	    return 2
	end

	if Act20_RoleData[RoleID] ~= nil then
	    return 3
	end

	if Act21_RoleData[RoleID] ~= nil then
	    return 4
	end
	--传送到侠客场地
	if RoleLevel >= 40 and RoleLevel < 60 then
		return 1
	end

	--传送到豪杰场地
	if RoleLevel >= 60 and RoleLevel < 80 then
		return 2
	end

	--传送到英雄场地
	if RoleLevel >= 80 and RoleLevel < 100 then
		return 3
	end

	--传送到天下场地
	if RoleLevel >= 100 then
		return 4
	end

	return 0
end

aux.RegisterMapEvent("m22",  14, "m22_GetOnePerfectMap")

-------------------------------------------------------------------------------------------------------
--检测玩家是够能够进入活动场景
function m22_CanEnter(MapID, InstanceID, RoleID)
	local RoleLevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local TeamKey = role.GetRoleScriptData(RoleID, 1, RoleDataType["Act18_TeamKey"])
	--玩家等级小于40级
	if RoleLevel < 40 then
		return 0
	end

	--如果玩家是死亡后离开，判断玩家当前金钱是否足够。
	local MoneyNum = role.GetRoleSilver(MapID, InstanceID, RoleID)
	--if RoleLevel < 60 then
		if Act18_TeamRoleData[TeamKey] and Act18_TeamRoleData[TeamKey][RoleID] and MoneyNum < 2000 then
			--提示玩家金钱不足
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20,	158)
			msg.AddMsgEvent(MsgID, 21,	5)			    --取消
		    msg.AddMsgEvent(MsgID, 1,	22)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
			return 0
		end
	--elseif RoleLevel < 80 then
		if Act19_TeamRoleData[TeamKey] and Act19_TeamRoleData[TeamKey][RoleID] and MoneyNum < 5000 then
			--提示玩家金钱不足
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20,	158)
			msg.AddMsgEvent(MsgID, 21,	5)			    --取消
		    msg.AddMsgEvent(MsgID, 1,	22)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
			return 0
		end
	--elseif RoleLevel < 100 then
		if Act19_TeamRoleData[TeamKey] and Act19_TeamRoleData[TeamKey][RoleID] and MoneyNum < 10000 then
			--提示玩家金钱不足
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20,	158)
			msg.AddMsgEvent(MsgID, 21,	5)			    --取消
		    msg.AddMsgEvent(MsgID, 1,	22)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
			return 0
		end
	--else
		if Act19_TeamRoleData[TeamKey] and Act19_TeamRoleData[TeamKey][RoleID] and MoneyNum < 20000 then
			--提示玩家金钱不足
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20,	158)
			msg.AddMsgEvent(MsgID, 21,	5)			    --取消
		    msg.AddMsgEvent(MsgID, 1,	22)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
			return 0
		end
	--end

	return 1
end

aux.RegisterMapEvent("m22",  15, "m22_CanEnter")

-------------------------------------------------------------------------------------------------------
--客户请求同步排名或活动界面
function Act18_DefaultRequest(ActID, RoleID, EventType)
	local Index = 1
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 50, 1)

	for k in pairsByKeys(Act18_TeamData, Act18_Cmp) do
		msg.AddMsgEvent(MsgID, 15, Index)
		msg.AddMsgEvent(MsgID, 2, Act18_TeamData[k].LeaderID)
		msg.AddMsgEvent(MsgID, 14, Act18_TeamData[k].Score)
		msg.AddMsgEvent(MsgID, 18, k)

		Index = Index + 1
		if Index > 10 then
			break
		end
	end
	msg.DispatchRoleMsgEvent(RoleID, MsgID)
end

aux.RegisterActEvent(18, 5, "Act18_DefaultRequest")
aux.RegisterActEvent(103, 5, "Act18_DefaultRequest")

-------------------------------------------------------------------------------------------------------
--活动中的敌我判断
function m22_FriendEnemy(MapID, InstanceID, RoleID, TargetID)
	-- 先确定活动是否开始
	local bActIsStart = act.GetActIsStart(18)
    local bActIsStart1 = act.GetActIsStart(103)
	-- 活动没有开始无法pk
	if bActIsStart ~= true and bActIsStart1 ~= true then
		return false, false, false, true
	end

	if unit.IsPlayer(RoleID) and unit.IsPlayer(TargetID) then
	    local RoleTeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
	    local TargetTeamID = role.IsRoleHaveTeam(MapID, InstanceID, TargetID)
		if RoleTeamID ~= 4294967295 and RoleTeamID ~= TargetTeamID then
		    return false, true, false, true
		elseif RoleTeamID == 4294967295 then
		    return false, true, false, true
		else
		    return false, false, false, false
		end
	else
		return false, false, false, false
	end
end

aux.RegisterMapEvent("m22", 16, "m22_FriendEnemy")

-------------------------------------------------------------------------------------------------------
--活动中无法邀请其它玩家组队
function m22_CanInviteJoinTeam(MapID, InstanceID)
    return 9
end

aux.RegisterMapEvent("m22", 8, "m22_CanInviteJoinTeam")

-------------------------------------------------------------------------------------------------------
--活动中玩家无法离开小队
function m22_CanLeaveTeam(MapID, InstanceID)
    return 10
end

aux.RegisterMapEvent("m22", 9, "m22_CanLeaveTeam")

-------------------------------------------------------------------------------------------------------
--活动中无法移交队长
function m22_CanChangeLeader(MapID, InstanceID)
    return 11
end

aux.RegisterMapEvent("m22", 10, "m22_CanChangeLeader")

-------------------------------------------------------------------------------------------------------
--活动中小队无法踢人
function m22_CanKickMember(MapID, InstanceID)
    return 12
end
aux.RegisterMapEvent("m22", 17, "m22_CanKickMember")]]
