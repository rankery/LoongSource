--[[固定活动群雄逐鹿豪杰组

--ScriptData中各个字段意义说明
--0:活动的届数。1- 20为前10名小队编号，顺序按照名次进行存储。

--活动中玩家数据(主键RoleID, RebornTime, IsDeadLeave, IsInMap)
Act21_RoleData = {} --侠客组

--活动中玩家数据(主键主键小队编号, Act18_RoleData1, Act18_RoleData2, Act18_RoleData3，Act18_RoleData4，Act18_RoleData5，Act18_RoleData6)
Act21_TeamRoleData = {}

--活动中的玩家小队数据(主键小队编号, Score, JoinTime)
Act21_TeamData = {}

--固定活动已开始的时间（单位是分钟）
Act21_LastTime = 0

--活动中小队编号最大值
Act21_TeamIndex = 1

--活动当前届数
Act21_Expire = 0

--是否已经创建了副本
IsCreateAct21Instance = 0

--活动奖励
Act21_RewardItem = {1220011, 1230011, 1210003, 1420007, 1420011, 1420015, 1420019, 1420031, 1420035}

--活动中刷出的晶球数量
Act21_CrystalNum = 0

--记录每个玩家杀掉的敌人的次数
PKAct21_killrole = {}
-------------------------------------------------------------------------------------------------------
--排序比较函数
function Act21_Cmp(a,b)
    if Act21_TeamData[b].Score < Act21_TeamData[a].Score then
		return true
	elseif Act21_TeamData[b].Score == Act21_TeamData[a].Score and Act21_TeamData[b].JoinTime < Act21_TeamData[a].JoinTime then
		return true
	else
		return false
    end
end

--------------------------------------------------------------------------------------------------------
--存储当前排名
function SaveAct21TeamData()
    act.SetActScriptData(21, 1, 0, Act21_Expire)
    local Index = 1
    for k in pairsByKeys(Act21_TeamData, Act21_Cmp) do
        act.SetActScriptData(21, 1, Index, k)
        Index = Index + 1
        if Index > 10 then
            break
        end
	end
end

--------------------------------------------------------------------------------------------------------
--固定活动开始
function Act21_OnStart(ActID)
	--初始化活动已进行时间
	Act21_LastTime = 0

	--活动中刷出的晶球数量
    Act21_CrystalNum = 0
	Act21_Expire = act.GetActScriptData(21, 1, 0)

	--同步所有场中中玩家在线时间
	for n in pairs(Act21_TeamRoleData) do
		for k in pairs(Act21_TeamRoleData[n]) do
		    if Act21_TeamRoleData[n][k].IsInMap then
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

function Act21_OnTimerMin(ActID)
	--累加活动进行时间
    Act21_LastTime = Act21_LastTime + 1

		for k in pairs(Act21_TeamRoleData) do
			for n in pairs(Act21_TeamRoleData[k]) do
				--玩家在活动地图中
				if Act21_TeamRoleData[k][n].IsInMap then
					--玩家没有死亡
					Act21_TeamData[k].Score = Act21_TeamData[k].Score + 6
					--玩家是否在活跃区域
					if map.IsInArea(2983743695, 4, n, 636)	then
						if unit.IsDead(2983743695, 4, n) then
							Act21_TeamData[k].Score = Act21_TeamData[k].Score + 6
						end
					end
				end
			end
			--同步客户端积分
			SendScore2Teamate(154, Act21_TeamData[k].Score, Act21_TeamRoleData[k])
		end
    if Act21_LastTime % 5 == 0 then
		local total = 0
		local avaliable = {}
		for ii=1,20 do
			if m22_SpawnPos[ii].Crystal100 == false then
				total = total+1
				avaliable[total] = ii
			end
		end
		while Act21_CrystalNum < 5 and total > 0 do
		    --随机一个坐标
			local Index = math.random(1,total)
			Index = avaliable[Index]
		    avaliable[Index] = avaliable[total]
			Act21_CrystalNum = Act21_CrystalNum + 1
			m22_SpawnPos[Index].Crystal100 = true
		    total = total -1
			--随机得到一个水晶球ID
			local CrystalBallID = math.random(1520001, 1520007)
			--刷出水晶球
			local CreatureID = map.MapCreateCreature(2983743695, 4, CrystalBallID, m22_SpawnPos[Index].x, m22_SpawnPos[Index].y, m22_SpawnPos[Index].z)
			cre.SetCreatureScriptData(2983743695, 4, CreatureID, 1, 0, Index)
		end
	end

	--累加玩家参加活动的时间
	for k in pairs(Act21_TeamData) do
	    Act21_TeamData[k].JoinTime = Act21_TeamData[k].JoinTime + 1
	    if (Act21_TeamData[k].JoinTime % 10) == 0 then
		    for n in pairs(Act21_TeamRoleData[k]) do
		        if Act21_TeamRoleData[k][n].IsInMap and false == unit.IsDead(2983743695, 1, n)then
			        --奖励2000银
			        role.AddRoleSilver(2983743695, 1, n, 2000, 1001)
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 101, 2853)   --您参与活动的累计时间达到10的倍数，获得额外的金钱奖励
					msg.DispatchRoleMsgEvent(n, MsgID)
			    end
			end
		end
	end
	--每隔15分钟，活动场景内策划指定地点刷新出1个凶星怪物
	if Act21_LastTime % 10 == 0 then
        local Index = math.random(1, 2)
        map.MapCreateCreature(2983743695, 4, 1520021, XiongXing_BornPos[Index].x, XiongXing_BornPos[Index].y, XiongXing_BornPos[Index].z)
	end
end

--活动结束
function Act21_OnEnd(actID)
	--删除活动副本
	IsCreateAct21Instance = 0
	SaveAct21TeamData()
    PKAct21_killrole = {}
    Act21_RoleData = {}
	local rank ={}
	for i=1,10 do
	    rank[act.GetActScriptData(21, 1, i)] = i
	end

	--随机抽取一件奖励给活动中存活的玩家
	for k in pairs(Act21_TeamData) do
	    --随机抽取一件奖励给活动中存活的玩家
		for n in pairs(Act21_TeamRoleData[k]) do
			if Act21_TeamRoleData[k][n].IsInMap then
				local AddExp = role.GetRoleLevelUpExp(2983743695, 4, n)
				local rolelevel = role.GetRoleLevel(2983743695, 4, n)
				local Scoreact = Act21_TeamData[k].Score
				if Scoreact >3000 then
					Scoreact = 3000
				end
				local exp = AddExp * Scoreact / 3000 * 130 / (rolelevel - 25) / (rolelevel - 25)
				role.AddRoleExp(2983743695, 4, n, exp)
				local wushiyin = 0
				if Act21_TeamData[k].Score >= 2000 then
					wushiyin = 30
				elseif Act21_TeamData[k].Score >= 1000 then
					wushiyin = 20
				elseif Act21_TeamData[k].Score >= 500 then
					wushiyin = 15
				elseif Act21_TeamData[k].Score >= 250 then
					wushiyin = 10
				elseif Act21_TeamData[k].Score >= 150 then
					wushiyin = 6
				elseif Act21_TeamData[k].Score >= 100 then
					wushiyin = 4
				elseif Act21_TeamData[k].Score >= 50 then
					wushiyin = 2
				else
					wushiyin = 1
				end
				if rank[k] ~= nil then
					if rank[k] == 1 then
						--role.AddRoleItem(2983743695, 4, n, 2612621, 1, -1, 13, 1001)
						--role.AddRoleItem(2983743695, 4, n, 2612629, 10, -1, 13, 1001)
						role.AddRoleItem(2983743695, 4, n, 2616345, wushiyin, -1, 13, 1001)
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
					   --role.AddRoleItem(2983743695, 4, n, 2612629, itemnum, -1, 13, 1001)
						role.AddRoleItem(2983743695, 4, n, 2616345, wushiyin, -1, 13, 1001)
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 71, 2858)  --恭喜您在群雄逐鹿活动中获得第X名，得到经验<color=0xffff0000>XX<color=0xfffff7e0>点和<color=0xffff0000>X<color=0xfffff7e0>个<color=0xff00a2ff>武士印<color=0xfffff7e0>。您可以在涿鹿英雄榜处领取<color=0xffff0000>X<color=0xfffff7e0>个<color=0xff00a2ff>英雄帖<color=0xfffff7e0>！
						msg.AddMsgEvent(MsgID, 9, rank[k])
						msg.AddMsgEvent(MsgID, 9, exp)
						msg.AddMsgEvent(MsgID, 9, wushiyin)
						msg.AddMsgEvent(MsgID, 9, itemnum)
						msg.DispatchRoleMsgEvent(n, MsgID)
					end
				else
					role.AddRoleItem(2983743695, 4, n, 2616345, wushiyin, -1, 13, 1001)
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 71, 2859)  --您在群雄逐鹿活动中获得经验<color=0xffff0000>XX<color=0xfffff7e0>点和<color=0xffff0000>X<color=0xfffff7e0>个<color=0xff00a2ff>武士印<color=0xfffff7e0>希望您再接再励。
					msg.AddMsgEvent(MsgID, 9, exp)
					msg.AddMsgEvent(MsgID, 9, wushiyin)
					msg.DispatchRoleMsgEvent(n, MsgID)
				end
				随机抽取一件物品作为奖励
				if 1 <= role.GetBagFreeSize(n) then
					role.AddRoleItem(2983743695, 4, n, 2612621, 1, -1, 13, 1001)

					--提示玩家"您参与英雄乱武活动，获得###奖励，希望下次再接再厉。"
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 71, 164)
					msg.AddMsgEvent(MsgID, 4, Act21_RewardItem[Index])
					msg.DispatchRoleMsgEvent(n, MsgID)
				end
			end
		end
	end
	map.DeleteInstance(2983743695, 4)
end

--注册该活动的相关事件
aux.RegisterActEvent(21, 2, "Act21_OnStart")
aux.RegisterActEvent(21, 4, "Act21_OnTimerMin")
aux.RegisterActEvent(21, 3, "Act21_OnEnd")
aux.RegisterActEvent(106, 2, "Act21_OnStart")
aux.RegisterActEvent(106, 4, "Act21_OnTimerMin")
aux.RegisterActEvent(106, 3, "Act21_OnEnd")

-------------------------------------------------------------------------------------------------------
--客户请求同步排名或活动界面
function Act21_DefaultRequest(ActID, RoleID, EventType)
	local Index = 1
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 50, 1)

	for k in pairsByKeys(Act21_TeamData, Act21_Cmp) do
		msg.AddMsgEvent(MsgID, 15, Index)
		msg.AddMsgEvent(MsgID, 2, Act21_TeamData[k].LeaderID)
		msg.AddMsgEvent(MsgID, 14, Act21_TeamData[k].Score)
		msg.AddMsgEvent(MsgID, 18, k)

		Index = Index + 1
		if Index > 10 then
			break
		end
	end
	msg.DispatchRoleMsgEvent(RoleID, MsgID)
end

aux.RegisterActEvent(21, 5, "Act21_DefaultRequest")
aux.RegisterActEvent(106, 5, "Act21_DefaultRequest")]]
