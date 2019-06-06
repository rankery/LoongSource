--捉鬼：13807
--需求1：与NPC蒙齐[3040608]，莲凤[3040609]，耶力丹[3040610]对话后分别刷新怪物奔扑霸[1006612]，霸扑奔[1006613]，扑奔霸[1006614]

--NPC对话函数
function x13807_OnNPCTalk(mapID, instanceID, questID, ownerID, npcid, npctypeid)
	if npctypeid == 3040608 then
		local x, y ,z = unit.GetPosition(mapID, instanceID, ownerID)
		map.MapCreateCreature(mapID, instanceID, 1006612, x+2, y, z+2)
	end

	if npctypeid == 3040609 then
		local x, y ,z = unit.GetPosition(mapID, instanceID, ownerID)
		map.MapCreateCreature(mapID, instanceID, 1006613, x+2, y, z+2)
	end

	if npctypeid == 3040610 then
		local x, y ,z = unit.GetPosition(mapID, instanceID, ownerID)
		map.MapCreateCreature(mapID, instanceID, 1006614, x+2, y, z+2)
	end
end

--注册
aux.RegisterQuestEvent(13807, 6, "x13807_OnNPCTalk")
