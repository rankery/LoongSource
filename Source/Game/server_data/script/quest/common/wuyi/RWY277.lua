--RWY277：苏迷幻界
--需求1：完成任务时传送进入副本苏迷幻界
function q14277_OnComplete(MapID, InstanceID, QuestID, RoleID, NPCID)
	role.RoleGotoNewMap(MapID, InstanceID, RoleID, 1826988839, 266, 1286, 270)
end

aux.RegisterQuestEvent(14277, 1, "q14277_OnComplete")
