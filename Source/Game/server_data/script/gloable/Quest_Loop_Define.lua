--任务环相关属性定义

QuestTargetType = {}
QuestTargetType["Kill"]    = 1		--击杀某种怪物
QuestTargetType["Collect"] = 2		--收集物品
QuestTargetType["NPCTalk"] = 3		--NPC对话


--得到随机任务table中一个任务的相关属性（tablename 随机任务表，Num 表的最大索引值）
function GetRandQuestTable(tablename, Index)
	--击杀怪物类型的任务
	if tablename[Index].TargetType == QuestTargetType["Kill"] then
		--返回怪物ID和数量
		if tablename[Index].MsgID then
		    return QuestTargetType["Kill"], tablename[Index].MsgID, tablename[Index].TargetID1, tablename[Index].Num1, tablename[Index].TargetID2, tablename[Index].Num2, tablename[Index].TargetID3, tablename[Index].Num3, tablename[Index].TargetID4, tablename[Index].Num4
		else
		    return QuestTargetType["Kill"], 0, tablename[Index].TargetID1, tablename[Index].Num1, tablename[Index].TargetID2, tablename[Index].Num2, tablename[Index].TargetID3, tablename[Index].Num3, tablename[Index].TargetID4, tablename[Index].Num4
		end
	end

	--收集物品类型的任务
	if tablename[Index].TargetType == QuestTargetType["Collect"] then
		--返回物品TypeID，和数量
		if tablename[Index].MsgID then
		    return QuestTargetType["Collect"], tablename[Index].MsgID, tablename[Index].TargetID1, tablename[Index].Num1, tablename[Index].TargetID2, tablename[Index].Num2, tablename[Index].TargetID3, tablename[Index].Num3, tablename[Index].TargetID4, tablename[Index].Num4
		else
		    return QuestTargetType["Collect"], 0, tablename[Index].TargetID1, tablename[Index].Num1, tablename[Index].TargetID2, tablename[Index].Num2, tablename[Index].TargetID3, tablename[Index].Num3, tablename[Index].TargetID4, tablename[Index].Num4    
		end
	end

	--NPC对话类型的任务
	if tablename[Index].TargetType == QuestTargetType["NPCTalk"] then
		--返回NPCTypeID
		if tablename[Index].MsgID then
		    return QuestTargetType["NPCTalk"], tablename[Index].MsgID, tablename[Index].TargetID1, tablename[Index].TargetID2, tablename[Index].TargetID3, tablename[Index].TargetID4
	    else
	        return QuestTargetType["NPCTalk"], 0, tablename[Index].TargetID1, tablename[Index].TargetID2, tablename[Index].TargetID3, tablename[Index].TargetID4 
	    end
    end
end

