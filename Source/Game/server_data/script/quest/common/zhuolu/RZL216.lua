--封魔石：12216
--需求1：在地物“封魔石”附近可使用道具封魔咒[2612138]

--道具封魔咒使用判断脚本函数
function i2612138_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false

	--不在涿鹿不能使用
	if MapID ~= 3017299663 then
	    bRet = 43
	else
	    local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
	    if x<=1936 or x>=1968 or z<=1991 or z >=2023 then --必须在改范围内使用
	        bRet = 43
	    end
	end

    --返回
	return bRet, bIgnore
end

--注册
aux.RegisterItemEvent(2612138, 0, "i2612138_CanUse")


function x12216_OnNPCTalk(mapID, instanceID, questID, ownerID, npcid, npctypeid)
	if npctypeid == 3030122 then
	unit.AddBuff(mapID, instanceID, ownerID, 2012101, ownerID)
	end
end

--注册
aux.RegisterQuestEvent(12216, 6, "x12216_OnNPCTalk")
