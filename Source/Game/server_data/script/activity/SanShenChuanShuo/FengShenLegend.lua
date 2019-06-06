
--用于标记玩家在本次活动中是否已完成过任务的全局table，以玩家RoleID为索引，对应记录不同玩家
FengShenLegend_Quest_IsComplete = {};
--记录每次活动中任务被完成的总次数
FengShenLegend_CompleteQuestTime = 0;
--各个城市被刷出的活动NPC“封妖道长”的CreatureID
FengShen_CreatureID = {};
--设置总共会有多少个NPC被刷出
FengShen_Quantity = 3;
--设置每个被刷出的NPC的信息
FengShen_CreatureID[1] = {MapID=3017298127,x=2184,y=6940,z=2289,CreatureID=0};	--龙城:[2184,6940,2289]
FengShen_CreatureID[2] = {MapID=3017298383,x=888,y=10464,z=2033,CreatureID=0};	--凤翔:[888,10464,2033]
FengShen_CreatureID[3] = {MapID=3017299663,x=2105,y=18338,z=2698,CreatureID=0};--涿鹿:[2105,18338,2698]

FengShenLegend_TimerMin = 0;		--记录活动进行了多长时间

--封神传说任务奖励，用于给特殊名次完成任务的玩家特殊奖励
FengShen_Reward = {};
FengShen_Reward[20247]={Exp=7476,Gold=900};
FengShen_Reward[20248]={Exp=11914,Gold=1600};
FengShen_Reward[20249]={Exp=19210,Gold=2500};
FengShen_Reward[20250]={Exp=31231,Gold=3600};
FengShen_Reward[20251]={Exp=50572,Gold=4900};
FengShen_Reward[20252]={Exp=83059,Gold=6400};
FengShen_Reward[20253]={Exp=141423,Gold=8100};
FengShen_Reward[20254]={Exp=217118,Gold=10000};
FengShen_Reward[20255]={Exp=302112,Gold=12100};
FengShen_Reward[20256]={Exp=374212,Gold=14400};
FengShen_Reward[20257]={Exp=411895,Gold=16900};
FengShen_Reward[20258]={Exp=434118,Gold=19600};
FengShen_Reward[20259]={Exp=453047,Gold=22500};
FengShen_Reward[20260]={Exp=525552,Gold=25600};
FengShen_Reward[20261]={Exp=728003,Gold=28900};
FengShen_Reward[20262]={Exp=937925,Gold=32400};
FengShen_Reward[20263]={Exp=1067081,Gold=36100};
FengShen_Reward[20321]={Exp=1267081,Gold=40100};
FengShen_Reward[20322]={Exp=1567081,Gold=45100};

function FengShenLegend_OnStart(actID)	--活动开始

	--初始化活动全局变量
	FengShenLegend_TimerMin = 0;
	FengShenLegend_CompleteQuestTime = 0;

	--刷出活动NPC 封妖道长[3610205]
	for i=1,FengShen_Quantity do
		FengShen_CreatureID[i].CreatureID = map.MapCreateCreature(FengShen_CreatureID[i].MapID, -1, 3610205, FengShen_CreatureID[i].x, FengShen_CreatureID[i].y, FengShen_CreatureID[i].z);
	end

	--活动开始播放广播：
	--“本次封神传说活动开始，各位玩家可在玄天龙城、凤翔府、黄帝城内找到"封妖道长"接取活动任务，完成后会有丰厚的奖励！”
	local MsgID = msg.BeginMsgEvent();
	if WorldCup_On== 0 then
		msg.AddMsgEvent(MsgID,102,100053)
	else
		msg.AddMsgEvent(MsgID,102,100119)
	end
	msg.DispatchBroadcast(MsgID,-1,-1,-1);
end



function FengShenLegend_OnTimerMin(actID)	--活动每经过1分钟

	FengShenLegend_TimerMin = FengShenLegend_TimerMin+1;

	local isRestart =0

	for i=1,FengShen_Quantity do
		if FengShen_CreatureID[i].CreatureID == 0 then
			FengShen_CreatureID[i].CreatureID = map.MapCreateCreature(FengShen_CreatureID[i].MapID, -1, 3610205, FengShen_CreatureID[i].x, FengShen_CreatureID[i].y, FengShen_CreatureID[i].z);
				isRestart = isRestart+1;
			if isRestart==3 then
				local MsgID = msg.BeginMsgEvent();
				if WorldCup_On== 0 then
					msg.AddMsgEvent(MsgID,102,100053)
				else
					msg.AddMsgEvent(MsgID,102,100119)
				end
				msg.DispatchBroadcast(MsgID,-1,-1,-1);
				isRestart = 0;
			end
		end
	end
	if FengShenLegend_TimerMin==5 then
		local MsgID = msg.BeginMsgEvent();
		msg.AddMsgEvent(MsgID,102,100054);
		msg.DispatchBroadcast(MsgID,-1,-1,-1);
		for i=1,FengShen_Quantity do
			if FengShen_CreatureID[i].CreatureID ~= nil then
				cre.MonsterSay(FengShen_CreatureID[i].MapID, -1, FengShen_CreatureID[i].CreatureID, 20133)
			end
		end
	elseif FengShenLegend_TimerMin==10 then
		local MsgID = msg.BeginMsgEvent();
		msg.AddMsgEvent(MsgID,102,100054);
		msg.DispatchBroadcast(MsgID,-1,-1,-1);
	elseif FengShenLegend_TimerMin==50 then
	--活动进行到50分钟时播放广播：
	--“封妖道长”将在10分钟后返回天庭，需要交付任务的玩家请尽快前往，以免因错过时间而占用下次活动的任务名额。
		local MsgID = msg.BeginMsgEvent();
		msg.AddMsgEvent(MsgID,102,100060);
		msg.AddMsgEvent(MsgID,5,3610205);
		msg.AddMsgEvent(MsgID,9,10);
		msg.DispatchBroadcast(MsgID,-1,-1,-1);
	elseif FengShenLegend_TimerMin==55 then
	--活动进行到55分钟时播放广播：
	--“封妖道长”将在5分钟后返回天庭，需要交付任务的玩家请尽快前往，以免因错过时间而占用下次活动的任务名额。
		local MsgID = msg.BeginMsgEvent();
		msg.AddMsgEvent(MsgID,102,100060);
		msg.AddMsgEvent(MsgID,5,3610205);
		msg.AddMsgEvent(MsgID,9,5);
		msg.DispatchBroadcast(MsgID,-1,-1,-1);
		for i=1,FengShen_Quantity do
			if FengShen_CreatureID[i].CreatureID ~= nil then
				cre.MonsterSay(FengShen_CreatureID[i].MapID, -1, FengShen_CreatureID[i].CreatureID, 20127)
			end
		end
--在活动的第10-55分钟期间，每3-7分钟封妖道长一定几率随机在[20132 ~ 20135]中间随进选择一条内容进行喊话
	elseif FengShenLegend_TimerMin>10 and FengShenLegend_TimerMin<55 and math.mod(FengShenLegend_TimerMin,math.random(3,7))==0 then
		local r = math.random(1,5);
		if r==1 then
			for i=1,FengShen_Quantity do
				if FengShen_CreatureID[i].CreatureID ~= nil then
					cre.MonsterSay(FengShen_CreatureID[i].MapID, -1, FengShen_CreatureID[i].CreatureID, 20132)
				end
			end
		elseif r==2 then
			for i=1,FengShen_Quantity do
				if FengShen_CreatureID[i].CreatureID ~= nil then
					cre.MonsterSay(FengShen_CreatureID[i].MapID, -1, FengShen_CreatureID[i].CreatureID, 20133)
				end
			end
		elseif r==3 then
			for i=1,FengShen_Quantity do
				if FengShen_CreatureID[i].CreatureID ~= nil then
					cre.MonsterSay(FengShen_CreatureID[i].MapID, -1, FengShen_CreatureID[i].CreatureID, 20134)
				end
			end
		elseif r==4 then
			for i=1,FengShen_Quantity do
				if FengShen_CreatureID[i].CreatureID ~= nil then
					cre.MonsterSay(FengShen_CreatureID[i].MapID, -1, FengShen_CreatureID[i].CreatureID, 20135)
				end
			end
		end
	end
end



function FengShenLegend_OnEnd(actID)	--活动结束

	--判断活动NPC是否已达到生存时间自动消失（若未消失则在场景中删除该NPC），并释放NPC的CreatureID
	for i=1,FengShen_Quantity do
		if FengShen_CreatureID[i].CreatureID ~= nil and unit.IsDead(FengShen_CreatureID[i].MapID,-1,FengShen_CreatureID[i].CreatureID)==false then
			map.MapDeleteCreature(FengShen_CreatureID[i].MapID,-1,FengShen_CreatureID[i].CreatureID);
			FengShen_CreatureID[i].CreatureID = nil;
		else
			FengShen_CreatureID[i].CreatureID = nil;
		end
	end
	FengShenLegend_Quest_IsComplete = {};
	FengShenLegend_CompleteQuestTime = 0;
	FengShenLegend_TimerMin = 0;
	--活动结束播放广播：
	--“本次封神传说活动已经结束，尚未完成任务的玩家可继续进行任务，并在下次活动时交付任务！”
	local MsgID = msg.BeginMsgEvent();
	msg.AddMsgEvent(MsgID,102,100055);
	msg.DispatchBroadcast(MsgID,-1,-1,-1);
end

function FengShenLegend_OnComplete(MapID, InstanceID, QuestID, RoleID, NPCID)	--玩家完成任务

	FengShenLegend_Quest_IsComplete[RoleID] = 1;--记录玩家在本次活动中已完成过任务
	FengShenLegend_CompleteQuestTime = FengShenLegend_CompleteQuestTime+1

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

	if FengShenLegend_CompleteQuestTime==1 then	--玩家第一个完成任务，额外给予任务的1倍经验和金钱奖励
		role.AddRoleExp(MapID, InstanceID, RoleID, FengShen_Reward[QuestID].Exp);
		role.AddRoleSilver(MapID, InstanceID, RoleID, FengShen_Reward[QuestID].Gold, 102);
		local MsgID_1 = msg.BeginMsgEvent();
		local MsgID_2 = msg.BeginMsgEvent();
		msg.AddMsgEvent(MsgID_1, 13, 0);
		msg.AddMsgEvent(MsgID_1, 1, 2721);
		msg.DispatchRoleMsgEvent(RoleID, MsgID_1);
		msg.AddMsgEvent(MsgID_2, 102, 100056);
		msg.AddMsgEvent(MsgID_2, 2, RoleID);
		msg.DispatchBroadcast(MsgID_2,-1,-1,-1);
		msg.AddMsgEvent(MsgID, 9, FengShen_Reward[QuestID].Exp*2);
		msg.AddMsgEvent(MsgID, 9, FengShen_Reward[QuestID].Gold*2);
		--增加活动进阶实力值
		Increase_shili(MapID, InstanceID, RoleID, 0, 12,"jinjieshili_A")
	elseif FengShenLegend_CompleteQuestTime==10 then	--玩家第10个完成任务，额外给予任务的0.5倍经验和金钱奖励
		local ex = math.floor(FengShen_Reward[QuestID].Exp/2);
		local go = math.floor(FengShen_Reward[QuestID].Gold/2);
		role.AddRoleExp(MapID, InstanceID, RoleID, ex);
		role.AddRoleSilver(MapID, InstanceID, RoleID, go, 102);
		local MsgID_1 = msg.BeginMsgEvent();
		local MsgID_2 = msg.BeginMsgEvent();
		msg.AddMsgEvent(MsgID_1, 13, 0);
		msg.AddMsgEvent(MsgID_1, 1, 2722);
		msg.AddMsgEvent(MsgID_1, 9, 10);
		msg.DispatchRoleMsgEvent(RoleID, MsgID_1);
		msg.AddMsgEvent(MsgID_2, 102, 100057);
		msg.AddMsgEvent(MsgID_2, 2, RoleID);
		msg.DispatchBroadcast(MsgID_2,-1,-1,-1);
		msg.AddMsgEvent(MsgID, 9, ShiShen_Reward[QuestID].Exp+ex);
		msg.AddMsgEvent(MsgID, 9, ShiShen_Reward[QuestID].Gold+go);
		--增加活动进阶实力值
		Increase_shili(MapID, InstanceID, RoleID, 0, 9,"jinjieshili_A")
	elseif FengShenLegend_CompleteQuestTime==50 then	--玩家第50个完成任务，额外给予任务的0.5倍经验和金钱奖励
		local ex = math.floor(FengShen_Reward[QuestID].Exp/2);
		local go = math.floor(FengShen_Reward[QuestID].Gold/2);
		role.AddRoleExp(MapID, InstanceID, RoleID, ex);
		role.AddRoleSilver(MapID, InstanceID, RoleID, go, 102);
		local MsgID_1 = msg.BeginMsgEvent();
		local MsgID_2 = msg.BeginMsgEvent();
		msg.AddMsgEvent(MsgID_1, 13, 0);
		msg.AddMsgEvent(MsgID_1, 1, 2722);
		msg.AddMsgEvent(MsgID_1, 9, 50);
		msg.DispatchRoleMsgEvent(RoleID, MsgID_1);
		msg.AddMsgEvent(MsgID_2, 102, 100059);
		msg.AddMsgEvent(MsgID_2, 2, RoleID);
		msg.DispatchBroadcast(MsgID_2,-1,-1,-1);
		msg.AddMsgEvent(MsgID, 9, FengShen_Reward[QuestID].Exp+ex);
		msg.AddMsgEvent(MsgID, 9, FengShen_Reward[QuestID].Gold+go);
		--增加活动进阶实力值
		Increase_shili(MapID, InstanceID, RoleID, 0, 9,"jinjieshili_A")
	elseif FengShenLegend_CompleteQuestTime==100 then	--玩家第100个完成任务，额外给予任务的0.5倍经验和金钱奖励
		local ex = math.floor(FengShen_Reward[QuestID].Exp/2);
		local go = math.floor(FengShen_Reward[QuestID].Gold/2);
		role.AddRoleExp(MapID, InstanceID, RoleID, ex);
		role.AddRoleSilver(MapID, InstanceID, RoleID, go, 102);
		local MsgID_1 = msg.BeginMsgEvent();
		local MsgID_2 = msg.BeginMsgEvent();
		msg.AddMsgEvent(MsgID_1, 13, 0);
		msg.AddMsgEvent(MsgID_1, 1, 2722);
		msg.AddMsgEvent(MsgID_1, 9, 100);
		msg.DispatchRoleMsgEvent(RoleID, MsgID_1);
		msg.AddMsgEvent(MsgID_2, 102, 100058);
		msg.AddMsgEvent(MsgID_2, 2, RoleID);
		msg.DispatchBroadcast(MsgID_2,-1,-1,-1);
		msg.AddMsgEvent(MsgID, 9, FengShen_Reward[QuestID].Exp+ex);
		msg.AddMsgEvent(MsgID, 9, FengShen_Reward[QuestID].Gold+go);
		--增加活动进阶实力值
		Increase_shili(MapID, InstanceID, RoleID, 0, 9,"jinjieshili_A")
	else
		msg.AddMsgEvent(MsgID, 9, FengShen_Reward[QuestID].Exp);
		msg.AddMsgEvent(MsgID, 9, FengShen_Reward[QuestID].Gold);
		--增加活动进阶实力值
		Increase_shili(MapID, InstanceID, RoleID, 0, 6,"jinjieshili_A")
	end
	msg.DispatchRoleMsgEvent(RoleID, MsgID);
	--玩家交付任务时，NPC有50%几率随机[20128 ~ 20131]中随机一条喊话
	local r = math.random(1,8);
	if r==1 then
		for i=1,FengShen_Quantity do
			if FengShen_CreatureID[i].MapID == MapID and FengShen_CreatureID[i].CreatureID ~= nil then
				cre.MonsterSay(FengShen_CreatureID[i].MapID, -1, FengShen_CreatureID[i].CreatureID, 20128);
			end
		end
	elseif r==2 then
		for i=1,FengShen_Quantity do
			if FengShen_CreatureID[i].MapID == MapID and FengShen_CreatureID[i].CreatureID ~= nil then
				cre.MonsterSay(FengShen_CreatureID[i].MapID, -1, FengShen_CreatureID[i].CreatureID, 20129);
			end
		end
	elseif r==3 then
		for i=1,FengShen_Quantity do
			if FengShen_CreatureID[i].MapID == MapID and FengShen_CreatureID[i].CreatureID ~= nil then
				cre.MonsterSay(FengShen_CreatureID[i].MapID, -1, FengShen_CreatureID[i].CreatureID, 20130);
			end
		end
	elseif r==4 then
		for i=1,FengShen_Quantity do
			if FengShen_CreatureID[i].MapID == MapID and FengShen_CreatureID[i].CreatureID ~= nil then
				cre.MonsterSay(FengShen_CreatureID[i].MapID, -1, FengShen_CreatureID[i].CreatureID, 20131);
			end
		end
	end
end

--判断玩家在本次活动期间内是否已经完成过任务
function FengShenLegend_OnCheckAccept(MapID, InstanceID, QuestID, RoleID, NPCID)
	if FengShenLegend_Quest_IsComplete[RoleID] == 1 then
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

--判断玩家在本次活动期间内是否已经完成过任务
function FengShenLegend_OnAccept(MapID, InstanceID, QuestID, RoleID, NPCID)

	FengShenLegend_Quest_IsComplete[RoleID] = 1;

end

function FengShenLegend_OnCheckComplete(MapID, InstanceID, QuestID, RoleID, NPCID)

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
aux.RegisterActEvent(115, 2, "FengShenLegend_OnStart")
aux.RegisterActEvent(116, 2, "FengShenLegend_OnStart")

--活动结束
aux.RegisterActEvent(115, 3, "FengShenLegend_OnEnd")
aux.RegisterActEvent(116, 3, "FengShenLegend_OnEnd")

--活动以“分钟”为单位计时
aux.RegisterActEvent(115, 4, "FengShenLegend_OnTimerMin")
aux.RegisterActEvent(116, 4, "FengShenLegend_OnTimerMin")

--玩家完成“封神传说”任务
aux.RegisterQuestEvent(20247, 1, "FengShenLegend_OnComplete")
aux.RegisterQuestEvent(20248, 1, "FengShenLegend_OnComplete")
aux.RegisterQuestEvent(20249, 1, "FengShenLegend_OnComplete")
aux.RegisterQuestEvent(20250, 1, "FengShenLegend_OnComplete")
aux.RegisterQuestEvent(20251, 1, "FengShenLegend_OnComplete")
aux.RegisterQuestEvent(20252, 1, "FengShenLegend_OnComplete")
aux.RegisterQuestEvent(20253, 1, "FengShenLegend_OnComplete")
aux.RegisterQuestEvent(20254, 1, "FengShenLegend_OnComplete")
aux.RegisterQuestEvent(20255, 1, "FengShenLegend_OnComplete")
aux.RegisterQuestEvent(20256, 1, "FengShenLegend_OnComplete")
aux.RegisterQuestEvent(20257, 1, "FengShenLegend_OnComplete")
aux.RegisterQuestEvent(20258, 1, "FengShenLegend_OnComplete")
aux.RegisterQuestEvent(20259, 1, "FengShenLegend_OnComplete")
aux.RegisterQuestEvent(20260, 1, "FengShenLegend_OnComplete")
aux.RegisterQuestEvent(20261, 1, "FengShenLegend_OnComplete")
aux.RegisterQuestEvent(20262, 1, "FengShenLegend_OnComplete")
aux.RegisterQuestEvent(20263, 1, "FengShenLegend_OnComplete")
aux.RegisterQuestEvent(20321, 1, "FengShenLegend_OnComplete")
aux.RegisterQuestEvent(20322, 1, "FengShenLegend_OnComplete")

--玩家接取“封神传说”任务
aux.RegisterQuestEvent(20247, 0, "FengShenLegend_OnAccept")
aux.RegisterQuestEvent(20248, 0, "FengShenLegend_OnAccept")
aux.RegisterQuestEvent(20249, 0, "FengShenLegend_OnAccept")
aux.RegisterQuestEvent(20250, 0, "FengShenLegend_OnAccept")
aux.RegisterQuestEvent(20251, 0, "FengShenLegend_OnAccept")
aux.RegisterQuestEvent(20252, 0, "FengShenLegend_OnAccept")
aux.RegisterQuestEvent(20253, 0, "FengShenLegend_OnAccept")
aux.RegisterQuestEvent(20254, 0, "FengShenLegend_OnAccept")
aux.RegisterQuestEvent(20255, 0, "FengShenLegend_OnAccept")
aux.RegisterQuestEvent(20256, 0, "FengShenLegend_OnAccept")
aux.RegisterQuestEvent(20257, 0, "FengShenLegend_OnAccept")
aux.RegisterQuestEvent(20258, 0, "FengShenLegend_OnAccept")
aux.RegisterQuestEvent(20259, 0, "FengShenLegend_OnAccept")
aux.RegisterQuestEvent(20260, 0, "FengShenLegend_OnAccept")
aux.RegisterQuestEvent(20261, 0, "FengShenLegend_OnAccept")
aux.RegisterQuestEvent(20262, 0, "FengShenLegend_OnAccept")
aux.RegisterQuestEvent(20263, 0, "FengShenLegend_OnAccept")
aux.RegisterQuestEvent(20321, 0, "FengShenLegend_OnAccept")
aux.RegisterQuestEvent(20322, 0, "FengShenLegend_OnAccept")

--检测玩家是否可接取“封神传说”任务
aux.RegisterQuestEvent(20247, 4, "FengShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20248, 4, "FengShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20249, 4, "FengShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20250, 4, "FengShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20251, 4, "FengShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20252, 4, "FengShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20253, 4, "FengShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20254, 4, "FengShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20255, 4, "FengShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20256, 4, "FengShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20257, 4, "FengShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20258, 4, "FengShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20259, 4, "FengShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20260, 4, "FengShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20261, 4, "FengShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20262, 4, "FengShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20263, 4, "FengShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20321, 4, "FengShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20322, 4, "FengShenLegend_OnCheckAccept")


--检测玩家是否可完成“封神传说”任务
aux.RegisterQuestEvent(20247, 5, "FengShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20248, 5, "FengShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20249, 5, "FengShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20250, 5, "FengShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20251, 5, "FengShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20252, 5, "FengShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20253, 5, "FengShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20254, 5, "FengShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20255, 5, "FengShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20256, 5, "FengShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20257, 5, "FengShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20258, 5, "FengShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20259, 5, "FengShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20260, 5, "FengShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20261, 5, "FengShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20262, 5, "FengShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20263, 5, "FengShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20321, 5, "FengShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20322, 5, "FengShenLegend_OnCheckComplete")
