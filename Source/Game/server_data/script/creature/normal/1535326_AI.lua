
--怪物死亡喊话
function c1535326_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local k = math.random(100)
	if k == 77 then
	    if role.GetBagFreeSize(RoleID) > 1 then
			role.AddRoleItem(MapID, InstanceID, RoleID, 4000023, 1, -1, 8, 420)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 3131)    --XXX完成了对赤火麒麟的最后一击，获得了传说中的座骑九天麟日兽！
			msg.DispatchWorldMsgEvent(MsgID)
		end
	end

end

aux.RegisterCreatureEvent(1535326, 4, "c1535326_OnDie")

