--需求6  1006715 进入战斗后5秒释放技能（2333301），在其位置刷出 1006716

function s2333301_Cast(MapID, InstanceID, SkillID, RoleID)
         local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
		 map.MapCreateCreature(MapID, InstanceID, 1006716, x, y, z)
end

aux.RegisterSkillEvent(2333301, 1, "s2333301_Cast")
