--战例需求：
--在武夷南部的[须弥台][x,y,z]，[百转源][x,y,z]，[天绝枯道][x,y,z]和[密幽林附近][x,y,z]四区域，使用物品5支[魔族令箭][2614044],召唤出[魔族将领][1007041]
--须弥台地图区域objID为 7929(地图编号为m06_1)
--百转源地图区域objID为 7930(地图编号为m06_1)
--天绝枯道地图区域objID为 7931(地图编号为m06_1)
--密幽林附近地图区域objID为 7932(地图编号为m06_1)




--在武夷南部的[须弥台][x,y,z]，[百转源][x,y,z]，[天绝枯道][x,y,z]和[密幽林附近][x,y,z]使用物品5支[魔族令箭][2614044],召唤出[魔族将领][1007041]
function I2614044_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	--删除5支[魔族令箭][2614044]
	role.RemoveFromRole(MapID, InstanceID, TargetID, 2614044, 4, 101)
	--召唤出[魔族将领][1007041]
	local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
	map.MapCreateCreature(MapID, InstanceID, 1007041, x+2, y, z+2)

end


function I2614044_CanUse(MapID, InstanceID, TypeID, TargetID)
	local num = role.GetRoleItemNum(TargetID, 2614044)
	local bRet, bIgnore = 0, false
	local a = map.IsInArea(1146339967, InstanceID, TargetID, 7929)
	local b = map.IsInArea(1146339967, InstanceID, TargetID, 7930)
	local c = map.IsInArea(1146339967, InstanceID, TargetID, 7931)
	local d = map.IsInArea(1146339967, InstanceID, TargetID, 7932)

	if num < 5 then
		bRet = 32
	else
		if (a == false) and (b == false) and (c == false) and (d == false) then
		bRet = 43
		end
	end
	return bRet, bIgnore

end



aux.RegisterItemEvent(2614044, 1, "I2614044_QuestUsable")
aux.RegisterItemEvent(2614044, 0, "I2614044_CanUse")
