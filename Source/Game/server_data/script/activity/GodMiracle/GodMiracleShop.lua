function GodMiracle_On_Can_Buy(MapID, InstanceID, ShopID, RoleID, ItemID)

	local MasterGuildID = GetGuildByGodMiracle(MapID)

	local GuildID = guild.GetRoleGuildID(RoleID)

	if MasterGuildID~=GuildID then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 1601)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end

	for i=1,8 do
		if ItemID == 3200818+i then

			local Role_Expoit = role.GetGuildExploit(RoleID)
			local Exploit_Needed = {}
			Exploit_Needed[3200819]=360
			Exploit_Needed[3200820]=360
			Exploit_Needed[3200821]=1080
			Exploit_Needed[3200822]=1080
			Exploit_Needed[3200823]=3240
			Exploit_Needed[3200824]=3240
			Exploit_Needed[3200825]=16200
			Exploit_Needed[3200826]=16200

			if Role_Expoit<Exploit_Needed[ItemID] then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 1603)
				msg.AddMsgEvent(MsgID, 9, Exploit_Needed[ItemID])
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				return 0
			end
		end
	end

	return 1

end


aux.RegisterCreatureEvent(4900504, 16, "GodMiracle_On_Can_Buy")
aux.RegisterCreatureEvent(4900505, 16, "GodMiracle_On_Can_Buy")
aux.RegisterCreatureEvent(4900506, 16, "GodMiracle_On_Can_Buy")
aux.RegisterCreatureEvent(4900507, 16, "GodMiracle_On_Can_Buy")
aux.RegisterCreatureEvent(4900508, 16, "GodMiracle_On_Can_Buy")
