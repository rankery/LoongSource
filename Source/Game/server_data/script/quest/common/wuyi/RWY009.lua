
----RWY009 战神之谷[重复]
--需求1：与NPC姜樊对话接取任务后将玩家传送至副本战神谷 13412/50,1143,22267/50
--需求2：进入副本获得BUFF战神祝福，持续时间10分钟
--需求4: 完成任务离开副本至姜樊

--需求1：与NPC姜樊对话接取任务后将玩家传送至副本战神谷 13412/50,1143,22267/50
--需求2：与NPC姜樊对话接取任务后获得BUFF战神祝福，持续时间10分钟
function q14009_OnAccept(mapID, instanceID, questID, ownerID, accepterID)
	role.RoleGotoNewMap(mapID, instanceID, ownerID, 1826991143, 260, 1201, 460)
end

--需求4: 完成任务离开副本至姜樊
function q14009_OnComplete(MapID, InstanceID, QuestID, OwnerID, NPCID)
	role.RoleGotoNewMap(MapID, InstanceID, OwnerID, 3017299151, 957, 20140, 848)
end

aux.RegisterQuestEvent(14009, 0, "q14009_OnAccept")
aux.RegisterQuestEvent(14009, 1,"q14009_OnComplete")


--进入副本玩家获得一个buff 战神祝福
function q07_OnPlayerEnter(MapID, InstanceID,RoleID)
	 unit.AddBuff(MapID, InstanceID,RoleID, 2015801,RoleID)
end
--注册
aux.RegisterMapEvent("q07", 2, "q07_OnPlayerEnter")
