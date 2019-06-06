local function xiulianchang_award(MapID,InstanceID,RoleID,ItemID1,ItemID2,ItemID3,ItemID4)
	local r = math.random(100)
	if r > 79 then
		role.AddRoleItem(MapID, InstanceID, RoleID, ItemID1, 1, -1, 8, 102)
	elseif r > 58 then
		role.AddRoleItem(MapID, InstanceID, RoleID, ItemID2, 1, -1, 8, 102)
	elseif r > 29 then
		role.AddRoleItem(MapID, InstanceID, RoleID, ItemID3, 1, -1, 8, 102)
	elseif r > 0 then
		role.AddRoleItem(MapID, InstanceID, RoleID, ItemID4, 1, -1, 8, 102)
	end

	local exp_award = {}
	exp_award[25]=12123
	exp_award[35]=31981
	exp_award[45]=87946
	exp_award[55]=200918
	exp_award[65]=298201
	exp_award[75]=328322
	exp_award[85]=474676
	exp_award[95]=764874

	local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
	rolelevel = math.floor(rolelevel/10)*10
	if rolelevel > 90 then
	    rolelevel = 90
	end
	role.AddRoleExp(MapID, InstanceID, RoleID, exp_award[rolelevel+5])

end

function q20203_20204_OnComplete(MapID, InstanceID, QuestID, RoleID, NPCID)
	xiulianchang_award(MapID, InstanceID,RoleID,1330007,1330008,1330009,1330010)
	--增加进阶实力值
	Increase_shili(MapID, InstanceID, RoleID, 0, 4, "jinjieshili_Q")
end

function q20205_20206_20207_OnComplete(MapID, InstanceID, QuestID, RoleID, NPCID)
	xiulianchang_award(MapID, InstanceID,RoleID,1330011,1330012,1330013,1330014)
	--增加进阶实力值
	Increase_shili(MapID, InstanceID, RoleID, 0, 4, "jinjieshili_Q")
end

function q20208_20209_20210_OnComplete(MapID, InstanceID, QuestID, RoleID, NPCID)
	xiulianchang_award(MapID, InstanceID,RoleID,1330015,1330016,1330017,1330018)
	--增加进阶实力值
	Increase_shili(MapID, InstanceID, RoleID, 0, 4, "jinjieshili_Q")
end

aux.RegisterQuestEvent(20203, 1, "q20203_20204_OnComplete")
aux.RegisterQuestEvent(20204, 1, "q20203_20204_OnComplete")
aux.RegisterQuestEvent(20205, 1, "q20205_20206_20207_OnComplete")
aux.RegisterQuestEvent(20206, 1, "q20205_20206_20207_OnComplete")
aux.RegisterQuestEvent(20207, 1, "q20205_20206_20207_OnComplete")
aux.RegisterQuestEvent(20208, 1, "q20208_20209_20210_OnComplete")
aux.RegisterQuestEvent(20209, 1, "q20208_20209_20210_OnComplete")
aux.RegisterQuestEvent(20210, 1, "q20208_20209_20210_OnComplete")


function xiulian_OnCheckComplete(MapID, InstanceID, QuestID, RoleID, NPCID)
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(RoleID)
	if(FreeSize < 2) then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 142)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end
	return 1
end
aux.RegisterQuestEvent(20203, 5, "xiulian_OnCheckComplete")
aux.RegisterQuestEvent(20204, 5, "xiulian_OnCheckComplete")
aux.RegisterQuestEvent(20205, 5, "xiulian_OnCheckComplete")
aux.RegisterQuestEvent(20206, 5, "xiulian_OnCheckComplete")
aux.RegisterQuestEvent(20207, 5, "xiulian_OnCheckComplete")
aux.RegisterQuestEvent(20208, 5, "xiulian_OnCheckComplete")
aux.RegisterQuestEvent(20209, 5, "xiulian_OnCheckComplete")
aux.RegisterQuestEvent(20210, 5, "xiulian_OnCheckComplete")
