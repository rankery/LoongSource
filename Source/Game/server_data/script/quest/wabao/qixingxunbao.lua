--是否可以使用七杀图,破军图，贪狼图
function I1350001_CanUseItem(MapID, InstanceID, TypeID, TargetID)
    if role.IsRoleHaveQuest(MapID, InstanceID, TargetID, 20035) or role.IsRoleHaveQuest(MapID, InstanceID, TargetID, 20036) then
    	return 45, false
    elseif role.IsRoleHaveQuest(MapID, InstanceID, TargetID, 20034) then
    	return 41, false
    end

	--判断背包是否还有地方放七星宝图
	local BagFreeSize = role.GetBagFreeSize(TargetID)
	if(BagFreeSize <= 0) then
		return 40, false
	end

	return 0, false
end

--是否可以使用七星宝图
function I1350004_CanUseItem(MapID, InstanceID, TypeID, TargetID)
	--检测玩家已到达了指定坐标点
	local bool = WaBao_EnterNearArea(MapID, InstanceID, TargetID)
	--如果玩家不在指定坐标附近，将指定坐标发给客户端
	if bool == false then
		--提示玩家 “寻宝地点：###[###,###]”
		local a,b,c,localMap = role.GetRoleScriptData(TargetID, 4, RoleDataType["PositionA"], RoleDataType["PositionB"], RoleDataType["PositionC"], RoleDataType["PositionMap"])
	  --将地图crc值截成两半
		--local MapI = (localMap - localMap%100000)/100000
		--local MapII = localMap%100000

	    local MsgID = msg.BeginMsgEvent()
	   -- msg.AddMsgEvent(MsgID, 13, 0)
	    msg.AddMsgEvent(MsgID, 71, 62)
	    msg.AddMsgEvent(MsgID, 6, localMap)
	    --msg.AddMsgEvent(MsgID, 6, MapI)
			--msg.AddMsgEvent(MsgID, 6, MapII)
	    msg.AddMsgEvent(MsgID, 9, a)
	    msg.AddMsgEvent(MsgID, 9, c)
	    msg.DispatchRoleMsgEvent(TargetID, MsgID)
	    return 36, false
    else
		local bRet, bIgnore = 0, false
		--判断背包空闲空间是否足够
		local FreeSize = role.GetBagFreeSize(TargetID)
		if(FreeSize < 1) then
		--提示玩家背包空间不足
			bRet = 40
		end
		return bRet, bIgnore
	end

end

--使用七杀图（七星迷踪任务获得）
function I1350001_UseItem(MapID, InstanceID, TypeID, TargetID)
	--接到七星寻宝活动任务
  role.AddQuest(TargetID, 20034)
	--获得七星宝图(1350004)
	role.QuestAddRoleItem(MapID, InstanceID, TargetID, 20034, 1350004, 1, -1, 300)
	--随机生成某个场景中的一个坐标点
	WaBao_GetPoint(MapID, InstanceID, TargetID)
	--将生成的坐标点发给客户端
	--“寻宝地点：###[###,###]”
	local localA,localB,localC,localMap = role.GetRoleScriptData(TargetID, 4, RoleDataType["PositionA"], RoleDataType["PositionB"], RoleDataType["PositionC"], RoleDataType["PositionMap"])
	--将地图crc值截成两半
	--local MapI = (localMap - localMap%100000)/100000
	--local MapII = localMap%100000

	local MsgID = msg.BeginMsgEvent()
	--msg.AddMsgEvent(MsgID, 13, 0)
	msg.AddMsgEvent(MsgID, 71, 62)
	msg.AddMsgEvent(MsgID, 6, localMap)
	--msg.AddMsgEvent(MsgID, 6, MapI)
	--msg.AddMsgEvent(MsgID, 6, MapII)
	msg.AddMsgEvent(MsgID, 9, localA)
	msg.AddMsgEvent(MsgID, 9, localC)
	msg.DispatchRoleMsgEvent(TargetID, MsgID)
end

--使用七星宝图
function I1350004_UseItem(MapID, InstanceID, TypeID, TargetID)
	--到达指定坐标点,开始挖宝
	QiXing_DigTreasure(MapID, InstanceID, TargetID)
	--任务完成
	role.RoleCompleteQuest(TargetID, 20034, -1)
	--再次生成随机坐标
	WaBao_GetPoint(MapID, InstanceID, TargetID)
end

--任务完成
function x20034_OnComplete(mapID, instanceID, questID, ownerID, enderID)
end

--取消任务
function x20034_OnDelete(mapID, instanceID, questID, ownerID)
	--取消任务要删除七星宝图
	role.RemoveFromRole(mapID, instanceID, ownerID, 1350004, 1, 103)
end


aux.RegisterItemEvent(1350001, 0, "I1350001_CanUseItem")
aux.RegisterItemEvent(1350004, 0, "I1350004_CanUseItem")
aux.RegisterItemEvent(1350001, 1, "I1350001_UseItem")
aux.RegisterItemEvent(1350004, 1, "I1350004_UseItem")
aux.RegisterQuestEvent(20034, 1, "x20034_OnComplete")
aux.RegisterQuestEvent(20034, 2, "x20034_OnDelete")

