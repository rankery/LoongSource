--使用技能喊话

function s2316101_Cast(mapid, instanceid, skillid, ownerid)
     local TypeID = cre.GetCreatureTypeID(mapid,instanceid, ownerid)
     if TypeID == 1005433 then
     local temp = math.random(1,100)
	    if temp>=1 and temp<=20 then
		    --20%几率喊话
                 cre.MonsterSay(mapid, instanceid, ownerid, 20068)
           end
     end
     return 0
end


--注册

aux.RegisterSkillEvent(2316101, 1, "s2316101_Cast")
