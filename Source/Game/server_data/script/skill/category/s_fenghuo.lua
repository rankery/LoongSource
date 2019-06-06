--¼¼ÄÜ¡ª¡ª·ç»ğÀ×Óê 1010701
--Ö»ÄÜÔÚĞŞÁ¶³¡Ê¹ÓÃ£¬Ê¹ÓÃÒ»´ÎÏûºÄÒ»¸ö·ç»ğÊ¯

function s1010701_CanCast(MapID, InstanceID, SkillID, OwnerID, Dummy)
    if MapID == 2291722097 or MapID == 2291721329 or MapID == 2291721585 or MapID == 2291720817 or MapID == 2291721073 or MapID == 2291720305 or MapID == 2291720561 then
        if role.GetRoleItemNum(OwnerID, 3200464) > 0 then
     		return 0
        else
		    local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 13, 0)
		    msg.AddMsgEvent(MsgID, 1, 600)       --Ã¿´ÎÊ¹ÓÃ·ç»ğÀ×Óê¶¼ĞèÒªÏûºÄÒ»Ã¶·ç»ğÊ¯£¬Äú¿ÉÒÔÔÚÓÎÏ·ÉÌ³Ç¹ºÂò¸ÃµÀ¾ß¡££
		    msg.DispatchRoleMsgEvent(OwnerID, MsgID)
            return 7
        end
	else
	    local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 13, 0)
		msg.AddMsgEvent(MsgID, 1, 601)       --·ç»ğÀ×ÓêÖ»ÄÜÔÚĞŞÁ¶³¡ÖĞÊ¹ÓÃ¡£
		msg.DispatchRoleMsgEvent(OwnerID, MsgID)
	    return 7
	end
end

function s1010701_Cast(MapID, InstanceID, SkillID, OwnerID)
	role.RemoveFromRole(MapID, InstanceID, OwnerID, 3200464, 1, 420)
	return 0
end


--×¢²á

aux.RegisterSkillEvent(1010701, 1, "s1010701_Cast")
aux.RegisterSkillEvent(1010701, 0, "s1010701_CanCast")
