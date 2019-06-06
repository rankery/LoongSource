
--[食]永福饺子(3303029)

function I3303029_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	role.AddRoleExp(MapID, InstanceID, TargetID, 20000)
end

aux.RegisterItemEvent(3303029, 1, "I3303029_QuestUsable")


--[食]名人饺子(7999909)

function I7999909_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	role.AddRoleExp(MapID, InstanceID, TargetID, 10000000)
end

aux.RegisterItemEvent(7999909, 1, "I7999909_QuestUsable")




--[食]高升饺子(3303030)

function I3303030_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	role.AddRoleExp(MapID, InstanceID, TargetID, 20000)
end

aux.RegisterItemEvent(3303030, 1, "I3303030_QuestUsable")

--[食]飞升饺子(3303056)

function I3303056_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	role.AddRoleExp(MapID, InstanceID, TargetID, 500000)
end

aux.RegisterItemEvent(3303056, 1, "I3303056_QuestUsable")


--[食]飞升娇耳（3303057、3303058)

function I3303057_canuse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local targetlevel = role.GetRoleLevel(MapID, InstanceID, TargetID)
	if targetlevel < 50 or targetlevel > 100   then
        bRet = 32
	end
	return bRet, bIgnore
end

function I3303057_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	role.AddRoleExp(MapID, InstanceID, TargetID, 500000)
end

aux.RegisterItemEvent(3303057, 0, "I3303057_canuse")
aux.RegisterItemEvent(3303058, 0, "I3303057_canuse")
aux.RegisterItemEvent(3303057, 1, "I3303057_QuestUsable")
aux.RegisterItemEvent(3303058, 1, "I3303057_QuestUsable")

function I3303059_canuse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local targetlevel = role.GetRoleLevel(MapID, InstanceID, TargetID)
	if targetlevel < 50 or targetlevel > 110   then
        bRet = 32
	end
	return bRet, bIgnore
end

function I3303059_OnUse(MapID, InstanceID, TypeID, TargetID)
	role.AddRoleExp(MapID, InstanceID, TargetID, 500000)
end
aux.RegisterItemEvent(3303059, 0, "I3303059_canuse")
aux.RegisterItemEvent(3303059, 1, "I3303059_OnUse")
aux.RegisterItemEvent(3303060, 0, "I3303059_canuse")
aux.RegisterItemEvent(3303060, 1, "I3303059_OnUse")

--[食]盛世饺子(3303031)

expadd = {}
expadd[1]=	600000000
expadd[2]=	600000000
expadd[3]=	600000000
expadd[4]=	600000000
expadd[5]=	600000000
expadd[6]=	600000000
expadd[7]=	600000000
expadd[8]=	600000000
expadd[9]=	600000000
expadd[10]=	600000000
expadd[11]=	600000000
expadd[12]=	600000000
expadd[13]=	600000000
expadd[14]=	600000000
expadd[15]=	600000000
expadd[16]=	600000000
expadd[17]=	600000000
expadd[18]=	600000000
expadd[19]=	600000000
expadd[20]=	600000000
expadd[21]=	600000000
expadd[22]=	600000000
expadd[23]=	600000000
expadd[24]=	600000000
expadd[25]=	600000000
expadd[26]=	600000000
expadd[27]=	600000000
expadd[28]=	600000000
expadd[29]=	600000000
expadd[30]=	600000000
expadd[31]=	600000000
expadd[32]=	600000000
expadd[33]=	600000000
expadd[34]=	600000000
expadd[35]=	600000000
expadd[36]=	600000000
expadd[37]=	600000000
expadd[38]=	600000000
expadd[39]=	600000000
expadd[40]=	600000000
expadd[41]=	600000000
expadd[42]=	600000000
expadd[43]=	600000000
expadd[44]=	600000000
expadd[45]=	600000000
expadd[46]=	600000000
expadd[47]=	600000000
expadd[48]=	600000000
expadd[49]=	600000000
expadd[50]=	600000000
expadd[51]=	600000000
expadd[52]=	600000000
expadd[53]=	600000000
expadd[54]=	600000000
expadd[55]=	600000000
expadd[56]=	600000000
expadd[57]=	600000000
expadd[58]=	600000000
expadd[59]=	600000000
expadd[60]=	600000000
expadd[61]=	600000000
expadd[62]=	600000000
expadd[63]=	600000000
expadd[64]=	600000000
expadd[65]=	600000000
expadd[66]=	600000000
expadd[67]=	600000000
expadd[68]=	600000000
expadd[69]=	600000000
expadd[70]=	600000000
expadd[71]=	600000000
expadd[72]=	600000000
expadd[73]=	600000000
expadd[74]=	600000000
expadd[75]=	600000000
expadd[76]=	600000000
expadd[77]=	600000000
expadd[78]=	600000000
expadd[79]=	600000000
expadd[80]=	600000000
expadd[81]=	600000000
expadd[82]=	600000000
expadd[83]=	600000000
expadd[84]=	600000000
expadd[85]=	600000000
expadd[86]=	600000000
expadd[87]=	600000000
expadd[88]=	600000000
expadd[89]=	600000000
expadd[90]=	600000000
expadd[91]=	600000000
expadd[92]=	600000000
expadd[93]=	600000000
expadd[94]=	600000000
expadd[95]=	600000000
expadd[96]=	600000000
expadd[97]=	600000000
expadd[98]=	600000000
expadd[99]=	600000000
expadd[100]= 600000000
expadd[101]= 600000000
expadd[102]= 600000000
expadd[103]= 600000000
expadd[104]= 600000000
expadd[105]= 600000000
expadd[106]= 600000000
expadd[107]= 600000000
expadd[108]= 600000000
expadd[109]= 600000000
expadd[110]= 600000000


function I3303031_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	local a = role.GetRoleLevel(MapID, InstanceID, TargetID)
	if a > 110 then
	    a = 110
	end
		role.AddRoleExp(MapID, InstanceID, TargetID, expadd[a])
end

aux.RegisterItemEvent(3303031, 1, "I3303031_QuestUsable")


--青团
function I3303711_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	local a = role.GetRoleLevel(MapID, InstanceID, TargetID)
	if a > 110 then
	    a = 110
	end
		role.AddRoleExp(MapID, InstanceID, TargetID, expadd[a]/10)
end

aux.RegisterItemEvent(3303711, 1, "I3303711_QuestUsable")











