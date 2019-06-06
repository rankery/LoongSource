--是否可以使用破军图
function I1350002_CanUseItem(MapID, InstanceID, TypeID, TargetID)
    if role.IsRoleHaveQuest(MapID, InstanceID, TargetID, 20034) or role.IsRoleHaveQuest(MapID, InstanceID, TargetID, 20036) then
        return 45, false
    elseif role.IsRoleHaveQuest(MapID, InstanceID, TargetID, 20035) then
    	return 41, false
    end

	--判断背包是否还有地方放璇玑宝图
	local BagFreeSize = role.GetBagFreeSize(TargetID)
	if(BagFreeSize <= 0) then
		return 40, false
	end

	return 0, false
end

--是否可以使用璇玑宝图
function I1350005_CanUseItem(MapID, InstanceID, TypeID, TargetID)
    --检测玩家已到达了指定坐标点
	local bool = WaBao_EnterNearArea(MapID, InstanceID, TargetID)
	--如果玩家不在指定坐标附近，将指定坐标发给客户端
	if bool == false then
		--提示玩家 “寻宝地点：###[###,###]”
		local a,b,c,localMap = role.GetRoleScriptData(TargetID, 4, RoleDataType["PositionA"], RoleDataType["PositionB"], RoleDataType["PositionC"], RoleDataType["PositionMap"])
	    local MsgID = msg.BeginMsgEvent()
	    msg.AddMsgEvent(MsgID, 71, 62)
	    msg.AddMsgEvent(MsgID, 6, localMap)
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

--使用破军图（七星寻宝任务几率获得）
function I1350002_UseItem(MapID, InstanceID, TypeID, TargetID)
	--接到璇玑寻宝活动任务（为了防止添加任务不成功却给了玩家宝图，所以将接任务的操作放在这里）
	role.AddQuest(TargetID, 20035)
	--获得璇玑宝图(1350005)
	role.QuestAddRoleItem(MapID, InstanceID, TargetID, 20035, 1350005, 1, -1, 300)
	--在任一指定的可选择场景中选择一个坐标点
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

--使用璇玑宝图(1350005)
function I1350005_UseItem(MapID, InstanceID, TypeID, TargetID)
	--如果已经到达指定坐标点，则开始挖宝
	XuanJi_DigTreasure(MapID, InstanceID, TargetID)
	--任务完成
	role.RoleCompleteQuest(TargetID, 20035, -1)
	--再次生成随机坐标
	WaBao_GetPoint(MapID, InstanceID, TargetID)
end

--任务完成
function x20035_OnComplete(mapID, instanceID, questID, ownerID, enderID)
end



--取消任务
function x20035_OnDelete(mapID, instanceID, questID, ownerID)
	--取消任务要删除璇玑宝图
	role.RemoveFromRole(mapID, instanceID, ownerID, 1350005, 1, 103)
end

aux.RegisterItemEvent(1350002, 0, "I1350002_CanUseItem")
aux.RegisterItemEvent(1350005, 0, "I1350005_CanUseItem")
aux.RegisterItemEvent(1350002, 1, "I1350002_UseItem")
aux.RegisterItemEvent(1350005, 1, "I1350005_UseItem")
aux.RegisterQuestEvent(20035, 1, "x20035_OnComplete")
aux.RegisterQuestEvent(20035, 2, "x20035_OnDelete")

