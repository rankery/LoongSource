

--重阳节活动内容
Chongyang_Creature = {}
Chongyang_Creature[1]={typeid=3610232,x=2222,y=6646,z=2222,creid=0}
Chongyang_Creature[2]={typeid=3610233,x=2389,y=10295,z=3413,creid=0}
Chongyang_Creature[3]={typeid=3610234,x=1272,y=4971,z=2794,creid=0}
Chongyang_Creature[4]={typeid=3610235,x=847,y=3856,z=1950,creid=0}
Chongyang_Creature[5]={typeid=3610236,x=940,y=7370,z=407,creid=0}
Chongyang_Creature[6]={typeid=3610237,x=2465,y=3832,z=1051,creid=0}
Chongyang_Creature[7]={typeid=3610238,x=2389,y=10285,z=3423,creid=0}
Chongyang_Creature[8]={typeid=3610239,x=1278,y=4980,z=2802,creid=0}
Chongyang_Creature[9]={typeid=3610240,x=855,y=3907,z=1955,creid=0}
Chongyang_Creature[10]={typeid=3610241,x=930,y=7287,z=410,creid=0}
Chongyang_Creature[11]={typeid=3610242,x=2464,y=3819,z=1059,creid=0}
Chongyang_Creature[12]={typeid=3610243,x=2184,y=8095,z=2355,creid=0}
Chongyang_Creature[13]={typeid=3610244,x=2184,y=10775,z=2377,creid=0}
Chongyang_Creature[14]={typeid=3610245,x=2183,y=14167,z=2418,creid=0}

--任务ID ：20331-20335

function Chongyang_OnTimerMin(actID)

	if  Chongyangjie == 1 then
		local curmon = tonumber(os.date("%m"))
		local curday = tonumber(os.date("%d"))
		local curhour = tonumber(os.date("%H")) -- 获取服务器当前小时数
		local curmin = tonumber(os.date("%M"))  -- 获取服务器当前分钟数
		for i = 1, 12 do
			if  Chongyang_Creature[i].creid == 0 then
				Chongyang_Creature[i].creid =  map.MapCreateColCreature(3017298127, -1, Chongyang_Creature[i].typeid, Chongyang_Creature[i].x, Chongyang_Creature[i].y, Chongyang_Creature[i].z, 1)
			end
		end
		-- 每天18点初始郊游祭祖任务完成数量，删除 下凡仙人·乙 和 下凡仙人·丙
		if curhour == 18 and curmin == 0 then
			local k = jizurenwushuliang
			if k ~= 0 then
				jizurenwushuliang = 0
			end
			for i = 13, 14 do
				if Chongyang_Creature[i] ~= 0 then
					local MsgID = msg.BeginMsgEvent() -- 世界公告
					msg.AddMsgEvent(MsgID, 100, 4122) -- 下凡仙人·乙 和 下凡仙人·丙 已经离开龙城，通过完成郊游祭祖任务可以再次将他们召唤出来
					msg.DispatchWorldMsgEvent(MsgID)
					map.MapDeleteCreature(3017298127, -1, Chongyang_Creature[i].creid)
					Chongyang_Creature[i].creid = 0
				end
			end
		end
	end
end

function Chongyang_OnStart(actID)
	if Chongyangjie == 1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 4123)	--重阳节活动已经开始了，请各位玩家到龙城（2222,2222）找到重阳节使者了解活动内容。
		msg.DispatchWorldMsgEvent(MsgID)
	end
end

function Chongyang_OnEnd(actID)
	if Chongyangjie == 1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 4124)	--重阳节活动已经结束了，感谢大家的积极参与。
		msg.DispatchWorldMsgEvent(MsgID)
		for i = 1 , 12 do
			if  Chongyang_Creature[i].creid ~= 0 then
				map.MapDeleteCreature(3017298127, -1, Chongyang_Creature[i].creid)
				Chongyang_Creature[i].creid =0
			end
		end
		Chongyangjie = 0
	end
end

--注册该活动的相关事件
aux.RegisterActEvent(154, 4, "Chongyang_OnTimerMin")	--活动ID
aux.RegisterActEvent(154, 2, "Chongyang_OnStart")	--活动ID
aux.RegisterActEvent(154, 3, "Chongyang_OnEnd")		--活动ID
















