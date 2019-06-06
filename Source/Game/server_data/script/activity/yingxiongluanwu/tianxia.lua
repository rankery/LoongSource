--[[固定活动英雄乱舞天下组

--ScriptData中各个字段意义说明
--1 - 20为前20名玩家的ID，顺序按照名次进行存储

--活动中玩家数据((主键RoleID, RebornTime, Score，IsDeadLeave, IsInMap, JoinTime)
Act17_RoleData = {} --天下组

--固定活动已经开始的时间(单位是分钟)
Act17_LastTime = 0

--活动奖励
Act17_RewardItem = {1220007, 1230007, 1210003, 1420007, 1420011, 1420015, 1420019, 1420031, 1420035}

--活动中刷出的晶球数量
Act17_CrystalNum = 0

--记录每个玩家杀掉的敌人的次数
PKAct17_killrole = {}

--排序比较函数
local function Act17_Cmp(a, b)
	if Act17_RoleData[b].Score < Act17_RoleData[a].Score then
		return true
	elseif Act17_RoleData[b].Score == Act17_RoleData[a].Score and Act17_RoleData[b].KillPlayer < Act17_RoleData[a].KillPlayer then
		return true
	elseif Act17_RoleData[b].Score == Act17_RoleData[a].Score and Act17_RoleData[b].KillPlayer == Act17_RoleData[a].KillPlayer and Act17_RoleData[b].JoinTime < Act17_RoleData[a].JoinTime then
		return true
	else
		return false
    end
end

--存储当前排名
function SaveAct17RoleData()
    local Index = 1
    for k in pairsByKeys(Act17_RoleData, Act17_Cmp) do
        act.SetActScriptData(17, 1, Index, k)
        Index = Index + 1
        if Index > 20 then
            break
        end
    end
end

--------------------------------------------------------------------------------------------------------
--固定活动开始
function Act17_OnStart(ActID)
	--创建活动副本
	map.CreateInstance(2983744463, 4)

	--初始化活动进行时间
	Act17_LastTime = 0
	--初始化水晶球数量
	Act17_CrystalNum = 0

	--初始化活动中玩家数据
    local Empty = {}
	Act17_RoleData = Empty
end

function Act17_OnTimerMin(ActID)
	--累加活动进行时间
	Act17_LastTime = Act17_LastTime + 1

	--活动开始后的第5整数倍时间（第5、10……50、55分钟）, 存活的玩家积分+5
	--if Act17_LastTime % 5 == 0 then
		for k in pairs(Act17_RoleData) do
			--玩家在活动地图中
			if Act17_RoleData[k].IsInMap then
			    Act17_RoleData[k].Score = Act17_RoleData[k].Score + 33
				--同步客户端积分
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 13,	1)
				msg.AddMsgEvent(MsgID, 1, 2851)			--活动时间积分奖励：积分增加33点
				msg.DispatchRoleMsgEvent(k, MsgID)
			    if map.IsInArea(2983744463, 4, k, 636)	then--玩家是否在活跃区域
					--玩家没有死亡
					if false == unit.IsDead(2983744463, 4, k) then
						Act17_RoleData[k].Score = Act17_RoleData[k].Score + 33
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 13,	1)
						msg.AddMsgEvent(MsgID, 1, 2852)			--处于活跃区域，积分奖励翻倍！
						msg.DispatchRoleMsgEvent(k, MsgID)
					end
				end
				--同步客户端积分
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 60,	153)								--侠客组
				msg.AddMsgEvent(MsgID, 14,	Act17_RoleData[k].Score)			--积分
				msg.DispatchRoleMsgEvent(k, MsgID)
			end
		end
    if Act17_LastTime % 5 == 0 then
		local total = 0
		local avaliable = {}
		for ii=1,20 do
			if m21_SpawnPos[ii].Crystal100 == false then
				total = total+1
				avaliable[total] = ii
			end
		end
		while Act17_CrystalNum < 5 and total>0 do
		    --随机一个坐标
			local Index = math.random(1,total)
			Index = avaliable[Index]
		    avaliable[Index] = avaliable[total]
			Act17_CrystalNum = Act17_CrystalNum + 1
			m21_SpawnPos[Index].Crystal100 = true
		    total = total -1
			--随机得到一个水晶球ID
			local CrystalBallID = math.random(1520001, 1520007)
			--刷出水晶球
			local CreatureID = map.MapCreateCreature(2983744463, 4, CrystalBallID, m21_SpawnPos[Index].x, m21_SpawnPos[Index].y, m21_SpawnPos[Index].z)
			cre.SetCreatureScriptData(2983744463, 4, CreatureID, 1, 0, Index)
		end
	end

	--累加玩家参加活动的时间
	for n in pairs(Act17_RoleData) do
		if Act17_RoleData[n].IsInMap then
			Act17_RoleData[n].JoinTime = Act17_RoleData[n].JoinTime + 1
			if (Act17_RoleData[n].JoinTime % 10) == 0 and false == unit.IsDead(2983744463, 4, n) then
				--奖励2000银
				role.AddRoleSilver(2983744463, 4, n, 2000, 1001)
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 101, 2853)   --您参与活动的累计时间达到10的倍数，获得额外的金钱奖励
				msg.DispatchRoleMsgEvent(n, MsgID)
			end
		end
	end

	--每隔10分钟，活动场景内策划指定地点刷新出1个凶星怪物
	if Act17_LastTime % 10 == 0 then
        local Index = math.random(1, 2)
        map.MapCreateCreature(2983744463, 4, 1520020, XiongXing_BornPos[Index].x, XiongXing_BornPos[Index].y, XiongXing_BornPos[Index].z)
	end

end

--活动结束
function Act17_OnEnd(actID)
	SaveAct17RoleData()
	PKAct17_killrole = {}
    local rank ={}
	for i=1,10 do
	    rank[act.GetActScriptData(17, 1, i)] = i
	end

	--随机抽取一件奖励给活动中存活的玩家
	for n in pairs(Act17_RoleData) do
		if Act17_RoleData[n].IsInMap then
		    local AddExp = role.GetRoleLevelUpExp(2983744463, 4, n)
			local rolelevel = role.GetRoleLevel(2983744463, 4, n)
			local Scoreact = Act17_RoleData[n].Score
			if Scoreact >3000 then
			    Scoreact = 3000
			end
			local exp = AddExp * Scoreact / 3000 * 130 / (rolelevel - 25) / (rolelevel - 25)
			role.AddRoleExp(2983744463, 4, n, exp)
			local wushiyin = 0
			if Act17_RoleData[n].Score >= 2000 then
                wushiyin = 30
            elseif Act17_RoleData[n].Score >= 1000 then
			    wushiyin = 20
			elseif Act17_RoleData[n].Score >= 500 then
			    wushiyin = 15
			elseif Act17_RoleData[n].Score >= 250 then
			    wushiyin = 10
			elseif Act17_RoleData[n].Score >= 150 then
			    wushiyin = 6
			elseif Act17_RoleData[n].Score >= 100 then
			    wushiyin = 4
			elseif Act17_RoleData[n].Score >= 50 then
			    wushiyin = 2
			else
			    wushiyin = 1
			end
			if rank[n] ~= nil then
			    if rank[n] == 1 then
				    --role.AddRoleItem(2983744463, 1, n, 2612621, 1, -1, 13, 1001)
					--role.AddRoleItem(2983744463, 1, n, 2612629, 10, -1, 13, 1001)
					role.AddRoleItem(2983744463, 4, n, 2616345, wushiyin, -1, 13, 1001)
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
				    role.AddRoleItem(2983744463, 4, n, 2616345, wushiyin, -1, 13, 1001)
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 71, 2855)  --恭喜您在群雄乱舞活动中获得第X名，得到经验<color=0xffff0000>XX<color=0xfffff7e0>点和<color=0xffff0000>X<color=0xfffff7e0>个<color=0xff00a2ff>武士印<color=0xfffff7e0>。您可以在涿鹿英雄榜处领取<color=0xffff0000>X<color=0xfffff7e0>个<color=0xff00a2ff>英雄帖<color=0xfffff7e0>！
					msg.AddMsgEvent(MsgID, 9, rank[n])
					msg.AddMsgEvent(MsgID, 9, exp)
					msg.AddMsgEvent(MsgID, 9, wushiyin)
					msg.AddMsgEvent(MsgID, 9, itemnum)
					msg.DispatchRoleMsgEvent(n, MsgID)
				end
			else
			    role.AddRoleItem(2983744463, 4, n, 2616345, wushiyin, -1, 13, 1001)
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 71, 2856)  --您在群雄乱舞活动中获得经验<color=0xffff0000>XX<color=0xfffff7e0>点和<color=0xffff0000>X<color=0xfffff7e0>个<color=0xff00a2ff>武士印<color=0xfffff7e0>希望您再接再励。
				msg.AddMsgEvent(MsgID, 9, exp)
				msg.AddMsgEvent(MsgID, 9, wushiyin)
				msg.DispatchRoleMsgEvent(n, MsgID)
			end
		end
	end

	--删除活动副本
	map.DeleteInstance(2983744463, 4)
end

--注册该活动的相关事件
aux.RegisterActEvent(17, 2, "Act17_OnStart") 	--天下组
aux.RegisterActEvent(17, 4, "Act17_OnTimerMin") --天下组
aux.RegisterActEvent(17, 3, "Act17_OnEnd") 		--天下组
aux.RegisterActEvent(102, 2, "Act17_OnStart") 	--天下组
aux.RegisterActEvent(102, 4, "Act17_OnTimerMin") --天下组
aux.RegisterActEvent(102, 3, "Act17_OnEnd") 	--天下组

--客户请求同步排名或活动界面
function Act17_DefaultRequest(ActID, RoleID, EventType)
	local Index = 1
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 50, 0)

	for k in pairsByKeys(Act17_RoleData, Act17_Cmp) do
		msg.AddMsgEvent(MsgID, 15, Index)
		msg.AddMsgEvent(MsgID, 2, k)
		msg.AddMsgEvent(MsgID, 14, Act17_RoleData[k].Score)

		Index = Index + 1
		if Index > 10 then
			break
		end
	end
	msg.DispatchRoleMsgEvent(RoleID, MsgID)
end


aux.RegisterActEvent(17, 5, "Act17_DefaultRequest")
aux.RegisterActEvent(102, 5, "Act17_DefaultRequest")]]
