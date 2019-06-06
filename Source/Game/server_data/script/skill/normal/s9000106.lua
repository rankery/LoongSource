
-- 采集资源0.1%几率在附近刷新山神

function s9000106_Cast(Mapid, Instanceid, Skillid, Ownerid)
		if Mapid == 3017298127 then
		  local x, y ,z = unit.GetPosition(Mapid, Instanceid, Ownerid)
	      local temp = math.random(1,1000)
		   if temp==1 then
             map.MapCreateCreature(Mapid, Instanceid, 1534001, x, y, z)
		   end
	    elseif Mapid == 3017298383 then
		  local x, y ,z = unit.GetPosition(Mapid, Instanceid, Ownerid)
	      local temp = math.random(1,1000)
		   if temp==1 then
             map.MapCreateCreature(Mapid, Instanceid, 1534002, x, y, z)
		   end
		elseif Mapid == 3017299663 then
		  local x, y ,z = unit.GetPosition(Mapid, Instanceid, Ownerid)
	      local temp = math.random(1,1000)
		   if temp==1 then
             map.MapCreateCreature(Mapid, Instanceid, 1534003, x, y, z)
		   end
		elseif Mapid == 3017299919 then
		  local x, y ,z = unit.GetPosition(Mapid, Instanceid, Ownerid)
	      local temp = math.random(1,1000)
		   if temp==1 then
             map.MapCreateCreature(Mapid, Instanceid, 1534004, x, y, z)
		   end
		elseif Mapid == 3017299151 then
		  local x, y ,z = unit.GetPosition(Mapid, Instanceid, Ownerid)
	      local temp = math.random(1,1000)
		   if temp==1 then
             map.MapCreateCreature(Mapid, Instanceid, 1534005, x, y, z)
		   end
		elseif Mapid == 3017299407 then
		  local x, y ,z = unit.GetPosition(Mapid, Instanceid, Ownerid)
	      local temp = math.random(1,1000)
		   if temp==1 then
             map.MapCreateCreature(Mapid, Instanceid, 1534006, x, y, z)
		   end
		elseif Mapid == 1146339967 then
		  local x, y ,z = unit.GetPosition(Mapid, Instanceid, Ownerid)
	      local temp = math.random(1,1000)
		   if temp==1 then
             map.MapCreateCreature(Mapid, Instanceid, 1534005, x, y, z)
		   end
	   end
        return 0
   end


aux.RegisterSkillEvent(9000106, 1, "s9000106_Cast")

