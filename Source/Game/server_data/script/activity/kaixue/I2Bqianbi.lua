--使用后删除作业纸和铅笔，获得随机奖励
function I2Bqianbi(MapID, InstanceID, TypeID, TargetID)
	role.RemoveFromRole(MapID, InstanceID, TargetID, 3303207, 1, 420)
    local k = math.random(100)
	if k <= 3 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 3303210, 1, -1, 8, 420)
	elseif k <= 18 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 3303211, 1, -1, 8, 420)
	elseif k <= 38 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 3303212, 1, -1, 8, 420)
	elseif k <= 48 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 3303213, 1, -1, 8, 420)
	elseif k <= 51 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 3303214, 1, -1, 8, 420)
	elseif k <= 61 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 3303215, 1, -1, 8, 420)
	elseif k <= 76 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 3303216, 1, -1, 8, 420)
	elseif k <= 96 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 3303217, 1, -1, 8, 420)
	elseif k <= 100 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 3303218, 1, -1, 8, 420)
	end
end
--可否使用函数
function I2Bqianbi_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false
    local i = role.GetRoleItemNum(TargetID, 3303207)
	if i < 1 then
		bRet = 32
	end
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
	end
    return bRet, bIgnore
end

aux.RegisterItemEvent(3303208, 1, "I2Bqianbi")
aux.RegisterItemEvent(3303208, 0, "I2Bqianbi_CanUse")
