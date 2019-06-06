--[[人心难测：13233
--需求1：接取任务以后在NPC付江川身边刷新2个苍莽刀斧手[1006207]和一个苍莽死囚[1006209]，坐标为：
--[m02: 1949, 4500, 600]
--[m02: 1954, 4500, 585]
--[m02: 1943, 4500, 588]

--任务接取时
function x13233_OnAccept(MapID, InstanceID, QuestID, TargetID, AccepterID)

	--m02地图索引5，13233 个苍莽刀斧手[1006207]记录数量
	local a = map.GetMapScriptData(MapID,InstanceID,1,5)
	if a == nil then
		a = 0
	end
	if a < 10 then
		local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
		map.MapCreateCreature(MapID, InstanceID, 1006207, 1954, 4500, 585)--苍莽刀斧手
		map.MapCreateCreature(MapID, InstanceID, 1006207, 1943, 4500, 588)--苍莽刀斧手
		map.SetMapScriptData(MapID,InstanceID,1,5,a+2)
	end

	--m02地图索引6，13233 苍莽死囚[1006209]记录数量
	local b = map.GetMapScriptData(MapID,InstanceID,1,6)
	if b == nil then
		b = 0
	end
	if b < 5 then
		local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
		map.MapCreateCreature(MapID, InstanceID, 1006209, 1949, 4500, 600)
		map.SetMapScriptData(MapID,InstanceID,1,6,b+1)
	end

end

--注册
aux.RegisterQuestEvent(13233, 0, "x13233_OnAccept")

function q13233_OnCheckAccept(MapID, InstanceID, QuestID, RoleID, NPCID)
	if map.GetMapScriptData(MapID,InstanceID,1,5) >= 10 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 13, 0)
		msg.AddMsgEvent(MsgID, 1, 148)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	elseif map.GetMapScriptData(MapID,InstanceID,1,6) >= 5 then
		local MsgID1 = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID1, 13, 0)
		msg.AddMsgEvent(MsgID1, 1, 148)
		msg.DispatchRoleMsgEvent(RoleID, MsgID1)
		return 0
	end
	return 1
end

aux.RegisterQuestEvent(13233, 4, "q13233_OnCheckAccept")



--怪死亡，将ScriptData索引 数值减1

function c1006207_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local a = map.GetMapScriptData(MapID,InstanceID,1,5) --副本Data索引5
		if a < 1 or a > 50 then
			a = 1
		end
	map.SetMapScriptData(MapID,InstanceID,1,5,a-1)
end
aux.RegisterCreatureEvent(1006207,4,"c1006207_OnDie")
--怪物喊话脚本中已写死亡函数
function c1006209_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
    cre.MonsterSay(MapID, InstanceID, TargetID, 30011)
	local a = map.GetMapScriptData(MapID,InstanceID,1,6) --副本Data索引6
		if a < 1 or a > 50 then
			a = 1
		end
	map.SetMapScriptData(MapID,InstanceID,1,6,a-1)
end
aux.RegisterCreatureEvent(1006209,4,"c1006209_OnDie")
]]
