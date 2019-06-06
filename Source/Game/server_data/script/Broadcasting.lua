--测试新广播机制功能
---------------------------------------------------------------------------------------------
function I7999901_OnUse (MapID, InstanceID, TypeID, RoleID)	--使用物品测试广播1[7999901]
	local MsgID = msg.BeginMsgEvent();
	msg.AddMsgEvent(MsgID,102,100001);
	msg.AddMsgEvent(MsgID,2,RoleID);
	msg.DispatchBroadcast(MsgID,-1,-1,-1);	--全服广播
end
---------------------------------------------------------------------------------------------
function I7999902_OnUse (MapID, InstanceID, TypeID, RoleID)	--使用物品测试广播2[7999902]
	local MsgID = msg.BeginMsgEvent();
	msg.AddMsgEvent(MsgID,102,100002);
	msg.AddMsgEvent(MsgID,2,RoleID);
	msg.AddMsgEvent(MsgID,4,TypeID);
	msg.DispatchBroadcast(MsgID,-1,-1,-1);
end
---------------------------------------------------------------------------------------------
function I7999903_OnUse (MapID, InstanceID, TypeID, RoleID)	--使用物品测试广播3[7999903]
	local MsgID = msg.BeginMsgEvent();
	msg.AddMsgEvent(MsgID,102,100003);
	msg.AddMsgEvent(MsgID,2,RoleID);
	msg.AddMsgEvent(MsgID,2,RoleID);
	msg.AddMsgEvent(MsgID,4,TypeID);
	msg.DispatchBroadcast(MsgID,-1,-1,-1);
end
---------------------------------------------------------------------------------------------
function I7999904_OnUse (MapID, InstanceID, TypeID, RoleID)	--使用物品测试广播4[7999904]
	local MsgID = msg.BeginMsgEvent();
	msg.AddMsgEvent(MsgID,102,100004);
	msg.DispatchBroadcast(MsgID,-1,-1,-1);
end
---------------------------------------------------------------------------------------------
function I7999905_OnUse (MapID, InstanceID, TypeID, RoleID)	--使用物品测试广播5[7999905]
	local MsgID = msg.BeginMsgEvent();
	msg.AddMsgEvent(MsgID,102,100005);
	msg.DispatchBroadcast(MsgID,-1,-1,-1);
end
---------------------------------------------------------------------------------------------
function I7999906_OnUse (MapID, InstanceID, TypeID, RoleID)	--使用物品测试广播6[7999906]
	local MsgID = msg.BeginMsgEvent();
	msg.AddMsgEvent(MsgID,102,100006);
	msg.AddMsgEvent(MsgID,2,RoleID);
	msg.AddMsgEvent(MsgID,2,RoleID);
	msg.DispatchBroadcast(MsgID,-1,-1,-1);	--全服广播
end
---------------------------------------------------------------------------------------------
function I7999907_OnUse (MapID, InstanceID, TypeID, RoleID)	--使用物品测试广播7[7999907]
	local MsgID = msg.BeginMsgEvent();
	msg.AddMsgEvent(MsgID,102,100007);
	msg.DispatchBroadcast(MsgID,-1,-1,-1);
end
---------------------------------------------------------------------------------------------
function I7999908_OnUse (MapID, InstanceID, TypeID, RoleID)	--使用物品测试广播8[7999908]
	local MsgID = msg.BeginMsgEvent();
	msg.AddMsgEvent(MsgID,102,100008);
	msg.DispatchBroadcast(MsgID,-1,-1,-1);
end
---------------------------------------------------------------------------------------------


--玩家完成任务
---------------------------------------------------------------------------------------------
function Bqc11017 (MapID, InstanceID, QuestID, RoleID, NPCID)	--玩家完成任务 闯三关-下篇[11017]
	local MsgID = msg.BeginMsgEvent();
	msg.AddMsgEvent(MsgID,102,100028);
	msg.AddMsgEvent(MsgID,2,RoleID);
	msg.DispatchBroadcast(MsgID,3017298383--[[凤翔]],InstanceID,-1);
end
---------------------------------------------------------------------------------------------




---------------------------------------------------------------------------------------------
--******************************************分割线***************************************************--
--以下为事件注册内容；
--事件注册；

--测试新广播机制功能相关的事件注册
aux.RegisterItemEvent(7999901,1,"I7999901_OnUse");	--使用物品测试广播1[7999901]
aux.RegisterItemEvent(7999902,1,"I7999902_OnUse");	--使用物品测试广播2[7999902]
aux.RegisterItemEvent(7999903,1,"I7999903_OnUse");	--使用物品测试广播3[7999903]
aux.RegisterItemEvent(7999904,1,"I7999904_OnUse");	--使用物品测试广播4[7999904]
aux.RegisterItemEvent(7999905,1,"I7999905_OnUse");	--使用物品测试广播5[7999905]
aux.RegisterItemEvent(7999906,1,"I7999906_OnUse");	--使用物品测试广播6[7999906]
aux.RegisterItemEvent(7999907,1,"I7999907_OnUse");	--使用物品测试广播7[7999907]
aux.RegisterItemEvent(7999908,1,"I7999908_OnUse");	--使用物品测试广播8[7999908]


--玩家完成任务相关的事件注册
aux.RegisterQuestEvent(11017,1 ,"Bqc11017"); --玩家完成任务 闯三关-下篇[11017]
