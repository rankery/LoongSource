

--重阳节活动

--重阳节使者对话内容


jizurenwushuliang = 0

function Chongyangjieshizhe_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)

    if TalkIndex == -1 then
        local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 4101) -- "重阳节使者：\n 农历九月九日为传统的重阳节，又称“老人节”。因为《易经》中把“六”定为阴数，把“九”定为阳数，九月九日，日月并阳，两九相重，故而叫重阳，也叫重九。重阳节早在战国时期就已经形成，到了唐代，重阳被正式定为民间的节日，此后历朝历代沿袭至今。"
		msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
		msg.AddMsgEvent(MsgID, 21, 4) -- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 4102) -- “重阳节活动时间”
		msg.AddMsgEvent(MsgID, 21, 5) -- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 4103) -- "重阳节活动内容-郊游祭祖"
		msg.AddMsgEvent(MsgID, 21, 6)   --取消按钮
		msg.AddMsgEvent(MsgID, 1, 4104)   --"重阳节活动内容-登高寻仙"
		msg.AddMsgEvent(MsgID, 21, 7) -- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 4105) -- "查看任务完成环数"
		--msg.AddMsgEvent(MsgID, 21, 8)   --取消按钮
		--msg.AddMsgEvent(MsgID, 1, 4106)   --"仿-超神器（未开锋）"
		msg.AddMsgEvent(MsgID, 21, 9)   --取消按钮
		msg.AddMsgEvent(MsgID, 1, 4107)   --"接取任务：郊游祭祖（一）"
		msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
	end

	if TalkIndex == 4 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 4108) -- 2010年10月15日至2010年10月19日
		msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
		msg.AddMsgEvent(MsgID, 21, -1) -- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 4125) -- 返回
		msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
	end

	if TalkIndex == 5 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 4109) -- 每个角色在重阳活动开始以后，均可以在我这里接取系列任务“郊游祭祖”，该系列任务一共5环，接取每个任务时均需缴纳6金，完成每环任务均可获得不同数量（第1环1个，后面每环任务递增1个）的上古钱币（代币），可在登高寻仙活动中兑换各种奖励！注：服务器内完成任务的环数将被记录并影响登高寻仙的奖励，具体内容请查看登高寻仙的活动说明。
		msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
		msg.AddMsgEvent(MsgID, 21, -1) -- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 4125) -- 返回
		msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
	end

	if TalkIndex == 6 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 4110) -- 重阳活动开始以后，在龙城（2184，2355）会出现下凡先人，在他那里可以使用上古钱币兑换重阳糕（使用后立刻降低附近所有玩家5%体力，无起手，恢复时间30秒）或以10金的价格购买上古钱币。另外当服务器内完成“郊游祭祖”任务的环数达到100时将分别在龙城（坐标2184，2377）、（坐标2183，2418）出现下凡仙人（龙城最高建筑物的屋顶上），可分别使用上古钱币在他们那里兑换低级守魂石、中级守魂石、可附魂武器（100级橙色，未开锋）等各种珍稀道具（其中部分珍贵道具限量出售）！注：每日18:00或服务器维护时该所有次数会被重置，之前出现的下凡先人也会随之消失！
		msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
		msg.AddMsgEvent(MsgID, 21, -1) -- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 4125) -- 返回
		msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
	end

	if TalkIndex == 7 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 4111) -- 当前服务器内完成任务的总环数为X，每日18:00或服务器维护时环数会被重置！
		msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
		msg.AddMsgEvent(MsgID, 12, jizurenwushuliang) -- 完成任务总环数
		msg.AddMsgEvent(MsgID, 21, -1) -- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 4125) -- 返回
		msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
	end

	--[[if TalkIndex == 8 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 4112) -- 下凡先人出售的仿-超神器均尚未开锋，在您获得以后需要使用铸剑石进行开锋才能装备使用，铸剑石可在游戏商城内购买
		msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
		msg.AddMsgEvent(MsgID, 21, -1) -- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 4125) -- 返回
		msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		end]]--

	if TalkIndex == 9 then
		local a = role.GetRoleSilver(MapID, InstanceID,RoleID)
		local b = role.GetBagFreeSize(RoleID)
		if a < 60000 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 4113)   --金币数量不足，接取该任务需要消耗6金
			msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		elseif b < 1 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 4114)   --背包空间不足，无法接取该任务
			msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		else
			if role.IsRoleHaveQuest(MapID, InstanceID, RoleID, 20331)  == true then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 4126)   --你已经接取了该任务，无法再接取
				msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
			else
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 4115)   --接取任务：郊游祭祖（一）
				msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
				role.AddQuest(RoleID,20331) --添加任务：郊游祭祖（一）
				role.DecRoleSilver(MapID, InstanceID, RoleID, 60000, 101) -- 扣除6金
				--role.AddRoleItem(MapID, InstanceID, RoleID, 6000201, 1, -1, 8, 420) 	-- 添加龙脑香道具
			end
		end
	end
end

aux.RegisterCreatureEvent(3610232, 7, "Chongyangjieshizhe_OnTalk")


--完成郊游祭祖任务
--任务ID ：20331-20335

function Chongyang_Jizurenwu(MapID, InstanceID, QuestID, RoleID, TargetID)

	local k = jizurenwushuliang
	jizurenwushuliang = k + 1
	if jizurenwushuliang == 1000 then
		for i = 13, 14 do
			if Chongyang_Creature[i].creid == 0 then
				local MsgID = msg.BeginMsgEvent() -- 世界公告
				msg.AddMsgEvent(MsgID, 100, 4116) -- 下凡仙人·乙 和 下凡仙人·丙 已经分别出现于龙城 【2184,2377】 和 【2183,2418】, 请各位玩家速速赶去购买宝物
				msg.AddMsgEvent(MsgID, 2, RoleID)
				msg.DispatchWorldMsgEvent(MsgID)
				Chongyang_Creature[i].creid =  map.MapCreateColCreature(3017298127, -1, Chongyang_Creature[i].typeid, Chongyang_Creature[i].x, Chongyang_Creature[i].y, Chongyang_Creature[i].z, 1)
			end
		end
	end

end

aux.RegisterQuestEvent(20331, 1, "Chongyang_Jizurenwu")
aux.RegisterQuestEvent(20332, 1, "Chongyang_Jizurenwu")
aux.RegisterQuestEvent(20333, 1, "Chongyang_Jizurenwu")
aux.RegisterQuestEvent(20334, 1, "Chongyang_Jizurenwu")
aux.RegisterQuestEvent(20335, 1, "Chongyang_Jizurenwu")



-- 使用龙脑香道具

--龙脑香	在【2389，3413】的香炉附近使用	6000201
--龙脑香	在【1272，2794】的香炉附近使用	6000202
--龙脑香	在【847，1950】的香炉附近使用	6000203
--龙脑香	在【940，407】的香炉附近使用	6000204
--龙脑香	在【2465，1051】的香炉附近使用	6000205
--龙脑香灰烬	交还任务	6000207
--龙脑香灰烬	交还任务	6000208
--龙脑香灰烬	交还任务	6000209
--龙脑香灰烬	交还任务	6000210
--龙脑香灰烬	交还任务	6000211
--先祖护佑 3302701


function I6000201_CanUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(RoleID)
	local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
		return bRet, bIgnore
	elseif (x <= 2379 or x >= 2399) and  (z <= 3403 or z >=3423) then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 4117)  -- 请在龙城 [2389,3413]附近使用
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		bRet = -1
		return bRet, bIgnore
	else
		return 0, bIgnore
	end
end

function I6000201_OnUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local Level = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local exp = Level*50
	--unit.AddBuff(MapID, InstanceID, RoleID, 3322701 ,RoleID) -- 增加BUFF
	role.AddRoleItem(MapID, InstanceID, RoleID, 6000207, 1, -1, 8, 420) -- 获得龙脑灰烬
	role.AddRoleExp(MapID, InstanceID, RoleID, exp) -- 增加经验
end


function I6000202_CanUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(RoleID)
	local x , y , z = unit.GetPosition(MapID, InstanceID,RoleID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
		return bRet, bIgnore
	elseif (x <= 1262 or x >= 1282) and  (z <= 2784 or z >=2804) then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 4118)  -- 请在龙城 [1272,2794]附近使用
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		bRet = -1
		return bRet, bIgnore
	else
		return 0, bIgnore
	end
end

function I6000202_OnUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local Level = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local exp = Level*50
	--unit.AddBuff(MapID, InstanceID, TargetID, 3322701 ,RoleID) -- 增加BUFF
	role.AddRoleItem(MapID, InstanceID, RoleID, 6000208, 1, -1, 8, 420) -- 获得龙脑灰烬
	role.AddRoleExp(MapID, InstanceID, RoleID, exp) -- 增加经验
end


function I6000203_CanUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(RoleID)
	local x , y , z = unit.GetPosition(MapID, InstanceID,RoleID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
		return bRet, bIgnore
	elseif (x <= 837 or x >= 857) and  (z <= 1940 or z >=1960) then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 4119)  -- 请在龙城 [847,1950]附近使用
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		bRet = -1
		return bRet, bIgnore
	else
		return 0, bIgnore
	end
end

function I6000203_OnUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local Level = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local exp = Level*50
	--unit.AddBuff(MapID, InstanceID, RoleID, 3322701 ,RoleID) -- 增加BUFF
	role.AddRoleItem(MapID, InstanceID, RoleID, 6000209, 1, -1, 8, 420) -- 获得龙脑灰烬
	role.AddRoleExp(MapID, InstanceID, RoleID, exp) -- 增加经验
end

function I6000204_CanUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(RoleID)
	local x , y , z = unit.GetPosition(MapID, InstanceID,RoleID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
		return bRet, bIgnore
	elseif (x <= 930 or x >= 950) and  (z <= 397 or z >=417) then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 4120)  -- 请在龙城 [940,407]附近使用
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		bRet = -1
		return bRet, bIgnore
	else
		return 0, bIgnore
	end
end

function I6000204_OnUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local Level = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local exp = Level*50
	--unit.AddBuff(MapID, InstanceID, RoleID, 3322701 ,RoleID) -- 增加BUFF
	role.AddRoleItem(MapID, InstanceID, RoleID, 6000210, 1, -1, 8, 420) -- 获得龙脑灰烬
	role.AddRoleExp(MapID, InstanceID, RoleID, exp) -- 增加经验
end



function I6000205_CanUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(RoleID)
	local x , y , z = unit.GetPosition(MapID, InstanceID,RoleID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
		return bRet, bIgnore
	elseif (x <= 2455 or x >= 2475) and  (z <= 1041 or z >= 1061) then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 4121)  -- 请在龙城 [2465,1051]附近使用
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		bRet = -1
		return bRet, bIgnore
	else
		return 0, bIgnore
	end
end

function I6000205_OnUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local Level = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local exp = Level*50
	--unit.AddBuff(MapID, InstanceID, RoleID, 3322701 ,RoleID) -- 增加BUFF
	role.AddRoleItem(MapID, InstanceID, RoleID, 6000211, 1, -1, 8, 420) -- 获得龙脑灰烬
	role.AddRoleExp(MapID, InstanceID, RoleID, exp) -- 增加经验
end


aux.RegisterItemEvent(6000201, 0, "I6000201_CanUse")
aux.RegisterItemEvent(6000202, 0, "I6000202_CanUse")
aux.RegisterItemEvent(6000203, 0, "I6000203_CanUse")
aux.RegisterItemEvent(6000204, 0, "I6000204_CanUse")
aux.RegisterItemEvent(6000205, 0, "I6000205_CanUse")
aux.RegisterItemEvent(6000201, 1, "I6000201_OnUse")
aux.RegisterItemEvent(6000202, 1, "I6000202_OnUse")
aux.RegisterItemEvent(6000203, 1, "I6000203_OnUse")
aux.RegisterItemEvent(6000204, 1, "I6000204_OnUse")
aux.RegisterItemEvent(6000205, 1, "I6000205_OnUse")


--仿神器礼包
--[[
6000100	幽芒伏魔剑-仿
6000101	瑶光龙纹刃-仿
6000102	苍穹陨星戟-仿
6000103	碧鳞乾坤弓-仿
6000104	烈火磐涅印-仿
6000105	仙羽青霓卦-仿
6000106	银刺玄天手-仿
6000107	迷弦清音琴-仿
--]]

function I6000212_CanUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(RoleID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
		return bRet, bIgnore
	else
		return 0, bIgnore
	end
end


function I6000212_OnUse(MapID, InstanceID, TypeID, RoleID, TargetID)

	local k = math.random(100)
	if k <= 1 and k <= 5 then  --5%
		local MsgID = msg.BeginMsgEvent()   -- 世界公告
		msg.AddMsgEvent(MsgID, 100, 4127)--聊天信息栏显示玩家获得道具
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 4, 6000212) --获得道具名称
		msg.AddMsgEvent(MsgID, 4, 6000100) --获得道具名称
		msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, RoleID, 6000100, 1, -1, 8, 420) -- 获得仿神器
	elseif
		k >=6 and k <= 10 then  --5%
		local MsgID = msg.BeginMsgEvent()   -- 世界公告
		msg.AddMsgEvent(MsgID, 100, 4127)--聊天信息栏显示玩家获得道具
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 4, 6000212) --获得道具名称
		msg.AddMsgEvent(MsgID, 4, 6000101) --获得道具名称
		msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, RoleID, 6000101, 1, -1, 8, 420) -- 获得仿神器
	elseif
		k >=11 and k <= 30 then  --20%
		local MsgID = msg.BeginMsgEvent()   -- 世界公告
		msg.AddMsgEvent(MsgID, 100, 4127)--聊天信息栏显示玩家获得道具
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 4, 6000212) --获得道具名称
		msg.AddMsgEvent(MsgID, 4, 6000102) --获得道具名称
		msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, RoleID, 6000102, 1, -1, 8, 420) -- 获得仿神器
	elseif
		k >=31 and k <= 35 then  --5%
		local MsgID = msg.BeginMsgEvent()   -- 世界公告
		msg.AddMsgEvent(MsgID, 100, 4127)--聊天信息栏显示玩家获得道具
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 4, 6000212) --获得道具名称
		msg.AddMsgEvent(MsgID, 4, 6000103) --获得道具名称
		msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, RoleID, 6000103, 1, -1, 8, 420) -- 获得仿神器
	elseif
		k >=36 and k <= 55 then  --20%
		local MsgID = msg.BeginMsgEvent()   -- 世界公告
		msg.AddMsgEvent(MsgID, 100, 4127)--聊天信息栏显示玩家获得道具
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 4, 6000212) --获得道具名称
		msg.AddMsgEvent(MsgID, 4, 6000104) --获得道具名称
		msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, RoleID, 6000104, 1, -1, 8, 420) -- 获得仿神器
	elseif
		k >=56 and k <= 75 then  --20%
		local MsgID = msg.BeginMsgEvent()   -- 世界公告
		msg.AddMsgEvent(MsgID, 100, 4127)--聊天信息栏显示玩家获得道具
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 4, 6000212) --获得道具名称
		msg.AddMsgEvent(MsgID, 4, 6000105) --获得道具名称
		msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, RoleID, 6000105, 1, -1, 8, 420) -- 获得仿神器
	elseif
		k >=76 and k <= 80 then  -- 5%
		local MsgID = msg.BeginMsgEvent()   -- 世界公告
		msg.AddMsgEvent(MsgID, 100, 4127)--聊天信息栏显示玩家获得道具
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 4, 6000212) --获得道具名称
		msg.AddMsgEvent(MsgID, 4, 6000106) --获得道具名称
		msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, RoleID, 6000106, 1, -1, 8, 420) -- 获得仿神器
	else	--20%
		local MsgID = msg.BeginMsgEvent()   -- 世界公告
		msg.AddMsgEvent(MsgID, 100, 4127)--聊天信息栏显示玩家获得道具
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 4, 6000212) --获得道具名称
		msg.AddMsgEvent(MsgID, 4, 6000107) --获得道具名称
		msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, RoleID, 6000107, 1, -1, 8, 420) -- 获得仿神器
	end
end


--<Msg id="4127" name="chongyangjie_27" desc="恭喜玩家&lt;p1&gt;打开&lt;p2&gt;，获得了&lt;p3&gt;"/>

function I3303031_OnUse(MapID, InstanceID, TypeID, RoleID, TargetID)
		local MsgID = msg.BeginMsgEvent()   -- 世界公告
		msg.AddMsgEvent(MsgID, 100, 4127)--聊天信息栏显示玩家获得道具
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 4, 3303031) --获得道具名称
		msg.AddMsgEvent(MsgID, 4, 3303031) --获得道具名称
		msg.DispatchWorldMsgEvent(MsgID)
end

aux.RegisterItemEvent(6000212, 0, "I6000212_CanUse")
aux.RegisterItemEvent(6000212, 1, "I6000212_OnUse")
aux.RegisterItemEvent(3303031, 1, "I3303031_OnUse")    --3303031   修改1【盛世饺子】































