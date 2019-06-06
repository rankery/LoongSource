--技能——龙魂爆发和龙魂激增
--龙魂爆发技能ID——9001301
--龙魂激增技能ID——9001401

--龙魂爆发技能是否可以使用
function s9001301_CanCast(mapid, instanceid, skillid, ownerid, dummy)
    local num = role.GetLongHunID(ownerid)  --检测玩家身上有无龙魂ID
    if num == nil or num == 0 then 
        return 7
    else
        return 0
    end
end

--龙魂爆发技能效果
function s9001301_Cast(mapid, instanceid, skillid, ownerid)
    role.CastLongHun(ownerid, skillid)
    return 1
end

--龙魂激增是否可使用
function s9001401_CanCast(mapid, instanceid, skillid, ownerid, dummy)
	local bRet = role.ProlifeLongHun(ownerid, 90013, 0)
	if bRet == nil then
		return 7
	else
		return 0
	end
end

--龙魂激增效果
function s9001401_Cast(mapid, instanceid, skillid, ownerid)
	role.ProlifeLongHun(ownerid, 90013, 1)
	return 1
end

--注册
aux.RegisterSkillEvent(9001301, 0, "s9001301_CanCast")
aux.RegisterSkillEvent(9001301, 1, "s9001301_Cast")
aux.RegisterSkillEvent(9001401, 0, "s9001401_CanCast")
aux.RegisterSkillEvent(9001401, 1, "s9001401_Cast")