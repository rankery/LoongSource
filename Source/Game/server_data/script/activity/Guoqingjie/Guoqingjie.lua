

--1006730	物资看守
--3610223	物资箱
--RoleDataType["guoqing_tongmingguiyu"]   = 152  记录国庆节玩家进入通冥鬼域副本调查物资箱子次数
--RoleDataType["Tongmingguiyu_wuzixiang"]   = 156  --记录国庆节玩家进入通冥鬼域副本次数

--对话物资库，刷出物资看守
function Wuzixiang_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)

	local  Level = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local  x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
	if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 4009)	-- "装满战备物资的箱子，不过看起来似乎有些异常？"
		msg.AddMsgEvent(MsgID, 24, TargetID)
		msg.AddMsgEvent(MsgID, 21, 4)	-- 选项
		msg.AddMsgEvent(MsgID, 1, 4010)	-- "试着调查一下（召唤物资看守）"
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	end

	if TalkIndex == 4 then  -- 试着调查一下（召唤物资看守）
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 4012)  --物资看守被召唤出来了，请大家做好战斗的准备
		msg.DispatchMapMsgEvent(MsgID, "m42", -1)
		map.MapCreateColCreature(MapID, InstanceID, 1006730, x, y, z, 1)
		map.MapDeleteCreature(MapID, InstanceID, Tongmingguiyu_wuzixiang) -- 删除物资箱
	end
end
aux.RegisterCreatureEvent(3610223, 7, "Wuzixiang_OnTalk")



--击杀物资看守，副本小队成员获得贵重物资道具

function c1006730_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
	local Role = {}
	Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
	for i=1, 6 do
	    if Role[i] and Role[i]~= 4294967295 then
		    local Target_MapID, Target_InstanceID = role.GetRoleMapID(Role[i])
			local k = math.random(2,6)
			if Target_MapID == MapID then
			    role.AddRoleItem(Target_MapID, Target_InstanceID, Role[i], 6000110, k, -1, 8, 420) --为副本中的小队成员每人添加2~6个贵重物资
			end
	    end
	end

end
--注册
aux.RegisterCreatureEvent(1006730,4,"c1006730_OnDie")


--使用杂兵包裹获得烟花，战例等道具
--2431162	[利]开光灵符
--2431322	[利]散碎法器


function I6000109_OnUse(MapID, InstanceID, TypeID, RoleID)
	local k = math.random(2)
	local l = math.random(2)
	if k == 1 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 6000111, 1, -1, 8, 420)
	else
		role.AddRoleItem(MapID, InstanceID, RoleID, 6000112, 1, -1, 8, 420)
	end
	if l == 1 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 2431162, 1, -1, 8, 420)
	else
		role.AddRoleItem(MapID, InstanceID, RoleID, 2431322, 1, -1, 8, 420)
	end
end



function I6000109_CanUse(MapID, InstanceID, TypeID, RoleID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(RoleID)
	if(FreeSize < 2) then
		--提示玩家背包空间不足
		bRet = 40
		return bRet, bIgnore
	end
end


--在NPC阅兵处可以释放烟花，获得各类道具

--9437701			普天同庆
--9437801		烟花
--9438001			烟花



function I6000111_OnUse(MapID, InstanceID, TypeID, RoleID)

	local x, y, z = unit.GetPosition(MapID, InstanceID, RoleID)
	local a = map.MapCreateCreature(MapID, InstanceID, 4900601, x+2, y, z+2)
	unit.AddBuff(MapID, InstanceID, a, 2023201, a)
	unit.AddBuff(MapID, InstanceID, RoleID, 9437701, RoleID)
end



function I6000111_CanUse(MapID, InstanceID, TypeID, RoleID)
	local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
	local t = map.IsInArea(3017299663, -1, RoleID, 10994)	--返回true或false
	local FreeSize = role.GetBagFreeSize(RoleID)
	if Guoqingjie == 1 then
		if Npcyuebing ~= 1 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 4013) -- 龙神阅兵尚未开始，请在龙神阅兵开始后，站在阅兵队伍附近使用
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
			return 32, false
		elseif t ~= true  then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 4014) -- 必须站在龙神阅兵队伍附近才可以使用
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
			return 32, false
		elseif (FreeSize < 1) then
			return 40, false
		else
			return 0, false
		end
	else 							--国庆节期间限制在固定时间和地点开启，国庆节之后，无限制。
		if (FreeSize < 1) then
			return 40, false
		else
			return 0, false
		end
	end
end

function I6000112_OnUse(MapID, InstanceID, TypeID, RoleID)

	local x, y, z = unit.GetPosition(MapID, InstanceID, RoleID)
	local a = map.MapCreateCreature(MapID, InstanceID, 4900601, x+2, y, z+2)
	unit.AddBuff(MapID, InstanceID, a, 2023401, a)
	unit.AddBuff(MapID, InstanceID, RoleID, 9437701, RoleID)
end



function I6000112_CanUse(MapID, InstanceID, TypeID, RoleID)
	local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
	local t = map.IsInArea(3017299663, -1, RoleID, 10994)	--返回true或false
	local FreeSize = role.GetBagFreeSize(RoleID)
	if Guoqingjie == 1 then
		if Npcyuebing ~= 1 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 4013) -- 龙神阅兵尚未开始，请在龙神阅兵开始后，站在阅兵队伍附近使用
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
			return 32, false
		elseif t ~= true  then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 4014) -- 必须站在龙神阅兵队伍附近才可以使用
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
			return 32, false
		elseif (FreeSize < 1) then
			return 40, false
		else
			return 0, false
		end
	else 							--国庆节期间限制在固定时间和地点开启，国庆节之后，无限制。
		if (FreeSize < 1) then
			return 40, false
		else
			return 0, false
		end
	end
end



aux.RegisterItemEvent(6000109, 1, "I6000109_OnUse")
aux.RegisterItemEvent(6000111, 1, "I6000111_OnUse")
aux.RegisterItemEvent(6000112, 1, "I6000112_OnUse")
aux.RegisterItemEvent(6000109, 0, "I6000109_CanUse")
aux.RegisterItemEvent(6000111, 0, "I6000111_CanUse")
aux.RegisterItemEvent(6000112, 0, "I6000112_CanUse")



--完成任务
function x20330_OnComplete(MapID, InstanceID, QuestID, RoleID, NPCID)

    --[[local CurTime = tonumber(os.date("%j"))
	if role.GetRoleScriptData(RoleID, 1, RoleDataType["Guoqing_Date"]) ~= CurTime then
	    role.SetRoleScriptData(RoleID, 1, RoleDataType["Guoqing_Date"], CurTime)
		role.SetRoleScriptData(RoleID, 1, RoleDataType["Guoqing_WuZi"], 0)
	end]]
	role.SetRoleScriptData(RoleID, 1, RoleDataType["Guoqing_WuZi"], role.GetRoleScriptData(RoleID, 1, RoleDataType["Guoqing_WuZi"])+1)

end

function x20330_OnCheckAccept(MapID, InstanceID, QuestID, RoleID, NPCID)

	local CurTime = tonumber(os.date("%j"))
	if role.GetRoleScriptData(RoleID, 1, RoleDataType["Guoqing_Date"]) ~= CurTime then
	    role.SetRoleScriptData(RoleID, 1, RoleDataType["Guoqing_Date"], CurTime)
		role.SetRoleScriptData(RoleID, 1, RoleDataType["Guoqing_WuZi"], 0)
	end

	if role.GetRoleScriptData(RoleID, 1, RoleDataType["Guoqing_WuZi"]) >= 3 then
	    local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 147)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	    return 0
	else
	    return 1
	end

end

aux.RegisterQuestEvent(20330, 1, "x20330_OnComplete")
aux.RegisterQuestEvent(20330, 4, "x20330_OnCheckAccept")






































