function ShenLongCaiLi(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 1330702, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1330703, 1, -1, 8, 420)

    --如果玩家等级大于50还可额外获得一个狂热者的证明
    local level = role.GetRoleLevel(MapID, InstanceID, TargetID)
    if level>=50 then
        role.AddRoleItem(MapID, InstanceID, TargetID, 3303502, 1, -1, 8, 420)
    end

end

function ShenLongCaiLi_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local level = role.GetRoleLevel(MapID, InstanceID, TargetID)
    local FreeSize = role.GetBagFreeSize(TargetID)
	local RequireSize = 2
	
    if level>=50 and Salary_NPC_On==1 then
        RequireSize =3
    end

    if(FreeSize < RequireSize) then
        --提示玩家背包空间不足
        bRet = 40
    end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1330701, 1, "ShenLongCaiLi")
aux.RegisterItemEvent(1330701, 0, "ShenLongCaiLi_CanUse")
