--使用技能喊话

function s2315501_Cast(mapid, instanceid, skillid, ownerid)
     local TypeID = cre.GetCreatureTypeID(mapid,instanceid, ownerid)
     if TypeID == 1005419 then
     local temp = math.random(1,100)
	    if temp>=1 and temp<=20 then
		    --20%几率喊话
                 cre.MonsterSay(mapid, instanceid, ownerid, 20060)
           end
     end
     return 0
end


--注册

aux.RegisterSkillEvent(2315501, 1, "s2315501_Cast")
