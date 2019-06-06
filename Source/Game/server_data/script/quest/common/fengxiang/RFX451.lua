--凶相毕露：11451
--需求1：接取任务时将玩家传送至吞山童子附近[m03: 2531, 15600, 2475]

--[[任务接取时函数
function x11451_OnAccept(mapID, instanceID, questID, ownerID, accepterID)
	role.RoleGotoNewMap(mapID, instanceID, ownerID, 3017298383, 2531, 15600, 2475)
end

--注册
aux.RegisterQuestEvent(11451, 0, "x11451_OnAccept")]]
