--[[固定活动英雄乱舞豪杰组

--ScriptData中各个字段意义说明
--1 - 20为前20名玩家的ID，顺序按照名次进行存储

--活动中玩家数据((主键RoleID, RebornTime, Score，IsDeadLeave, IsInMap, JoinTime)
Act15_RoleData = {} --豪杰组

--固定活动已经开始的时间(单位是分钟)
Act15_LastTime = 0

--活动奖励
Act15_RewardItem = {1220007, 1230007, 1210003, 1420007, 1420011, 1420015, 1420019, 1420031, 1420035}

--活动中刷出的晶球数量
Act15_CrystalNum = 0

--记录每个玩家杀掉的敌人的次数
PKAct15_killrole = {}

--排序比较函数
local function Act15_Cmp(a, b)
	if Act15_RoleData[b].Score < Act15_RoleData[a].Score then
		return true
	elseif Act15_RoleData[b].Score == Act15_RoleData[a].Score and Act15_RoleData[b].KillPlayer < Act15_RoleData[a].KillPlayer then
		return true
	elseif Act15_RoleData[b].Score == Act15_RoleData[a].Score and Act15_RoleData[b].KillPlayer == Act15_RoleData[a].KillPlayer and Act15_RoleData[b].JoinTime < Act15_RoleData[a].JoinTime then
		return true
	else
		return false
    end
end

--存储当前排名
function SaveAct15RoleData()
    local Index = 1
    for k in pairsByKeys(Act15_RoleData, Act15_Cmp) do
        act.SetActScriptData(15, 1, Index, k)
        Index = Index + 1
        if Index > 20 then
            break
        end
    end
end

--------------------------------------------------------------------------------------------------------
--固定活动开始
function act15_OnStart(ActID)
	--创建活动副本
	map.CreateInstance(2983744463, 2)

	--初始化活动进行时间
	Act15_LastTime = 0
	--初始化水晶球数量
	Act15_CrystalNum = 0

	--初始化活动中玩家数据
    local Empty = {}
	Act15_RoleData = Empty
end

function act15_OnTimerMin(ActID)
	--累加活动进行时间
	Act15_LastTime = Act15_LastTime + 1

	--活动开始后的第5整数倍时间（第5、10……50、55分钟）, 存活的玩家积分+5
	--if Act15_LastTime % 5 == 0 then
		for k in pairs(Act15_RoleData) do
			--玩家在活动地图中
			if Act15_RoleData[k].IsInMap then
			    Act15_RoleData[k].Score = Act15_RoleData[k].Score + 33
				--同步客户端积分
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 13,	1)
				msg.AddMsgEvent(MsgID, 1, 2851)			--活动时间积分奖励：积分增加33点
				msg.DispatchRoleMsgEvent(k, MsgID)
			    if map.IsInArea(2983744463, 2, k, 636)	then--玩家是否在活跃区域
					--玩家没有死亡
					if false == unit.IsDead(2983744463, 2, k) then
						Act15_RoleData[k].Score = Act15_RoleData[k].Score + 33
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 13,	1)
						msg.AddMsgEvent(MsgID, 1, 2852)			--处于活跃区域，积分奖励翻倍！
						msg.DispatchRoleMsgEvent(k, MsgID)
					end
				end
				--同步客户端积分
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 60,	153)								--侠客组
				msg.AddMsgEvent(MsgID, 14,	Act15_RoleData[k].Score)			--积分
				msg.DispatchRoleMsgEvent(k, MsgID)
			end
		end
    if Act15_LastTime % 5 == 0 then
		local total = 0
		local avaliable = {}
		for ii=1,20 do
			if m21_SpawnPos[ii].Crystal60 == false then
				total = total+1
				avaliable[total] = ii
			end
		end
		while Act15_CrystalNum < 5 and total > 0 do
		    --随机一个坐标
			local Index = math.random(1,total)
			Index = avaliable[Index]
		    avaliable[Index] = avaliable[total]
			Act15_CrystalNum = Act15_CrystalNum + 1
			m21_SpawnPos[Index].Crystal60 = true
		    total = total -1
			--随机得到一个水晶球ID
			local CrystalBallID = math.random(1520001, 1520007)
			--刷出水晶球
			local CreatureID = map.MapCreateCreature(2983744463, 2, CrystalBallID, m21_SpawnPos[Index].x, m21_SpawnPos[Index].y, m21_SpawnPos[Index].z)
			cre.SetCreatureScriptData(2983744463, 2, CreatureID, 1, 0, Index)
		end
	end

	--累加玩家参加活动的时间
	for n in pairs(Act15_RoleData) do
		if Act15_RoleData[n].IsInMap then
			Act15_RoleData[n].JoinTime = Act15_RoleData[n].JoinTime + 1
			if (Act15_RoleData[n].JoinTime % 10) == 0 and false == unit.IsDead(2983744463, 2, n) then
				--奖励2000银
				role.AddRoleSilver(2983744463, 2, n, 2000, 1001)
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 101, 2853)   --您参与活动的累计时间达到10的倍数，获得额外的金钱奖励
				msg.DispatchRoleMsgEvent(n, MsgID)
			end
		end
	end

	--每隔10分钟，活动场景内策划指定地点刷新出1个凶星怪物
	if Act15_LastTime % 10 == 0 then
        local Index = math.random(1, 2)
        map.MapCreateCreature(2983744463, 2, 1520020, XiongXing_BornPos[Index].x, XiongXing_BornPos[Index].y, XiongXing_BornPos[Index].z)
	end

end

--活动结束
function act15_OnEnd(actID)
	SaveAct15RoleData()
	PKAct15_killrole = {}
    local rank ={}
	for i=1,10 do
	    rank[act.GetActScriptData(15, 1, i)] = i
	end

	--随机抽取一件奖励给活动中存活的玩家
	for n in pairs(Act15_RoleData) do
		if Act15_RoleData[n].IsInMap then
		    local AddExp = role.GetRoleLevelUpExp(2983744463, 2, n)
			local rolelevel = role.GetRoleLevel(2983744463, 2, n)
			local Scoreact = Act15_RoleData[n].Score
			if Scoreact >3000 then
			    Scoreact = 3000
			end
			local exp = AddExp * Scoreact / 3000 * 130 / (rolelevel - 25) / (rolelevel - 25)
			role.AddRoleExp(2983744463, 2, n, exp)
			local wushiyin = 0
			if Act15_RoleData[n].Score >= 2000 then
                wushiyin = 30
            elseif Act15_RoleData[n].Score >= 1000 then
			    wushiyin = 20
			elseif Act15_RoleData[n].Score >= 500 then
			    wushiyin = 15
			elseif Act15_RoleData[n].Score >= 250 then
			    wushiyin = 10
			elseif Act15_RoleData[n].Score >= 150 then
			    wushiyin = 6
			elseif Act15_RoleData[n].Score >= 100 then
			    wushiyin = 4
			elseif Act15_RoleData[n].Score >= 50 then
			    wushiyin = 2
			else
			    wushiyin = 1
			end
			if rank[n] ~= nil then
			    if rank[n] == 1 then
				    --role.AddRoleItem(2983744463, 1, n, 2612621, 1, -1, 13, 1001)
					--role.AddRoleItem(2983744463, 1, n, 2612629, 10, -1, 13, 1001)
					role.AddRoleItem(2983744463, 2, n, 2616345, wushiyin, -1, 13, 1001)
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 71, 2854)  --您在群雄乱舞活动中获得第一名，得到经验<color=0xffff0000>XX<color=0xfffff7e0>点和<color=0xffff0000>X<color=0xfffff7e0>个<color=0xff00a2ff>武士印<color=0xfffff7e0>。您可以在涿鹿英雄榜处领取专属称号和<color=0xffff0000>10<color=0xfffff7e0>个<color=0xff00a2ff>英雄帖<color=0xfffff7e0>经验<color=0xffff0000>XX<color=0xfffff7e0>点！
					msg.AddMsgEvent(MsgID, 9, exp)
					msg.AddMsgEvent(MsgID, 9, wushiyin)
					msg.DispatchRoleMsgEvent(n, MsgID)
				elseif rank[n] <= 10 then
				    local itemnum = 1
					if rank[n] == 2 or rank[n] == 3 then
					    itemnum = 8
					elseif rank[n] == 4 or rank[n] == 5 or rank[n] == 6 then
					    itemnum = 4
					elseif rank[n] == 7 or rank[n] == 8 or rank[n] == 9 or rank[n] == 10 then
					    itemnum = 2
					end
				   --role.AddRoleItem(2983744463, 1, n, 2612629, itemnum, -1, 13, 1001)
				    role.AddRoleItem(2983744463, 2, n, 2616345, wushiyin, -1, 13, 1001)
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 71, 2855)  --恭喜您在群雄乱舞活动中获得第X名，得到经验<color=0xffff0000>XX<color=0xfffff7e0>点和<color=0xffff0000>X<color=0xfffff7e0>个<color=0xff00a2ff>武士印<color=0xfffff7e0>。您可以在涿鹿英雄榜处领取<color=0xffff0000>X<color=0xfffff7e0>个<color=0xff00a2ff>英雄帖<color=0xfffff7e0>！
					msg.AddMsgEvent(MsgID, 9, rank[n])
					msg.AddMsgEvent(MsgID, 9, exp)
					msg.AddMsgEvent(MsgID, 9, wushiyin)
					msg.AddMsgEvent(MsgID, 9, itemnum)
					msg.DispatchRoleMsgEvent(n, MsgID)
				end
			else
			    role.AddRoleItem(2983744463, 2, n, 2616345, wushiyin, -1, 13, 1001)
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 71, 2856)  --您在群雄乱舞活动中获得经验<color=0xffff0000>XX<color=0xfffff7e0>点和<color=0xffff0000>X<color=0xfffff7e0>个<color=0xff00a2ff>武士印<color=0xfffff7e0>希望您再接再励。
				msg.AddMsgEvent(MsgID, 9, exp)
				msg.AddMsgEvent(MsgID, 9, wushiyin)
				msg.DispatchRoleMsgEvent(n, MsgID)
			end
		end
    end
	--删除活动副本
	map.DeleteInstance(2983744463, 2)
end

--注册该活动的相关事件
aux.RegisterActEvent(15, 2, "act15_OnStart") 	--豪杰组
aux.RegisterActEvent(15, 4, "act15_OnTimerMin") --豪杰组
aux.RegisterActEvent(15, 3, "act15_OnEnd") 		--豪杰组
aux.RegisterActEvent(98, 2, "act15_OnStart") 	--豪杰组
aux.RegisterActEvent(98, 4, "act15_OnTimerMin") --豪杰组
aux.RegisterActEvent(98, 3, "act15_OnEnd") 		--豪杰组

--客户请求同步排名或活动界面
function Act15_DefaultRequest(ActID, RoleID, EventType)
	local Index = 1
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 50, 0)

	for k in pairsByKeys(Act15_RoleData, Act15_Cmp) do
		msg.AddMsgEvent(MsgID, 15, Index)
		msg.AddMsgEvent(MsgID, 2, k)
		msg.AddMsgEvent(MsgID, 14, Act15_RoleData[k].Score)

		Index = Index + 1
		if Index > 10 then
			break
		end
	end
	msg.DispatchRoleMsgEvent(RoleID, MsgID)
end


aux.RegisterActEvent(15, 5, "Act15_DefaultRequest")
aux.RegisterActEvent(98, 5, "Act15_DefaultRequest")]]
