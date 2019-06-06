--ScriptData已用索引1,2
--ScriptData 索引3-9 用于祭魔台

--副本Data已用索引10,11(小怪，BOSS)
--12用于领取奖励的人数

--进入副本玩家获得一个buff，增加所受伤害

function m42_OnPlayerEnter(MapID, InstanceID,RoleID)
	 unit.AddBuff(MapID, InstanceID,RoleID, 2017301,RoleID)

	--进入副本时删除玩家的坐骑buff
	local bhave = role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 99996)
	if bhave == true then
	 unit.CancelBuff(MapID, InstanceID, RoleID, 9999601)
	 end
end

--注册
aux.RegisterMapEvent("m42", 2, "m42_OnPlayerEnter")



--需求11 进入地图区域 2160，每5秒收到一次焚烧伤害 BUFF2017401，减血30%
function m42_OnEnterArea(MapID, InstanceID, RoleID, ObjID)
	if ObjID == 2160 then
		local lTime = map.GetMapScriptData(MapID,InstanceID,1,2) --副本Data索引2
		if lTime == nil then
			lTime = 0
		end
		local cTime = os.time() --当前时间
		local dTime = cTime - lTime --上次触发距现在时间
		if dTime > 5 then --判断触发间隔是否大于5秒钟
			unit.AddBuff(MapID, InstanceID, RoleID, 2017401, RoleID)
			map.SetMapScriptData(MapID,InstanceID,1,2,cTime)
		end
	end
end

aux.RegisterMapEvent("m42", 7, "m42_OnEnterArea")





--调查西极仙石 ID：3040527 BUFF添加 ID：2017302

function c3040527_OnInvest(MapID, InstanceID, CreatureID, CreatureTypeID, RoleID)

	unit.AddBuff(MapID, InstanceID, RoleID, 2017302, RoleID)

end
aux.RegisterCreatureEvent(3040527, 6, "c3040527_OnInvest")




--调查东极仙石 ID：3040528 BUFF添加 ID：2017302

function c3040528_OnInvest(MapID, InstanceID, CreatureID, CreatureTypeID, RoleID)

	unit.AddBuff(MapID, InstanceID, RoleID, 2017302, RoleID)

end
aux.RegisterCreatureEvent(3040528, 6, "c3040528_OnInvest")




--调查南极仙石 ID：3040529  BUFF添加 ID：2017302

function c3040529_OnInvest(MapID, InstanceID, CreatureID, CreatureTypeID, RoleID)

	unit.AddBuff(MapID, InstanceID, RoleID, 2017302, RoleID)

end
aux.RegisterCreatureEvent(3040529, 6, "c3040529_OnInvest")



--调查北极仙石 ID：3040530 BUFF添加 ID：2017302

function c3040530_OnInvest(MapID, InstanceID, CreatureID, CreatureTypeID, RoleID)

	unit.AddBuff(MapID, InstanceID, RoleID, 2017302, RoleID)

end
aux.RegisterCreatureEvent(3040530, 6, "c3040530_OnInvest")



--进入副本后，部分技能无法使用
--m42_SkillLimit = {}
--m42_SkillLimit[11005] = true
--m42_SkillLimit[11009] = true
--m42_SkillLimit[11012] = true
--m42_SkillLimit[11014] = true
--m42_SkillLimit[90012] = true

--function m42_CanUseSkill(MapID, InstanceID, SkillTypeID)
--    if m42_SkillLimit[SkillTypeID] then
--        return false
--    end
--    return true
--end

--aux.RegisterMapEvent("m42", 21, "m42_CanUseSkill")


