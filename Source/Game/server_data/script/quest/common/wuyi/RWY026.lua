--RWY026 铁证如山
--需求1：该任务第二环节，与NPC诘珂对话完成后刷出5个怪物诘珂的同党
--m06地图索引6，14026 诘珂的同党 1007026 记录数量

function n14026_OnNPCTalk(MapID, InstanceID, QuestID, RoleID, Npcid, Npctypeid)
	if Npctypeid == 3050019 then
		local a = map.GetMapScriptData(MapID,InstanceID,1,6)
		if a < 15 then
			local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
			map.MapCreateCreature(MapID, InstanceID, 1007026, x+4, y, z)
			map.MapCreateCreature(MapID, InstanceID, 1007026, x-4, y, z)
			map.MapCreateCreature(MapID, InstanceID, 1007026, x+2, y, z+2)

			map.SetMapScriptData(MapID,InstanceID,1,6,a+3)
		end
	end
end
aux.RegisterQuestEvent(14026, 6, "n14026_OnNPCTalk")

function q14026_OnCheckAccept(MapID, InstanceID, QuestID, RoleID, NPCID)
	local a = map.GetMapScriptData(MapID,InstanceID,1,6)
	if a == nil then
		a = 0
		map.SetMapScriptData(MapID,InstanceID,1,6,0)
	end
	if map.GetMapScriptData(MapID,InstanceID,1,6) >= 15 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 13, 0)
		msg.AddMsgEvent(MsgID, 1, 148)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end
	return 1
end
aux.RegisterQuestEvent(14026, 4, "q14026_OnCheckAccept")

--怪死亡，将ScriptData索引6 数值减1
function c1007026_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local a = map.GetMapScriptData(MapID,InstanceID,1,6) --副本Data索引1
	map.SetMapScriptData(MapID,InstanceID,1,6,a-1)
end
aux.RegisterCreatureEvent(1007026,4,"c1007026_OnDie")
