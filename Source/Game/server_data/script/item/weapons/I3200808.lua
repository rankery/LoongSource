--[[合成90级乐师职业装备

--可否使用函数
function i3200808_CanUse(MapID, InstanceID, TypeID, TargetID)
	local shengchanlingzhuang={}
	shengchanlingzhuang[1] = 8084090
	shengchanlingzhuang[2] = 8094090
	shengchanlingzhuang[3] = 8104090
	shengchanlingzhuang[4] = 8114090
	shengchanlingzhuang[5] = 8124090
	shengchanlingzhuang[6] = 8147080
	shengchanlingzhuang[7] = 8157090
	shengchanlingzhuang[8] = 8167085
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

function i3200808_QuestUsable(MapID, InstanceID, TypeID, TargetID)

	local shengchanlingzhuang={}
	shengchanlingzhuang[1] = {OriginalID = 8084090,NewID = 8220004}
	shengchanlingzhuang[2] = {OriginalID = 8094090,NewID = 8220020}
	shengchanlingzhuang[3] = {OriginalID = 8104090,NewID = 8220036}
	shengchanlingzhuang[4] = {OriginalID = 8114090,NewID = 8220052}
	shengchanlingzhuang[5] = {OriginalID = 8124090,NewID = 8220068}
	shengchanlingzhuang[6] = {OriginalID = 8147080,NewID = 8220084}
	shengchanlingzhuang[7] = {OriginalID = 8157090,NewID = 8220108}
	shengchanlingzhuang[8] = {OriginalID = 8167085,NewID = 8220124}

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
aux.RegisterItemEvent(3200808, 1, "i3200808_QuestUsable")
aux.RegisterItemEvent(3200808, 0, "i3200808_CanUse")]]
