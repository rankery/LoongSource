--ʹ�ü��ܺ���

function s2311503_Cast(mapid, instanceid, skillid, ownerid)
     local TypeID = cre.GetCreatureTypeID(mapid,instanceid, ownerid)
     if TypeID == 1004425 then
     local temp = math.random(1,100)
	    if temp>=1 and temp<=20 then
		    --20%���ʺ���
                 cre.MonsterSay(mapid, instanceid, ownerid, 10086)
           end
     end
     return 0
end


--ע��

aux.RegisterSkillEvent(2311503, 1, "s2311503_Cast")