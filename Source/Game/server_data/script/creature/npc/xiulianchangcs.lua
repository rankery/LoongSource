--NPC修炼场传送人脚本

function xiulianchangcs(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
	local curhour = tonumber(os.date("%H"))
	local curmin = tonumber(os.date("%M"))
	local CanEnterXLC_1 = 0

	if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
		if TargetTypeID==4500402 then
			msg.AddMsgEvent(MsgID, 20,	70)		--“是否进入副本”
		elseif TargetTypeID==4500422 then
			msg.AddMsgEvent(MsgID, 20,	71)		--“是否进入副本”
		elseif TargetTypeID==4500442 then
			msg.AddMsgEvent(MsgID, 20,	72)		--“是否进入副本”
		elseif TargetTypeID==4500462 then
			msg.AddMsgEvent(MsgID, 20,	73)		--“是否进入副本”
		elseif TargetTypeID==4500522 then
			msg.AddMsgEvent(MsgID, 20,	74)		--“是否进入副本”
		elseif TargetTypeID==4500482 then
			msg.AddMsgEvent(MsgID, 20,	75)		--“是否进入副本”
		elseif TargetTypeID==4500502 then
			msg.AddMsgEvent(MsgID, 20,	76)		--“是否进入副本”
		end
		if (curhour == 0 and curmin >= 30 and curmin <= 59) or (curhour == 12 and curmin >= 30 and curmin <= 59) or (curhour == 21 and curmin >= 30 and curmin <= 59) then
			msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
			msg.AddMsgEvent(MsgID, 21,	4)			--确定按钮
			msg.AddMsgEvent(MsgID, 1,	80)			--“确定”
			msg.AddMsgEvent(MsgID, 21,	5)			--取消按钮
			msg.AddMsgEvent(MsgID, 1,	81)			--“取消”
			msg.AddMsgEvent(MsgID, 21,	6)			--免费进入
			msg.AddMsgEvent(MsgID, 1,	1597)			--“免费进入修炼场”
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
			msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
			msg.AddMsgEvent(MsgID, 21,	4)			--确定按钮
			msg.AddMsgEvent(MsgID, 1,	80)			--“确定”
			msg.AddMsgEvent(MsgID, 21,	5)			--取消按钮
			msg.AddMsgEvent(MsgID, 1,	81)			--“取消”
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		end
	elseif TalkIndex == 4 then
		local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
		local EnterType = 0
		if TargetTypeID==4500402 then
			if rolelevel>=40 and rolelevel<=50 then
				if 0==role.RemoveFromRole(MapID, InstanceID, RoleID, 3200401, 1, 420) then
					CanEnterXLC_1 = 1
				end
			end

		elseif TargetTypeID==4500422 then
			if rolelevel>=50 and rolelevel<=60 then
				if 0==role.RemoveFromRole(MapID, InstanceID, RoleID, 3200411, 1, 420) then
					CanEnterXLC_1 = 1
				end
			end

		elseif TargetTypeID==4500442 then
			if rolelevel>=60 and rolelevel<=70 then
				if 0==role.RemoveFromRole(MapID, InstanceID, RoleID, 3200421, 1, 420) then
					CanEnterXLC_1 = 1
				end
			end

		elseif TargetTypeID==4500462 then
			if rolelevel>=70 and rolelevel<=80 then
				if 0==role.RemoveFromRole(MapID, InstanceID, RoleID, 3200431, 1, 420) then
					CanEnterXLC_1 = 1
				end
			end
		elseif TargetTypeID==4500522 then
			if rolelevel>=10 and rolelevel<=40 then
				if 0==role.RemoveFromRole(MapID, InstanceID, RoleID, 3200472, 1, 420) then
					CanEnterXLC_1 = 1
					EnterType = 1
				end
			end
			if CanEnterXLC_1~=1 and rolelevel>=25 and rolelevel<=40 then
				if 0==role.RemoveFromRole(MapID, InstanceID, RoleID, 3200467, 1, 420) then
					CanEnterXLC_1 = 1
				end
			end
		elseif TargetTypeID==4500482 then
			if rolelevel>=80 and rolelevel<=90 then
				if 0==role.RemoveFromRole(MapID, InstanceID, RoleID, 3200441, 1, 420) then
					CanEnterXLC_1 = 1
				end
			end
		elseif TargetTypeID==4500502 then
			if rolelevel>=90  then
				if 0==role.RemoveFromRole(MapID, InstanceID, RoleID, 3200451, 1, 420) then
					CanEnterXLC_1 = 1
				end
			end
		end

		if CanEnterXLC_1==1 and TargetTypeID==4500402 then
			role.RoleGotoNewMap(MapID, InstanceID, RoleID, 2291722097, 251, 17136, 700)
			local Times = role.GetRoleScriptData(RoleID, 1, RoleDataType["JinRuXiuLianChang"])
			if Times == nil then
				Times = 1
				role.SetRoleScriptData(RoleID, 1, RoleDataType["JinRuXiuLianChang"], 1)
			else
				role.SetRoleScriptData(RoleID, 1, RoleDataType["JinRuXiuLianChang"], Times + 1)
				if Times + 1 == 1 then
					role.SigTitleEvent(OwnerID, 154)
				elseif Times + 1 == 20 then
					role.SigTitleEvent(OwnerID, 155)
				elseif Times + 1 == 50 then
					role.SigTitleEvent(OwnerID, 156)
				end
			end
		elseif CanEnterXLC_1==1 and TargetTypeID==4500422 then
			role.RoleGotoNewMap(MapID, InstanceID, RoleID, 2291721329, 499, 0, 448)
			local Times = role.GetRoleScriptData(RoleID, 1, RoleDataType["JinRuXiuLianChang"])
			if Times == nil then
				Times = 1
				role.SetRoleScriptData(RoleID, 1, RoleDataType["JinRuXiuLianChang"], 1)
			else
				role.SetRoleScriptData(RoleID, 1, RoleDataType["JinRuXiuLianChang"], Times + 1)
				if Times + 1 == 1 then
					role.SigTitleEvent(OwnerID, 154)
				elseif Times + 1 == 20 then
					role.SigTitleEvent(OwnerID, 155)
				elseif Times + 1 == 50 then
					role.SigTitleEvent(OwnerID, 156)
				end
			end
		elseif CanEnterXLC_1==1 and TargetTypeID==4500442 then
			role.RoleGotoNewMap(MapID, InstanceID, RoleID, 2291721585, 251, 17136, 700)
            local Times = role.GetRoleScriptData(RoleID, 1, RoleDataType["JinRuXiuLianChang"])
			if Times == nil then
				Times = 1
				role.SetRoleScriptData(RoleID, 1, RoleDataType["JinRuXiuLianChang"], 1)
			else
				role.SetRoleScriptData(RoleID, 1, RoleDataType["JinRuXiuLianChang"], Times + 1)
				if Times + 1 == 1 then
					role.SigTitleEvent(OwnerID, 154)
				elseif Times + 1 == 20 then
					role.SigTitleEvent(OwnerID, 155)
				elseif Times + 1 == 50 then
					role.SigTitleEvent(OwnerID, 156)
				end
			end
		elseif CanEnterXLC_1==1 and TargetTypeID==4500462 then
			role.RoleGotoNewMap(MapID, InstanceID, RoleID, 2291720817, 499, 0, 448)
		    local Times = role.GetRoleScriptData(RoleID, 1, RoleDataType["JinRuXiuLianChang"])
			if Times == nil then
				Times = 1
				role.SetRoleScriptData(RoleID, 1, RoleDataType["JinRuXiuLianChang"], 1)
			else
				role.SetRoleScriptData(RoleID, 1, RoleDataType["JinRuXiuLianChang"], Times + 1)
				if Times + 1 == 1 then
					role.SigTitleEvent(OwnerID, 154)
				elseif Times + 1 == 20 then
					role.SigTitleEvent(OwnerID, 155)
				elseif Times + 1 == 50 then
					role.SigTitleEvent(OwnerID, 156)
				end
			end
		elseif CanEnterXLC_1==1 and TargetTypeID==4500522 then
			role.RoleGotoNewMap(MapID, InstanceID, RoleID, 2291721073, 500, 40, 500)
			role.SetRoleScriptData(RoleID, 1, RoleDataType["XSXLC_EnterType"], EnterType)

		    local Times = role.GetRoleScriptData(RoleID, 1, RoleDataType["JinRuXiuLianChang"])
			if Times == nil then
				Times = 1
				role.SetRoleScriptData(RoleID, 1, RoleDataType["JinRuXiuLianChang"], 1)
			else
				role.SetRoleScriptData(RoleID, 1, RoleDataType["JinRuXiuLianChang"], Times + 1)
				if Times + 1 == 1 then
					role.SigTitleEvent(OwnerID, 154)
				elseif Times + 1 == 20 then
					role.SigTitleEvent(OwnerID, 155)
				elseif Times + 1 == 50 then
					role.SigTitleEvent(OwnerID, 156)
				end
			end
		elseif CanEnterXLC_1==1 and TargetTypeID==4500482 then
			role.RoleGotoNewMap(MapID, InstanceID, RoleID, 2291720305, 500, 40, 500)
            local Times = role.GetRoleScriptData(RoleID, 1, RoleDataType["JinRuXiuLianChang"])
			if Times == nil then
				Times = 1
				role.SetRoleScriptData(RoleID, 1, RoleDataType["JinRuXiuLianChang"], 1)
			else
				role.SetRoleScriptData(RoleID, 1, RoleDataType["JinRuXiuLianChang"], Times + 1)
				if Times + 1 == 1 then
					role.SigTitleEvent(OwnerID, 154)
				elseif Times + 1 == 20 then
					role.SigTitleEvent(OwnerID, 155)
				elseif Times + 1 == 50 then
					role.SigTitleEvent(OwnerID, 156)
				end
			end
		elseif CanEnterXLC_1==1 and TargetTypeID==4500502 then
			role.RoleGotoNewMap(MapID, InstanceID, RoleID, 2291720561, 499, 0, 448)
		    local Times = role.GetRoleScriptData(RoleID, 1, RoleDataType["JinRuXiuLianChang"])
			if Times == nil then
				Times = 1
				role.SetRoleScriptData(RoleID, 1, RoleDataType["JinRuXiuLianChang"], 1)
			else
				role.SetRoleScriptData(RoleID, 1, RoleDataType["JinRuXiuLianChang"], Times + 1)
				if Times + 1 == 1 then
					role.SigTitleEvent(OwnerID, 154)
				elseif Times + 1 == 20 then
					role.SigTitleEvent(OwnerID, 155)
				elseif Times + 1 == 50 then
					role.SigTitleEvent(OwnerID, 156)
				end
			end
		else
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 	82)	--您等级不足，不能进入该副本
			msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
			msg.AddMsgEvent(MsgID, 1, 	22)		--取消
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		end
	elseif TalkIndex == 6 then
		local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
		if TargetTypeID==4500402 then
			if rolelevel>=40 and rolelevel<=50 then
				CanEnterXLC_1 = 1
			end
		elseif TargetTypeID==4500422 then
			if rolelevel>=50 and rolelevel<=60 then
				CanEnterXLC_1 = 1
			end
		elseif TargetTypeID==4500442 then
			if rolelevel>=60 and rolelevel<=70 then
				CanEnterXLC_1 = 1
			end
		elseif TargetTypeID==4500462 then
			if rolelevel>=70 and rolelevel<=80 then
				CanEnterXLC_1 = 1
			end
		elseif TargetTypeID==4500522 then
			if rolelevel>=10 and rolelevel<=40 then
				CanEnterXLC_1 = 1
			end
		elseif TargetTypeID==4500482 then
			if rolelevel>=80 and rolelevel<=90 then
				CanEnterXLC_1 = 1
			end
		elseif TargetTypeID==4500502 then
			if rolelevel>=90 then
				CanEnterXLC_1 = 1
			end
		end

		if CanEnterXLC_1==1 then

			if TargetTypeID==4500402 then
				role.RoleGotoNewMap(MapID, InstanceID, RoleID, 2291722097, 251, 17136, 700)
			elseif TargetTypeID==4500422 then
				role.RoleGotoNewMap(MapID, InstanceID, RoleID, 2291721329, 499, 0, 448)
			elseif TargetTypeID==4500442 then
				role.RoleGotoNewMap(MapID, InstanceID, RoleID, 2291721585, 251, 17136, 700)
			elseif TargetTypeID==4500462 then
				role.RoleGotoNewMap(MapID, InstanceID, RoleID, 2291720817, 499, 0, 448)
			elseif TargetTypeID==4500522 then
				role.RoleGotoNewMap(MapID, InstanceID, RoleID, 2291721073, 500, 40, 500)
			elseif TargetTypeID==4500482 then
				role.RoleGotoNewMap(MapID, InstanceID, RoleID, 2291720305, 500, 40, 500)
			elseif TargetTypeID==4500502 then
				role.RoleGotoNewMap(MapID, InstanceID, RoleID, 2291720561, 499, 0, 448)
			end
			unit.AddBuff(MapID, InstanceID, RoleID, 2020201, RoleID)
		else
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 	1598)	--您等级不足，不能进入该修炼场
			msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
			msg.AddMsgEvent(MsgID, 1, 	22)		--取消
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		end
	end
end

-- 注册
aux.RegisterCreatureEvent(4500402, 7, "xiulianchangcs")
aux.RegisterCreatureEvent(4500422, 7, "xiulianchangcs")
aux.RegisterCreatureEvent(4500442, 7, "xiulianchangcs")
aux.RegisterCreatureEvent(4500462, 7, "xiulianchangcs")
aux.RegisterCreatureEvent(4500522, 7, "xiulianchangcs")
aux.RegisterCreatureEvent(4500482, 7, "xiulianchangcs")
aux.RegisterCreatureEvent(4500502, 7, "xiulianchangcs")
