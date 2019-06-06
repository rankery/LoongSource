--使用技能喊话

function s2424601_Cast(mapid, instanceid, skillid, ownerid)
     local TypeID = cre.GetCreatureTypeID(mapid,instanceid, ownerid)
     if TypeID == 1009020 or 1009021 then
     local temp = math.random(1,100)
	    if temp>=1 and temp<=20 then
		    --20%几率喊话
                 cre.MonsterSay(mapid, instanceid, ownerid, 50005)
           end
     end
     return 0
end


--注册

aux.RegisterSkillEvent(2424601, 1, "s2424601_Cast")

