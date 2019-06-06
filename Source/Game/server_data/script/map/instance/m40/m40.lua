--SciptData 索引0 触发器901
--SciptData 索引1-7 用于祭魔台

m40_DataType = {}
m40_DataType["SumNum"] = 0
m40_DataType["roleid1"] = 1
m40_DataType["roleid2"] = 2
m40_DataType["roleid3"] = 3
m40_DataType["roleid4"] = 4
m40_DataType["roleid5"] = 5
m40_DataType["roleid6"] = 6

--8用于击杀boss情况记录
--9用于触发点刷怪
--10用于记录几个人领过经验奖励
--11用于击杀小怪总数

--m40 SciptData 记录触发器901 索引0数值为0则刷怪
function m40_OnEnterTrigger(MapID, InstanceID, RoleID, ObjID)

--激活触发器 901， 在坐标：x 726 y 5190 z 312 刷出 1005466， 按导航点 c01 巡逻
	if ObjID == 901 then
		local Activated0 = map.GetMapScriptData(MapID,InstanceID,1,0) --索引9数值为0则刷怪
		if Activated0 == 0 then --判断是否被触发过
		map.SetMapScriptData(MapID,InstanceID,1,9,1)
		map.MapCreateColCreature(MapID, InstanceID, 1005466, 726, 5190, 312, 1, "c01")
		end
	end
end
aux.RegisterMapEvent("m40",6,"m40_OnEnterTrigger")

--角色进入副本后，部分技能无法使用
--m40_SkillLimit = {}
--m40_SkillLimit[11005] = true
--m40_SkillLimit[11009] = true
--m40_SkillLimit[11012] = true
--m40_SkillLimit[11014] = true
--m40_SkillLimit[90012] = true

--function m40_CanUseSkill(MapID, InstanceID, SkillTypeID)
--    if m40_SkillLimit[SkillTypeID] then
--        return false
--    end
--    return true
--end

--aux.RegisterMapEvent("m40", 21, "m40_CanUseSkill")

--进入副本时删除玩家的坐骑buff
function m40_OnPlayerEnter(MapID, InstanceID,RoleID)
	local bhave = role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 99996)
	if bhave == true then
	 unit.CancelBuff(MapID, InstanceID, RoleID, 9999601)
	 end
end

--注册
aux.RegisterMapEvent("m40", 2, "m40_OnPlayerEnter")
