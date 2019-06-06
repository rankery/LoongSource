--三八妇女节活动

--使用物品神秘花篮
function I3303314_OnUse(MapID, InstanceID, TypeID, RoleID)
    local i = math.random(100)
	local k = math.random(40)
	if i <= 10 then
	    role.AddRoleItem(MapID, InstanceID, RoleID, 3303008, 1, 1, 8, 420)
	elseif i <= 14 then
	    role.AddRoleItem(MapID, InstanceID, RoleID, 3303316, 1, 1, 8, 420)
	elseif i <= 54 then
	    role.AddRoleItem(MapID, InstanceID, RoleID, 3303309, 1, 1, 8, 420)
	elseif i <= 74 then
	    role.AddRoleItem(MapID, InstanceID, RoleID, 3303310, 1, 1, 8, 420)
	elseif i <= 84 then
	    role.AddRoleItem(MapID, InstanceID, RoleID, 3303311, 1, 1, 8, 420)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 13, 1)
		msg.AddMsgEvent(MsgID, 1, 2410)       --XXX开启了神秘花篮，获得了XXX！
        msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 4, 3303311)
		msg.DispatchWorldMsgEvent(MsgID)
	elseif i <= 94 then
	    role.AddRoleItem(MapID, InstanceID, RoleID, 3303313, 1, 1, 8, 420)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 13, 1)
		msg.AddMsgEvent(MsgID, 1, 2410)          --XXX开启了神秘花篮，获得了XXX！
        msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 4, 3303313)
		msg.DispatchWorldMsgEvent(MsgID)
	elseif i == 95 then
	    role.AddRoleItem(MapID, InstanceID, RoleID, 3303312, 1, 1, 8, 420)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 13, 1)
		msg.AddMsgEvent(MsgID, 1, 2410)             --XXX开启了神秘花篮，获得了XXX！
        msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 4, 3303312)
		msg.DispatchWorldMsgEvent(MsgID)
	elseif i == 96 then
        role.AddRoleItem(MapID, InstanceID, RoleID, 3303307, 1, 1, 8, 420)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 2412)             --<color=0xff05ff00>###（玩家名称）<color=0xfffff7e0>通过开启神秘花篮获得了<color=0xffffff00>【封号：怜香惜玉】<color=0xfffff7e0>
        msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.DispatchWorldMsgEvent(MsgID)
	elseif i <= 100 then
	    if k <= 39 then
		    role.AddRoleItem(MapID, InstanceID, RoleID, 3000011, 1, 1, 8, 420)
		elseif k ==40 then
		    role.AddRoleItem(MapID, InstanceID, RoleID, 3000012, 1, 1, 8, 420)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 13, 1)
			msg.AddMsgEvent(MsgID, 1, 2405)          --XXX开启了神秘花篮，获得了XXX！
			msg.AddMsgEvent(MsgID, 2, RoleID)
			msg.AddMsgEvent(MsgID, 4, 3000012)
			msg.DispatchWorldMsgEvent(MsgID)
	    end
	end
end

function I3303314_CanUse(MapID, InstanceID, TypeID, RoleID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(RoleID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
		return bRet, bIgnore
	end
end

aux.RegisterItemEvent(3303314, 1, "I3303314_OnUse")
aux.RegisterItemEvent(3303314, 0, "I3303314_CanUse")

--使用物品神秘礼盒
function I3303315_OnUse(MapID, InstanceID, TypeID, RoleID)
    local i = math.random(100)
	local k = math.random(100)
	if i <= 30 then
	    role.AddRoleSilver(MapID, InstanceID, RoleID, 2000, 102)
	elseif i <= 32 then
	    role.AddRoleSilver(MapID, InstanceID, RoleID, 20000, 102)
	elseif i <= 40 then
	    role.AddRoleItem(MapID, InstanceID, RoleID, 3303323, 1, 1, 8, 420)
	elseif i <= 45 then
	    role.AddRoleItem(MapID, InstanceID, RoleID, 3303308, 5, 1, 8, 420)
	elseif i <= 59 then
	    local j =math.random(7)
		if j == 1 then
	        role.AddRoleItem(MapID, InstanceID, RoleID, 1230010, 1, 1, 8, 420)
		elseif j == 2 then
		    role.AddRoleItem(MapID, InstanceID, RoleID, 1420008, 1, 1, 8, 420)
		elseif j == 3 then
		    role.AddRoleItem(MapID, InstanceID, RoleID, 1420012, 1, 1, 8, 420)
		elseif j == 4 then
		    role.AddRoleItem(MapID, InstanceID, RoleID, 1420016, 1, 1, 8, 420)
		elseif j == 5 then
		    role.AddRoleItem(MapID, InstanceID, RoleID, 1420020, 1, 1, 8, 420)
		elseif j == 6 then
		    role.AddRoleItem(MapID, InstanceID, RoleID, 1420032, 1, 1, 8, 420)
		elseif j == 7 then
		    role.AddRoleItem(MapID, InstanceID, RoleID, 1420036, 1, 1, 8, 420)
		end
	elseif i <= 74 then
	    role.AddRoleItem(MapID, InstanceID, RoleID, 4500002, 1, 1, 8, 420)
	elseif i == 79 then
	    role.AddRoleItem(MapID, InstanceID, RoleID, 4500004, 1, 1, 8, 420)
	elseif i == 94 then
        role.AddRoleItem(MapID, InstanceID, RoleID, 4700003, 1, 1, 8, 420)
	elseif i <= 99 then
	    role.AddRoleItem(MapID, InstanceID, RoleID, 4700005, 1, 1, 8, 420)
	elseif i == 100 then
	    if k <= 5 then
		    role.AddRoleItem(MapID, InstanceID, RoleID, 1361106, 1, 1, 8, 420)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 13, 1)
			msg.AddMsgEvent(MsgID, 1, 2404)          --XXX开启了神秘礼盒，获得了XXX！
			msg.AddMsgEvent(MsgID, 2, RoleID)
			msg.AddMsgEvent(MsgID, 4, 1361106)
			msg.DispatchWorldMsgEvent(MsgID)
		elseif k <= 25 then
		    role.AddRoleItem(MapID, InstanceID, RoleID, 8820022, 1, -1, 8, 420)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 2416)          --XXX开启了神秘礼盒，获得了XXX！
			msg.AddMsgEvent(MsgID, 2, RoleID)
			msg.AddMsgEvent(MsgID, 4, 8820022)
			msg.DispatchWorldMsgEvent(MsgID)
		elseif k <= 75 then
		    role.AddRoleItem(MapID, InstanceID, RoleID, 8820058, 1, -1, 8, 420)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 2416)          --XXX开启了神秘礼盒，获得了XXX！
			msg.AddMsgEvent(MsgID, 2, RoleID)
			msg.AddMsgEvent(MsgID, 4, 8820058)
			msg.DispatchWorldMsgEvent(MsgID)
		elseif k <= 95 then
		    role.AddRoleItem(MapID, InstanceID, RoleID, 8820059, 1, -1, 8, 420)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 2416)          --XXX开启了神秘礼盒，获得了XXX！
			msg.AddMsgEvent(MsgID, 2, RoleID)
			msg.AddMsgEvent(MsgID, 4, 8820059)
			msg.DispatchWorldMsgEvent(MsgID)
		elseif k <= 100 then
		    role.AddRoleItem(MapID, InstanceID, RoleID, 8820060, 1, -1, 8, 420)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 2416)          --XXX开启了神秘礼盒，获得了XXX！
			msg.AddMsgEvent(MsgID, 2, RoleID)
			msg.AddMsgEvent(MsgID, 4, 8820060)
			msg.DispatchWorldMsgEvent(MsgID)
	    end
	end
end

function I3303315_CanUse(MapID, InstanceID, TypeID, RoleID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(RoleID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
		return bRet, bIgnore
	end
end

aux.RegisterItemEvent(3303315, 1, "I3303315_OnUse")
aux.RegisterItemEvent(3303315, 0, "I3303315_CanUse")

--使用物品 封号：绝代佳人
function I3303301_OnUse(MapID, InstanceID, TypeID, RoleID)
    local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 2413)          --<color=0xff05ff00>###（玩家名称）<color=0xfffff7e0>通过妇女节活动获得了独一无二的超级称号<color=0xffffff00>“绝代佳人”<color=0xfffff7e0>
	msg.AddMsgEvent(MsgID, 2, RoleID)
	msg.DispatchWorldMsgEvent(MsgID)
end

aux.RegisterItemEvent(3303301, 1, "I3303301_OnUse")

--使用物品 封号：倾国倾城
function I3303302_OnUse(MapID, InstanceID, TypeID, RoleID)
    local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 2414)          --<color=0xff05ff00>###（玩家名称）<color=0xfffff7e0>通过妇女节活动获得了独一无二的超级称号<color=0xffffff00>“绝代佳人”<color=0xfffff7e0>
	msg.AddMsgEvent(MsgID, 2, RoleID)
	msg.DispatchWorldMsgEvent(MsgID)
end

aux.RegisterItemEvent(3303302, 1, "I3303302_OnUse")

--使用物品 封号：千娇百媚
function I3303303_OnUse(MapID, InstanceID, TypeID, RoleID)
    local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 2415)          --<color=0xff05ff00>###（玩家名称）<color=0xfffff7e0>通过妇女节活动获得了独一无二的超级称号<color=0xffffff00>“绝代佳人”<color=0xfffff7e0>
	msg.AddMsgEvent(MsgID, 2, RoleID)
	msg.DispatchWorldMsgEvent(MsgID)
end

aux.RegisterItemEvent(3303303, 1, "I3303303_OnUse")

--使用物品神秘礼盒
function sanbajieshizhuang_OnUse(MapID, InstanceID, TypeID, RoleID)
    for i = 1,5 do
		if TypeID == 3303320 then
		    role.AddRoleItem(MapID, InstanceID, RoleID, 9140250 + i, 1, 1, 8, 420)
		elseif TypeID == 3303321 then
		    role.AddRoleItem(MapID, InstanceID, RoleID, 9130230 + i, 1, 1, 8, 420)
		elseif TypeID == 3303322 then
		    role.AddRoleItem(MapID, InstanceID, RoleID, 9110230 + i, 1, 1, 8, 420)
		else
		    break
		end
	end
	for i = 1,6 do
		if TypeID == 3303317 then
		    role.AddRoleItem(MapID, InstanceID, RoleID, 9140260 + i, 1, 1, 8, 420)
		elseif TypeID == 3303318 then
		    role.AddRoleItem(MapID, InstanceID, RoleID, 9130240 + i, 1, 1, 8, 420)
		elseif TypeID == 3303319 then
		    role.AddRoleItem(MapID, InstanceID, RoleID, 9110240 + i, 1, 1, 8, 420)
		else
		    break
		end
	end
end

function sanbajieshizhuang_CanUse(MapID, InstanceID, TypeID, RoleID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(RoleID)
	if TypeID == 3303317 or TypeID == 3303318 or TypeID == 3303319 then
		if(FreeSize < 6) then
			--提示玩家背包空间不足
			bRet = 40
		end
		return bRet, bIgnore
	elseif TypeID == 3303320 or TypeID == 3303321 or TypeID == 3303322 then
	    if(FreeSize < 5) then
			--提示玩家背包空间不足
			bRet = 40
		end
		return bRet, bIgnore
	end
	return 32, bIgnore
end
function I6010837_CanUse(MapID, InstanceID, TypeID, RoleID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(RoleID)
	local a = role.GetRoleItemNum(RoleID,6010200)			--判断背包内是否有暗器
	local b = role.GetRoleItemNum(RoleID,6010300)
	if (a+b) == 0 then
		bRet, bIgnore = 32, false
	end
	if(FreeSize < 1) then
			--提示玩家背包空间不足
			bRet = 40
	end
	return bRet, bIgnore
end
function I6010837_OnUse(MapID, InstanceID, TypeID, RoleID)
	local a = role.GetRoleItemNum(RoleID,6010200)
	local b = role.GetRoleItemNum(RoleID,6010300)
	if a > b then
		role.RemoveFromRole(MapID,InstanceID,RoleID,6010200,1,420)
		role.AddRoleItem(MapID, InstanceID, RoleID,6010201, 1, 1, 8, 420)
	elseif a < b then
		role.RemoveFromRole(MapID,InstanceID,RoleID,6010300,1,420)
		role.AddRoleItem(MapID, InstanceID, RoleID,6010301, 1, 1, 8, 420)
	elseif a == b then
		local index = math.random(100)
		if index < 51 then
			role.RemoveFromRole(MapID,InstanceID,RoleID,6010200,1,420)
			role.AddRoleItem(MapID, InstanceID, RoleID,6010201, 1, 1, 8, 420)
		else
			role.RemoveFromRole(MapID,InstanceID,RoleID,6010300,1,420)
			role.AddRoleItem(MapID, InstanceID, RoleID,6010301, 1, 1, 8, 420)
		end
	end
end
aux.RegisterItemEvent(3303317, 1, "sanbajieshizhuang_OnUse")
aux.RegisterItemEvent(3303317, 0, "sanbajieshizhuang_CanUse")
aux.RegisterItemEvent(3303318, 1, "sanbajieshizhuang_OnUse")
aux.RegisterItemEvent(3303318, 0, "sanbajieshizhuang_CanUse")
aux.RegisterItemEvent(3303319, 1, "sanbajieshizhuang_OnUse")
aux.RegisterItemEvent(3303319, 0, "sanbajieshizhuang_CanUse")
aux.RegisterItemEvent(3303320, 1, "sanbajieshizhuang_OnUse")
aux.RegisterItemEvent(3303320, 0, "sanbajieshizhuang_CanUse")
aux.RegisterItemEvent(3303321, 1, "sanbajieshizhuang_OnUse")
aux.RegisterItemEvent(3303321, 0, "sanbajieshizhuang_CanUse")
aux.RegisterItemEvent(3303322, 1, "sanbajieshizhuang_OnUse")
aux.RegisterItemEvent(3303322, 0, "sanbajieshizhuang_CanUse")
aux.RegisterItemEvent(6010837, 1, "I6010837_OnUse")
aux.RegisterItemEvent(6010837, 0, "I6010837_CanUse")
