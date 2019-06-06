--围困之难：10423
--需求1：接受任务后刷新五只半里雄蜂[1003081]于原地，坐标为：
--[3401, 3642, 780]
--[3403, 3642, 784]
--[3402, 3642, 778]


--任务接取时函数
function q10423_OnAccept(MapID, InstanceID, QuestID, TargetID, AccepterID)
	--m02地图索引1，10423 半里雄蜂[1003081]记录数量
	local a = map.GetMapScriptData(MapID,InstanceID,1,1)

	if a < 15 then
		local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
		map.MapCreateCreature(MapID, InstanceID, 1003081, 3401, 3643, 780)
		map.MapCreateCreature(MapID, InstanceID, 1003081, 3403, 3643, 784)
		map.MapCreateCreature(MapID, InstanceID, 1003081, 3402, 3643, 778)
		map.SetMapScriptData(MapID,InstanceID,1,1,a+3)
	end
end

--注册
aux.RegisterQuestEvent(10423, 0, "q10423_OnAccept")

function q10423_OnCheckAccept(MapID, InstanceID, QuestID, RoleID, NPCID)
	local a = map.GetMapScriptData(MapID,InstanceID,1,1)
	if a == nil then
		a = 0
		map.SetMapScriptData(MapID,InstanceID,1,1,0)
	end
	if map.GetMapScriptData(MapID,InstanceID,1,1) >= 15 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 13, 0)
		msg.AddMsgEvent(MsgID, 1, 148)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end
	return 1
end

aux.RegisterQuestEvent(10423, 4, "q10423_OnCheckAccept")

--怪死亡，将ScriptData索引1 数值减1

function c1003081_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local a = map.GetMapScriptData(MapID,InstanceID,1,1) --副本Data索引1
	map.SetMapScriptData(MapID,InstanceID,1,1,a-1)
end
aux.RegisterCreatureEvent(1003081,4,"c1003081_OnDie")
