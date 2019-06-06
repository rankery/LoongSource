--此处用于配置全局道具ID及宝箱ID。
TotalRandomItemNum = 2				--所需背包空间
RandomBoxID =  6010809 				--宝箱ID（需要把下面事件注册内的BoxID改成填写ID）


RandomItemID1 = 6010001		--0.1% 有公告
RandomItemNum1 = 1
RandomItemID2 = 6010101		--0.1% 有公告
RandomItemNum2 = 1
RandomItemID3 = 3304163		--0.1% 有公告
RandomItemNum3 = 1
RandomItemID4 = 1351208		--0.7%
RandomItemNum4 = 1
RandomItemID5 = 6000008		--2%
RandomItemNum5 = 1
RandomItemID6 = 6000200		--2%
RandomItemNum6 = 1
RandomItemID7 = 1360002		--5%
RandomItemNum7 = 1
RandomItemID8 = 6000221		--5%
RandomItemNum8 = 1
RandomItemID9 = 6000002		--10%
RandomItemNum9 = 1
RandomItemID10 = 1000209		--15%
RandomItemNum10 = 1
RandomItemID11 = 6000007		--20%
RandomItemNum11 = 1
RandomItemID12 = 1000017		--20%
RandomItemNum12 = 1
RandomItemID13 = 1000117		--20%
RandomItemNum13 = 1



aux.RegisterItemEvent(6010809,1,"RandomBoxID_OnUse")  		--这里位置靠前的BoxID改成宝箱ID
aux.RegisterItemEvent(6010809,0,"RandomBoxID_CanUse")		--这里位置靠前的BoxID改成宝箱ID



function RandomBoxID_CanUse(MapID,InstanceID,TypeID,RoleID)

	local bRet,bIgnore = 0,false

	local FreeSize = role.GetBagFreeSize(RoleID)

	if FreeSize <= TotalRandomItemNum then

		bRet = 40
        end
	return bRet,bIgnore

end

function RandomBoxID_OnUse(MapID,InstanceID,TypeID,RoleID)
    local function GetrandomItem(MapID,InstanceID,TypeID,RoleID)
		local k = math.random(1002)

		if k > 0 and k <= 10 then
			role.AddRoleItem(MapID,InstanceID,RoleID,RandomItemID4,RandomItemNum,-1,8,420)
		elseif k > 10 and k <= 20 then
			role.AddRoleItem(MapID,InstanceID,RoleID,RandomItemID5,RandomItemNum,-1,8,420)
		elseif k > 20 and k <= 40 then
			role.AddRoleItem(MapID,InstanceID,RoleID,RandomItemID6,RandomItemNum,-1,8,420)
		elseif k > 40 and k <= 90 then
			role.AddRoleItem(MapID,InstanceID,RoleID,RandomItemID7,RandomItemNum,-1,8,420)
		elseif k > 90 and k <= 140 then
			role.AddRoleItem(MapID,InstanceID,RoleID,RandomItemID8,RandomItemNum,-1,8,420)
		elseif k > 140 and k <= 240 then
			role.AddRoleItem(MapID,InstanceID,RoleID,RandomItemID9,RandomItemNum,-1,8,420)
		elseif k > 240 and k <= 390 then
			role.AddRoleItem(MapID,InstanceID,RoleID,RandomItemID10,RandomItemNum,-1,8,420)
		elseif k > 390 and k <= 590 then
			role.AddRoleItem(MapID,InstanceID,RoleID,RandomItemID11,RandomItemNum,-1,8,420)
		elseif k > 590 and k <= 790 then
			role.AddRoleItem(MapID,InstanceID,RoleID,RandomItemID12,RandomItemNum,-1,8,420)
		elseif k > 790 and k < 1000 then
			role.AddRoleItem(MapID,InstanceID,RoleID,RandomItemID13,RandomItemNum,-1,8,420)
		elseif k == 1000 then
			role.AddRoleItem(MapID,InstanceID,RoleID,RandomItemID1,RandomItemNum,-1,8,420)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 4413)
			msg.AddMsgEvent(MsgID, 2, RoleID)
			msg.AddMsgEvent(MsgID, 4, RandomBoxID)
			msg.AddMsgEvent(MsgID, 4, RandomItemID1)
			msg.DispatchWorldMsgEvent(MsgID)
		elseif k == 1001 then
			role.AddRoleItem(MapID,InstanceID,RoleID,RandomItemID2,RandomItemNum,-1,8,420)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 4413)
			msg.AddMsgEvent(MsgID, 2, RoleID)
			msg.AddMsgEvent(MsgID, 4, RandomBoxID)
			msg.AddMsgEvent(MsgID, 4, RandomItemID2)
			msg.DispatchWorldMsgEvent(MsgID)
		elseif k == 1002 then
			role.AddRoleItem(MapID,InstanceID,RoleID,RandomItemID3,RandomItemNum,-1,8,420)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 4413)
			msg.AddMsgEvent(MsgID, 2, RoleID)
			msg.AddMsgEvent(MsgID, 4, RandomBoxID)
			msg.AddMsgEvent(MsgID, 4, RandomItemID3)
			msg.DispatchWorldMsgEvent(MsgID)
		end
	end
	local  Level = role.GetRoleLevel(MapID, InstanceID, RoleID)
	GetrandomItem(MapID,InstanceID,TypeID,RoleID,0)
	role.AddRoleExp(MapID, InstanceID, RoleID, Level*500)
	role.AddRoleSilver(MapID, InstanceID, RoleID, Level*800, 102)
	local k = math.random(2)
	if k == 2 then
		GetrandomItem(MapID,InstanceID,TypeID,RoleID)
	end
end


