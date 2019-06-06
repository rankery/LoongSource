--RZL910
--使用朱红木匣
--试炼幻境mapscriptdata(1,0)记录的是BOSS是否被杀死。如果被杀死，则可以刷新小怪。
function i3200565_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false
	local i = role.GetBagFreeSize(TargetID)
	if i <= 2 then
	    bRet = 40
	end
	return bRet, bIgnore
end

--使用效果
function i3200565_QuestUsable(MapID, InstanceID, TypeID, TargetID)
       role.AddRoleItem(MapID, InstanceID, TargetID, 3200571, 1, -1, 8, 420)
       role.AddRoleItem(MapID, InstanceID, TargetID, 3200532, 1, -1, 8, 420)
       role.AddRoleItem(MapID, InstanceID, TargetID, 3200533, 1, -1, 8, 420)
end

--注册
aux.RegisterItemEvent(3200565, 1, "i3200565_QuestUsable")
aux.RegisterItemEvent(3200565, 0, "i3200565_CanUse")




--使用破力·龙啸于野
function i3200532_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false
    local i=role.GetRoleAttValue(MapID, InstanceID, TargetID, 24)
	if i <= 49 then
	    bRet = 32
	end
	--如果不在Q02，则不能使用
	if MapID ~= 1826990375 then
	    bRet = 43
	end

	--返回
	return bRet, bIgnore
end

--使用效果,使用龙游于野，消耗50龙隐
function i3200532_QuestUsable(MapID, InstanceID, TypeID, TargetID)
       unit.AddBuff(MapID, InstanceID, TargetID, 4040103, TargetID)
       unit.AddBuff(MapID, InstanceID, TargetID, 2019601, TargetID)
end

--注册
aux.RegisterItemEvent(3200532, 1, "i3200532_QuestUsable")
aux.RegisterItemEvent(3200532, 0, "i3200532_CanUse")

--使用破力·化龙于形
function i3200533_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false
	local i=role.GetRoleAttValue(MapID, InstanceID, TargetID, 24)
	if i <= 49 then
	    bRet = 32
	end
	--如果不在Q02，则不能使用
	if MapID ~= 1826990375 then
	    bRet = 43
	end
	--返回
	return bRet, bIgnore
end

--使用效果,使用化龙于形，消耗50龙隐
function i3200533_QuestUsable(MapID, InstanceID, TypeID, TargetID)
       unit.AddBuff(MapID, InstanceID, TargetID, 4040403, TargetID)
       unit.AddBuff(MapID, InstanceID, TargetID, 2019601, TargetID)
end

--注册
aux.RegisterItemEvent(3200533, 1, "i3200533_QuestUsable")
aux.RegisterItemEvent(3200533, 0, "i3200533_CanUse")



--怪物死亡回复龙隐值
function c1531007_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	unit.AddBuff(MapID, InstanceID, RoleID, 2017201, RoleID)
end

aux.RegisterCreatureEvent(1531007, 4, "c1531007_OnDie")

--怪物死亡回复龙隐值
function c1531008_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.SetMapScriptData(MapID, InstanceID, 1, 0, 0)
end

aux.RegisterCreatureEvent(1531008, 4, "c1531008_OnDie")

--NPC对话函数
function x12910_OnNPCTalk(mapID, instanceID, questID, ownerID, npcid, npctypeid)
    --检查是否为殷万筹
	if npctypeid == 3011002 then
		role.QuestAddRoleItem(mapID, instanceID, ownerID, questID, 3200565, 1, 0, 102)
	end
end

--注册
aux.RegisterQuestEvent(12910, 6, "x12910_OnNPCTalk")


--任务接取时函数
--function x12910_OnAccept(MapID, InstanceID, questID, ownerID, accepterID)
--	role.RoleGotoNewMap(MapID, InstanceID, ownerID, 1826990375, 171, 5, 32)

--end

--注册
--aux.RegisterQuestEvent(12910, 0, "x12910_OnAccept")


