
--用于标记玩家在本次活动中是否已完成过任务的全局table，以玩家RoleID为索引，对应记录不同玩家
ShiShenLegend_Quest_IsComplete = {};
--记录每次活动中任务被完成的总次数
ShiShenLegend_CompleteQuestTime = 0;
--各个城市被刷出的活动NPC“下凡食神”的CreatureID
ShiShen_CreatureID = {};
--设置总共会有多少个NPC被刷出
ShiShen_Quantity = 3;
--设置每个被刷出的NPC的信息
ShiShen_CreatureID[1] = {MapID=3017298127,x=2184,y=6940,z=2289,CreatureID=0};	--龙城:[2184,6940,2289]
ShiShen_CreatureID[2] = {MapID=3017298383,x=888,y=10464,z=2033,CreatureID=0};	--凤翔:[888,10464,2033]
ShiShen_CreatureID[3] = {MapID=3017299663,x=2105,y=18338,z=2698,CreatureID=0};--涿鹿:[2105,18338,2698]

ShiShenLegend_TimerMin = 0;		--记录活动进行了多长时间

--食神传说任务奖励，用于给特殊名次完成任务的玩家特殊奖励
ShiShen_Reward = {};
ShiShen_Reward[20211]={Exp=5751,Gold=675};
ShiShen_Reward[20212]={Exp=9165,Gold=1200};
ShiShen_Reward[20213]={Exp=14777,Gold=1875};
ShiShen_Reward[20214]={Exp=24024,Gold=2700};
ShiShen_Reward[20215]={Exp=38902,Gold=3675};
ShiShen_Reward[20216]={Exp=63892,Gold=4800};
ShiShen_Reward[20217]={Exp=108787,Gold=6075};
ShiShen_Reward[20218]={Exp=167014,Gold=7500};
ShiShen_Reward[20219]={Exp=232394,Gold=9075};
ShiShen_Reward[20220]={Exp=287856,Gold=10800};
ShiShen_Reward[20221]={Exp=316843,Gold=12675};
ShiShen_Reward[20222]={Exp=333937,Gold=14700};
ShiShen_Reward[20223]={Exp=348498,Gold=16875};
ShiShen_Reward[20224]={Exp=404271,Gold=19200};
ShiShen_Reward[20225]={Exp=560003,Gold=21675};
ShiShen_Reward[20226]={Exp=721481,Gold=24300};
ShiShen_Reward[20227]={Exp=820832,Gold=27075};
ShiShen_Reward[20325]={Exp=1020832,Gold=37075};
ShiShen_Reward[20326]={Exp=1220832,Gold=47075};

function ShiShenLegend_OnStart(actID)	--活动开始

	ShiShenLegend_TimerMin = 0;
	ShiShenLegend_CompleteQuestTime = 0

	--刷出活动NPC 下凡食神[3610203]
	for i=1,ShiShen_Quantity do
		ShiShen_CreatureID[i].CreatureID = map.MapCreateCreature(ShiShen_CreatureID[i].MapID, -1, 3610203, ShiShen_CreatureID[i].x, ShiShen_CreatureID[i].y, ShiShen_CreatureID[i].z);
	end

	--活动开始播放广播：
	--“本次食神传说活动开始，各位玩家可在玄天龙城、凤翔府、黄帝城内找到"下凡食神"接取活动任务，完成后会有丰厚的奖励！”
	local MsgID = msg.BeginMsgEvent();
	if WorldCup_On== 0 then
		msg.AddMsgEvent(MsgID,102,100041)
	else
		msg.AddMsgEvent(MsgID,102,100120)
	end
	msg.DispatchBroadcast(MsgID,-1,-1,-1);
end



function ShiShenLegend_OnTimerMin(actID)	--活动每经过1分钟

	ShiShenLegend_TimerMin = ShiShenLegend_TimerMin + 1;
	local isRestart = 0;
	if ShiShen_CreatureID[1].CreatureID == 0 then
		for i=1,ShiShen_Quantity do
			ShiShen_CreatureID[i].CreatureID = map.MapCreateCreature(ShiShen_CreatureID[i].MapID, -1, 3610203, ShiShen_CreatureID[i].x, ShiShen_CreatureID[i].y, ShiShen_CreatureID[i].z);
			isRestart = isRestart+1;
			if isRestart==3 then
				local MsgID = msg.BeginMsgEvent();
				if WorldCup_On== 0 then
					msg.AddMsgEvent(MsgID,102,100041)
				else
					msg.AddMsgEvent(MsgID,102,100120)
				end
				msg.DispatchBroadcast(MsgID,-1,-1,-1);
				isRestart = 0;
			end
		end
	end
	if ShiShenLegend_TimerMin==5 then
		local MsgID = msg.BeginMsgEvent();
		msg.AddMsgEvent(MsgID,102,100042);
		msg.DispatchBroadcast(MsgID,-1,-1,-1);
		for i=1,ShiShen_Quantity do
			if ShiShen_CreatureID[i].CreatureID ~= nil then
				cre.MonsterSay(ShiShen_CreatureID[i].MapID, -1, ShiShen_CreatureID[i].CreatureID, 20116)
			end
		end
	elseif ShiShenLegend_TimerMin==10 then
		local MsgID = msg.BeginMsgEvent();
		msg.AddMsgEvent(MsgID,102,100042);
		msg.DispatchBroadcast(MsgID,-1,-1,-1);
	elseif ShiShenLegend_TimerMin==50 then
	--活动进行到50分钟时播放广播：
	--“下凡食神”将在10分钟后返回天庭，需要交付任务的玩家请尽快前往，以免因错过时间而占用下次活动的任务名额。
		local MsgID = msg.BeginMsgEvent();
		msg.AddMsgEvent(MsgID,102,100060);
		msg.AddMsgEvent(MsgID,5,3610203);
		msg.AddMsgEvent(MsgID,9,10);
		msg.DispatchBroadcast(MsgID,-1,-1,-1);
	elseif ShiShenLegend_TimerMin==55 then
	--活动进行到55分钟时播放广播：
	--“下凡食神”将在5分钟后返回天庭，需要交付任务的玩家请尽快前往，以免因错过时间而占用下次活动的任务名额。
		local MsgID = msg.BeginMsgEvent();
		msg.AddMsgEvent(MsgID,102,100060);
		msg.AddMsgEvent(MsgID,5,3610203);
		msg.AddMsgEvent(MsgID,9,5);
		msg.DispatchBroadcast(MsgID,-1,-1,-1);
		for i=1,ShiShen_Quantity do
			if ShiShen_CreatureID[i].CreatureID ~= nil then
				cre.MonsterSay(ShiShen_CreatureID[i].MapID, -1, ShiShen_CreatureID[i].CreatureID, 20111)
			end
		end
	--在活动的第10-55分钟期间，每3-7分钟下凡食神一定几率随机在[20115 ~ 20119]中间随进选择一条内容进行喊话
	elseif ShiShenLegend_TimerMin>10 and ShiShenLegend_TimerMin<55 and math.mod(ShiShenLegend_TimerMin,math.random(3,7))==0 then
		local r = math.random(1,6);
		if r==1 then
			for i=1,ShiShen_Quantity do
				if ShiShen_CreatureID[i].CreatureID ~= nil then
					cre.MonsterSay(ShiShen_CreatureID[i].MapID, -1, ShiShen_CreatureID[i].CreatureID, 20115)
				end
			end
		elseif r==2 then
			for i=1,ShiShen_Quantity do
				if ShiShen_CreatureID[i].CreatureID ~= nil then
					cre.MonsterSay(ShiShen_CreatureID[i].MapID, -1, ShiShen_CreatureID[i].CreatureID, 20116)
				end
			end
		elseif r==3 then
			for i=1,ShiShen_Quantity do
				if ShiShen_CreatureID[i].CreatureID ~= nil then
					cre.MonsterSay(ShiShen_CreatureID[i].MapID, -1, ShiShen_CreatureID[i].CreatureID, 20117)
				end
			end
		elseif r==4 then
			for i=1,ShiShen_Quantity do
				if ShiShen_CreatureID[i].CreatureID ~= nil then
					cre.MonsterSay(ShiShen_CreatureID[i].MapID, -1, ShiShen_CreatureID[i].CreatureID, 20118)
				end
			end
		elseif r==5 then
			for i=1,ShiShen_Quantity do
				if ShiShen_CreatureID[i].CreatureID ~= nil then
					cre.MonsterSay(ShiShen_CreatureID[i].MapID, -1, ShiShen_CreatureID[i].CreatureID, 20119)
				end
			end
		end
	end
end



function ShiShenLegend_OnEnd(actID)	--活动结束
	--判断活动NPC是否已达到生存时间自动消失（若未消失则在场景中删除该NPC），并释放NPC的CreatureID
	for i=1,ShiShen_Quantity do
		if ShiShen_CreatureID[i].CreatureID ~= nil and unit.IsDead(ShiShen_CreatureID[i].MapID,-1,ShiShen_CreatureID[i].CreatureID)==false then
			map.MapDeleteCreature(ShiShen_CreatureID[i].MapID,-1,ShiShen_CreatureID[i].CreatureID);
			ShiShen_CreatureID[i].CreatureID = nil;
		else
			ShiShen_CreatureID[i].CreatureID = nil;
		end
	end

	ShiShenLegend_Quest_IsComplete = {};
	ShiShenLegend_CompleteQuestTime = 0;
	ShiShenLegend_TimerMin = 0;
	--活动结束播放广播：
	--“本次食神传说活动已经结束，尚未完成任务的玩家可继续进行任务，并在下次活动时交付任务！”
	local MsgID = msg.BeginMsgEvent();
	msg.AddMsgEvent(MsgID,102,100043);
	msg.DispatchBroadcast(MsgID,-1,-1,-1);
end

function ShiShenLegend_OnComplete(MapID, InstanceID, QuestID, RoleID, NPCID)	--玩家完成任务
	ShiShenLegend_CompleteQuestTime = ShiShenLegend_CompleteQuestTime + 1;
	ShiShenLegend_Quest_IsComplete[RoleID] = 1;

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
	if ShiShenLegend_CompleteQuestTime ==1 then	--玩家第一个完成任务，额外给予任务的1倍经验和金钱奖励
		role.AddRoleExp(MapID, InstanceID, RoleID, ShiShen_Reward[QuestID].Exp);
		role.AddRoleSilver(MapID, InstanceID, RoleID, ShiShen_Reward[QuestID].Gold, 102);
		local MsgID_1 = msg.BeginMsgEvent();
		local MsgID_2 = msg.BeginMsgEvent();
		msg.AddMsgEvent(MsgID_1, 13, 0);
		msg.AddMsgEvent(MsgID_1, 1, 2721);
		msg.DispatchRoleMsgEvent(RoleID, MsgID_1);
		msg.AddMsgEvent(MsgID_2, 102, 100044);
		msg.AddMsgEvent(MsgID_2, 2, RoleID);
		msg.DispatchBroadcast(MsgID_2,-1,-1,-1);
		msg.AddMsgEvent(MsgID, 9, ShiShen_Reward[QuestID].Exp*2);
		msg.AddMsgEvent(MsgID, 9, ShiShen_Reward[QuestID].Gold*2);
		--增加活动进阶实力值
		Increase_shili(MapID, InstanceID, RoleID, 0, 12,"jinjieshili_A")
	elseif ShiShenLegend_CompleteQuestTime==10 then	--玩家第10个完成任务，额外给予任务的0.5倍经验和金钱奖励
		local ex = math.floor(ShiShen_Reward[QuestID].Exp/2);
		local go = math.floor(ShiShen_Reward[QuestID].Gold/2);
		role.AddRoleExp(MapID, InstanceID, RoleID, ex);
		role.AddRoleSilver(MapID, InstanceID, RoleID, go, 102);
		local MsgID_1 = msg.BeginMsgEvent();
		local MsgID_2 = msg.BeginMsgEvent();
		msg.AddMsgEvent(MsgID_1, 13, 0);
		msg.AddMsgEvent(MsgID_1, 1, 2722);
		msg.AddMsgEvent(MsgID_1, 9, 10);
		msg.DispatchRoleMsgEvent(RoleID, MsgID_1);
		msg.AddMsgEvent(MsgID_2, 102, 100045);
		msg.AddMsgEvent(MsgID_2, 2, RoleID);
		msg.DispatchBroadcast(MsgID_2,-1,-1,-1);
		msg.AddMsgEvent(MsgID, 9, ShiShen_Reward[QuestID].Exp+ex);
		msg.AddMsgEvent(MsgID, 9, ShiShen_Reward[QuestID].Gold+go);
		--增加活动进阶实力值
		Increase_shili(MapID, InstanceID, RoleID, 0, 9,"jinjieshili_A")
	elseif ShiShenLegend_CompleteQuestTime==100 then	--玩家第100个完成任务，额外给予任务的0.5倍经验和金钱奖励
		local ex = math.floor(ShiShen_Reward[QuestID].Exp/2);
		local go = math.floor(ShiShen_Reward[QuestID].Gold/2);
		role.AddRoleExp(MapID, InstanceID, RoleID, ex);
		role.AddRoleSilver(MapID, InstanceID, RoleID, go, 102);
		local MsgID_1 = msg.BeginMsgEvent();
		local MsgID_2 = msg.BeginMsgEvent();
		msg.AddMsgEvent(MsgID_1, 13, 0);
		msg.AddMsgEvent(MsgID_1, 1, 2722);
		msg.AddMsgEvent(MsgID_1, 9, 100);
		msg.DispatchRoleMsgEvent(RoleID, MsgID_1);
		msg.AddMsgEvent(MsgID_2, 102, 100046);
		msg.AddMsgEvent(MsgID_2, 2, RoleID);
		msg.DispatchBroadcast(MsgID_2,-1,-1,-1);
		msg.AddMsgEvent(MsgID, 9, ShiShen_Reward[QuestID].Exp+ex);
		msg.AddMsgEvent(MsgID, 9, ShiShen_Reward[QuestID].Gold+go);
		--增加活动进阶实力值
		Increase_shili(MapID, InstanceID, RoleID, 0, 9,"jinjieshili_A")
	else
		msg.AddMsgEvent(MsgID, 9, ShiShen_Reward[QuestID].Exp);
		msg.AddMsgEvent(MsgID, 9, ShiShen_Reward[QuestID].Gold);
		--增加活动进阶实力值
		Increase_shili(MapID, InstanceID, RoleID, 0, 6,"jinjieshili_A")
	end
	msg.DispatchRoleMsgEvent(RoleID, MsgID);
	--玩家交付任务时，NPC有50%几率随机[20012 ~ 20014]中随机一条喊话
	local r = math.random(1,6);
	if r==1 then
		for i=1,ShiShen_Quantity do
			if ShiShen_CreatureID[i].MapID == MapID and ShiShen_CreatureID[i].CreatureID ~= nil then
				cre.MonsterSay(ShiShen_CreatureID[i].MapID, -1, ShiShen_CreatureID[i].CreatureID, 20112);
			end
		end
	elseif r==2 then
		for i=1,ShiShen_Quantity do
			if ShiShen_CreatureID[i].MapID == MapID and ShiShen_CreatureID[i].CreatureID ~= nil then
				cre.MonsterSay(ShiShen_CreatureID[i].MapID, -1, ShiShen_CreatureID[i].CreatureID, 20113);
			end
		end
	elseif r==3 then
		for i=1,ShiShen_Quantity do
			if ShiShen_CreatureID[i].MapID == MapID and ShiShen_CreatureID[i].CreatureID ~= nil then
				cre.MonsterSay(ShiShen_CreatureID[i].MapID, -1, ShiShen_CreatureID[i].CreatureID, 20114);
			end
		end
	end
end


function ShiShenLegend_OnCheckAccept(MapID, InstanceID, QuestID, RoleID, NPCID)
	if ShiShenLegend_Quest_IsComplete[RoleID] == 1 then
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

function ShiShenLegend_OnAccept(MapID, InstanceID, QuestID, RoleID, NPCID)

	ShiShenLegend_Quest_IsComplete[RoleID] = 1;

end

function ShiShenLegend_OnCheckComplete(MapID, InstanceID, QuestID, RoleID, NPCID)

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
aux.RegisterActEvent(107, 2, "ShiShenLegend_OnStart")
aux.RegisterActEvent(108, 2, "ShiShenLegend_OnStart")
aux.RegisterActEvent(109, 2, "ShiShenLegend_OnStart")
aux.RegisterActEvent(110, 2, "ShiShenLegend_OnStart")
--活动结束
aux.RegisterActEvent(107, 3, "ShiShenLegend_OnEnd")
aux.RegisterActEvent(108, 3, "ShiShenLegend_OnEnd")
aux.RegisterActEvent(109, 3, "ShiShenLegend_OnEnd")
aux.RegisterActEvent(110, 3, "ShiShenLegend_OnEnd")

--活动以“分钟”为单位计时
aux.RegisterActEvent(107, 4, "ShiShenLegend_OnTimerMin")
aux.RegisterActEvent(108, 4, "ShiShenLegend_OnTimerMin")
aux.RegisterActEvent(109, 4, "ShiShenLegend_OnTimerMin")
aux.RegisterActEvent(110, 4, "ShiShenLegend_OnTimerMin")



--玩家完成“食神传说”任务
aux.RegisterQuestEvent(20211, 1, "ShiShenLegend_OnComplete")
aux.RegisterQuestEvent(20212, 1, "ShiShenLegend_OnComplete")
aux.RegisterQuestEvent(20213, 1, "ShiShenLegend_OnComplete")
aux.RegisterQuestEvent(20214, 1, "ShiShenLegend_OnComplete")
aux.RegisterQuestEvent(20215, 1, "ShiShenLegend_OnComplete")
aux.RegisterQuestEvent(20216, 1, "ShiShenLegend_OnComplete")
aux.RegisterQuestEvent(20217, 1, "ShiShenLegend_OnComplete")
aux.RegisterQuestEvent(20218, 1, "ShiShenLegend_OnComplete")
aux.RegisterQuestEvent(20219, 1, "ShiShenLegend_OnComplete")
aux.RegisterQuestEvent(20220, 1, "ShiShenLegend_OnComplete")
aux.RegisterQuestEvent(20221, 1, "ShiShenLegend_OnComplete")
aux.RegisterQuestEvent(20222, 1, "ShiShenLegend_OnComplete")
aux.RegisterQuestEvent(20223, 1, "ShiShenLegend_OnComplete")
aux.RegisterQuestEvent(20224, 1, "ShiShenLegend_OnComplete")
aux.RegisterQuestEvent(20225, 1, "ShiShenLegend_OnComplete")
aux.RegisterQuestEvent(20226, 1, "ShiShenLegend_OnComplete")
aux.RegisterQuestEvent(20227, 1, "ShiShenLegend_OnComplete")
aux.RegisterQuestEvent(20325, 1, "ShiShenLegend_OnComplete")
aux.RegisterQuestEvent(20326, 1, "ShiShenLegend_OnComplete")

--玩家接取“食神传说”任务
aux.RegisterQuestEvent(20211, 0, "ShiShenLegend_OnAccept")
aux.RegisterQuestEvent(20212, 0, "ShiShenLegend_OnAccept")
aux.RegisterQuestEvent(20213, 0, "ShiShenLegend_OnAccept")
aux.RegisterQuestEvent(20214, 0, "ShiShenLegend_OnAccept")
aux.RegisterQuestEvent(20215, 0, "ShiShenLegend_OnAccept")
aux.RegisterQuestEvent(20216, 0, "ShiShenLegend_OnAccept")
aux.RegisterQuestEvent(20217, 0, "ShiShenLegend_OnAccept")
aux.RegisterQuestEvent(20218, 0, "ShiShenLegend_OnAccept")
aux.RegisterQuestEvent(20219, 0, "ShiShenLegend_OnAccept")
aux.RegisterQuestEvent(20220, 0, "ShiShenLegend_OnAccept")
aux.RegisterQuestEvent(20221, 0, "ShiShenLegend_OnAccept")
aux.RegisterQuestEvent(20222, 0, "ShiShenLegend_OnAccept")
aux.RegisterQuestEvent(20223, 0, "ShiShenLegend_OnAccept")
aux.RegisterQuestEvent(20224, 0, "ShiShenLegend_OnAccept")
aux.RegisterQuestEvent(20225, 0, "ShiShenLegend_OnAccept")
aux.RegisterQuestEvent(20226, 0, "ShiShenLegend_OnAccept")
aux.RegisterQuestEvent(20227, 0, "ShiShenLegend_OnAccept")
aux.RegisterQuestEvent(20325, 0, "ShiShenLegend_OnAccept")
aux.RegisterQuestEvent(20326, 0, "ShiShenLegend_OnAccept")

--检测玩家是否可接取“食神传说”任务
aux.RegisterQuestEvent(20211, 4, "ShiShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20212, 4, "ShiShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20213, 4, "ShiShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20214, 4, "ShiShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20215, 4, "ShiShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20216, 4, "ShiShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20217, 4, "ShiShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20218, 4, "ShiShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20219, 4, "ShiShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20220, 4, "ShiShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20221, 4, "ShiShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20222, 4, "ShiShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20223, 4, "ShiShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20224, 4, "ShiShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20225, 4, "ShiShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20226, 4, "ShiShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20227, 4, "ShiShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20325, 4, "ShiShenLegend_OnCheckAccept")
aux.RegisterQuestEvent(20326, 4, "ShiShenLegend_OnCheckAccept")

--检测玩家是否可接取“食神传说”任务
aux.RegisterQuestEvent(20211, 5, "ShiShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20212, 5, "ShiShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20213, 5, "ShiShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20214, 5, "ShiShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20215, 5, "ShiShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20216, 5, "ShiShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20217, 5, "ShiShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20218, 5, "ShiShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20219, 5, "ShiShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20220, 5, "ShiShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20221, 5, "ShiShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20222, 5, "ShiShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20223, 5, "ShiShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20224, 5, "ShiShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20225, 5, "ShiShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20226, 5, "ShiShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20227, 5, "ShiShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20325, 5, "ShiShenLegend_OnCheckComplete")
aux.RegisterQuestEvent(20326, 5, "ShiShenLegend_OnCheckComplete")
