GM_fubenhuodong2_on = 0
GM_fubenhuodong2_t = 0
GM_fubenhuodong2_h = 0
GM_fubenhuodong2_m = 0

function GM_fubenhuodong2(MapID, InstanceID, TypeID, TargetID)

	GM_fubenhuodong2_on = 1
	GM_fubenhuodong2_t = 0
	GM_fubenhuodong2_h = tonumber(os.date("%H"))
	GM_fubenhuodong2_m = tonumber(os.date("%M"))

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 1811)	--新服活动之“镇压！枭吴祭坛”已开启，在3小时内所有挑战枭吴祭坛副本并击杀首领“枭吴教头狂爪”
	--的37级以下玩家均可获得相当于当前等级升级经验值的经验奖励！
	msg.AddMsgEvent(MsgID, 9, 30*(6-GM_fubenhuodong2_t))
	GM_fubenhuodong2_t=1
	msg.DispatchWorldMsgEvent(MsgID)

end


aux.RegisterItemEvent(1131102, 1, "GM_fubenhuodong2")
