

--中秋节活动开关

--3610220	嫦娥
--3610221	桂树
--3610222	吴刚


Zhongqiu_Creature = {}
Zhongqiu_Creature[1] = { TypeID=3610220,x=1000,y=20140,z=969 ,CreID=0}
Zhongqiu_Creature[2] = { TypeID=3610221,x=976,y=20140,z=969 ,CreID=0}
Zhongqiu_Creature[3] = { TypeID=3610222,x=955,y=20140,z=970 ,CreID=0}

yueguangbaohe = 0		-- 开启月光宝盒 全局变量


function Zhongqiu_OnTimerMin(actID)

	if 	Zhongqiujie == 1 then

		local curmon = tonumber(os.date("%m"))
		local curday = tonumber(os.date("%d"))
		local curhour = tonumber(os.date("%H")) -- 获取服务器当前小时数
		local curmin = tonumber(os.date("%M"))  -- 获取服务器当前分钟数


		if (curmon == 9 and curday == 22) and (curhour == 19 and curmin == 55) then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 3902)	--再过5分钟，月光宝盒就可以开启了，请各位玩家做好准备
		msg.DispatchWorldMsgEvent(MsgID)
		end

		if (curmon == 9 and curday > 22) and (curhour == 18 and curmin == 55) then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 3902)	--再过5分钟，月光宝盒就可以开启了，请各位玩家做好准备
		msg.DispatchWorldMsgEvent(MsgID)
		end
		if (curmon == 9 and curday >= 22) and (curhour == 20 and curmin == 55) then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 3903)	--月光宝盒的开启时间还剩下5分钟，请各位玩家抓紧时间
		msg.DispatchWorldMsgEvent(MsgID)
		end
		for i = 1 , 3 do
			if Zhongqiu_Creature[i].CreID == 0 then
			Zhongqiu_Creature[1].CreID =  map.MapCreateColCreature(3017299919, -1, Zhongqiu_Creature[1].TypeID, Zhongqiu_Creature[1].x, Zhongqiu_Creature[1].y, Zhongqiu_Creature[1].z, 1)
			Zhongqiu_Creature[2].CreID =  map.MapCreateColCreature(3017299919, -1, Zhongqiu_Creature[2].TypeID, Zhongqiu_Creature[2].x, Zhongqiu_Creature[2].y, Zhongqiu_Creature[2].z, 1)
			Zhongqiu_Creature[3].CreID =  map.MapCreateColCreature(3017299919, -1, Zhongqiu_Creature[3].TypeID, Zhongqiu_Creature[3].x, Zhongqiu_Creature[3].y, Zhongqiu_Creature[3].z, 1)
			end
		end

	end

end

function Zhongqiu_OnStart(actID)
	if Zhongqiujie == 1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 3904)	--中秋节活动已经开始了,请各位玩家到巫水找到：嫦娥[1000,969]、吴刚[955，970]、桂树[976，969]与他们共度佳节；
		msg.DispatchBroadcast(MsgID, -1, -1, -1)
		Zhongqiu_Creature[1].CreID =  map.MapCreateColCreature(3017299919, -1, Zhongqiu_Creature[1].TypeID, Zhongqiu_Creature[1].x, Zhongqiu_Creature[1].y, Zhongqiu_Creature[1].z, 1)
		Zhongqiu_Creature[2].CreID =  map.MapCreateColCreature(3017299919, -1, Zhongqiu_Creature[2].TypeID, Zhongqiu_Creature[2].x, Zhongqiu_Creature[2].y, Zhongqiu_Creature[2].z, 1)
		Zhongqiu_Creature[3].CreID =  map.MapCreateColCreature(3017299919, -1, Zhongqiu_Creature[3].TypeID, Zhongqiu_Creature[3].x, Zhongqiu_Creature[3].y, Zhongqiu_Creature[3].z, 1)
	end
end

function Zhongqiu_OnEnd(actID)
	if Zhongqiujie == 1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 3905)	--中秋节活动已经结束了,感谢大家的参与。
		msg.DispatchWorldMsgEvent(MsgID)
		for i = 1 , 3 do
			if Zhongqiu_Creature[i].CreID ~= 0 then

				map.MapDeleteCreature(3017299919, -1, Zhongqiu_Creature[i].CreID)
				Zhongqiu_Creature[i].CreID =0
			end
		end
		Zhongqiujie = 0
	end
end

--注册该活动的相关事件
aux.RegisterActEvent(152, 4, "Zhongqiu_OnTimerMin")	--活动ID
aux.RegisterActEvent(152, 2, "Zhongqiu_OnStart")	--活动ID
aux.RegisterActEvent(152, 3, "Zhongqiu_OnEnd")		--活动ID




-- 使用月亮原石

function Yueliangshi_CanUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(RoleID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
		return bRet, bIgnore
	elseif yueguangbaohe == 0 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3906)  -- 提示玩家月亮原石只能在活动期间，每晚的19:00-21:00可以使用
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		bRet = -1
		return bRet, bIgnore
	else
		if yueguangbaohe == 1 then
		return 0, bIgnore
		end
	end
end

function Yueliangshi_OnUse(MapID, InstanceID, TypeID, RoleID, TargetID)

role.AddRoleItem(MapID, InstanceID, RoleID, 6000005, 1, -1, 8, 420) -- 获得月亮石

end


aux.RegisterItemEvent(6000010, 0, "Yueliangshi_CanUse")
aux.RegisterItemEvent(6000010, 1, "Yueliangshi_OnUse")
























