--15029 任务接取后添加BUFF防毒[2019901]

function RKL029_OnAccept(MapID, InstanceID, QuestID, RoleID, NPCID)
   unit.AddBuff(MapID, InstanceID, RoleID, 2019901, RoleID)
end

aux.RegisterQuestEvent(15029, 0, "RKL029_OnAccept")