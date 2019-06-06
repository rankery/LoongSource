--报告师傅：13469
--需求1：接取任务后刷新3只心欲魔[1006151]

--任务接取时函数
function q13469_OnAccept(MapID, InstanceID, QuestID, RoleID, AccepterID)

	--m05地图索引2，13469 心欲魔[1006151]记录数量
	local a = map.GetMapScriptData(MapID,InstanceID,1,2)
	if a == nil then
		a = 0
	end
	if a < 15 then
		local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
		map.MapCreateCreature(MapID, InstanceID, 1006151, x-2, y, z)
		map.MapCreateCreature(MapID, InstanceID, 1006151, x+2, y, z)
		map.MapCreateCreature(MapID, InstanceID, 1006151, x, y, z+2)
		map.SetMapScriptData(MapID,InstanceID,1,2,a+3)
	end
end

--注册
aux.RegisterQuestEvent(13469, 0, "q13469_OnAccept")

function q13469_OnCheckAccept(MapID, InstanceID, QuestID, RoleID, NPCID)
	if map.GetMapScriptData(MapID,InstanceID,1,2) >= 15 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 13, 0)
		msg.AddMsgEvent(MsgID, 1, 148)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end
	return 1
end

aux.RegisterQuestEvent(13469, 4, "q13469_OnCheckAccept")

--怪死亡，将ScriptData索引 数值减1

function c1006151_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local a = map.GetMapScriptData(MapID,InstanceID,1,2) --副本Data索引2
	map.SetMapScriptData(MapID,InstanceID,1,2,a-1)
end
aux.RegisterCreatureEvent(1006151,4,"c1006151_OnDie")
