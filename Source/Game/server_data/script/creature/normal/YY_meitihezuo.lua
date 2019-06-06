--1534206	小浣熊
--1534207	浣熊爸爸
--1534208	浣熊妈妈
--4900603	透明盒子1
--4900604	透明盒子2
--4005203	[圣]YY邪宠
--固定点刷怪：涿鹿 (3017299663, 1765, 12413, 2465)



function YY_xiaohuanxiong_OnRespawn(MapID, InstanceID, TargetID)

		local  MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 100, 363)--顽皮的小浣熊正在涿鹿[1765, 2465]大肆捣乱，请各路英雄前往驯服！
				msg.DispatchWorldMsgEvent(MsgID)
end
aux.RegisterCreatureEvent(1534206,1,"YY_xiaohuanxiong_OnRespawn")


function YY_xiaohuanxiong_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

		local  MsgID = msg.BeginMsgEvent()
		--##驯服了顽皮的小浣熊，愤怒的浣熊爸爸将在5分钟后出现
				msg.AddMsgEvent(MsgID, 100, 359)
				msg.AddMsgEvent(MsgID, 2, RoleID)
				msg.DispatchWorldMsgEvent(MsgID)
	    map.MapCreateCreature(3017299663, InstanceID, 4900603, 1765, 12413, 2465)
end

aux.RegisterCreatureEvent(1534206,4,"YY_xiaohuanxiong_OnDie")

function YY_tmhz_1_OnCreatureDisappear(MapID, InstanceID, TargetID, AI)
	    map.MapCreateCreature(3017299663, InstanceID, 1534207, 1765, 12413, 2465)
end

aux.RegisterCreatureEvent(4900603,13,"YY_tmhz_1_OnCreatureDisappear")


function YY_huanxiongbaba_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

		local  MsgID = msg.BeginMsgEvent()
		--##驯服了浣熊爸爸，暴躁的浣熊妈妈将在5分钟后出现
				msg.AddMsgEvent(MsgID, 100, 360)
				msg.AddMsgEvent(MsgID, 2, RoleID)
				msg.DispatchWorldMsgEvent(MsgID)
	    map.MapCreateCreature(3017299663, InstanceID, 4900604, 1765, 12413, 2465)
end

aux.RegisterCreatureEvent(1534207,4,"YY_huanxiongbaba_OnDie")

function YY_tmhz_2_OnCreatureDisappear(MapID, InstanceID, TargetID, AI)
	    map.MapCreateCreature(3017299663, InstanceID, 1534208, 1765, 12413, 2465)
end

aux.RegisterCreatureEvent(4900604,13,"YY_tmhz_2_OnCreatureDisappear")

function YY_huanxiongmama_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	    local a = role.GetBagFreeSize(RoleID)
	    if a < 1 then
				local  MsgID = msg.BeginMsgEvent()
		        msg.AddMsgEvent(MsgID, 100, 362) --##驯服了浣熊妈妈，但由于背包已满，难得一见的##跑丢了！
		        msg.AddMsgEvent(MsgID, 2, RoleID)
		        msg.AddMsgEvent(MsgID, 4, 4005203)
		        msg.DispatchWorldMsgEvent(MsgID)
        else
				local temp = math.random(1,10)
			if temp >= 5 then
				--50%几率获得
		        local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 100, 361) --##击败了浣熊妈妈，幸运的得到一只难得一见的##！
				msg.AddMsgEvent(MsgID, 2, RoleID)
				msg.AddMsgEvent(MsgID, 4, 4005203)
				msg.DispatchWorldMsgEvent(MsgID)
		        role.AddRoleItem(MapID, InstanceID, RoleID, 4005203, 1, -1, 3, 420)
			else
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 100, 365) --##击败了浣熊妈妈，遗憾的是，未能见到传说中的##！
				msg.AddMsgEvent(MsgID, 2, RoleID)
				msg.AddMsgEvent(MsgID, 4, 4005203)
				msg.DispatchWorldMsgEvent(MsgID)
		    end
		return 1
		end
end

aux.RegisterCreatureEvent(1534208,4,"YY_huanxiongmama_OnDie")


