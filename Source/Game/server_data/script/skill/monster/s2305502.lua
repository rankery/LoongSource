--ʹ�ü��ܺ���

function s2305502_Cast(mapid, instanceid, skillid, ownerid)
     local TypeID = cre.GetCreatureTypeID(mapid,instanceid, ownerid)
     if TypeID == 1004528 then
     local temp = math.random(1,100)
	    if temp>=1 and temp<=20 then
		    --20%���ʺ���
                 cre.MonsterSay(mapid, instanceid, ownerid, 10062)
           end
     elseif TypeID == 1004530 then
     local temp = math.random(1,100)
	    if temp>=1 and temp<=20 then
		    --20%���ʺ���
                 cre.MonsterSay(mapid, instanceid, ownerid, 10068)
           end
     end
     return 0
end


--ע��

aux.RegisterSkillEvent(2305502, 1, "s2305502_Cast")
