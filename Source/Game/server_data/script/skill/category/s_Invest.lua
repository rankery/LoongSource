

function Skill_Invest(MapID, InstanceID,SkillID,CasterID,TargetID)
	if MapID == 3017298127 or MapID == 3017298383 or MapID == 3695619387 then
		local TypeID = unit.GetUnitTypeID(MapID, InstanceID, TargetID)
		local seedowner = cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 1)--所属者ID
		if TypeID==4900410 or TypeID==4900413 or TypeID==4900416 then
			if CasterID ~= seedowner then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 13, 0)
				msg.AddMsgEvent(MsgID, 1, 1322)--此植物不属于您！
				msg.DispatchRoleMsgEvent(CasterID, MsgID)
				local MsgID1 = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID1, 26, 1322)	-- "此植物不属于您！"
				msg.DispatchRoleMsgEvent(CasterID, MsgID1)
				return 999
			else
				return 0
			end
		elseif TypeID==4900710 then
		    local Role_GuildID = guild.GetRoleGuildID(CasterID)
			local Master_GuilID = city.GetCityAppointedAtt(3, 2)
			local Role_ZhiWei = -1
			if Role_GuildID ~= nil and Role_GuildID ~= 4294967295 then
				Role_ZhiWei = guild.GetRoleGuildPos(Role_GuildID, CasterID)
			end
		    if Role_GuildID ~= -1 and Role_GuildID ~= 4294967295 and Role_GuildID == Master_GuilID and Role_ZhiWei == 2 then
			    local FreeSize = role.GetBagFreeSize(CasterID)
				if FreeSize >= 2 then
			        return 0
				else
				    local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 26, 3450)--需要两个空格
					msg.DispatchRoleMsgEvent(CasterID, MsgID)
				    return 999
				end
			else
			    local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 3449)--城主才能开
				msg.DispatchRoleMsgEvent(CasterID, MsgID)
			    return 999
			end
		else
			return 0
		end
	elseif MapID == 3084409807 then
	    local TypeID = unit.GetUnitTypeID(MapID, InstanceID, TargetID)
		if TypeID==4900712 then
		    local k = role.GetRoleItemNum(CasterID, 6000219)
			if k < 1 then
			    local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 12221)--需要一个古墓钥匙才能开启宝箱
				msg.DispatchRoleMsgEvent(CasterID, MsgID)
				return 999
			else
			    return 0
			end
		end
	else
		return 0
	end
end
aux.RegisterSkillEvent(9001001,0,"Skill_Invest")
