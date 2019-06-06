--[[疯狂的雪人：任务完成后根据玩家等级奖励经验
function x20146_OnComplete(mapID, instanceID, questID, ownerID, enderID)
	local RoleLvl = role.GetRoleLevel(mapID, instanceID, ownerID)
	RoleLvl = math.floor(RoleLvl/10)

	if RoleLvl > 11 then
	    RoleLvl = 11
	end

	local exp_award = {}
	exp_award[1] = 3588
	exp_award[2] = 5888
	exp_award[3] = 8694
	exp_award[4] = 11730
	exp_award[5] = 16330
	exp_award[6] = 21528
	exp_award[7] = 29348
	exp_award[8] = 37536
	exp_award[9] = 47564
	exp_award[10] = 51336
	exp_award[11] = 58722

	role.AddRoleExp(mapID, instanceID, ownerID, exp_award[RoleLvl])
end

aux.RegisterQuestEvent(20146, 1, "x20146_OnComplete")

--随机奖励：任务完成后奖励随机BUFF
function x20147_OnComplete(mapID, instanceID, questID, ownerID, enderID)
	local rate = math.random(100)

	if rate<=2 then
		--加掉率buff
		unit.AddBuff(mapID, instanceID, ownerID, 2020401, ownerID)
	elseif rate<=17 then
		--加体型buff
		unit.AddBuff(mapID, instanceID, ownerID, 2020501, ownerID)
	elseif rate<=40 then
		--加移动速度buff
		unit.AddBuff(mapID, instanceID, ownerID, 2020601, ownerID)
	elseif rate<=70 then
		--加攻击buff
		unit.AddBuff(mapID, instanceID, ownerID, 2020701, ownerID)
	elseif rate<=100 then
		--加经验buff
		unit.AddBuff(mapID, instanceID, ownerID, 2020801, ownerID)
	end

end

aux.RegisterQuestEvent(20147, 1, "x20147_OnComplete")
]]