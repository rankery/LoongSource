--以毒攻毒：11436
--需求1：与地物万毒石[3020223]对话后添加BUFF慢性毒药[2011401]
--需求2：完成任务时删除BUFF慢性毒药[2011401]

--与万毒石对话函数
function x11436_OnNPCTalk(mapID, instanceID, questID, ownerID, npcid, npctypeid)
	if npctypeid == 3020223 then
	    unit.AddBuff(mapID, instanceID, ownerID, 2011401, ownerID)
	end
end

--任务完成函数
function x11436_OnComplete(mapID, instanceID, questID, ownerID, accepterID)
	unit.CancelBuff(mapID, instanceID, ownerID, 2011401)
end

--要不要加任务放弃函数

--注册
aux.RegisterQuestEvent(11436, 6, "x11436_OnNPCTalk")
aux.RegisterQuestEvent(11436, 1, "x11436_OnComplete")


--任务放弃时
function x11436_OnCancel(mapID, instanceID, questID, ownerID, enderID)
	unit.CancelBuff(mapID, instanceID, ownerID, 2011401)
end

--注册
aux.RegisterQuestEvent(11436, 2, "x11436_OnCancel")
