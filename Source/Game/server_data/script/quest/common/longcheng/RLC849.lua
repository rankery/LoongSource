--考察眼力：10849
--需求1：接任务后刷出3个小动物[3010235],[3010236],[3010237]

--任务接取时函数
function q10849_OnAccept(MapID, InstanceID, QuestID, RoleID, NpcID)
	local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
	map.MapCreateCreature(MapID, InstanceID, 3010235, x, y, z+2)
	map.MapCreateCreature(MapID, InstanceID, 3010236, x+2, y, z)
	map.MapCreateCreature(MapID, InstanceID, 3010237, x-2, y, z)

end

--注册
aux.RegisterQuestEvent(10849, 0, "q10849_OnAccept")

