--固定活动烽火连天

--烽火连天活动相关全局变量声明
Act4_Gan_Num = 0     --诱敌深入任务中交付的伪情报数量
Act4_Dead_Creature_Num = 0  --蛮族援军被击杀数
Act4_Forage = 0	--活动中收集的蛮族军粮数量


--时间事件的触发时间
act_Time = {900, 1800, 2700}

--刷出50只蛮族士兵
function FengHuo_CreateCreature()
	for i=1, 50 do
		local TypeID = math.random(1003158,1003160)
		local randPoint = math.random(1, 100)
		local x = map_list[3017298127].map[randPoint].x
		local y = map_list[3017298127].map[randPoint].y
		local z = map_list[3017298127].map[randPoint].z

		--刷出相应怪物
		map.MapCreateCreature(3017298127, -1, TypeID, x, y, z)
	end
end

--活动开始事件
function act4_OnStart(actID)
	--刷出50只蛮族士兵
	FengHuo_CreateCreature()
	--全服广播活动开始
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 100, 14)
	msg.DispatchMapMsgEvent(MsgID, 3017298127, -1)

end

function act4_OnInit(actID)
	--初始化活动时间事件的触发时间
	act.InitEventTime("act_Time", 3, actID)
	Act4_Forage = 0
end

function act4_OnTimer(actID)
	--刷出50只蛮族士兵
	FengHuo_CreateCreature()
end

--蛮族密信是否可用
function I2610159_CanUesItem(MapID, InstanceID, TypeID, TargetID)
	local bIgnore = false
	local bStart = false

	--检测玩家是否背包已满
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		return 40, bIgnore
	end

	--检测活动是否开始
	for i=5, 7 do
		bStart = act.GetActIsStart(i)
		if bStart then
			break
		end
	end

	--如果活动已经开始检测玩家是否已经接过任务
	if bStart then
		for k=20001, 20003 do
			if role.IsRoleHaveQuest(MapID, InstanceID, TargetID, k) then
				--提示玩家已经接取过该任务
				return 31, bIgnore
			end
		end
	else
		--提示玩家只能在活动中使用
		return 42, bIgnore
	end

	return 0, bIgnore
end

--使用蛮族密信，随机接取营救勇士、烧毁军粮、诱敌深入
function I2610159_UesItem(MapID, InstanceID, TypeID, TargetID)
	local QuestID = math.random(20001, 20003)
	if QuestID == 20001 then
		role.AddQuest(TargetID, 20001)

		--随机抽取三个触发器
		local Index = {}
			local i = 1
		while i ~=4 do
			local RandIndex = math.random(1, map_trigger[3017298127].triggerNum)
			Index[i] = map_trigger_m02[RandIndex]

			for k=1, i-1 do
				if Index[k] == Index[i] then
					i = i - 1
				end
			end
			i = i + 1
		end

		role.SetQuestScriptData(TargetID, 20001, 6, 1, Index[1], 2, Index[2], 3, Index[3])

	elseif QuestID == 20002 then

		role.AddQuest(TargetID, 20002)

	elseif QuestID == 20003 then

		role.AddQuest(TargetID, 20003)

	end
end

--[[注册该活动的相关事件
aux.RegisterActEvent(5, 2, "act4_OnStart")
aux.RegisterActEvent(5, 0, "act4_OnInit")
aux.RegisterActEvent(5, 1, "act4_OnTimer")
aux.RegisterActEvent(6, 2, "act4_OnStart")
aux.RegisterActEvent(6, 0, "act4_OnInit")
aux.RegisterActEvent(6, 1, "act4_OnTimer")
aux.RegisterActEvent(7, 2, "act4_OnStart")
aux.RegisterActEvent(7, 0, "act4_OnInit")
aux.RegisterActEvent(7, 1, "act4_OnTimer")
aux.RegisterItemEvent(2610159, 0, "I2610159_CanUesItem")
aux.RegisterItemEvent(2610159, 1, "I2610159_UesItem")]]

