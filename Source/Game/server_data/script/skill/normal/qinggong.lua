--5个技能不能在副本使用（骑乘状态需要先将宠物驯化才能使用）
--先判断能在大地图使用，然后判断不能在副本使用

Skill_MapCRC = {}
Skill_MapCRC[1] = 3017298127 --m02
Skill_MapCRC[2] = 3017298383 --m03
Skill_MapCRC[3] = 3017299663 --m04
Skill_MapCRC[4] = 3017299919 --m05
Skill_MapCRC[5] = 3017299151 --m06
Skill_MapCRC[6] = 1146339967 --m06_1
Skill_MapCRC[7] = 3017299407 --m07
Skill_MapCRC[8] = 3017298895 --m01

Skill_InstanceCRC = {}
Skill_InstanceCRC[1] = 3000522447 --m14
Skill_InstanceCRC[2] = 3000522703 --m15
Skill_InstanceCRC[3] = 3000521935 --m16
Skill_InstanceCRC[4] = 3000522191 --m17
Skill_InstanceCRC[5] = 3000523471 --m18
Skill_InstanceCRC[6] = 3000523727 --m19
Skill_InstanceCRC[7] = 3084407503 --m40
Skill_InstanceCRC[8] = 3084407759 --m41
Skill_InstanceCRC[9] = 3084406991 --m42
Skill_InstanceCRC[10] = 3084407247 --m43
Skill_InstanceCRC[11] = 3084408527 --m44
Skill_InstanceCRC[12] = 3084408783 --m45
Skill_InstanceCRC[13] = 3084408015 --m46
Skill_InstanceCRC[14] = 3084409807 --m49
Skill_InstanceCRC[15] = 3067629775 --m49


Skill_CityBattle = {}        --需要禁掉梯云纵的地图
Skill_CityBattle[1] = 3695619387 --c03
Skill_CityBattle[2] = 2092185140 --a06

function s1100501_CanCast(MapID, InstanceID, SkillID, OwnerID, Dummy)
	for i in pairs(Skill_MapCRC) do
        if MapID == Skill_MapCRC[i] then
            return 0
	    end
	end
	for k in pairs(Skill_InstanceCRC) do
	    if MapID == Skill_InstanceCRC[k] then
		    local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 13, 0)
		    msg.AddMsgEvent(MsgID, 1, 602)       --该技能不能再副本中使用
		    msg.DispatchRoleMsgEvent(OwnerID, MsgID)
            return 7
	    end
	end
	return 0
end


--注册
aux.RegisterSkillEvent(1100501, 0, "s1100501_CanCast")

function s1100502_CanCast(MapID, InstanceID, SkillID, OwnerID, Dummy)
    for i in pairs(Skill_MapCRC) do
        if MapID == Skill_MapCRC[i] then
            return 0
	    end
	end
	for k in pairs(Skill_InstanceCRC) do
	    if MapID == Skill_InstanceCRC[k] then
		    local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 13, 0)
		    msg.AddMsgEvent(MsgID, 1, 602)       --该技能不能再副本中使用
		    msg.DispatchRoleMsgEvent(OwnerID, MsgID)
            return 7
	    end
	end
	return 0
end


--注册
aux.RegisterSkillEvent(1100502, 0, "s1100502_CanCast")

function s1100503_CanCast(MapID, InstanceID, SkillID, OwnerID, Dummy)
    for i in pairs(Skill_MapCRC) do
        if MapID == Skill_MapCRC[i] then
            return 0
	    end
	end
	for k in pairs(Skill_InstanceCRC) do
	    if MapID == Skill_InstanceCRC[k] then
		    local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 13, 0)
		    msg.AddMsgEvent(MsgID, 1, 602)       --该技能不能再副本中使用
		    msg.DispatchRoleMsgEvent(OwnerID, MsgID)
            return 7
	    end
	end
	return 0
end


--注册
aux.RegisterSkillEvent(1100503, 0, "s1100503_CanCast")

function s1100504_CanCast(MapID, InstanceID, SkillID, OwnerID, Dummy)
    for i in pairs(Skill_MapCRC) do
        if MapID == Skill_MapCRC[i] then
            return 0
	    end
	end
	for k in pairs(Skill_InstanceCRC) do
	    if MapID == Skill_InstanceCRC[k] then
		    local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 13, 0)
		    msg.AddMsgEvent(MsgID, 1, 602)       --该技能不能再副本中使用
		    msg.DispatchRoleMsgEvent(OwnerID, MsgID)
            return 7
	    end
	end
	return 0
end


--注册
aux.RegisterSkillEvent(1100504, 0, "s1100504_CanCast")

-----------------------------------------------------------------------

function s1100901_CanCast(MapID, InstanceID, SkillID, OwnerID, Dummy)
    for i in pairs(Skill_MapCRC) do
        if MapID == Skill_MapCRC[i] then
            return 0
	    end
	end
	for k in pairs(Skill_InstanceCRC) do
	    if MapID == Skill_InstanceCRC[k] then
		    local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 13, 0)
		    msg.AddMsgEvent(MsgID, 1, 602)       --该技能不能再副本中使用
		    msg.DispatchRoleMsgEvent(OwnerID, MsgID)
            return 7
	    end
	end
	return 0
end


--注册
aux.RegisterSkillEvent(1100901, 0, "s1100901_CanCast")

function s1100902_CanCast(MapID, InstanceID, SkillID, OwnerID, Dummy)
    for i in pairs(Skill_MapCRC) do
        if MapID == Skill_MapCRC[i] then
            return 0
	    end
	end
	for k in pairs(Skill_InstanceCRC) do
	    if MapID == Skill_InstanceCRC[k] then
		    local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 13, 0)
		    msg.AddMsgEvent(MsgID, 1, 602)       --该技能不能再副本中使用
		    msg.DispatchRoleMsgEvent(OwnerID, MsgID)
            return 7
	    end
	end
	return 0
end


--注册
aux.RegisterSkillEvent(1100902, 0, "s1100902_CanCast")

function s1100903_CanCast(MapID, InstanceID, SkillID, OwnerID, Dummy)
    for i in pairs(Skill_MapCRC) do
        if MapID == Skill_MapCRC[i] then
            return 0
	    end
	end
	for k in pairs(Skill_InstanceCRC) do
	    if MapID == Skill_InstanceCRC[k] then
		    local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 13, 0)
		    msg.AddMsgEvent(MsgID, 1, 602)       --该技能不能再副本中使用
		    msg.DispatchRoleMsgEvent(OwnerID, MsgID)
            return 7
	    end
	end
	return 0
end


--注册
aux.RegisterSkillEvent(1100903, 0, "s1100903_CanCast")

function s1100904_CanCast(MapID, InstanceID, SkillID, OwnerID, Dummy)
    for i in pairs(Skill_MapCRC) do
        if MapID == Skill_MapCRC[i] then
            return 0
	    end
	end
	for k in pairs(Skill_InstanceCRC) do
	    if MapID == Skill_InstanceCRC[k] then
		    local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 13, 0)
		    msg.AddMsgEvent(MsgID, 1, 602)       --该技能不能再副本中使用
		    msg.DispatchRoleMsgEvent(OwnerID, MsgID)
            return 7
	    end
	end
	return 0
end


--注册
aux.RegisterSkillEvent(1100904, 0, "s1100904_CanCast")

-------------------------------------------------------------------------


function s1101201_CanCast(MapID, InstanceID, SkillID, OwnerID, Dummy)
    for i in pairs(Skill_MapCRC) do
        if MapID == Skill_MapCRC[i] then
            return 0
	    end
	end
	for k in pairs(Skill_InstanceCRC) do
	    if MapID == Skill_InstanceCRC[k] then
		    local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 13, 0)
		    msg.AddMsgEvent(MsgID, 1, 602)       --该技能不能再副本中使用
		    msg.DispatchRoleMsgEvent(OwnerID, MsgID)
            return 7
	    end
	end
	for j in pairs(Skill_CityBattle) do
        if MapID == Skill_CityBattle[j] then
            local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 13, 0)
		    msg.AddMsgEvent(MsgID, 1, 603)       --该技能不能在战场中使用
		    msg.DispatchRoleMsgEvent(OwnerID, MsgID)
            return 7
	    end
	end
	return 0
end


--注册
aux.RegisterSkillEvent(1101201, 0, "s1101201_CanCast")

function s1101202_CanCast(MapID, InstanceID, SkillID, OwnerID, Dummy)
    for i in pairs(Skill_MapCRC) do
        if MapID == Skill_MapCRC[i] then
            return 0
	    end
	end
	for k in pairs(Skill_InstanceCRC) do
	    if MapID == Skill_InstanceCRC[k] then
		    local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 13, 0)
		    msg.AddMsgEvent(MsgID, 1, 602)       --该技能不能再副本中使用
		    msg.DispatchRoleMsgEvent(OwnerID, MsgID)
            return 7
	    end
	end
	for j in pairs(Skill_CityBattle) do
        if MapID == Skill_CityBattle[j] then
            local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 13, 0)
		    msg.AddMsgEvent(MsgID, 1, 603)       --该技能不能在战场中使用
		    msg.DispatchRoleMsgEvent(OwnerID, MsgID)
            return 7
	    end
	end
	return 0
end


--注册
aux.RegisterSkillEvent(1101202, 0, "s1101202_CanCast")

function s1101203_CanCast(MapID, InstanceID, SkillID, OwnerID, Dummy)
    for i in pairs(Skill_MapCRC) do
        if MapID == Skill_MapCRC[i] then
            return 0
	    end
	end
	for k in pairs(Skill_InstanceCRC) do
	    if MapID == Skill_InstanceCRC[k] then
		    local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 13, 0)
		    msg.AddMsgEvent(MsgID, 1, 602)       --该技能不能再副本中使用
		    msg.DispatchRoleMsgEvent(OwnerID, MsgID)
            return 7
	    end
	end
	for j in pairs(Skill_CityBattle) do
        if MapID == Skill_CityBattle[j] then
            local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 13, 0)
		    msg.AddMsgEvent(MsgID, 1, 603)       --该技能不能在战场中使用
		    msg.DispatchRoleMsgEvent(OwnerID, MsgID)
            return 7
	    end
	end
	return 0
end


--注册
aux.RegisterSkillEvent(1101203, 0, "s1101203_CanCast")

function s1101204_CanCast(MapID, InstanceID, SkillID, OwnerID, Dummy)
    for i in pairs(Skill_MapCRC) do
        if MapID == Skill_MapCRC[i] then
            return 0
	    end
	end
	for k in pairs(Skill_InstanceCRC) do
	    if MapID == Skill_InstanceCRC[k] then
			local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 13, 0)
		    msg.AddMsgEvent(MsgID, 1, 602)       --该技能不能再副本中使用
		    msg.DispatchRoleMsgEvent(OwnerID, MsgID)
            return 7
	    end
	end
	for j in pairs(Skill_CityBattle) do
        if MapID == Skill_CityBattle[j] then
            local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 13, 0)
		    msg.AddMsgEvent(MsgID, 1, 603)       --该技能不能在战场中使用
		    msg.DispatchRoleMsgEvent(OwnerID, MsgID)
            return 7
	    end
	end
	return 0
end


--注册
aux.RegisterSkillEvent(1101204, 0, "s1101204_CanCast")

----------------------------------------------------------------------

function s1101401_CanCast(MapID, InstanceID, SkillID, OwnerID, Dummy)
    for i in pairs(Skill_MapCRC) do
        if MapID == Skill_MapCRC[i] then
            return 0
	    end
	end
	for k in pairs(Skill_InstanceCRC) do
	    if MapID == Skill_InstanceCRC[k] then
		    local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 13, 0)
		    msg.AddMsgEvent(MsgID, 1, 602)       --该技能不能再副本中使用
		    msg.DispatchRoleMsgEvent(OwnerID, MsgID)
            return 7
	    end
	end
	return 0
end


--注册
aux.RegisterSkillEvent(1101401, 0, "s1101401_CanCast")

function s1101402_CanCast(MapID, InstanceID, SkillID, OwnerID, Dummy)
    for i in pairs(Skill_MapCRC) do
        if MapID == Skill_MapCRC[i] then
            return 0
	    end
	end
	for k in pairs(Skill_InstanceCRC) do
	    if MapID == Skill_InstanceCRC[k] then
		    local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 13, 0)
		    msg.AddMsgEvent(MsgID, 1, 602)       --该技能不能再副本中使用
		    msg.DispatchRoleMsgEvent(OwnerID, MsgID)
            return 7
	    end
	end
	return 0
end


--注册
aux.RegisterSkillEvent(1101402, 0, "s1101402_CanCast")

function s1101403_CanCast(MapID, InstanceID, SkillID, OwnerID, Dummy)
    for i in pairs(Skill_MapCRC) do
        if MapID == Skill_MapCRC[i] then
            return 0
	    end
	end
	for k in pairs(Skill_InstanceCRC) do
	    if MapID == Skill_InstanceCRC[k] then
		    local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 13, 0)
		    msg.AddMsgEvent(MsgID, 1, 602)       --该技能不能再副本中使用
		    msg.DispatchRoleMsgEvent(OwnerID, MsgID)
            return 7
	    end
	end
	return 0
end


--注册
aux.RegisterSkillEvent(1101403, 0, "s1101403_CanCast")

------------------------------------------------------------------

function s9001201_CanCast(MapID, InstanceID, SkillID, OwnerID, Dummy)
    local bRet = role.CanMount(OwnerID) --玩家是否可以处在可以骑乘的状态
    for i in pairs(Skill_MapCRC) do
        if MapID == Skill_MapCRC[i] then
	        return bRet
	    end
	end
	for k in pairs(Skill_InstanceCRC) do
	    if MapID == Skill_InstanceCRC[k] then
		    local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 13, 0)
		    msg.AddMsgEvent(MsgID, 1, 602)       --该技能不能再副本中使用
		    msg.DispatchRoleMsgEvent(OwnerID, MsgID)
            return 7
	    end
	end
	return bRet
end


--注册
aux.RegisterSkillEvent(9001201, 0, "s9001201_CanCast")

--进入副本后，部分技能无法使用
--m14_SkillLimit = {}
--m14_SkillLimit[11005] = true 4
--m14_SkillLimit[11009] = true 4
--m14_SkillLimit[11012] = true 4
--m14_SkillLimit[11014] = true 3
--m14_SkillLimit[90012] = true 1

--function m14_CanUseSkill(MapID, InstanceID, SkillTypeID)
--    if m14_SkillLimit[SkillTypeID] then
--        return false
--    end
--    return true
--end
--注册
--aux.RegisterMapEvent("m14", 21, "m14_CanUseSkill")

--进入副本时删除玩家的坐骑buff
--function m14_OnPlayerEnter(MapID, InstanceID,RoleID)
--	local bhave = role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 99996)
--	if bhave == true then
--	 unit.CancelBuff(MapID, InstanceID, RoleID, 9999601)
--	 end
--end

--注册
--aux.RegisterMapEvent("m14", 2, "m14_OnPlayerEnter")
