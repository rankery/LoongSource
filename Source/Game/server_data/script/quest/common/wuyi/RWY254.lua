--RWY254：深潭猛兽
--需求1：接完任务刷新BOSS深潭猛兽

function x14254_OnAccept(MapID, InstanceID, QuestID, TargetID, AccepterID)
	--m06地图索引5，14254 BOSS深潭猛兽 1007113 记录数量
	local a = map.GetMapScriptData(MapID,InstanceID,1,5)
	if a == nil then
		a = 0
	end
	if a < 5 then
		local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
		map.MapCreateCreature(MapID, InstanceID, 1007113, x, y, z+4)
		map.SetMapScriptData(MapID,InstanceID,1,5,a+1)
	end
end

aux.RegisterQuestEvent(14254, 0, "x14254_OnAccept")


function q14254_OnCheckAccept(MapID, InstanceID, QuestID, RoleID, NPCID)
	if map.GetMapScriptData(MapID,InstanceID,1,5) >= 5 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 13, 0)
		msg.AddMsgEvent(MsgID, 1, 148)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end
	return 1
end
aux.RegisterQuestEvent(14254, 4, "q14254_OnCheckAccept")



--怪死亡，将ScriptData索引 数值减1
function c1007113_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local a = map.GetMapScriptData(MapID,InstanceID,1,5) --副本Data索引5
	map.SetMapScriptData(MapID,InstanceID,1,5,a-1)
end
aux.RegisterCreatureEvent(1007113,4,"c1007113_OnDie")
