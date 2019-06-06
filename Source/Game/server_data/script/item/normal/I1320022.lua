MianZui = {}
MianZui[1] = 2022301
MianZui[2] = 2022401
MianZui[3] = 2022501
MianZui[4] = 2022601
MianZui[5] = 2022701
MianZui[6] = 2022801
MianZui[7] = 2022901
MianZui[8] = 2023001
MianZui[9] = 2023101

-- 免罪符
function I1320022_OnUse(MapID, InstanceID, TypeID, TargetID)
	local hostility = role.GetRoleAttValue(MapID, InstanceID, TargetID, 76)
	local cansub = 3			-- 一次减3点戾气值
	if hostility < 3 then
		cansub = hostility
	end
	-- 先去除buff
	unit.CancelBuff(MapID, InstanceID, TargetID, MianZui[hostility])
	role.ModRoleAttValue(MapID, InstanceID, TargetID, 76, 0 - cansub)

	-- 添加buff
	hostility = hostility - cansub
	if hostility > 0 then
		unit.AddBuff(MapID, InstanceID, TargetID, MianZui[hostility], TargetID)
	end
end


aux.RegisterItemEvent(1320022, 1, "I1320022_OnUse")