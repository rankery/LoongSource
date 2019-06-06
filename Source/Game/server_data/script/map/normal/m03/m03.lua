
m03_Trigger = {} -- ¼ÇÂ¼trigger¼¤»îÆµÂÊî
m03_Trigger[1] = {ID = 6631, Time = 0}
m03_Trigger[2] = {ID = 6632, Time = 0}
m03_Trigger[3] = {ID = 6634, Time = 0}
m03_Trigger[4] = {ID = 6635, Time = 0}
m03_Trigger[5] = {ID = 6636, Time = 0}
m03_Trigger[6] = {ID = 6637, Time = 0}

--¼ÇÂ¼m03µØÍ¼ScriptData¹ÖÎïÍ¬Ê±´æÔÚµÄÊıÁ¿
--m03µØÍ¼Ë÷Òı1£¬11279 ¾ÆÍÌÍ¯×Ó[1004607]¼ÇÂ¼ÊıÁ¿
--m03µØÍ¼Ë÷Òı2£¬11905 íöÄ¿¾ŞÔ³[1534134]¼ÇÂ¼ÊıÁ¿

--¹ÖÎïÏûÊ§»òÕßËÀÍö£¬½«ScriptDataË÷Òı1 ÊıÖµ¼õ1
function m03_OnDisappear(MapID,InstanceID,CreatureID,CreatureTypeID)
	if CreatureTypeID == 1004607 then
		local a = map.GetMapScriptData(MapID,InstanceID,1,1)
		map.SetMapScriptData(MapID,InstanceID,1,1,a-1)
	end
	if CreatureTypeID == 1534134 then  --11905
		local a = map.GetMapScriptData(MapID,InstanceID,1,2)
		map.SetMapScriptData(MapID,InstanceID,1,2,a-1)
	end
end
aux.RegisterMapEvent("m03", 18, "m03_OnDisappear")


--Íæ¼ÒÓĞÈÎÎñ11305£¬½øÈë´¥·¢Æ÷id£º6631 (m03,543,13468,902),Ë¢ĞÂ¹ÖÎï Ã°ÅÆ·Ë¾ü[1004249]X5,Ã°ÅÆ·ËÊ×[1004250]X1
function m03_OnEnterTrigger(MapID, InstanceID, RoleID, ObjID)
	local bHaveQuest = role.IsRoleHaveQuest(MapID, InstanceID, RoleID, 11305)
	if bHaveQuest then
		if ObjID == 6631 then
		local cTime = os.time() --µ±Ç°Ê±¼ä
		local dTime = cTime - m03_Trigger[1].Time --ÉÏ´Î´¥·¢¾àÏÖÔÚÊ±¼ä
			if dTime > 180 then --ÅĞ¶Ï´¥·¢¼ä¸ôÊÇ·ñ´óÓÚ3·ÖÖÓ
			local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
			map.MapCreateCreature(MapID, InstanceID, 1004249, x-4, y+1, z-2)
			map.MapCreateCreature(MapID, InstanceID, 1004249, x-2, y+1, z-2)
			map.MapCreateCreature(MapID, InstanceID, 1004249, x, y+1, z-2)
			map.MapCreateCreature(MapID, InstanceID, 1004249, x+2, y+1, z-2)
			map.MapCreateCreature(MapID, InstanceID, 1004249, x+4, y+1, z-2)

			map.MapCreateCreature(MapID, InstanceID, 1004250, x, y+1, z+2)
			m03_Trigger[1].Time = os.time() --¼ÇÂ¼´Ë´¥·¢Æ÷×îĞÂÉúĞ§Ê±¼ä
			end

		--Íæ¼ÒÓĞÈÎÎñ11305£¬½øÈë´¥·¢Æ÷id£º6632 (m03,621,11250,658),Ë¢ĞÂ¹ÖÎï Ã°ÅÆ·Ë¾ü[1004249]X5,Ã°ÅÆ·ËÊ×[1004250]X1
		elseif ObjID == 6632 then
			local cTime = os.time() --µ±Ç°Ê±¼ä
			local dTime = cTime - m03_Trigger[2].Time --ÉÏ´Î´¥·¢¾àÏÖÔÚÊ±¼ä
			if dTime > 180 then --ÅĞ¶Ï´¥·¢¼ä¸ôÊÇ·ñ´óÓÚ3·ÖÖÓ
			local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
			map.MapCreateCreature(MapID, InstanceID, 1004249, x-4, y+1, z-2)
			map.MapCreateCreature(MapID, InstanceID, 1004249, x-2, y+1, z-2)
			map.MapCreateCreature(MapID, InstanceID, 1004249, x, y+1, z-2)
			map.MapCreateCreature(MapID, InstanceID, 1004249, x+2, y+1, z-2)
			map.MapCreateCreature(MapID, InstanceID, 1004249, x+4, y+1, z-2)

			map.MapCreateCreature(MapID, InstanceID, 1004250, x, y+1, z+2)
			m03_Trigger[2].Time = os.time() --¼ÇÂ¼´Ë´¥·¢Æ÷×îĞÂÉúĞ§Ê±¼ä
			end

		--Íæ¼ÒÓĞÈÎÎñ11305£¬½øÈë´¥·¢Æ÷id£º6634 (m03,863,9428,566),Ë¢ĞÂ¹ÖÎï Ã°ÅÆ·Ë¾ü[1004249]X5,Ã°ÅÆ·ËÊ×[1004250]X1
		elseif ObjID == 6634 then
			local cTime = os.time() --µ±Ç°Ê±¼ä
			local dTime = cTime - m03_Trigger[3].Time --ÉÏ´Î´¥·¢¾àÏÖÔÚÊ±¼ä
			if dTime > 180 then --ÅĞ¶Ï´¥·¢¼ä¸ôÊÇ·ñ´óÓÚ3·ÖÖÓ
			local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
			map.MapCreateCreature(MapID, InstanceID, 1004249, x-4, y+1, z-2)
			map.MapCreateCreature(MapID, InstanceID, 1004249, x-2, y+1, z-2)
			map.MapCreateCreature(MapID, InstanceID, 1004249, x, y+1, z-2)
			map.MapCreateCreature(MapID, InstanceID, 1004249, x+2, y+1, z-2)
			map.MapCreateCreature(MapID, InstanceID, 1004249, x+4, y+1, z-2)
			map.MapCreateCreature(MapID, InstanceID, 1004250, x, y+1, z+2)
			m03_Trigger[3].Time = os.time() --¼ÇÂ¼´Ë´¥·¢Æ÷×îĞÂÉúĞ§Ê±¼ä
			end

		--Íæ¼ÒÓĞÈÎÎñ11305£¬½øÈë´¥·¢Æ÷id£º6635 (m03,900,7692,745),Ë¢ĞÂ¹ÖÎï Ã°ÅÆ·Ë¾ü[1004249]X5,Ã°ÅÆ·ËÊ×[1004250]X1
		elseif ObjID == 6635 then
			local cTime = os.time() --µ±Ç°Ê±¼ä
			local dTime = cTime - m03_Trigger[4].Time --ÉÏ´Î´¥·¢¾àÏÖÔÚÊ±¼ä
			if dTime > 180 then --ÅĞ¶Ï´¥·¢¼ä¸ôÊÇ·ñ´óÓÚ3·ÖÖÓ
			local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
			map.MapCreateCreature(MapID, InstanceID, 1004249, x-4, y+1, z-2)
			map.MapCreateCreature(MapID, InstanceID, 1004249, x-2, y+1, z-2)
			map.MapCreateCreature(MapID, InstanceID, 1004249, x, y+1, z-2)
			map.MapCreateCreature(MapID, InstanceID, 1004249, x+2, y+1, z-2)
			map.MapCreateCreature(MapID, InstanceID, 1004249, x+4, y+1, z-2)

			map.MapCreateCreature(MapID, InstanceID, 1004250, x, y+1, z+2)

			m03_Trigger[4].Time = os.time() --¼ÇÂ¼´Ë´¥·¢Æ÷×îĞÂÉúĞ§Ê±¼ä
			end

		--Íæ¼ÒÓĞÈÎÎñ11305£¬½øÈë´¥·¢Æ÷id£º6636 (m03,422,13918,994),Ë¢ĞÂ¹ÖÎï Ã°ÅÆ·Ë¾ü[1004249]X5,Ã°ÅÆ·ËÊ×[1004250]X1
		elseif ObjID == 6636 then
			local cTime = os.time() --µ±Ç°Ê±¼ä
			local dTime = cTime - m03_Trigger[5].Time --ÉÏ´Î´¥·¢¾àÏÖÔÚÊ±¼ä
			if dTime > 180 then --ÅĞ¶Ï´¥·¢¼ä¸ôÊÇ·ñ´óÓÚ3·ÖÖÓ
			local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
			map.MapCreateCreature(MapID, InstanceID, 1004249, x-4, y+1, z-2)
			map.MapCreateCreature(MapID, InstanceID, 1004249, x-2, y+1, z-2)
			map.MapCreateCreature(MapID, InstanceID, 1004249, x, y+1, z-2)
			map.MapCreateCreature(MapID, InstanceID, 1004249, x+2, y+1, z-2)
			map.MapCreateCreature(MapID, InstanceID, 1004249, x+4, y+1, z-2)
			map.MapCreateCreature(MapID, InstanceID, 1004250, x, y+1, z+2)
			m03_Trigger[5].Time = os.time() --¼ÇÂ¼´Ë´¥·¢Æ÷×îĞÂÉúĞ§Ê±¼ä
			end

		--Íæ¼ÒÓĞÈÎÎñ11305£¬½øÈë´¥·¢Æ÷id£º6637 (m03,490,14114,829),Ë¢ĞÂ¹ÖÎï Ã°ÅÆ·Ë¾ü[1004249]X5,Ã°ÅÆ·ËÊ×[1004250]X1
		elseif ObjID == 6637 then
		local cTime = os.time() --µ±Ç°Ê±¼ä
		local dTime = cTime - m03_Trigger[6].Time --ÉÏ´Î´¥·¢¾àÏÖÔÚÊ±¼ä
			if dTime > 180 then --ÅĞ¶Ï´¥·¢¼ä¸ôÊÇ·ñ´óÓÚ3·ÖÖÓ
			local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
			map.MapCreateCreature(MapID, InstanceID, 1004249, x-4, y+1, z-2)
			map.MapCreateCreature(MapID, InstanceID, 1004249, x-2, y+1, z-2)
			map.MapCreateCreature(MapID, InstanceID, 1004249, x, y+1, z-2)
			map.MapCreateCreature(MapID, InstanceID, 1004249, x+2, y+1, z-2)
			map.MapCreateCreature(MapID, InstanceID, 1004249, x+4, y+1, z-2)
			map.MapCreateCreature(MapID, InstanceID, 1004250, x, y+1, z+2)
			m03_Trigger[6].Time = os.time() --¼ÇÂ¼´Ë´¥·¢Æ÷×îĞÂÉúĞ§Ê±¼ä
			end

		end
	end
end
aux.RegisterMapEvent("m03",6,"m03_OnEnterTrigger")
