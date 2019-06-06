function I3304015_OnUse(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.SetSoarValue(TargetID, 1)
	role.OpenTrain(TargetID, 87)
	role.SigTitleEvent(TargetID, 282)
	unit.AddBuff(MapID, InstanceID, TargetID, 9431201, TargetID)
	local k = act.GetActScriptData(99, 1, 6) + 1
	if k <= 5 then
	    act.SetActScriptData(99, 1, k, TargetID)
		--act.SetActScriptData(99, 1, k+6, 10)
	end
	act.SetActScriptData(99, 1, 6, k)
	act.SaveActScriptData(99)
	local MsgID = msg.BeginMsgEvent()		-- XXX服下了飞升丹，成为被服务器第X位飞升的玩家！
	msg.AddMsgEvent(MsgID, 100, 3112)
	msg.AddMsgEvent(MsgID, 2, TargetID)
	msg.AddMsgEvent(MsgID, 9, k)
	msg.DispatchWorldMsgEvent(MsgID)
	local MsgID = msg.BeginMsgEvent()		-- 恭喜您成功飞升！这是一个全新的开始，您可以对自己的装备进行新的强化——炼器和熔铸，更可以进入更有挑战性的副本——上古无回洞穴和心魔界！
	msg.AddMsgEvent(MsgID, 71, 3113)
	msg.DispatchRoleMsgEvent(TargetID, MsgID)     --发送消息
	if k <= 200 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 102, 100122)    --XXX服下了飞升丹，成为被服务器第X位飞升的玩家！
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 9, k)
		msg.DispatchBroadcast(MsgID,-1,-1,-1)
		--[[local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 102, 100122)    --XXX服下了飞升丹，成为被服务器第X位飞升的玩家！
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 9, k)
		msg.DispatchBroadcast(MsgID,-1,-1,-1)]]
		local MsgID = msg.BeginMsgEvent()		-- XXX服下了飞升丹，成为被服务器第X位飞升的玩家！
		msg.AddMsgEvent(MsgID, 101, 3111)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 9, k)
		msg.DispatchWorldMsgEvent(MsgID)
		--[[local MsgID = msg.BeginMsgEvent()		-- XXX服下了飞升丹，成为被服务器第X位飞升的玩家！
		msg.AddMsgEvent(MsgID, 101, 3111)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 9, k)
		msg.DispatchWorldMsgEvent(MsgID)]]
	end

end

aux.RegisterItemEvent(3304015, 1, "I3304015_OnUse")

