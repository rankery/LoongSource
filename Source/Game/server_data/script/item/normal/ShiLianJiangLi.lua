--竞技场试炼物品
--使用效果
function ShiLian_Arena_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	if TypeID == 3110201 then
	    role.ModRoleAttValue(MapID, InstanceID, TargetID, 80, 240)
	elseif TypeID == 3110202 then
	    role.ModRoleAttValue(MapID, InstanceID, TargetID, 80, 570)
	elseif TypeID == 3110203 then
	    role.ModRoleAttValue(MapID, InstanceID, TargetID, 80, 1280)
    end
end

--注册
aux.RegisterItemEvent(3110201, 1, "ShiLian_Arena_QuestUsable")
aux.RegisterItemEvent(3110202, 1, "ShiLian_Arena_QuestUsable")
aux.RegisterItemEvent(3110203, 1, "ShiLian_Arena_QuestUsable")


--帮派贡献试炼物品
function ShiLian_Guild_OnUse(MapID, InstanceID, TypeID, TargetID)
    local Guild = guild.GetRoleGuildID(TargetID)
	if Guild ~= nil and Guild ~= -1 and Guild ~= 4294967295 then
		if TypeID == 3110101 then
			guild.ModifyRoleContribute(Guild, TargetID, 36, 104)
		elseif TypeID == 3110102 then
			guild.ModifyRoleContribute(Guild, TargetID, 70, 104)
		elseif TypeID == 3110103 then
			guild.ModifyRoleContribute(Guild, TargetID, 120, 104)
		elseif TypeID == 3110104 then
			guild.ModifyRoleContribute(Guild, TargetID, 200, 104)
		elseif TypeID == 3110105 then
			guild.ModifyRoleContribute(Guild, TargetID, 320, 104)
		end
	end
end

function ShiLian_Guild_CanUse(MapID, InstanceID, TypeID, TargetID)
	local Guild = guild.GetRoleGuildID(TargetID)
	if Guild == nil or Guild == -1 or Guild == 4294967295 then
		return 32, false
	else
		return 0, false
	end

end

aux.RegisterItemEvent(3110101, 1, "ShiLian_Guild_OnUse")
aux.RegisterItemEvent(3110101, 0, "ShiLian_Guild_CanUse")
aux.RegisterItemEvent(3110102, 1, "ShiLian_Guild_OnUse")
aux.RegisterItemEvent(3110102, 0, "ShiLian_Guild_CanUse")
aux.RegisterItemEvent(3110103, 1, "ShiLian_Guild_OnUse")
aux.RegisterItemEvent(3110103, 0, "ShiLian_Guild_CanUse")
aux.RegisterItemEvent(3110104, 1, "ShiLian_Guild_OnUse")
aux.RegisterItemEvent(3110104, 0, "ShiLian_Guild_CanUse")
aux.RegisterItemEvent(3110105, 1, "ShiLian_Guild_OnUse")
aux.RegisterItemEvent(3110105, 0, "ShiLian_Guild_CanUse")
