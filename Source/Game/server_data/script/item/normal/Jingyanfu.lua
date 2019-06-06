--包含修炼场的几个脚本：S_fenghuo,jingyanfu,xinshouxiulian

function ShuangBeiFu_canUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local GM_Exp_Eff = aux.GetGMExpRate()
	if(GM_Exp_Eff >= 2) then
		--提示玩家背包空间不足
		bRet = 32
		return bRet, bIgnore
	elseif TypeID == 3301005 then
	    if MapID == 2291722097 or MapID == 2291721329 or MapID == 2291721585 or MapID == 2291720817 or MapID == 2291721073 or MapID == 2291720305 or MapID == 2291720561 or MapID == 2291723889 then
	        return bRet, bIgnore
	    else
	        bRet = 43
			return bRet, bIgnore
		end
	end
end

aux.RegisterItemEvent(1340001, 0, "ShuangBeiFu_canUse")
aux.RegisterItemEvent(1340002, 0, "ShuangBeiFu_canUse")
aux.RegisterItemEvent(3301001, 0, "ShuangBeiFu_canUse")
aux.RegisterItemEvent(3301002, 0, "ShuangBeiFu_canUse")
aux.RegisterItemEvent(3301003, 0, "ShuangBeiFu_canUse")
aux.RegisterItemEvent(3301004, 0, "ShuangBeiFu_canUse")
aux.RegisterItemEvent(3301005, 0, "ShuangBeiFu_canUse")

function SanBeiFu_canUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local GM_Exp_Eff = aux.GetGMExpRate()
	if(GM_Exp_Eff >= 3) then
		--提示玩家背包空间不足
		bRet = 32
	elseif MapID == 2291722097 or MapID == 2291721329 or MapID == 2291721585 or MapID == 2291720817 or MapID == 2291721073 or MapID == 2291720305 or MapID == 2291720561 or MapID == 2291723889 then
	    return bRet, bIgnore
	else
	    bRet = 43
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(3200462, 0, "SanBeiFu_canUse")

