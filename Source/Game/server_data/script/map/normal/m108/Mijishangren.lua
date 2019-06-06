
--[[秘籍商人

1442201	秘籍·剑
1442202	秘籍·刀
1442203	秘籍·长兵
1442204	秘籍·弓
1442205	秘籍·阳刚
1442206	秘籍·阴柔
1442207	秘籍·医毒
1442208	秘籍·音律

]]--

--背包中是否有可以兑换的秘籍



function mijijiance(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	local Silver = 	role.GetRoleSilver(MapID, InstanceID, RoleID)
	local Item = role.GetRoleItemNum(RoleID,2310170)
	local MiJi_Request = false
	local chiyoumijishu = 0
	local	MiJi_ID = {}
			MiJi_ID[1] = 1442201
			MiJi_ID[2] = 1442202
			MiJi_ID[3] = 1442203
			MiJi_ID[4] = 1442204
			MiJi_ID[5] = 1442205
			MiJi_ID[6] = 1442206
			MiJi_ID[7] = 1442207
			MiJi_ID[8] = 1442208
	for k = 1,8 do
		local n = role.GetRoleItemNum(RoleID,MiJi_ID[k])
		chiyoumijishu = chiyoumijishu + n
	end
	if chiyoumijishu > 0 then
		if	Silver >= 10000000 then
			if	Item >= 100	then
				MiJi_Request = true
			else
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 4202)   --道具数量不足
				msg.AddMsgEvent(MsgID, 4, 2310170) --获得道具名称
				msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
			end
		else
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 4201)   --金钱不足
			msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		end
	else
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 4203)   --缺少秘籍
		msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
	end
	return MiJi_Request
end

function mijishangren_On_Talk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)

	local MijiID_1 = {1442202,1442203,1442204,1442205,1442206,1442207,1442208}
	local MijiID_2 = {1442201,1442203,1442204,1442205,1442206,1442207,1442208}
	local MijiID_3 = {1442201,1442202,1442204,1442205,1442206,1442207,1442208}
	local MijiID_4 = {1442201,1442202,1442203,1442205,1442206,1442207,1442208}
	local MijiID_5 = {1442201,1442202,1442203,1442204,1442206,1442207,1442208}
	local MijiID_6 = {1442201,1442202,1442203,1442204,1442205,1442207,1442208}
	local MijiID_7 = {1442201,1442202,1442203,1442204,1442205,1442206,1442208}
	local MijiID_8 = {1442201,1442202,1442203,1442204,1442205,1442206,1442207}
	local Silver = 	role.GetRoleSilver(MapID, InstanceID, RoleID)
	local Item = role.GetRoleItemNum(RoleID,2310170)

	if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 4204)	-- "当你拥有于自己转职天资不匹配的秘籍，可以在此消耗一定的道具和金钱换取你需要的秘籍。\n 兑换条件如下：任一其他秘籍 + 震珠界之粟*100 + 1000金"
		msg.AddMsgEvent(MsgID, 24, TargetID)
		msg.AddMsgEvent(MsgID, 21, 4)	-- 选项一
		msg.AddMsgEvent(MsgID, 1, 4205)	-- "兑换外功天资秘籍"
		msg.AddMsgEvent(MsgID, 21, 5)	-- 选项二
		msg.AddMsgEvent(MsgID, 1, 4206)	-- "兑换内功天资秘籍"
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	elseif TalkIndex == 4 then 	-- 选择外功天资秘籍
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 4207)	-- "外功天资秘籍"
			msg.AddMsgEvent(MsgID, 24, TargetID)
			msg.AddMsgEvent(MsgID, 21, 6)	-- 选项1
			msg.AddMsgEvent(MsgID, 1, 4209)	-- "兑换秘籍·剑"
			msg.AddMsgEvent(MsgID, 21, 7)	-- 选项2
			msg.AddMsgEvent(MsgID, 1, 4210)	-- "兑换秘籍·刀"
			msg.AddMsgEvent(MsgID, 21, 8)	-- 选项3
			msg.AddMsgEvent(MsgID, 1, 4211)	-- "兑换秘籍·长兵"
			msg.AddMsgEvent(MsgID, 21, 9)	-- 选项4
			msg.AddMsgEvent(MsgID, 1, 4212)	-- "兑换秘籍·弓"
			msg.AddMsgEvent(MsgID, 21, -1)	-- 选项5
			msg.AddMsgEvent(MsgID, 1, 4125)	-- "返回"
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
	elseif TalkIndex == 6 then
			if mijijiance(MapID, InstanceID, TargetID, TargetTypeID, RoleID) then
				for i = 1, 7 do
					local m = MijiID_1[i]
					local n = role.GetRoleItemNum(RoleID,m)
					if n ~= 0 then
						role.RemoveFromRole(MapID, InstanceID, RoleID, m, 1, 420)
						role.RemoveFromRole(MapID, InstanceID, RoleID, 2310170, 100, 420)
						role.DecRoleSilver(MapID, InstanceID, RoleID, 10000000, 101)
						role.AddRoleItem(MapID, InstanceID, RoleID, 1442201, 1, -1, 8, 420)
					break
					end
				end
			end
	elseif TalkIndex == 7 then
			if mijijiance(MapID, InstanceID, TargetID, TargetTypeID, RoleID)  then
				for i = 1, 7 do
					local m = MijiID_2[i]
					local n = role.GetRoleItemNum(RoleID,m)
					if n ~= 0 then
						role.RemoveFromRole(MapID, InstanceID, RoleID, m, 1, 420)
						role.RemoveFromRole(MapID, InstanceID, RoleID, 2310170, 100, 420)
						role.DecRoleSilver(MapID, InstanceID, RoleID, 10000000, 101)
						role.AddRoleItem(MapID, InstanceID, RoleID, 1442202, 1, -1, 8, 420)
					break
					end
				end
			end
	elseif TalkIndex == 8 then
			if mijijiance(MapID, InstanceID, TargetID, TargetTypeID, RoleID)  then
				for i = 1, 7 do
					local m = MijiID_3[i]
					local n = role.GetRoleItemNum(RoleID, m)
					if n ~= 0 then
						role.RemoveFromRole(MapID, InstanceID, RoleID, m, 1, 420)
						role.RemoveFromRole(MapID, InstanceID, RoleID, 2310170, 100, 420)
						role.DecRoleSilver(MapID, InstanceID, RoleID, 10000000, 101)
						role.AddRoleItem(MapID, InstanceID, RoleID, 1442203, 1, -1, 8, 420)
					break
					end
				end
			end
	elseif TalkIndex == 9 then
			if mijijiance(MapID, InstanceID, TargetID, TargetTypeID, RoleID)  then
				for i = 1, 7 do
					local m = MijiID_4[i]
					local n = role.GetRoleItemNum(RoleID,m)
					if n ~= 0 then
						role.RemoveFromRole(MapID, InstanceID, RoleID, m, 1, 420)
						role.RemoveFromRole(MapID, InstanceID, RoleID, 2310170, 100, 420)
						role.DecRoleSilver(MapID, InstanceID, RoleID, 10000000, 101)
						role.AddRoleItem(MapID, InstanceID, RoleID, 1442204, 1, -1, 8, 420)
					break
					end
				end
			end
	elseif TalkIndex == 5 then	--选择内功天资秘籍
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 4208)	-- "内功天资秘籍"
			msg.AddMsgEvent(MsgID, 24, TargetID)
			msg.AddMsgEvent(MsgID, 21, 10)	-- 选项1
			msg.AddMsgEvent(MsgID, 1, 4213)	-- "兑换秘籍·阳刚"
			msg.AddMsgEvent(MsgID, 21, 11)	-- 选项2
			msg.AddMsgEvent(MsgID, 1, 4214)	-- "兑换秘籍·阴柔"
			msg.AddMsgEvent(MsgID, 21, 12)	-- 选项3
			msg.AddMsgEvent(MsgID, 1, 4215)	-- "兑换秘籍·医毒"
			msg.AddMsgEvent(MsgID, 21, 13)	-- 选项4
			msg.AddMsgEvent(MsgID, 1, 4216)	-- "兑换秘籍·音律"
			msg.AddMsgEvent(MsgID, 21, -1)	-- 选项5
			msg.AddMsgEvent(MsgID, 1, 4125)	-- "返回"
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
	elseif TalkIndex == 10 then
			if mijijiance(MapID, InstanceID, TargetID, TargetTypeID, RoleID)  then
				for i = 1, 7 do
					local m = MijiID_5[i]
					local n = role.GetRoleItemNum(RoleID,m)
					if n ~= 0 then
						role.RemoveFromRole(MapID, InstanceID, RoleID, m, 1, 420)
						role.RemoveFromRole(MapID, InstanceID, RoleID, 2310170, 100, 420)
						role.DecRoleSilver(MapID, InstanceID, RoleID, 10000000, 101)
						role.AddRoleItem(MapID, InstanceID, RoleID, 1442205, 1, -1, 8, 420)
					break
					end
				end
			end
	elseif TalkIndex == 11 then
			if mijijiance(MapID, InstanceID, TargetID, TargetTypeID, RoleID)  then
				for i = 1, 7 do
					local m = MijiID_6[i]
					local n = role.GetRoleItemNum(RoleID,m)
					if n ~= 0 then
						role.RemoveFromRole(MapID, InstanceID, RoleID, m, 1, 420)
						role.RemoveFromRole(MapID, InstanceID, RoleID, 2310170, 100, 420)
						role.DecRoleSilver(MapID, InstanceID, RoleID, 10000000, 101)
						role.AddRoleItem(MapID, InstanceID, RoleID, 1442206, 1, -1, 8, 420)
					break
					end
				end
			end
	elseif TalkIndex == 12 then
			if mijijiance(MapID, InstanceID, TargetID, TargetTypeID, RoleID) then
				for i = 1, 7 do
					local m = MijiID_7[i]
					local n = role.GetRoleItemNum(RoleID,m)
					if n ~= 0 then
						role.RemoveFromRole(MapID, InstanceID, RoleID, m, 1, 420)
						role.RemoveFromRole(MapID, InstanceID, RoleID, 2310170, 100, 420)
						role.DecRoleSilver(MapID, InstanceID, RoleID, 10000000, 101)
						role.AddRoleItem(MapID, InstanceID, RoleID, 1442207, 1, -1, 8, 420)
					break
					end
				end
			end
	elseif TalkIndex == 13 then
			if mijijiance(MapID, InstanceID, TargetID, TargetTypeID, RoleID) then
				for i = 1, 7 do
					local m = MijiID_8[i]
					local n = role.GetRoleItemNum(RoleID,m)
					if n ~= 0 then
						role.RemoveFromRole(MapID, InstanceID, RoleID, m, 1, 420)
						role.RemoveFromRole(MapID, InstanceID, RoleID, 2310170, 100, 420)
						role.DecRoleSilver(MapID, InstanceID, RoleID, 10000000, 101)
						role.AddRoleItem(MapID, InstanceID, RoleID, 1442208, 1, -1, 8, 420)
					break
					end
				end
			end
	end
end


aux. RegisterCreatureEvent(4902006, 7, "mijishangren_On_Talk")























