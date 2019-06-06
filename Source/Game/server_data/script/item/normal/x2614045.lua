--战例需求：
--在武夷南部的[须弥台][x,y,z]，[百转源][x,y,z]，[天绝枯道][x,y,z]和[密幽林附近][x,y,z]区域，使用物品3支[魔族号角][2614045],各33%几率召唤出[碎厌分身][1007042][叱涅分身][1007043][青婴分身][1007044]
--须弥台地图区域objID为 7929(地图编号为m06_1)
--百转源地图区域objID为 7930(地图编号为m06_1)
--天绝枯道地图区域objID为 7931(地图编号为m06_1)
--密幽林附近地图区域objID为 7932(地图编号为m06_1)

function I2614045_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	--删除2支[魔族号角][2614045]
	role.RemoveFromRole(MapID, InstanceID, TargetID, 2614045, 2, 101)

	local Rate = math.random(1, 100)
	local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)

	--33%几率召唤出[碎厌分身][1007042]
	if Rate < 34 then
	map.MapCreateCreature(MapID, InstanceID, 1007042, x+2, y, z+2)

	--33%几率召唤出[叱涅分身][1007043]
	elseif Rate >=34 and Rate < 67 then
	map.MapCreateCreature(MapID, InstanceID, 1007043, x+2, y, z+2)

	--33%几率召唤出[青婴分身][1007044]
	elseif Rate>= 67 and Rate <= 100 then
	map.MapCreateCreature(MapID, InstanceID, 1007044, x+2, y, z+2)
	end
end

function I2614045_CanUse(MapID, InstanceID, TypeID, TargetID)
	local num = role.GetRoleItemNum(TargetID, 2614045)
	local bRet, bIgnore = 0, false
	local a = map.IsInArea(1146339967,InstanceID, TargetID, 7929)
	local b = map.IsInArea(1146339967,InstanceID, TargetID, 7930)
	local c = map.IsInArea(1146339967,InstanceID, TargetID, 7931)
	local d = map.IsInArea(1146339967,InstanceID, TargetID, 7932)

	if num < 3 then
		bRet = 32
	else
		if (a == false) and (b == false) and (c == false) and (d == false) then
		bRet = 43
		end
	end
	return bRet, bIgnore

end

aux.RegisterItemEvent(2614045, 1, "I2614045_QuestUsable")
aux.RegisterItemEvent(2614045, 0, "I2614045_CanUse")
