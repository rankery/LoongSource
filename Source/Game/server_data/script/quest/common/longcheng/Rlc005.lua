--身轻如燕无阻拦：10005
--需求1：与黑猫[3010006]对话后任务背包添加物品玄铁曲尺[2610018]

--NPC对话函数
function x10005_OnNPCTalk(mapID, instanceID, questID, ownerID, npcid, npctypeid)
    --检查是否为黑猫
	if npctypeid == 3010006 then
		role.QuestAddRoleItem(mapID, instanceID, ownerID, questID, 2610018, 1, 0, 102)
	end
end

function Bqc10005 (MapID, InstanceID, QuestID, RoleID, NPCID)	--玩家完成任务播放服务器广播
	local MsgID = msg.BeginMsgEvent();
	msg.AddMsgEvent(MsgID,102,100029);
	msg.AddMsgEvent(MsgID,2,RoleID);
	msg.DispatchBroadcast(MsgID,3017298127--[[凤翔]],InstanceID,-1);
end

--注册
aux.RegisterQuestEvent(10005, 6, "x10005_OnNPCTalk")
aux.RegisterQuestEvent(10005, 1, "Bqc10005"); --玩家完成任务 身轻如燕无阻拦[10005]（龙城跳猫任务）
