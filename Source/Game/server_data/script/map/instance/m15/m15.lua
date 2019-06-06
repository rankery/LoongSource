--map script data:
--0:赤风部众击杀数
--1:残风
--3:风鹿
--4:风刃


function m15_OnEnterTrigger(MapID, InstanceID, RoleID, ObjID)
	--进入触发器 1102
	--怪A X=338，Y=3856，Z=789
	--怪B X=360，Y=3856，Z=789
	--怪C X=337，Y=3856，Z=804
	--怪D X=363，Y=3856，Z=802
	--刷出自爆狂猿（ID：1004437）
	if ObjID == 1102 then
		local Activated0 = map.GetMapScriptData(MapID,InstanceID,1,0) --索引0数值为0则刷怪
		if Activated0 == 0 then
		map.SetMapScriptData(MapID,InstanceID,1,0,1)
		map.MapCreateCreature(MapID, InstanceID, 1004437, 338, 3856, 789)
		map.MapCreateCreature(MapID, InstanceID, 1004437, 360, 7248, 789)
		map.MapCreateCreature(MapID, InstanceID, 1004437, 337, 7248, 804)
		map.MapCreateCreature(MapID, InstanceID, 1004437, 363, 7248, 802)
		end
--进入触发器 1131
--怪A X=768，Y=4839，Z=715
--怪B X=777，Y=4776，Z=707
--怪C X=791，Y=4842，Z=717
--怪D X=770，Y=4810，Z=723
--刷出自爆狂猿（ID：1004437）
	elseif ObjID == 1131 then
		local Activated1 = map.GetMapScriptData(MapID,InstanceID,1,1) -----索引1数值为0则刷怪
		if Activated1 == 0 then
		map.SetMapScriptData(MapID,InstanceID,1,1,1)
		map.MapCreateCreature(MapID, InstanceID, 1004437, 768, 4839, 715)
		map.MapCreateCreature(MapID, InstanceID, 1004437, 777, 4776, 707)
		map.MapCreateCreature(MapID, InstanceID, 1004437, 791, 4842, 717)
		map.MapCreateCreature(MapID, InstanceID, 1004437, 770, 4810, 723)
		end
--进入触发器 1142
--怪A X=627，Y=3704，Z=303
--怪B X=617，Y=3742，Z=322
--刷出自爆狂猿（ID：1004437）
	elseif ObjID == 1142 then
		local Activated2 = map.GetMapScriptData(MapID,InstanceID,1,2) -----索引2数值为0则刷怪
		if Activated2 == 0 then
		map.SetMapScriptData(MapID,InstanceID,1,2,1)
		map.MapCreateCreature(MapID, InstanceID, 1004437, 627, 3704, 303)
		map.MapCreateCreature(MapID, InstanceID, 1004437, 617, 3742, 322)
		end
--进入触发器 1169
--怪A X=481，Y=7234，Z=338
--怪B X=492，Y=7296，Z=346
--怪C X=483，Y=7296，Z=363
--怪D X=474，Y=7254，Z=358
--刷出自爆狂猿（ID：1004437）
	elseif ObjID == 1169 then
		local Activated3 = map.GetMapScriptData(MapID,InstanceID,1,3) -----索引3数值为0则刷怪
		if Activated3 == 0 then
		map.SetMapScriptData(MapID,InstanceID,1,3,1)
		map.MapCreateCreature(MapID, InstanceID, 1004437, 481, 7234, 338)
		map.MapCreateCreature(MapID, InstanceID, 1004437, 492, 7296, 346)
		map.MapCreateCreature(MapID, InstanceID, 1004437, 483, 7296, 363)
		map.MapCreateCreature(MapID, InstanceID, 1004437, 474, 7254, 358)
		end
	end
end

aux.RegisterMapEvent("m15",6,"m15_OnEnterTrigger")

--进入副本后，部分技能无法使用
--m15_SkillLimit = {}
--m15_SkillLimit[11005] = true
--m15_SkillLimit[11009] = true
--m15_SkillLimit[11012] = true
--m15_SkillLimit[11014] = true
--m15_SkillLimit[90012] = true

--function m15_CanUseSkill(MapID, InstanceID, SkillTypeID)
--    if m15_SkillLimit[SkillTypeID] then
--        return false
--    end
--    return true
--end

--aux.RegisterMapEvent("m15", 21, "m15_CanUseSkill")

--进入副本时删除玩家的坐骑buff
function m15_OnPlayerEnter(MapID, InstanceID,RoleID)
	local bhave = role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 99996)
	if bhave == true then
	 unit.CancelBuff(MapID, InstanceID, RoleID, 9999601)
	 end
end

--注册
aux.RegisterMapEvent("m15", 2, "m15_OnPlayerEnter")
