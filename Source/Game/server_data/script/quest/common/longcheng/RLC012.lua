--塔顶上龙城探秀：10012
--需求1：接受任务时传送至玄天龙城[m02: 2046, 10500, 2119]
--需求2：完成任务时传送至玄天龙城[m02：2176, 5800, 2083]

--任务接取时函数
function x10012_OnAccept(mapID, instanceID, questID, ownerID, accepterID)
	role.RoleGotoNewMap(mapID, instanceID, ownerID, 3017298127, 2046, 10500, 2119)
end

--任务提交时函数
function x10012_OnComplete(mapID, instanceID, questID, ownerID, enderID)
	role.RoleGotoNewMap(mapID, instanceID, ownerID, 3017298127, 2176, 5800, 2083)
end

--注册
aux.RegisterQuestEvent(10012, 0, "x10012_OnAccept")
aux.RegisterQuestEvent(10012, 1, "x10012_OnComplete")

