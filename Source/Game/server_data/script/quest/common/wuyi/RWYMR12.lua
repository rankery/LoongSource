
--RWYMR12：毒池戾气 14312
--使用[银签][2614037]后，删除一个[未鉴别的毒液][2614038]，有40%几率获得[红色毒液][2614039]，40%几率获得[绿色毒液][2614040]，如果有任务，20%几率获得[烈性毒液][2614041]
function I2614037_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	--删除一个[未鉴别的毒液][2614038]
	role.RemoveFromRole(MapID, InstanceID, TargetID, 2614038, 1, 101)

	local Rate = math.random(1, 100)
	if Rate < 51 then
	--20%几率获得[烈性毒液][2614041]
	role.AddRoleItem(MapID, InstanceID, TargetID, 2614041, 1, -1, 8,420)

	--40%几率获得[绿色毒液][2614040]
	elseif Rate >= 51 and Rate < 75 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 2614040, 1, -1, 8,420)

	--40%几率获得[红色毒液][2614039]
	elseif Rate >= 75 and Rate <= 100 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 2614039, 1, -1, 8,420)

	end
end

--行囊内有一个[未鉴别的毒液][2614038]，银签才可使用
function I2614037_CanUse(MapID, InstanceID, TypeID, TargetID)
	local num = role.GetRoleItemNum(TargetID, 2614038)

	local bRet, bIgnore = 0, false

	if num < 1 then
	bRet = 32
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(2614037, 1, "I2614037_QuestUsable")
aux.RegisterItemEvent(2614037, 0, "I2614037_CanUse")

--使用[绿色毒液][2614040]，获得嗜血毒BUFF[2016101]
function I2614040_Use(MapID, InstanceID, TypeID, TargetID)
	unit.AddBuff(MapID, InstanceID, TargetID, 2016101, TargetID)
end
--使用[红色毒液][2614039]，获得狂暴毒BUFF[2016001]
function I2614039_Use(MapID, InstanceID, TypeID, TargetID)
	unit.AddBuff(MapID, InstanceID, TargetID, 2016001, TargetID)
end
aux.RegisterItemEvent(2614040,1, "I2614040_Use")
aux.RegisterItemEvent(2614039,1, "I2614039_Use")
