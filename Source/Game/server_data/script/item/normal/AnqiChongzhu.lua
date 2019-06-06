	--RoleDataType["ChongzhuYellow"]	= 157			黄色暗器重铸记录
	--RoleDataType["ChongzhuGreen"]	= 158				绿色暗器重铸记录
	--RoleDataType["ChongzhuBlue"]		= 159			蓝色暗器重铸记录
	--RoleDataType["ChongzhuOrange"]		= 160		橙色暗器重铸记录
function I6000224_OnUse(MapID,InstanceID,TypeID,RoleID)
	local ChongzhuYellow = role.GetRoleScriptData(RoleID,1,RoleDataType["ChongzhuYellow"])			--获取玩家黄色暗器重铸记录
	for yellow = 8700001,8700006 do
	    if role.GetRoleItemNum(RoleID,yellow) > 0 then												--如果背包内有黄色暗器
			local yellowUp = yellow + 6
			local yellowDown = yellow - 6
			local shibaiNum = math.floor(ChongzhuYellow/10000)										--获取黄色暗器重铸失败次数
			local bubianNum = math.floor(ChongzhuYellow/10)%1000									--获取黄色暗器重铸不变次数
			local shengjiNum = ChongzhuYellow%10													--获取黄色暗器是否可直接升级
			if shengjiNum >= 1 or bubianNum >= 2 then												--如果暗器可直接升级或者不变次数为3以上
				role.RemoveFromRole(MapID,InstanceID,RoleID,yellow,1,420)							--删除旧有黄色暗器
				role.AddRoleItem(MapID, InstanceID, RoleID, yellowUp, 1, 2, 8, 420)					--添加绿色暗器
				role.SetRoleScriptData(RoleID,1,RoleDataType["ChongzhuYellow"],0)					--所有记录清0
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 13, 0)
				msg.AddMsgEvent(MsgID, 1, 4304)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			elseif	bubianNum < 2 and shengjiNum == 0 then											--如果不变次数小于3
				role.RemoveFromRole(MapID, InstanceID, RoleID, yellow, 1, 420)						--删除旧有黄色暗器
				role.AddRoleItem(MapID, InstanceID, RoleID, yellow, 1, 1, 8, 420)					--添加黄色暗器
				role.SetRoleScriptData(RoleID,1,RoleDataType["ChongzhuYellow"],ChongzhuYellow+10)	--黄色暗器重铸不变次数加1
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 13, 0)
				msg.AddMsgEvent(MsgID, 1, 4302)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
			return
		end
	end

	local ChongzhuGreen = role.GetRoleScriptData(RoleID,1,RoleDataType["ChongzhuGreen"]) 		 	--获取玩家绿色暗器重铸记录

	for green = 8700007,8700012 do
		if role.GetRoleItemNum(RoleID,green) > 0 then												--如果背包内有绿色暗器
			local greenUp = green + 6
			local greenDown = green - 6
			local shibaiNum2 = math.floor(ChongzhuGreen/10000)											--获取绿色暗器失败次数
			local bubianNum2 = math.floor(ChongzhuGreen/10)%1000										--获取绿色暗器不变次数
			local shengjiNum2 = ChongzhuGreen%10														--获取绿色暗器是否可直接升级
			if bubianNum2 >= 9 and shibaiNum2 >= 3 or shengjiNum2 >= 1 then							--如果不变次数大于等于9并且失败次数大于等于3或者可直接升级为1
				role.RemoveFromRole(MapID,InstanceID,RoleID,green,1,420)								--删除旧有绿色暗器
				role.AddRoleItem(MapID, InstanceID,RoleID,greenUp,1,3,8,420)							--添加蓝色暗器
				role.SetRoleScriptData(RoleID,1,RoleDataType["ChongzhuGreen"],0)						--所有记录清0
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 13, 0)
				msg.AddMsgEvent(MsgID, 1, 4304)
				msg.DispatchRoleMsgEvent(RoleID,MsgID)
			elseif bubianNum2 < 9 or shibaiNum2 < 3 and shengjiNum2 == 0 then							--如果不变次数小于9或者失败次数小于3并且不可直接升级
				local temp2 = math.random(100)
				if temp2 <= 75 then
					role.RemoveFromRole(MapID,InstanceID,RoleID,green,1,420)							--删除旧有绿色暗器
					role.AddRoleItem(MapID,InstanceID,RoleID,green,1,2,8,420)							--添加绿色暗器
					role.SetRoleScriptData(RoleID,1,RoleDataType["ChongzhuGreen"],ChongzhuGreen+10)		--绿色暗器不变次数加1
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 13, 0)
					msg.AddMsgEvent(MsgID, 1, 4302)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
					else
					role.RemoveFromRole(MapID,InstanceID,RoleID,green,1,420)							--删除旧有绿色暗器
					role.AddRoleItem(MapID,InstanceID,RoleID,greenDown,1,1,8,420)						--暗器降级
					role.SetRoleScriptData(RoleID,1,RoleDataType["ChongzhuGreen"],ChongzhuGreen+10000)	--绿色暗器失败次数加1
					role.SetRoleScriptData(RoleID,1,RoleDataType["ChongzhuYellow"],ChongzhuYellow+1)	--降级后的暗器可直接升级
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 13, 0)
					msg.AddMsgEvent(MsgID, 1, 4303)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				end
			end
			return
		end
	end

	local ChongzhuBlue = role.GetRoleScriptData(RoleID,1,RoleDataType["ChongzhuBlue"]) 		 	--获取玩家蓝色暗器重铸记录

	for blue = 8700013,8700018 do
		if role.GetRoleItemNum(RoleID,blue) > 0 then												--如果背包内有蓝色暗器
			local blueUp = blue + 6
			local blueDown = blue - 6
			local shibaiNum3 = math.floor(ChongzhuBlue/10000)											--获取蓝色暗器失败次数
			local bubianNum3 = math.floor(ChongzhuBlue/10)%1000										--获取蓝色暗器不变次数
			local shengjiNum3 = ChongzhuBlue%10														--获取蓝色暗器是否可直接升级
			if bubianNum3 >= 30 and shibaiNum3 >= 10 or shengjiNum3 >= 1 then							--如果不变次数大于等于15并且失败次数大于等于5或者可直接升级为1
				role.RemoveFromRole(MapID,InstanceID,RoleID,blue,1,420)								--删除旧有蓝色暗器
				role.AddRoleItem(MapID, InstanceID,RoleID,blueUp,1,4,8,420)							--添加橙色暗器
				role.SetRoleScriptData(RoleID,1,RoleDataType["ChongzhuBlue"],0)						--所有记录清0
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 100, 4301)
				msg.AddMsgEvent(MsgID, 2, RoleID)
				msg.AddMsgEvent(MsgID, 4, 6000224)
				msg.AddMsgEvent(MsgID, 4, blue)
				msg.AddMsgEvent(MsgID, 4, blueUp)
				msg.DispatchWorldMsgEvent(MsgID)
			elseif bubianNum3 < 30 or shibaiNum3 < 10 and shengjiNum3 == 0 then							--如果不变次数小于15或者失败次数小于5并且不可直接升级
				local temp3 = math.random(100)
				if temp3 <= 75 then
					role.RemoveFromRole(MapID,InstanceID,RoleID,blue,1,420)							--删除旧有蓝色暗器
					role.AddRoleItem(MapID,InstanceID,RoleID,blue,1,3,8,420)							--添加蓝色暗器
					role.SetRoleScriptData(RoleID,1,RoleDataType["ChongzhuBlue"],ChongzhuBlue+10)		--蓝色暗器不变次数加1
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 13, 0)
					msg.AddMsgEvent(MsgID, 1, 4302)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
					else
					role.RemoveFromRole(MapID,InstanceID,RoleID,blue,1,420)							--删除旧有蓝色暗器
					role.AddRoleItem(MapID,InstanceID,RoleID,blueDown,1,2,8,420)						--暗器降级
					role.SetRoleScriptData(RoleID,1,RoleDataType["ChongzhuBlue"],ChongzhuBlue+10000)	--蓝色暗器失败次数加1
					role.SetRoleScriptData(RoleID,1,RoleDataType["ChongzhuGreen"],ChongzhuGreen+1)	--降级后的暗器可直接升级
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 13, 0)
					msg.AddMsgEvent(MsgID, 1, 4303)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				end
			end
			return
		end
	end

	local ChongzhuOrange = role.GetRoleScriptData(RoleID,1,RoleDataType["ChongzhuOrange"]) 		 	--获取玩家橙色暗器重铸记录

	for orange = 8700019,8700024 do
		if role.GetRoleItemNum(RoleID,orange) > 0 then												--如果背包内有橙色暗器
			local orangeUp = orange + 6
			local orangeDown = orange - 6
			local shibaiNum4 = math.floor(ChongzhuOrange/10000)											--获取橙色暗器失败次数
			local bubianNum4 = math.floor(ChongzhuOrange/10)%1000										--获取橙色暗器不变次数
			local shengjiNum4 = ChongzhuOrange%10														--获取橙色暗器是否可直接升级
			if bubianNum4 >= 90 and shibaiNum4 >= 30 or shengjiNum4 >= 1 then							--如果不变次数大于等于15并且失败次数大于等于5或者可直接升级为1
				role.RemoveFromRole(MapID,InstanceID,RoleID,orange,1,420)								--删除旧有橙色暗器
				role.AddRoleItem(MapID, InstanceID,RoleID,orangeUp,1,5,8,420)							--添加紫色暗器
				role.SetRoleScriptData(RoleID,1,RoleDataType["ChongzhuOrange"],0)						--所有记录清0
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 100, 4301)
				msg.AddMsgEvent(MsgID, 2, RoleID)
				msg.AddMsgEvent(MsgID, 4, 6000224)
				msg.AddMsgEvent(MsgID, 4, orange)
				msg.AddMsgEvent(MsgID, 4, orangeUp)
				msg.DispatchWorldMsgEvent(MsgID)
			elseif bubianNum4 < 90 or shibaiNum4 < 30 and shengjiNum4 == 0 then							--如果不变次数小于45或者失败次数小于15并且不可直接升级
				local temp4 = math.random(100)
				if temp4 <= 75 then
					role.RemoveFromRole(MapID,InstanceID,RoleID,orange,1,420)							--删除旧有橙色暗器
					role.AddRoleItem(MapID,InstanceID,RoleID,orange,1,4,8,420)							--添加橙色暗器
					role.SetRoleScriptData(RoleID,1,RoleDataType["ChongzhuOrange"],ChongzhuOrange+10)		--橙色暗器不变次数加1
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 13, 0)
					msg.AddMsgEvent(MsgID, 1, 4302)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
					else
					role.RemoveFromRole(MapID,InstanceID,RoleID,orange,1,420)							--删除旧有橙色暗器
					role.AddRoleItem(MapID,InstanceID,RoleID,orangeDown,1,3,8,420)						--暗器降级
					role.SetRoleScriptData(RoleID,1,RoleDataType["ChongzhuOrange"],ChongzhuOrange+10000)	--橙色暗器失败次数加1
					role.SetRoleScriptData(RoleID,1,RoleDataType["ChongzhuBlue"],ChongzhuBlue+1)	--降级后的暗器可直接升级
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 13, 0)
					msg.AddMsgEvent(MsgID, 1, 4303)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				end
			end
			return
		end
	end

	for purple = 8700025,8700030 do
		if  role.GetRoleItemNum(RoleID,purple) > 0 then
			role.RemoveFromRole(MapID,InstanceID,RoleID,purple,1,420)								--删除旧有橙色暗器
			role.AddRoleItem(MapID, InstanceID,RoleID,purple,1,5,8,420)							--添加紫色暗器
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 13, 0)
			msg.AddMsgEvent(MsgID, 1, 4302)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return
		end
	end

end


aux.RegisterItemEvent(6000224, 1, "I6000224_OnUse")


function I6000224_CanUse(MapID, InstanceID, TypeID, RoleID)
    local bRet, bIgnore = 32, false
	local FreeSize = role.GetBagFreeSize(RoleID)
	for i = 8700001,8700030 do
		if 1 <= role.GetRoleItemNum(RoleID,i) then				--判断背包内是否有暗器
			bRet, bIgnore = 0, false
		end
	end
	if FreeSize < 1 then										--判断背包空间是否大于等于1
	    bRet, bIgnore = 40, false
	end
	return bRet, bIgnore
end
aux.RegisterItemEvent(6000224, 0, "I6000224_CanUse")
