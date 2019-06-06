GM_fubenhuodong_on = 0
GM_fubenhuodong_t = 0
GM_fubenhuodong_h = 0
GM_fubenhuodong_m = 0

function GM_fubenhuodong(MapID, InstanceID, TypeID, TargetID)

	GM_fubenhuodong_on = 1
	GM_fubenhuodong_t = 0
	GM_fubenhuodong_h = tonumber(os.date("%H"))
	GM_fubenhuodong_m = tonumber(os.date("%M"))

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 1809)	--新服活动之“镇压！裕祥山麓”已开启，在3小时内所有挑战裕祥山麓副本并击杀首领“风刃霸山”
	--的37级以下玩家均可获得相当于当前等级升级经验值的经验奖励！
	msg.AddMsgEvent(MsgID, 9, 30*(6-GM_fubenhuodong_t))
	GM_fubenhuodong_t=1
	msg.DispatchWorldMsgEvent(MsgID)

end


aux.RegisterItemEvent(1131101, 1, "GM_fubenhuodong")
