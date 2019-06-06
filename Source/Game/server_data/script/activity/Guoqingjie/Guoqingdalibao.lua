

--RoleDataType["guoqingdalibao_geren"]   = 153   --记录国庆节玩家开启国庆大礼包个数

Guoqingdalibao_jiedian = 0 --记录服务器中国庆大礼包打开总数
Guoqingdalibao_weishenqi = 0 -- 记录服务器中伪神器产出总数

function Guoqinglibao_PT(MapID, InstanceID, RoleID) -- 普通几率获得道具

	local  Level = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local k = math.random(1000)
	if  k <= 130 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 4022)   --屏显提示玩家获得道具xx
		msg.AddMsgEvent(MsgID, 4, 6000110)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		role.AddRoleItem(MapID, InstanceID, RoleID, 6000110, 1, -1, 8, 420) -- 贵重物资
	elseif
		k >=131 and k <= 346 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 4022)   --屏显提示玩家获得道具xx
		msg.AddMsgEvent(MsgID, 4, 6000111)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		role.AddRoleItem(MapID, InstanceID, RoleID, 6000111, 1, -1, 8, 420) -- 国庆礼花
	elseif
		k >=347 and k <= 562 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 4022)   --屏显提示玩家获得道具xx
		msg.AddMsgEvent(MsgID, 4, 6000112)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		role.AddRoleItem(MapID, InstanceID, RoleID, 6000112, 1, -1, 8, 420) -- 国庆礼花
	elseif
		k >= 563 and k<= 874 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 4023)   --屏显提示玩家获得经验xxx
		msg.AddMsgEvent(MsgID, 12, Level*50)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		role.AddRoleExp(MapID, InstanceID, RoleID, Level*50)
		role.AddRoleSilver(MapID, InstanceID, RoleID, Level*50, 102)
	else
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 4024)   --屏显提示玩家什么也没有获得
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	end
end


function I6000113_OnUse(MapID, InstanceID, TypeID, RoleID)
	local	kaiqilibao= role.GetRoleScriptData(RoleID, 1, RoleDataType["guoqingdalibao_geren"]) --记录玩家每日开启国庆礼包的数量
	local  Level = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local  Num = kaiqilibao +1
			role.SetRoleScriptData(RoleID, 1, RoleDataType["guoqingdalibao_geren"], Num)
			Guoqingdalibao_jiedian = Guoqingdalibao_jiedian + 1

	if Guoqingdalibao_jiedian == 250 then
		if Guoqingdalibao_weishenqi <=5 then -- 如果伪神器产出超过5把，则按照普通几率获得道具
			local a = math.random(6000100,6000107)
			local MsgID = msg.BeginMsgEvent()   -- 世界公告
			msg.AddMsgEvent(MsgID, 100, 4025)--玩家xx是本服今日第xx位打开国庆大礼包的玩家，恭喜他获得了xx
			msg.AddMsgEvent(MsgID, 2, RoleID)
			msg.AddMsgEvent(MsgID, 12, Guoqingdalibao_jiedian)
			msg.AddMsgEvent(MsgID, 4, a) --获得道具名称
			msg.DispatchWorldMsgEvent(MsgID)
			role.AddRoleItem(MapID, InstanceID, RoleID, a, 1, 4, 8, 420)
			Guoqingdalibao_weishenqi = Guoqingdalibao_weishenqi + 1
		else
			Guoqinglibao_PT(MapID, InstanceID, RoleID)
		end
	elseif
		Guoqingdalibao_jiedian%1000 == 0 then
		if Guoqingdalibao_weishenqi <=5 then -- 如果伪神器产出超过5把，则按照普通几率获得道具
			local b = math.random(6000100,6000107)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 4025)--玩家xx是本服今日第xx位打开国庆大礼包的玩家，恭喜他获得了xx
			msg.AddMsgEvent(MsgID, 2, RoleID)
			msg.AddMsgEvent(MsgID, 12, Guoqingdalibao_jiedian)
			msg.AddMsgEvent(MsgID, 4, b) --获得道具名称
			msg.DispatchWorldMsgEvent(MsgID)
			role.AddRoleItem(MapID, InstanceID, RoleID, b , 1, 4, 8, 420)
			Guoqingdalibao_weishenqi = Guoqingdalibao_weishenqi + 1
		else
			Guoqinglibao_PT(MapID, InstanceID, RoleID)
		end
	else
		if  Num%200 == 0 then
			if Guoqingdalibao_weishenqi <=5 then -- 如果伪神器产出超过5把，则按照普通几率获得道具
				local c = math.random(1000)
				if c <= 640 then
					local ca = math.random(6000100,6000107)
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 100, 4026)--玩家xx目前已经开启了xx个国庆大礼包，恭喜他获得了xx
					msg.AddMsgEvent(MsgID, 2, RoleID)
					msg.AddMsgEvent(MsgID, 12, Num)
					msg.AddMsgEvent(MsgID, 4, ca) --获得道具名称
					msg.DispatchWorldMsgEvent(MsgID)
					role.AddRoleItem(MapID, InstanceID, RoleID, ca, 1, 4, 8, 420)
					Guoqingdalibao_weishenqi = Guoqingdalibao_weishenqi + 1
				else
					local cb = math.random(3400001,3400004)
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 100, 4026)--玩家xx目前已经开启了xx个国庆大礼包，恭喜他获得了xx
					msg.AddMsgEvent(MsgID, 2, RoleID)
					msg.AddMsgEvent(MsgID, 12, Num)
					msg.AddMsgEvent(MsgID, 4, cb) --获得道具名称
					msg.DispatchWorldMsgEvent(MsgID)
					role.AddRoleItem(MapID, InstanceID, RoleID, cb, 1, 4, 8, 420)
				end
			end
		elseif Num%20 == 0 then
			local d = math.random(1000)
			if d <= 330 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 4022)   --屏显提示玩家获得道具
				msg.AddMsgEvent(MsgID, 4, 3300103)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				role.AddRoleItem(MapID, InstanceID, RoleID, 3300103, 1, -1, 8, 420) -- 仙界天劫令
			elseif
				d >= 331 and d <= 661 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 4022)   --屏显提示玩家获得道具
				msg.AddMsgEvent(MsgID, 4, 3300102)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				role.AddRoleItem(MapID, InstanceID, RoleID, 3300102, 1, -1, 8, 420) -- 仙界渡劫令
			else
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 4023)   --屏显提示玩家获得经验
				msg.AddMsgEvent(MsgID, 12, Level*50)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				role.AddRoleExp(MapID, InstanceID, RoleID, Level*50)
			end
		else
			Guoqinglibao_PT(MapID, InstanceID, RoleID)
		end
	end
end



function I6000113_CanUse(MapID, InstanceID, TypeID, RoleID)

	local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
	local t = map.IsInArea(3017299663, -1, RoleID, 10994)	--返回true或false
	local FreeSize = role.GetBagFreeSize(RoleID)
	if Guoqingjie == 1 then
		if Npcyuebing ~= 1 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 4013) -- 龙神阅兵尚未开始，请在龙神阅兵开始后，站在阅兵队伍附近使用
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
			return 32, false
		elseif t ~= true  then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 4014) -- 必须站在龙神阅兵队伍附近才可以使用
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
			return 32, false
		elseif (FreeSize < 1) then
			return 40, false
		else
			return 0, false
		end
	else 							--国庆节期间限制在固定时间和地点开启，国庆节之后，无限制。
		if (FreeSize < 1) then
			return 40, false
		else
			return 0, false
		end
	end
end


aux.RegisterItemEvent(6000113, 1, "I6000113_OnUse")
aux.RegisterItemEvent(6000113, 0, "I6000113_CanUse")


















































