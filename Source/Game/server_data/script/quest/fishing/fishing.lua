--共修改2处
--修改1：行 119	原数组定义只到110，后面判断120就超过最大成员了
--修改2：行 413

-- 钓鱼经验
fishing_exp = {}

fishing_exp[1] = 0
fishing_exp[2] = 0
fishing_exp[3] = 0
fishing_exp[4] = 0
fishing_exp[5] = 0
fishing_exp[6] = 0
fishing_exp[7] = 0
fishing_exp[8] = 0
fishing_exp[9] = 0
fishing_exp[10] = 0
fishing_exp[11] = 0
fishing_exp[12] = 0
fishing_exp[13] = 0
fishing_exp[14] = 0
fishing_exp[15] = 0
fishing_exp[16] = 0
fishing_exp[17] = 0
fishing_exp[18] = 0
fishing_exp[19] = 0

fishing_exp[20] = 65
fishing_exp[21] = 68
fishing_exp[22] = 72
fishing_exp[23] = 74
fishing_exp[24] = 79
fishing_exp[25] = 81
fishing_exp[26] = 86
fishing_exp[27] = 88
fishing_exp[28] = 92
fishing_exp[29] = 95
fishing_exp[30] = 99
fishing_exp[31] = 101
fishing_exp[32] = 106
fishing_exp[33] = 108
fishing_exp[34] = 113
fishing_exp[35] = 115
fishing_exp[36] = 119
fishing_exp[37] = 122
fishing_exp[38] = 126
fishing_exp[39] = 128
fishing_exp[40] = 133
fishing_exp[41] = 135
fishing_exp[42] = 142
fishing_exp[43] = 146
fishing_exp[44] = 153
fishing_exp[45] = 158
fishing_exp[46] = 164
fishing_exp[47] = 169
fishing_exp[48] = 176
fishing_exp[49] = 180
fishing_exp[50] = 187
fishing_exp[51] = 191
fishing_exp[52] = 198
fishing_exp[53] = 203
fishing_exp[54] = 209
fishing_exp[55] = 214
fishing_exp[56] = 221
fishing_exp[57] = 225
fishing_exp[58] = 232
fishing_exp[59] = 236
fishing_exp[60] = 245
fishing_exp[61] = 254
fishing_exp[62] = 263
fishing_exp[63] = 272
fishing_exp[64] = 281
fishing_exp[65] = 290
fishing_exp[66] = 299
fishing_exp[67] = 308
fishing_exp[68] = 317
fishing_exp[69] = 326
fishing_exp[70] = 335
fishing_exp[71] = 344
fishing_exp[72] = 353
fishing_exp[73] = 362
fishing_exp[74] = 371
fishing_exp[75] = 380
fishing_exp[76] = 389
fishing_exp[77] = 398
fishing_exp[78] = 407
fishing_exp[79] = 416
fishing_exp[80] = 430
fishing_exp[81] = 443
fishing_exp[82] = 457
fishing_exp[83] = 470
fishing_exp[84] = 484
fishing_exp[85] = 497
fishing_exp[86] = 506
fishing_exp[87] = 515
fishing_exp[88] = 524
fishing_exp[89] = 533
fishing_exp[90] = 542
fishing_exp[91] = 551
fishing_exp[92] = 560
fishing_exp[93] = 569
fishing_exp[94] = 578
fishing_exp[95] = 587
fishing_exp[96] = 596
fishing_exp[97] = 605
fishing_exp[98] = 614
fishing_exp[99] = 623
fishing_exp[100] = 632
fishing_exp[101] = 642
fishing_exp[102] = 652
fishing_exp[103] = 662
fishing_exp[104] = 672
fishing_exp[105] = 682
fishing_exp[106] = 692
fishing_exp[107] = 702
fishing_exp[108] = 712
fishing_exp[109] = 722
fishing_exp[110] = 732
--修改1
fishing_exp[111] = 742
fishing_exp[112] = 752
fishing_exp[113] = 762
fishing_exp[114] = 772
fishing_exp[115] = 782
fishing_exp[116] = 792
fishing_exp[117] = 802
fishing_exp[118] = 812
fishing_exp[119] = 822
fishing_exp[120] = 832

-- 鱼饵是否能使用
function IFishing_CanUse(MapID, InstanceID, TypeID, TargetID)
	if MapID == 2983744207 then
		local bCanUse = map.IsInArea(MapID, InstanceID, TargetID, 2235)
		if bCanUse == true then
			return 0, false
		else
			return 43, false	-- 不可在该位置使用物品
		end
	end
	return 43, false
end

-- 普通鱼饵使用
function I3200709_OnUse(MapID, InstanceID, TypeID, TargetID)
	local rolelevel = role.GetRoleLevel(MapID, InstanceID, TargetID)
	local roleexp = fishing_exp[rolelevel]
	local curhour = os.date("%H")
	if curhour == "18" or curhour == "19" or curhour == "20" or curhour == "21" then
		roleexp = roleexp*0.8
	end

	--升级经验为 等级*50
	local a = role.GetRoleScriptData(TargetID, 1, RoleDataType["FishingLevel"])
	local b = role.GetRoleScriptData(TargetID, 1, RoleDataType["FishingNum"])

	if a == 0 then
		a = 1
	end
	if a > 100 then
		a = 100
	end
	local i = a * 0.002
	role.AddRoleExp(MapID, InstanceID, TargetID, roleexp * (i+1))	-- 增加经验

	local r = math.random(1,100)

	if r > 98 then

		if role.GetRoleItemNum(TargetID, 3304145) > 0 then
			role.AddRoleItem(MapID, InstanceID, TargetID, 3200712, 1, -1, 8, 420)
			role.AddRoleItem(MapID, InstanceID, TargetID, 3304150, 1, -1, 8, 420)
		elseif role.GetRoleItemNum(TargetID, 3200715) > 0 or role.GetRoleItemNum(TargetID, 3200716) > 0 or role.GetRoleItemNum(TargetID, 3200717) > 0 then
			role.AddRoleItem(MapID, InstanceID, TargetID, 3200712, 1, -1, 8, 420)
		else
			role.AddRoleItem(MapID, InstanceID, TargetID, 3200714, 1, -1, 8, 420)
		end

	end

	if b >= a*50 - 1 then
		b = 0 		--升级后点数重置
		a = a + 1 	--熟练度升级+1
		if a > 100 then
		a = 100
		end
	else
		b = b + 1
	end

	role.SetRoleScriptData(TargetID, 1, RoleDataType["FishingLevel"],a)
	role.SetRoleScriptData(TargetID, 1, RoleDataType["FishingNum"],b)

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 286) --您的钓鱼熟练度等级为<p1>,还差<p2>点数升级
		msg.AddMsgEvent(MsgID, 9, a)
		msg.AddMsgEvent(MsgID, 9, a*50-b)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)

	role.FishingSendClient(MapID, InstanceID, TargetID) --告诉客户端重新使用鱼饵
end

-- 高级鱼饵使用
function I3200710_OnUse(MapID, InstanceID, TypeID, TargetID)
	local rolelevel = role.GetRoleLevel(MapID, InstanceID, TargetID)
	local roleexp = fishing_exp[rolelevel]*1.5
	local curhour = os.date("%H")

	if curhour == "18" or curhour == "19" or curhour == "20" or curhour == "21" then
	roleexp = roleexp * 0.8
	end

	--升级经验为 等级*50
	local a = role.GetRoleScriptData(TargetID, 1, RoleDataType["FishingLevel"])
	local b = role.GetRoleScriptData(TargetID, 1, RoleDataType["FishingNum"])

	if a == 0 then
		a = 1
	end
	if a > 100 then
		a = 100
	end
	local i = a * 0.002

	role.AddRoleExp(MapID, InstanceID, TargetID, roleexp * (i+1))	-- 增加经验

	local r = math.random(1,10000)

	if r >= (9960 - a*0.4) then --高级龙鱼
		if role.GetRoleItemNum(TargetID, 3304145) > 0 then
			role.AddRoleItem(MapID, InstanceID, TargetID, 3200713, 1, -1, 8, 420)
			role.AddRoleItem(MapID, InstanceID, TargetID, 3304150, 1, -1, 8, 420)
		elseif role.GetRoleItemNum(TargetID, 3200715) > 0 or role.GetRoleItemNum(TargetID, 3200716) > 0 or role.GetRoleItemNum(TargetID, 3200717) > 0 then
			role.AddRoleItem(MapID, InstanceID, TargetID, 3200713, 1, -1, 8, 420)
		else
			role.AddRoleItem(MapID, InstanceID, TargetID, 3200714, 1, -1, 8, 420)
		end
	elseif r <= (200 + a*2) then --普通龙鱼
		if role.GetRoleItemNum(TargetID, 3304145) > 0 then
			role.AddRoleItem(MapID, InstanceID, TargetID, 3200712, 1, -1, 8, 420)
			role.AddRoleItem(MapID, InstanceID, TargetID, 3304150, 1, -1, 8, 420)
		elseif role.GetRoleItemNum(TargetID, 3200715) > 0 or role.GetRoleItemNum(TargetID, 3200716) > 0 or role.GetRoleItemNum(TargetID, 3200717) > 0 then
			role.AddRoleItem(MapID, InstanceID, TargetID, 3200712, 1, -1, 8, 420)
		else
			role.AddRoleItem(MapID, InstanceID, TargetID, 3200714, 1, -1, 8, 420)
		end
	end

	if b >= a*50 - 2 then
		b = 0 		--升级后点数重置
		a = a + 1 	--熟练度升级+1
		if a > 100 then
		a = 100
		end
	else
		b = b + 2
	end
	role.SetRoleScriptData(TargetID, 1, RoleDataType["FishingLevel"],a)
	role.SetRoleScriptData(TargetID, 1, RoleDataType["FishingNum"],b)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 286) --您的钓鱼熟练度等级为<p1>,还差<p2>点数升级
		msg.AddMsgEvent(MsgID, 9, a)
		msg.AddMsgEvent(MsgID, 9, a*50-b)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	role.FishingSendClient(MapID, InstanceID, TargetID)
end

-- 特级鱼饵使用
function I3200711_OnUse(MapID, InstanceID, TypeID, TargetID)
	local rolelevel = role.GetRoleLevel(MapID, InstanceID, TargetID)
	local roleexp = fishing_exp[rolelevel]*2.25
	local curhour = os.date("%H")

	if curhour == "18" or curhour == "19" or curhour == "20" or curhour == "21" then
	roleexp = roleexp * 0.8
	end

	--升级经验为 等级*50
	local a = role.GetRoleScriptData(TargetID, 1, RoleDataType["FishingLevel"])
	local b = role.GetRoleScriptData(TargetID, 1, RoleDataType["FishingNum"])

	if a == 0 then
		a = 1
	end
	if a > 100 then
		a = 100
	end
	local i = a * 0.002

	role.AddRoleExp(MapID, InstanceID, TargetID, roleexp * (i+1))	-- 增加经验

	local r = math.random(1,10000)

	if r >= (9960 - a*0.4) then --高级龙鱼
		if role.GetRoleItemNum(TargetID, 3304145) > 0 then
			role.AddRoleItem(MapID, InstanceID, TargetID, 3200713, 1, -1, 8, 420)
			role.AddRoleItem(MapID, InstanceID, TargetID, 3304150, 1, -1, 8, 420)
		elseif role.GetRoleItemNum(TargetID, 3200715) > 0 or role.GetRoleItemNum(TargetID, 3200716) > 0 or role.GetRoleItemNum(TargetID, 3200717) > 0 then
			role.AddRoleItem(MapID, InstanceID, TargetID, 3200713, 1, -1, 8, 420)
		else
			role.AddRoleItem(MapID, InstanceID, TargetID, 3200714, 1, -1, 8, 420)
		end
	elseif r <= (200 + a*2) then --普通龙鱼
		if role.GetRoleItemNum(TargetID, 3304145) > 0 then
			role.AddRoleItem(MapID, InstanceID, TargetID, 3200712, 1, -1, 8, 420)
			role.AddRoleItem(MapID, InstanceID, TargetID, 3304150, 1, -1, 8, 420)
		elseif role.GetRoleItemNum(TargetID, 3200715) > 0 or role.GetRoleItemNum(TargetID, 3200716) > 0 or role.GetRoleItemNum(TargetID, 3200717) > 0 then
			role.AddRoleItem(MapID, InstanceID, TargetID, 3200712, 1, -1, 8, 420)
		else
			role.AddRoleItem(MapID, InstanceID, TargetID, 3200714, 1, -1, 8, 420)
		end
	end

	if b >= a*50 - 3 then
		b = 0 		--升级后点数重置
		a = a + 1 	--熟练度升级+1
		if a > 100 then
		a = 100
		end
	else
		b = b + 3
	end
	role.SetRoleScriptData(TargetID, 1, RoleDataType["FishingLevel"],a)
	role.SetRoleScriptData(TargetID, 1, RoleDataType["FishingNum"],b)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 286) --您的钓鱼熟练度等级为<p1>,还差<p2>点数升级
		msg.AddMsgEvent(MsgID, 9, a)
		msg.AddMsgEvent(MsgID, 9, a*50-b)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	role.FishingSendClient(MapID, InstanceID, TargetID)
end

-- 3200718寅虎吉祥鱼饵使用
function I3200718_OnUse(MapID, InstanceID, TypeID, TargetID)
	local rolelevel = role.GetRoleLevel(MapID, InstanceID, TargetID)
	local roleexp = fishing_exp[rolelevel]*1.5
	local curhour = os.date("%H")

	if curhour == "18" or curhour == "19" or curhour == "20" or curhour == "21" then
	roleexp = roleexp * 0.8
	end

	role.AddRoleExp(MapID, InstanceID, TargetID, roleexp)	-- 增加经验

--[[时限制鱼饵不能获得物品，不受熟练度加成
	--升级经验为 等级*50
	local a = role.GetRoleScriptData(TargetID, 1, RoleDataType["FishingLevel"])
	local b = role.GetRoleScriptData(TargetID, 1, RoleDataType["FishingNum"])

	if a == 0 then
		a = 1
	end

	local i = a * 0.002


	local r = math.random(1,10000)

	if r >= (9960 - a*0.4) then --高级龙鱼
		if role.GetRoleItemNum(TargetID, 3200715) > 0 or role.GetRoleItemNum(TargetID, 3200716) > 0 or role.GetRoleItemNum(TargetID, 3200717) > 0 then
			role.AddRoleItem(MapID, InstanceID, TargetID, 3200713, 1, -1, 8, 420)
		else
			role.AddRoleItem(MapID, InstanceID, TargetID, 3200714, 1, -1, 8, 420)
		end
	elseif r <= (200 + a*2) then --普通龙鱼
		if role.GetRoleItemNum(TargetID, 3200715) > 0 or role.GetRoleItemNum(TargetID, 3200716) > 0 or role.GetRoleItemNum(TargetID, 3200717) > 0 then
			role.AddRoleItem(MapID, InstanceID, TargetID, 3200712, 1, -1, 8, 420)
		else
			role.AddRoleItem(MapID, InstanceID, TargetID, 3200714, 1, -1, 8, 420)
		end
	end

	if b >= a*50 - 2 then
		b = 0 		--升级后点数重置
		a = a + 1 	--熟练度升级+1
		if a > 100 then
		a = 100
		end
	else
		b = b + 2
	end
	role.SetRoleScriptData(TargetID, 1, RoleDataType["FishingLevel"],a)
	role.SetRoleScriptData(TargetID, 1, RoleDataType["FishingNum"],b)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 286) --您的钓鱼熟练度等级为<p1>,还差<p2>点数升级
		msg.AddMsgEvent(MsgID, 9, a)
		msg.AddMsgEvent(MsgID, 9, a*50-b)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
]]
	role.FishingSendClient(MapID, InstanceID, TargetID)
end

-- 获取玩家单次钓鱼经验
function RGetRoleFishingExp(MapID, InstanceID, RoleID)
	local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)

	--升级经验为 等级*50
	local a = role.GetRoleScriptData(RoleID, 1, RoleDataType["FishingLevel"])
	local b = role.GetRoleScriptData(RoleID, 1, RoleDataType["FishingNum"])

	if a == 0 then
		a = 1
	end

	local i = a * 0.002
	local curhour = os.date("%H")
	local Now_exp

	if curhour == "18" or curhour == "19" or curhour == "20" or curhour == "21" then
		Now_exp = fishing_exp[rolelevel]* 0.8 *(i+1)
	
	--修改2
	else
		if rolelevel > 120 then
			Now_exp = fishing_exp[120]*(i+2)
		else
			Now_exp = fishing_exp[rolelevel]*(i+1)
		end
	end
	return Now_exp
end

aux.RegisterItemEvent(3200709, 0, "IFishing_CanUse")
aux.RegisterItemEvent(3200710, 0, "IFishing_CanUse")
aux.RegisterItemEvent(3200709, 1, "I3200709_OnUse")
aux.RegisterItemEvent(3200710, 1, "I3200710_OnUse")
aux.RegisterItemEvent(3200711, 0, "IFishing_CanUse")
aux.RegisterItemEvent(3200711, 1, "I3200711_OnUse")

aux.RegisterItemEvent(3200718, 0, "IFishing_CanUse")
aux.RegisterItemEvent(3200718, 1, "I3200718_OnUse")

aux.RegisterRoleEvent(21, "RGetRoleFishingExp")
