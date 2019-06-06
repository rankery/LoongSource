
function yuanxiaogouhuo_Time()
    local curhour = tonumber(os.date("%H"))
	local curmin = tonumber(os.date("%M"))
	if curhour == 21 and curmin == 0 then
	    return true
	else
	    return false
	end
end

function yuanxiaoguaiwu_Time()
    local curhour = tonumber(os.date("%H"))
	local curmin = tonumber(os.date("%M"))
	if curhour <= 23 and curhour >= 19 and curmin % 3 == 0 then
	    return true
	else
	    return false
	end
end

function yuanxiaogonggao_Time()
    local curhour = tonumber(os.date("%H"))
	local curmin = tonumber(os.date("%M"))
	if curhour == 19 and curmin == 0 then
	    return true
	else
	    return false
	end
end
