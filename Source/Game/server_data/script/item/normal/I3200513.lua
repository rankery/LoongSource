

--[[在地图魔王殿（3000523471），礼天殿（3000521935），嫘祖之谷（3000523727）使用物品聚灵之尘（3200513），可以按照不同的地图刷出怪物和NPC：
m46昆仑密道3084408015在500, 5664, 1763刷出乾虚之灵1531011；
m44玉虚峰3084408527在812, 940, 380刷出雪痕之灵1531012；
魔王殿（3000523471）在246，15541，713刷出峒巫之灵（1531004）；
礼天殿（3000521935）在697，11590，1804刷出血唳之灵（1531001）；
嫘祖之谷（3000523727）在1510，10460，1408刷出啖炎之灵（1531003）。
使用物品前判断是否进入怪物刷新点一定的距离以内（范围可以稍大），在使用物品的时候先判断该位是否为1，如果是则无法召唤。"
]]

pos = {}
pos[1] = {x= 246,z= 713} --m18魔王殿
pos[2] = {x= 697,z= 1804} --m16礼天殿
pos[3] = {x= 1510,z= 1408} --m19嫘祖之谷
pos[4] = {x= 500,z= 1763} --m46昆仑密道
pos[5] = {x= 812,z= 380} --m44玉虚峰
pos[6] = {x= 246,z= 713} --m18
pos[7] = {x= 246,z= 713} --m18
pos[8] = {x= 246,z= 713} --m18
pos[9] = {x= 246,z= 713} --m18
function I3200513_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	if MapID == 3000523471 then --m18
	map.MapCreateColCreature(MapID, InstanceID, 1531004, 246, 15541, 713, 1, "")
	--map.MapCreateCreature(MapID, InstanceID, 3011006, 237, 15541, 701)
	map.SetMapScriptData(MapID,InstanceID,1,3,1) --对应副本中索引值，到副本lua内查询

	elseif MapID == 3000521935 then  --m16
	map.MapCreateColCreature(MapID, InstanceID, 1531001, 697, 11590, 1804, 1, "")
	--map.MapCreateCreature(MapID, InstanceID, 3011006, 685, 11590, 1789)
	map.SetMapScriptData(MapID,InstanceID,1,0,1)

	elseif MapID == 3000523727 then --m19
	map.MapCreateColCreature(MapID, InstanceID, 1531003, 1510, 10460, 1408, 1, "")
	--map.MapCreateCreature(MapID, InstanceID, 3011006, 1522, 10460, 1395)
	map.SetMapScriptData(MapID,InstanceID,1,0,1)

	elseif MapID == 3084408015 then --m46
	map.MapCreateColCreature(MapID, InstanceID, 1531011, 500, 5664, 1763, 1, "")
	map.SetMapScriptData(MapID,InstanceID,1,4,1)

        elseif MapID == 3084408527 then --m44 
	map.MapCreateColCreature(MapID, InstanceID, 1531012, 812, 940, 380, 1, "")
	map.SetMapScriptData(MapID,InstanceID,1,23,1)

	end
end


function I3200513_CanUse(MapID, InstanceID, TypeID, TargetID)
	local num = 0
	local x,y,z = unit.GetPosition(MapID, InstanceID, TargetID)
	if MapID == 3000523471 then --m18
		num = map.GetMapScriptData(MapID,InstanceID,1,3)
		if num == 1 then
			return 32,false
		else
			if (x > pos[1].x-50 and x < pos[1].x+50) and (z > pos[1].z-50 and z < pos[1].z+50) then
			return 0,false
			else
			return 43,false
			end
		end
	elseif MapID == 3000521935 then  --m16
		num = map.GetMapScriptData(MapID,InstanceID,1,0)
		if num == 1 then
			return 32,false
		else
			if (x > pos[2].x-50 and x < pos[2].x+50) and (z > pos[2].z-50 and z < pos[2].z+50) then
			return 0,false
			else
			return 43,false
			end
		end
	elseif MapID == 3000523727 then --m19
		num = map.GetMapScriptData(MapID,InstanceID,1,0)
		if num == 1 then
			return 32,false
		else
			if (x > pos[3].x-50 and x < pos[3].x+50) and (z > pos[3].z-50 and z < pos[3].z+50) then
			return 0,false
			else
			return 43,false
			end
		end
	elseif MapID == 3084408015 then --m46
		num = map.GetMapScriptData(MapID,InstanceID,1,4)
		if num == 1 then
			return 32,false
		else
			if (x > pos[4].x-50 and x < pos[4].x+50) and (z > pos[4].z-50 and z < pos[4].z+50) then
			return 0,false
			else
			return 43,false
			end
		end
	elseif MapID == 3084408527 then --m44
		num = map.GetMapScriptData(MapID,InstanceID,1,23)
		if num == 1 then
			return 32,false
		else
			if (x > pos[5].x-50 and x < pos[5].x+50) and (z > pos[5].z-50 and z < pos[5].z+50) then
			return 0,false
			else
			return 43,false
			end
		end
	else
		return 43,false
	end
end



aux.RegisterItemEvent(3200513, 1, "I3200513_QuestUsable")
aux.RegisterItemEvent(3200513, 0, "I3200513_CanUse")
