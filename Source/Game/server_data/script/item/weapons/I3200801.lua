--[[合成90级剑客职业装备

--可否使用函数
function i3200801_CanUse(MapID, InstanceID, TypeID, TargetID)
	local shengchanlingzhuang={}
	shengchanlingzhuang[1] = 8086090
	shengchanlingzhuang[2] = 8096090
	shengchanlingzhuang[3] = 8106090
	shengchanlingzhuang[4] = 8116090
	shengchanlingzhuang[5] = 8126090
	shengchanlingzhuang[6] = 8146080
	shengchanlingzhuang[7] = 8156090
	shengchanlingzhuang[8] = 8166085
	local k = role.GetRoleItemNum(TargetID, 3200809)
	local bR = 32
	local bI = false
	if k > 9 then
		for i=1, 8 do
			local n = shengchanlingzhuang[i]
			local nRet = role.GetRoleItemNum(TargetID, n)
			if nRet >= 1 then
				bR = 0
			break
			end
		end
	end
	return bR,bI
end

--使用效果

function i3200801_QuestUsable(MapID, InstanceID, TypeID, TargetID)

	local shengchanlingzhuang={}
	shengchanlingzhuang[1] = {OriginalID = 8086090,NewID = 8220009}
	shengchanlingzhuang[2] = {OriginalID = 8096090,NewID = 8220025}
	shengchanlingzhuang[3] = {OriginalID = 8106090,NewID = 8220041}
	shengchanlingzhuang[4] = {OriginalID = 8116090,NewID = 8220057}
	shengchanlingzhuang[5] = {OriginalID = 8126090,NewID = 8220073}
	shengchanlingzhuang[6] = {OriginalID = 8146080,NewID = 8220089}
	shengchanlingzhuang[7] = {OriginalID = 8156090,NewID = 8220097}
	shengchanlingzhuang[8] = {OriginalID = 8166085,NewID = 8220113}

	for i=1, 8 do
		local a = role.GetRoleItemNum(TargetID, shengchanlingzhuang[i].OriginalID)
		if  a > 0 then
		    role.RemoveFromRole(MapID, InstanceID, TargetID, shengchanlingzhuang[i].OriginalID, 1, 420)
		    role.RemoveFromRole(MapID, InstanceID, TargetID, 3200809, 10, 420)
		    role.AddRoleItem(MapID, InstanceID, TargetID, shengchanlingzhuang[i].NewID, 1, -1, 8, 420)
			break
		end
	end
end

--注册
aux.RegisterItemEvent(3200801, 1, "i3200801_QuestUsable")
aux.RegisterItemEvent(3200801, 0, "i3200801_CanUse")]]
