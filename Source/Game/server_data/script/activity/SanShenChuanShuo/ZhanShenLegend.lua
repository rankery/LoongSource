
--用于标记玩家在本次活动中是否已完成过任务的全局table，以玩家RoleID为索引，对应记录不同玩家
ZhanShenLegend_Quest_IsComplete = {};
--记录每次活动中任务被完成的总次数
ZhanShenLegend_CompleteQuestTime = 0;
--各个城市被刷出的活动NPC“天界战神”的CreatureID
ZhanShen_CreatureID = {};
--设置总共会有多少个NPC被刷出
ZhanShen_Quantity = 3;
--设置每个被刷出的NPC的信息
ZhanShen_CreatureID[1] = {MapID=3017298127,x=2184,y=6940,z=2289,CreatureID=0};	--龙城:[2184,6940,2289]
ZhanShen_CreatureID[2] = {MapID=3017298383,x=888,y=10464,z=2033,CreatureID=0};	--凤翔:[888,10464,2033]
ZhanShen_CreatureID[3] = {MapID=3017299663,x=2105,y=18338,z=2698,CreatureID=0};--涿鹿:[2105,18338,2698]

ZhanShenLegend_TimerMin = 0;		--记录活动进行了多长时间

--战神传说任务奖励，用于给特殊名次完成任务的玩家特殊奖励
ZhanShen_Reward = {};
ZhanShen_Reward[20229]={Exp=5751,Gold=675};
ZhanShen_Reward[20230]={Exp=9165,Gold=1200};
ZhanShen_Reward[20231]={Exp=14777,Gold=1875};
ZhanShen_Reward[20232]={Exp=24024,Gold=2700};
ZhanShen_Reward[20233]={Exp=38902,Gold=3675};
ZhanShen_Reward[20234]={Exp=63892,Gold=4800};
ZhanShen_Reward[20235]={Exp=108787,Gold=6075};
ZhanShen_Reward[20236]={Exp=167014,Gold=7500};
ZhanShen_Reward[20237]={Exp=232394,Gold=9075};
ZhanShen_Reward[20238]={Exp=287856,Gold=10800};
ZhanShen_Reward[20239]={Exp=316843,Gold=12675};
ZhanShen_Reward[20240]={Exp=333937,Gold=14700};
ZhanShen_Reward[20241]={Exp=348498,Gold=16875};
ZhanShen_Reward[20242]={Exp=404271,Gold=19200};
ZhanShen_Reward[20243]={Exp=560003,Gold=21675};
ZhanShen_Reward[20244]={Exp=721481,Gold=24300};
ZhanShen_Reward[20245]={Exp=820832,Gold=27075};
ZhanShen_Reward[20323]={Exp=1020832,Gold=37075};
ZhanShen_Reward[20324]={Exp=1270832,Gold=47075};

function ZhanShenLegend_OnStart(actID)	--活动开始

	--初始化活动全局变量
	ZhanShenLegend_TimerMin = 0;
	ZhanShenLegend_CompleteQuestTime = 0;

	--刷出活动NPC 天界战神[3610204]
	for i=1,ZhanShen_Quantity do
		ZhanShen_CreatureID[i].CreatureID = map.MapCreateCreature(ZhanShen_CreatureID[i].MapID, -1, 3610204, ZhanShen_CreatureID[i].x, ZhanShen_CreatureID[i].y, ZhanShen_CreatureID[i].z);
	end

	--活动开始播放广播：
	--“本次战神传说活动开始，各位玩家可在玄天龙城、凤翔府、黄帝城内找到"天界战神"接取活动任务，完成后会有丰厚的奖励！”
	local MsgID = msg.BeginMsgEvent();
	if WorldCup_On== 0 then
		msg.AddMsgEvent(MsgID,102,100047)
	else
		msg.AddMsgEvent(MsgID,102,100120)
	end
	msg.DispatchBroadcast(MsgID,-1,-1,-1);
end



function ZhanShenLegend_OnTimerMin(actID)	--活动每经过1分钟
	ZhanShenLegend_TimerMin = ZhanShenLegend_TimerMin+1;
	local isRestart = 0;
	for i=1,ZhanShen_Quantity do
		if ZhanShen_CreatureID[i].CreatureID == 0 then
			ZhanShen_CreatureID[i].CreatureID = map.MapCreateCreature(ZhanShen_CreatureID[i].MapID, -1, 3610204, ZhanShen_CreatureID[i].x, ZhanShen_CreatureID[i].y, ZhanShen_CreatureID[i].z);
			isRestart = isRestart+1;
			if isRestart==3 then
				local MsgID = msg.BeginMsgEvent();
				if WorldCup_On== 0 then
					msg.AddMsgEvent(MsgID,102,100047)
				else
					msg.AddMsgEvent(MsgID,102,100120)
				end
				msg.DispatchBroadcast(MsgID,-1,-1,-1);
				isRestart = 0;
			end
		end
	end
	if ZhanShenLegend_TimerMin==5 then
		local MsgID = msg.BeginMsgEvent();
		msg.AddMsgEvent(MsgID,102,100048);
		msg.DispatchBroadcast(MsgID,-1,-1,-1);
		for i=1,ZhanShen_Quantity do
			if  ZhanShen_CreatureID[i].CreatureID ~= nil then
				cre.MonsterSay(ZhanShen_CreatureID[i].MapID, -1, ZhanShen_CreatureID[i].CreatureID, 20124)
			end
		end
	elseif ZhanShenLegend_TimerMin==10 then
		local MsgID = msg.BeginMsgEvent();
		msg.AddMsgEvent(MsgID,102,100048);
		msg.DispatchBroadcast(MsgID,-1,-1,-1);
	elseif ZhanShenLegend_TimerMin==50 then
	--活动进行到50分钟时播放广播：
	--“天界战神”将在10分钟后返回天庭，需要交付任务的玩家请尽快前往，以免因错过时间而占用下次活动的任务名额。
		local MsgID = msg.BeginMsgEvent();
		msg.AddMsgEvent(MsgID,102,100060);
		msg.AddMsgEvent(MsgID,5,3610204);
		msg.AddMsgEvent(MsgID,9,10);
		msg.DispatchBroadcast(MsgID,-1,-1,-1);
	elseif ZhanShenLegend_TimerMin==55 then
	--活动进行到55分钟时
	--播放广播：
	--“天界战神”将在5分钟后返回天庭，需要交付任务的玩家请尽快前往，以免因错过时间而占用下次活动的任务名额。
	--天界战神喊话：
	--“本座即将返回天界复命，若不能在本座离去前交付任务，便只能等本座再次下凡，占用下回的任务限额了！”
		local MsgID = msg.BeginMsgEvent();
		msg.AddMsgEvent(MsgID,102,100060);
		msg.AddMsgEvent(MsgID,5,3610204);
		msg.AddMsgEvent(MsgID,9,5);
		msg.DispatchBroadcast(MsgID,-1,-1,-1);
		for i=1,ZhanShen_Quantity do
			if  ZhanShen_CreatureID[i].CreatureID ~= nil then
				cre.MonsterSay(ZhanShen_CreatureID[i].MapID, -1, ZhanShen_CreatureID[i].CreatureID, 20120)
			end
		end
	--在活动的第10-55分钟期间，每3-7分钟天界战神一定几率随机在[20123 ~ 20126]中间随进选择一条内容进行喊话
	elseif ZhanShenLegend_TimerMin>10 and ZhanShenLegend_TimerMin<55 and math.mod(ZhanShenLegend_TimerMin,math.random(3,7))==0 then
		local r = math.random(1,4);
		if r==1 then
			for i=1,ZhanShen_Quantity do
				if  ZhanShen_CreatureID[i].CreatureID ~= nil then
					cre.MonsterSay(ZhanShen_CreatureID[i].MapID, -1, ZhanShen_CreatureID[i].CreatureID, 20123)
				end
			end
		elseif r==2 then
			for i=1,ZhanShen_Quantity do
				if  ZhanShen_CreatureID[i].CreatureID ~= nil then
					cre.MonsterSay(ZhanShen_CreatureID[i].MapID, -1, ZhanShen_CreatureID[i].CreatureID, 20124)
				end
			end
		elseif r==3 then
			for i=1,ZhanShen_Quantity do
				if  ZhanShen_CreatureID[i].CreatureID ~= nil then
					cre.MonsterSay(ZhanShen_CreatureID[i].MapID, -1, ZhanShen_CreatureID[i].CreatureID, 20125)
				end
			end
		elseif r==4 then
			for i=1,ZhanShen_Quantity do
				if  ZhanShen_CreatureID[i].CreatureID ~= nil then
					cre.MonsterSay(ZhanShen_CreatureID[i].MapID, -1, ZhanShen_CreatureID[i].CreatureID, 20126)
				end
			end
		end
	end
end



function ZhanShenLegend_OnEnd(actID)	--活动结束

	--判断活动NPC是否已达到生存时间自动消失（若未消失则在场景中删除该NPC），并释放NPC的CreatureID
	for i=1,ZhanShen_Quantity do
		if ZhanShen_CreatureID[i].CreatureID ~= nil and unit.IsDead(ZhanShen_CreatureID[i].MapID,-1,ZhanShen_CreatureID[i].CreatureID)==false then
			map.MapDeleteCreature(ZhanShen_CreatureID[i].MapID,-1,ZhanShen_CreatureID[i].CreatureID);
			ZhanShen_CreatureID[i].CreatureID = nil;
		else
			ZhanShen_CreatureID[i].CreatureID = nil;
		end
	end

	ZhanShenLegend_Quest_IsComplete = {};
	ZhanShenLegend_CompleteQuestTime = 0;
	ZhanShenLegend_TimerMin = 0;
	--活动结束播放广播：
	--“本次战神传说活动已经结束，尚未完成任务的玩家可继续进行任务，并在下次活动时交付任务！”
	local MsgID = msg.BeginMsgEvent();
	msg.AddMsgEvent(MsgID,102,100049);
	msg.DispatchBroadcast(MsgID,-1,-1,-1);
end

function ZhanShenLegend_OnComplete(MapID, InstanceID, QuestID, RoleID, NPCID)	--玩家完成任务
	ZhanShenLegend_Quest_IsComplete[RoleID] = 1;
	ZhanShenLegend_CompleteQuestTime = ZhanShenLegend_CompleteQuestTime + 1;

	if WorldCup_On==1 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 3304100, 1, -1, 8, 420)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 71, 3270)		--世界杯期间额外获得助威号奖励
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	end

	local MsgID = msg.BeginMsgEvent();
	msg.AddMsgEvent(MsgID, 13, 0);
	msg.AddMsgEvent(MsgID, 1, 2730);
	msg.AddMsgEvent(MsgID, 11, QuestID);
	if ZhanShenLegend_CompleteQuestTime==1 then	--玩家第一个完成任务，额外给予任务的1倍经验和金钱奖励
		role.AddRoleExp(MapID, InstanceID, RoleID, ZhanShen_Reward[QuestID].Exp);
		role.AddRoleSilver(MapID, InstanceID, RoleID, ZhanShen_Reward[QuestID].Gold, 102);
		local MsgID_1 = msg.BeginMsgEvent();
		local MsgID_2 = msg.BeginMsgEvent();
		msg.AddMsgEvent(MsgID_1, 13, 0);
		msg.AddMsgEvent(MsgID_1, 1, 2721);
		msg.DispatchRoleMsgEvent(RoleID, MsgID_1);
		msg.AddMsgEvent(MsgID_2, 102, 100050);
		msg.AddMsgEvent(MsgID_2, 2, RoleID);
		msg.DispatchBroadcast(MsgID_2,-1,-1,-1);
		msg.AddMsgEvent(MsgID, 9, ZhanShen_Reward[QuestID].Exp*2);
		msg.AddMsgEvent(MsgID, 9, ZhanShen_Reward[QuestID].Gold*2);
		--增加活动进阶实力值
		Increase_shili(MapID, InstanceID, RoleID, 0, 12,"jinjieshili_A")
	elseif ZhanShenLegend_CompleteQuestTime==10 then	--玩家第10个完成任务，额外给予任务的0.5倍经验和金钱奖励
		local ex = math.floor(ZhanShen_Reward[QuestID].Exp/2);
		local go = math.floor(ZhanShen_Reward[QuestID].Gold/2);
		role.AddRoleExp(MapID, InstanceID, RoleID, ex);
		role.AddRoleSilver(MapID, InstanceID, RoleID, go, 102);
		local MsgID_1 = msg.BeginMsgEvent();
		local MsgID_2 = msg.BeginMsgEvent();
		msg.AddMsgEvent(MsgID_1, 13, 0);
		msg.AddMsgEvent(MsgID_1, 1, 2722);
		msg.AddMsgEvent(MsgID_1, 9, 10);
		msg.DispatchRoleMsgEvent(RoleID, MsgID_1);
		msg.AddMsgEvent(MsgID_2, 102, 100051);
		msg.AddMsgEvent(MsgID_2, 2, RoleID);
		msg.DispatchBroadcast(MsgID_2,-1,-1,-1);
		msg.AddMsgEvent(MsgID, 9, ZhanShen_Reward[QuestID].Exp+ex);
		msg.AddMsgEvent(MsgID, 9, ZhanShen_Reward[QuestID].Gold+go);
		--增加活动进阶实力值
		Increase_shili(MapID, InstanceID, RoleID, 0, 9,"jinjieshili_A")
	elseif ZhanShenLegend_CompleteQuestTime==100 then	--玩家第100个完成任务，额外给予任务的0.5倍经验和金钱奖励
		local ex = math.floor(ZhanShen_Reward[QuestID].Exp/2);
		local go = math.floor(ZhanShen_Reward[QuestID].Gold/2);
		role.AddRoleExp(MapID, InstanceID, RoleID, ex);
		role.AddRoleSilver(MapID, InstanceID, RoleID, go, 102);
		local MsgID_1 = msg.BeginMsgEvent();
		local MsgID_2 = msg.BeginMsgEvent();
		msg.AddMsgEvent(MsgID_1, 13, 0);
		msg.AddMsgEvent(MsgID_1, 1, 2722);
		msg.AddMsgEvent(MsgID_1, 9, 100);
		msg.DispatchRoleMsgEvent(RoleID, MsgID_1);
		msg.AddMsgEvent(MsgID_2, 102, 100052);
		msg.AddMsgEvent(MsgID_2, 2, RoleID);
		msg.DispatchBroadcast(MsgID_2,-1,-1,-1);
		msg.AddMsgEvent(MsgID, 9, ZhanShen_Reward[QuestID].Exp+ex);
		msg.AddMsgEvent(MsgID, 9, ZhanShen_Reward[QuestID].Gold+go);
		--增加活动进阶实力值
		Increase_shili(MapID, InstanceID, RoleID, 0, 9,"jinjieshili_A")
	else
		msg.AddMsgEvent(MsgID, 9, ZhanShen_Reward[QuestID].Exp);
		msg.AddMsgEvent(MsgID, 9, ZhanShen_Reward[QuestID].Gold);
		--增加活动进阶实力值
		Increase_shili(MapID, InstanceID, RoleID, 0, 6,"jinjieshili_A")
	end
	msg.DispatchRoleMsgEvent(RoleID, MsgID);
	--玩家交付任务时，NPC有50%几率随机[20121 ~ 20122]中随机一条喊话
	local r = math.random(1,4);
	if r==1 then
		for i=1,ZhanShen_Quantity do
			if ZhanShen_CreatureID[i].MapID == MapID and ZhanShen_CreatureID[i].CreatureID ~= nil then
				cre.MonsterSay(ZhanShen_CreatureID[i].MapID, -1, ZhanShen_CreatureID[i].CreatureID, 20121);
			end
		end
	elseif r==2 then
		for i=1,ZhanShen_Quantity do
			if ZhanShen_CreatureID[i].MapID == MapID and ZhanShen_CreatureID[i].CreatureID ~= nil then
				cre.MonsterSay(ZhanShen_CreatureID[i].MapID, -1, ZhanShen_CreatureID[i].CreatureID, 20122);
			end
		end
	end

end


function ZhanShenLegend_OnCheckAccept(MapID, InstanceID, QuestID, RoleID, NPCID)
	if ZhanShenLegend_Quest_IsComplete[RoleID] == 1 then
		local MsgID_1 = msg.BeginMsgEvent();
		local MsgID_2 = msg.BeginMsgEvent();
		msg.AddMsgEvent(MsgID_1, 26, 2723);
		msg.DispatchRoleMsgEvent(RoleID, MsgID_1);
		msg.AddMsgEvent(MsgID_2, 26, 2724);
		msg.DispatchRoleMsgEvent(RoleID, MsgID_2);

		return 0;
	end

	return 1;
end

function ZhanShenLegend_OnAccept(MapID, InstanceID, QuestID, RoleID, NPCID)

	ZhanShenLegend_Quest_IsComplete[RoleID] = 1;

end

function ZhanShenLegend_OnCheckComplete(MapID, InstanceID, QuestID, RoleID, NPCID)

	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(RoleID)
	if(FreeSize < 3+WorldCup_On) then
		--提示玩家背包空间不足
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3250)--需要x格行囊空间
		msg.AddMsgEvent(MsgID, 9, 3+WorldCup_On)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end
	return 1
end

--[[________________________注册活动相关事件________________________]]--

--活动开始
aux.RegisterActEvent(111, 2, "ZhanShenLegend_OnStart")
aux.RegisterActEvent(112, 2, "ZhanShenLegend_OnStart")
aux.RegisterActEvent(113, 2, "ZhanShenLegend_OnStart")
aux.RegisterActEvent(114, 2, "ZhanShenLegend_OnStart")

--活动结束
aux.RegisterActEvent(111, 3, "ZhanShenLegend_OnEnd")
aux.RegisterActEvent(112, 3, "ZhanShenLegend_OnEnd")
aux.RegisterActEvent(113, 3, "ZhanShenLegend_OnEnd")
aux.RegisterActEvent(114, 3, "ZhanShenLegend_OnEnd")

--活动以“分钟”为单位计时
aux.RegisterActEvent(111, 4, "ZhanShenLegend_OnTimerMin")
aux.RegisterActEvent(112, 4, "ZhanShenLegend_OnTimerMin")
aux.RegisterActEvent(113, 4, "ZhanShenLegend_OnTimerMin")
aux.RegisterActEvent(114, 4, "ZhanShenLegend_OnTimerMin")

--玩家接取
aux.RegisterQuestEvent(20229, 0, "ZhanShenLegend_OnAccept")
aux.RegisterQuestEvent(20230, 0, "ZhanShenLegend_OnAccept")
aux.RegisterQuestEvent(20231, 0, "ZhanShenLegend_OnAccept")
aux.RegisterQuestEvent(20232, 0, "ZhanShenLegend_OnAccept")
aux.RegisterQuestEvent(20233, 0, "ZhanShenLegend_OnAccept")
aux.RegisterQuestEvent(20234, 0, "ZhanShenLegend_OnAccept")
aux.RegisterQuestEvent(20235, 0, "ZhanShenLegend_OnAccept")
aux.RegisterQuestEvent(20236, 0, "ZhanShenLegend_OnAccept")
aux.RegisterQuestEvent(20237, 0, "ZhanShenLegend_OnAccept")
aux.RegisterQuestEvent(20238, 0, "ZhanShenLegend_OnAccept")
aux.RegisterQuestEvent(20239, 0, "ZhanShenLegend_OnAccept")
aux.RegisterQuestEvent(20240, 0, "ZhanShenLegend_OnAccept")
aux.RegisterQuestEvent(20241, 0, "ZhanShenLegend_OnAccept")
aux.RegisterQuestEvent(20242, 0, "ZhanShenLegend_OnAccept")
aux.RegisterQuestEvent(20243, 0, "ZhanShenLegend_OnAccept")
aux.RegisterQuestEvent(20244, 0, "ZhanShenLegend_OnAccept")
aux.RegisterQuestEvent(20245, 0, "ZhanShenLegend_OnAccept")
aux.RegisterQuestEvent(20323, 0, "ZhanShenLegend_OnAccept")
aux.RegisterQuestEvent(20324, 0, "ZhanShenLegend_OnAccept")

--玩家完成“战神传说”任务
aux.RegisterQuestEvent(20229, 1, "ZhanShenLegend_OnComplete")
aux.RegisterQuestEvent(20230, 1, "ZhanShenLegend_OnComplete")
aux.RegisterQuestEvent(20231, 1, "ZhanShenLegend_OnComplete")
aux.RegisterQuestEvent(20232, 1, "ZhanShenLegend_OnComplete")
aux.RegisterQuestEvent(20233, 1, "ZhanShenLegend_OnComplete")
aux.RegisterQuestEvent(20234, 1, "ZhanShenLegend_OnComplete")
aux.RegisterQuestEvent(20235, 1, "ZhanShenLegend_OnComplete")
aux.RegisterQuestEvent(20236, 1, "ZhanShenLegend_OnComplete")
aux.RegisterQuestEvent(20237, 1, "ZhanShenLegend_OnComplete")
aux.RegisterQuestEvent(20238, 1, "ZhanShenLegend_OnComplete")
aux.RegisterQuestEvent(20239, 1, "ZhanShenLegend_OnComplete")
aux.RegisterQuestEvent(20240, 1, "ZhanShenLegend_OnComplete")
aux.RegisterQuestEvent(20241, 1, "ZhanShenLegend_OnComplete")
aux.RegisterQuestEvent(20242, 1, "ZhanShenLegend_OnComplete")
aux.RegisterQuestEvent(20243, 1, "ZhanShenLegend_OnComplete")
aux.RegisterQuestEvent(20244, 1, "ZhanShenLegend_OnComplete")
aux.RegisterQuestEvent(20245, 1, "ZhanShenLegend_OnComplete")
aux.RegisterQuestEvent(20323, 1, "ZhanShenLegend_OnComplete")
aux.RegisterQuestEvent(20324, 1, "ZhanShenLegend_OnComplete")

--检测玩家是否可接取“战神传说”任务
aux.RegisterQuestEvent(20229, 4, "ZhanShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20230, 4, "ZhanShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20231, 4, "ZhanShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20232, 4, "ZhanShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20233, 4, "ZhanShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20234, 4, "ZhanShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20235, 4, "ZhanShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20236, 4, "ZhanShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20237, 4, "ZhanShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20238, 4, "ZhanShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20239, 4, "ZhanShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20240, 4, "ZhanShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20241, 4, "ZhanShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20242, 4, "ZhanShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20243, 4, "ZhanShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20244, 4, "ZhanShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20245, 4, "ZhanShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20323, 4, "ZhanShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20324, 4, "ZhanShenLegend_OnCheckAccept")

--检测玩家是否可接取“战神传说”任务
aux.RegisterQuestEvent(20229, 5, "ZhanShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20230, 5, "ZhanShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20231, 5, "ZhanShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20232, 5, "ZhanShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20233, 5, "ZhanShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20234, 5, "ZhanShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20235, 5, "ZhanShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20236, 5, "ZhanShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20237, 5, "ZhanShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20238, 5, "ZhanShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20239, 5, "ZhanShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20240, 5, "ZhanShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20241, 5, "ZhanShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20242, 5, "ZhanShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20243, 5, "ZhanShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20244, 5, "ZhanShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20245, 5, "ZhanShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20323, 5, "ZhanShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20324, 5, "ZhanShenLegend_OnCheckComplete")
