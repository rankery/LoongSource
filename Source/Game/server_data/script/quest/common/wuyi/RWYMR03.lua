--RWYMR03：查微知著
--需求1：在莲花池[x,y,z]，明镜台[x,y,z]，须弥台[x,y,z]，百转源[x,y,z]四地区域使用物品[空明盘][2614026],各得到[莲花池记录][2614027],[明镜台记录][2614028],[须弥台记录][2614029],[百转源记录][2614030]
--莲花池地图区域objID为5494(地图编号为m06)
--明镜台地图区域objID为5496(地图编号为m06)
--须弥台地图区域objID为7929(地图编号为m06_1)
--百转源地图区域objID为7930(地图编号为m06_1)


function I2614026_CanUse(MapID, InstanceID, TypeID, RoleID)
	local bRet, bIgnore = 0, false
	local a = map.IsInArea(3017299151, InstanceID, RoleID, 5494)
	local b = map.IsInArea(3017299151, InstanceID, RoleID, 5496)
	local c = map.IsInArea(1146339967, InstanceID, RoleID, 7929)
	local d = map.IsInArea(1146339967, InstanceID, RoleID, 7930)
	if a ~= true and b ~= true and c ~= true and d ~= true then --必须在以上四区域使用
		bRet = 43
	end
	return bRet, bIgnore

end

function I2614026_QuestUsable(MapID, InstanceID, TypeID, RoleID)
	local a = map.IsInArea(3017299151, InstanceID, RoleID, 5494)
	local b = map.IsInArea(3017299151, InstanceID, RoleID, 5496)
	local c = map.IsInArea(1146339967, InstanceID, RoleID, 7929)
	local d = map.IsInArea(1146339967, InstanceID, RoleID, 7930)
	if a == true then
	role.QuestAddRoleItem(MapID, InstanceID, RoleID, 14303, 2614027, 1, -1, 101)
	elseif b == true then
	role.QuestAddRoleItem(MapID, InstanceID, RoleID, 14303, 2614028, 1, -1, 101)
	elseif c == true then
	role.QuestAddRoleItem(MapID, InstanceID, RoleID, 14303, 2614029, 1, -1, 101)
	elseif d == true then
	role.QuestAddRoleItem(MapID, InstanceID, RoleID, 14303, 2614030, 1, -1, 101)
	end
end

aux.RegisterItemEvent(2614026, 1, "I2614026_QuestUsable")
aux.RegisterItemEvent(2614026, 0, "I2614026_CanUse")
