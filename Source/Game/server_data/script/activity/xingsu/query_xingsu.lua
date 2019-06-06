--查询星宿坐标
function I2610158_UseItem(MapID, InstanceID, TypeID, TargetID)
	for k=3600020, 3600025 do
		if MapID == tiangongxingguan[k].MapCrc then
			--查询是否还有存活的星宿
			aux.WriteLog(tiangongxingguan[k].IsDead)
			if false == tiangongxingguan[k].IsDead then
				--提示玩家“在####，####（坐标）附近有星宿之灵气。”
				local MsgID = msg.BeginMsgEvent()
				--msg.AddMsgEvent(MsgID, 13, 0)
				msg.AddMsgEvent(MsgID, 71, 4)
				msg.AddMsgEvent(MsgID, 9, tiangongxingguan[k].x)
				msg.AddMsgEvent(MsgID, 9, tiangongxingguan[k].z)
				msg.DispatchRoleMsgEvent(TargetID, MsgID)
				return
			end
		end
	end
	--提示玩家“没有发现任何星宿之灵气，星宿已全部重返天界。”
	local MsgID = msg.BeginMsgEvent()
	--msg.AddMsgEvent(MsgID, 13, 0)
	msg.AddMsgEvent(MsgID, 71, 2)
	msg.DispatchRoleMsgEvent(TargetID, MsgID)
end

aux.RegisterItemEvent(2610158, 1, "I2610158_UseItem")
