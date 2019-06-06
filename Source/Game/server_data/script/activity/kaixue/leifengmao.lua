
--由于每个人身上都记录了雷锋帽的数量，如果想要明年继续开启该活动，
--需要在人身上记录年份，每当玩家获得一个新的雷锋帽，就判断年份，
--如果年份和记录不一样，则将雷锋帽的数量置零，然后显示。
function HuoDeLeiFengMao(RoleID)
    local i = role.GetRoleScriptData(RoleID, 1, RoleDataType["LeiFengMao"])
	if i == nil then
	    i = 0
	end
	for q=0,4 do                       --容错性修改
	    if act.GetActScriptData(77, 1, q) == RoleID then
		    if act.GetActScriptData(77, 1, q+5) >= i then
			    i = act.GetActScriptData(77, 1, q+5)
			end
		end
	end
	i = i + 1
	role.SetRoleScriptData(RoleID, 1, RoleDataType["LeiFengMao"], i)

	if i % 100 == 0 and i ~= 0 and i ~= nil then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 2370)  --<玩家名>在本次开学活动中已经累计获得了×××个雷锋帽，真是精神可嘉！
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 9, i)
		msg.DispatchWorldMsgEvent(MsgID)
    end

	local arrID = {}
	local arrNum = {}
	local bNew = 1
	for l=0,4 do
		arrID[l] = act.GetActScriptData(77, 1, l)
		if arrID[l] == nil then
		    arrID[l] = 0
		end
		if arrID[l] == RoleID  then
			arrNum[l] = i
			bNew = 0
		else
			arrNum[l] = act.GetActScriptData(77, 1, l+5)
			if arrNum[l] == nil then
		        arrNum[l] = 0
		    end
		end
	end

	if bNew == 1 then
		arrID[5] = RoleID;
		arrNum[5] = i;
	else
		arrID[5] = 0;
		arrNum[5] = 0;
	end

	for m=0, 4 do
		for n=m+1,5 do
			if arrNum[n] > arrNum[m] then
				local temp = arrNum[m]
				arrNum[m] = arrNum[n]
				arrNum[n] = temp
				temp = arrID[m]
				arrID[m] = arrID[n]
				arrID[n] = temp
			end
		end
	end

	for j=0,4 do
		act.SetActScriptData(77, 1, j, arrID[j])
		act.SetActScriptData(77, 1, j+5, arrNum[j])
	end

	act.SaveActScriptData(77)



	--[[for q=0,4 do                       --容错性修改
	    if act.GetActScriptData(77, 1, q) == RoleID then
		    if act.GetActScriptData(77, 1, q+5) >= i then
			    i = act.GetActScriptData(77, 1, q+5) + 1
				role.SetRoleScriptData(RoleID, 1, RoleDataType["LeiFengMao"], i)
			end
		end
	end

	--为前5名排序
	local m = 0  --记录活动记录中有用值的数量
	for n = 1, 5 do
	    local kRole = act.GetActScriptData(77, 1, 5-n)
		if kRole ~= 0 and kRole ~= nil then
			m = 6 - n
			break
		end
	end
	if act.GetActScriptData(77, 1, 4) == 0 or act.GetActScriptData(77, 1, 4) == nil or i >= act.GetActScriptData(77, 1, 9) then
		if m ~= 0 then
			local no = nil
			for n = 0, m-1 do
				if act.GetActScriptData(77, 1, n+5) < i then
					no = n
					break
				end
			end
			if no ~= nil then
			    if act.GetActScriptData(77, 1, no) ~= RoleID then
				    local size = nil
				    for i=0, 4 do
					    if act.GetActScriptData(77, 1, i) == RoleID then
						    size = i
							break
						end
					end
					if size ~= nil and size ~= no then
					    local kkk = size
						while kkk ~= no do
							act.SetActScriptData(77, 1, kkk, act.GetActScriptData(77, 1, kkk - 1))
							kkk = kkk - 1
						end
						local kka = size + 5
						while kka ~= no + 5 do
							act.SetActScriptData(77, 1, kka, act.GetActScriptData(77, 1, kka - 1))
							kka = kka - 1
						end
					else
						local kk = 4
						while kk ~= no do
							act.SetActScriptData(77, 1, kk, act.GetActScriptData(77, 1, kk - 1))
							kk = kk - 1
						end
						local ka = 9
						while ka ~= no + 5 do
							act.SetActScriptData(77, 1, ka, act.GetActScriptData(77, 1, ka - 1))
							ka = ka - 1
						end
					end
					act.SetActScriptData(77, 1, no, RoleID)
					act.SetActScriptData(77, 1, no + 5, i)
				else
				    act.SetActScriptData(77, 1, no, RoleID)
					act.SetActScriptData(77, 1, no + 5, i)
				end
			else
			    if m < 5 then
				    act.SetActScriptData(77, 1, m, RoleID)
			        act.SetActScriptData(77, 1, m+5, i)
				end
			end
		else
			act.SetActScriptData(77, 1, 0, RoleID)
			act.SetActScriptData(77, 1, 5, i)
		end
	end
	act.SaveActScriptData(77)]]
end


--杀死逃学微龙获得雷锋帽
function c1534125_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
    local FreeSize = role.GetBagFreeSize(RoleID)
	if(FreeSize > 1) then
		role.AddRoleItem(MapID, InstanceID, RoleID, 3303206, 1, 1, 8, 420)
		HuoDeLeiFengMao(RoleID)
	end
end

aux.RegisterCreatureEvent(1534125, 4, "c1534125_OnDie")
aux.RegisterCreatureEvent(1534126, 4, "c1534125_OnDie")
aux.RegisterCreatureEvent(1534127, 4, "c1534125_OnDie")
aux.RegisterCreatureEvent(1534128, 4, "c1534125_OnDie")
aux.RegisterCreatureEvent(1534129, 4, "c1534125_OnDie")
aux.RegisterCreatureEvent(1534130, 4, "c1534125_OnDie")










