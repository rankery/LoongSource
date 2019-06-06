--[[固定活动群雄逐鹿豪杰组

--ScriptData中各个字段意义说明
--0:活动的届数。1- 20为前10名小队编号，顺序按照名次进行存储。

--活动中玩家数据(主键RoleID, RebornTime, IsDeadLeave, IsInMap)
Act19_RoleData = {} --侠客组

--活动中玩家数据(主键主键小队编号, Act19_RoleData1, Act19_RoleData2, Act19_RoleData3，Act19_RoleData4，Act19_RoleData5，Act19_RoleData6)
Act19_TeamRoleData = {}

--活动中的玩家小队数据(主键小队编号, Score, JoinTime)
Act19_TeamData = {}

--固定活动已开始的时间（单位是分钟）
Act19_LastTime = 0

--活动中小队编号最大值
Act19_TeamIndex = 1

--活动当前届数
Act19_Expire = 0

--是否已经创建了副本
IsCreateAct19Instance = 0

--活动奖励
Act19_RewardItem = {1220007, 1230007, 1210003, 1420007, 1420011, 1420015, 1420019, 1420031, 1420035}

--活动中刷出的晶球数量
Act19_CrystalNum = 0

--记录每个玩家杀掉的敌人的次数
PKAct19_killrole = {}

-------------------------------------------------------------------------------------------------------
--排序比较函数
function Act19_Cmp(a,b)
    if Act19_TeamData[b].Score < Act19_TeamData[a].Score then
		return true
	elseif Act19_TeamData[b].Score == Act19_TeamData[a].Score and Act19_TeamData[b].JoinTime < Act19_TeamData[a].JoinTime then
		return true
	else
		return false
    end
end

--------------------------------------------------------------------------------------------------------
--存储当前排名
function SaveAct19TeamData()
    act.SetActScriptData(19, 1, 0, Act19_Expire)
    local Index = 1
    for k in pairsByKeys(Act19_TeamData, Act19_Cmp) do
        act.SetActScriptData(19, 1, Index, k)
        Index = Index + 1
        if Index > 10 then
            break
        end
	end
end

--------------------------------------------------------------------------------------------------------
--固定活动开始
function act19_OnStart(ActID)
	--初始化活动已进行时间
	Act19_LastTime = 0

	--活动中刷出的晶球数量
    Act19_CrystalNum = 0
	Act19_Expire = act.GetActScriptData(19, 1, 0)

	--同步所有场中中玩家在线时间
	for n in pairs(Act19_TeamRoleData) do
		for k in pairs(Act19_TeamRoleData[n]) do
		    if Act19_TeamRoleData[n][k].IsInMap then
		        --发送活动剩余时间
	            local MsgID = msg.BeginMsgEvent()
	            msg.AddMsgEvent(MsgID, 60,	153)
	            msg.AddMsgEvent(MsgID, 16,	1800)							                 --剩余时间
	            msg.DispatchRoleMsgEvent(k, MsgID)

	            --设置玩家为入擂台状态(用于客户端敌我判断）
	            role.SetRoleState(k, 8, true)
	        end
	    end
	end
end

function act19_OnTimerMin(ActID)
	--累加活动进行时间
    Act19_LastTime = Act19_LastTime + 1

		for k in pairs(Act19_TeamRoleData) do
			for n in pairs(Act19_TeamRoleData[k]) do
				--玩家在活动地图中
				if Act19_TeamRoleData[k][n].IsInMap then
					--玩家没有死亡
					Act19_TeamData[k].Score = Act19_TeamData[k].Score + 6
					--玩家是否在活跃区域
					if map.IsInArea(2983743695, 2, n, 636)	then
						if unit.IsDead(2983743695, 2, n) then
							Act19_TeamData[k].Score = Act19_TeamData[k].Score + 6
						end
					end
				end
			end
			--同步客户端积分
			SendScore2Teamate(154, Act19_TeamData[k].Score, Act19_TeamRoleData[k])
		end
    if Act19_LastTime % 5 == 0 then
		local total = 0
		local avaliable = {}
		for ii=1,20 do
			if m22_SpawnPos[ii].Crystal60 == false then
				total = total+1
				avaliable[total] = ii
			end
		end
		while Act19_CrystalNum < 5 and total > 0 do
		    --随机一个坐标
			local Index = math.random(1,total)
			Index = avaliable[Index]
		    avaliable[Index] = avaliable[total]
			Act19_CrystalNum = Act19_CrystalNum + 1
			m22_SpawnPos[Index].Crystal60 = true
		    total = total -1
			--随机得到一个水晶球ID
			local CrystalBallID = math.random(1520001, 1520007)
			--刷出水晶球
			local CreatureID = map.MapCreateCreature(2983743695, 2, CrystalBallID, m22_SpawnPos[Index].x, m22_SpawnPos[Index].y, m22_SpawnPos[Index].z)
			cre.SetCreatureScriptData(2983743695, 2, CreatureID, 1, 0, Index)
		end
	end

	--累加玩家参加活动的时间
	for k in pairs(Act19_TeamData) do
	    Act19_TeamData[k].JoinTime = Act19_TeamData[k].JoinTime + 1
	    if (Act19_TeamData[k].JoinTime % 10) == 0 then
		    for n in pairs(Act19_TeamRoleData[k]) do
		        if Act19_TeamRoleData[k][n].IsInMap and false == unit.IsDead(2983743695, 2, n)then
			        --奖励2000银
			        role.AddRoleSilver(2983743695, 2, n, 2000, 1001)
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 101, 2853)   --您参与活动的累计时间达到10的倍数，获得额外的金钱奖励
					msg.DispatchRoleMsgEvent(n, MsgID)
			    end
			end
		end
	end
end

--活动结束
function act19_OnEnd(actID)
	--删除活动副本
	IsCreateAct19Instance = 0
	SaveAct19TeamData()
    PKAct19_killrole = {}
    Act19_RoleData = {}
	local rank ={}
	for i=1,10 do
	    rank[act.GetActScriptData(19, 1, i)] = i
	end

	--随机抽取一件奖励给活动中存活的玩家
	for k in pairs(Act19_TeamData) do
	    --随机抽取一件奖励给活动中存活的玩家
		for n in pairs(Act19_TeamRoleData[k]) do
			if Act19_TeamRoleData[k][n].IsInMap then
				local AddExp = role.GetRoleLevelUpExp(2983743695, 2, n)
				local rolelevel = role.GetRoleLevel(2983743695, 2, n)
				local Scoreact = Act19_TeamData[k].Score
				if Scoreact >3000 then
					Scoreact = 3000
				end
				local exp = AddExp * Scoreact / 3000 * 130 / (rolelevel - 25) / (rolelevel - 25)
				role.AddRoleExp(2983743695, 2, n, exp)
				local wushiyin = 0
				if Act19_TeamData[k].Score >= 2000 then
					wushiyin = 30
				elseif Act19_TeamData[k].Score >= 1000 then
					wushiyin = 20
				elseif Act19_TeamData[k].Score >= 500 then
					wushiyin = 15
				elseif Act19_TeamData[k].Score >= 250 then
					wushiyin = 10
				elseif Act19_TeamData[k].Score >= 150 then
					wushiyin = 6
				elseif Act19_TeamData[k].Score >= 100 then
					wushiyin = 4
				elseif Act19_TeamData[k].Score >= 50 then
					wushiyin = 2
				else
					wushiyin = 1
				end
				if rank[k] ~= nil then
					if rank[k] == 1 then
						--role.AddRoleItem(2983743695, 2, n, 2612621, 1, -1, 13, 1001)
						--role.AddRoleItem(2983743695, 2, n, 2612629, 10, -1, 13, 1001)
						role.AddRoleItem(2983743695, 2, n, 2616345, wushiyin, -1, 13, 1001)
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
					   --role.AddRoleItem(2983743695, 2, n, 2612629, itemnum, -1, 13, 1001)
						role.AddRoleItem(2983743695, 2, n, 2616345, wushiyin, -1, 13, 1001)
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 71, 2858)  --恭喜您在群雄逐鹿活动中获得第X名，得到经验<color=0xffff0000>XX<color=0xfffff7e0>点和<color=0xffff0000>X<color=0xfffff7e0>个<color=0xff00a2ff>武士印<color=0xfffff7e0>。您可以在涿鹿英雄榜处领取<color=0xffff0000>X<color=0xfffff7e0>个<color=0xff00a2ff>英雄帖<color=0xfffff7e0>！
						msg.AddMsgEvent(MsgID, 9, rank[k])
						msg.AddMsgEvent(MsgID, 9, exp)
						msg.AddMsgEvent(MsgID, 9, wushiyin)
						msg.AddMsgEvent(MsgID, 9, itemnum)
						msg.DispatchRoleMsgEvent(n, MsgID)
					end
				else
					role.AddRoleItem(2983743695, 2, n, 2616345, wushiyin, -1, 13, 1001)
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 71, 2859)  --您在群雄逐鹿活动中获得经验<color=0xffff0000>XX<color=0xfffff7e0>点和<color=0xffff0000>X<color=0xfffff7e0>个<color=0xff00a2ff>武士印<color=0xfffff7e0>希望您再接再励。
					msg.AddMsgEvent(MsgID, 9, exp)
					msg.AddMsgEvent(MsgID, 9, wushiyin)
					msg.DispatchRoleMsgEvent(n, MsgID)
				end
				--随机抽取一件物品作为奖励
				if 1 <= role.GetBagFreeSize(n) then
					role.AddRoleItem(2983743695, 2, n, 2612621, 1, -1, 13, 1001)

					--提示玩家"您参与英雄乱武活动，获得###奖励，希望下次再接再厉。"
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 71, 164)
					msg.AddMsgEvent(MsgID, 4, Act19_RewardItem[Index])
					msg.DispatchRoleMsgEvent(n, MsgID)
				end
			end
		end
	end
	map.DeleteInstance(2983743695, 2)
end

--注册该活动的相关事件
aux.RegisterActEvent(19, 2, "act19_OnStart") 	--侠客组
aux.RegisterActEvent(19, 4, "act19_OnTimerMin") --侠客组
aux.RegisterActEvent(19, 3, "act19_OnEnd") 		--侠客组
aux.RegisterActEvent(104, 2, "act19_OnStart") 	--侠客组
aux.RegisterActEvent(104, 4, "act19_OnTimerMin") --侠客组
aux.RegisterActEvent(104, 3, "act19_OnEnd") 		--侠客组
-------------------------------------------------------------------------------------------------------
--客户请求同步排名或活动界面
function Act19_DefaultRequest(ActID, RoleID, EventType)
	local Index = 1
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 50, 1)

	for k in pairsByKeys(Act19_TeamData, Act19_Cmp) do
		msg.AddMsgEvent(MsgID, 15, Index)
		msg.AddMsgEvent(MsgID, 2, Act19_TeamData[k].LeaderID)
		msg.AddMsgEvent(MsgID, 14, Act19_TeamData[k].Score)
		msg.AddMsgEvent(MsgID, 18, k)

		Index = Index + 1
		if Index > 20 then
			break
		end
	end
	msg.DispatchRoleMsgEvent(RoleID, MsgID)
end

aux.RegisterActEvent(19, 5, "Act19_DefaultRequest")
aux.RegisterActEvent(104, 5, "Act19_DefaultRequest")]]
