--使用后删除作业纸和铅笔，获得随机奖励
function IHBqianbi(MapID, InstanceID, TypeID, TargetID)
	role.RemoveFromRole(MapID, InstanceID, TargetID, 3303207, 1, 420)
    local k = math.random(10000)
	if k <= 300 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 3303210, 1, -1, 8, 420)
	elseif k <= 1800 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 3303211, 1, -1, 8, 420)
	elseif k <= 3800 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 3303212, 1, -1, 8, 420)
	elseif k <= 4800 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 3303213, 1, -1, 8, 420)
	elseif k <= 4800 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 3303213, 1, -1, 8, 420)
	elseif k <= 5450 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 2005201, 1, -1, 8, 420)
	elseif k <= 5650 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1440201, 1, -1, 8, 420)
	elseif k <= 5850 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1440203, 1, -1, 8, 420)
	elseif k <= 6050 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1440205, 1, -1, 8, 420)
	elseif k <= 6250 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1440207, 1, -1, 8, 420)
	elseif k <= 6450 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1440208, 1, -1, 8, 420)
	elseif k <= 6650 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1441201, 1, -1, 8, 420)
	elseif k <= 6850 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1441203, 1, -1, 8, 420)
	elseif k <= 7050 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1441205, 1, -1, 8, 420)
	elseif k <= 7250 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1441207, 1, -1, 8, 420)
	elseif k <= 7450 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1441208, 1, -1, 8, 420)
	elseif k <= 7650 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1441701, 1, -1, 8, 420)
	elseif k <= 7850 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1441703, 1, -1, 8, 420)
	elseif k <= 8050 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1441705, 1, -1, 8, 420)
	elseif k <= 8250 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1441707, 1, -1, 8, 420)
	elseif k <= 8450 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1441708, 1, -1, 8, 420)
	elseif k <= 8650 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1442101, 1, -1, 8, 420)
	elseif k <= 8850 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1442103, 1, -1, 8, 420)
	elseif k <= 9050 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1442105, 1, -1, 8, 420)
	elseif k <= 9250 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1442107, 1, -1, 8, 420)
	elseif k <= 9450 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1442108, 1, -1, 8, 420)
	elseif k <= 9458 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1411001, 1, -1, 8, 420)
	elseif k <= 9466 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1411002, 1, -1, 8, 420)
	elseif k <= 9474 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1411003, 1, -1, 8, 420)
	elseif k <= 9482 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1411004, 1, -1, 8, 420)
	elseif k <= 9490 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1411005, 1, -1, 8, 420)
	elseif k <= 9575 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1440403, 1, -1, 8, 420)
	elseif k <= 9660 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1440404, 1, -1, 8, 420)
	elseif k <= 9745 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1441403, 1, -1, 8, 420)
	elseif k <= 9830 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1441404, 1, -1, 8, 420)
	elseif k <= 9915 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1441903, 1, -1, 8, 420)
	elseif k <= 10000 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1441904, 1, -1, 8, 420)
	end
end
--可否使用函数
function IHBqianbi_CanUse(MapID, InstanceID, TypeID, TargetID)
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

aux.RegisterItemEvent(3303209, 1, "IHBqianbi")
aux.RegisterItemEvent(3303209, 0, "IHBqianbi_CanUse")

