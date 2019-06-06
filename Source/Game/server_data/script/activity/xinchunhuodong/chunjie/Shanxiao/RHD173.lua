

--任务完成时函数

function q20173_OnComplete(MapID, InstanceID, QuestID, TargetID, AccepterID)
	map.MapCreateColCreature(MapID, InstanceID, 1534104, 321, 6323, 324,1)---------------
end


--山魈项圈奖励
function q20174_OnComplete(MapID, InstanceID, QuestID, RoleID, NPCID)
	 local k = math.random(1000)
	if k <= 50 then
	    role.AddRoleItem(MapID, InstanceID, RoleID, 3303002, 1, -1, 8, 420)
	--服务器广播
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 100, 2125)
	msg.AddMsgEvent(MsgID, 2, RoleID)	--传入玩家名
	msg.DispatchWorldMsgEvent(MsgID)
	end

end

--注册
aux.RegisterQuestEvent(20173, 1, "q20173_OnComplete")
aux.RegisterQuestEvent(20174, 1, "q20174_OnComplete")


function q20174_OnCheckComplete(MapID, InstanceID, QuestID, RoleID, NPCID)

	local FreeSize = role.GetBagFreeSize(RoleID)
	if(FreeSize < 5) then
		--提示玩家背包空间不足
		return 0
	end
	return 1
end

aux.RegisterQuestEvent(20174, 5, "q20174_OnCheckComplete")
