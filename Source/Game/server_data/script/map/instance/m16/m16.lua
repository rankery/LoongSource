--ScriptData 索引
--0:物品 3200513 已用
--1:杀小怪数量
--2:礼天殿后三将
--3:礼天殿后BOSS
function m16_OnEnterArea(MapID, InstanceID, RoleID, ObjID)
--当角色进入objID为2907的地图区域时，将角色传送到x=936 y=3918 z=2342 这个位置
	if ObjID == 2907 then
	role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3000521935, 936, 3918, 2342)
--当角色进入objID为2908的地图区域时，将角色传送到x=936 y=3918 z=2342 这个位置
	elseif ObjID == 2908 then
	role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3000521935, 936, 3918, 2342)
	end
end

aux.RegisterMapEvent("m16", 7, "m16_OnEnterArea")

--当角色进入副本后，无法使用部分技能
--m16_SkillLimit = {}
--m16_SkillLimit[11005] = true
--m16_SkillLimit[11009] = true
--m16_SkillLimit[11012] = true
--m16_SkillLimit[11014] = true
--m16_SkillLimit[90012] = true

--function m16_CanUseSkill(MapID, InstanceID, SkillTypeID)
--    if m16_SkillLimit[SkillTypeID] then
--        return false
--    end
--    return true
--end

--aux.RegisterMapEvent("m16", 21, "m16_CanUseSkill")

--进入副本时删除玩家的坐骑buff
function m16_OnPlayerEnter(MapID, InstanceID,RoleID)
	local bhave = role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 99996)
	if bhave == true then
	 unit.CancelBuff(MapID, InstanceID, RoleID, 9999601)
	 end
end

--注册
aux.RegisterMapEvent("m16", 2, "m16_OnPlayerEnter")
