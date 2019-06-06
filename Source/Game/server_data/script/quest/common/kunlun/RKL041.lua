--15041 与对话地物 魔火[3070707] update对话后 添加物品魔火[2615015]
--物品魔火[2615015]使用后召唤怪物 秉烛夜魔[1009019]
--魔火[2615015]只能在 昆仑 3543-50<x<3543+50,3320-50<z<3320+50 范围内使用

--NPC对话函数
function x15041_OnNPCTalk(MapID, InstanceID, QuestID, RoleID, NpcID, NpcTypeID)
    --检查是否为魔火[3070707]
	if NpcTypeID == 3070707 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 2615015, 1, -1, 8, 420)
	end
end

--注册
aux.RegisterQuestEvent(15041, 6, "x15041_OnNPCTalk")


--可否使用函数
function i2615015_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false
	--如果不在昆仑，则不能使用
	if MapID ~= 3017299407 then
	    return 43, false
	else
	--如果不在指定地点，则不能使用
	    local x,y,z = unit.GetPosition(MapID, InstanceID, TargetID)
	    if x < 3493 or x > 3593 or z < 3270 or z > 3370 then
	          return 43, false
            end
	end
    return bRet, bIgnore
end

--使用效果
function i2615015_QuestUsable(MapID, InstanceID, TypeID, TargetID)
       local x,y,z = unit.GetPosition(MapID, InstanceID, TargetID)
       map.MapCreateCreature(MapID, InstanceID, 1009019, x, y, z)
end

--注册
aux.RegisterItemEvent(2615015, 1, "i2615015_QuestUsable")
aux.RegisterItemEvent(2615015, 0, "i2615015_CanUse")
