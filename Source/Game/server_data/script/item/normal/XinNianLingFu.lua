--可否使用函数
function i1350415_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false

	for i = 8990051, 8990054 do
		if 1 <= role.GetRoleItemNum(TargetID, i) then
			return bRet, bIgnore
		end
	end


	for i = 8990041, 8990044 do
		if 1 <= role.GetRoleItemNum(TargetID, i) then
			return bRet, bIgnore
		end
	end

	for i = 8990031, 8990034 do
		if 1 <= role.GetRoleItemNum(TargetID, i) then
			return bRet, bIgnore
		end
	end

	for i = 8990021, 8990024 do
		if 1 <= role.GetRoleItemNum(TargetID, i) then
			return bRet, bIgnore
		end
	end

	for i = 8990011, 8990014 do
		if 1 <= role.GetRoleItemNum(TargetID, i) then
			return bRet, bIgnore
		end
	end

	for i = 8990001, 8990004 do
		if 1 <= role.GetRoleItemNum(TargetID, i) then
			return bRet, bIgnore
		end
	end

	--返回
	return 32, bIgnore
end

aux.RegisterItemEvent(1350415, 0, "i1350415_CanUse")

function I1350415_OnUse(MapID, InstanceID, TypeID, TargetID)
	for i = 8990051, 8990054 do
		if 0 == role.RemoveFromRole(MapID, InstanceID, TargetID, i, 1, 420) then
			role.AddRoleItem(MapID, InstanceID, TargetID, i, 1, 4, 8, 420)
			return
		end
	end


	for i = 8990041, 8990044 do
		if 0 == role.RemoveFromRole(MapID, InstanceID, TargetID, i, 1, 420) then
			local temp=math.random(100)
			if temp<=5 then
				role.AddRoleItem(MapID, InstanceID, TargetID, i+10, 1, 4, 8, 420)
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 13, 1)
				msg.AddMsgEvent(MsgID, 1, 357)
				msg.AddMsgEvent(MsgID, 2, TargetID)
				msg.AddMsgEvent(MsgID, 4, i+10)
				msg.AddMsgEvent(MsgID, 9, (math.floor((i-8990000)/10)+1)*20)
				msg.DispatchWorldMsgEvent(MsgID)
			else
				role.AddRoleItem(MapID, InstanceID, TargetID, i-10, 1, 4, 8, 420)
			end
			return
		end
	end

	for i = 8990031, 8990034 do
		if 0 == role.RemoveFromRole(MapID, InstanceID, TargetID, i, 1, 420) then
			local temp=math.random(100)
			if temp<=12 then
				role.AddRoleItem(MapID, InstanceID, TargetID, i+10, 1, 4, 8, 420)
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 13, 1)
				msg.AddMsgEvent(MsgID, 1, 357)
				msg.AddMsgEvent(MsgID, 2, TargetID)
				msg.AddMsgEvent(MsgID, 4, i+10)
				msg.AddMsgEvent(MsgID, 9, (math.floor((i-8990000)/10)+1)*20)
				msg.DispatchWorldMsgEvent(MsgID)
			else
				role.AddRoleItem(MapID, InstanceID, TargetID, i, 1, 4, 8, 420)
			end
			return
		end
	end

	for i = 8990021, 8990024 do
		if 0 == role.RemoveFromRole(MapID, InstanceID, TargetID, i, 1, 420) then
			local temp=math.random(100)
			if temp<=40 then
				role.AddRoleItem(MapID, InstanceID, TargetID, i+10, 1, 4, 8, 420)
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 13, 1)
				msg.AddMsgEvent(MsgID, 1, 357)
				msg.AddMsgEvent(MsgID, 2, TargetID)
				msg.AddMsgEvent(MsgID, 4, i+10)
				msg.AddMsgEvent(MsgID, 9, (math.floor((i-8990000)/10)+1)*20)
				msg.DispatchWorldMsgEvent(MsgID)
			else
				role.AddRoleItem(MapID, InstanceID, TargetID, i-20, 1, 4, 8, 420)
			end
			return
		end
	end

	for i = 8990011, 8990014 do
		if 0 == role.RemoveFromRole(MapID, InstanceID, TargetID, i, 1, 420) then
			local temp=math.random(100)
			if temp<=75 then
				role.AddRoleItem(MapID, InstanceID, TargetID, i+10, 1, 4, 8, 420)
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 13, 1)
				msg.AddMsgEvent(MsgID, 1, 357)
				msg.AddMsgEvent(MsgID, 2, TargetID)
				msg.AddMsgEvent(MsgID, 4, i+10)
				msg.AddMsgEvent(MsgID, 9, (math.floor((i-8990000)/10)+1)*20)
				msg.DispatchWorldMsgEvent(MsgID)
			else
				role.AddRoleItem(MapID, InstanceID, TargetID, i-10, 1, 4, 8, 420)
			end
			return
		end
	end

	for i = 8990001, 8990004 do
		if 0 == role.RemoveFromRole(MapID, InstanceID, TargetID, i, 1, 420) then
			role.AddRoleItem(MapID, InstanceID, TargetID, i+10, 1, 4, 8, 420)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 13, 1)
			msg.AddMsgEvent(MsgID, 1, 357)
			msg.AddMsgEvent(MsgID, 2, TargetID)
			msg.AddMsgEvent(MsgID, 4, i+10)
			msg.AddMsgEvent(MsgID, 9, (math.floor((i-8990000)/10)+1)*20)
			msg.DispatchWorldMsgEvent(MsgID)
			return
		end
	end
end

aux.RegisterItemEvent(1350415, 1, "I1350415_OnUse")
