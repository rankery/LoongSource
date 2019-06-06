function WordCup_On_Can_Buy(MapID, InstanceID, ShopID, RoleID, ItemID)


	for i=1,8 do
		if ItemID == 8907000+i then

			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 3276)	--∂”ŒÈ√˚
			msg.DispatchRoleMsgEvent(RoleID, MsgID)

			return 0
		end
	end

	return 1

end


aux.RegisterCreatureEvent(4900489, 16, "WordCup_On_Can_Buy")
