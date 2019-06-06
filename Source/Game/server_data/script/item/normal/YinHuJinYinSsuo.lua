--可否使用函数
function i3303151_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false

	for i = 8990202, 8990206 do
		if 1 <= role.GetRoleItemNum(TargetID, i) then
			return bRet, bIgnore
		end
	end

	--   返回
	return 32, bIgnore
end

aux.RegisterItemEvent(3303151, 0, "i3303151_CanUse")

function I3303151_OnUse(MapID, InstanceID, TypeID, TargetID)
	if 0 < role.GetRoleItemNum(TargetID, 8990206) then
		if 0 == role.RemoveFromRole(MapID, InstanceID, TargetID, 8990206, 1, 420) then
			role.AddRoleItem(MapID, InstanceID, TargetID, 8990206, 1, 4, 8, 420)
			return
		end
	end


	if 0 < role.GetRoleItemNum(TargetID, 8990205) then
		if 0 == role.RemoveFromRole(MapID, InstanceID, TargetID, 8990205, 1, 420) then
			local temp=math.random(100)
			if temp<=5 then
				role.AddRoleItem(MapID, InstanceID, TargetID, 8990206, 1, 4, 8, 420)
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 13, 1)
				msg.AddMsgEvent(MsgID, 1, 358)
				msg.AddMsgEvent(MsgID, 2, TargetID)
				msg.AddMsgEvent(MsgID, 4, 8990206)
				msg.AddMsgEvent(MsgID, 9, 80)
				msg.DispatchWorldMsgEvent(MsgID)
			else
				role.AddRoleItem(MapID, InstanceID, TargetID, 8990204, 1, 4, 8, 420)
			    local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 13, 1)
				msg.AddMsgEvent(MsgID, 1, 366)
				msg.AddMsgEvent(MsgID, 9, 40)
				msg.DispatchRoleMsgEvent(TargetID, MsgID)     --发送消息
			end
			return
		end
	end

	if 0 < role.GetRoleItemNum(TargetID, 8990204) then
		if 0 == role.RemoveFromRole(MapID, InstanceID, TargetID, 8990204, 1, 420) then
			local temp=math.random(100)
			if temp<=12 then
				role.AddRoleItem(MapID, InstanceID, TargetID, 8990205, 1, 4, 8, 420)
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 13, 1)
				msg.AddMsgEvent(MsgID, 1, 358)
				msg.AddMsgEvent(MsgID, 2, TargetID)
				msg.AddMsgEvent(MsgID, 4, 8990205)
				msg.AddMsgEvent(MsgID, 9, 60)
				msg.DispatchWorldMsgEvent(MsgID)
			else
				role.AddRoleItem(MapID, InstanceID, TargetID, 8990204, 1, 4, 8, 420)
			    local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 13, 1)
				msg.AddMsgEvent(MsgID, 1, 366)
				msg.AddMsgEvent(MsgID, 9, 40)
				msg.DispatchRoleMsgEvent(TargetID, MsgID)     --发送消息
			end
			return
		end
	end

	if 0 < role.GetRoleItemNum(TargetID, 8990203) then
		if 0 == role.RemoveFromRole(MapID, InstanceID, TargetID, 8990203, 1, 420) then
			local temp=math.random(100)
			if temp<=40 then
				role.AddRoleItem(MapID, InstanceID, TargetID, 8990204, 1, 4, 8, 420)
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 13, 1)
				msg.AddMsgEvent(MsgID, 1, 358)
				msg.AddMsgEvent(MsgID, 2, TargetID)
				msg.AddMsgEvent(MsgID, 4, 8990204)
				msg.AddMsgEvent(MsgID, 9, 40)
				msg.DispatchWorldMsgEvent(MsgID)
			else
				role.AddRoleItem(MapID, InstanceID, TargetID, 8990202, 1, 4, 8, 420)
	            local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 13, 1)
				msg.AddMsgEvent(MsgID, 1, 366)
				msg.AddMsgEvent(MsgID, 9, 1)
				msg.DispatchRoleMsgEvent(TargetID, MsgID)     --发送消息
    	end
			return
		end
	end

	if 0 < role.GetRoleItemNum(TargetID, 8990202) then
		if 0 == role.RemoveFromRole(MapID, InstanceID, TargetID, 8990202, 1, 420) then
			local temp=math.random(100)
			if temp<=75 then
				role.AddRoleItem(MapID, InstanceID, TargetID, 8990203, 1, 4, 8, 420)
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 13, 1)
				msg.AddMsgEvent(MsgID, 1, 358)
				msg.AddMsgEvent(MsgID, 2, TargetID)
				msg.AddMsgEvent(MsgID, 4, 8990203)
				msg.AddMsgEvent(MsgID, 9, 20)
				msg.DispatchWorldMsgEvent(MsgID)
			else
				role.AddRoleItem(MapID, InstanceID, TargetID, 8990202, 1, 4, 8, 420)
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 13, 1)
				msg.AddMsgEvent(MsgID, 1, 366)
				msg.AddMsgEvent(MsgID, 9, 1)
				msg.DispatchRoleMsgEvent(TargetID, MsgID)     --发送消息
			end
			return
		end
	end

end

aux.RegisterItemEvent(3303151, 1, "I3303151_OnUse")
