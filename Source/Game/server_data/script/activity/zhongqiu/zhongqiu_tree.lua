


--与桂树对话
--RoleDataType["zhongqiu_yueliangshi"]     = 148  		--中秋节玩家已使用绑定月亮石的总数量

zhongqiu_kanfahenji = 0 	--记录当前服务器中，玩家已对桂树砍伐的痕迹数量，活动开启后，每日20:00将该值初始化

function GuiShu_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)

    --local curmon = tonumber(os.date("%m"))
	--local curday = tonumber(os.date("%d"))
	--local curhour = tonumber(os.date("%H")) -- 获取服务器当前小时数
	--local curmin = os.date("%M")  -- 获取服务器当前分钟数
	local	Num = role.GetRoleScriptData(RoleID, 1, RoleDataType["zhongqiu_yueliangshi"]) --记录玩家每日获得月亮石的个数
	local  Level = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local  Bebuff = role.IsRoleHaveBuff(MapID, InstanceID, RoleID,89999) -- 记录玩家获得月亮原石时间 24小时		8999980

	if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 3907)	-- "天选之人在哪里？"
		msg.AddMsgEvent(MsgID, 24, TargetID)
		msg.AddMsgEvent(MsgID, 21, 4)	-- 选项一
		msg.AddMsgEvent(MsgID, 1, 3908)	-- "试着砍一下（需要金色斧头，并确保背包有一定空间）"
		msg.AddMsgEvent(MsgID, 21, 5)	-- 选项二
		msg.AddMsgEvent(MsgID, 1, 3909)	-- "查看砍伐的痕迹"
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	end

	if TalkIndex == 4 then  -- 试着砍一下
			-- 对玩家背包中的金斧子进行判断
		if role.GetRoleItemNum(RoleID, 6000006) == 0 then -- 如果背包中金色斧头数量为0
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 71, 3910)	--提示:"您缺少道具金色斧头，无法进行砍伐，\n金色斧头可以从嫦娥处领取，或者从吴刚处购买”
			msg.DispatchRoleMsgEvent(RoleID,MsgID)
		else
			zhongqiu_kanfahenji = zhongqiu_kanfahenji + 1 	-- 每进行一次砍伐，砍伐次数都会 +1
			local a = math.floor(zhongqiu_kanfahenji/1000)				-- 	获取砍伐树木的千位数字
			local b = math.floor((zhongqiu_kanfahenji-a*1000)/100)		--	获取砍伐树木的百位数字
			local c = math.floor((zhongqiu_kanfahenji-a*1000-b*100)/10)	--	获取砍伐树木的十位数字
			local d = math.floor((zhongqiu_kanfahenji-a*1000-b*100-c*10))	--	获取砍伐树木的个位数字
			if zhongqiu_kanfahenji >= 10000 then
				local n = math.random(1,100)
				if n == 1 then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 26, 3911)   --屏显提示玩家获得道具
					msg.AddMsgEvent(MsgID, 4, 6000001) --获得道具名称
					msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 13, 0)
					msg.AddMsgEvent(MsgID, 1, 3911)--聊天信息栏显示玩家获得道具
					msg.AddMsgEvent(MsgID, 4, 6000001) --获得道具名称
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
					local MsgID = msg.BeginMsgEvent()   -- 世界公告
					msg.AddMsgEvent(MsgID, 100, 3922)--聊天信息栏显示玩家获得道具
					msg.AddMsgEvent(MsgID, 2, RoleID)
					msg.AddMsgEvent(MsgID, 4, 6000001) --获得道具名称
					msg.DispatchWorldMsgEvent(MsgID)
				role.AddRoleItem(MapID, InstanceID, RoleID, 6000001, 1, -1, 8, 420) 	-- 1%几率获得天选之人称号道具
				end
				local k = math.random(50)
				local l = math.random(600)
				local exp = Level*50
				if k == 1 and l ~= 1 then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 26, 3918)   --啊 不小心触怒了神树，遭到天罚了……
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
					unit.AddBuff(MapID, InstanceID, RoleID, 5005501 ,RoleID)
				elseif
					l == 1 and k ~= 1 then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 26, 3911)   --屏显提示玩家获得道具
					msg.AddMsgEvent(MsgID, 4, 3304163)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
					role.AddRoleItem(MapID, InstanceID, RoleID, 3304163, 1, -1, 8, 420) -- 添加低级守魂石  100宝
					local MsgID = msg.BeginMsgEvent()   -- 世界公告
					msg.AddMsgEvent(MsgID, 100, 3922)--聊天信息栏显示玩家获得道具
					msg.AddMsgEvent(MsgID, 2, RoleID)
					msg.AddMsgEvent(MsgID, 4, 3304163) --获得道具名称
					msg.DispatchWorldMsgEvent(MsgID)
				else
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 26, 3914)   --除了增加了一些经验，似乎没有什么其他的效果，试着再砍一下，也许会有惊喜
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
					role.AddRoleExp(MapID, InstanceID, RoleID, exp)
				end

			else
				--	当满足各个尾数条件时，获得道具及金钱
				if  a == 9 and b == 9 and c ==9 and d == 9  then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 26, 3912)   --屏显提示玩家获得道具及金钱
					msg.AddMsgEvent(MsgID, 12, 9999) -- 砍伐桂树的次数
					msg.AddMsgEvent(MsgID, 4, 6000002) --获得道具名称
					msg.AddMsgEvent(MsgID, 12, 50000) -- 获得金钱数量
					msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 13, 0)
					msg.AddMsgEvent(MsgID, 1, 3912)--聊天信息栏显示玩家获得道具及金钱
					msg.AddMsgEvent(MsgID, 12, 9999) -- 砍伐桂树的次数
					msg.AddMsgEvent(MsgID, 4, 6000002) --获得道具名称
					msg.AddMsgEvent(MsgID, 12, 50000) -- 获得金钱数量
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
					local MsgID = msg.BeginMsgEvent()   -- 世界公告
					msg.AddMsgEvent(MsgID, 100, 3919)--聊天信息栏显示玩家获得道具
					msg.AddMsgEvent(MsgID, 2, RoleID)
					msg.AddMsgEvent(MsgID, 12, 9999)
					msg.AddMsgEvent(MsgID, 4, 6000002) --获得道具名称
					msg.AddMsgEvent(MsgID, 12, 50000) -- 获得金钱数量
					msg.DispatchWorldMsgEvent(MsgID)
					role.AddRoleSilver(MapID, InstanceID, RoleID, 500000000, 102)	-- 获得50000金奖励
					role.AddRoleItem(MapID, InstanceID, RoleID, 6000002, 1, -1, 8, 420) -- 获得天选之人称号(3days)
				elseif
					a == 6 and b == 6 and c == 6 and d == 6 then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 26, 3912)
					msg.AddMsgEvent(MsgID, 12, 6666)
					msg.AddMsgEvent(MsgID, 4, 6000001)
					msg.AddMsgEvent(MsgID, 12, 50000)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 13, 0)
					msg.AddMsgEvent(MsgID, 1, 3912)
					msg.AddMsgEvent(MsgID, 12, 6666)
					msg.AddMsgEvent(MsgID, 4, 6000001)
					msg.AddMsgEvent(MsgID, 12, 50000)
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 100, 3919)--聊天信息栏显示玩家获得道具
					msg.AddMsgEvent(MsgID, 2, RoleID)
					msg.AddMsgEvent(MsgID, 12, 6666)
					msg.AddMsgEvent(MsgID, 4, 6000001) --获得道具名称
					msg.AddMsgEvent(MsgID, 12, 50000) -- 获得金钱数量
					msg.DispatchWorldMsgEvent(MsgID)
					role.AddRoleSilver(MapID, InstanceID, RoleID, 500000000, 102)	-- 获得50000金奖励
					role.AddRoleItem(MapID, InstanceID, RoleID, 6000001, 1, -1, 8, 420) -- 获得天选之人称号(1days)
				elseif
					a == 3 and b == 6 and c == 6 and d == 6 then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 26, 3913)
					msg.AddMsgEvent(MsgID, 12, 3666)
					msg.AddMsgEvent(MsgID, 12, 15000)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 13, 0)
					msg.AddMsgEvent(MsgID, 1, 3913)
					msg.AddMsgEvent(MsgID, 12, 3666)
					msg.AddMsgEvent(MsgID, 12, 15000)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 100, 3920)--聊天信息栏显示玩家获得道具
					msg.AddMsgEvent(MsgID, 2, RoleID)
					msg.AddMsgEvent(MsgID, 12, 3666)
					msg.AddMsgEvent(MsgID, 12, 15000) -- 获得金钱数量
					msg.DispatchWorldMsgEvent(MsgID)
					role.AddRoleSilver(MapID, InstanceID, RoleID, 150000000, 102)	-- 获得15000金奖励
				elseif
					a == 0 and b == 6 and c == 6 and d == 6 then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 26, 3913)
					msg.AddMsgEvent(MsgID, 12, 666)
					msg.AddMsgEvent(MsgID, 12, 5000)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 13, 0)
					msg.AddMsgEvent(MsgID, 1, 3913)
					msg.AddMsgEvent(MsgID, 12, 666)
					msg.AddMsgEvent(MsgID, 12, 5000)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 100, 3920)--聊天信息栏显示玩家获得道具
					msg.AddMsgEvent(MsgID, 2, RoleID)
					msg.AddMsgEvent(MsgID, 12, 666)
					msg.AddMsgEvent(MsgID, 12, 5000) -- 获得金钱数量
					msg.DispatchWorldMsgEvent(MsgID)
					role.AddRoleSilver(MapID, InstanceID, RoleID, 50000000, 102)	-- 获得5000金奖励
				elseif
					a == 0 and b == 0 and c == 6 and d == 6 then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 26, 3913)
					msg.AddMsgEvent(MsgID, 12, 66)
					msg.AddMsgEvent(MsgID, 12, 500)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 13, 0)
					msg.AddMsgEvent(MsgID, 1, 3913)
					msg.AddMsgEvent(MsgID, 12, 66)
					msg.AddMsgEvent(MsgID, 12, 500)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 100, 3920)--聊天信息栏显示玩家获得道具
					msg.AddMsgEvent(MsgID, 2, RoleID)
					msg.AddMsgEvent(MsgID, 12, 66)
					msg.AddMsgEvent(MsgID, 12, 500) -- 获得金钱数量
					msg.DispatchWorldMsgEvent(MsgID)
					role.AddRoleSilver(MapID, InstanceID, RoleID, 5000000, 102)	-- 获得500金奖励
				else
					local k = math.random(50)
					local l = math.random(600)
					local exp = Level*50
					if k == 1 and l ~= 1 then
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 26, 3918)   --啊 不小心触怒了神树，遭到天罚了……
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						unit.AddBuff(MapID, InstanceID, RoleID, 5005501 ,RoleID)
					elseif
						l == 1 and k ~= 1 then
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 26, 3911)   --屏显提示玩家获得道具
						msg.AddMsgEvent(MsgID, 4, 3304163)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						local MsgID = msg.BeginMsgEvent()   -- 世界公告
						msg.AddMsgEvent(MsgID, 100, 3922)--聊天信息栏显示玩家获得道具
						msg.AddMsgEvent(MsgID, 2, RoleID)
						msg.AddMsgEvent(MsgID, 4, 3304163) --获得道具名称
						msg.DispatchWorldMsgEvent(MsgID)
						role.AddRoleItem(MapID, InstanceID, RoleID, 3304163, 1, -1, 8, 420) -- 添加低级守魂石  100宝
					else
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 26, 3914)   --除了增加了一些经验，似乎没有什么其他的效果，试着再砍一下，也许会有惊喜
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						role.AddRoleExp(MapID, InstanceID, RoleID, exp)
					end
				end
			end
		--每当砍伐尾数为6时，获得道具
			if  d == 6 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 3915)
				msg.AddMsgEvent(MsgID, 4, 6000003)
				msg.AddMsgEvent(MsgID, 4, 6000004)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 13, 0)
				msg.AddMsgEvent(MsgID, 1, 3915)
				msg.AddMsgEvent(MsgID, 4, 6000003)
				msg.AddMsgEvent(MsgID, 4, 6000004)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				role.AddRoleItem(MapID, InstanceID, RoleID, 6000003, 1, -1, 8, 420) -- 添加桂树嫩枝
				role.AddRoleItem(MapID, InstanceID, RoleID, 6000004, 1, -1, 8, 420) -- 添加月光宝盒
			end

		--每天获得月亮石个数 不超过10个

				if Num <10  and Bebuff ~= true   then
				    local k = math.random(150)	-- 1.5%几率获得月光原石
				    if 	k == 1 then
					Num = Num +1
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 26, 3916)
					msg.AddMsgEvent(MsgID, 12, Num)
					msg.AddMsgEvent(MsgID, 4, 6000010)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 13, 0)
					msg.AddMsgEvent(MsgID, 1, 3916)
					msg.AddMsgEvent(MsgID, 12, Num)
					msg.AddMsgEvent(MsgID, 4, 6000010)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
					role.AddRoleItem(MapID, InstanceID, RoleID, 6000010, 1, -1, 8, 420) -- 1.5%几率获得月光原石
					role.SetRoleScriptData(RoleID, 1, RoleDataType["zhongqiu_yueliangshi"], Num) --角色获得月光原石数量 +1
				    end
				end
			if  Num == 10 then   	-- 每天晚上获得月光原石个数不能超过10个
				unit.AddBuff(MapID, InstanceID, RoleID, 8999980 ,RoleID) --给玩家添加24小时的BUFF，用于记录月亮原石获得时间
				role.SetRoleScriptData(RoleID, 1, RoleDataType["zhongqiu_yueliangshi"], 0)
			end
		role.RemoveFromRole(MapID, InstanceID, RoleID, 6000006, 1, 420)
		end
	end

	if TalkIndex == 5 then  -- 查看砍伐的痕迹
		local a =  math.floor(zhongqiu_kanfahenji/1000)					-- 	获取砍伐树木的千位数字
		local b =  math.floor((zhongqiu_kanfahenji-a*1000)/100)			--	获取砍伐树木的百位数字
		local c =  math.floor((zhongqiu_kanfahenji-a*1000-b*100)/10)		--	获取砍伐树木的十位数字
		local d =  math.floor((zhongqiu_kanfahenji-a*1000-b*100-c*10))	--	获取砍伐树木的个位数字
		if	a ~= 0 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 71, 3917)	--提示:"树干上隐隐约约有些痕迹，粗粗数了一下，大概有xxxx道以上”。--当砍伐次数为个位数时则显示精确数字
			msg.AddMsgEvent(MsgID, 12, a*1000)
			msg.DispatchRoleMsgEvent(RoleID,MsgID)
		elseif a == 0 and b ~= 0 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 71, 3917)	--提示:"树干上隐隐约约有些痕迹，粗粗数了一下，大概有xxxx道以上”。--当砍伐次数为个位数时则显示精确数字
			msg.AddMsgEvent(MsgID, 12, b*100)
			msg.DispatchRoleMsgEvent(RoleID,MsgID)
		elseif a == 0 and b == 0 and c ~= 0 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 71, 3917)	--提示:"树干上隐隐约约有些痕迹，粗粗数了一下，大概有xxxx道以上”。--当砍伐次数为个位数时则显示精确数字
			msg.AddMsgEvent(MsgID, 12, c*10)
			msg.DispatchRoleMsgEvent(RoleID,MsgID)
		else
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 71, 3917)	--提示:"树干上隐隐约约有些痕迹，粗粗数了一下，大概有xxxx道以上”。--当砍伐次数为个位数时则显示精确数字
			msg.AddMsgEvent(MsgID, 12, zhongqiu_kanfahenji)
			msg.DispatchRoleMsgEvent(RoleID,MsgID)
		end
	end
end

aux.RegisterCreatureEvent(3610221, 7, "GuiShu_OnTalk")










