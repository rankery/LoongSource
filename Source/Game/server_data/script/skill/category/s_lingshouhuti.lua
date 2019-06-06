
--使用灵兽护体、高级护体技能 根据角色体力上限，添加不同恢复BUFF
--1400701	护体
--1400702	高级护体

function s1400701_Cast(mapid, instanceid, skillid, ownerid, roleid)

local k = role.GetRoleAttValue(mapid, instanceid, roleid, 8)
	if k >= 100000 then
	unit.AddBuff(mapid, instanceid,roleid,8002601,roleid)
	else
	unit.AddBuff(mapid, instanceid,roleid,8001301,roleid)
	end

end
--注册

aux.RegisterPetSkillEvent(1400701, 1, "s1400701_Cast")


function s1400702_Cast(mapid, instanceid, skillid, ownerid, roleid)

local k = role.GetRoleAttValue(mapid, instanceid, roleid, 8)
	if k >= 100000 then
	unit.AddBuff(mapid, instanceid,roleid,8002701,roleid)
	else
	unit.AddBuff(mapid, instanceid,roleid,8001401,roleid)
	end

end
--注册
--[[
1401101
1401401
1401701
1402001
--]]
aux.RegisterPetSkillEvent(1400702, 1, "s1400702_Cast")
aux.RegisterPetSkillEvent(1401101, 1, "s1400702_Cast")
aux.RegisterPetSkillEvent(1401401, 1, "s1400702_Cast")
aux.RegisterPetSkillEvent(1401701, 1, "s1400702_Cast")
aux.RegisterPetSkillEvent(1402001, 1, "s1400702_Cast")
aux.RegisterPetSkillEvent(1402301, 1, "s1400702_Cast")
aux.RegisterPetSkillEvent(1402601, 1, "s1400702_Cast")
aux.RegisterPetSkillEvent(1402901, 1, "s1400702_Cast")













